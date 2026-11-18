#include "simulation_options.h"

#include <algorithm>
#include <cmath>

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpressionValidator>
#include <QSaveFile>
#include <QScrollArea>
#include <QSettings>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QThread>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "diagnostics.h"
#include "runtime_backend.h"
#include "game_icons.h"
#include "game_attributes.h"
#include "talent_panel.h"
#include "flow_macro.h"
#include "flow_timeline_editor.h"
#include "controls/code_editor/code_editor.h"
#include "controls/equipment_panel/equipment_panel.h"
#include "controls/theme/segmented_tabs.h"
#include "controls/theme/header_group_box.h"
#include "controls/theme/fixed_unit_spin_box.h"

namespace mw = JX3DPS::runtime::mo_wen;
namespace tx = JX3DPS::runtime::tai_xu;
using namespace JX3DPS;

namespace {
class ConfigurationRows final : public QFormLayout {
public:
    explicit ConfigurationRows(QWidget *parent) : QFormLayout(parent) {}
    void SetExpanded(bool expanded) { expanded_ = expanded; invalidate(); }
    void SetColumns(int columns) { columns_ = columns; invalidate(); }
    void SetCompactFieldWidth(int width) { compactFieldWidth_ = width; invalidate(); }
    bool HasCompactFields() const { return compactFieldWidth_ > 0; }
    QSize minimumSize() const override { return columns_ == 1 ? QFormLayout::minimumSize() : GridSize(); }
    QSize sizeHint() const override { return columns_ == 1 ? QFormLayout::sizeHint() : GridSize(); }
    void setGeometry(const QRect &rect) override {
        if (columns_ > 1) {
            QLayout::setGeometry(rect);
            const auto margins = contentsMargins();
            const auto area = rect.marginsRemoved(margins);
            constexpr int gutter = 18;
            const int cellWidth = (area.width() - gutter * (columns_ - 1)) / columns_;
            const int rowHeight = RowHeight();
            int y = area.top(), cell = 0;
            for (int row = 0; row < rowCount(); ++row) {
                auto *field = Field(row);
                if (!field || (field->widget() && field->widget()->isHidden())) continue;
                auto *label = itemAt(row, LabelRole);
                if (!label) {
                    if (cell) { y += rowHeight + verticalSpacing(); cell = 0; }
                    field->setGeometry(QRect(area.left(), y, area.width(), field->sizeHint().height()));
                    y += field->sizeHint().height() + verticalSpacing();
                    continue;
                }
                const int x = area.left() + cell * (cellWidth + gutter);
                const int labelWidth = label->sizeHint().width();
                label->setGeometry(QRect(x, y, labelWidth, rowHeight));
                const int fieldWidth = qMin(expanded_ ? 200 : cellWidth, cellWidth - labelWidth - horizontalSpacing());
                field->setGeometry(QRect(x + cellWidth - fieldWidth, y, fieldWidth, rowHeight));
                if (++cell == columns_) { cell = 0; y += rowHeight + verticalSpacing(); }
            }
            return;
        }
        if (!expanded_ && compactFieldWidth_ > 0) {
            int labelWidth = 0, fieldWidth = compactFieldWidth_;
            for (int row = 0; row < rowCount(); ++row) {
                if (!isRowVisible(row)) continue;
                const auto *label = itemAt(row, LabelRole);
                const auto *field = itemAt(row, FieldRole);
                if (!label || !field) continue;
                labelWidth = qMax(labelWidth, label->sizeHint().width());
                fieldWidth = qMax(fieldWidth, field->minimumSize().width());
            }
            const auto margins = contentsMargins();
            const int width = qMin(rect.width(), margins.left() + labelWidth + horizontalSpacing() + fieldWidth + margins.right());
            // Center the label/input pairs together, rather than separating them across the card.
            QFormLayout::setGeometry(QRect(rect.x() + (rect.width() - width) / 2, rect.y(), width, rect.height()));
            return;
        }
        QFormLayout::setGeometry(rect);
        if (!expanded_) return;
        // Right-align fields without inflating the form's minimum width on resize.
        for (int row = 0; row < rowCount(); ++row) {
            auto *field = itemAt(row, QFormLayout::FieldRole);
            if (!field || !itemAt(row, QFormLayout::LabelRole)) continue;
            auto bounds = field->geometry();
            bounds.setLeft(qMax(bounds.left(), bounds.right() - 199));
            field->setGeometry(bounds);
        }
    }
private:
    QLayoutItem *Field(int row) const {
        auto *field = itemAt(row, FieldRole);
        return field ? field : itemAt(row, SpanningRole);
    }
    int RowHeight() const {
        int height = 0;
        for (int row = 0; row < rowCount(); ++row) {
            const auto *field = Field(row);
            if (field && itemAt(row, LabelRole) && !(field->widget() && field->widget()->isHidden()))
                height = qMax(height, field->sizeHint().height());
        }
        return height;
    }
    QSize GridSize() const {
        int fields = 0, height = 0, pairWidth = 0, spanWidth = 0;
        for (int row = 0; row < rowCount(); ++row) {
            const auto *field = Field(row);
            if (!field || (field->widget() && field->widget()->isHidden())) continue;
            const auto *label = itemAt(row, LabelRole);
            if (label) {
                ++fields;
                pairWidth = qMax(pairWidth, label->minimumSize().width() + horizontalSpacing() + field->minimumSize().width());
            } else {
                height += field->sizeHint().height() + verticalSpacing();
                spanWidth = qMax(spanWidth, field->minimumSize().width());
            }
        }
        const int lines = (fields + columns_ - 1) / columns_;
        const auto margins = contentsMargins();
        height += lines * RowHeight() + qMax(0, lines - 1) * verticalSpacing();
        return {qMax(spanWidth, columns_ * pairWidth + (columns_ - 1) * 18) + margins.left() + margins.right(),
                height + margins.top() + margins.bottom()};
    }
    int columns_ = 1;
    int compactFieldWidth_ = 0;
    bool expanded_ = false;
};

class ConfigurationForm final : public QWidget {
public:
    explicit ConfigurationForm(QWidget *parent = nullptr) : QWidget(parent), form(new ConfigurationRows(this)) {
        setProperty("configurationForm", true);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        form->setHorizontalSpacing(6);
        SetExpanded(false);
    }
    void SetExpanded(bool expanded) {
        expanded_ = expanded;
        const int horizontalMargin = expanded ? 18 : form->HasCompactFields() ? 4 : 10;
        const int verticalMargin = expanded ? 10 : form->HasCompactFields() ? 2 : 4;
        form->setContentsMargins(horizontalMargin, verticalMargin, horizontalMargin, verticalMargin);
        form->setVerticalSpacing(expanded ? 14 : form->HasCompactFields() ? 2 : 3);
        form->SetExpanded(expanded);
        update();
    }
    ConfigurationRows *form;
protected:
    void paintEvent(QPaintEvent *event) override {
        QWidget::paintEvent(event);
        if (!expanded_) return;
        QPainter painter(this);
        painter.setPen(QColor("#2c2c32"));
        for (int row = 0; row < form->rowCount() - 1; ++row) {
            const auto *field = form->itemAt(row, QFormLayout::FieldRole);
            if (!field || (field->widget() && !field->widget()->isVisible())) continue;
            const int y = field->geometry().bottom() + 11;
            painter.drawLine(18, y, width() - 18, y);
        }
    }
private:
    bool expanded_ = false;
};

QSpinBox *Integer(QFormLayout *form, const char *key, const QString &label, int value, int minimum, int maximum)
{
    auto *input = new QSpinBox;
    input->setObjectName(QString::fromLatin1(key));
    input->setRange(minimum, maximum);
    input->setValue(value);
    input->setGroupSeparatorShown(true);
    input->setProperty("defaultValue", value);
    form->addRow(label, input);
    return input;
}

template<typename Input = QDoubleSpinBox>
Input *Decimal(QFormLayout *form, const char *key, const QString &label, double value, double minimum, double maximum, int decimals = 2)
{
    auto *input = new Input;
    input->setObjectName(QString::fromLatin1(key));
    input->setDecimals(decimals);
    input->setRange(minimum, maximum);
    input->setValue(value);
    input->setProperty("defaultValue", value);
    form->addRow(label, input);
    return input;
}
} // namespace

