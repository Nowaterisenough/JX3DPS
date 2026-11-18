#include "workbench.h"
#include "game_icons.h"
#include "game_attributes.h"
#include "history_panel.h"

#include <QAction>
#include <QCloseEvent>
#include <QClipboard>
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPointer>
#include <QProgressBar>
#include <QResizeEvent>
#include <QPushButton>
#include <QSaveFile>
#include <QScrollArea>
#include <QSettings>
#include <QShortcut>
#include <QSplitter>
#include <QStackedWidget>
#include <QStyle>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "code_editor/code_editor.h"
#include "code_editor/debug_toolbar.h"
#include "code_editor/simulation_toolbar.h"
#include "controls/player_state_panel/player_state_panel.h"
#include "controls/timeline/timeline.h"
#include "controls/theme/workbench_theme.h"
#include "controls/theme/segmented_tabs.h"
#include "controls/theme/magpie_shell.h"
#include "controls/theme/header_group_box.h"

#include "batch_worker.h"
#include "buff_panel.h"
#include "debug_session.h"
#include "diagnostics.h"
#include "macro_editor.h"
#include "macro_search_bar.h"
#include "macro_trace_panel.h"
#include "resources.h"
#include "results_panel.h"
#include "simulation_options.h"

namespace rt = JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;

namespace {
QString Name(std::string_view text)
{
    return QString::fromUtf8(text.data(), static_cast<int>(text.size()));
}

QTableWidget *Table(const QStringList &headers, QWidget *parent)
{
    auto *table = new QTableWidget(0, headers.size(), parent);
    table->setHorizontalHeaderLabels(headers);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->verticalHeader()->hide();
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setStretchLastSection(true);
    return table;
}

QString MacroHelp(desktop::Specialization specialization)
{
    QString html = QStringLiteral(
        "<h2>宏与断点调试</h2><p>/cast 每轮尝试施放；/fcast 允许打断引导；"
        "/scast 与 /sfcast 每场只成功执行一次，成功前会阻止后续行。</p>"
        "<p>条件支持 &amp;（且）、|（或）、&lt;、&lt;=、=、!=、&gt;、&gt;=。以 # 开头的整行为注释。</p>"
        "<p>buff:玄门 与 buff:玄门&gt;0 等价。nobuff 表示不存在，tbuff / tnobuff 查询目标。"
        "bufftime、tbufftime 与 skill_cd 使用秒；调试器展示参与计算的帧数（每秒 16 帧）。</p>"
        "<p>qidian（气点）、energy（能量）、rage（怒气）使用整数；life、mana、tlife 使用 0 至 1 的比例。"
        "skill_energy 查询充能，last_skill 查询上次技能。</p>"
        "<h3>操作</h3><p>点击行号设置断点，启动调试后停在首行。F5 继续，F6 暂停，F10 执行一行，"
        "F11 执行一个条件或动作，Shift+F11 执行本轮，Shift+F5 停止。调试期间宏与配置锁定。</p>"
        "<p>Ctrl+Space 打开补全，回车或 Tab 插入；Ctrl+F 查找，Ctrl+H 替换，F3 / Shift+F3 定位。"
        "调试期间仍可查找。</p><p>单次记录与调试使用所选随机种子的第一场战斗；批量模拟保留汇总结果。</p>"
        "<h3>当前心法技能（中文名 / 拼音别名）</h3><table>");
    if (specialization == desktop::Specialization::MoWen) {
        html.prepend(QStringLiteral("<p><b>莫问：</b>强制施放行会在判断条件前打断引导；调试轨迹会单独记录这一事件。"
                                    "持续伤害可使用 tbuff:商 与 tbuff:角；影子计时和当前曲风显示在调试状态中。</p>"));
    }
    const auto catalog = desktop::Describe(specialization);
    for (const auto slot : catalog.castable) {
        html += QStringLiteral("<tr><td>%1</td><td>%2</td></tr>").arg(Name(catalog.skills[slot].name), Name(catalog.skillAliases[slot]));
    }
    html += QStringLiteral("</table><h3>增益名称（可用于宏）</h3><table>");
    for (std::size_t slot = 0; slot < catalog.buffs.size(); ++slot) {
        html += QStringLiteral("<tr><td>%1</td><td>%2</td></tr>").arg(Name(catalog.buffs[slot].name), Name(catalog.buffAliases[slot]));
    }
    return html + QStringLiteral("</table>");
}
} // namespace

struct Workbench::Impl
{
    Workbench              *window;
    desktop::MagpieShell    *shell;
    QWidget                *homePage;
    QWidget                *homeEditor;
    QScrollArea            *configurationScroll;
    QSplitter              *body;
    QLabel                 *pageNote;
    QWidget                *statusBar;
    QWidget                *settingsPage;
    QWidget                *aboutPage;
    QString                 currentPage = QStringLiteral("simulation");
    CodeEditor             *editor;
    MacroSearchBar         *search;
    SimulationOptions      *options;
    SimulationToolbar      *simToolbar;
    QToolButton            *debugToggle;
    QToolButton            *macroRun;
    DebugToolbar           *debugToolbar;
    DebugSession           *debugSession;
    Timeline               *timeline;
    HistoryPanel           *history;
    PlayerStatePanel       *statePanel;
    QTextBrowser           *inspector;
    QTextBrowser           *help;
    QListWidget            *breakpointList;
    QTabWidget             *debugTabs;
    QSplitter              *macroSplit;
    QLabel                 *debugPosition;
    desktop::Specialization documentClass = desktop::Specialization::TaiXu;

    struct Draft
    {
        QString    text, path;
        bool       modified = false;
        QList<int> breakpoints;
    };

    std::array<std::optional<Draft>, 2> drafts;
    QTableWidget                       *eventsTable;
    QTableWidget                       *problems;
    ResultsPanel                       *results;
    BuffPanel                          *buffs;
    MacroTracePanel                    *macroSteps;
    QTabWidget                         *views;
    QDialog                            *analysisDialog;
    QWidget                            *editorPage;
    QLabel                             *macroStatus;
    QLabel                             *meanDps;
    QLabel                             *outputDetails;
    QDialog                            *helpDialog;
    QLabel                             *result;
    QLabel                             *documentLabel;
    QProgressBar                       *progress;
    QPushButton                        *cancel;
    QTimer                             *progressTimer;
    QPointer<BatchWorker>               batch;
    QAction                            *traceAction;
    QAction                            *gainsAction;
    QAction                            *exportAction;
    QAction                            *openAction;
    QAction                            *newAction;
    QAction                            *completeAction;
    CombatEvents                        events;
    QString                             filePath;
    rt::BatchOptions                    activeOptions;
    QElapsedTimer                       elapsed;
    bool                                cancelling = false;
    bool                                closing    = false;

