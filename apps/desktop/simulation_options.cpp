#include "simulation_options.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <initializer_list>
#include <utility>
#include <vector>

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
#include <QSignalBlocker>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTextBlock>
#include <QTextBrowser>
#include <QThread>
#include <QTimer>
#include <QToolButton>
#include <QToolTip>
#include <QVBoxLayout>

#include "controls/code_editor/code_editor.h"
#include "controls/equipment_panel/equipment_panel.h"
#include "controls/theme/fixed_unit_spin_box.h"
#include "controls/theme/header_group_box.h"
#include "controls/theme/segmented_tabs.h"

#include "diagnostics.h"
#include "flow_macro.h"
#include "flow_timeline_editor.h"
#include "game_attributes.h"
#include "game_icons.h"
#include "runtime_backend.h"
#include "talent_panel.h"

using namespace JX3DPS;

namespace {
QString FoodFamily(desktop::Specialization spec);
QJsonObject SchoolPanel(const runtime::season50::Book &book)
{
    const auto &a = book.default_attributes;
    return { { "attackBase", int(a.attack_base) }, { "attackClass", int(a.attack_by_class) },
             { "weapon", int(a.weapon) }, { "crit", int(a.crit) }, { "critPower", int(a.crit_power) },
             { "overcomeBase", int(a.overcome) }, { "strain", int(a.strain) }, { "surplus", int(a.surplus) },
             { "haste", int(a.haste) }, { "importedMajor", book.default_major },
             { "attackBonus", a.attack_percent * 100.0 / 1024 }, { "overcomeBonus", 0 }, { "critBonus", 0 }, { "critPowerBonus", 0 },
             { "hasteBonus", 0 }, { "damageBonus", 0 }, { "strainBonus", 0 }, { "pveBonus", 0 }, { "ignoreShield", 0 } };
}

void ApplySchoolPanel(QWidget *form, const QJsonObject &defaults, const QJsonObject &draft)
{
    for (auto it = defaults.begin(); it != defaults.end(); ++it) {
        const auto value = draft.value(it.key()).isDouble() ? draft.value(it.key()) : it.value();
        if (auto *integer = form->findChild<QSpinBox *>(it.key())) {
            integer->setProperty("defaultValue", it.value().toInt());
            integer->setValue(value.toInt());
        } else if (auto *decimal = form->findChild<QDoubleSpinBox *>(it.key())) {
            decimal->setProperty("defaultValue", it.value().toDouble());
            decimal->setValue(value.toDouble());
        }
    }
}

class ConfigurationRows final : public QFormLayout
{
public:
    explicit ConfigurationRows(QWidget *parent) : QFormLayout(parent) { }

    void SetExpanded(bool expanded)
    {
        expanded_ = expanded;
        invalidate();
    }

    void SetColumns(int columns)
    {
        columns_ = columns;
        invalidate();
    }

    void SetCompactFieldWidth(int width)
    {
        compactFieldWidth_ = width;
        invalidate();
    }

    bool HasCompactFields() const { return compactFieldWidth_ > 0; }

    QSize minimumSize() const override { return columns_ == 1 ? QFormLayout::minimumSize() : GridSize(); }

    QSize sizeHint() const override { return columns_ == 1 ? QFormLayout::sizeHint() : GridSize(); }

    void setGeometry(const QRect &rect) override
    {
        if (columns_ > 1) {
            QLayout::setGeometry(rect);
            const auto    MARGINS    = contentsMargins();
            const auto    AREA       = rect.marginsRemoved(MARGINS);
            constexpr int GUTTER     = 18;
            const int     CELL_WIDTH = (AREA.width() - (GUTTER * (columns_ - 1))) / columns_;
            const int     ROW_HEIGHT = RowHeight();
            int           y          = AREA.top();
            int           cell       = 0;
            for (int row = 0; row < rowCount(); ++row) {
                auto *field = Field(row);
                if ((field == nullptr) || ((field->widget() != nullptr) && field->widget()->isHidden())) {
                    continue;
                }
                auto *label = itemAt(row, LabelRole);
                if (label == nullptr) {
                    if (cell != 0) {
                        y    += ROW_HEIGHT + verticalSpacing();
                        cell  = 0;
                    }
                    field->setGeometry(QRect(AREA.left(), y, AREA.width(), field->sizeHint().height()));
                    y += field->sizeHint().height() + verticalSpacing();
                    continue;
                }
                const int X           = AREA.left() + (cell * (CELL_WIDTH + GUTTER));
                const int LABEL_WIDTH = label->sizeHint().width();
                label->setGeometry(QRect(X, y, LABEL_WIDTH, ROW_HEIGHT));
                const int FIELD_WIDTH = qMin(expanded_ ? 200 : CELL_WIDTH, CELL_WIDTH - LABEL_WIDTH - horizontalSpacing());
                field->setGeometry(QRect(X + CELL_WIDTH - FIELD_WIDTH, y, FIELD_WIDTH, ROW_HEIGHT));
                if (++cell == columns_) {
                    cell  = 0;
                    y    += ROW_HEIGHT + verticalSpacing();
                }
            }
            return;
        }
        if (!expanded_ && compactFieldWidth_ > 0) {
            int label_width = 0;
            int field_width = compactFieldWidth_;
            for (int row = 0; row < rowCount(); ++row) {
                if (!isRowVisible(row)) {
                    continue;
                }
                const auto *label = itemAt(row, LabelRole);
                const auto *field = itemAt(row, FieldRole);
                if ((label == nullptr) || (field == nullptr)) {
                    continue;
                }
                label_width = qMax(label_width, label->sizeHint().width());
                field_width = qMax(field_width, field->minimumSize().width());
            }
            const auto MARGINS = contentsMargins();
            const int  WIDTH =
                qMin(rect.width(), MARGINS.left() + label_width + horizontalSpacing() + field_width + MARGINS.right());
            // Center the label/input pairs together, rather than separating them across the card.
            QFormLayout::setGeometry(QRect(rect.x() + ((rect.width() - WIDTH) / 2), rect.y(), WIDTH, rect.height()));
            return;
        }
        QFormLayout::setGeometry(rect);
        if (!expanded_) {
            return;
        }
        // Right-align fields without inflating the form's minimum width on resize.
        for (int row = 0; row < rowCount(); ++row) {
            auto *field = itemAt(row, QFormLayout::FieldRole);
            if ((field == nullptr) || (itemAt(row, QFormLayout::LabelRole) == nullptr)) {
                continue;
            }
            auto bounds = field->geometry();
            bounds.setLeft(qMax(bounds.left(), bounds.right() - 199));
            field->setGeometry(bounds);
        }
    }

private:
    QLayoutItem *Field(int row) const
    {
        auto *field = itemAt(row, FieldRole);
        return (field != nullptr) ? field : itemAt(row, SpanningRole);
    }

    int RowHeight() const
    {
        int height = 0;
        for (int row = 0; row < rowCount(); ++row) {
            const auto *field = Field(row);
            if ((field != nullptr) && (itemAt(row, LabelRole) != nullptr) &&
                !((field->widget() != nullptr) && field->widget()->isHidden()))
            {
                height = qMax(height, field->sizeHint().height());
            }
        }
        return height;
    }

    QSize GridSize() const
    {
        int fields     = 0;
        int height     = 0;
        int pair_width = 0;
        int span_width = 0;
        for (int row = 0; row < rowCount(); ++row) {
            const auto *field = Field(row);
            if ((field == nullptr) || ((field->widget() != nullptr) && field->widget()->isHidden())) {
                continue;
            }
            const auto *label = itemAt(row, LabelRole);
            if (label != nullptr) {
                ++fields;
                pair_width = qMax(pair_width, label->minimumSize().width() + horizontalSpacing() + field->minimumSize().width());
            } else {
                height     += field->sizeHint().height() + verticalSpacing();
                span_width  = qMax(span_width, field->minimumSize().width());
            }
        }
        const int  LINES    = (fields + columns_ - 1) / columns_;
        const auto MARGINS  = contentsMargins();
        height             += (LINES * RowHeight()) + (qMax(0, LINES - 1) * verticalSpacing());
        return { qMax(span_width, (columns_ * pair_width) + ((columns_ - 1) * 18)) + MARGINS.left() + MARGINS.right(),
                 height + MARGINS.top() + MARGINS.bottom() };
    }

    int  columns_           = 1;
    int  compactFieldWidth_ = 0;
    bool expanded_          = false;
};

class ConfigurationForm final : public QWidget
{
public:
    explicit ConfigurationForm(QWidget *parent = nullptr) : QWidget(parent), form(new ConfigurationRows(this))
    {
        setProperty("configurationForm", true);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        form->setHorizontalSpacing(6);
        SetExpanded(false);
    }

    void SetExpanded(bool expanded)
    {
        expanded_                   = expanded;
        const int HORIZONTAL_MARGIN = expanded ? 18 : form->HasCompactFields() ? 4 : 10;
        const int VERTICAL_MARGIN   = expanded ? 10 : form->HasCompactFields() ? 2 : 4;
        form->setContentsMargins(HORIZONTAL_MARGIN, VERTICAL_MARGIN, HORIZONTAL_MARGIN, VERTICAL_MARGIN);
        form->setVerticalSpacing(expanded ? 14 : form->HasCompactFields() ? 2 : 3);
        form->SetExpanded(expanded);
        update();
    }