SimulationOptions::SimulationOptions(QWidget *parent) : QWidget(parent)
{
    Q_INIT_RESOURCE(desktop_data);
    auto *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    auto group = [&](const QString &title, const char *name, int row, int column, int rowSpan = 1, int columnSpan = 1) {
        auto *box = new desktop::HeaderGroupBox(title, this);
        box->setObjectName(QString::fromLatin1(name));
        auto *content = new QVBoxLayout(box);
        content->setContentsMargins(10, 6, 10, 8);
        content->setSpacing(4);
        layout->addWidget(box, row, column, rowSpan, columnSpan);
        return content;
    };
    auto *profileLayout  = group(QStringLiteral("配置方案"), "configurationSchemes", 0, 0, 1, 2);
    auto *settingsLayout = group(QStringLiteral("模拟设置"), "runSettings", 1, 0);
    profileLayout->parentWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    profileLayout->setAlignment(Qt::AlignTop);
    m_output              = group(QStringLiteral("模拟结果"), "outputPanel", 3, 0, 1, 2);
    m_output->parentWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    auto *permanentLayout = group(QStringLiteral("常驻增益"), "permanentPanel", 1, 1);
    for (auto *box : {settingsLayout->parentWidget(), permanentLayout->parentWidget()}) {
        box->setStyleSheet(QStringLiteral("QGroupBox#%1 { margin-top: %2px; } QGroupBox#%1::title { top: 5px; }")
            .arg(box->objectName()).arg(desktop::AttributeHeaderHeight + desktop::AttributeHeaderSpacing));
    }
    auto *permanentHeader = static_cast<desktop::HeaderGroupBox *>(permanentLayout->parentWidget())->HeaderActions(desktop::AttributeHeaderHeight);
    auto *panelMode = new desktop::SegmentedTabBar;
    panelMode->SetSegmentHeight(desktop::AttributeHeaderHeight);
    panelMode->setUsesScrollButtons(false);
    m_permanentsInPanel = panelMode;
    m_permanentsInPanel->setObjectName(QStringLiteral("permanentsInPanel"));
    m_permanentsInPanel->setAccessibleName(QStringLiteral("常驻增益面板显示方式"));
    m_permanentsInPanel->addTab(QStringLiteral("计入面板"));
    m_permanentsInPanel->addTab(QStringLiteral("基础面板"));
    m_permanentsInPanel->setCurrentIndex(QSettings().value(QStringLiteral("interface/permanentsInPanel"), true).toBool() ? 0 : 1);
    const auto panelHint = QStringLiteral("计入面板：显示包含常驻增益的总属性，数值前的括号标出净变化（绿色增加、红色减少）。\n"
        "基础面板：仅显示未计入常驻增益的属性。\n此切换只影响面板展示，所选常驻增益始终参与模拟；收益也仍按实际模拟配置计算。");
    m_permanentsInPanel->setToolTip(panelHint);
    m_permanentsInPanel->setTabToolTip(0, panelHint);
    m_permanentsInPanel->setTabToolTip(1, panelHint);
    permanentHeader->addWidget(m_permanentsInPanel);
    auto *settingsRow = new QWidget(this);
    settingsRow->setObjectName(QStringLiteral("simulationSettingsRow"));
    auto *settingsRowLayout = new QHBoxLayout(settingsRow);
    settingsRowLayout->setContentsMargins(0, 0, 0, 0);
    settingsRowLayout->setSpacing(4);
    settingsRow->hide();
    settingsLayout->parentWidget()->setMaximumHeight(245);
    permanentLayout->parentWidget()->setMaximumHeight(245);
    m_inputPanels         = { profileLayout->parentWidget() };
    m_runActions = new QVBoxLayout;
    m_runActions->setContentsMargins(0, 0, 0, 0);
    SetupProfiles(profileLayout);
    m_specialization = new desktop::IconComboBox(QStringLiteral("心法"), this, false);
    m_specialization->setObjectName(QStringLiteral("specialization"));
    m_specialization->addItem(desktop::GameIcon(desktop::Specialization::TaiXu, QStringLiteral("太虚剑意")), QStringLiteral("太虚剑意 · 纯阳"));
    m_specialization->addItem(desktop::GameIcon(desktop::Specialization::MoWen, QStringLiteral("莫问")), QStringLiteral("莫问 · 长歌"));
    for (int index = 0; index < m_specialization->count(); ++index)
        m_specialization->setItemData(index, m_specialization->itemText(index) + QStringLiteral("\n点击图标切换心法"), Qt::ToolTipRole);
    m_specialization->setToolTip(QStringLiteral("心法：太虚剑意 · 纯阳\n点击图标切换心法"));
    auto *specializationRow = new QHBoxLayout;
    specializationRow->setSpacing(8);
    specializationRow->addWidget(m_specialization);
    auto *specializationName = new QLabel(m_specialization->currentText());
    specializationName->setObjectName(QStringLiteral("selectedSpecializationName"));
    specializationRow->addWidget(specializationName, 1);
    connect(m_specialization, &QComboBox::currentTextChanged, specializationName, &QLabel::setText);
    settingsLayout->addLayout(specializationRow);
    m_inputPanels.push_back(m_specialization);
    auto *runContent = new ConfigurationForm;
    runContent->setObjectName(QStringLiteral("runParameters"));
    auto *run = runContent->form;
    run->SetCompactFieldWidth(136);
    settingsLayout->addWidget(runContent, 1);
    m_inputPanels.push_back(runContent);
    auto *flowLayout = group(QStringLiteral("流程宏"), "flowMacroPanel", 2, 2);
    flowLayout->parentWidget()->setStyleSheet(QStringLiteral("QGroupBox#flowMacroPanel { margin-top: 32px; } QGroupBox#flowMacroPanel::title { top: 5px; }"));
    m_flowValues = desktop::FlowDefaults();
    m_flowEditor = new CodeEditor;
    m_flowEditor->setObjectName(QStringLiteral("flowMacroEditor"));
    m_flowEditor->setAccessibleName(QStringLiteral("流程宏脚本"));
    m_flowEditor->SetSyntaxType(CodeEditor::JX3Macro);
    m_flowEditor->SetShowLineNumbers(false);
    m_flowEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_flowEditor->SetSyntaxValidator([](const QString &source) {
        QList<SyntaxError> errors;
        for (const auto &error : desktop::CompileFlowMacro(source).errors)
            errors.append({error.line, error.column, error.length, SyntaxError::Error, error.message});
        return errors;
    });
    m_flowTimeline = new desktop::FlowTimelineEditor(m_flowEditor);
    flowLayout->addWidget(m_flowTimeline, 1);
    m_flowStatus = new QLabel;
    m_flowStatus->setObjectName(QStringLiteral("flowMacroStatus"));
    m_flowStatus->setTextFormat(Qt::PlainText);
    m_flowStatus->setFixedHeight(30);
    m_flowStatus->setWordWrap(true);
    flowLayout->addWidget(m_flowStatus);
    m_flowTimeline->statusChanged = [this](const QString &text, bool error) {
        m_flowStatus->setText(text);
        m_flowStatus->setToolTip(text);
        m_flowStatus->setStyleSheet(QStringLiteral("font-size: 11px; color: %1;").arg(error ? "#f08078" : "#92929d"));
    };
    auto *flowHelp = new QDialog(this);
    flowHelp->setObjectName(QStringLiteral("flowMacroHelpDialog"));
    flowHelp->setWindowTitle(QStringLiteral("流程宏语法 · 时间轴"));
    flowHelp->resize(650, 500);
    auto *flowHelpLayout = new QVBoxLayout(flowHelp);
    auto *flowHelpText = new QTextBrowser;
    flowHelpText->setHtml(QStringLiteral("<h2>流程宏与时间轴</h2><p>无时间前缀或 0 秒节点用于每场开战前初始化，随后运行右侧循环宏。"
        "省略的参数采用默认值，分行重复赋值以后面的值为准。支持 # 和 // 注释。</p><pre>%1</pre>"
        "<p><b>/set_target</b>：level 等级、shield 防御、life 气血、distance 距离、vulnerable 易伤。"
        "life 和 vulnerable 建议带百分号；不带百分号时按比例解释，例如 life=0.5 等于 life=50%。</p>"
        "<p><b>/set_player</b>：qidian 太虚初始气点；shadows 莫问初始影子；style 可写阳春白雪/高山流水或 0/1。"
        "各心法仅使用对应的初始资源；太虚沿用旧规则的开场 +1 气点，上限 10。</p><p>一行可写多个 参数=值；等号两侧不要留空格。"
        "可使用 00:00 或 00:00.00 前缀表示开场。当前只有一个目标，id 仅接受 1。</p>"
        "<p><b>战中节点仅支持目标气血和距离：</b></p><pre>00:30 /set_target life=50%\n00:45.0625 /set_target distance=3</pre>"
        "<p>时间格式为 分:秒，最多四位小数，精度 1/16 秒。节点按时间执行，相同时间按源码顺序执行。"
        "等级、防御、易伤和角色初始资源只能在 0 秒设置。距离沿用技能现有的判距规则，不自动取消读条或持续伤害。</p>"
        "<p>时间轴支持双击添加/编辑、拖动调整时间、滚轮缩放和空白处拖动平移；修改与脚本同步，可撤销。"
        "最多 256 个节点，时间不超过 24 小时。战斗区间不包含结束帧，位于结束处或之后的节点标灰且不会执行。</p>"
        "<p><b>尚不支持多目标、切换目标、等待或停止指令。</b>这些内容会报错并阻止模拟，不会被忽略。"
        "流程宏不设置技能断点，技能循环与断点调试仍在循环宏中。</p>")
        .arg(desktop::FlowMacroFromValues(m_flowValues).toHtmlEscaped()));
    flowHelpLayout->addWidget(flowHelpText);
    auto *help = new QToolButton;
    help->setObjectName(QStringLiteral("openFlowMacroHelp"));
    help->setText(QStringLiteral("语法…"));
    auto *flowActions = static_cast<desktop::HeaderGroupBox *>(flowLayout->parentWidget())->HeaderActions(desktop::AttributeHeaderHeight);
    auto *expandFlow = new QToolButton;
    expandFlow->setObjectName(QStringLiteral("openFlowTimeline"));
    expandFlow->setText(QStringLiteral("展开…"));
    flowActions->addWidget(expandFlow);
    flowActions->addWidget(help);
    connect(expandFlow, &QToolButton::clicked, m_flowTimeline, &desktop::FlowTimelineEditor::OpenEditor);
    connect(help, &QToolButton::clicked, flowHelp, [flowHelp] { flowHelp->show(); flowHelp->raise(); flowHelp->activateWindow(); });
    auto *tabs = new desktop::SegmentedTabs(this);
    tabs->setObjectName(QStringLiteral("configurationTabs"));
    static_cast<desktop::SegmentedTabBar *>(tabs->tabBar())->SetSegmentHeight(desktop::AttributeHeaderHeight);
    tabs->setStyleSheet(QStringLiteral("QTabWidget#configurationTabs::pane { top: %1px; }")
        .arg(desktop::AttributeHeaderSpacing));
    m_inputPanels.push_back(tabs);
    auto page = [&](QTabWidget *owner, const QString &title) {
        auto *content = new ConfigurationForm;
        auto *form = content->form;
        auto *scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidget(content);
        owner->addTab(scroll, title);
        return form;
    };
    m_attributeEditor = new QDialog(this);
    m_attributeEditor->setObjectName(QStringLiteral("attributeValueEditor"));
    m_attributeEditor->setWindowTitle(QStringLiteral("属性输入 · 旧规则"));
    m_attributeEditor->setWindowModality(Qt::WindowModal);
    m_attributeEditor->resize(540, 510);
    auto *attributeEditorLayout = new QVBoxLayout(m_attributeEditor);
    auto *attributeNote = new QLabel(QStringLiteral("这里编辑等级、基础数值和额外加成，主面板显示折算后的结果。修改即时生效。"));
    attributeNote->setWordWrap(true);
    attributeEditorLayout->addWidget(attributeNote);
    auto *editorTabs = new desktop::SegmentedTabs(m_attributeEditor);
    editorTabs->setObjectName(QStringLiteral("attributeEditorTabs"));
    attributeEditorLayout->addWidget(editorTabs, 1);
    auto *attributes = page(editorTabs, QStringLiteral("数值"));
    auto *bonuses = page(editorTabs, QStringLiteral("加成"));
    attributes->SetColumns(1);
    bonuses->SetColumns(1);
    editorTabs->widget(1)->setObjectName(QStringLiteral("bonusesScroll"));
    m_inputPanels.push_back(attributes->parentWidget());
    m_inputPanels.push_back(bonuses->parentWidget());
    auto *attributeButtons = new QDialogButtonBox(QDialogButtonBox::Close);
    attributeButtons->button(QDialogButtonBox::Close)->setText(QStringLiteral("关闭"));
    attributeEditorLayout->addWidget(attributeButtons);
    connect(attributeButtons, &QDialogButtonBox::rejected, m_attributeEditor, &QDialog::close);
    auto *attributeScroll = new QScrollArea(this);
    attributeScroll->setObjectName(QStringLiteral("attributesScroll"));
    attributeScroll->setWidgetResizable(true);
    attributeScroll->setFrameShape(QFrame::NoFrame);
    m_attributePane = attributeScroll;
    m_gameAttributes = new desktop::GameAttributePanel;
    attributeScroll->setWidget(m_gameAttributes);
    m_gameGains = new desktop::AttributeGainsPanel(this);
    m_inputPanels.push_back(m_gameGains->calculate);
    auto highlightAttribute = [this](int index) { m_gameAttributes->HighlightRow(index); m_gameGains->HighlightRow(index); };
    m_gameAttributes->hoverChanged = highlightAttribute;
    m_gameGains->hoverChanged = highlightAttribute;
    m_inputPanels.push_back(m_gameAttributes->edit);
    connect(m_gameAttributes->edit, &QPushButton::clicked, this, [this] { m_attributeEditor->show(); m_attributeEditor->raise(); });
    m_equipmentDialog = new QDialog(this);
    m_equipmentDialog->setObjectName(QStringLiteral("equipmentDialog"));
    m_equipmentDialog->resize(720, 680);
    m_equipmentDialog->setModal(false);
    auto *equipmentDialogLayout = new QVBoxLayout(m_equipmentDialog);
    auto *equipmentNote = new QLabel(QStringLiteral("配装预览不会自动覆盖模拟属性；装备特效用于下一次模拟。关闭窗口后保留本次选择。"));
    equipmentNote->setWordWrap(true);
    equipmentDialogLayout->addWidget(equipmentNote);
    connect(m_gameAttributes->equipment, &QToolButton::clicked, this, &SimulationOptions::ShowEquipment);
    auto *equipmentPage = new QWidget;
    equipmentPage->setObjectName(QStringLiteral("equipmentPage"));
    auto *equipmentLayout = new QVBoxLayout(equipmentPage);
    equipmentLayout->setContentsMargins(7, 7, 7, 7);
    equipmentLayout->setSpacing(6);
    equipmentLayout->setAlignment(Qt::AlignTop);
    auto *equipmentPanel = new EquipmentPanel(equipmentPage);
    equipmentLayout->addWidget(equipmentPanel);
    auto equipmentEffects = [&] {
        auto *content = new QWidget(equipmentPage);
        content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
        auto *form = new QFormLayout(content);
        form->setContentsMargins(0, 0, 0, 0);
        form->setVerticalSpacing(6);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        equipmentLayout->addWidget(content);
        m_inputPanels.push_back(content);
        return form;
    };
    auto *equipmentScroll = new QScrollArea;
    equipmentScroll->setObjectName(QStringLiteral("equipmentScroll"));
    equipmentScroll->setWidgetResizable(true);
    equipmentScroll->setFrameShape(QFrame::NoFrame);
    equipmentScroll->setWidget(equipmentPage);
    equipmentDialogLayout->addWidget(equipmentScroll, 1);
    auto *equipmentButtons = new QDialogButtonBox(QDialogButtonBox::Close);
    equipmentButtons->button(QDialogButtonBox::Close)->setText(QStringLiteral("关闭"));
    equipmentDialogLayout->addWidget(equipmentButtons);
    connect(equipmentButtons, &QDialogButtonBox::rejected, m_equipmentDialog, &QDialog::close);
    auto *talentStack = new QStackedWidget;
    auto *talentHeaders = new QStackedWidget;
    talentHeaders->setObjectName(QStringLiteral("talentHeaders"));
    talentHeaders->setFixedHeight(desktop::AttributeHeaderHeight);
    tabs->setCornerWidget(talentHeaders, Qt::TopRightCorner);
    connect(tabs, &QTabWidget::currentChanged, talentHeaders, [talentHeaders](int index) { talentHeaders->setVisible(index == 0); });
    connect(tabs, &QTabWidget::currentChanged, this, [this] { UpdateTalentHeight(); });
    auto *recipeStack = new QStackedWidget;
    tabs->addTab(talentStack, QStringLiteral("奇穴"));
    tabs->addTab(recipeStack, QStringLiteral("秘籍"));
    auto effectPage = [](QStackedWidget *owner) {
        auto *content = new QWidget;
        auto *form    = new QFormLayout(content);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        auto *scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidget(content);
        owner->addWidget(scroll);
        return form;
    };
    auto talentPage = [&](desktop::Specialization specialization) {
        auto *panel = new desktop::TalentPanel(specialization);
        panel->setObjectName(QStringLiteral("talents%1").arg(static_cast<int>(specialization)));
        m_talentPanels[static_cast<int>(specialization)] = panel;
        panel->SetColumns(9);
        talentHeaders->addWidget(panel->TakeHeader());
        connect(panel->HeaderWidget()->findChild<QToolButton *>(QStringLiteral("legacyTalentToggle")),
            &QToolButton::toggled, this, [this] { UpdateTalentHeight(); });
        auto *scroll = new QScrollArea;
        scroll->setObjectName(QStringLiteral("talentsScroll%1").arg(static_cast<int>(specialization)));
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidget(panel);
        talentStack->addWidget(scroll);
        return panel;
    };
    auto *txTalents   = talentPage(desktop::Specialization::TaiXu);
    auto *txRecipes   = effectPage(recipeStack);
    auto *txEquipment = equipmentEffects();
    m_iterations      = Integer(run, "iterations", QStringLiteral("模拟次数"), 1000, 1, 1000000000);
    m_workers = Integer(run, "workers", QStringLiteral("工作线程"), std::max(1, QThread::idealThreadCount() / 2), 1, 256);
    auto *seconds = Decimal<desktop::FixedUnitDoubleSpinBox>(run, "seconds", QStringLiteral("战斗时长"), 300, 0.0625, 86400, 4);
    seconds->SetUnit(QStringLiteral("秒"));
    seconds->setAccessibleDescription(QStringLiteral("单位：秒"));
    m_seconds = seconds;
    m_seconds->setSingleStep(0.0625);
    connect(m_seconds, &QDoubleSpinBox::editingFinished, this, [this] {
        m_seconds->setValue(std::llround(m_seconds->value() * 16.0) / 16.0);
    });
    m_seed = new QLineEdit(QStringLiteral("0"));
    m_seed->setObjectName(QStringLiteral("seed"));
    m_seed->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[0-9]{1,20}")), m_seed));
    auto seedHint = [this] {
        m_seed->setToolTip(QStringLiteral("当前种子：%1\n相同种子可复现相同战斗；调试和单次记录复现其中第一场。")
            .arg(m_seed->text()));
    };
    connect(m_seed, &QLineEdit::textChanged, this, seedHint);
    seedHint();
    run->addRow(QStringLiteral("随机种子"), m_seed);
    Integer(run, "delay", QStringLiteral("输入延迟"), 0, 0, 160)->setSuffix(QStringLiteral(" 帧"));
    // Native range-based size hints must not widen this small, non-scrolling form.
    for (int row = 0; row < run->rowCount(); ++row)
        run->itemAt(row, QFormLayout::FieldRole)->widget()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    m_bindings.push_back([](tx::Config &config, const QJsonObject &values) {
        config.initial_qidian  = values.value("qidian").toInt();
        config.delay           = values.value("delay").toInt();
        config.target.level    = values.value("targetLevel").toInt();
        config.target.shield   = values.value("shield").toInt();
        config.target.life     = values.value("targetLife").toDouble() / 100.0;
        config.target.distance = values.value("distance").toDouble();
    });
    const tx::Config defaults;
    auto attribute = [&](const char *key, const QString &label, value_t tx::Attributes::*member, int initial = -1) {
        auto *input =
            Integer(attributes, key, label, initial < 0 ? static_cast<int>(defaults.attributes.*member) : initial, 0, 1000000);
        m_bindings.push_back([key = input->objectName(), member](tx::Config &config, const QJsonObject &values) {
            config.attributes.*member = values.value(key).toInt();
        });
    };
    attribute("attackBase", QStringLiteral("基础攻击"), &tx::Attributes::attack_base);
    attribute("attackClass", QStringLiteral("心法攻击"), &tx::Attributes::attack_by_class);
    attribute("weapon", QStringLiteral("武器伤害"), &tx::Attributes::weapon);
    attribute("crit", QStringLiteral("会心等级"), &tx::Attributes::crit);
    attribute("critPower", QStringLiteral("会效等级"), &tx::Attributes::crit_power);
    attribute("overcomeBase", QStringLiteral("基础破防"), &tx::Attributes::overcome_base, 25000);
    attribute("overcomeClass", QStringLiteral("心法破防"), &tx::Attributes::overcome_by_class);
    attribute("strain", QStringLiteral("无双等级"), &tx::Attributes::strain);
    attribute("surplus", QStringLiteral("破招等级"), &tx::Attributes::surplus);
    attribute("haste", QStringLiteral("加速等级"), &tx::Attributes::haste);
    auto bonus = [&](const char *key, const QString &label, int tx::Attributes::*member, int scale = 1024, int maximum = 10000) {
        auto *input = Decimal(bonuses, key, label, 0, 0, maximum * 100.0 / scale, 3);
        input->setSuffix(QStringLiteral(" %"));
        m_bindings.push_back([key = input->objectName(), member, scale](tx::Config &config, const QJsonObject &values) {
            config.attributes.*member = static_cast<int>(std::llround(values.value(key).toDouble() * scale / 100.0));
        });
    };
    bonus("attackBonus", QStringLiteral("攻击加成"), &tx::Attributes::attack_percent);
    bonus("overcomeBonus", QStringLiteral("破防加成"), &tx::Attributes::overcome_percent);
    bonus("critBonus", QStringLiteral("会心几率加成"), &tx::Attributes::crit_basis_points, 10000);
    bonus("critPowerBonus", QStringLiteral("会效加成"), &tx::Attributes::crit_power_percent);
    bonus("hasteBonus", QStringLiteral("加速加成"), &tx::Attributes::haste_percent);
    bonus("damageBonus", QStringLiteral("伤害加成"), &tx::Attributes::damage_percent);
    bonus("strainBonus", QStringLiteral("无双加成"), &tx::Attributes::strain_percent);
    bonus("pveBonus", QStringLiteral("非侠士伤害加成"), &tx::Attributes::pve_percent);
    bonus("ignoreShield", QStringLiteral("无视防御"), &tx::Attributes::ignore_shield, 1024, 1024);
    m_bindings.push_back([](tx::Config &config, const QJsonObject &values) {
        config.target.vulnerable = static_cast<int>(std::llround(values.value("vulnerable").toDouble() * 1024 / 100));
    });

    auto flags = [&](const QString                                           &title,
                     const char                                              *prefix,
                     std::initializer_list<std::pair<const char *, unsigned>> entries,
                     std::function<void(tx::Config &, unsigned)>              assign) {
        const bool talent = QString::fromLatin1(prefix) == QStringLiteral("talent");
        auto *group = talent ? nullptr : new QGroupBox(title);
        auto *rows  = group ? new QGridLayout(group) : nullptr;
        if (rows) rows->setVerticalSpacing(5);
        const bool equipment =
            QString::fromLatin1(prefix) == QStringLiteral("equipment") || QString::fromLatin1(prefix) == QStringLiteral("team");
        if (!talent && !equipment) group->setTitle(QString(title).replace(QStringLiteral("秘籍（最多四本）"), QStringLiteral("（最多四本）")));
        int                                       index   = 0;
        std::vector<std::pair<QString, unsigned>> inputs;
        for (const auto &[label, flag] : entries) {
            auto *input = talent ? txTalents->AddTalent(QString::fromUtf8(label), QString::fromLatin1(prefix) + QString::number(flag))
                                 : new QCheckBox(QString::fromUtf8(label), group);
            input->setObjectName(QString::fromLatin1(prefix) + QString::number(flag));
            input->setProperty("defaultValue", false);
            if (rows) rows->addWidget(input, index, 0);
            ++index;
            inputs.emplace_back(input->objectName(), flag);
        }
        if (!talent) (equipment ? txEquipment : txRecipes)->addRow(group);
        m_bindings.push_back([inputs, assign](tx::Config &config, const QJsonObject &values) {
            unsigned result = 0;
            for (const auto &[key, flag] : inputs) {
                if (values.value(key).toBool()) {
                    result |= flag;
                }
            }
            assign(config, result);
        });
    };
    flags(QStringLiteral("装备特效"),
          "equipment",
          {
              { "橙武特效",  tx::WeaponCW     },
              { "套装·剑鸣", tx::SetAttribute }
    },
          [](tx::Config &c, unsigned v) { c.equipment = v; });
    flags(QStringLiteral("团队增益"),
          "team",
          {
              { "游刃", tx::YouRen }
    },
          [](tx::Config &c, unsigned v) { c.team_effects = v; });
    auto *teamGroup = new QGroupBox(QStringLiteral("外部增益时间表（太虚）"));
    auto *teamRows = new QGridLayout(teamGroup);
    teamRows->addWidget(new QLabel(QStringLiteral("增益")), 0, 0);
    teamRows->addWidget(new QLabel(QStringLiteral("开始（秒）")), 0, 1);
    teamRows->addWidget(new QLabel(QStringLiteral("持续（秒）")), 0, 2);
    teamRows->addWidget(new QLabel(QStringLiteral("层数")), 0, 3);
    for (unsigned i = 0; i < tx::team::Count; ++i) {
        const auto &definition = tx::team::Definitions[i];
        const auto prefix = QStringLiteral("txTeam%1_").arg(i);
        auto *enabled = new QCheckBox(QString::fromUtf8(definition.name.data(), definition.name.size()));
        enabled->setObjectName(prefix + "enabled");
        enabled->setProperty("defaultValue", false);
        teamRows->addWidget(enabled, i + 1, 0);
        for (const auto &field : {QStringLiteral("start"), QStringLiteral("duration")}) {
            auto *input = new QDoubleSpinBox;
            input->setObjectName(prefix + field);
            input->setDecimals(4);
            input->setSingleStep(0.0625);
            input->setRange(field == "start" ? 0 : 0.0625, 86400);
            input->setValue(field == "start" ? 0 : definition.duration / 16.0);
            if (i == tx::team::HaoLingSanJun && field == "duration") input->setRange(60, 60);
            input->setProperty("defaultValue", input->value());
            teamRows->addWidget(input, i + 1, field == "start" ? 1 : 2);
        }
        auto *stacks = new QSpinBox;
        stacks->setObjectName(prefix + "stacks");
        stacks->setRange(1, definition.max_stacks);
        stacks->setProperty("defaultValue", 1);
        if (i == tx::team::HaoLingSanJun) {
            stacks->setMinimum(2);
            stacks->setSingleStep(2);
            stacks->setValue(48);
            stacks->setProperty("defaultValue", 48);
            stacks->setToolTip(QStringLiteral("旧版奇数层扣除不一致，当前支持偶数初始层数；30 秒后减半，60 秒结束，期间重复施加无效。"));
        }
        teamRows->addWidget(stacks, i + 1, 3);
        m_bindings.push_back([i, prefix](tx::Config &config, const QJsonObject &values) {
            if (values.value(prefix + "enabled").toBool())
                config.team_buffs.push_back({static_cast<tx::team::Kind>(i),
                    static_cast<tick_t>(std::llround(values.value(prefix + "start").toDouble() * 16)),
                    static_cast<tick_t>(std::llround(values.value(prefix + "duration").toDouble() * 16)),
                    values.value(prefix + "stacks").toInt()});
        });
    }
    txEquipment->addRow(teamGroup);
    flags(QStringLiteral("奇穴"),
          "talent",
          {
              { "叠刃",   tx::DieRenTalent  },
              { "无意",   tx::WuYi          },
              { "深埋",   tx::ShenMai       },
              { "若水",   tx::RuoShui       },
              { "风逝",   tx::FengShiTalent },
              { "无欲",   tx::WuYu          },
              { "环月",   tx::HuanYue       },
              { "切玉",   tx::QieYu         },
              { "心固",   tx::XinGu         },
              { "同根",   tx::TongGen       },
              { "负阴",   tx::FuYin         },
              { "故长",   tx::GuChang       },
              { "期声",   tx::QiSheng       },
              { "长生",   tx::ChangSheng    },
              { "玄门",   tx::XuanMen       },
              { "裂云",   tx::LieYun        },
              { "虚极",   tx::XuJi          },
              { "剑入",   tx::JianRu        },
              { "镜花影", tx::JingHuaTalent }
    },
          [](tx::Config &c, unsigned v) { c.talents = v; });
    flags(QStringLiteral("无我无剑秘籍（最多四本）"),
          "wuwu",
          {
              { "会心 +2%", tx::Crit2   },
              { "会心 +3%", tx::Crit3   },
              { "会心 +4%", tx::Crit4   },
              { "伤害 +3%", tx::Damage3 },
              { "伤害 +4%", tx::Damage4 },
              { "伤害 +5%", tx::Damage5 },
              { "气点",     tx::Qidian  }
    },
          [](tx::Config &c, unsigned v) { c.recipes[tx::WuWo] = v; });
    flags(QStringLiteral("八荒归元秘籍（最多四本）"),
          "bahuang",
          {
              { "伤害 +3%", tx::Damage3  },
              { "伤害 +4%", tx::Damage4  },
              { "伤害 +5%", tx::Damage5  },
              { "气点",     tx::Qidian   },
              { "冷却缩短", tx::Cooldown }
    },
          [](tx::Config &c, unsigned v) { c.recipes[tx::BaHuang] = v; });
    flags(QStringLiteral("三环套月秘籍（最多四本）"),
          "sanhuan",
          {
              { "会心 +2%", tx::Crit2    },
              { "会心 +3%", tx::Crit3    },
              { "会心 +4%", tx::Crit4    },
              { "伤害 +3%", tx::Damage3  },
              { "伤害 +4%", tx::Damage4  },
              { "伤害 +5%", tx::Damage5  },
              { "冷却缩短", tx::Cooldown }
    },
          [](tx::Config &c, unsigned v) { c.recipes[tx::SanHuan] = v; });
    flags(QStringLiteral("生太极秘籍"),
          "shengtaiji",
          {
              { "运功缩短一", tx::Prepare1 },
              { "运功缩短二", tx::Prepare2 },
              { "运功缩短三", tx::Prepare3 },
              { "冷却缩短",   tx::Cooldown }
    },
          [](tx::Config &c, unsigned v) { c.shengtaiji_recipes = v; });
    flags(QStringLiteral("人剑合一秘籍（最多四本）"),
          "renjian",
          {
              { "冷却缩短",    tx::Cooldown         },
              { "距离增加一",  tx::Range1           },
              { "距离增加二",  tx::Range2           },
              { "距离增加三",  tx::Range3           },
              { "伤害 +40%",   tx::Damage40         },
              { "伤害 +60%",   tx::Damage60         },
              { "持续伤害",    tx::Dot              },
              { "云中·碎星",   tx::YunZhongSuiXing  },
              { "云中·生太极", tx::YunZhongShengTai },
              { "云中·吞日月", tx::YunZhongTunRi    }
    },
          [](tx::Config &c, unsigned v) { c.renjian_recipes = v; });

    auto *mwTalents      = talentPage(desktop::Specialization::MoWen);
    auto *mwRecipes      = effectPage(recipeStack);
    auto *mwEquipment    = equipmentEffects();
    auto mwFlags = [&](const QString                                           &title,
                       const char                                              *prefix,
                       std::initializer_list<std::pair<const char *, unsigned>> entries,
                       std::function<void(mw::Config &, unsigned)>              assign) {
        const bool talent    = QString::fromLatin1(prefix) == QStringLiteral("mwTalent");
        auto *group = talent ? nullptr : new QGroupBox(title);
        auto *rows  = group ? new QGridLayout(group) : nullptr;
        if (rows) rows->setVerticalSpacing(5);
        const bool equipment = QString::fromLatin1(prefix) == QStringLiteral("mwEquipment") ||
                               QString::fromLatin1(prefix) == QStringLiteral("mwTeam");
        if (!talent && !equipment) group->setTitle(QString(title).replace(QStringLiteral("秘籍（最多四本）"), QStringLiteral("（最多四本）")));
        int        index     = 0;
        std::vector<std::pair<QString, unsigned>> inputs;
        for (const auto &[label, flag] : entries) {
            const auto text  = QString::fromUtf8(label);
            auto *input = talent ? mwTalents->AddTalent(text, QString::fromLatin1(prefix) + QString::number(flag))
                                 : new QCheckBox(text.section(QStringLiteral("（"), 0, 0), group);
            if (!talent) input->setToolTip(text);
            input->setObjectName(QString::fromLatin1(prefix) + QString::number(flag));
            input->setProperty("defaultValue", false);
            if (rows) rows->addWidget(input, index, 0);
            ++index;
            inputs.emplace_back(input->objectName(), flag);
        }
        if (!talent) (equipment ? mwEquipment : mwRecipes)->addRow(group);
        m_mowenBindings.push_back([inputs, assign](mw::Config &config, const QJsonObject &values) {
            unsigned result = 0;
            for (const auto &[key, flag] : inputs) {
                if (values.value(key).toBool()) {
                    result |= flag;
                }
            }
            assign(config, result);
        });
    };
    mwFlags(QStringLiteral("装备特效"),
            "mwEquipment",
            {
                { "大附魔·鞋", mw::EnchantShoes },
                { "大附魔·腰", mw::EnchantBelt  },
                { "大附魔·腕", mw::EnchantWrist },
                { "套装·属性", mw::SetAttribute },
                { "套装·技能", mw::SetSkill     },
                { "橙武特效",  mw::WeaponCW     },
                { "水·斩流",   mw::WeaponWater  }
    },
            [](mw::Config &c, unsigned v) { c.equipment = v; });
    mwFlags(QStringLiteral("团队增益"),
            "mwTeam",
            {
                { "游刃", mw::YouRen   },
                { "精妙", mw::JingMiao }
    },
            [](mw::Config &c, unsigned v) { c.team_effects = v; });
    mwFlags(QStringLiteral("奇穴"),
            "mwTalent",
            {
                { "号钟",               mw::HaoZhong       },
                { "飞帆",               mw::FeiFan         },
                { "长清（旧版无效果）", mw::ChangQing      },
                { "弦风",               mw::XianFengTalent },
                { "流照",               mw::LiuZhaoTalent  },
                { "豪情",               mw::HaoQing        },
                { "师襄",               mw::ShiXiang       },
                { "知止",               mw::ZhiZhi         },
                { "刻梦",               mw::KeMeng         },
                { "争鸣",               mw::ZhengMing      },
                { "明津",               mw::MingJinTalent  },
                { "修期（旧版无效果）", mw::XiuQi          },
                { "云汉",               mw::YunHanTalent   },
                { "参连",               mw::CanLianTalent  },
                { "正律和鸣",           mw::ZhengLvHeMing  }
    },
            [](mw::Config &c, unsigned v) { c.talents = v; });
    mwFlags(QStringLiteral("宫秘籍（最多四本）"),
            "mwGong",
            {
                { "会心 +2%",   mw::GongCrit2    },
                { "会心 +3%",   mw::GongCrit3    },
                { "会心 +4%",   mw::GongCrit4    },
                { "伤害 +3%",   mw::GongDamage3  },
                { "伤害 +4%",   mw::GongDamage4  },
                { "运功缩短一", mw::GongPrepare1 },
                { "运功缩短二", mw::GongPrepare2 },
                { "距离增加",   mw::GongRange    }
    },
            [](mw::Config &c, unsigned v) { c.recipes[0] = v; });
    mwFlags(QStringLiteral("商秘籍（最多四本）"),
            "mwShang",
            {
                { "会心 +2%", mw::ShangCrit2   },
                { "会心 +3%", mw::ShangCrit3   },
                { "会心 +4%", mw::ShangCrit4   },
                { "伤害 +3%", mw::ShangDamage3 },
                { "伤害 +4%", mw::ShangDamage4 },
                { "伤害 +5%", mw::ShangDamage5 }
    },
            [](mw::Config &c, unsigned v) { c.recipes[1] = v; });
    mwFlags(QStringLiteral("徵秘籍（最多四本）"),
            "mwZhi",
            {
                { "会心 +2%", mw::ZhiCrit2   },
                { "会心 +3%", mw::ZhiCrit3   },
                { "会心 +4%", mw::ZhiCrit4   },
                { "伤害 +3%", mw::ZhiDamage3 },
                { "伤害 +4%", mw::ZhiDamage4 },
                { "距离增加", mw::ZhiRange   }
    },
            [](mw::Config &c, unsigned v) { c.recipes[2] = v; });
    mwFlags(QStringLiteral("羽秘籍（最多四本）"),
            "mwYu",
            {
                { "会心 +3%", mw::YuCrit3   },
                { "会心 +4%", mw::YuCrit4   },
                { "伤害 +3%", mw::YuDamage3 },
                { "伤害 +4%", mw::YuDamage4 }
    },
            [](mw::Config &c, unsigned v) { c.recipes[3] = v; });
    auto *permanentScroll = new QScrollArea;
    permanentScroll->setObjectName(QStringLiteral("permanentsScroll"));
    m_inputPanels.push_back(permanentScroll);
    permanentScroll->setWidgetResizable(true);
    permanentScroll->setFrameShape(QFrame::NoFrame);
    auto *permanentStack = new QStackedWidget;
    permanentScroll->setWidget(permanentStack);
    permanentLayout->addWidget(permanentScroll);
    permanentScroll->setToolTip(QStringLiteral("沿用旧版常驻增益数值。动态游刃、精妙在配装中选择。"));
    QFile permanentFile(QStringLiteral(":/desktop/permanents.json"));
    if (!permanentFile.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("缺少常驻增益数据资源");
    }
    const auto permanentData = QJsonDocument::fromJson(permanentFile.readAll()).object();
    const std::pair<const char *, const char *> categories[] = {
        { "TeamCore",        "阵法"     },
        { "FoodEnhance",     "增强食品" },
        { "FoodSupport",     "辅助食品" },
        { "MedEnhance",      "增强药品" },
        { "MedSupport",      "辅助药品" },
        { "HomeCook",        "家园炊事" },
        { "HomeWine",        "家园酿造" },
        { "WeaponWhetstone", "武器磨石" },
        { "Others",          "宴席"     }
    };
    for (int specialization = 0; specialization < 2; ++specialization) {
        auto *content = new QWidget;
        content->setProperty("permanentChoices", true);
        auto *grid = new QGridLayout(content);
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setHorizontalSpacing(10);
        grid->setVerticalSpacing(6);
        grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        permanentStack->addWidget(content);
        int position = 0;
        const auto groups = permanentData.value(QString::number(specialization)).toObject();
        for (const auto &[category, label] : categories) {
            const auto items = groups.value(QString::fromLatin1(category)).toArray();
            const bool feast = QString::fromLatin1(category) == "Others";
            auto       add   = [&](const QJsonArray &entries, const QString &suffix, const QString &title) {
                auto *cell = new QWidget;
                auto *cellLayout = new QHBoxLayout(cell);
                cellLayout->setContentsMargins(0, 0, 0, 0);
                cellLayout->setSpacing(6);
                auto *combo = new desktop::IconComboBox(title);
                combo->setObjectName(QStringLiteral("permanent%1%2%3").arg(specialization).arg(QString::fromLatin1(category), suffix));
                combo->setFixedSize(38, 38);
                combo->setStyleSheet(QStringLiteral("QComboBox { min-height: 38px; max-height: 38px; padding: 0; border: 0; }"));
                combo->setProperty("specialization", specialization);
                combo->addItem(QStringLiteral("无"), QJsonArray{});
                combo->setItemData(0, QStringLiteral("不使用此项增益"), Qt::ToolTipRole);
                for (const auto &value : entries) {
                    const auto entry = value.toObject();
                    const auto label = entry.value("Name").toString();
                    combo->addItem(desktop::GameIcon(entry.value("Icon").toInt()), label, entry.value("AttributeAdditional").toArray());
                    combo->setItemData(combo->count() - 1, label + '\n' + entry.value("Description").toString(), Qt::ToolTipRole);
                }
                combo->setToolTip(title + QStringLiteral("：无；点击图标选择"));
                cellLayout->addWidget(combo);
                auto *caption = new QLabel(feast ? QStringLiteral("宴席") : title);
                caption->setObjectName(combo->objectName() + QStringLiteral("Caption"));
                caption->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                caption->setStyleSheet(QStringLiteral("font-size: 11px; color: #92929d;"));
                cellLayout->addWidget(caption);
                grid->addWidget(cell, position / 3, position % 3);
                ++position;
                m_permanents.push_back(combo);
            };
            if (feast) {
                for (int index = 0; index < items.size(); ++index) {
                    add({ items[index] }, QString::number(index), items[index].toObject().value("Name").toString());
                }
            } else {
                add(items, {}, QString::fromUtf8(label));
            }
        }
    }
    auto *importAttributes = new QPushButton(QStringLiteral("导入魔盒属性"));
    importAttributes->setObjectName(QStringLiteral("importAttributes"));
    importAttributes->setProperty("moduleAction", true);
    m_gameAttributes->tools->insertWidget(0, importAttributes, 1);
    m_inputPanels.push_back(importAttributes);
    connect(importAttributes, &QPushButton::clicked, this, [this] {
        QDialog dialog(this);
        dialog.setWindowTitle(QStringLiteral("导入魔盒属性"));
        dialog.resize(620, 480);
        auto *rows = new QVBoxLayout(&dialog);
        auto *note = new QLabel(QStringLiteral("粘贴魔盒导出的属性对象。按当前心法读取外功或阴性内功属性；导入会替换属"
                                               "性并清空常驻增益选择，避免重复叠加。"));
        note->setWordWrap(true);
        rows->addWidget(note);
        auto *text = new QPlainTextEdit(&dialog);
        rows->addWidget(text);
        auto *status = new QLabel(&dialog);
        status->setWordWrap(true);
        rows->addWidget(status);
        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
        buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("导入"));
        buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("取消"));
        rows->addWidget(buttons);
        connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
        connect(buttons, &QDialogButtonBox::accepted, &dialog, [&, this] {
            QJsonParseError parse;
            const auto      json = QJsonDocument::fromJson(text->toPlainText().toUtf8(), &parse);
            QString         error;
            if (parse.error != QJsonParseError::NoError || !json.isObject()) {
                status->setText(QStringLiteral("请输入有效的属性对象。"));
            } else if (!ImportAttributes(json.object(), error)) {
                status->setText(error);
            } else {
                dialog.accept();
            }
        });
        dialog.exec();
    });
    Integer(bonuses, "importedMajor", QStringLiteral("已含身法 / 根骨"), 0, 0, 1000000)
        ->setToolTip(QStringLiteral("当前会心与心法攻击已包含此主属性；用于精确计算食品、药品增加的主属性收益。"));
    Integer(bonuses, "importedStrength", QStringLiteral("已含力道"), 41, 0, 1000000);
    auto updateClass = [=, this] {
        const bool moWen = Specialization() == desktop::Specialization::MoWen;
        m_equipmentDialog->setWindowTitle(desktop::ClassName(Specialization()) + QStringLiteral(" · 配装"));
        talentStack->setCurrentIndex(moWen ? 1 : 0);
        talentHeaders->setCurrentIndex(moWen ? 1 : 0);
        UpdateTalentHeight();
        recipeStack->setCurrentIndex(moWen ? 1 : 0);
        txEquipment->parentWidget()->setVisible(!moWen);
        mwEquipment->parentWidget()->setVisible(moWen);
        equipmentPanel->setVisible(!moWen);
        attributes->setRowVisible(findChild<QSpinBox *>("overcomeClass"), !moWen);
        permanentStack->setCurrentIndex(m_specialization->currentIndex());
        emit SpecializationChanged();
    };
    connect(m_specialization, &QComboBox::currentIndexChanged, this, updateClass);
    QSettings settings;
    for (auto *input : m_permanents) {
        input->setCurrentIndex(std::clamp(settings.value("simulation/" + input->objectName(), 0).toInt(), 0, input->count() - 1));
    }
    m_specialization->setCurrentIndex(settings.value("simulation/specialization", 0).toInt() == 1 ? 1 : 0);
    updateClass();
    for (auto *input : findChildren<QSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        input->setValue(settings.value("simulation/" + input->objectName(), input->value()).toInt());
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        input->setValue(settings.value("simulation/" + input->objectName(), input->value()).toDouble());
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        input->setChecked(settings.value("simulation/" + input->objectName(), false).toBool());
    }
    for (int index = 0; index < 2; ++index) {
        const auto stored = settings.value(QStringLiteral("simulation/talentPreview%1").arg(index)).toByteArray();
        if (stored.isEmpty()) continue;
        const auto selected = QJsonDocument::fromJson(stored).array();
        QString error;
        if (m_talentPanels[index]->ValidateSelection(selected, error)) m_talentPanels[index]->RestoreSelection(selected);
    }
    m_seed->setText(settings.value(QStringLiteral("simulation/seed"), QStringLiteral("0")).toString());
    auto legacyFlow = desktop::FlowDefaults();
    for (const auto &field : desktop::FlowFields)
        legacyFlow.insert(QString::fromLatin1(field.key), settings.value("simulation/" + QString::fromLatin1(field.key), field.initial).toDouble());
    if (std::all_of(desktop::FlowFields.begin(), desktop::FlowFields.end(), [&](const auto &field) {
            return desktop::ValidFlowValue(field, legacyFlow.value(QString::fromLatin1(field.key)));
        })) m_flowValues = legacyFlow;
    m_flowEditor->setPlainText(settings.value(QStringLiteral("simulation/flowMacro"), desktop::FlowMacroFromValues(legacyFlow)).toString());
    auto *reset = findChild<QMenu *>(QStringLiteral("configurationMenu"))->addAction(QStringLiteral("重置配置"));
    reset->setObjectName(QStringLiteral("resetConfiguration"));
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 0);
    layout->setRowMinimumHeight(1, 235);
    layout->setRowStretch(2, 1);
    connect(reset, &QAction::triggered, this, [this] {
        for (auto *panel : m_talentPanels) panel->ClearSelection();
        for (auto *input : findChildren<QSpinBox *>()) {
            if (input->property("flowViewOnly").toBool()) continue;
            input->setValue(input->property("defaultValue").toInt());
        }
        for (auto *input : findChildren<QDoubleSpinBox *>()) {
            if (input->property("flowViewOnly").toBool()) continue;
            input->setValue(input->property("defaultValue").toDouble());
        }
        for (auto *input : findChildren<QCheckBox *>()) {
            if (input->property("flowViewOnly").toBool()) continue;
            input->setChecked(false);
        }
        m_seed->setText(QStringLiteral("0"));
        m_flowEditor->setPlainText(desktop::FlowMacroFromValues(desktop::FlowDefaults()));
        for (auto *input : m_permanents) {
            input->setCurrentIndex(0);
        }
    });
    auto refresh = [this] {
        if (m_refreshQueued) return;
        m_refreshQueued = true;
        QTimer::singleShot(0, this, [this] { m_refreshQueued = false; RefreshGameAttributes(); });
    };
    for (auto *input : findChildren<QSpinBox *>()) connect(input, &QSpinBox::valueChanged, this, refresh);
    for (auto *input : findChildren<QDoubleSpinBox *>()) connect(input, &QDoubleSpinBox::valueChanged, this, refresh);
    for (auto *input : findChildren<QComboBox *>()) connect(input, &QComboBox::currentIndexChanged, this, refresh);
    for (auto *input : findChildren<QCheckBox *>()) connect(input, &QCheckBox::toggled, this, refresh);
    connect(m_seed, &QLineEdit::textChanged, this, refresh);
    connect(m_permanentsInPanel, &QTabBar::currentChanged, this, [this] (int index) {
        QSettings().setValue(QStringLiteral("interface/permanentsInPanel"), index == 0);
        RefreshGameAttributes();
    });
    connect(m_flowEditor, &QPlainTextEdit::textChanged, this, &SimulationOptions::UpdateFlowMacro);
    connect(m_seconds, &QDoubleSpinBox::valueChanged, this, [this](double seconds) { m_flowTimeline->SetDuration(qRound(seconds * 16)); });
    m_flowTimeline->SetDuration(qRound(m_seconds->value() * 16));
    UpdateFlowMacro();
    ShowSection(QStringLiteral("simulation"));
    RefreshGameAttributes();
}

