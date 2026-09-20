#include "workbench.h"
#include "game_icons.h"
#include "history_panel.h"

#include <QAction>
#include <QCloseEvent>
#include <QDialog>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QSaveFile>
#include <QScrollArea>
#include <QSettings>
#include <QShortcut>
#include <QSplitter>
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
#include "controls/equipment_panel/equipment_panel.h"
#include "controls/timeline/timeline.h"

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
    CodeEditor             *editor;
    MacroSearchBar         *search;
    SimulationOptions      *options;
    SimulationToolbar      *simToolbar;
    DebugToolbar           *debugToolbar;
    DebugSession           *debugSession;
    Timeline               *timeline;
    HistoryPanel           *history;
    PlayerStatePanel       *statePanel;
    QTextBrowser           *inspector;
    QTextBrowser           *help;
    QListWidget            *breakpointList;
    QToolBar               *debugCommands;
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
    QTabWidget                         *workspace;
    QWidget                            *editorPage;
    QVBoxLayout                        *homeEditorLayout;
    QVBoxLayout                        *debugEditorLayout;
    QLabel                             *macroStatus;
    QLabel                             *meanDps;
    QLabel                             *outputDetails;
    QTableWidget                       *gainOverview;
    QDialog                            *helpDialog;
    QDialog                            *equipmentDialog = nullptr;
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
        central->setStyleSheet(QStringLiteral(R"(
            QWidget#workbenchContent { background: #171c22; }
            QGroupBox { border: 1px solid #39424b; border-radius: 4px; margin-top: 10px; font-weight: 600; }
            QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #b6c9ce; }
            QTabWidget::pane { border: 1px solid #39424b; background: #1d232a; }
            QTabBar::tab { padding: 7px 12px; background: #20272f; color: #a8b4be; border-bottom: 2px solid transparent; }
            QTabBar::tab:selected { color: #d1ebe1; border-bottom-color: #76bba4; background: #2a353e; }
            QTabBar::tab:hover { background: #303b44; }
            QToolBar { border: 0; spacing: 3px; }
            QToolButton { padding: 5px 7px; }
            QPushButton { min-height: 24px; padding: 2px 7px; }
            QComboBox, QSpinBox, QDoubleSpinBox, QLineEdit { min-height: 23px; }
            QSplitter::handle { background: #171c22; }
            QTableWidget { gridline-color: #323b44; }
        )"));
        auto *root = new QVBoxLayout(central);
        root->setContentsMargins(10, 4, 10, 8);
        root->setSpacing(6);
        workspace = new QTabWidget(central);
        workspace->setObjectName(QStringLiteral("workspaceTabs"));
        auto *equipmentButton = new QToolButton(workspace);
        equipmentButton->setObjectName(QStringLiteral("openEquipment"));
        equipmentButton->setText(QStringLiteral("太虚配装"));
        equipmentButton->setToolTip(QStringLiteral("查看魔盒装备、套装、技能、奇穴与秘籍资料"));
        workspace->setCornerWidget(equipmentButton, Qt::TopRightCorner);
        QObject::connect(equipmentButton, &QToolButton::clicked, window, [this] {
            if (!equipmentDialog) {
                equipmentDialog = new QDialog(window);
                equipmentDialog->setObjectName(QStringLiteral("equipmentDialog"));
                equipmentDialog->setWindowTitle(QStringLiteral("太虚剑意 · 魔盒配装"));
                equipmentDialog->resize(600, 800);
                auto *layout = new QVBoxLayout(equipmentDialog);
                auto *note = new QLabel(QStringLiteral("选择装备可预览配装；当前模拟仍使用主界面的属性、奇穴与秘籍配置。"), equipmentDialog);
                note->setWordWrap(true);
                layout->addWidget(note);
                auto *panel = new EquipmentPanel(equipmentDialog);
                panel->setMaximumWidth(QWIDGETSIZE_MAX);
                layout->addWidget(panel, 1);
            }
            equipmentDialog->show();
            equipmentDialog->raise();
            equipmentDialog->activateWindow();
        });
        root->addWidget(workspace, 1);
        auto *homePage = new QWidget;
        homePage->setObjectName(QStringLiteral("simulationPage"));
        auto *homeLayout = new QVBoxLayout(homePage);
        homeLayout->setContentsMargins(8, 8, 8, 8);
        auto *body = new QSplitter(Qt::Horizontal, homePage);
        body->setObjectName(QStringLiteral("bodySplitter"));
        body->setChildrenCollapsible(false);
        homeLayout->addWidget(body);
        workspace->addTab(homePage, QStringLiteral("模拟配置"));
        auto *configurationScroll = new QScrollArea(body);
        configurationScroll->setObjectName(QStringLiteral("configurationScroll"));
        configurationScroll->setFrameShape(QFrame::NoFrame);
        configurationScroll->setWidgetResizable(true);
        configurationScroll->setMinimumWidth(730);
        options = new SimulationOptions;
        configurationScroll->setWidget(options);
        options->setObjectName(QStringLiteral("simulationOptions"));
        options->setStyleSheet(QStringLiteral("QWidget { font-size: 13px; }"
                                             "QWidget#simulationOptions { background: #1d232a; }"
                                              "QComboBox, QSpinBox, QDoubleSpinBox, QLineEdit { min-height: 20px; }"
                                              "QLineEdit { placeholder-text-color: #94a5b0; }"));
        auto configurationFont = options->font();
        configurationFont.setPixelSize(13);
        options->setFont(configurationFont);
        options->setMinimumSize(710, 660);
        options->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        auto *homeEditor = new QWidget(body);
        homeEditor->setMinimumWidth(340);
        homeEditorLayout = new QVBoxLayout(homeEditor);
        homeEditorLayout->setContentsMargins(0, 0, 0, 0);
        body->setSizes({ 810, 510 });
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
        gainsAction  = action(QStringLiteral("计算收益"), "attributeGains", {}, [this] { StartBatch(false, true); });
        exportAction = action(QStringLiteral("导出战斗记录"), "exportTrace", {}, [this] { ExportTrace(); });
        exportAction->setEnabled(false);
        auto *clearBreakpoints =
            action(QStringLiteral("清除断点"), "clearBreakpoints", {}, [this] { editor->ClearAllBreakpoints(); });

        editorPage = new QGroupBox(QStringLiteral("循环宏"));
        editorPage->setObjectName(QStringLiteral("macroPanel"));
        homeEditorLayout->addWidget(editorPage);
        auto *editorLayout = new QVBoxLayout(editorPage);
        editorLayout->setContentsMargins(8, 16, 8, 8);
        editorLayout->setSpacing(5);
        auto *files = new QToolBar(editorPage);
        files->addActions({ newAction, openAction, save, check });
        auto *more = new QToolButton(files);
        more->setText(QStringLiteral("更多"));
        more->setPopupMode(QToolButton::InstantPopup);
        auto *menu = new QMenu(more);
        more->setMenu(menu);
        menu->addAction(saveAs);
        menu->addSeparator();
        files->addWidget(more);
        editorLayout->addWidget(files);
        documentLabel = new QLabel(editorPage);
        documentLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        documentLabel->setStyleSheet(QStringLiteral("color: #94a5b0; padding: 2px;"));
        editorLayout->addWidget(documentLabel);
        auto *macroEditor = new MacroEditor(editorPage);
        editor            = macroEditor;
        editor->setObjectName(QStringLiteral("macroEditor"));
        editor->setFont(Resources::MonoFont());
        editor->SetSyntaxType(CodeEditor::JX3Macro);
        search = new MacroSearchBar(editor, editorPage);
        editorLayout->addWidget(search);
        editorLayout->addWidget(editor, 1);
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
        helpDialog->setWindowTitle(QStringLiteral("宏语法与调试帮助"));
        helpDialog->resize(760, 620);
        auto *helpLayout = new QVBoxLayout(helpDialog);
        help             = new QTextBrowser(helpDialog);
        helpLayout->addWidget(help);
        auto *helpAction = action(QStringLiteral("宏语法帮助"), "macroHelp", {}, [this] {
            helpDialog->show();
            helpDialog->raise();
        });
        menu->addSeparator();
        menu->addAction(helpAction);
        problems = Table({ QStringLiteral("宏行"), QStringLiteral("说明") }, editorPage);
        problems->setObjectName(QStringLiteral("macroProblems"));
        problems->setMaximumHeight(135);
        problems->hide();
        editorLayout->addWidget(problems);
        macroStatus = new QLabel(QStringLiteral("点击行号设置断点 · Ctrl+Space 补全"), editorPage);
        macroStatus->setStyleSheet(QStringLiteral("color: #94a5b0;"));
        macroStatus->setWordWrap(true);
        editorLayout->addWidget(macroStatus);

        simToolbar = new SimulationToolbar(options);
        simToolbar->setObjectName(QStringLiteral("simulationToolbar"));
        options->RunActions()->addWidget(simToolbar);
        options->OutputLayout()->addWidget(new QLabel(QStringLiteral("平均秒伤 · 上次模拟")));
        meanDps = new QLabel(QStringLiteral("--"));
        meanDps->setObjectName(QStringLiteral("meanDps"));
        meanDps->setStyleSheet(QStringLiteral("font-size: 27px; font-weight: 600; color: #acd8c4;"));
        options->OutputLayout()->addWidget(meanDps);
        outputDetails = new QLabel(QStringLiteral("运行模拟后显示结果"));
        outputDetails->setObjectName(QStringLiteral("outputDetails"));
        outputDetails->setWordWrap(true);
        options->OutputLayout()->addWidget(outputDetails);
        auto *outputActions = new QHBoxLayout;
        auto  outputButton  = [&](const QString &label, const char *name, auto callback) {
            auto *button = new QPushButton(label);
            button->setObjectName(QString::fromLatin1(name));
            outputActions->addWidget(button);
            QObject::connect(button, &QPushButton::clicked, window, callback);
        };
        outputButton(QStringLiteral("统计"), "showStatistics", [this] { ShowAnalysis(results); });
        outputButton(QStringLiteral("时间轴"), "showTimeline", [this] { ShowAnalysis(views->widget(0)); });
        options->OutputLayout()->addLayout(outputActions);
        auto *recordButton = new QToolButton;
        recordButton->setDefaultAction(traceAction);
        recordButton->setToolTip(QStringLiteral("记录第一场战斗，查看时间轴、技能伤害和增益变化。"));
        outputActions->addWidget(recordButton);
        auto *gainTools  = new QHBoxLayout;
        auto *gainButton = new QToolButton;
        gainButton->setDefaultAction(gainsAction);
        gainTools->addWidget(gainButton);
        gainTools->addStretch();
        options->GainsLayout()->addLayout(gainTools);
        gainOverview = Table({ QStringLiteral("属性"), QStringLiteral("每点收益"), QStringLiteral("提升") }, options);
        gainOverview->setObjectName(QStringLiteral("gainOverview"));
        gainOverview->setMinimumHeight(184);
        gainOverview->horizontalHeader()->setStretchLastSection(false);
        gainOverview->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        gainOverview->horizontalHeader()->setMinimumSectionSize(40);
        gainOverview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        gainOverview->verticalHeader()->setMinimumSectionSize(20);
        gainOverview->verticalHeader()->setDefaultSectionSize(20);
        options->GainsLayout()->addWidget(gainOverview, 1);

        auto *analysisPage = new QWidget;
        analysisPage->setObjectName(QStringLiteral("analysisPage"));
        auto *analysisLayout = new QVBoxLayout(analysisPage);
        auto *analysisTools  = new QToolBar(analysisPage);
        analysisTools->addAction(traceAction);
        analysisTools->addAction(exportAction);
        analysisLayout->addWidget(analysisTools);
        views = new QTabWidget(analysisPage);
        views->setObjectName(QStringLiteral("analysisViews"));
        analysisLayout->addWidget(views, 1);
        workspace->addTab(analysisPage, QStringLiteral("结果分析"));
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

        auto *debugPage = new QWidget;
        debugPage->setObjectName(QStringLiteral("debugPage"));
        auto *debugLayout = new QVBoxLayout(debugPage);
        workspace->addTab(debugPage, QStringLiteral("宏调试"));
        debugToolbar = new DebugToolbar(window);
        debugToolbar->HideFloating();
        debugSession  = new DebugSession(window);
        debugCommands = new QToolBar(debugPage);
        debugCommands->setObjectName(QStringLiteral("debugCommands"));
        debugLayout->addWidget(debugCommands);
        auto debugAction = [&](const QString &text, const char *name, auto callback) {
            auto *item = debugCommands->addAction(text);
            item->setObjectName(QString::fromLatin1(name));
            QObject::connect(item, &QAction::triggered, window, callback);
        };
        debugAction(QStringLiteral("开始调试"), "debugStart", [this] { StartDebug(); });
        debugCommands->addSeparator();
        debugAction(QStringLiteral("继续 F5"), "debugContinue", [this] { ContinueDebug(); });
        debugAction(QStringLiteral("暂停 F6"), "debugPause", [this] { debugSession->Pause(); });
        debugAction(QStringLiteral("执行一行 F10"), "debugStepOver", [this] { Step(1); });
        debugAction(QStringLiteral("条件 / 动作 F11"), "debugStepInto", [this] { Step(0); });
        debugAction(QStringLiteral("执行本轮 Shift+F11"), "debugStepOut", [this] { Step(2); });
        debugCommands->addSeparator();
        debugAction(QStringLiteral("重新调试"), "debugRestart", [this] { StartDebug(); });
        debugAction(QStringLiteral("停止 Shift+F5"), "debugStop", [this] { debugSession->Stop(); });
        auto *debugBody = new QSplitter(Qt::Horizontal, debugPage);
        debugBody->setChildrenCollapsible(false);
        debugLayout->addWidget(debugBody, 1);
        auto *debugLeft = new QSplitter(Qt::Vertical, debugBody);
        debugLeft->setChildrenCollapsible(false);
        auto *debugEditor = new QWidget(debugLeft);
        debugEditorLayout = new QVBoxLayout(debugEditor);
        debugEditorLayout->setContentsMargins(0, 0, 0, 0);
        auto *traceGroup  = new QGroupBox(QStringLiteral("执行轨迹"), debugLeft);
        auto *traceLayout = new QVBoxLayout(traceGroup);
        macroSteps        = new MacroTracePanel(traceGroup);
        traceLayout->addWidget(macroSteps);
        debugLeft->setSizes({ 430, 310 });
        auto *debugState       = new QGroupBox(QStringLiteral("调试状态"), debugBody);
        auto *debugStateLayout = new QVBoxLayout(debugState);
        debugState->setMinimumWidth(340);
        auto *debugHint = new QLabel(QStringLiteral("点击行号设置断点。F11 查看条件，F10 执行一行，F5 继续。"), debugState);
        debugHint->setWordWrap(true);
        debugStateLayout->addWidget(debugHint);
        auto *debugTabs = new QTabWidget(debugState);
        debugTabs->setObjectName(QStringLiteral("debugTabs"));
        debugStateLayout->addWidget(debugTabs, 1);
        inspector = new QTextBrowser(debugTabs);
        inspector->setObjectName(QStringLiteral("debugInspector"));
        inspector->setPlaceholderText(
            QStringLiteral("点击“开始调试”，在首条宏前暂停。运行状态、技能冷却与增益将在这里显示。"));
        debugTabs->addTab(inspector, QStringLiteral("运行状态"));
        auto *playerPage   = new QWidget(debugTabs);
        auto *playerLayout = new QVBoxLayout(playerPage);
        statePanel         = new PlayerStatePanel(playerPage);
        statePanel->setMinimumHeight(300);
        playerLayout->addWidget(statePanel);
        playerLayout->addStretch();
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
        debugBody->setSizes({ 900, 410 });
        debugBody->setStretchFactor(0, 3);
        debugBody->setStretchFactor(1, 1);
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
            workspace->setCurrentIndex(2);
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
        QObject::connect(workspace, &QTabWidget::currentChanged, window, [this](int index) { SwitchWorkspace(index); });

        result = new QLabel(QStringLiteral("准备就绪。设置参数与宏后，点击“开始模拟”。"), central);
        result->setObjectName(QStringLiteral("simulationResult"));
        result->setWordWrap(false);
        result->setFixedHeight(26);
        result->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        result->setTextInteractionFlags(Qt::TextSelectableByMouse);
        result->setStyleSheet(QStringLiteral("color: #a8bdc6; padding: 3px;"));
        root->addWidget(result);
        auto *status = new QHBoxLayout;
        progress     = new QProgressBar(central);
        progress->setObjectName(QStringLiteral("batchProgress"));
        progress->setRange(0, 1000);
        progress->setValue(0);
        progress->hide();
        cancel = new QPushButton(QStringLiteral("取消模拟"), central);
        cancel->setObjectName(QStringLiteral("cancelBatch"));
        cancel->setEnabled(false);
        cancel->hide();
        status->addWidget(progress, 1);
        status->addWidget(cancel);
        root->addLayout(status);
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
                workspace->setCurrentIndex(2);
            } else if (workspace->currentIndex() == 2) {
                workspace->setCurrentIndex(0);
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
            auto *item = new QShortcut(key, window);
            QObject::connect(item, &QShortcut::activated, window, callback);
        };
        shortcut(QKeySequence(Qt::Key_F5), [this] {
            if (simToolbar->IsDebugModeEnabled() || debugSession->GetState() == DebugSession::Paused ||
                debugSession->GetState() == DebugSession::Running)
            {
                workspace->setCurrentIndex(2);
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
            workspace->setCurrentIndex(0);
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
            gainOverview->setRowCount(0);
            result->setText(QStringLiteral("已切换至%1，修改配置后可重新模拟。").arg(desktop::ClassName(documentClass)));
            result->setToolTip({});
            UpdateTitle();
            QMetaObject::invokeMethod(editor, "CheckSyntax", Qt::DirectConnection);
        });
        QSettings settings;
        for (int i = 0; i < 2; ++i) {
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
        filePath = settings.value(QStringLiteral("macro/file")).toString();
        editor->setPlainText(settings.value(QStringLiteral("macro/draft"), desktop::DefaultMacro(documentClass)).toString());
        editor->document()->setModified(settings.value(QStringLiteral("macro/modified"), false).toBool());
        for (const auto &value : settings.value(QStringLiteral("macro/breakpoints")).toList()) {
            const int line = value.toInt();
            if (line > 0 && line <= editor->blockCount()) {
                editor->SetBreakpoint(line, true);
            }
        }
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

    void SwitchWorkspace(int index)
    {
        // Keep one editor/document so undo, breakpoints and drafts survive page changes.
        if (index == 0) {
            homeEditorLayout->addWidget(editorPage);
        }
        if (index == 2) {
            debugEditorLayout->addWidget(editorPage);
        }
        editorPage->show();
        simToolbar->findChild<QToolButton *>(QStringLiteral("debugButton"))->setChecked(index == 2);
        UpdateControls();
    }

    void ShowAnalysis(QWidget *page)
    {
        views->setCurrentWidget(page);
        workspace->setCurrentIndex(1);
    }

    void ShowEditor()
    {
        if (workspace->currentIndex() == 1) {
            workspace->setCurrentIndex(0);
        }
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
        gainOverview->setRowCount(stats.gains.size());
        for (int row = 0; row < stats.gains.size(); ++row) {
            const auto       &gain  = stats.gains[row];
            const auto        delta = gain.stats.mean_damage / stats.FightSeconds() - stats.MeanDps();
            const QStringList cells{
                gain.attribute,
                QString::number(delta / gain.increment, 'f', 2),
                stats.MeanDps() ? QString::number(delta * 100 / stats.MeanDps(), 'f', 2) + '%' : QStringLiteral("--")
            };
            for (int column = 0; column < cells.size(); ++column) {
                auto *item = new QTableWidgetItem(cells[column]);
                if (column) {
                    item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                }
                gainOverview->setItem(row, column, item);
            }
        }
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
        window->setWindowTitle(QStringLiteral("JX3DPS · %1 · %2").arg(desktop::ClassName(documentClass), name));
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
            ->setEnabled(!batch && (!debugging || workspace->currentIndex() == 2));
        traceAction->setEnabled(!batch && !debugging);
        gainsAction->setEnabled(!batch && !debugging);
        cancel->setEnabled(batch && !cancelling);
        cancel->setVisible(batch);
        progress->setVisible(batch || debugging);
        exportAction->setEnabled(!events.isEmpty());
        debugCommands->setVisible(simToolbar->IsDebugModeEnabled());
        debugCommands->setEnabled(!batch);
        window->findChild<QAction *>(QStringLiteral("debugStart"))->setVisible(state == DebugSession::Stopped);
        window->findChild<QAction *>(QStringLiteral("debugRestart"))->setVisible(state != DebugSession::Stopped);
        window->findChild<QAction *>(QStringLiteral("debugContinue"))->setEnabled(!batch && state == DebugSession::Paused);
        window->findChild<QAction *>(QStringLiteral("debugPause"))->setEnabled(!batch && state == DebugSession::Running);
        for (const auto *name : { "debugStepOver", "debugStepInto", "debugStepOut" }) {
            window->findChild<QAction *>(QString::fromLatin1(name))->setEnabled(!batch && state == DebugSession::Paused);
        }
        window->findChild<QAction *>(QStringLiteral("debugStop"))->setEnabled(!batch && state != DebugSession::Stopped);
        debugToolbar->SetDebugState(state == DebugSession::Running ? DebugToolbar::Running
                                    : debugging                    ? DebugToolbar::Paused
                                                                   : DebugToolbar::Stopped);
        debugToolbar->HideFloating();
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
            if (workspace->currentIndex() == 1) {
                workspace->setCurrentIndex(0);
            }
            problems->show();
            result->setText(QStringLiteral("宏有 %1 处错误；点击诊断可定位对应行。").arg(problems->rowCount()));
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

    void StartBatch(bool trace, bool gains = false)
    {
        if (batch || debugSession->GetState() == DebugSession::Running || debugSession->GetState() == DebugSession::Paused)
        {
            return;
        }
        if (!CheckMacro()) {
            return;
        }
        try {
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
            batch        = new BatchWorker(std::move(compiled.first.program), run, config, window, trace, gains);
            auto *worker = batch.data();
            QObject::connect(worker,
                             &BatchWorker::StatisticsReady,
                             window,
                             [this, source = editor->toPlainText(), snapshot, trace](const SimulationResult &stats) {
                                 results->AddResult(stats, source, snapshot);
                                 UpdateOutput(stats);
                                 if (!trace) {
                                     views->setCurrentWidget(results);
                                 }
                             });
            QObject::connect(worker, &BatchWorker::ResultReady, window, [this](const QString &text) {
                progressTimer->stop();
                progress->setValue(static_cast<int>(batch->Completed() * 1000 / batch->Requested()));
                result->setText(QStringLiteral("%1 | 平均秒伤 %2 | 详细结果见“统计”")
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
            activeOptions = options->Batch();
            debugSession->Stop();
            ClearEvents();
            debugSession->SetSimulationOptions(options->ActiveConfig(), activeOptions.duration, activeOptions.seed);
            progress->setValue(0);
            progress->setFormat(QStringLiteral("调试 %p%"));
            workspace->setCurrentIndex(2);
            macroSteps->Reset(editor->toPlainText());
            debugSession->Start(editor->toPlainText());
            workspace->setCurrentIndex(2);
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
    resize(1400, 900);
    setMinimumSize(1120, 720);
    d = std::make_unique<Impl>(this);
    QSettings settings;
    if (settings.contains(QStringLiteral("window/geometry"))) {
        restoreGeometry(settings.value(QStringLiteral("window/geometry")).toByteArray());
    }
}

Workbench::~Workbench() = default;

void Workbench::closeEvent(QCloseEvent *event)
{
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