    ConfigurationRows *form;

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QWidget::paintEvent(event);
        if (!expanded_) {
            return;
        }
        QPainter painter(this);
        painter.setPen(QColor("#2c2c32"));
        for (int row = 0; row < form->rowCount() - 1; ++row) {
            const auto *field = form->itemAt(row, QFormLayout::FieldRole);
            if ((field == nullptr) || ((field->widget() != nullptr) && !field->widget()->isVisible())) {
                continue;
            }
            const int Y = field->geometry().bottom() + 11;
            painter.drawLine(18, Y, width() - 18, Y);
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

template <typename Input = QDoubleSpinBox>
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
    const auto GROUP = [&](const QString &title, const char *name, int row, int column, int row_span = 1, int column_span = 1) {
        auto *box = new desktop::HeaderGroupBox(title, this);
        box->setObjectName(QString::fromLatin1(name));
        auto *content = new QVBoxLayout(box);
        content->setContentsMargins(10, 6, 10, 8);
        content->setSpacing(4);
        layout->addWidget(box, row, column, row_span, column_span);
        return content;
    };
    auto *profile_layout  = GROUP(QStringLiteral("配置方案"), "configurationSchemes", 0, 0, 1, 2);
    auto *settings_layout = GROUP(QStringLiteral("模拟设置"), "runSettings", 1, 0);
    profile_layout->parentWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    profile_layout->setAlignment(Qt::AlignTop);
    m_output = GROUP(QStringLiteral("模拟结果"), "outputPanel", 3, 0, 1, 2);
    m_output->parentWidget()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    auto *permanent_layout = GROUP(QStringLiteral("常驻增益"), "permanentPanel", 1, 1);
    for (auto *box : { settings_layout->parentWidget(), permanent_layout->parentWidget() }) {
        box->setStyleSheet(QStringLiteral("QGroupBox#%1 { margin-top: %2px; } QGroupBox#%1::title { top: 5px; }")
                               .arg(box->objectName())
                               .arg(desktop::AttributeHeaderHeight + desktop::AttributeHeaderSpacing));
    }
    auto *permanent_header =
        static_cast<desktop::HeaderGroupBox *>(permanent_layout->parentWidget())->HeaderActions(desktop::AttributeHeaderHeight);
    auto *panel_mode = new desktop::SegmentedTabBar;
    panel_mode->SetSegmentHeight(desktop::AttributeHeaderHeight);
    panel_mode->setUsesScrollButtons(false);
    m_permanentsInPanel = panel_mode;
    m_permanentsInPanel->setObjectName(QStringLiteral("permanentsInPanel"));
    m_permanentsInPanel->setAccessibleName(QStringLiteral("常驻增益面板显示方式"));
    m_permanentsInPanel->addTab(QStringLiteral("计入面板"));
    m_permanentsInPanel->addTab(QStringLiteral("基础面板"));
    m_permanentsInPanel->setCurrentIndex(QSettings().value(QStringLiteral("interface/permanentsInPanel"), true).toBool() ? 0 : 1);
    const auto PANEL_HINT =
        QStringLiteral("计入面板：显示包含常驻增益的总属性，数值前的括号标出净变化（绿色增加、红色减少）。\n"
                       "基础面板：仅显示未计入常驻增益的属性。\n此切换只影响面板展示，所选常驻增益始终参与模拟；收益也"
                       "仍按实际模拟配置计算。");
    m_permanentsInPanel->setToolTip(PANEL_HINT);
    m_permanentsInPanel->setTabToolTip(0, PANEL_HINT);
    m_permanentsInPanel->setTabToolTip(1, PANEL_HINT);
    permanent_header->addWidget(m_permanentsInPanel);
    auto *raid = new QCheckBox(QStringLiteral("开荒拉满"));
    raid->setObjectName("raidFull");
    raid->setToolTip(QStringLiteral("套用本心法的副本阵眼、小药和团队增益。手选同类项目可覆盖预设。"));
    permanent_header->addWidget(raid);
    auto *settings_row = new QWidget(this);
    settings_row->setObjectName(QStringLiteral("simulationSettingsRow"));
    auto *settings_row_layout = new QHBoxLayout(settings_row);
    settings_row_layout->setContentsMargins(0, 0, 0, 0);
    settings_row_layout->setSpacing(4);
    settings_row->hide();
    settings_layout->parentWidget()->setMaximumHeight(245);
    permanent_layout->parentWidget()->setMaximumHeight(245);
    m_inputPanels = { profile_layout->parentWidget() };
    m_runActions  = new QVBoxLayout;
    m_runActions->setContentsMargins(0, 0, 0, 0);
    SetupProfiles(profile_layout);
    m_specialization = new desktop::IconComboBox(QStringLiteral("心法"), this, false);
    m_specialization->setObjectName(QStringLiteral("specialization"));
    for (const auto &book : runtime::season50::Books()) {
        const auto spec = static_cast<desktop::Specialization>(m_specialization->count());
        m_specialization->addItem(desktop::GameIcon(spec, desktop::Text(book.name)),
                                  desktop::Text(book.name) + " · " + desktop::Text(desktop::Info(spec).faction),
                                  desktop::Text(book.code));
    }
    for (int index = 0; index < m_specialization->count(); ++index) {
        m_specialization->setItemData(index, m_specialization->itemText(index) + QStringLiteral("\n点击图标切换心法"), Qt::ToolTipRole);
    }
    m_specialization->setToolTip(QStringLiteral("心法：太虚剑意 · 纯阳\n点击图标切换心法"));
    auto *specialization_row = new QHBoxLayout;
    specialization_row->setSpacing(8);
    specialization_row->addWidget(m_specialization);
    auto *specialization_name = new QLabel(m_specialization->currentText());
    specialization_name->setObjectName(QStringLiteral("selectedSpecializationName"));
    specialization_row->addWidget(specialization_name, 1);
    connect(m_specialization, &QComboBox::currentTextChanged, specialization_name, &QLabel::setText);
    settings_layout->addLayout(specialization_row);
    m_inputPanels.push_back(m_specialization);
    auto *run_content = new ConfigurationForm;
    run_content->setObjectName(QStringLiteral("runParameters"));
    auto *run = run_content->form;
    run->SetCompactFieldWidth(136);
    settings_layout->addWidget(run_content, 1);
    m_inputPanels.push_back(run_content);
    auto *flow_layout = GROUP(QStringLiteral("流程宏"), "flowMacroPanel", 2, 2);
    flow_layout->parentWidget()->setStyleSheet(
        QStringLiteral("QGroupBox#flowMacroPanel { margin-top: 32px; } QGroupBox#flowMacroPanel::title { top: 5px; }"));
    m_flowValues = desktop::FlowDefaults();
    m_flowEditor = new CodeEditor;
    m_flowEditor->setObjectName(QStringLiteral("flowMacroEditor"));
    m_flowEditor->setAccessibleName(QStringLiteral("流程宏脚本"));
    m_flowEditor->SetSyntaxType(CodeEditor::J_X3_MACRO);
    m_flowEditor->SetShowLineNumbers(false);
    m_flowEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_flowEditor->SetSyntaxValidator([](const QString &source) {
        QList<SyntaxError> errors;
        for (const auto &error : desktop::CompileFlowMacro(source).errors) {
            errors.append({ error.line, error.column, error.length, SyntaxError::SYNTAX_ERROR, error.message });
        }
        return errors;
    });
    m_flowTimeline = new desktop::FlowTimelineEditor(m_flowEditor);
    flow_layout->addWidget(m_flowTimeline, 1);
    m_flowStatus = new QLabel;
    m_flowStatus->setObjectName(QStringLiteral("flowMacroStatus"));
    m_flowStatus->setTextFormat(Qt::PlainText);
    m_flowStatus->setFixedHeight(30);
    m_flowStatus->setWordWrap(true);
    flow_layout->addWidget(m_flowStatus);
    m_flowTimeline->statusChanged = [this](const QString &text, bool error) {
        m_flowStatus->setText(text);
        m_flowStatus->setToolTip(text);
        m_flowStatus->setStyleSheet(QStringLiteral("font-size: 11px; color: %1;").arg(error ? "#f08078" : "#92929d"));
    };
    auto *flow_help = new QDialog(this);
    flow_help->setObjectName(QStringLiteral("flowMacroHelpDialog"));
    flow_help->setWindowTitle(QStringLiteral("流程宏语法 · 时间轴"));
    flow_help->resize(650, 500);
    auto *flow_help_layout = new QVBoxLayout(flow_help);
    auto *flow_help_text   = new QTextBrowser;
    flow_help_text->setHtml(
        QStringLiteral(
            "<h2>流程宏与时间轴</h2><p>无时间前缀或 0 秒节点用于每场开战前初始化，随后运行右侧循环宏。"
            "省略的参数采用默认值，分行重复赋值以后面的值为准。支持 # 和 // 注释。</p><pre>%1</pre>"
            "<p><b>/set_target</b>：level 等级、shield 防御、life 气血、distance 距离、vulnerable 易伤。"
            "life 和 vulnerable 建议带百分号；不带百分号时按比例解释，例如 life=0.5 等于 life=50%。</p>"
            "<p><b>/set_player</b> 可用 life 设置自身气血，hit 按最大气血百分比扣除一次伤害。qidian 设置太虚、紫霞的初始气点，范围 0 至 10。</p><p>一行可写多个 "
            "参数=值；等号两侧不要留空格。"
            "可使用 00:00 或 00:00.00 前缀表示开场。当前只有一个目标，id 仅接受 1。</p>"
            "<p>战中可设置目标气血和距离、自身气血和单次受击。同一行先设置气血再扣除伤害。</p><pre>00:30 /set_target life=50%\n00:45.0625 /set_target "
            "distance=3\n00:50 /set_player hit=31%</pre>"
            "<p>时间格式为 分:秒，最多四位小数，精度 1/16 秒。节点按时间执行，相同时间按源码顺序执行。"
            "等级、防御、易伤和角色初始资源只能在 0 秒设置。距离沿用技能现有的判距规则，不自动取消读条或持续伤害。</p>"
            "<p>时间轴支持双击添加/编辑、拖动调整时间、滚轮缩放和空白处拖动平移；修改与脚本同步，可撤销。"
            "最多 256 个节点，时间不超过 24 小时。战斗区间不包含结束帧，位于结束处或之后的节点标灰且不会执行。</p>"
            "<p><b>尚不支持多目标、切换目标、等待或停止指令。</b>这些内容会报错并阻止模拟，不会被忽略。"
            "流程宏不设置技能断点，技能循环与断点调试仍在循环宏中。</p>")
            .arg(desktop::FlowMacroFromValues(m_flowValues).toHtmlEscaped()));
    flow_help_layout->addWidget(flow_help_text);
    auto *help = new QToolButton;
    help->setObjectName(QStringLiteral("openFlowMacroHelp"));
    help->setText(QStringLiteral("语法…"));
    auto *flow_actions =
        static_cast<desktop::HeaderGroupBox *>(flow_layout->parentWidget())->HeaderActions(desktop::AttributeHeaderHeight);
    auto *expand_flow = new QToolButton;
    expand_flow->setObjectName(QStringLiteral("openFlowTimeline"));
    expand_flow->setText(QStringLiteral("展开…"));
    flow_actions->addWidget(expand_flow);
    flow_actions->addWidget(help);
    connect(expand_flow, &QToolButton::clicked, m_flowTimeline, &desktop::FlowTimelineEditor::OpenEditor);
    connect(help, &QToolButton::clicked, flow_help, [flow_help] {
        flow_help->show();
        flow_help->raise();
        flow_help->activateWindow();
    });
    auto *tabs = new desktop::SegmentedTabs(this);
    tabs->setObjectName(QStringLiteral("configurationTabs"));
    static_cast<desktop::SegmentedTabBar *>(tabs->tabBar())->SetSegmentHeight(desktop::AttributeHeaderHeight);
    tabs->setStyleSheet(QStringLiteral("QTabWidget#configurationTabs::pane { top: %1px; }").arg(desktop::AttributeHeaderSpacing));
    m_inputPanels.push_back(tabs);
    const auto PAGE = [&](QTabWidget *owner, const QString &title) {
        auto *content = new ConfigurationForm;
        auto *form    = content->form;
        auto *scroll  = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidget(content);
        owner->addTab(scroll, title);
        return form;
    };
    m_attributeEditor = new QDialog(this);
    m_attributeEditor->setObjectName(QStringLiteral("attributeValueEditor"));
    m_attributeEditor->setWindowTitle(QStringLiteral("属性输入 · 50级"));
    m_attributeEditor->setWindowModality(Qt::WindowModal);
    m_attributeEditor->resize(540, 510);
    auto *attribute_editor_layout = new QVBoxLayout(m_attributeEditor);
    auto *attribute_note =
        new QLabel(QStringLiteral("这里编辑等级、基础数值和额外加成，主面板显示折算后的结果。修改即时生效。"));
    attribute_note->setWordWrap(true);
    attribute_editor_layout->addWidget(attribute_note);
    auto *editor_tabs = new desktop::SegmentedTabs(m_attributeEditor);
    editor_tabs->setObjectName(QStringLiteral("attributeEditorTabs"));
    attribute_editor_layout->addWidget(editor_tabs, 1);
    auto *attributes = PAGE(editor_tabs, QStringLiteral("数值"));
    auto *bonuses    = PAGE(editor_tabs, QStringLiteral("加成"));
    attributes->SetColumns(1);
    bonuses->SetColumns(1);
    editor_tabs->widget(1)->setObjectName(QStringLiteral("bonusesScroll"));
    m_inputPanels.push_back(attributes->parentWidget());
    m_inputPanels.push_back(bonuses->parentWidget());
    auto *attribute_buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    attribute_buttons->button(QDialogButtonBox::Close)->setText(QStringLiteral("关闭"));
    attribute_editor_layout->addWidget(attribute_buttons);
    connect(attribute_buttons, &QDialogButtonBox::rejected, m_attributeEditor, &QDialog::close);
    auto *attribute_scroll = new QScrollArea(this);
    attribute_scroll->setObjectName(QStringLiteral("attributesScroll"));
    attribute_scroll->setWidgetResizable(true);
    attribute_scroll->setFrameShape(QFrame::NoFrame);
    m_attributePane  = attribute_scroll;
    m_gameAttributes = new desktop::GameAttributePanel;
    attribute_scroll->setWidget(m_gameAttributes);
    m_gameGains = new desktop::AttributeGainsPanel(this);
    m_inputPanels.push_back(m_gameGains->calculate);
    const auto HIGHLIGHT_ATTRIBUTE = [this](int index) {
        m_gameAttributes->HighlightRow(index);
        m_gameGains->HighlightRow(index);
    };
    m_gameAttributes->hoverChanged = HIGHLIGHT_ATTRIBUTE;
    m_gameGains->hoverChanged      = HIGHLIGHT_ATTRIBUTE;
    m_inputPanels.push_back(m_gameAttributes->edit);
    connect(m_gameAttributes->edit, &QPushButton::clicked, this, [this] {
        m_attributeEditor->show();
        m_attributeEditor->raise();
    });
    m_equipmentDialog = new QDialog(this);
    m_equipmentDialog->setObjectName(QStringLiteral("equipmentDialog"));
    m_equipmentDialog->resize(720, 680);
    m_equipmentDialog->setModal(false);
    auto *equipment_dialog_layout = new QVBoxLayout(m_equipmentDialog);
    connect(m_gameAttributes->equipment, &QToolButton::clicked, this, &SimulationOptions::ShowEquipment);
    auto *equipmentScroll = new QScrollArea(m_equipmentDialog);
    equipmentScroll->setObjectName("equipmentScroll");
    equipmentScroll->setWidgetResizable(true);
    auto *preview = new EquipmentPanel;
    equipmentScroll->setWidget(preview);
    equipment_dialog_layout->addWidget(equipmentScroll, 1);
    auto *equipment_stack = new QStackedWidget;
    equipment_dialog_layout->addWidget(equipment_stack);
    m_inputPanels.push_back(equipment_stack);
    auto *equipment_buttons = new QDialogButtonBox(QDialogButtonBox::Close);
    equipment_dialog_layout->addWidget(equipment_buttons);
    connect(equipment_buttons, &QDialogButtonBox::rejected, m_equipmentDialog, &QDialog::close);
    auto *talent_stack   = new QStackedWidget;
    auto *talent_headers = new QStackedWidget;
    talent_headers->setObjectName(QStringLiteral("talentHeaders"));
    talent_headers->setFixedHeight(desktop::AttributeHeaderHeight);
    tabs->setCornerWidget(talent_headers, Qt::TopRightCorner);
    connect(tabs, &QTabWidget::currentChanged, talent_headers, [talent_headers](int index) {
        talent_headers->setVisible(index == 0);
    });
    connect(tabs, &QTabWidget::currentChanged, this, [this] { UpdateTalentHeight(); });
    auto *recipe_stack = new QStackedWidget;
    tabs->addTab(talent_stack, QStringLiteral("奇穴"));
    tabs->addTab(recipe_stack, QStringLiteral("秘籍"));
    const auto EFFECT_PAGE = [](QStackedWidget *owner) {
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
    for (int index = 0; index < m_specialization->count(); ++index) {
        const auto  spec  = static_cast<desktop::Specialization>(index);
        const auto &book  = desktop::School(spec);
        const auto  code  = desktop::Text(book.code);
        auto       *panel = new desktop::TalentPanel(spec);
        m_talentPanels.push_back(panel);
        panel->SetColumns(9);
        panel->RestoreSelection(panel->DefaultSelection());
        panel->setObjectName(QStringLiteral("talents%1").arg(index));
        panel->setProperty("schoolCode", code);
        talent_headers->addWidget(panel->TakeHeader());
        auto *scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setObjectName(QStringLiteral("talentsScroll%1").arg(index));
        scroll->setWidget(panel);
        talent_stack->addWidget(scroll);
        auto                        *recipe_form = EFFECT_PAGE(recipe_stack);
        auto                        *gear_form   = EFFECT_PAGE(equipment_stack);
        QMap<QString, QGridLayout *> recipe_groups;
        for (auto name : book.recipes) {
            auto binding = std::ranges::find(book.recipe_groups, name, &std::pair<std::string_view, std::string_view>::first);
            auto groupName = binding == book.recipe_groups.end() ? desktop::Text(name) : desktop::Text(binding->second);
            if (!recipe_groups.contains(groupName)) {
                auto *group              = new QGroupBox(groupName);
                auto *grid               = new QGridLayout(group);
                recipe_groups[groupName] = grid;
                recipe_form->addRow(group);
            }
            auto *input = new QCheckBox(desktop::Text(name).section("·", -1));
            if (std::ranges::contains(book.preset_only_recipes, name)) {
                input->setToolTip(QStringLiteral("方案中保留此秘籍，当前木桩模拟尚未结算此效果。"));
            }
            if (auto info = book.single_target_recipe_notes.find(name); info != book.single_target_recipe_notes.end()) {
                input->setToolTip(desktop::Text(info->second));
            }
            input->setObjectName("recipe/" + code + "/" + desktop::Text(name));
            input->setChecked(std::ranges::find(book.default_recipes, name) != book.default_recipes.end());
            input->setProperty("defaultChecked", input->isChecked());
            auto *grid = recipe_groups[groupName];
            int   pos  = grid->count();
            grid->addWidget(input, pos / 3, pos % 3);
            connect(input, &QCheckBox::toggled, this, [grid, input](bool on) {
                if (!on) {
                    return;
                }
                int count = 0;
                for (int i = 0; i < grid->count(); ++i) {
                    count += static_cast<QCheckBox *>(grid->itemAt(i)->widget())->isChecked();
                }
                if (count > 4) {
                    input->setChecked(false);
                    QToolTip::showText(input->mapToGlobal(QPoint(0, 0)), QStringLiteral("每个技能最多选择四本秘籍"), input);
                }
            });
        }
        if (book.recipes.empty()) {
            recipe_form->addRow(new QLabel(QStringLiteral("此心法暂无已接入的秘籍")));
        }
        for (auto name : book.equipment) {
            auto *box = new QCheckBox(desktop::Text(name));
            box->setObjectName("equipment/" + code + "/" + desktop::Text(name));
            box->setChecked(std::ranges::find(book.default_equipment, name) != book.default_equipment.end());
            box->setProperty("defaultChecked", box->isChecked());
            const auto slot = runtime::season50::EquipmentSlot(book, name);
            if (!slot.empty()) {
                box->setProperty("enchantSlot", desktop::Text(slot));
                connect(box, &QCheckBox::toggled, this, [box, gear_form](bool on) {
                    if (!on) return;
                    const auto slot = box->property("enchantSlot");
                    QList<QCheckBox *> others;
                    for (auto *other : gear_form->parentWidget()->findChildren<QCheckBox *>()) {
                        if (other != box && other->isChecked() && other->property("enchantSlot") == slot) others.append(other);
                    }
                    const int capacity = slot.toString() == QStringLiteral("戒指") ? 2 : 1;
                    while (others.size() >= capacity) others.takeFirst()->setChecked(false);
                });
            }
            gear_form->addRow(box);
        }
        if (book.equipment.empty()) {
            gear_form->addRow(new QLabel(QStringLiteral("此心法暂无已接入的装备特效")));
        } else if (!book.default_equipment.empty()) {
            auto *restore = new QPushButton(QStringLiteral("使用本心法默认特效"));
            restore->setObjectName("restoreGear/" + code);
            gear_form->addRow(restore);
            connect(restore, &QPushButton::clicked, this, [gear_form] {
                for (auto *box : gear_form->parentWidget()->findChildren<QCheckBox *>())
                    box->setChecked(box->property("defaultChecked").toBool());
            });
        }
    }
    m_iterations       = Integer(run, "iterations", QStringLiteral("模拟次数"), 1000, 1, 1000000000);
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
    const auto seed_hint = [this] {
        m_seed->setToolTip(
            QStringLiteral("当前种子：%1\n相同种子可复现相同战斗；调试和单次记录复现其中第一场。").arg(m_seed->text()));
    };
    connect(m_seed, &QLineEdit::textChanged, this, seed_hint);
    seed_hint();
    run->addRow(QStringLiteral("随机种子"), m_seed);
    Integer(run, "delay", QStringLiteral("输入延迟"), 0, 0, 160)->setSuffix(QStringLiteral(" 帧"));
    // Native range-based size hints must not widen this small, non-scrolling form.
    for (int row = 0; row < run->rowCount(); ++row) {
        run->itemAt(row, QFormLayout::FieldRole)->widget()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    }

    const struct
    {
        const char *key;
        const char *label;
    } baseFields[] = {
        { "attackBase", "基础攻击" }, { "attackClass", "心法攻击" }, { "weapon", "武器伤害" },
        { "crit", "会心等级" }, { "critPower", "会效等级" }, { "overcomeBase", "基础破防" },
        { "strain", "无双等级" }, { "surplus", "破招等级" }, { "haste", "加速等级" }, { "importedMajor", "已计入的主属性" }
    };
    const auto initialPanel = SchoolPanel(desktop::School(Specialization()));
    for (const auto &field : baseFields) {
        Integer(attributes, field.key, QString::fromUtf8(field.label), initialPanel[field.key].toInt(), 0, 1000000);
    }
    for (const auto &[key, label] :
         std::initializer_list<std::pair<const char *, const char *>>{
             { "attackBonus",    "攻击加成"       },
             { "overcomeBonus",  "破防加成"       },
             { "critBonus",      "会心几率加成"   },
             { "critPowerBonus", "会效加成"       },
             { "hasteBonus",     "加速加成"       },
             { "damageBonus",    "伤害加成"       },
             { "strainBonus",    "无双加成"       },
             { "pveBonus",       "非侠士伤害加成" },
             { "ignoreShield",   "无视防御"       }
    })
    {
        Decimal(bonuses, key, QString::fromUtf8(label), 0, 0, QString::fromLatin1(key) == "ignoreShield" ? 100 : 976.562, 3)
            ->setSuffix(" %");
    }
    auto *permanent_scroll = new QScrollArea;
    permanent_scroll->setObjectName(QStringLiteral("permanentsScroll"));
    m_inputPanels.push_back(permanent_scroll);
    permanent_scroll->setWidgetResizable(true);
    permanent_scroll->setFrameShape(QFrame::NoFrame);
    auto *permanent_stack = new QStackedWidget;
    permanent_scroll->setWidget(permanent_stack);
    permanent_layout->addWidget(permanent_scroll);
    QFile permanent_file(":/desktop/permanents50.json");
    if (!permanent_file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("missing level-50 consumables");
    }
    const auto        consumables = QJsonDocument::fromJson(permanent_file.readAll()).object();
    const QStringList categories{ "阵法",     "食品增强", "食品辅助", "药品增强", "药品辅助",
                                  "家园菜品", "家园酒品", "武器磨石", "团队宴席", "家园香水" };
    for (const auto &family : QStringList{ "外功", "内功", "天罗诡道" }) {
        auto *content = new QWidget;
        content->setProperty("permanentChoices", true);
        auto *grid = new QGridLayout(content);
        grid->setContentsMargins(0, 0, 0, 0);
        grid->setSpacing(4);
        permanent_stack->addWidget(content);
        for (const auto &category : categories) {
            auto entries = consumables[family].toObject()[category].toArray();
            if (entries.isEmpty()) {
                continue;
            }
            auto *cell = new QWidget;
            auto *row  = new QHBoxLayout(cell);
            row->setContentsMargins(0, 0, 0, 0);
            auto *combo = new desktop::IconComboBox(category);
            combo->setObjectName("permanent/" + family + "/" + category);
            combo->setFixedSize(38, 38);
            combo->setProperty("family", family);
            combo->addItem(QStringLiteral("无"), QJsonObject{});
            for (const auto &v : entries) {
                auto e = v.toObject();
                combo->addItem(desktop::GameIcon(e["icon"].toInt()), e["name"].toString(), e["stats"].toObject());
            }
            row->setSpacing(6);
            row->addWidget(combo);
            auto *caption = new QLabel(category);
            caption->setObjectName(combo->objectName() + "Caption");
            caption->setStyleSheet("font-size:11px; color:#92929d");
            row->addWidget(caption);
            const auto pos = grid->count();
            grid->addWidget(cell, pos / 3, pos % 3);
            m_permanents.push_back(combo);
        }
    }
    auto *import_attributes = new QPushButton(QStringLiteral("导入魔盒属性"));
    import_attributes->setObjectName(QStringLiteral("importAttributes"));
    import_attributes->setProperty("moduleAction", true);
    m_gameAttributes->tools->insertWidget(0, import_attributes, 1);
    m_inputPanels.push_back(import_attributes);
    connect(import_attributes, &QPushButton::clicked, this, [this] {
        QDialog dialog(this);
        dialog.setWindowTitle(QStringLiteral("导入魔盒属性"));
        dialog.resize(620, 480);
        auto *rows = new QVBoxLayout(&dialog);
        auto *note = new QLabel(QStringLiteral("粘贴魔盒导出的属性对象。按当前心法读取对应伤害类型的属性；导入会替换属"
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
            const auto      JSON = QJsonDocument::fromJson(text->toPlainText().toUtf8(), &parse);
            QString         error;
            if (parse.error != QJsonParseError::NoError || !JSON.isObject()) {
                status->setText(QStringLiteral("请输入有效的属性对象。"));
            } else if (!ImportAttributes(JSON.object(), error)) {
                status->setText(error);
            } else {
                dialog.accept();
            }
        });
        dialog.exec();
    });
    const auto updateSchool = [=, this] {
        auto spec  = Specialization();
        int  index = static_cast<int>(spec);
        const auto code = m_specialization->currentData().toString();
        if (m_panelSchool != code) {
            const auto defaults = SchoolPanel(desktop::School(spec));
            if (!m_panelSchool.isEmpty()) {
                const auto values = Values();
                QJsonObject panel;
                for (auto it = defaults.begin(); it != defaults.end(); ++it) panel.insert(it.key(), values[it.key()]);
                m_panelDrafts.insert(m_panelSchool, panel);
            }
            m_panelSchool = code;
            ApplySchoolPanel(this, defaults, m_panelDrafts.value(code).toObject());
        }
        m_equipmentDialog->setWindowTitle(desktop::ClassName(spec) + " · 配装");
        talent_stack->setCurrentIndex(index);
        talent_headers->setCurrentIndex(index);
        recipe_stack->setCurrentIndex(index);
        equipment_stack->setCurrentIndex(index);
        permanent_stack->setCurrentIndex(FoodFamily(spec) == "天罗诡道"      ? 2
                                         : desktop::Info(spec).damage == "Physics" ? 0
                                                                                   : 1);
        if (m_equipmentDialog->isVisible()) {
            ShowEquipment();
        }
        m_specialization->setToolTip(m_specialization->currentText());
        UpdateTalentHeight();
        emit SpecializationChanged();
    };
    connect(m_specialization, &QComboBox::currentIndexChanged, this, updateSchool);
    QSettings SETTINGS;
    m_panelDrafts = QJsonDocument::fromJson(SETTINGS.value("simulation/schoolPanels").toByteArray()).object();
    updateSchool();
    m_flowEditor->setPlainText(desktop::FlowMacroFromValues(desktop::FlowDefaults()));
    const auto saved = SETTINGS.value("simulation/season50").toByteArray();
    if (!saved.isEmpty()) {
        QString error;
        auto snapshot = QJsonDocument::fromJson(saved).object();
        auto values = snapshot.value("values").toObject();
        const int index = m_specialization->findData(snapshot.value("specialization").toString());
        if (!SETTINGS.contains("simulation/schoolPanels") && index >= 0 && snapshot["version"] == 5) {
            const auto defaults = SchoolPanel(desktop::School(static_cast<desktop::Specialization>(index)));
            bool placeholder = true;
            for (auto it = defaults.begin(); it != defaults.end(); ++it)
                placeholder &= values[it.key()].isDouble() && values[it.key()].toDouble() == (it.key() == "attackBase" ? 10000 : 0);
            if (placeholder) {
                for (auto it = defaults.begin(); it != defaults.end(); ++it) values[it.key()] = it.value();
                for (const auto &school : runtime::season50::Books()) {
                    const auto prefix = "recipe/" + desktop::Text(school.code) + "/";
                    const bool chosen = std::ranges::any_of(school.recipes, [&](auto recipe) {
                        return values[prefix + desktop::Text(recipe)].toBool();
                    });
                    if (!chosen) {
                        for (auto recipe : school.default_recipes) values[prefix + desktop::Text(recipe)] = true;
                    }
                }
                snapshot["values"] = values;
            }
        }
        if (!RestoreSnapshot(snapshot, error)) {
            qWarning() << "Invalid saved 50-level configuration:" << error;
        }
    }
    if (saved.isEmpty()) {
        m_specialization->setCurrentIndex(SETTINGS.value("simulation/specialization", 0).toInt() == 1 ? 1 : 0);
    }
    if (SETTINGS.contains("simulation/season50FlowDraft")) {
        m_flowEditor->setPlainText(SETTINGS.value("simulation/season50FlowDraft").toString());
    }
    updateSchool();
    auto *reset = findChild<QMenu *>("configurationMenu")->addAction(QStringLiteral("重置配置"));
    reset->setObjectName("resetConfiguration");
    connect(reset, &QAction::triggered, this, [this] {
        for (auto *panel : m_talentPanels) {
            panel->RestoreSelection(panel->DefaultSelection());
        }
        for (auto *input : findChildren<QSpinBox *>()) {
            if (input->property("defaultValue").isValid()) {
                input->setValue(input->property("defaultValue").toInt());
            }
        }
        for (auto *input : findChildren<QDoubleSpinBox *>()) {
            if (input->property("defaultValue").isValid()) {
                input->setValue(input->property("defaultValue").toDouble());
            }
        }
        for (auto *input : findChildren<QCheckBox *>()) {
            if (!input->property("flowViewOnly").toBool()) {
                const QSignalBlocker blocker(input->property("defaultChecked").isValid() ? input : nullptr);
                input->setChecked(input->property("defaultChecked").toBool());
            }
        }
        for (auto *cb : m_permanents) {
            cb->setCurrentIndex(0);
        }
        m_flowEditor->setPlainText(desktop::FlowMacroFromValues(desktop::FlowDefaults()));
        m_seed->setText("0");
    });
    layout->setColumnStretch(0, 1);
    layout->setRowMinimumHeight(1, 235);
    layout->setRowStretch(2, 1);
    const auto REFRESH = [this] {
        if (m_refreshQueued) {
            return;
        }
        m_refreshQueued = true;
        QTimer::singleShot(0, this, [this] {
            m_refreshQueued = false;
            RefreshGameAttributes();
        });
    };
    for (const auto *input : findChildren<QSpinBox *>()) {
        connect(input, &QSpinBox::valueChanged, this, REFRESH);
    }
    for (const auto *input : findChildren<QDoubleSpinBox *>()) {
        connect(input, &QDoubleSpinBox::valueChanged, this, REFRESH);
    }
    for (const auto *input : findChildren<QComboBox *>()) {
        connect(input, &QComboBox::currentIndexChanged, this, REFRESH);
    }
    for (const auto *input : findChildren<QCheckBox *>()) {
        connect(input, &QCheckBox::toggled, this, REFRESH);
    }
    for (auto *panel : m_talentPanels) {
        connect(panel, &desktop::TalentPanel::SelectionChanged, this, REFRESH);
    }
    connect(m_seed, &QLineEdit::textChanged, this, REFRESH);
    connect(m_permanentsInPanel, &QTabBar::currentChanged, this, [this](int index) {
        QSettings().setValue(QStringLiteral("interface/permanentsInPanel"), index == 0);
        RefreshGameAttributes();
    });
    connect(m_flowEditor, &QPlainTextEdit::textChanged, this, &SimulationOptions::UpdateFlowMacro);
    connect(m_seconds, &QDoubleSpinBox::valueChanged, this, [this](double seconds) {
        m_flowTimeline->SetDuration(qRound(seconds * JX3DPS::FRAMES_PER_SECOND));
    });
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
    const auto VALUES   = Values();
    const bool INCLUDED = m_permanentsInPanel->currentIndex() == 0;
    try {
        const auto READ_PANEL = [&](const QJsonObject &values) -> std::array<double, 10> {
            auto cfg = ConfigFromValues(values, Specialization(), {});
            auto &a = cfg.attributes;
            const double major = cfg.raid_full ? runtime::season50::ApplyPermanents(*cfg.book, a, *cfg.base_major, cfg.permanents, true)
                                               : values["importedMajor"].toDouble() + PermanentMajor(values, Specialization());
            return { major,
                     double(a.attack_base * (1024 + a.attack_percent) / 1024 + a.attack_by_class),
                     100.,
                     std::min(100., 100 * a.crit / (9.609 * 990) + a.crit_basis_points / 100.),
                     175. + 100 * std::min(1280, runtime::season50::Rating(a.crit_power, 3.54) + a.crit_power_percent) / 1024.,
                     100. * std::min(256, runtime::season50::Rating(a.haste, 10.21) + a.haste_percent) / 1024.,
                     100. * runtime::season50::Rating(a.overcome * (1024 + a.overcome_percent) / 1024, 10.483) / 1024.,
                     100. * (runtime::season50::Rating(a.strain, 7.117) + a.strain_percent) / 1024.,
                     double(a.surplus),
                     double(a.weapon) };
        };
        // Clear only permanent selections in a copy; both projections retain manual bonuses and integer rounding.
        auto base_values = VALUES;
        base_values.insert("raidFull", false);
        const auto defaults = runtime::season50::FullRaidPermanents(desktop::School(Specialization()));
        for (const auto *input : m_permanents) {
            base_values.insert(input->objectName(), 0);
        }
        for (auto *input : m_permanents) {
            if (input->property("family").toString() != FoodFamily(Specialization())) continue;
            const auto &name = defaults.at(input->objectName().section('/', -1).toStdString());
            const auto label = VALUES["raidFull"].toBool() ? desktop::Text(name) : QStringLiteral("无");
            if (input->itemText(0) != label) {
                const QSignalBlocker blocker(input);
                input->setItemText(0, label);
            }
        }
        const auto BASE    = READ_PANEL(base_values);
        const auto current = INCLUDED ? READ_PANEL(VALUES) : BASE;
        for (int i = 0; std::cmp_less(i, panel.size()); ++i) {
            const bool PERCENTAGE = i >= 2 && i <= 7;
            const int  DECIMALS   = PERCENTAGE ? 2 : 0;
            const auto SUFFIX     = PERCENTAGE ? QStringLiteral("%") : QString{};
            panel[i]              = i == 0 && VALUES.value("importedMajor").toInt() <= 0
                                        ? QStringLiteral("--")
                                        : QString::number(current[i], 'f', DECIMALS) + SUFFIX;
            const double SCALE    = PERCENTAGE ? 100 : 1;
            const auto   DELTA    = (std::round(current[i] * SCALE) - std::round(BASE[i] * SCALE)) / SCALE;
            if (INCLUDED && DELTA != 0) {
                changes[i] = (DELTA > 0 ? QStringLiteral("+") : QString{}) + QString::number(DELTA, 'f', DECIMALS) + SUFFIX;
            }
        }
        m_gameAttributes->setToolTip({});
    } catch (const std::exception &error) {
        m_gameAttributes->setToolTip(QString::fromUtf8(error.what()));
    }
    m_gameAttributes->SetValues(Specialization(), panel, changes, INCLUDED);
    if (m_gainSnapshot.isEmpty() || m_gainSnapshot != Snapshot()) {
        ClearAttributeGains();
    }
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
    const auto COMPILED = desktop::CompileFlowMacro(m_flowEditor->toPlainText());
    m_flowEditor->setFocus();
    if (COMPILED.errors.isEmpty()) {
        return;
    }
    const auto &error = COMPILED.errors.front();
    const auto  BLOCK = m_flowEditor->document()->findBlockByNumber(error.line - 1);
    if (!BLOCK.isValid()) {
        return;
    }
    QTextCursor cursor(BLOCK);
    cursor.setPosition(BLOCK.position() + error.column);
    cursor.setPosition(qMin(BLOCK.position() + error.column + error.length, BLOCK.position() + BLOCK.length() - 1),
                       QTextCursor::KeepAnchor);
    m_flowEditor->setTextCursor(cursor);
    m_flowEditor->ensureCursorVisible();
}

void SimulationOptions::UpdateFlowMacro()
{
    const auto SOURCE = m_flowEditor->toPlainText();
    // Syntax highlighting and theme changes also emit textChanged; only source edits change configuration.
    if (m_flowInitialized && SOURCE == m_flowSource) {
        return;
    }
    m_flowInitialized   = true;
    m_flowSource        = SOURCE;
    const auto COMPILED = desktop::CompileFlowMacro(SOURCE);
    if (COMPILED.errors.isEmpty()) {
        m_flowValues = COMPILED.values;
        m_flowEvents = COMPILED.TargetEvents();
    }
    RefreshGameAttributes();
}

void SimulationOptions::UpdateTalentHeight()
{
    if (m_section != QStringLiteral("simulation") && m_section != QStringLiteral("attributes")) {
        return;
    }
    if (m_talentPanels.empty()) {
        return;
    }
    auto *tabs = findChild<QTabWidget *>(QStringLiteral("configurationTabs"));

    tabs->setFixedHeight(tabs->currentIndex() == 0 ? 100 : 270);
}

void SimulationOptions::SetAttributeGainResult(const SimulationResult &result)
{
    if (m_gainSnapshot != Snapshot()) {
        m_gameGains->Clear(Specialization());
    }
    m_gainSnapshot = result.gainsRequested ? Snapshot() : QJsonObject{};
    m_gameGains->SetResult(result, Specialization());
}

desktop::GainComparisonMode SimulationOptions::GainMode() const
{
    return m_gameGains->Mode();
}

bool SimulationOptions::AutoCalculateGains() const
{
    return m_gameGains->calculate->isChecked();
}

void SimulationOptions::ClearAttributeGains()
{
    m_gainSnapshot = {};
    m_gameGains->Clear(Specialization());
}

void SimulationOptions::SetInputsEnabled(bool enabled)
{
    if (!enabled) {
        m_attributeEditor->hide();
    }
    m_flowTimeline->SetReadOnly(!enabled);
    for (auto *panel : m_inputPanels) {
        panel->setEnabled(enabled);
    }
}

void SimulationOptions::ShowAttributeGains()
{
    m_gameGains->calculate->setFocus(Qt::OtherFocusReason);
}

QToolButton *SimulationOptions::GainsButton() const
{
    return m_gameGains->calculate;
}

void SimulationOptions::ShowEquipment()
{
    const auto  spec = Specialization();
    const auto &book = desktop::School(spec);
    auto        info = desktop::Info(spec);
    QJsonArray  skills, recipes;
    for (auto slot : book.castable) {
        skills.append(QJsonObject{
            { "skillName", desktop::Text(book.skills[slot].name) }
        });
    }
    for (auto name : book.recipes) {
        recipes.append(QJsonObject{
            { "name", desktop::Text(name) }
        });
    }
    const auto groups = desktop::TalentPanel::Profile(spec).value("groups").toArray();
    QJsonArray tiers;
    for (int i = 0; i < groups.size(); ++i) {
        QJsonArray entries;
        for (const auto &v : groups[i].toObject()["choices"].toArray()) {
            auto t     = v.toObject();
            t["level"] = i + 1;
            t["icon"]  = QJsonObject{
                { "FileName", QString::number(t["icon"].toInt()) + ".png" }
            };
            entries.append(t);
        }
        tiers.append(QJsonObject{
            { "kungfuSkills", entries }
        });
    }
    QJsonObject ref{
        { "skill",          QJsonObject{ { "remarks", QJsonArray{ QJsonObject{ { "forceSkills", skills } } } } } },
        { "talent",         QJsonObject{ { "kungfuLevel", tiers } }                                              },
        { "recipe_catalog", recipes                                                                              }
    };
    m_equipmentDialog->findChild<EquipmentPanel *>()->SetSeasonSchool(
        desktop::Text(book.name),
        desktop::Text(info.faction),
        desktop::Text(info.major),
        info.damage == "Physics" ? "外功" : "内功",
        ref);
    m_equipmentDialog->show();
    m_equipmentDialog->raise();
    m_equipmentDialog->activateWindow();
}

void SimulationOptions::CloseAuxiliaryWindows()
{
    for (auto *dialog : findChildren<QDialog *>()) {
        dialog->close();
    }
}

QWidget *SimulationOptions::TakeOutputPanel()
{
    m_externalOutput = true;
    auto *panel      = m_output->parentWidget();
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
    const bool EXPANDED = section != QStringLiteral("simulation");
    setStyleSheet(QStringLiteral("QWidget { font-size: %1px; } QWidget#simulationOptions { background: transparent; }").arg(EXPANDED ? 14 : 13));
    if (!EXPANDED) {
        setStyleSheet(
            styleSheet() +
            QStringLiteral("QComboBox, QSpinBox, QDoubleSpinBox, QLineEdit { min-height: 20px; padding: 1px 6px; }"));
    }
    for (auto *widget : findChildren<QWidget *>()) {
        if (widget->property("configurationForm").toBool()) {
            static_cast<ConfigurationForm *>(widget)->SetExpanded(EXPANDED);
        }
        if (widget->property("permanentChoices").toBool()) {
            auto *choices = static_cast<QGridLayout *>(widget->layout());
            choices->setAlignment(EXPANDED ? Qt::AlignTop | Qt::AlignLeft : Qt::AlignTop);
            for (int column = 0; column < 3; ++column) {
                choices->setColumnStretch(column, EXPANDED ? 0 : 1);
            }
        }
    }
    auto *import = findChild<QPushButton *>(QStringLiteral("importAttributes"));
    import->setMaximumWidth(EXPANDED ? 200 : QWIDGETSIZE_MAX);
    auto *grid         = qobject_cast<QGridLayout *>(layout());
    auto *profiles     = findChild<QWidget *>(QStringLiteral("configurationSchemes"));
    auto *run          = findChild<QWidget *>(QStringLiteral("runSettings"));
    auto *permanents   = findChild<QWidget *>(QStringLiteral("permanentPanel"));
    auto *settings_row = findChild<QWidget *>(QStringLiteral("simulationSettingsRow"));
    auto *talents      = findChild<QWidget *>(QStringLiteral("configurationTabs"));
    auto *flow         = findChild<QWidget *>(QStringLiteral("flowMacroPanel"));
    auto *output       = m_output->parentWidget();
    for (auto *panel : { profiles, run, permanents, m_attributePane, static_cast<QWidget *>(m_gameGains), flow, talents, output })
    {
        if (panel == output && m_externalOutput) {
            continue;
        }
        if (panel->parentWidget() == settings_row) {
            settings_row->layout()->removeWidget(panel);
        } else {
            grid->removeWidget(panel);
        }
        panel->hide();
        panel->setMinimumSize(0, 0);
        panel->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }
    grid->removeWidget(settings_row);
    settings_row->hide();
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
    const auto ADD = [this, grid](QWidget *widget, int row, int col, int span = 1) {
        grid->addWidget(widget, row, col, 1, span);
        if (widget == m_attributePane || widget == m_gameGains) {
            grid->setAlignment(widget, Qt::AlignTop);
        }
        widget->show();
    };
    if (section == QStringLiteral("simulation")) {
        grid->setAlignment({});
        grid->setSpacing(4);
        ADD(profiles, 0, 0, 3);
        // The upper cards have their own columns, independent of attributes/gains below.
        run->setFixedWidth(224);
        settings_row->layout()->addWidget(run);
        settings_row->layout()->addWidget(permanents);
        run->show();
        permanents->show();
        ADD(settings_row, 1, 0, 3);
        settings_row->setMaximumHeight(236);
        run->setMaximumHeight(236);
        permanents->setMaximumHeight(236);
        ADD(m_attributePane, 2, 0);
        ADD(m_gameGains, 2, 1);
        ADD(flow, 2, 2);
        grid->setAlignment(flow, Qt::AlignTop);
        ADD(talents, 3, 0, 3);
        talents->setFixedHeight(100);
        grid->setAlignment(talents, Qt::AlignTop);
        if (!m_externalOutput) {
            ADD(output, 4, 0, 3);
        }
        grid->setRowMinimumHeight(1, 222);
        grid->setRowMinimumHeight(2, 270);
        grid->setRowMinimumHeight(3, 100);
        grid->setRowStretch(3, 1);
    } else {
        grid->setAlignment(Qt::AlignTop);
        grid->setSpacing(8);
        ADD(profiles, 0, 0, 3);
        if (section == "attributes") {
            ADD(m_attributePane, 1, 0);
            ADD(m_gameGains, 1, 1);
            ADD(flow, 1, 2);
            grid->setAlignment(flow, Qt::AlignTop);
            talents->setFixedHeight(100);
            ADD(talents, 2, 0, 3);
            grid->setAlignment(talents, Qt::AlignTop);
        } else {
            QWidget *panel = section == "talents" ? talents : section == "permanents" ? permanents : run;
            ADD(panel, 1, 0, 3);
            panel->setMinimumHeight(section == "permanents" ? 300 : 440);
        }
        grid->setRowStretch(1, 1);
    }
    UpdateTalentHeight();
    updateGeometry();
}

namespace {
QString FoodFamily(desktop::Specialization spec)
{
    return desktop::Text(runtime::season50::PermanentFamily(desktop::School(spec)));
}
} // namespace

int SimulationOptions::PermanentMajor(const QJsonObject &v, desktop::Specialization spec) const
{
    int result = 0;
    for (auto *combo : m_permanents) {
        if (combo->property("family").toString() == FoodFamily(spec)) {
            const auto stats  = combo->itemData(v[combo->objectName()].toInt()).toJsonObject();
            result           += stats["major"].toInt() + stats[desktop::Text(desktop::Info(spec).major)].toInt();
        }
    }
    return result;
}

runtime::season50::Config SimulationOptions::Config() const
{
    QString error;
    if (!CheckFlowMacro(error)) {
        throw std::invalid_argument(error.toStdString());
    }
    return ConfigFromValues(Values(), Specialization(), m_talentPanels.at(static_cast<int>(Specialization()))->Selection(), m_flowEvents);
}

desktop::Config SimulationOptions::ActiveConfig() const
{
    return Config();
}

desktop::Specialization SimulationOptions::Specialization() const
{
    return static_cast<desktop::Specialization>(std::max(0, m_specialization->currentIndex()));
}

runtime::season50::Config SimulationOptions::ConfigFromValues(
    const QJsonObject                    &values,
    desktop::Specialization               spec,
    const QJsonArray                     &talents,
    std::span<const runtime::TargetEvent> events) const
{
    runtime::season50::Config cfg;
    cfg.book                = &desktop::School(spec);
    cfg.precombat = cfg.book->precombat;
    const auto &book        = *cfg.book;
    const auto &v = values;
    std::map<std::string, std::string> selected;
    for (auto *combo : m_permanents) {
        const int index = v[combo->objectName()].toInt();
        if (index > 0 && combo->property("family").toString() == FoodFamily(spec))
            selected.emplace(combo->objectName().section('/', -1).toStdString(), combo->itemText(index).toStdString());
    }
    const auto pct = [&](const char *key, int scale = 1024) {
        return qRound(v[key].toDouble() * scale / 100.);
    };
    cfg.delay = v["delay"].toInt();
    cfg.player_life = v["playerLife"].toDouble(100.) / 100.;
    cfg.target = { v["targetLevel"].toInt(), v["shield"].toInt(), v["targetLife"].toDouble() / 100., v["distance"].toDouble(), pct("vulnerable") };
    cfg.target_events.assign(events.begin(), events.end());
    if (book.initial_qidian_option) {
        cfg.initial_qidian = v["qidian"].toInt();
    }
    auto &a             = cfg.attributes;
    a.attack_base       = v["attackBase"].toInt();
    a.weapon            = v["weapon"].toInt();
    a.crit              = v["crit"].toInt();
    a.crit_power        = v["critPower"].toInt();
    a.overcome          = v["overcomeBase"].toInt();
    a.strain            = v["strain"].toInt();
    a.surplus           = v["surplus"].toInt();
    a.haste             = v["haste"].toInt();
    a.attack_by_class    = v["attackClass"].toInt();
    a.attack_percent     = pct("attackBonus");
    a.overcome_percent   = pct("overcomeBonus");
    a.crit_basis_points  = pct("critBonus", 10000);
    a.crit_power_percent = pct("critPowerBonus");
    a.haste_percent      = pct("hasteBonus");
    a.damage_percent     = pct("damageBonus");
    a.strain_percent     = pct("strainBonus");
    a.pve_percent        = pct("pveBonus");
    a.ignore_shield      = pct("ignoreShield");
    cfg.raid_full = v["raidFull"].toBool();
    if (cfg.raid_full) {
        cfg.base_major = v["importedMajor"].toInt();
        cfg.permanents = std::move(selected);
    } else {
        runtime::season50::ApplyPermanents(book, a, v["importedMajor"].toInt(), selected);
    }
    for (const auto &name : desktop::TalentPanel::Names(spec, talents)) {
        cfg.talents.push_back(name.toStdString());
    }
    const auto code = desktop::Text(book.code);
    for (auto name : book.recipes) {
        if (v["recipe/" + code + "/" + desktop::Text(name)].toBool()) {
            cfg.recipes.emplace_back(name);
        }
    }
    for (auto name : book.equipment) {
        if (v["equipment/" + code + "/" + desktop::Text(name)].toBool()) {
            cfg.equipment.emplace_back(name);
        }
    }
    return cfg;
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
    batch.damage_capacity   = (static_cast<std::size_t>(batch.duration) * 16) + 64;
    batch.mutation_capacity = (static_cast<std::size_t>(batch.duration) * 40) + 128 + (m_flowEvents.size() * 2);
    return batch;
}

void SimulationOptions::SaveSettings() const
{
    auto      snapshot = Snapshot();
    QSettings settings;
    settings.setValue("simulation/schoolPanels", QJsonDocument(m_panelDrafts).toJson(QJsonDocument::Compact));
    settings.setValue("simulation/season50FlowDraft", m_flowEditor->toPlainText());
    if (!desktop::CompileFlowMacro(m_flowEditor->toPlainText()).errors.isEmpty()) {
        snapshot["flowMacro"] = desktop::FlowMacroFromValues(m_flowValues);
    }
    settings.setValue("simulation/season50", QJsonDocument(snapshot).toJson(QJsonDocument::Compact));
}
QJsonObject SimulationOptions::Values() const
{
    QJsonObject values = m_flowValues;
    for (const auto *input : findChildren<QSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) {
            continue;
        }
        values.insert(input->objectName(), input->value());
    }
    for (const auto *input : findChildren<QDoubleSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) {
            continue;
        }
        values.insert(input->objectName(), input->value());
    }
    for (const auto *input : findChildren<QCheckBox *>()) {
        if (input->property("flowViewOnly").toBool()) {
            continue;
        }
        values.insert(input->objectName(), input->isChecked());
    }
    // JSON numbers cannot portably preserve all uint64 seeds.
    values.insert(QStringLiteral("seed"), m_seed->text());
    for (const auto *input : m_permanents) {
        values.insert(input->objectName(), input->currentIndex());
    }
    return values;
}

QJsonObject SimulationOptions::Snapshot() const
{
    QJsonObject talents;
    for (int i = 0; i < m_specialization->count(); ++i) {
        talents.insert(m_specialization->itemData(i).toString(), m_talentPanels[i]->Selection());
    }
    return {
        { "format",         "jx3dps.desktop.config"                    },
        { "version",        8                                          },
        { "specialization", m_specialization->currentData().toString() },
        { "values",         Values()                                   },
        { "flowMacro",      m_flowEditor->toPlainText()                },
        { "talents",        talents                                    }
    };
}

bool SimulationOptions::ValidateSnapshot(const QJsonObject &snapshot, QString &error) const
{
    error.clear();
    const int index = m_specialization->findData(snapshot.value("specialization").toString());
    if (snapshot["version"] != 8 || snapshot["format"] != "jx3dps.desktop.config" || index < 0 || !snapshot["values"].isObject())
    {
        error = QStringLiteral("配置版本或心法无效");
        return false;
    }
    const auto VALUES = snapshot.value("values").toObject();
    if (!snapshot.value("flowMacro").isString()) {
        error = QStringLiteral("流程宏必须是文本。");
        return false;
    }
    const auto FLOW = desktop::CompileFlowMacro(snapshot.value("flowMacro").toString());
    if (!FLOW.errors.isEmpty()) {
        error = FLOW.ErrorText();
        return false;
    }
    for (const auto &field : desktop::FlowFields) {
        if (field.combat_only) continue;
        const auto KEY = QString::fromLatin1(field.key);
        if (!desktop::ValidFlowValue(field, VALUES.value(KEY)) || VALUES.value(KEY) != FLOW.values.value(KEY)) {
            error = QStringLiteral("流程宏与配置值不一致：%1").arg(desktop::FieldName(KEY));
            return false;
        }
    }
    const auto TALENTS = snapshot.value("talents").toObject();
    for (int i = 0; i < m_specialization->count(); ++i) {
        const auto t = TALENTS.value(m_specialization->itemData(i).toString());
        if (!t.isArray() || !m_talentPanels[i]->ValidateSelection(t.toArray(), error)) {
            if (error.isEmpty()) {
                error = QStringLiteral("奇穴配置无效");
            }
            return false;
        }
    }
    if (VALUES.keys() != Values().keys()) {
        error = QStringLiteral("配置缺少字段或包含未知字段。");
        return false;
    }
    const auto INVALID = [&](const QString &key) {
        error = QStringLiteral("配置值无效：%1").arg(desktop::FieldName(key));
        return false;
    };
    for (const auto *input : findChildren<QSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) {
            continue;
        }
        const auto v = VALUES.value(input->objectName());
        const auto N = v.toDouble();
        if (!v.isDouble() || !std::isfinite(N) || N < input->minimum() || N > input->maximum() || std::floor(N) != N) {
            return INVALID(input->objectName());
        }
    }
    for (const auto *input : findChildren<QDoubleSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) {
            continue;
        }
        const auto v     = VALUES.value(input->objectName());
        const auto N     = v.toDouble();
        const auto SCALE = std::pow(10.0, input->decimals());
        if (!v.isDouble() || !std::isfinite(N) || N < input->minimum() || N > input->maximum() ||
            std::abs((N * SCALE) - std::round(N * SCALE)) > 0.0001)
        {
            return INVALID(input->objectName());
        }
    }
    for (const auto *input : findChildren<QCheckBox *>()) {
        if (input->property("flowViewOnly").toBool()) {
            continue;
        }
        if (!VALUES.value(input->objectName()).isBool()) {
            return INVALID(input->objectName());
        }
    }
    for (const auto *input : m_permanents) {
        const auto value = VALUES.value(input->objectName());
        if (!value.isDouble() || value.toDouble() != value.toInt() || value.toInt() < 0 || value.toInt() >= input->count())
        {
            return INVALID(input->objectName());
        }
    }
    const auto SEED  = VALUES.value("seed");
    bool       valid = false;
    SEED.toString().toULongLong(&valid);
    if (!SEED.isString() || !valid || !QRegularExpression(QStringLiteral("^[0-9]{1,20}$")).match(SEED.toString()).hasMatch())
    {
        return INVALID(QStringLiteral("seed"));
    }
    try {
        // Validate coupled rules (recipes, haste, etc.) before changing any control.
        for (int i = 0; i < m_specialization->count(); ++i) {
            auto spec = static_cast<desktop::Specialization>(i);
            runtime::season50::Prepare(
                ConfigFromValues(VALUES, spec, TALENTS[m_specialization->itemData(i).toString()].toArray(), FLOW.TargetEvents()));
        }
    } catch (const std::exception &exception) {
        error = desktop::Diagnostic(QString::fromUtf8(exception.what()));
        return false;
    }
    return true;
}