SimulationOptions::~SimulationOptions()
{
    // Syntax-highlighter teardown can emit textChanged while child widgets are being deleted.
    disconnect(m_flowEditor, nullptr, this, nullptr);
    m_flowTimeline->statusChanged = {};
}

void SimulationOptions::RefreshGameAttributes()
{
    std::array<QString, 10> panel;
    std::array<QString, 10> changes{};
    panel.fill(QStringLiteral("--"));
    const auto values = Values();
    const bool included = m_permanentsInPanel->currentIndex() == 0;
    try {
        auto readPanel = [&](const QJsonObject &inputValues) {
            const auto config = ActiveConfigFromValues(inputValues, Specialization());
            value_t major = inputValues.value("importedMajor").toInt();
            for (const auto *input : m_permanents) {
                if (input->property("specialization").toInt() != static_cast<int>(Specialization())) continue;
                for (const auto &entry : input->itemData(inputValues.value(input->objectName()).toInt()).value<QJsonArray>()) {
                    const auto effect = entry.toObject();
                    if (effect.value("Type").toString() == QStringLiteral("身法") || effect.value("Type").toString() == QStringLiteral("根骨")) major += effect.value("Value").toInt();
                }
            }
            return std::visit([&](const auto &configuration) -> std::array<double, 10> {
                const auto &a = configuration.attributes;
                value_t overcome;
                if constexpr (requires { a.overcome_base; }) overcome = a.overcome_base * (1024 + a.overcome_percent) / 1024 + a.overcome_by_class;
                else overcome = a.overcome * (1024 + a.overcome_percent) / 1024;
                return {double(major), double(a.attack_base * (1024 + a.attack_percent) / 1024 + a.attack_by_class), 100.0,
                    100 * (a.crit / (9.530 * 8250) + a.crit_basis_points / 10000.0),
                    100 * (1.75 + (static_cast<int>(a.crit_power * 1024 / (3.335 * 8250)) + a.crit_power_percent) / 1024.0),
                    100 * (static_cast<int>(a.haste * 1024 / (11.695 * 8250)) + a.haste_percent) / 1024.0,
                    100 * static_cast<int>(overcome * 1024 / (9.530 * 8250)) / 1024.0,
                    100 * (static_cast<int>(a.strain * 1024 / (9.189 * 8250)) + a.strain_percent) / 1024.0,
                    double(a.surplus), double(a.weapon)};
            }, config);
        };
        // Clear only permanent selections in a copy; both projections retain manual bonuses and integer rounding.
        auto baseValues = values;
        for (const auto *input : m_permanents) baseValues.insert(input->objectName(), 0);
        const auto base = readPanel(baseValues);
        const auto current = included ? readPanel(values) : base;
        for (int i = 0; i < int(panel.size()); ++i) {
            const bool percentage = i >= 2 && i <= 7;
            const int decimals = percentage ? 2 : 0;
            const auto suffix = percentage ? QStringLiteral("%") : QString{};
            panel[i] = i == 0 && values.value("importedMajor").toInt() <= 0 ? QStringLiteral("--")
                : QString::number(current[i], 'f', decimals) + suffix;
            const double scale = percentage ? 100 : 1;
            const auto delta = (std::round(current[i] * scale) - std::round(base[i] * scale)) / scale;
            if (included && delta != 0)
                changes[i] = (delta > 0 ? QStringLiteral("+") : QString{}) + QString::number(delta, 'f', decimals) + suffix;
        }
        m_gameAttributes->setToolTip({});
    } catch (const std::exception &error) {
        m_gameAttributes->setToolTip(QString::fromUtf8(error.what()));
    }
    m_gameAttributes->SetValues(Specialization(), panel, changes, included);
    if (m_gainSnapshot.isEmpty() || m_gainSnapshot != Snapshot()) ClearAttributeGains();
}