    explicit Impl(Workbench *host) : window(host)
    {
        auto *central = window->ContentWidget();
        central->setObjectName(QStringLiteral("workbenchContent"));
        auto *root = new QVBoxLayout(central);
        root->setContentsMargins(0, 0, 0, 0);
        root->setSpacing(0);
        shell = new desktop::MagpieShell(central);
        root->addWidget(shell);
        homePage = new QWidget(central);
        homePage->setObjectName(QStringLiteral("simulationPage"));
        auto *homeLayout = new QVBoxLayout(homePage);
        homeLayout->setContentsMargins(0, 0, 0, 0);
        homeLayout->setSpacing(14);
        pageNote = new QLabel(QStringLiteral("设置心法、属性与循环宏，在本地运行战斗模拟。相同配置与随机种子可复现结果。"));
        pageNote->setProperty("muted", true);
        pageNote->setWordWrap(true);
        pageNote->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        homeLayout->addWidget(pageNote);
        body = new QSplitter(Qt::Horizontal, homePage);
        body->setObjectName(QStringLiteral("bodySplitter"));
        body->setChildrenCollapsible(false);
        homeLayout->addWidget(body, 1);
        shell->pages->addWidget(homePage);
        configurationScroll = new QScrollArea(body);
        configurationScroll->setObjectName(QStringLiteral("configurationScroll"));
        configurationScroll->setFrameShape(QFrame::NoFrame);
        configurationScroll->setWidgetResizable(true);
        configurationScroll->setMinimumWidth(664);
        configurationScroll->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        options = new SimulationOptions;
        configurationScroll->setWidget(options);
        options->setObjectName(QStringLiteral("simulationOptions"));
        options->setStyleSheet(QStringLiteral("QWidget { font-size: 13px; }"
                                             "QWidget#simulationOptions { background: transparent; }"
                                              "QComboBox, QSpinBox, QDoubleSpinBox, QLineEdit { min-height: 20px; }"
                                             "QLineEdit { placeholder-text-color: #56565f; }"));
        auto configurationFont = options->font();
        configurationFont.setPixelSize(13);
        configurationFont.setWeight(QFont::Normal);
        options->setFont(configurationFont);
        options->setMinimumSize(610, 0);
        options->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        homeEditor = new QWidget(body);
        homeEditor->setMinimumWidth(280);
        auto *homeEditorLayout = new QVBoxLayout(homeEditor);
        homeEditorLayout->setContentsMargins(0, 0, 0, 0);
        body->setHandleWidth(12);
        body->setSizes({ 664, 496 });
        body->setStretchFactor(0, 0);
        body->setStretchFactor(1, 1);

        auto action = [&](const QString &label, const char *name, const QKeySequence &shortcut, auto callback) {
            auto *item = new QAction(label, window);
            item->setObjectName(QString::fromLatin1(name));
            item->setShortcut(shortcut);
            window->addAction(item);
            QObject::connect(item, &QAction::triggered, window, callback);
            return item;
        };
        newAction  = action(QStringLiteral("新建"), "newMacro", QKeySequence::New, [this] { NewMacro(); });
        openAction = action(QStringLiteral("打开"), "openMacro", QKeySequence::Open, [this] { OpenMacro(); });
        auto *save = action(QStringLiteral("保存"), "saveMacro", QKeySequence::Save, [this] { SaveMacro(false); });
        auto *saveAs = action(QStringLiteral("另存为"), "saveMacroAs", QKeySequence::SaveAs, [this] { SaveMacro(true); });
        auto *check  = action(QStringLiteral("检查宏"), "checkMacro", QKeySequence(Qt::CTRL | Qt::Key_Return), [this] {
            CheckMacro();
        });
        traceAction  = action(QStringLiteral("单次记录"), "singleTrace", {}, [this] { StartBatch(true); });
        gainsAction = new QAction(QStringLiteral("自动收益"), window);
        gainsAction->setObjectName(QStringLiteral("attributeGains"));
        gainsAction->setCheckable(true);
        gainsAction->setChecked(options->AutoCalculateGains());
        gainsAction->setToolTip(QStringLiteral("选中后，开始模拟时一并计算当前模式的属性收益；切换不会立即启动任务。"));
        window->addAction(gainsAction);
        QObject::connect(gainsAction, &QAction::toggled, options->GainsButton(), &QToolButton::setChecked);
        QObject::connect(options->GainsButton(), &QToolButton::toggled, gainsAction, &QAction::setChecked);
        exportAction = action(QStringLiteral("导出战斗记录"), "exportTrace", {}, [this] { ExportTrace(); });
        exportAction->setEnabled(false);
        auto *clearBreakpoints =
            action(QStringLiteral("清除断点"), "clearBreakpoints", {}, [this] { editor->ClearAllBreakpoints(); });

        auto *macroPanel = new desktop::HeaderGroupBox(QStringLiteral("循环宏"));
        editorPage = macroPanel;
        editorPage->setObjectName(QStringLiteral("macroPanel"));
        editorPage->setStyleSheet(QStringLiteral("QGroupBox#macroPanel { margin-top: %1px; } QGroupBox#macroPanel::title { top: 5px; }")
            .arg(desktop::AttributeHeaderHeight + desktop::AttributeHeaderSpacing));
        auto *macroHeader = macroPanel->HeaderActions(desktop::AttributeHeaderHeight);
        auto *openHelp = new QToolButton;
        openHelp->setObjectName(QStringLiteral("openMacroHelp"));
        openHelp->setText(QStringLiteral("宏语法…"));
        openHelp->setToolTip(QStringLiteral("打开独立的宏语法帮助窗口，不切走当前页面或中断调试"));
        macroHeader->addWidget(openHelp);
        homeEditorLayout->addWidget(editorPage, 1);
        homeEditorLayout->addWidget(options->TakeOutputPanel());
        auto *editorLayout = new QVBoxLayout(editorPage);
        editorLayout->setContentsMargins(8, 16, 8, 8);
        editorLayout->setSpacing(5);
        auto *files = new QToolBar(editorPage);
        files->setStyleSheet(QStringLiteral("QToolButton { padding: 4px; }"));
        files->addActions({ newAction, openAction, save, check });
        auto *more = new QToolButton(files);
        more->setObjectName(QStringLiteral("macroMore"));
        more->setText(QStringLiteral("更多"));
        more->setPopupMode(QToolButton::InstantPopup);
        auto *menu = new QMenu(more);
        more->setMenu(menu);
        menu->addAction(saveAs);
        menu->addSeparator();
        auto *editorTools = new QHBoxLayout;
        editorTools->setSpacing(4);
        editorTools->addWidget(files, 1);
        editorTools->addWidget(more);
        editorLayout->addLayout(editorTools);
        auto *documentInfo = new QWidget(editorPage);
        auto *documentInfoLayout = new QHBoxLayout(documentInfo);
        documentInfoLayout->setContentsMargins(0, 0, 0, 0);
        documentLabel = new QLabel(editorPage);
        documentLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        documentLabel->setStyleSheet(QStringLiteral("color: #92929d; padding: 2px;"));
        documentInfoLayout->addWidget(documentLabel, 1);
        editorLayout->addWidget(documentInfo);
        auto *macroEditor = new MacroEditor(editorPage);
        editor            = macroEditor;
        editor->setObjectName(QStringLiteral("macroEditor"));
        editor->setFont(Resources::MonoFont());
        editor->SetSyntaxType(CodeEditor::JX3Macro);
        search = new MacroSearchBar(editor, editorPage);
        editorLayout->addWidget(search);
        macroSplit = new QSplitter(Qt::Vertical, editorPage);
        macroSplit->setObjectName(QStringLiteral("macroDebugSplitter"));
        macroSplit->setChildrenCollapsible(false);
        editor->setMinimumHeight(150);
        macroSplit->addWidget(editor);
        editorLayout->addWidget(macroSplit, 1);
        auto *find     = action(QStringLiteral("查找"), "findMacro", QKeySequence::Find, [this] {
            ShowEditor();
            search->Open();
        });
        auto *replace  = action(QStringLiteral("替换"), "replaceMacro", QKeySequence(Qt::CTRL | Qt::Key_H), [this] {
            ShowEditor();
            search->Open(true);
        });
        completeAction = action(QStringLiteral("代码补全"), "completeMacro", {}, [this, macroEditor] {
            ShowEditor();
            macroEditor->setFocus();
            macroEditor->Complete();
        });
        menu->addActions({ find, replace, completeAction, clearBreakpoints });
        helpDialog = new QDialog(window);
        helpDialog->setObjectName(QStringLiteral("macroHelpDialog"));
        helpDialog->setModal(false);
        helpDialog->setWindowTitle(QStringLiteral("宏语法与调试帮助"));
        helpDialog->resize(760, 620);
        auto *helpLayout = new QVBoxLayout(helpDialog);
        help             = new QTextBrowser(helpDialog);
        help->setObjectName(QStringLiteral("macroHelpBrowser"));
        helpLayout->addWidget(help);
        auto *helpAction = action(QStringLiteral("宏语法帮助"), "macroHelp", {}, [this] { ShowMacroHelp(); });
        QObject::connect(openHelp, &QToolButton::clicked, helpAction, &QAction::trigger);
        menu->addSeparator();
        menu->addAction(helpAction);
        problems = Table({ QStringLiteral("宏行"), QStringLiteral("说明") }, editorPage);
        problems->setObjectName(QStringLiteral("macroProblems"));
        problems->setMaximumHeight(135);
        problems->hide();
        editorLayout->addWidget(problems);
        macroStatus = new QLabel(QStringLiteral("点击行号设置断点 · Ctrl+Space 补全"), editorPage);
        macroStatus->setStyleSheet(QStringLiteral("color: #92929d; font-size: 12px;"));
        macroStatus->setWordWrap(true);
        editorLayout->addWidget(macroStatus);

        simToolbar = new SimulationToolbar(options);
        simToolbar->setObjectName(QStringLiteral("simulationToolbar"));
        simToolbar->setMinimumWidth(110);
        simToolbar->setMaximumWidth(140);
        options->RunActions()->addWidget(simToolbar);
        debugToggle = simToolbar->findChild<QToolButton *>(QStringLiteral("debugButton"));
        editorTools->addWidget(debugToggle);
        macroRun = new QToolButton(editorPage);
        macroRun->setObjectName(QStringLiteral("runMacroButton"));
        macroRun->setText(QStringLiteral("开始模拟"));
        macroRun->setToolTip(QStringLiteral("运行当前循环宏（F5）"));
        editorTools->addWidget(macroRun);
        QObject::connect(macroRun, &QToolButton::clicked, window, [this] {
            simToolbar->findChild<QToolButton *>(QStringLiteral("startButton"))->click();
        });
        auto *outputSummary = new QHBoxLayout;
        auto *damageSummary = new QVBoxLayout;
        damageSummary->setSpacing(2);
        damageSummary->addWidget(new QLabel(QStringLiteral("平均秒伤")));
        meanDps = new QLabel(QStringLiteral("--"));
        meanDps->setObjectName(QStringLiteral("meanDps"));
        auto metricFont = meanDps->font();
        metricFont.setFamily(QStringLiteral("Bahnschrift"));
        metricFont.setPixelSize(30);
        metricFont.setWeight(QFont::Normal);
        meanDps->setFont(metricFont);
        meanDps->setStyleSheet(QStringLiteral("font-family: 'Segoe UI'; font-size: 30px; font-weight: 600; color: #ededf1;"));
        meanDps->setMinimumWidth(100);
        damageSummary->addWidget(meanDps);
        outputSummary->addLayout(damageSummary);
        outputDetails = new QLabel(QStringLiteral("运行模拟后显示结果"));
        outputDetails->setObjectName(QStringLiteral("outputDetails"));
        outputDetails->setWordWrap(true);
        outputDetails->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        outputSummary->addWidget(outputDetails, 1);
        auto *outputActions = new QGridLayout;
        outputActions->setSpacing(4);
        int outputButtonIndex = 0;
        auto  outputButton  = [&](const QString &label, const char *name, auto callback) {
            auto *button = new QPushButton(label);
            button->setObjectName(QString::fromLatin1(name));
            outputActions->addWidget(button, outputButtonIndex / 2, outputButtonIndex % 2);
            ++outputButtonIndex;
            QObject::connect(button, &QPushButton::clicked, window, callback);
        };
        outputButton(QStringLiteral("技能统计"), "showAnalysis", [this] {
            results->ShowSkillStatistics();
            ShowAnalysis(results);
        });
        outputButton(QStringLiteral("时间轴"), "showTimeline", [this] { ShowAnalysis(views->widget(0)); });
        auto *recordButton = new QToolButton;
        recordButton->setObjectName(QStringLiteral("singleTraceButton"));
        recordButton->setDefaultAction(traceAction);
        recordButton->setToolTip(QStringLiteral("记录第一场战斗，查看时间轴、技能伤害和增益变化。"));
        outputActions->addWidget(recordButton, 1, 0);
        auto *quickGain = new desktop::MagpieCheckButton;
        quickGain->setObjectName(QStringLiteral("calculateAttributeGains"));
        quickGain->setDefaultAction(gainsAction);
        outputActions->addWidget(quickGain, 1, 1);
        outputSummary->addLayout(outputActions);
        outputSummary->setSpacing(10);
        options->OutputLayout()->addLayout(outputSummary);

        analysisDialog = new QDialog(window);
        analysisDialog->setWindowFlags(Qt::Widget);
        analysisDialog->setObjectName(QStringLiteral("resultAnalysis"));
        analysisDialog->setWindowTitle(QStringLiteral("结果分析"));
        shell->pages->addWidget(analysisDialog);
        analysisDialog->addActions({find, replace, completeAction});
        auto *analysisLayout = new QVBoxLayout(analysisDialog);
        analysisLayout->setContentsMargins(0, 0, 0, 0);
        auto *analysisTools  = new QToolBar(analysisDialog);
        analysisTools->addAction(traceAction);
        analysisTools->addAction(exportAction);
        auto *analysisSpace = new QWidget(analysisTools);
        analysisSpace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        analysisTools->addWidget(analysisSpace);
        auto *backToMacro = new QToolButton(analysisTools);
        backToMacro->setObjectName(QStringLiteral("closeAnalysis"));
        backToMacro->setText(QStringLiteral("返回循环宏"));
        QObject::connect(backToMacro, &QToolButton::clicked, window, [this] { ShowEditor(); });
        analysisTools->addWidget(backToMacro);
        analysisLayout->addWidget(analysisTools);
        views = new desktop::SegmentedTabs(analysisDialog);
        views->setObjectName(QStringLiteral("analysisViews"));
        analysisLayout->addWidget(views, 1);
        auto *timelinePage = new QWidget(views);
        timelinePage->setObjectName(QStringLiteral("timelinePage"));
        auto *timelineLayout = new QVBoxLayout(timelinePage);
        timelineLayout->setContentsMargins(0, 0, 0, 0);
        auto *timelineTools = new QToolBar(timelinePage);
        timelineLayout->addWidget(timelineTools);
        auto *timelineScroll = new QScrollArea(timelinePage);
        timelineScroll->setObjectName(QStringLiteral("timelineScroll"));
        timelineScroll->setWidgetResizable(true);
        timelineScroll->setFrameShape(QFrame::NoFrame);
        timeline = new Timeline;
        timeline->setObjectName(QStringLiteral("combatTimeline"));
        timeline->setMinimumHeight(210);
        timelineScroll->setWidget(timeline);
        auto *timelineSplit = new QSplitter(Qt::Horizontal);
        timelineSplit->setChildrenCollapsible(false);
        timelineSplit->addWidget(timelineScroll);
        history = new HistoryPanel;
        timelineSplit->addWidget(history);
        timelineSplit->setStretchFactor(0, 1);
        timelineSplit->setSizes({900, 290});
        timelineLayout->addWidget(timelineSplit, 1);
        QObject::connect(timeline, &Timeline::CursorTimeChanged, history, &HistoryPanel::Inspect);
        timelineTools->addSeparator();
        timelineTools->addWidget(new QLabel(QStringLiteral("移动鼠标查看属性 · 滚轮缩放 · 拖动平移")));
        QObject::connect(timelineTools->addAction(QStringLiteral("放大")), &QAction::triggered, timeline, &Timeline::ZoomIn);
        QObject::connect(timelineTools->addAction(QStringLiteral("缩小")), &QAction::triggered, timeline, &Timeline::ZoomOut);
        QObject::connect(timelineTools->addAction(QStringLiteral("适应全程")), &QAction::triggered, timeline, &Timeline::ResetZoom);
        eventsTable =
            Table({ QStringLiteral("帧"), QStringLiteral("秒"), QStringLiteral("技能"), QStringLiteral("伤害"), QStringLiteral("命中结果"), QStringLiteral("序号") },
                  views);
        eventsTable->setObjectName(QStringLiteral("damageEvents"));
        views->addTab(timelinePage, QStringLiteral("时间轴"));
        views->addTab(eventsTable, QStringLiteral("伤害事件"));
        results = new ResultsPanel(views);
        views->addTab(results, QStringLiteral("模拟统计"));
        buffs = new BuffPanel(views);
        views->addTab(buffs, QStringLiteral("增益分析"));
        views->setCurrentWidget(results);

        debugSession  = new DebugSession(window);
        debugToolbar = new DebugToolbar(window);
        debugToolbar->setObjectName(QStringLiteral("macroDebugToolbar"));
        debugToolbar->SetAnchorWidget(editor);
        debugPosition = new QLabel(documentInfo);
        debugPosition->setObjectName(QStringLiteral("debugPosition"));
        debugPosition->setStyleSheet(QStringLiteral("color: #63cfe8; padding: 2px; font-size: 12px;"));
        documentInfoLayout->addWidget(debugPosition);
        auto debugAction = [&](const QString &text, const char *name, QStyle::StandardPixmap icon,
                               const QString &tip, auto callback) {
            auto *item = action(text, name, {}, callback);
            item->setIcon(window->style()->standardIcon(icon));
            item->setToolTip(tip);
        };
        debugAction(QStringLiteral("开始调试"), "debugStart", QStyle::SP_MediaPlay,
                    QStringLiteral("开始调试，在首条宏前暂停"), [this] { StartDebug(); });
        debugAction(QStringLiteral("继续"), "debugContinue", QStyle::SP_MediaPlay,
                    QStringLiteral("继续至断点或结束（F5）"), [this] { ContinueDebug(); });
        debugAction(QStringLiteral("暂停"), "debugPause", QStyle::SP_MediaPause,
                    QStringLiteral("暂停（F6）"), [this] { debugSession->Pause(); });
        debugAction(QStringLiteral("执行一行"), "debugStepOver", QStyle::SP_MediaSkipForward,
                    QStringLiteral("执行一行（F10）"), [this] { Step(1); });
        debugAction(QStringLiteral("条件 / 动作"), "debugStepInto", QStyle::SP_ArrowDown,
                    QStringLiteral("执行一个条件或动作（F11）"), [this] { Step(0); });
        debugAction(QStringLiteral("执行本轮"), "debugStepOut", QStyle::SP_MediaSeekForward,
                    QStringLiteral("执行本轮宏（Shift+F11）"), [this] { Step(2); });
        debugAction(QStringLiteral("重新调试"), "debugRestart", QStyle::SP_BrowserReload,
                    QStringLiteral("重新调试当前宏"), [this] { StartDebug(); });
        debugAction(QStringLiteral("停止"), "debugStop", QStyle::SP_MediaStop,
                    QStringLiteral("停止调试（Shift+F5）"), [this] { debugSession->Stop(); });
        debugTabs = new desktop::SegmentedTabs(macroSplit);
        debugTabs->setObjectName(QStringLiteral("debugTabs"));
        debugTabs->setMinimumHeight(300);
        macroSplit->addWidget(debugTabs);
        macroSplit->setStretchFactor(0, 3);
        macroSplit->setStretchFactor(1, 2);
        macroSplit->setSizes({400, 270});
        inspector = new QTextBrowser(debugTabs);
        inspector->setObjectName(QStringLiteral("debugInspector"));
        inspector->setPlaceholderText(
            QStringLiteral("点击“开始调试”，在首条宏前暂停。运行状态、技能冷却与增益将在这里显示。"));
        debugTabs->addTab(inspector, QStringLiteral("运行状态"));
        macroSteps = new MacroTracePanel(debugTabs);
        debugTabs->addTab(macroSteps, QStringLiteral("执行轨迹"));
        auto *playerPage = new QScrollArea(debugTabs);
        playerPage->setFrameShape(QFrame::NoFrame);
        playerPage->setWidgetResizable(true);
        statePanel = new PlayerStatePanel;
        playerPage->setWidget(statePanel);
        debugTabs->addTab(playerPage, QStringLiteral("玩家与目标"));
        auto *breakpointPage   = new QWidget(debugTabs);
        auto *breakpointLayout = new QVBoxLayout(breakpointPage);
        breakpointList         = new QListWidget(breakpointPage);
        breakpointList->setObjectName(QStringLiteral("breakpointList"));
        breakpointLayout->addWidget(breakpointList);
        auto *clearButton = new QToolButton(breakpointPage);
        clearButton->setDefaultAction(clearBreakpoints);
        breakpointLayout->addWidget(clearButton);
        debugTabs->addTab(breakpointPage, QStringLiteral("断点"));
        auto updateBreakpoints = [this] {
            breakpointList->clear();
            auto lines = editor->GetBreakpoints().values();
            std::sort(lines.begin(), lines.end());
            for (int line : lines) {
                auto *item = new QListWidgetItem(
                    QStringLiteral("第 %1 行 · %2").arg(line).arg(editor->document()->findBlockByNumber(line - 1).text()),
                    breakpointList);
                item->setData(Qt::UserRole, line);
            }
        };
        QObject::connect(editor, &CodeEditor::BreakpointAdded, window, updateBreakpoints);
        QObject::connect(editor, &CodeEditor::BreakpointRemoved, window, updateBreakpoints);
        QObject::connect(breakpointList, &QListWidget::itemActivated, window, [this](QListWidgetItem *item) {
            editor->setTextCursor(QTextCursor(editor->document()->findBlockByNumber(item->data(Qt::UserRole).toInt() - 1)));
            editor->centerCursor();
        });
        QObject::connect(macroSteps, &MacroTracePanel::SourceRequested, window, [this](const QString &source, int line) {
            if (editor->toPlainText() != source) {
                result->setText(QStringLiteral("此记录属于先前的宏；修改后无法定位原代码，请重新调试当前宏。"));
                return;
            }
            const auto block = editor->document()->findBlockByNumber(line - 1);
            if (!block.isValid()) {
                return;
            }
            ShowEditor();
            editor->setTextCursor(QTextCursor(block));
            editor->centerCursor();
            editor->setFocus();
        });
        QObject::connect(buffs, &BuffPanel::CoverageChanged, timeline, [this] {
            timeline->SetBuffs(buffs->TimelineBuffs());
        });
        QObject::connect(buffs, &BuffPanel::DamageRequested, window, [this](quint32 sequence) {
            const auto hit = std::lower_bound(events.cbegin(), events.cend(), sequence, [](const CombatEvent &event, quint32 value) {
                return event.sequence < value;
            });
            if (hit == events.cend()) {
                result->setText(QStringLiteral("此增益变化后尚未记录伤害事件。"));
                return;
            }
            const int row = static_cast<int>(hit - events.cbegin());
            ShowAnalysis(eventsTable);
            eventsTable->selectRow(row);
            eventsTable->scrollToItem(eventsTable->item(row, 0));
        });
        QObject::connect(analysisDialog, &QDialog::finished, window, [this] { ShowEditor(); });

        result = new QLabel(QStringLiteral("准备就绪。设置参数与宏后，点击“开始模拟”。"), central);
        result->setObjectName(QStringLiteral("simulationResult"));
        result->setWordWrap(false);
        result->setFixedHeight(26);
        result->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        result->setTextInteractionFlags(Qt::TextSelectableByMouse);
        result->setStyleSheet(QStringLiteral("color: #92929d; font-size: 12px; padding: 3px;"));
        statusBar = new QWidget;
        statusBar->setObjectName(QStringLiteral("workspaceStatus"));
        auto *status = new QHBoxLayout(statusBar);
        status->setContentsMargins(0, 0, 0, 0);
        status->addWidget(result, 1);
        progress     = new QProgressBar(central);
        progress->setObjectName(QStringLiteral("batchProgress"));
        progress->setRange(0, 1000);
        progress->setValue(0);
        progress->setFixedWidth(190);
        progress->setFixedHeight(18);
        progress->hide();
        cancel = new QPushButton(QStringLiteral("取消模拟"), central);
        cancel->setObjectName(QStringLiteral("cancelBatch"));
        cancel->setEnabled(false);
        cancel->hide();
        status->addWidget(progress);
        status->addWidget(cancel);
        shell->bodyLayout->addWidget(statusBar);
        statusBar->setVisible(QSettings().value(QStringLiteral("appearance/showStatus"), true).toBool());
        progressTimer = new QTimer(window);
        progressTimer->setInterval(250);
        QObject::connect(progressTimer, &QTimer::timeout, window, [this] { UpdateProgress(); });
        QObject::connect(cancel, &QPushButton::clicked, window, [this] { CancelBatch(); });
        QObject::connect(simToolbar, &SimulationToolbar::StartSimulationClicked, window, [this] {
            if (batch) {
                return;
            }
            if (simToolbar->IsDebugModeEnabled()) {
                StartDebug();
            } else {
                StartBatch(false);
            }
        });
        QObject::connect(simToolbar, &SimulationToolbar::DebugModeToggled, window, [this](bool enabled) {
            if (enabled) {
                ShowEditor();
            } else {
                debugSession->Stop();
            }
            UpdateControls();
        });
        QObject::connect(editor, &CodeEditor::BreakpointAdded, debugSession, &DebugSession::AddBreakpoint);
        QObject::connect(editor, &CodeEditor::BreakpointRemoved, debugSession, &DebugSession::RemoveBreakpoint);
        QObject::connect(debugSession, &DebugSession::LineChanged, editor, &CodeEditor::SetCurrentDebugLine);
        QObject::connect(debugSession, &DebugSession::StateChanged, window, [this](DebugSession::State state) {
            if (state == DebugSession::Stopped) {
                statePanel->Clear();
                inspector->clear();
            }
            UpdateControls();
        });
        QObject::connect(debugSession, &DebugSession::DebugInfoUpdated, window, [this](const DebugSession::DebugInfo &info) {
            PlayerStatePanel::PlayerState state{
                info.lifePercent, info.manaPercent,       info.qidian,       info.rage,           info.energy,
                info.targetId,    info.targetLifePercent, info.currentFrame, info.currentSeconds, info.currentMacro,
                info.currentSkill
            };
            statePanel->UpdateState(state);
            inspector->setPlainText(QStringLiteral("下一阶段：%1\n%2\n\n上一步执行结果：\n%3\n\n%4")
                                        .arg(info.phase,
                                             info.condition,
                                             info.lastStep.isEmpty() ? QStringLiteral("尚未执行。") : info.lastStep,
                                             info.details.join('\n')));
            UpdateDebugPosition();
            progress->setValue(static_cast<int>(static_cast<qint64>(info.currentFrame) * 1000 / activeOptions.duration));
            result->setText(
                QStringLiteral("调试 | 第 %1 帧 | 第 %2 行 | %3").arg(info.currentFrame).arg(info.lineNumber).arg(info.phase));
            buffs->Observe(info.currentFrame);
        });
        QObject::connect(debugSession, &DebugSession::DamageEventsAvailable, window, [this](const CombatEvents &hits) {
            AppendEvents(hits);
        });
        QObject::connect(debugSession, &DebugSession::BuffEventsAvailable, buffs, &BuffPanel::Append);
        QObject::connect(debugSession, &DebugSession::HistoryAvailable, history, &HistoryPanel::Append);
        QObject::connect(debugSession, &DebugSession::MacroStepsAvailable, macroSteps, &MacroTracePanel::Append);
        QObject::connect(debugSession, &DebugSession::ErrorOccurred, window, [this](const QString &message) {
            Error(message);
        });
        QObject::connect(debugSession, &DebugSession::ExecutionFinished, window, [this] {
            result->setText(QStringLiteral("调试完成 | 总伤害 %1 | 秒伤 %2 | 共 %3 条事件")
                                .arg(debugSession->TotalDamage())
                                .arg(debugSession->TotalDamage() / (activeOptions.duration / 16.0), 0, 'f', 1)
                                .arg(events.size()));
        });
        QObject::connect(debugToolbar, &DebugToolbar::ContinueClicked, window, [this] { ContinueDebug(); });
        QObject::connect(debugToolbar, &DebugToolbar::PauseClicked, debugSession, &DebugSession::Pause);
        QObject::connect(debugToolbar, &DebugToolbar::StepIntoClicked, window, [this] { Step(0); });
        QObject::connect(debugToolbar, &DebugToolbar::StepOverClicked, window, [this] { Step(1); });
        QObject::connect(debugToolbar, &DebugToolbar::StepOutClicked, window, [this] { Step(2); });
        QObject::connect(debugToolbar, &DebugToolbar::RestartClicked, window, [this] { StartDebug(); });
        QObject::connect(debugToolbar, &DebugToolbar::StopClicked, debugSession, &DebugSession::Stop);

        auto shortcut = [&](const QKeySequence &key, auto callback) {
            for (auto *owner : {static_cast<QWidget *>(window), static_cast<QWidget *>(analysisDialog)}) {
                auto *item = new QShortcut(key, owner);
                QObject::connect(item, &QShortcut::activated, window, callback);
            }
        };
        shortcut(QKeySequence(Qt::Key_F5), [this] {
            if (simToolbar->IsDebugModeEnabled() || debugSession->GetState() == DebugSession::Paused ||
                debugSession->GetState() == DebugSession::Running)
            {
                ShowEditor();
                ContinueDebug();
            } else {
                StartBatch(false);
            }
        });
        shortcut(QKeySequence(Qt::Key_F6), [this] { debugSession->Pause(); });
        shortcut(QKeySequence(Qt::Key_F10), [this] { Step(1); });
        shortcut(QKeySequence(Qt::Key_F11), [this] { Step(0); });
        shortcut(QKeySequence(Qt::SHIFT | Qt::Key_F11), [this] { Step(2); });
        shortcut(QKeySequence(Qt::SHIFT | Qt::Key_F5), [this] {
            if (batch) {
                CancelBatch();
            } else {
                debugSession->Stop();
            }
        });
        shortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F5), [this] {
            if (!batch && simToolbar->IsDebugModeEnabled()) StartDebug();
        });
        shortcut(QKeySequence(Qt::Key_F3), [this] { search->Find(); });
        shortcut(QKeySequence(Qt::SHIFT | Qt::Key_F3), [this] { search->Find(true); });

        QObject::connect(editor, &CodeEditor::SyntaxErrorsChanged, window, [this](int) { ShowProblems(); });
        editor->SetSyntaxValidator([this](const QString &source) {
            const auto bytes    = source.toUtf8();
            const auto compiled = rt::MacroCompiler::Compile(std::string_view(bytes.constData(), bytes.size()),
                                                             desktop::MacroOptions(options->Specialization(), false));
            QList<SyntaxError> errors;
            const auto         lines = source.split('\n');
            for (const auto &error : compiled.second) {
                const int line = static_cast<int>(error.line);
                const int length = line > 0 && line <= lines.size() ? std::max(1, static_cast<int>(lines[line - 1].size())) : 1;
                errors.append(SyntaxError(line, 0, length, SyntaxError::Error, desktop::Diagnostic(Name(error.message))));
            }
            return errors;
        });
        QObject::connect(problems, &QTableWidget::cellActivated, window, [this](int row, int) {
            const int line = problems->item(row, 0)->text().toInt();
            editor->setTextCursor(QTextCursor(editor->document()->findBlockByNumber(line - 1)));
            editor->setFocus();
        });
        QObject::connect(timeline, &Timeline::EventClicked, window, [this](int index, const Timeline::EventItem &) {
            if (index < 0 || index >= eventsTable->rowCount()) {
                return;
            }
            ShowAnalysis(eventsTable);
            eventsTable->selectRow(index);
            eventsTable->scrollToItem(eventsTable->item(index, 0));
        });
        QObject::connect(editor->document(), &QTextDocument::modificationChanged, window, [this] { UpdateTitle(); });
        QObject::connect(editor->document(), &QTextDocument::contentsChanged, options, &SimulationOptions::ClearAttributeGains);
        QObject::connect(results, &ResultsPanel::InputsRequested, window, [this](const QString &source, const QJsonObject &configuration) {
            if (batch || editor->isReadOnly() || !ReplaceDraft()) {
                return;
            }
            QString error;
            if (!options->RestoreSnapshot(configuration, error)) {
                Error(error);
                return;
            }
            debugSession->Stop();
            filePath.clear();
            editor->ClearAllBreakpoints();
            editor->setPlainText(source);
            editor->document()->setModified(true);
            UpdateTitle();
            ShowEditor();
            result->setText(QStringLiteral("已恢复所选模拟的宏与配置。"));
        });

        documentClass = options->Specialization();
        macroEditor->SetSpecialization(documentClass);
        help->setHtml(MacroHelp(documentClass));
        QObject::connect(options, &SimulationOptions::SpecializationChanged, window, [this, macroEditor] {
            drafts[static_cast<int>(documentClass)] =
                Draft{ editor->toPlainText(), filePath, editor->document()->isModified(), editor->GetBreakpoints().values() };
            documentClass     = options->Specialization();
            const auto &draft = drafts[static_cast<int>(documentClass)];
            debugSession->Stop();
            editor->ClearAllBreakpoints();
            editor->setPlainText(draft ? draft->text : desktop::DefaultMacro(documentClass));
            filePath = draft ? draft->path : QString{};
            editor->document()->setModified(draft && draft->modified);
            if (draft) {
                for (int line : draft->breakpoints) {
                    editor->SetBreakpoint(line, true);
                }
            }
            macroEditor->SetSpecialization(documentClass);
            help->setHtml(MacroHelp(documentClass));
            ClearEvents();
            meanDps->setText(QStringLiteral("--"));
            outputDetails->setText(QStringLiteral("心法已切换，请重新模拟"));
            options->ClearAttributeGains();
            result->setText(QStringLiteral("已切换至%1，修改配置后可重新模拟。").arg(desktop::ClassName(documentClass)));
            result->setToolTip({});
            UpdateTitle();
            QMetaObject::invokeMethod(editor, "CheckSyntax", Qt::DirectConnection);
        });
        QSettings settings;
        const bool restoreDrafts = settings.value(QStringLiteral("workspace/restoreDrafts"), true).toBool();
        for (int i = 0; restoreDrafts && i < 2; ++i) {
            const auto key = QStringLiteral("macro/class%1/").arg(i);
            if (!settings.contains(key + "draft")) {
                continue;
            }
            Draft draft{ settings.value(key + "draft").toString(),
                         settings.value(key + "file").toString(),
                         settings.value(key + "modified").toBool(),
                         {} };
            for (const auto &line : settings.value(key + "breakpoints").toList()) {
                draft.breakpoints << line.toInt();
            }
            drafts[i] = draft;
        }
        filePath = restoreDrafts ? settings.value(QStringLiteral("macro/file")).toString() : QString{};
        editor->setPlainText(restoreDrafts ? settings.value(QStringLiteral("macro/draft"), desktop::DefaultMacro(documentClass)).toString()
                                          : desktop::DefaultMacro(documentClass));
        editor->document()->setModified(restoreDrafts && settings.value(QStringLiteral("macro/modified"), false).toBool());
        for (const auto &value : restoreDrafts ? settings.value(QStringLiteral("macro/breakpoints")).toList() : QVariantList{}) {
            const int line = value.toInt();
            if (line > 0 && line <= editor->blockCount()) {
                editor->SetBreakpoint(line, true);
            }
        }
        auto editorFont = editor->font();
        editorFont.setPixelSize(std::clamp(settings.value(QStringLiteral("appearance/editorSize"), 14).toInt(), 12, 20));
        editor->setFont(editorFont);
        settingsPage = BuildSettingsPage(false);
        aboutPage = BuildSettingsPage(true);
        shell->pages->addWidget(settingsPage);
        shell->pages->addWidget(aboutPage);
        QObject::connect(options, &SimulationOptions::SpecializationChanged, window, [this] {
            ApplySpecializationTheme();
        });
        shell->navigate = [this](const QString &page) { Navigate(page); };
        QObject::connect(views, &QTabWidget::currentChanged, window, [this](int) {
            if (shell->pages->currentWidget() != analysisDialog) return;
            currentPage = views->currentWidget() == results ? QStringLiteral("statistics")
                        : views->currentWidget() == buffs ? QStringLiteral("buffs") : QStringLiteral("timeline");
            shell->Select(currentPage);
        });
        QObject::connect(shell->refresh, &QToolButton::clicked, window, [this] { CheckMacro(); });
        ApplySpecializationTheme();
        Navigate(QStringLiteral("simulation"));
        UpdateTitle();
        UpdateControls();
    }

    ~Impl()
    {
        if (batch) {
            batch->RequestCancel();
            batch->wait();
        }
    }

    void ApplySpecializationTheme()
    {
        const auto *choice = options->findChild<QComboBox *>(QStringLiteral("specialization"));
        const auto image = choice->itemIcon(choice->currentIndex()).pixmap(QSize(64, 64), 1.0).toImage();
        const auto accent = desktop::theme::IconAccent(image);
        if (window->property("workbenchAccent").value<QColor>() == accent) return;
        window->setProperty("workbenchAccent", accent);
        auto colors = window->palette();
        colors.setColor(QPalette::Accent, accent);
        colors.setColor(QPalette::Link, accent);
        window->setPalette(colors);
        window->setStyleSheet(desktop::theme::WorkbenchStyleSheet(accent));
        simToolbar->SetAccentColor(accent);
        for (auto *widget : window->findChildren<QWidget *>()) {
            // QSS accent-color handles descendants; native child windows also need their own palette.
            if (widget->isWindow()) {
                widget->setAttribute(Qt::WA_WindowPropagation);
                auto childColors = widget->palette();
                childColors.setColor(QPalette::Accent, accent);
                childColors.setColor(QPalette::Link, accent);
                widget->setPalette(childColors);
            }
            widget->update();
        }
    }

    QWidget *BuildSettingsPage(bool about)
    {
        auto *scroll = new QScrollArea;
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidgetResizable(true);
        auto *page = new QWidget;
        page->setObjectName(about ? QStringLiteral("aboutPage") : QStringLiteral("settingsPage"));
        auto *layout = new QVBoxLayout(page);
        layout->setContentsMargins(0, 0, 8, 0);
        layout->setSpacing(18);
        auto *searchSettings = new QLineEdit;
        searchSettings->setObjectName(about ? QStringLiteral("searchAbout") : QStringLiteral("searchSettings"));
        searchSettings->setPlaceholderText(QStringLiteral("查找设置"));
        searchSettings->setClearButtonEnabled(true);
        searchSettings->setFixedWidth(200);
        searchSettings->setAccessibleName(QStringLiteral("查找设置"));
        shell->headerLayout->insertWidget(shell->headerLayout->count() - 1, searchSettings);
        searchSettings->hide();
        auto *card = new QFrame;
        card->setProperty("settingsCard", true);
        auto *rows = new QVBoxLayout(card);
        rows->setContentsMargins(1, 1, 1, 1);
        rows->setSpacing(0);
        auto add = [rows](const QString &title, const QString &description, QWidget *control) {
            auto *row = new QFrame;
            row->setProperty("settingsRow", true);
            row->setProperty("searchText", title + ' ' + description);
            row->setMinimumHeight(description.isEmpty() ? 54 : 67);
            auto *content = new QHBoxLayout(row);
            content->setContentsMargins(15, 14, 15, 14);
            content->setSpacing(20);
            auto *labels = new QVBoxLayout;
            labels->setSpacing(3);
            auto *name = new QLabel(title);
            name->setProperty("cardTitle", true);
            labels->addWidget(name);
            if (!description.isEmpty()) {
                auto *note = new QLabel(description);
                note->setProperty("muted", true);
                note->setWordWrap(true);
                labels->addWidget(note);
            }
            content->addLayout(labels, 1);
            control->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
            content->addWidget(control);
            rows->addWidget(row);
        };
        auto value = [](const QString &text) {
            auto *label = new QLabel(text);
            label->setProperty("muted", true);
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
            return label;
        };
        auto toggle = [this](const char *name, const char *key, bool fallback, std::function<void(bool)> changed = {}) {
            auto *control = new desktop::ToggleSwitch;
            control->setObjectName(QString::fromLatin1(name));
            control->setChecked(QSettings().value(QString::fromLatin1(key), fallback).toBool());
            QObject::connect(control, &QAbstractButton::toggled, window, [key = QString::fromLatin1(key), changed](bool enabled) {
                QSettings().setValue(key, enabled);
                if (changed) changed(enabled);
            });
            return control;
        };
        if (about) {
            add(QStringLiteral("版本"), QStringLiteral("JX3DPS · 剑网三战斗模拟器"), value(QStringLiteral("桌面工作台")));
            add(QStringLiteral("Qt 版本"), QStringLiteral("原生 Qt Widgets 界面，不嵌入网页"), value(QString::fromLatin1(qVersion())));
            add(QStringLiteral("支持心法"), QStringLiteral("独立保存心法配置与循环宏草稿"), value(QStringLiteral("太虚剑意 / 莫问")));
            add(QStringLiteral("模拟引擎"), QStringLiteral("本地多线程计算；随机种子、战斗记录与断点调试可复现"), value(QStringLiteral("本地运行")));
            add(QStringLiteral("规则与数据"), QStringLiteral("战斗使用已迁移的旧版规则。新版九重奇穴仅用于配装预览，尚未接入伤害计算。"), value(QStringLiteral("旧规则")));
            auto *syntax = new QToolButton;
            syntax->setObjectName(QStringLiteral("openMacroHelpFromAbout"));
            syntax->setText(QStringLiteral("打开"));
            QObject::connect(syntax, &QToolButton::clicked, window, [this] { ShowMacroHelp(); });
            add(QStringLiteral("宏语法"), QStringLiteral("查看宏命令、条件表达式和断点调试快捷键"), syntax);
            auto *copy = new QToolButton;
            copy->setObjectName(QStringLiteral("copySettingsLocation"));
            copy->setText(QStringLiteral("复制位置"));
            copy->setToolTip(QSettings().fileName());
            QObject::connect(copy, &QToolButton::clicked, window, [this] {
                QApplication::clipboard()->setText(QSettings().fileName());
                result->setText(QStringLiteral("已复制设置存储位置。"));
            });
            add(QStringLiteral("配置存储"), QStringLiteral("宏草稿、窗口设置与模拟配置通过 QSettings 保存在本机"), copy);
            add(QStringLiteral("界面"), QStringLiteral("按提供的 Magpie 截图还原视觉样式，功能为剑三模拟器"), value(QStringLiteral("Qt 6.12")));
        } else {
            add(QStringLiteral("外观"), QStringLiteral("与参考截图一致的炭灰色背景、列表卡片与分段控件"), value(QStringLiteral("深色")));
            auto *remember = toggle("rememberGeometry", "workspace/rememberGeometry", true);
            remember->setAccessibleName(QStringLiteral("记住窗口尺寸"));
            add(QStringLiteral("记住窗口尺寸"), QStringLiteral("下次启动时恢复窗口的大小与位置"), remember);
            auto *drafts = toggle("restoreDrafts", "workspace/restoreDrafts", true);
            drafts->setAccessibleName(QStringLiteral("恢复宏草稿"));
            add(QStringLiteral("恢复宏草稿"), QStringLiteral("启动时恢复各心法的宏、断点和未保存内容；关闭后使用默认宏"), drafts);
            auto *status = toggle("showStatus", "appearance/showStatus", true, [this](bool enabled) {
                const auto state = debugSession->GetState();
                statusBar->setVisible(enabled || batch || state == DebugSession::Running || state == DebugSession::Paused);
            });
            status->setAccessibleName(QStringLiteral("运行状态栏"));
            add(QStringLiteral("运行状态栏"), QStringLiteral("在页面底部显示语法诊断与模拟状态；运行时始终显示取消按钮"), status);
            auto *size = new QComboBox;
            size->setObjectName(QStringLiteral("editorFontSize"));
            for (int pixels : {12, 13, 14, 16, 18, 20}) size->addItem(QStringLiteral("%1 px").arg(pixels), pixels);
            size->setCurrentIndex(size->findData(editor->font().pixelSize()));
            QObject::connect(size, &QComboBox::currentIndexChanged, window, [this, size] {
                auto font = editor->font();
                font.setPixelSize(size->currentData().toInt());
                editor->setFont(font);
                QSettings().setValue(QStringLiteral("appearance/editorSize"), size->currentData());
            });
            add(QStringLiteral("编辑器字号"), QStringLiteral("仅调整循环宏编辑器，不改变界面布局与系统缩放"), size);
            add(QStringLiteral("配置导入与导出"), QStringLiteral("在配置库中管理命名方案，导入或导出完整的 JSON 配置"), [&] {
                auto *button = new QToolButton;
                button->setText(QStringLiteral("打开"));
                QObject::connect(button, &QToolButton::clicked, window, [this] { Navigate(QStringLiteral("profiles")); });
                return button;
            }());
        }
        if (rows->count()) rows->itemAt(rows->count() - 1)->widget()->setProperty("settingsLastRow", true);
        layout->addWidget(card);
        auto *empty = new QLabel(QStringLiteral("没有匹配的设置"));
        empty->setProperty("muted", true);
        empty->hide();
        layout->addWidget(empty);
        layout->addStretch();
        QObject::connect(searchSettings, &QLineEdit::textChanged, page, [rows, card, empty](const QString &text) {
            int found = 0;
            for (int i = 0; i < rows->count(); ++i) {
                auto *row = rows->itemAt(i)->widget();
                const bool matches = row->property("searchText").toString().contains(text.trimmed(), Qt::CaseInsensitive);
                row->setVisible(matches);
                found += matches;
            }
            card->setVisible(found > 0);
            empty->setVisible(found == 0);
        });
        scroll->setWidget(page);
        return scroll;
    }

    void Navigate(const QString &page)
    {
        currentPage = page;
        shell->Select(page);
        window->findChild<QLineEdit *>(QStringLiteral("searchSettings"))->setVisible(page == "general");
        window->findChild<QLineEdit *>(QStringLiteral("searchAbout"))->setVisible(page == "about");
        if (page == "general" || page == "about") {
            shell->pages->setCurrentWidget(page == "general" ? settingsPage : aboutPage);
        } else if (page == "statistics" || page == "timeline" || page == "buffs") {
            if (page == "statistics") results->ShowOverview();
            views->setCurrentWidget(page == "statistics" ? static_cast<QWidget *>(results)
                                  : page == "timeline" ? views->widget(0) : buffs);
            shell->pages->setCurrentWidget(analysisDialog);
            analysisDialog->show();
        } else {
            shell->pages->setCurrentWidget(homePage);
            const bool macro = page == "macro";
            const bool overview = page == "simulation";
            configurationScroll->setVisible(!macro);
            configurationScroll->setMaximumWidth(overview ? 760 : QWIDGETSIZE_MAX);
            homeEditor->setVisible(macro || overview);
            options->ShowSection(overview || macro ? QStringLiteral("simulation") : page);
            if (overview) body->setSizes({664, qMax(280, body->width() - 676)});
            pageNote->setText(macro ? QStringLiteral("编辑循环宏，点击行号设置断点。Ctrl+Space 补全 · F5 运行 · F10 单步。")
                : overview ? QStringLiteral("设置心法、属性与循环宏，在本地运行战斗模拟。相同配置与随机种子可复现结果。")
                : page == "attributes" ? QStringLiteral("属性与收益逐行对应；点击“配装…”打开独立窗口，数值和加成在“编辑数值”中调整。")
                : page == "talents" ? QStringLiteral("新版九重奇穴仅用于配装预览；战斗模拟使用已迁移的旧规则。")
                : page == "permanents" ? QStringLiteral("选择阵法、食品、药品与宴席。常驻属性在每场战斗开始前应用。")
                : QStringLiteral("保存、切换或导入配置方案。修改后点击开始模拟，结果保留在模拟统计中。"));
        }
        UpdateLayout();
        UpdateControls();
    }

    void UpdateLayout()
    {
        const bool compact = currentPage == QStringLiteral("simulation") && window->height() < 900;
        pageNote->setVisible(!compact);
        shell->SetCompactHeader(compact);
    }

    void ShowAnalysis(QWidget *page)
    {
        window->findChild<QLineEdit *>(QStringLiteral("searchSettings"))->hide();
        window->findChild<QLineEdit *>(QStringLiteral("searchAbout"))->hide();
        views->setCurrentWidget(page);
        currentPage = page == results ? QStringLiteral("statistics") : page == buffs ? QStringLiteral("buffs") : QStringLiteral("timeline");
        shell->Select(currentPage);
        shell->pages->setCurrentWidget(analysisDialog);
        analysisDialog->show();
        UpdateLayout();
        UpdateControls();
    }

    void ShowEditor()
    {
        if (shell->pages->currentWidget() != homePage || !homeEditor->isVisible()) Navigate(QStringLiteral("macro"));
    }

    void ShowMacroHelp()
    {
        helpDialog->show();
        helpDialog->raise();
        helpDialog->activateWindow();
    }

    void UpdateOutput(const SimulationResult &stats)
    {
        meanDps->setText(stats.stats.count ? QString::number(stats.MeanDps(), 'f', 1) : QStringLiteral("--"));
        outputDetails->setText(
            QStringLiteral("%1 · %2\n%3 次 · 耗时 %4 秒")
                .arg(desktop::ClassName(documentClass))
                .arg(stats.Complete() ? (stats.trace ? QStringLiteral("单次记录") : QStringLiteral("已完成")) : QStringLiteral("已取消"))
                .arg(stats.stats.count)
                .arg(stats.elapsedSeconds, 0, 'f', 2));
        outputDetails->setToolTip(QStringLiteral("%1 · 战斗时长 %2 秒 · 随机种子 %3")
                                      .arg(desktop::ClassName(documentClass))
                                      .arg(stats.FightSeconds())
                                      .arg(stats.options.seed));
    }

    void Error(const QString &message)
    {
        result->setText(message);
        QMessageBox::critical(window, QStringLiteral("JX3DPS"), message);
    }

    void UpdateTitle()
    {
        const auto name = filePath.isEmpty() ? QStringLiteral("未命名宏") : QFileInfo(filePath).fileName();
        documentLabel->setText(
            QStringLiteral("%1 · %2%3")
                .arg(desktop::ClassName(documentClass), name, editor->document()->isModified() ? QStringLiteral(" *") : QString{}));
        documentLabel->setToolTip(filePath.isEmpty() ? QStringLiteral("下次启动时恢复宏草稿和配置。") : filePath);
        window->setWindowTitle(QStringLiteral("JX3DPS"));
    }

    void UpdateControls()
    {
        const auto state     = debugSession->GetState();
        const bool debugging = state == DebugSession::Running || state == DebugSession::Paused;
        editor->setReadOnly(debugging);
        completeAction->setEnabled(!debugging);
        options->SetInputsEnabled(!batch && !debugging);
        results->SetInputsEnabled(!batch && !debugging);
        newAction->setEnabled(!debugging);
        openAction->setEnabled(!debugging);
        simToolbar->setEnabled(!batch);
        simToolbar->findChild<QToolButton *>(QStringLiteral("startButton"))
            ->setEnabled(!batch && !debugging);
        macroRun->setEnabled(!batch && !debugging);
        macroRun->setText(simToolbar->IsDebugModeEnabled() ? QStringLiteral("开始调试") : QStringLiteral("开始模拟"));
        macroRun->setVisible(currentPage == QStringLiteral("macro"));
        traceAction->setEnabled(!batch && !debugging);
        gainsAction->setEnabled(!batch && !debugging);
        cancel->setEnabled(batch && !cancelling);
        cancel->setVisible(batch);
        progress->setVisible(batch || debugging);
        statusBar->setVisible(batch || debugging || QSettings().value(QStringLiteral("appearance/showStatus"), true).toBool());
        shell->engineStatus->setText(QStringLiteral("<span style='color:#6fd99b'>●</span> &nbsp;本地引擎 · %1")
            .arg(batch ? QStringLiteral("计算中") : debugging ? QStringLiteral("调试中") : QStringLiteral("就绪")));
        exportAction->setEnabled(!events.isEmpty());
        const bool debugMode = simToolbar->IsDebugModeEnabled();
        options->OutputLayout()->parentWidget()->setVisible(currentPage == QStringLiteral("simulation") && !debugMode);
        debugToggle->setEnabled(!batch);
        const bool openingDetails = debugMode && debugTabs->isHidden();
        debugTabs->setVisible(debugMode);
        if (openingDetails) macroSplit->setSizes({qMax(150, macroSplit->height() - 315), 315});
        debugPosition->setVisible(debugMode);
        UpdateDebugPosition();
        window->findChild<QAction *>(QStringLiteral("debugStart"))->setVisible(state == DebugSession::Stopped);
        window->findChild<QAction *>(QStringLiteral("debugRestart"))->setVisible(state != DebugSession::Stopped);
        window->findChild<QAction *>(QStringLiteral("debugContinue"))->setEnabled(!batch && state == DebugSession::Paused);
        window->findChild<QAction *>(QStringLiteral("debugPause"))->setEnabled(!batch && state == DebugSession::Running);
        for (const auto *name : { "debugStepOver", "debugStepInto", "debugStepOut" }) {
            window->findChild<QAction *>(QString::fromLatin1(name))->setEnabled(!batch && state == DebugSession::Paused);
        }
        window->findChild<QAction *>(QStringLiteral("debugStop"))->setEnabled(!batch && state != DebugSession::Stopped);
        debugToolbar->SetDebugState(state == DebugSession::Running ? DebugToolbar::Running
                                    : state == DebugSession::Paused ? DebugToolbar::Paused
                                    : state == DebugSession::Finished ? DebugToolbar::Finished
                                                                     : DebugToolbar::Stopped);
        if (debugMode && !batch && shell->pages->currentWidget() == homePage && homeEditor->isVisible()) debugToolbar->ShowFloating();
        else debugToolbar->HideFloating();
    }

    void UpdateDebugPosition()
    {
        const auto state = debugSession->GetState();
        if (state == DebugSession::Stopped) {
            debugPosition->setText(QStringLiteral("点击行号设置断点 · 浮动工具条开始调试"));
            return;
        }
        const auto info = debugSession->GetDebugInfo();
        const auto label = state == DebugSession::Paused ? QStringLiteral("已暂停")
                         : state == DebugSession::Running ? QStringLiteral("运行中") : QStringLiteral("已完成");
        debugPosition->setText(QStringLiteral("%1 · 第 %2 帧 · %3 秒%4")
                                   .arg(label).arg(info.currentFrame).arg(info.currentSeconds, 0, 'f', 2)
                                   .arg(info.lineNumber > 0 ? QStringLiteral(" · 第 %1 行").arg(info.lineNumber) : QString{}));
    }

    void ShowProblems()
    {
        const auto errors = editor->GetSyntaxErrors();
        problems->setRowCount(errors.size());
        for (int row = 0; row < errors.size(); ++row) {
            problems->setItem(row, 0, new QTableWidgetItem(QString::number(errors[row].line)));
            problems->setItem(row, 1, new QTableWidgetItem(errors[row].message));
        }
        macroStatus->setText(errors.isEmpty() ? QStringLiteral("语法检查通过 · 点击行号设置断点 · Ctrl+Space 补全")
                                              : QStringLiteral("宏有 %1 处错误，点击“检查宏”查看详情").arg(errors.size()));
        if (errors.isEmpty()) {
            problems->hide();
        }
    }

    bool CheckMacro()
    {
        // Force validation immediately, including edits still awaiting the debounce timer.
        QMetaObject::invokeMethod(editor, "CheckSyntax", Qt::DirectConnection);
        ShowProblems();
        if (problems->rowCount()) {
            ShowEditor();
            problems->show();
            result->setText(QStringLiteral("宏有 %1 处错误；点击诊断可定位对应行。").arg(problems->rowCount()));
            return false;
        }
        QString flowError;
        if (!options->CheckFlowMacro(flowError)) {
            Navigate(QStringLiteral("simulation"));
            options->FocusFlowMacro();
            result->setText(flowError);
            return false;
        }
        result->setText(QStringLiteral("宏检查通过。"));
        return true;
    }

    void ClearEvents()
    {
        events.clear();
        history->Clear();
        timeline->Clear();
        timeline->SetTimeRange(0, static_cast<int>((static_cast<qint64>(activeOptions.duration) * 1000 + 15) / 16));
        buffs->Reset(activeOptions.duration, documentClass);
        macroSteps->Reset();
        eventsTable->setRowCount(0);
        results->ClearTrace(activeOptions.duration / 16.0);
        exportAction->setEnabled(false);
    }

    void AppendEvents(const CombatEvents &hits)
    {
        eventsTable->setUpdatesEnabled(false);
        const int begin  = static_cast<int>(events.size());
        events          += hits;
        eventsTable->setRowCount(events.size());
        for (int index = 0; index < hits.size(); ++index) {
            const auto   &hit = hits[index];
            const QString outcome =
                hit.outcome == 1   ? QStringLiteral("会心")
                : hit.outcome == 0 ? QStringLiteral("命中")
                : hit.outcome == 4 ? QStringLiteral("识破")
                                   : QStringLiteral("未命中");
            const QStringList cells{ QString::number(hit.frame),
                                     QString::number(hit.frame / 16.0, 'f', 4),
                                     hit.name,
                                     QString::number(hit.damage),
                                     outcome,
                                     QString::number(hit.sequence) };
            for (int column = 0; column < cells.size(); ++column) {
                eventsTable->setItem(begin + index, column, new QTableWidgetItem(cells[column]));
            }
            Timeline::EventItem event{};
            event.timestamp  = hit.frame * 62.5;
            event.name       = hit.name;
            event.damage     = hit.damage;
            event.rollResult = hit.outcome == 1 ? 2 : hit.outcome == 0 ? 1 : hit.outcome == 4 ? 3 : 0;
            event.color      = QColor::fromHsv((hit.skill * 37 + 190) % 360, 140, 210);
            event.macroName  = desktop::ClassName(documentClass);
            event.macroColor = QColor(100, 180, 150);
            event.icon = desktop::GameIcon(documentClass, hit.name).pixmap(40, 40);
            timeline->AddEvent(event);
        }
        eventsTable->setUpdatesEnabled(true);
        results->AppendTrace(hits);
        exportAction->setEnabled(!events.isEmpty());
    }

    void StartBatch(bool trace)
    {
        if (batch || debugSession->GetState() == DebugSession::Running || debugSession->GetState() == DebugSession::Paused)
        {
            return;
        }
        if (!CheckMacro()) {
            return;
        }
        try {
            const bool gains = !trace && options->AutoCalculateGains();
            auto run = options->Batch();
            if (trace) {
                run.iterations = 1;
                run.workers    = 1;
            }
            const auto source   = editor->toPlainText().toUtf8();
            auto       compiled = rt::MacroCompiler::Compile(std::string_view(source.constData(), source.size()),
                                                             desktop::MacroOptions(options->Specialization(), false));
            auto       config   = options->ActiveConfig();
            auto       snapshot = options->Snapshot();
            auto       values   = snapshot.value("values").toObject();
            values.insert("iterations", static_cast<int>(run.iterations));
            values.insert("workers", static_cast<int>(run.workers));
            values.insert("seconds", run.duration / 16.0);
            snapshot.insert("values", values);
            debugSession->Stop();
            activeOptions = run;
            ClearEvents();
            if (trace) {
                ShowAnalysis(views->widget(0));
            }
            cancelling = false;
            progress->setValue(0);
            progress->setFormat(QStringLiteral("%p%"));
            batch        = new BatchWorker(std::move(compiled.first.program), run, config, window, trace, gains, options->GainMode());
            auto *worker = batch.data();
            QObject::connect(worker,
                             &BatchWorker::StatisticsReady,
                             window,
                             [this, source = editor->toPlainText(), snapshot, trace](const SimulationResult &stats) {
                                 results->AddResult(stats, source, snapshot);
                                 UpdateOutput(stats);
                                 if (source == editor->toPlainText() && snapshot == options->Snapshot())
                                     options->SetAttributeGainResult(stats);
                                 else options->ClearAttributeGains();
                                 if (!trace) {
                                     views->setCurrentWidget(results);
                                 }
                             });
            QObject::connect(worker, &BatchWorker::ResultReady, window, [this](const QString &text) {
                progressTimer->stop();
                progress->setValue(static_cast<int>(batch->Completed() * 1000 / batch->Requested()));
                result->setText(QStringLiteral("%1 | 平均秒伤 %2 | 详细结果见“技能统计”")
                                    .arg(QString(outputDetails->text()).replace('\n', QStringLiteral(" · ")), meanDps->text()));
                result->setToolTip(text);
            });
            QObject::connect(worker, &BatchWorker::Failed, window, [this](const QString &message) {
                progressTimer->stop();
                result->setText(QStringLiteral("模拟失败：%1").arg(message));
            });
            QObject::connect(worker, &BatchWorker::TraceReady, window, [this](const CombatEvents &hits) {
                AppendEvents(hits);
                timeline->SetTimeRange(0, static_cast<int>((static_cast<qint64>(activeOptions.duration) * 1000 + 15) / 16));
            });
            QObject::connect(worker, &BatchWorker::BuffTraceReady, window, [this](const BuffEvents &events) {
                buffs->Append(events);
                buffs->Observe(activeOptions.duration);
            });
            QObject::connect(worker, &BatchWorker::HistoryReady, history, &HistoryPanel::Append);
            QObject::connect(worker, &QThread::finished, window, [this, worker] {
                batch = nullptr;
                progressTimer->stop();
                worker->deleteLater();
                UpdateControls();
                if (closing) {
                    window->close();
                }
            });
            elapsed.start();
            result->setText(QStringLiteral("正在准备 %1 次模拟，使用 %2 个线程……").arg(run.iterations).arg(run.workers));
            UpdateControls();
            progressTimer->start();
            worker->start();
        } catch (const std::exception &error) {
            Error(desktop::Diagnostic(QString::fromUtf8(error.what())));
        }
    }

    void UpdateProgress()
    {
        if (!batch) {
            return;
        }
        const auto completed = batch->Completed();
        progress->setValue(static_cast<int>(completed * 1000 / batch->Requested()));
        const auto seconds = elapsed.elapsed() / 1000.0;
        const auto rate    = seconds > 0 ? completed / seconds : 0;
        const auto eta = rate > 0 ? QString::number((batch->Requested() - completed) / rate, 'f', 1) : QStringLiteral("--");
        result->setText(QStringLiteral("%1 %2 / %3 次 | 每秒 %4 次 | 已用 %5 秒 | 剩余 %6 秒")
                            .arg(cancelling ? QStringLiteral("当前战斗完成后取消：") : QStringLiteral("运行中"))
                            .arg(completed)
                            .arg(batch->Requested())
                            .arg(rate, 0, 'f', 0)
                            .arg(seconds, 0, 'f', 1)
                            .arg(eta));
    }

    void CancelBatch()
    {
        if (!batch) {
            return;
        }
        cancelling = true;
        batch->RequestCancel();
        cancel->setEnabled(false);
        UpdateProgress();
    }

    bool StartDebug()
    {
        if (batch || !CheckMacro()) {
            return false;
        }
        try {
            debugToggle->setChecked(true);
            ShowEditor();
            activeOptions = options->Batch();
            debugSession->Stop();
            ClearEvents();
            debugSession->SetSimulationOptions(options->ActiveConfig(), activeOptions.duration, activeOptions.seed);
            progress->setValue(0);
            progress->setFormat(QStringLiteral("调试 %p%"));
            macroSteps->Reset(editor->toPlainText());
            debugSession->Start(editor->toPlainText());
            return debugSession->GetState() == DebugSession::Paused;
        } catch (const std::exception &error) {
            Error(desktop::Diagnostic(QString::fromUtf8(error.what())));
            return false;
        }
    }

    void ContinueDebug()
    {
        if (batch || !simToolbar->IsDebugModeEnabled()) {
            return;
        }
        ShowEditor();
        if (debugSession->GetState() == DebugSession::Stopped || debugSession->GetState() == DebugSession::Finished) {
            if (!StartDebug()) {
                return;
            }
        }
        debugSession->Continue();
    }

    void Step(int mode)
    {
        if (batch || !simToolbar->IsDebugModeEnabled()) {
            return;
        }
        ShowEditor();
        if (debugSession->GetState() == DebugSession::Stopped || debugSession->GetState() == DebugSession::Finished) {
            if (!StartDebug()) {
                return;
            }
        }
        if (mode == 0) {
            debugSession->StepInto();
        } else if (mode == 1) {
            debugSession->StepOver();
        } else {
            debugSession->StepOut();
        }
    }

    bool SaveMacro(bool saveAs)
    {
        auto path = filePath;
        if (saveAs || path.isEmpty()) {
            path = QFileDialog::getSaveFileName(window, QStringLiteral("保存宏"), path, QStringLiteral("宏文件 (*.txt);;所有文件 (*)"));
        }
        if (path.isEmpty()) {
            return false;
        }
        QSaveFile  file(path);
        const auto bytes = editor->toPlainText().toUtf8();
        if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
            Error(QStringLiteral("无法保存宏：%1").arg(file.errorString()));
            return false;
        }
        filePath = path;
        editor->document()->setModified(false);
        UpdateTitle();
        SaveDraft();
        return true;
    }

    bool ReplaceDraft()
    {
        if (!editor->document()->isModified()) {
            return true;
        }
        const auto choice =
            QMessageBox::question(window,
                                  QStringLiteral("宏尚未保存"),
                                  QStringLiteral("替换前是否保存当前宏？"),
                                  QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                  QMessageBox::Save);
        if (choice == QMessageBox::Save) {
            return SaveMacro(false);
        }
        return choice == QMessageBox::Discard;
    }

    void NewMacro()
    {
        if (!ReplaceDraft()) {
            return;
        }
        debugSession->Stop();
        filePath.clear();
        editor->ClearAllBreakpoints();
        editor->setPlainText(desktop::DefaultMacro(documentClass));
        editor->document()->setModified(false);
        ShowEditor();
        UpdateTitle();
    }

    void OpenMacro()
    {
        if (!ReplaceDraft()) {
            return;
        }
        const auto path =
            QFileDialog::getOpenFileName(window, QStringLiteral("打开宏"), filePath, QStringLiteral("宏文件 (*.txt);;所有文件 (*)"));
        if (path.isEmpty()) {
            return;
        }
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            Error(file.errorString());
            return;
        }
        auto bytes = file.readAll();
        if (file.error() != QFileDevice::NoError) {
            Error(file.errorString());
            return;
        }
        if (bytes.startsWith("\xef\xbb\xbf")) {
            bytes.remove(0, 3);
        }
        debugSession->Stop();
        filePath = path;
        editor->ClearAllBreakpoints();
        editor->setPlainText(QString::fromUtf8(bytes));
        editor->document()->setModified(false);
        ShowEditor();
        UpdateTitle();
    }

    void ExportTrace()
    {
        const auto path =
            QFileDialog::getSaveFileName(window, QStringLiteral("导出伤害记录"), QStringLiteral("trace.csv"), QStringLiteral("表格文件 (*.csv)"));
        if (path.isEmpty()) {
            return;
        }
        QSaveFile file(path);
        if (!file.open(QIODevice::WriteOnly)) {
            Error(file.errorString());
            return;
        }
        bool valid = file.write("frame,seconds,skill,damage,outcome,sequence\n") > 0;
        for (const auto &event : events) {
            auto skill = event.name;
            skill.replace('"', QStringLiteral("\"\""));
            const auto row =
                QStringLiteral("%1,%2,\"%3\",%4,%5,%6\n")
                    .arg(event.frame)
                    .arg(event.frame / 16.0, 0, 'f', 4)
                    .arg(skill)
                    .arg(event.damage)
                    .arg(event.outcome)
                    .arg(event.sequence)
                    .toUtf8();
            valid = valid && file.write(row) == row.size();
        }
        if (!valid || !file.commit()) {
            Error(QStringLiteral("无法导出记录：%1").arg(file.errorString()));
        }
    }

    void SaveDraft()
    {
        QSettings settings;
        settings.setValue(QStringLiteral("macro/draft"), editor->toPlainText());
        settings.setValue(QStringLiteral("macro/file"), filePath);
        settings.setValue(QStringLiteral("macro/modified"), editor->document()->isModified());
        QVariantList breakpoints;
        for (int line : editor->GetBreakpoints()) {
            breakpoints << line;
        }
        settings.setValue(QStringLiteral("macro/breakpoints"), breakpoints);
        if (settings.value(QStringLiteral("workspace/rememberGeometry"), true).toBool())
            settings.setValue(QStringLiteral("window/geometry"), window->saveGeometry());
        drafts[static_cast<int>(documentClass)] =
            Draft{ editor->toPlainText(), filePath, editor->document()->isModified(), editor->GetBreakpoints().values() };
        for (int i = 0; i < 2; ++i) {
            if (drafts[i]) {
                const auto  key   = QStringLiteral("macro/class%1/").arg(i);
                const auto &draft = *drafts[i];
                settings.setValue(key + "draft", draft.text);
                settings.setValue(key + "file", draft.path);
                settings.setValue(key + "modified", draft.modified);
                QVariantList lines;
                for (int line : draft.breakpoints) {
                    lines << line;
                }
                settings.setValue(key + "breakpoints", lines);
            }
        }
        options->SaveSettings();
    }
};

Workbench::Workbench(QWidget *parent) : Frameless(parent)
{
    SetupDefaultTitleBar();
    setWindowIcon(desktop::LineIcon(desktop::ShellIcon::Terminal));
    resize(1206, 860);
    setMinimumSize(1120, 820);
    d = std::make_unique<Impl>(this);
    QSettings settings;
    if (settings.value(QStringLiteral("workspace/rememberGeometry"), true).toBool() && settings.contains(QStringLiteral("window/geometry"))) {
        restoreGeometry(settings.value(QStringLiteral("window/geometry")).toByteArray());
    }
}

Workbench::~Workbench() = default;

void Workbench::resizeEvent(QResizeEvent *event)
{
    Frameless::resizeEvent(event);
    if (d) d->UpdateLayout();
}

void Workbench::closeEvent(QCloseEvent *event)
{
    d->analysisDialog->close();
    d->helpDialog->close();
    d->options->CloseAuxiliaryWindows();
    d->SaveDraft();
    d->debugSession->Stop();
    if (d->batch) {
        d->closing = true;
        d->CancelBatch();
        event->ignore();
    } else {
        d->debugToolbar->HideFloating();
        Frameless::closeEvent(event);
    }
}