QJsonObject SimulationOptions::NormalizeSnapshot(const QJsonObject &snapshot) const
{
    // A pre-compression configuration has incompatible combat stats and option IDs.
    // Carry only portable run settings and dated target movements into a fresh 50-level profile.
    const int version = snapshot["version"].toInt();
    const auto talentNames = [](const auto &names) {
        QStringList out;
        for (auto name : names) out.append(desktop::Text(name));
        return out;
    };
    if (version >= 5 && version <= 8 && snapshot["format"] == "jx3dps.desktop.config") {
        auto values = snapshot.value("values").toObject();
        auto talents = snapshot.value("talents").toObject();
        bool migrated = false;
        // Older workspaces have no entries at all for a school added after they were saved.
        if (snapshot.value("talents").isObject()) {
            for (int i = 0; i < m_specialization->count(); ++i) {
                const auto code = m_specialization->itemData(i).toString();
                if (talents.contains(code) || snapshot["specialization"] == code) continue;
                const auto recipes = "recipe/" + code + "/", gear = "equipment/" + code + "/";
                if (std::ranges::any_of(values.keys(), [&](const QString &key) {
                    return key.startsWith(recipes) || key.startsWith(gear);
                })) continue;
                const auto &book = desktop::School(static_cast<desktop::Specialization>(i));
                talents.insert(code, m_talentPanels[i]->DefaultSelection());
                for (auto name : book.recipes)
                    values.insert(recipes + desktop::Text(name), std::ranges::contains(book.default_recipes, name));
                for (auto name : book.equipment)
                    values.insert(gear + desktop::Text(name), std::ranges::contains(book.default_equipment, name));
                migrated = true;
            }
        }
        if (!values.contains("playerLife")) {
            values.insert("playerLife", 100.);
            migrated = true;
        }
        if (version == 8) {
            const auto add_unselected = [&](const QString &key) {
                if (!values.contains(key)) {
                    values.insert(key, false);
                    migrated = true;
                }
            };
            for (const auto &book : runtime::season50::Books()) {
                const auto prefix = "equipment/" + desktop::Text(book.code) + "/";
                for (const auto &item : runtime::season50::InstanceEquipment) add_unselected(prefix + desktop::Text(item.name));
                for (const auto &tier : runtime::season50::LegendaryWeapons) add_unselected(prefix + desktop::Text(tier.name));
                for (auto recipe : book.saved.added_recipes_v8)
                    add_unselected("recipe/" + desktop::Text(book.code) + "/" + desktop::Text(recipe));
            }
            add_unselected("raidFull");
        }
        for (const auto &book : runtime::season50::Books()) {
            const auto prefix = "recipe/" + desktop::Text(book.code) + "/";
            for (auto [oldName, newName] : book.recipe_aliases) {
                const auto from = prefix + desktop::Text(oldName), to = prefix + desktop::Text(newName);
                if (!values.contains(from)) continue;
                auto previous = values.take(from);
                if (!values.contains(to)) values.insert(to, previous);
                migrated = true;
            }
        }
        if (migrated) {
            auto next = snapshot;
            next["values"] = values;
            if (snapshot.value("talents").isObject()) next["talents"] = talents;
            return NormalizeSnapshot(next);
        }
    }
    if (version == 7 && snapshot["format"] == "jx3dps.desktop.config" && snapshot["values"].isObject()) {
        auto upgraded = snapshot;
        auto values = snapshot["values"].toObject();
        auto selections = snapshot["talents"].toObject();
        for (int i = 0; i < m_specialization->count(); ++i) {
            const auto spec = static_cast<desktop::Specialization>(i);
            const auto &book = desktop::School(spec);
            const auto code = desktop::Text(book.code);
            const auto prior = selections.value(code).toArray();
            QString error;
            if (!m_talentPanels[i]->ValidateSelection(prior, error)) continue;
            const auto current = m_talentPanels[i]->DefaultSelection();
            const auto priorNames = desktop::TalentPanel::Names(spec, prior);
            const auto expected = book.saved.talents_v7.empty() ? desktop::TalentPanel::Names(spec, current) : talentNames(book.saved.talents_v7);
            const bool factoryTalents = std::ranges::is_permutation(priorNames, expected);
            const auto prefix = "recipe/" + code + "/";
            bool factoryRecipes = true;
            for (auto recipe : book.recipes) {
                const auto key = prefix + desktop::Text(recipe);
                if (!values.contains(key)) continue;
                const bool added = book.saved.all_recipes_added_v7 || std::ranges::contains(book.saved.added_recipes_v7, recipe);
                const bool removed = std::ranges::contains(book.saved.removed_recipes_v7, recipe);
                const bool previouslySelected = removed || (!added && std::ranges::contains(book.default_recipes, recipe));
                if (!values[key].isBool() || values[key].toBool() != previouslySelected) factoryRecipes = false;
            }
            if (factoryTalents) selections[code] = current;
            for (auto recipe : book.recipes) {
                const auto key = prefix + desktop::Text(recipe);
                if (factoryRecipes && factoryTalents) values[key] = std::ranges::contains(book.default_recipes, recipe);
                else if (!values.contains(key)) values[key] = false;
            }
        }
        upgraded["talents"] = selections;
        upgraded["values"] = values;
        upgraded["version"] = 8;
        return NormalizeSnapshot(upgraded);
    }
    if (version == 6 && snapshot["format"] == "jx3dps.desktop.config" && snapshot["values"].isObject() && snapshot["talents"].isObject()) {
        // Upgrade factory loadouts; custom talent choices and explicitly selected recipes are preserved.
        auto upgraded=snapshot;
        auto values=snapshot["values"].toObject(), selections=snapshot["talents"].toObject();
        for(int i=0;i<m_specialization->count();++i){
            const auto spec=static_cast<desktop::Specialization>(i);
            const auto &book=desktop::School(spec);
            const auto code=desktop::Text(book.code);
            const auto prior=selections[code].toArray();
            QString error;
            if(!m_talentPanels[i]->ValidateSelection(prior,error)) continue;
            const auto names=desktop::TalentPanel::Names(spec,prior);
            const bool factory=std::ranges::is_permutation(names,talentNames(book.saved.talents_v6));
            const auto prefix="recipe/"+code+"/";
            bool factoryRecipes=true;
            for(auto recipe:book.recipes){
                auto key=prefix+desktop::Text(recipe);
                bool recommended=std::ranges::find(book.default_recipes,recipe)!=book.default_recipes.end();
                if(values.contains(key) && (!values[key].isBool() || values[key].toBool()!=recommended))factoryRecipes=false;
            }
            if(factory) selections[code]=m_talentPanels[i]->DefaultSelection();
            for(auto recipe:book.recipes){
                auto key=prefix+desktop::Text(recipe);
                if(!values.contains(key))
                    values[key]=factory && factoryRecipes && std::ranges::find(book.default_recipes,recipe)!=book.default_recipes.end();
            }
        }
        upgraded["values"]=values;
        upgraded["talents"]=selections;
        upgraded["version"]=8;
        return NormalizeSnapshot(upgraded);
    }
    if (version == 5 && snapshot["format"] == "jx3dps.desktop.config" && snapshot["values"].isObject()) {
        auto upgraded = snapshot;
        auto values = snapshot["values"].toObject();
        // Newly supported equipment was inactive when the saved fight was created.
        for (const auto &book : runtime::season50::Books()) {
            for (auto gear : book.equipment) {
                const auto key = "equipment/" + desktop::Text(book.code) + "/" + desktop::Text(gear);
                if (gear == runtime::season50::WindPendant || gear == runtime::season50::WaterWeapon || gear == runtime::season50::BeltEnchant ||
                    gear == runtime::season50::ClassSet || std::ranges::any_of(runtime::season50::DamageEnchants,
                        [&](const auto &enchant) { return enchant.name == gear; })) {
                    if (!values.contains(key)) values[key] = false;
                }
            }
        }
        upgraded["values"] = values;
        upgraded["version"] = 6;
        return NormalizeSnapshot(upgraded);
    }
    if (snapshot["format"] != "jx3dps.desktop.config" || version < 1 || version > 4) {
        return snapshot;
    }
    const auto id = snapshot["specialization"];
    if (!id.isUndefined() && id != 0 && id != 1) {
        return snapshot;
    }
    auto index = id.toInt(0);
    if (index != 0 && index != 1) {
        return snapshot;
    }
    auto migrated = Snapshot();
    auto values   = migrated["values"].toObject();
    for (auto *input : findChildren<QSpinBox *>()) {
        if (input->property("defaultValue").isValid()) {
            values[input->objectName()] = input->property("defaultValue").toInt();
        }
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        if (input->property("defaultValue").isValid()) {
            values[input->objectName()] = input->property("defaultValue").toDouble();
        }
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (values.contains(input->objectName())) {
            values[input->objectName()] = input->property("defaultChecked").toBool();
        }
    }
    for (auto *combo : m_permanents) {
        values[combo->objectName()] = 0;
    }
    if (!snapshot["values"].isObject()) {
        return snapshot;
    }
    auto old = snapshot["values"].toObject();
    const auto defaults = SchoolPanel(desktop::School(static_cast<desktop::Specialization>(index)));
    for (auto it = defaults.begin(); it != defaults.end(); ++it) values[it.key()] = it.value();
    for (const auto key : { "seed", "iterations", "workers", "seconds", "delay" }) {
        if (old.contains(key)) {
            values[key] = old[key];
        }
    }
    auto flow = desktop::FlowDefaults();
    for (auto &field : desktop::FlowFields) {
        if (field.combat_only) continue;
        values[field.key] = flow[field.key];
    }
    QString macro = desktop::FlowMacroFromValues(flow);
    for (const auto &line : snapshot["flowMacro"].toString().split('\n')) {
        if (QRegularExpression(QStringLiteral("^\\s*[0-9]+:[0-9]+")).match(line).hasMatch()) {
            macro += '\n' + line;
        }
    }
    auto       ts = migrated["talents"].toObject();
    for (int i = 0; i < m_specialization->count(); ++i)
        ts[m_specialization->itemData(i).toString()] = m_talentPanels[i]->DefaultSelection();
    migrated["talents"]        = ts;
    migrated["values"]         = values;
    migrated["flowMacro"]      = macro;
    migrated["specialization"] = index ? "mw" : "tx";
    return migrated;
}
bool SimulationOptions::RestoreSnapshot(const QJsonObject &original, QString &error)
{
    const auto snapshot = NormalizeSnapshot(original);
    if (!ValidateSnapshot(snapshot, error)) {
        return false;
    }
    const auto VALUES  = snapshot.value("values").toObject();
    const auto TALENTS = snapshot.value("talents").toObject();
    m_specialization->setCurrentIndex(m_specialization->findData(snapshot.value("specialization").toString()));
    for (int index = 0; index < m_specialization->count(); ++index) {
        m_talentPanels[index]->RestoreSelection(TALENTS.value(m_specialization->itemData(index).toString()).toArray());
    }
    for (auto *input : findChildren<QSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) {
            continue;
        }
        input->setValue(VALUES.value(input->objectName()).toInt());
    }
    for (auto *input : findChildren<QDoubleSpinBox *>()) {
        if (input->property("flowViewOnly").toBool()) {
            continue;
        }
        input->setValue(VALUES.value(input->objectName()).toDouble());
    }
    for (auto *input : findChildren<QCheckBox *>()) {
        if (input->property("flowViewOnly").toBool()) {
            continue;
        }
        const QSignalBlocker blocker(input->property("defaultChecked").isValid() ? input : nullptr);
        input->setChecked(VALUES.value(input->objectName()).toBool());
    }
    m_seed->setText(VALUES.value("seed").toString());
    m_flowEditor->setPlainText(snapshot.value("flowMacro").toString());
    for (auto *input : m_permanents) {
        input->setCurrentIndex(VALUES.value(input->objectName()).toInt());
    }
    UpdateFlowMacro();
    RefreshGameAttributes();
    return true;
}