bool SimulationOptions::CheckFlowMacro(QString &error) const
{
    error = desktop::CompileFlowMacro(m_flowEditor->toPlainText()).ErrorText();
    return error.isEmpty();
}

void SimulationOptions::FocusFlowMacro()
{
    m_flowTimeline->ShowScript();
    QMetaObject::invokeMethod(m_flowEditor, "CheckSyntax", Qt::DirectConnection);
    const auto compiled = desktop::CompileFlowMacro(m_flowEditor->toPlainText());
    m_flowEditor->setFocus();
    if (compiled.errors.isEmpty()) return;
    const auto &error = compiled.errors.front();
    const auto block = m_flowEditor->document()->findBlockByNumber(error.line - 1);
    if (!block.isValid()) return;
    QTextCursor cursor(block);
    cursor.setPosition(block.position() + error.column);
    cursor.setPosition(qMin(block.position() + error.column + error.length, block.position() + block.length() - 1), QTextCursor::KeepAnchor);
    m_flowEditor->setTextCursor(cursor);
    m_flowEditor->ensureCursorVisible();
}

void SimulationOptions::UpdateFlowMacro()
{
    const auto source = m_flowEditor->toPlainText();
    // Syntax highlighting and theme changes also emit textChanged; only source edits change configuration.
    if (m_flowInitialized && source == m_flowSource) return;
    m_flowInitialized = true;
    m_flowSource = source;
    const auto compiled = desktop::CompileFlowMacro(source);
    if (compiled.errors.isEmpty()) { m_flowValues = compiled.values; m_flowEvents = compiled.TargetEvents(); }
    RefreshGameAttributes();
}

void SimulationOptions::UpdateTalentHeight()
{
    if (m_section != QStringLiteral("simulation") && m_section != QStringLiteral("attributes")) return;
    auto *panel = m_talentPanels[static_cast<int>(Specialization())];
    if (!panel) return;
    auto *tabs = findChild<QTabWidget *>(QStringLiteral("configurationTabs"));
    const bool legacy = panel->HeaderWidget()->findChild<QToolButton *>(QStringLiteral("legacyTalentToggle"))->isChecked();
    tabs->setFixedHeight(tabs->currentIndex() == 0 && !legacy ? 100 : 270);
}

void SimulationOptions::SetAttributeGainResult(const SimulationResult &result)
{
    if (m_gainSnapshot != Snapshot()) m_gameGains->Clear(Specialization());
    m_gainSnapshot = result.gainsRequested ? Snapshot() : QJsonObject{};
    m_gameGains->SetResult(result, Specialization());
}

desktop::GainComparisonMode SimulationOptions::GainMode() const { return m_gameGains->Mode(); }
bool SimulationOptions::AutoCalculateGains() const { return m_gameGains->calculate->isChecked(); }

void SimulationOptions::ClearAttributeGains()
{
    m_gainSnapshot = {};
    m_gameGains->Clear(Specialization());
}