bool SimulationOptions::ExportConfiguration(const QString &path, QString &error) const
{
    const auto snapshot = Snapshot();
    if (!ValidateSnapshot(snapshot, error)) {
        return false;
    }
    const auto BYTES = QJsonDocument(snapshot).toJson();
    QSaveFile  file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(BYTES) != BYTES.size() || !file.commit()) {
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
    const auto BYTES = file.readAll();
    if (file.error() != QFileDevice::NoError) {
        error = file.errorString();
        return false;
    }
    QJsonParseError parse;
    const auto      document = QJsonDocument::fromJson(BYTES, &parse);
    if (parse.error != QJsonParseError::NoError || !document.isObject()) {
        error = QStringLiteral("配置文件格式错误，请检查文件内容（位置 %1）。").arg(parse.offset);
        return false;
    }
    return RestoreSnapshot(document.object(), error);
}

bool SimulationOptions::ImportAttributes(const QJsonObject &attributes, QString &error)
{
    auto                              snapshot  = Snapshot();
    auto                              values    = snapshot.value("values").toObject();
    const auto                        info      = desktop::Info(Specialization());
    const auto                        PREFIX    = desktop::Text(info.damage);
    const auto                        MAJOR_KEY =
        info.major == "身法"   ? "Agility"
        : info.major == "力道" ? "Strength"
        : info.major == "元气" ? "Spunk"
                               : "Spirit";
    const std::pair<QString, QString> FIELDS[] = {
        { PREFIX + "AttackPowerBase",     "attackBase"    },
        { PREFIX + "CriticalStrike",      "crit"          },
        { PREFIX + "CriticalDamagePower", "critPower"     },
        { PREFIX + "Overcome",            "overcomeBase"  },
        { "SurplusValue",                 "surplus"       },
        { "Strain",                       "strain"        },
        { "Haste",                        "haste"         },
        { MAJOR_KEY,                      "importedMajor" },

        { "MeleeWeaponDamage",            "weapon"        }
    };
    const auto READ = [&](const QString &key, int &number) {
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
    for (const auto &[SOURCE, DESTINATION] : FIELDS) {
        int value = 0;
        if (!READ(SOURCE, value)) {
            return false;
        }
        values.insert(DESTINATION, value);
    }
    int random_weapon = 0;
    if (!READ(QStringLiteral("MeleeWeaponDamageRand"), random_weapon)) {
        return false;
    }
    values.insert("weapon", values.value("weapon").toInt() + (random_weapon / 2));
    const auto MAJOR = static_cast<value_t>(values.value("importedMajor").toInt());
    values.insert("attackClass", static_cast<int>(MAJOR * info.attack_from_major / 1024));

    for (const char *key :
         { "attackBonus", "overcomeBonus", "critBonus", "critPowerBonus", "hasteBonus", "damageBonus", "strainBonus", "ignoreShield" })
    {
        values.insert(key, 0);
    }
    values.insert("pveBonus", 0);
    for (const auto *input : m_permanents) {
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
    const auto SAVED = QJsonDocument::fromJson(QSettings().value(QStringLiteral("simulation/profiles")).toByteArray()).object();
    profiles->addItems(SAVED.keys());
    profiles->setCurrentIndex(-1);
    profiles->setPlaceholderText(QStringLiteral("输入方案名称或选择已有方案"));
    profiles->lineEdit()->setPlaceholderText(QStringLiteral("输入方案名 / 选择已存方案"));
    auto placeholder_palette = profiles->lineEdit()->palette();
    placeholder_palette.setColor(QPalette::PlaceholderText, QColor(QStringLiteral("#94a5b0")));
    profiles->lineEdit()->setPalette(placeholder_palette);
    auto *buttons = new QHBoxLayout;
    buttons->addWidget(profiles, 1);
    const auto BUTTON = [&](const QString &label, const char *name, auto callback) {
        auto *item = new QPushButton(label, group);
        item->setObjectName(QString::fromLatin1(name));
        buttons->addWidget(item);
        connect(item, &QPushButton::clicked, this, callback);
    };
    const auto READ = [] {
        return QJsonDocument::fromJson(QSettings().value(QStringLiteral("simulation/profiles")).toByteArray()).object();
    };
    const auto WRITE = [](const QJsonObject &data) {
        QSettings().setValue(QStringLiteral("simulation/profiles"), QJsonDocument(data).toJson(QJsonDocument::Compact));
    };
    const auto report = [this](const QString &error) {
        QMessageBox::warning(this, QStringLiteral("配置"), error);
    };
    BUTTON(QStringLiteral("保存"), "saveProfile", [=, this] {
        const auto NAME = profiles->currentText().trimmed();
        if (NAME.isEmpty()) {
            report(QStringLiteral("请先输入方案名称。"));
            return;
        }
        QString    error;
        const auto snapshot = Snapshot();
        if (!ValidateSnapshot(snapshot, error)) {
            report(error);
            return;
        }
        auto data = READ();
        data.insert(NAME, snapshot);
        WRITE(data);
        if (profiles->findText(NAME) < 0) {
            profiles->addItem(NAME);
        }
        profiles->setCurrentText(NAME);
    });
    BUTTON(QStringLiteral("加载"), "loadProfile", [=, this] {
        const auto NAME = profiles->currentText().trimmed();
        QString    error;
        const auto DATA = READ();
        if (!DATA.contains(NAME)) {
            report(QStringLiteral("请先选择已保存的方案。"));
            return;
        }
        if (!RestoreSnapshot(DATA.value(NAME).toObject(), error)) {
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
    const auto MENU_ACTION = [&](const QString &label, const char *name, auto callback) {
        auto *item = menu->addAction(label);
        item->setObjectName(QString::fromLatin1(name));
        connect(item, &QAction::triggered, this, callback);
    };
    MENU_ACTION(QStringLiteral("删除方案"), "deleteProfile", [=, this] {
        const auto NAME = profiles->currentText().trimmed();
        auto       data = READ();
        if (!data.contains(NAME)) {
            return;
        }
        if (QMessageBox::question(this, QStringLiteral("删除方案"), QStringLiteral("确定删除“%1”？").arg(NAME)) != QMessageBox::Yes)
        {
            return;
        }
        data.remove(NAME);
        WRITE(data);
        profiles->removeItem(profiles->findText(NAME));
    });
    MENU_ACTION(QStringLiteral("导入配置"), "importConfiguration", [=, this] {
        const auto PATH = QFileDialog::getOpenFileName(this, QStringLiteral("导入配置"), {}, QStringLiteral("配置文件 (*.json)"));
        if (PATH.isEmpty()) {
            return;
        }
        QString error;
        if (!ImportConfiguration(PATH, error)) {
            report(error);
        } else {
            profiles->setCurrentIndex(-1);
        }
    });
    MENU_ACTION(QStringLiteral("导出配置"), "exportConfiguration", [=, this] {
        const auto PATH =
            QFileDialog::getSaveFileName(this, QStringLiteral("导出配置"), QStringLiteral("心法配置.json"), QStringLiteral("配置文件 (*.json)"));
        if (PATH.isEmpty()) {
            return;
        }
        QString error;
        if (!ExportConfiguration(PATH, error)) {
            report(error);
        }
    });
    menu->addSeparator();
}