void SimulationOptions::SetInputsEnabled(bool enabled)
{
    if (!enabled) m_attributeEditor->hide();
    m_flowTimeline->SetReadOnly(!enabled);
    for (auto *panel : m_inputPanels) {
        panel->setEnabled(enabled);
    }
}

void SimulationOptions::ShowAttributeGains()
{
    m_gameGains->calculate->setFocus(Qt::OtherFocusReason);
}

QToolButton *SimulationOptions::GainsButton() const { return m_gameGains->calculate; }

void SimulationOptions::ShowEquipment()
{
    m_equipmentDialog->show();
    m_equipmentDialog->raise();
    m_equipmentDialog->activateWindow();
}

void SimulationOptions::CloseAuxiliaryWindows()
{
    for (auto *dialog : findChildren<QDialog *>()) dialog->close();
}

QWidget *SimulationOptions::TakeOutputPanel()
{
    m_externalOutput = true;
    auto *panel = m_output->parentWidget();
    layout()->removeWidget(panel);
    auto *profiles = findChild<QGroupBox *>(QStringLiteral("configurationSchemes"));
    profiles->setTitle({});
    profiles->layout()->setContentsMargins(10, 6, 10, 6);
    qobject_cast<QGroupBox *>(panel)->setTitle({});
    panel->layout()->setContentsMargins(12, 8, 12, 8);
    return panel;
}

void SimulationOptions::ShowSection(const QString &section)
{
    m_section = section;
    setMaximumWidth(section == QStringLiteral("attributes") ? 664 : QWIDGETSIZE_MAX);
    const bool expanded = section != QStringLiteral("simulation");
    setStyleSheet(QStringLiteral("QWidget { font-size: %1px; } QWidget#simulationOptions { background: transparent; }")
        .arg(expanded ? 14 : 13));
    if (!expanded) setStyleSheet(styleSheet() + QStringLiteral(
        "QComboBox, QSpinBox, QDoubleSpinBox, QLineEdit { min-height: 20px; padding: 1px 6px; }"));
    for (auto *widget : findChildren<QWidget *>()) {
        if (widget->property("configurationForm").toBool())
            static_cast<ConfigurationForm *>(widget)->SetExpanded(expanded);
        if (widget->property("permanentChoices").toBool()) {
            auto *choices = static_cast<QGridLayout *>(widget->layout());
            choices->setAlignment(expanded ? Qt::AlignTop | Qt::AlignLeft : Qt::AlignTop);
            for (int column = 0; column < 3; ++column) choices->setColumnStretch(column, expanded ? 0 : 1);
        }
    }
    auto *import = findChild<QPushButton *>(QStringLiteral("importAttributes"));
    import->setMaximumWidth(expanded ? 200 : QWIDGETSIZE_MAX);
    auto *grid = qobject_cast<QGridLayout *>(layout());
    auto *profiles = findChild<QWidget *>(QStringLiteral("configurationSchemes"));
    auto *run = findChild<QWidget *>(QStringLiteral("runSettings"));
    auto *permanents = findChild<QWidget *>(QStringLiteral("permanentPanel"));
    auto *settingsRow = findChild<QWidget *>(QStringLiteral("simulationSettingsRow"));
    auto *talents = findChild<QWidget *>(QStringLiteral("configurationTabs"));
    auto *flow = findChild<QWidget *>(QStringLiteral("flowMacroPanel"));
    auto *output = m_output->parentWidget();
    for (auto *panel : {profiles, run, permanents, m_attributePane, static_cast<QWidget *>(m_gameGains), flow, talents, output}) {
        if (panel == output && m_externalOutput) continue;
        if (panel->parentWidget() == settingsRow) settingsRow->layout()->removeWidget(panel);
        else grid->removeWidget(panel);
        panel->hide();
        panel->setMinimumSize(0, 0);
        panel->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }
    grid->removeWidget(settingsRow);
    settingsRow->hide();
    for (int row = 0; row < grid->rowCount(); ++row) {
        grid->setRowStretch(row, 0);
        grid->setRowMinimumHeight(row, 0);
    }
    for (int column = 0; column < grid->columnCount(); ++column) {
        grid->setColumnStretch(column, 0);
        grid->setColumnMinimumWidth(column, 0);
    }
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    m_attributePane->setMinimumWidth(208);
    m_gameGains->setMinimumWidth(208);
    m_attributePane->setFixedHeight(270);
    m_gameGains->setFixedHeight(270);
    flow->setFixedSize(224, 270);
    auto add = [this, grid](QWidget *widget, int row, int col, int span = 1) {
        grid->addWidget(widget, row, col, 1, span);
        if (widget == m_attributePane || widget == m_gameGains) grid->setAlignment(widget, Qt::AlignTop);
        widget->show();
    };
    if (section == QStringLiteral("simulation")) {
        grid->setAlignment({});
        grid->setSpacing(4);
        add(profiles, 0, 0, 3);
        // The upper cards have their own columns, independent of attributes/gains below.
        run->setFixedWidth(224);
        settingsRow->layout()->addWidget(run);
        settingsRow->layout()->addWidget(permanents);
        run->show();
        permanents->show();
        add(settingsRow, 1, 0, 3);
        settingsRow->setMaximumHeight(236);
        run->setMaximumHeight(236);
        permanents->setMaximumHeight(236);
        add(m_attributePane, 2, 0);
        add(m_gameGains, 2, 1);
        add(flow, 2, 2);
        grid->setAlignment(flow, Qt::AlignTop);
        add(talents, 3, 0, 3);
        talents->setFixedHeight(100);
        grid->setAlignment(talents, Qt::AlignTop);
        if (!m_externalOutput) add(output, 4, 0, 3);
        grid->setRowMinimumHeight(1, 222);
        grid->setRowMinimumHeight(2, 270);
        grid->setRowMinimumHeight(3, 100);
        grid->setRowStretch(3, 1);
    } else {
        grid->setAlignment(Qt::AlignTop);
        grid->setSpacing(8);
        add(profiles, 0, 0, 3);
        if (section == "attributes") {
            add(m_attributePane, 1, 0);
            add(m_gameGains, 1, 1);
            add(flow, 1, 2);
            grid->setAlignment(flow, Qt::AlignTop);
            talents->setFixedHeight(100);
            add(talents, 2, 0, 3);
            grid->setAlignment(talents, Qt::AlignTop);
        } else {
            QWidget *panel = section == "talents" ? talents : section == "permanents" ? permanents : run;
            add(panel, 1, 0, 3);
            panel->setMinimumHeight(section == "permanents" ? 300 : 440);
        }
        grid->setRowStretch(1, 1);
    }
    UpdateTalentHeight();
    updateGeometry();
}

tx::Config SimulationOptions::Config() const
{
    QString flowError;
    if (!CheckFlowMacro(flowError)) throw std::invalid_argument(flowError.toUtf8().constData());
    if (m_talentPanels[0]->HasSelection())
        throw std::invalid_argument("新版九重奇穴目前仅支持配装预览，战斗机制尚未迁移。请清空新版配装后使用旧规则模拟选项。");
    return std::get<tx::Config>(ActiveConfigFromValues(Values(), desktop::Specialization::TaiXu, m_flowEvents));
}

tx::Config SimulationOptions::ConfigFromValues(const QJsonObject &values) const
{
    tx::Config config;
    for (const auto &assign : m_bindings) {
        assign(config, values);
    }
    return config;
}

desktop::Specialization SimulationOptions::Specialization() const
{
    return m_specialization->currentIndex() == 1 ? desktop::Specialization::MoWen : desktop::Specialization::TaiXu;
}

desktop::Config SimulationOptions::ActiveConfig() const
{
    QString flowError;
    if (!CheckFlowMacro(flowError)) throw std::invalid_argument(flowError.toUtf8().constData());
    if (m_talentPanels[static_cast<int>(Specialization())]->HasSelection())
        throw std::invalid_argument("新版九重奇穴目前仅支持配装预览，战斗机制尚未迁移。请清空新版配装后使用旧规则模拟选项。");
    return ActiveConfigFromValues(Values(), Specialization(), m_flowEvents);
}

desktop::Config SimulationOptions::ActiveConfigFromValues(const QJsonObject &values, desktop::Specialization specialization,
    std::span<const runtime::TargetEvent> events) const
{
    auto  taiXu      = ConfigFromValues(values);
    taiXu.target_events.assign(events.begin(), events.end());
    auto &attr       = taiXu.attributes;
    int   majorAdded = 0, strengthBonus = 0;
    for (auto *input : m_permanents) {
        if (input->property("specialization").toInt() != static_cast<int>(specialization)) {
            continue;
        }
        const auto effects = input->itemData(values.value(input->objectName()).toInt()).value<QJsonArray>();
        for (const auto &effect : effects) {
            const auto object = effect.toObject();
            const auto type   = object.value("Type").toString();
            const auto amount = object.value("Value").toInt();
            if (type == QStringLiteral("基础攻击")) {
                attr.attack_base += amount;
            } else if (type == QStringLiteral("基础破防等级")) {
                attr.overcome_base += amount;
            } else if (type == QStringLiteral("会心等级")) {
                attr.crit += amount;
            } else if (type == QStringLiteral("无双")) {
                attr.strain += amount;
            } else if (type == QStringLiteral("破招值")) {
                attr.surplus += amount;
            } else if (type == QStringLiteral("加速等级")) {
                attr.haste += amount;
            } else if (type == QStringLiteral("身法") || type == QStringLiteral("根骨")) {
                majorAdded += amount;
            } else if (type == QStringLiteral("力道加成")) {
                strengthBonus += amount;
            } else if (type == QStringLiteral("基础攻击加成")) {
                attr.attack_percent += amount;
            } else if (type == QStringLiteral("无双加成")) {
                attr.strain_percent += amount;
            } else if (type == QStringLiteral("伤害加成")) {
                attr.damage_percent += amount;
            } else if (type == QStringLiteral("外功会效加成")) {
                attr.crit_power_percent += amount;
            } else if (type == QStringLiteral("外功破防加成")) {
                attr.overcome_percent += amount;
            } else if (type.endsWith(QStringLiteral("会心加成"))) {
                attr.crit_basis_points += amount;
            } else {
                throw std::invalid_argument("常驻增益包含不支持的属性");
            }
        }
    }
    const value_t major             = values.value("importedMajor").toInt();
    const auto    attackCoefficient = specialization == desktop::Specialization::MoWen ? 1895 : 1485;
    const auto    critCoefficient   = specialization == desktop::Specialization::MoWen ? 389 : 594;
    attr.attack_by_class      += (major + majorAdded) * attackCoefficient / 1024 - major * attackCoefficient / 1024;
    attr.crit                 += (major + majorAdded) * critCoefficient / 1024 - major * critCoefficient / 1024 +
                                 (major + majorAdded) * 655 / 1024 - major * 655 / 1024;
    const value_t strength     = values.value("importedStrength").toInt();
    const value_t newStrength  = strength * (1024 + strengthBonus) / 1024;
    attr.attack_base          += newStrength * 153 / 1024 - strength * 153 / 1024;
    attr.overcome_base        += newStrength * 307 / 1024 - strength * 307 / 1024;
    if (specialization == desktop::Specialization::TaiXu) {
        return taiXu;
    }
    mw::Config  config;
    const auto &a     = taiXu.attributes;
    config.attributes = {
        a.attack_base,
        a.attack_by_class,
        a.weapon,
        a.crit,
        a.crit_power,
        a.overcome_base,
        a.strain,
        a.surplus,
        a.haste,
        a.attack_percent,
        a.overcome_percent,
        a.haste_percent,
        a.crit_basis_points,
        a.crit_power_percent,
        a.damage_percent,
        a.strain_percent,
        a.pve_percent,
        a.ignore_shield
    };
    config.target = { taiXu.target.level, taiXu.target.shield, taiXu.target.vulnerable, taiXu.target.life, taiXu.target.distance };
    config.target_events = std::move(taiXu.target_events);
    config.delay          = taiXu.delay;
    config.initial_yingzi = values.value("mwShadows").toInt();
    config.initial_style  = static_cast<mw::Style>(values.value("mwStyle").toInt());
    for (const auto &assign : m_mowenBindings) {
        assign(config, values);
    }
    return config;
}

runtime::BatchOptions SimulationOptions::Batch() const
{
    runtime::BatchOptions batch;
    batch.iterations = m_iterations->value();
    batch.workers    = m_workers->value();
    batch.duration   = static_cast<tick_t>(std::llround(m_seconds->value() * 16.0));
    bool valid       = false;
    batch.seed       = m_seed->text().toULongLong(&valid);
    if (!valid) {
        throw std::invalid_argument("随机种子须为 0 至 18446744073709551615 之间的整数。");
    }
    batch.damage_capacity   = static_cast<std::size_t>(batch.duration) * 16 + 64;
    batch.mutation_capacity = static_cast<std::size_t>(batch.duration) * 40 + 128 + m_flowEvents.size() * 2;
    return batch;
}

void SimulationOptions::SaveSettings() const
{
    QSettings settings;
    for (int index = 0; index < 2; ++index)
        settings.setValue(QStringLiteral("simulation/talentPreview%1").arg(index), QJsonDocument(m_talentPanels[index]->Selection()).toJson(QJsonDocument::Compact));
    for (auto *input : findChildren<QSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        settings.setValue("simulation/" + input->objectName(), input->value());
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        settings.setValue("simulation/" + input->objectName(), input->value());
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        settings.setValue("simulation/" + input->objectName(), input->isChecked());
    }
    settings.setValue(QStringLiteral("simulation/seed"), m_seed->text());
    settings.setValue("simulation/specialization", m_specialization->currentIndex());
    settings.setValue(QStringLiteral("simulation/flowMacro"), m_flowEditor->toPlainText());
    for (const auto &field : desktop::FlowFields)
        settings.setValue("simulation/" + QString::fromLatin1(field.key), m_flowValues.value(QString::fromLatin1(field.key)).toVariant());
    for (auto *input : m_permanents) {
        settings.setValue("simulation/" + input->objectName(), input->currentIndex());
    }
}

QJsonObject SimulationOptions::Values() const
{
    QJsonObject values = m_flowValues;
    for (auto *input : findChildren<QSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        values.insert(input->objectName(), input->value());
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        values.insert(input->objectName(), input->value());
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        values.insert(input->objectName(), input->isChecked());
    }
    // JSON numbers cannot portably preserve all uint64 seeds.
    values.insert(QStringLiteral("seed"), m_seed->text());
    for (auto *input : m_permanents) {
        values.insert(input->objectName(), input->currentIndex());
    }
    return values;
}

QJsonObject SimulationOptions::Snapshot() const
{
    return {
        { "format",         "jx3dps.desktop.config"          },
        { "version",        4                                },
        { "specialization", m_specialization->currentIndex() },
        { "values",         Values()                         },
        { "flowMacro",      m_flowEditor->toPlainText()       },
        { "talents",        QJsonObject{{"version", desktop::TalentPanel::DataVersion()},
                               {"0", m_talentPanels[0]->Selection()}, {"1", m_talentPanels[1]->Selection()}} }
    };
}

bool SimulationOptions::ValidateSnapshot(const QJsonObject &snapshot, QString &error) const
{
    error.clear();
    if (snapshot.value("format") != QJsonValue("jx3dps.desktop.config") || snapshot.value("version") != QJsonValue(4) ||
        !snapshot.value("values").isObject())
    {
        error = QStringLiteral("不支持此配置格式或版本。");
        return false;
    }
    if (snapshot.value("specialization") != QJsonValue(0) && snapshot.value("specialization") != QJsonValue(1)) {
        error = QStringLiteral("心法编号无效。");
        return false;
    }
    const auto values = snapshot.value("values").toObject();
    if (!snapshot.value("flowMacro").isString()) {
        error = QStringLiteral("流程宏必须是文本。"); return false;
    }
    const auto flow = desktop::CompileFlowMacro(snapshot.value("flowMacro").toString());
    if (!flow.errors.isEmpty()) { error = flow.ErrorText(); return false; }
    for (const auto &field : desktop::FlowFields) {
        const auto key = QString::fromLatin1(field.key);
        if (!desktop::ValidFlowValue(field, values.value(key)) || values.value(key) != flow.values.value(key)) {
            error = QStringLiteral("流程宏与配置值不一致：%1").arg(desktop::FieldName(key)); return false;
        }
    }
    const auto talents = snapshot.value("talents").toObject();
    if (talents.value("version") != QJsonValue(desktop::TalentPanel::DataVersion())) {
        error = QStringLiteral("奇穴数据版本不匹配。"); return false;
    }
    for (int index = 0; index < 2; ++index) {
        const auto selected = talents.value(QString::number(index));
        if (!selected.isArray()) { error = QStringLiteral("奇穴配装字段无效。"); return false; }
        if (!m_talentPanels[index]->ValidateSelection(selected.toArray(), error)) return false;
    }
    if (values.value("mwStyle") != QJsonValue(0) && values.value("mwStyle") != QJsonValue(1)) {
        error = QStringLiteral("初始曲目无效。");
        return false;
    }
    if (values.keys() != Values().keys()) {
        error = QStringLiteral("配置缺少字段或包含未知字段。");
        return false;
    }
    auto invalid = [&](const QString &key) {
        error = QStringLiteral("配置值无效：%1").arg(desktop::FieldName(key));
        return false;
    };
    for (auto *input : findChildren<QSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        const auto v = values.value(input->objectName());
        const auto n = v.toDouble();
        if (!v.isDouble() || !std::isfinite(n) || n < input->minimum() || n > input->maximum() || std::floor(n) != n) {
            return invalid(input->objectName());
        }
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        const auto v     = values.value(input->objectName());
        const auto n     = v.toDouble();
        const auto scale = std::pow(10.0, input->decimals());
        if (!v.isDouble() || !std::isfinite(n) || n < input->minimum() || n > input->maximum() ||
            std::abs(n * scale - std::round(n * scale)) > 0.0001)
        {
            return invalid(input->objectName());
        }
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        if (!values.value(input->objectName()).isBool()) {
            return invalid(input->objectName());
        }
    }
    for (auto *input : m_permanents) {
        const auto value = values.value(input->objectName());
        if (!value.isDouble() || value.toDouble() != value.toInt() || value.toInt() < 0 || value.toInt() >= input->count())
        {
            return invalid(input->objectName());
        }
    }
    const auto seed  = values.value("seed");
    bool       valid = false;
    seed.toString().toULongLong(&valid);
    if (!seed.isString() || !valid || !QRegularExpression(QStringLiteral("^[0-9]{1,20}$")).match(seed.toString()).hasMatch())
    {
        return invalid(QStringLiteral("seed"));
    }
    try {
        // Validate coupled rules (recipes, haste, etc.) before changing any control.
        std::visit([](const auto &config) { desktop::MakeRules(config); },
                   ActiveConfigFromValues(values, static_cast<desktop::Specialization>(snapshot.value("specialization").toInt()), flow.TargetEvents()));
    } catch (const std::exception &exception) {
        error = desktop::Diagnostic(QString::fromUtf8(exception.what()));
        return false;
    }
    return true;
}

QJsonObject SimulationOptions::NormalizeSnapshot(const QJsonObject &snapshot) const
{
    auto withTeamDefaults = [this](QJsonObject result) {
        if (result.value("version") == QJsonValue(2)) {
            result.remove("flowMacro");
            QJsonObject talents{{"version", desktop::TalentPanel::DataVersion()}};
            for (int index = 0; index < 2; ++index) {
                QJsonArray empty;
                for (int slot = 0; slot < m_talentPanels[index]->Selection().size(); ++slot) empty.append(QString{});
                talents.insert(QString::number(index), empty);
            }
            result.insert("version", 3);
            result.insert("talents", talents);
        }
        auto values = result.value("values").toObject();
        for (auto *input : findChildren<QWidget *>()) {
            const auto key = input->objectName();
            if (!key.startsWith("txTeam") || values.contains(key)) continue;
            if (qobject_cast<QCheckBox *>(input)) values.insert(key, false);
            else if (qobject_cast<QSpinBox *>(input) || qobject_cast<QDoubleSpinBox *>(input))
                values.insert(key, QJsonValue::fromVariant(input->property("defaultValue")));
        }
        result.insert("values", values);
        if (!result.contains("flowMacro")) result.insert("flowMacro", desktop::FlowMacroFromValues(values));
        result.insert("version", 4);
        return result;
    };
    if (snapshot.value("format") != QJsonValue("jx3dps.tai-xu.config") || snapshot.value("version") != QJsonValue(1)) {
        return snapshot.value("format") == QJsonValue("jx3dps.desktop.config") &&
            (snapshot.value("version") == QJsonValue(2) || snapshot.value("version") == QJsonValue(3) || snapshot.value("version") == QJsonValue(4))
            ? withTeamDefaults(snapshot) : snapshot;
    }
    auto converted = snapshot;
    auto values    = snapshot.value("values").toObject();
    for (const auto &key : values.keys()) {
        if (key.startsWith("mw") || key.startsWith("permanent") || key.startsWith("imported")) {
            return snapshot;
        }
    }
    // Older Tai Xu snapshots predate the Mo Wen fields; fill only these new keys.
    for (auto *input : findChildren<QSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        if (input->objectName().startsWith("mw")) {
            values.insert(input->objectName(), input->property("defaultValue").toInt());
        }
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        if (input->objectName().startsWith("mw")) {
            values.insert(input->objectName(), false);
        }
    }
    values.insert("mwStyle", 1);
    values.insert("mwShadows", 0);
    values.insert("importedMajor", 0);
    values.insert("importedStrength", 41);
    for (auto *input : m_permanents) {
        values.insert(input->objectName(), 0);
    }
    converted.insert("format", "jx3dps.desktop.config");
    converted.insert("version", 2);
    converted.insert("specialization", 0);
    converted.insert("values", values);
    return withTeamDefaults(converted);
}

bool SimulationOptions::RestoreSnapshot(const QJsonObject &original, QString &error)
{
    const auto snapshot = NormalizeSnapshot(original);
    if (!ValidateSnapshot(snapshot, error)) {
        return false;
    }
    const auto values = snapshot.value("values").toObject();
    const auto talents = snapshot.value("talents").toObject();
    for (int index = 0; index < 2; ++index) m_talentPanels[index]->RestoreSelection(talents.value(QString::number(index)).toArray());
    for (auto *input : findChildren<QSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        input->setValue(values.value(input->objectName()).toInt());
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        input->setValue(values.value(input->objectName()).toDouble());
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (input->property("flowViewOnly").toBool()) continue;
        input->setChecked(values.value(input->objectName()).toBool());
    }
    m_seed->setText(values.value("seed").toString());
    m_flowEditor->setPlainText(snapshot.value("flowMacro").toString());
    for (auto *input : m_permanents) {
        input->setCurrentIndex(values.value(input->objectName()).toInt());
    }
    m_specialization->setCurrentIndex(snapshot.value("specialization").toInt());
    return true;
}

bool SimulationOptions::ExportConfiguration(const QString &path, QString &error) const
{
    const auto snapshot = Snapshot();
    if (!ValidateSnapshot(snapshot, error)) {
        return false;
    }
    const auto bytes = QJsonDocument(snapshot).toJson();
    QSaveFile  file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
        error = file.errorString();
        return false;
    }
    return true;
}

bool SimulationOptions::ImportConfiguration(const QString &path, QString &error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        error = file.errorString();
        return false;
    }
    if (file.size() > 1024 * 1024) {
        error = QStringLiteral("配置文件超过 1 MiB。");
        return false;
    }
    const auto bytes = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        error = file.errorString();
        return false;
    }
    QJsonParseError parse;
    const auto      document = QJsonDocument::fromJson(bytes, &parse);
    if (parse.error != QJsonParseError::NoError || !document.isObject()) {
        error = QStringLiteral("配置文件格式错误，请检查文件内容（位置 %1）。").arg(parse.offset);
        return false;
    }
    return RestoreSnapshot(document.object(), error);
}

bool SimulationOptions::ImportAttributes(const QJsonObject &attributes, QString &error)
{
    auto                              snapshot = Snapshot();
    auto                              values   = snapshot.value("values").toObject();
    const bool                        moWen    = Specialization() == desktop::Specialization::MoWen;
    const QString                     prefix   = moWen ? QStringLiteral("Lunar") : QStringLiteral("Physics");
    const QString                     majorKey = moWen ? QStringLiteral("Spirit") : QStringLiteral("Agility");
    const std::pair<QString, QString> fields[] = {
        { prefix + "AttackPowerBase",     "attackBase"       },
        { prefix + "CriticalStrike",      "crit"             },
        { prefix + "CriticalDamagePower", "critPower"        },
        { prefix + "Overcome",            "overcomeBase"     },
        { "SurplusValue",                 "surplus"          },
        { "Strain",                       "strain"           },
        { "Haste",                        "haste"            },
        { majorKey,                       "importedMajor"    },
        { "Strength",                     "importedStrength" },
        { "MeleeWeaponDamage",            "weapon"           }
    };
    auto read = [&](const QString &key, int &number) {
        const auto value = attributes.value(key);
        if (!value.isDouble() || !std::isfinite(value.toDouble()) || value.toDouble() < 0 ||
            value.toDouble() > 1000000 || std::floor(value.toDouble()) != value.toDouble())
        {
            error = QStringLiteral("魔盒属性缺少有效的整数项目：%1").arg(key);
            return false;
        }
        number = value.toInt();
        return true;
    };
    for (const auto &[source, destination] : fields) {
        int value = 0;
        if (!read(source, value)) {
            return false;
        }
        values.insert(destination, value);
    }
    int randomWeapon = 0;
    if (!read(QStringLiteral("MeleeWeaponDamageRand"), randomWeapon)) {
        return false;
    }
    values.insert("weapon", values.value("weapon").toInt() + randomWeapon / 2);
    const auto major = static_cast<value_t>(values.value("importedMajor").toInt());
    values.insert("attackClass", static_cast<int>(major * (moWen ? 1895 : 1485) / 1024));
    values.insert("overcomeClass", 0);
    for (const char *key :
         { "attackBonus", "overcomeBonus", "critBonus", "critPowerBonus", "hasteBonus", "damageBonus", "strainBonus", "ignoreShield" })
    {
        values.insert(key, 0);
    }
    values.insert("pveBonus", std::round((moWen ? 61 : 184) * 100000.0 / 1024) / 1000);
    for (auto *input : m_permanents) {
        values.insert(input->objectName(), 0);
    }
    snapshot.insert("values", values);
    return RestoreSnapshot(snapshot, error);
}

void SimulationOptions::SetupProfiles(QVBoxLayout *layout)
{
    auto *group    = layout->parentWidget();
    auto *rows     = layout;
    auto *profiles = new QComboBox(group);
    profiles->setObjectName(QStringLiteral("configurationProfile"));
    profiles->setEditable(true);
    profiles->setInsertPolicy(QComboBox::NoInsert);
    profiles->setAccessibleName(QStringLiteral("方案名称"));
    profiles->setToolTip(QStringLiteral("输入名称保存当前配置，或选择已有方案加载。"));
    profiles->lineEdit()->setMaxLength(100);
    const auto saved = QJsonDocument::fromJson(QSettings().value(QStringLiteral("simulation/profiles")).toByteArray()).object();
    profiles->addItems(saved.keys());
    profiles->setCurrentIndex(-1);
    profiles->setPlaceholderText(QStringLiteral("输入方案名称或选择已有方案"));
    profiles->lineEdit()->setPlaceholderText(QStringLiteral("输入方案名 / 选择已存方案"));
    auto placeholderPalette = profiles->lineEdit()->palette();
    placeholderPalette.setColor(QPalette::PlaceholderText, QColor(QStringLiteral("#94a5b0")));
    profiles->lineEdit()->setPalette(placeholderPalette);
    auto *buttons = new QHBoxLayout;
    buttons->addWidget(profiles, 1);
    auto  button  = [&](const QString &label, const char *name, auto callback) {
        auto *item = new QPushButton(label, group);
        item->setObjectName(QString::fromLatin1(name));
        buttons->addWidget(item);
        connect(item, &QPushButton::clicked, this, callback);
    };
    auto read = [] {
        return QJsonDocument::fromJson(QSettings().value(QStringLiteral("simulation/profiles")).toByteArray()).object();
    };
    auto write = [](const QJsonObject &data) {
        QSettings().setValue(QStringLiteral("simulation/profiles"), QJsonDocument(data).toJson(QJsonDocument::Compact));
    };
    auto report = [this](const QString &error) {
        QMessageBox::warning(this, QStringLiteral("配置"), error);
    };
    button(QStringLiteral("保存"), "saveProfile", [=, this] {
        const auto name = profiles->currentText().trimmed();
        if (name.isEmpty()) {
            report(QStringLiteral("请先输入方案名称。"));
            return;
        }
        QString    error;
        const auto snapshot = Snapshot();
        if (!ValidateSnapshot(snapshot, error)) {
            report(error);
            return;
        }
        auto data = read();
        data.insert(name, snapshot);
        write(data);
        if (profiles->findText(name) < 0) {
            profiles->addItem(name);
        }
        profiles->setCurrentText(name);
    });
    button(QStringLiteral("加载"), "loadProfile", [=, this] {
        const auto name = profiles->currentText().trimmed();
        QString    error;
        const auto data = read();
        if (!data.contains(name)) {
            report(QStringLiteral("请先选择已保存的方案。"));
            return;
        }
        if (!RestoreSnapshot(data.value(name).toObject(), error)) {
            report(error);
        }
    });
    auto *more = new QToolButton(group);
    more->setText(QStringLiteral("更多"));
    more->setPopupMode(QToolButton::InstantPopup);
    auto *menu = new QMenu(more);
    menu->setObjectName(QStringLiteral("configurationMenu"));
    more->setMenu(menu);
    buttons->addWidget(more);
    buttons->addLayout(m_runActions);
    rows->addLayout(buttons);
    auto menuAction = [&](const QString &label, const char *name, auto callback) {
        auto *item = menu->addAction(label);
        item->setObjectName(QString::fromLatin1(name));
        connect(item, &QAction::triggered, this, callback);
    };
    menuAction(QStringLiteral("删除方案"), "deleteProfile", [=, this] {
        const auto name = profiles->currentText().trimmed();
        auto       data = read();
        if (!data.contains(name)) {
            return;
        }
        if (QMessageBox::question(this, QStringLiteral("删除方案"), QStringLiteral("确定删除“%1”？").arg(name)) != QMessageBox::Yes)
        {
            return;
        }
        data.remove(name);
        write(data);
        profiles->removeItem(profiles->findText(name));
    });
    menuAction(QStringLiteral("导入配置"), "importConfiguration", [=, this] {
        const auto path = QFileDialog::getOpenFileName(this, QStringLiteral("导入配置"), {}, QStringLiteral("配置文件 (*.json)"));
        if (path.isEmpty()) {
            return;
        }
        QString error;
        if (!ImportConfiguration(path, error)) {
            report(error);
        } else {
            profiles->setCurrentIndex(-1);
        }
    });
    menuAction(QStringLiteral("导出配置"), "exportConfiguration", [=, this] {
        const auto path =
            QFileDialog::getSaveFileName(this, QStringLiteral("导出配置"), QStringLiteral("心法配置.json"), QStringLiteral("配置文件 (*.json)"));
        if (path.isEmpty()) {
            return;
        }
        QString error;
        if (!ExportConfiguration(path, error)) {
            report(error);
        }
    });
    menu->addSeparator();
}
