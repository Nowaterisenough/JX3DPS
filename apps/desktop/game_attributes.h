#pragma once

#include <QLabel>
#include <QFrame>
#include <QEnterEvent>
#include <QPainter>
#include <QPushButton>
#include <QSettings>
#include <QStackedLayout>
#include <QTableWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <array>
#include <functional>
#include <optional>
#include "controls/theme/segmented_tabs.h"
#include "controls/theme/magpie_check_button.h"
#include "runtime_catalog.h"
#include "simulation_result.h"

namespace desktop {
struct GameAttributeRow { const char *key; const char *label; const char *gain; };
// Confirmed game-client screenshot in JX3BOX post 65071; runtime-only rows follow it.
inline constexpr std::array<GameAttributeRow, 10> GameAttributeRows{{
    {"major", "主属性", ""}, {"attack", "攻击力", "基础攻击"}, {"hit", "命中", ""},
    {"critical", "会心", "会心等级"}, {"criticalPower", "会心效果", "会效等级"},
    {"haste", "加速", "加速等级"}, {"overcome", "破防", "破防等级"},
    {"strain", "无双", "无双等级"}, {"surplus", "破招", "破招等级"}, {"weapon", "武器伤害", "武器伤害"}
}};
inline QString GameAttributeName(int row, Specialization specialization) {
    return row ? QString::fromUtf8(GameAttributeRows.at(row).label)
               : specialization == Specialization::MoWen ? QStringLiteral("根骨") : QStringLiteral("身法");
}
inline const AttributeGain *GameAttributeGain(const SimulationResult &result, int row) {
    const auto name = QString::fromUtf8(GameAttributeRows.at(row).gain);
    if (name.isEmpty()) return nullptr;
    for (const auto &gain : result.gains) if (gain.attribute == name) return &gain;
    return nullptr;
}
inline QString MissingGainText(int row, const SimulationResult &result) {
    return row == 0 ? QStringLiteral("暂未支持") : row == 2 ? QStringLiteral("固定命中")
         : result.gainsRequested && !result.Complete() ? QStringLiteral("未完成") : QStringLiteral("未计算");
}
inline void FillGameGainTable(QTableWidget *table, const SimulationResult &result, Specialization specialization, bool detailed) {
    table->setToolTip(GainComparisonDescription(result.gainMode));
    table->setRowCount(GameAttributeRows.size());
    for (int row = 0; row < int(GameAttributeRows.size()); ++row) {
        const auto *gain = GameAttributeGain(result, row);
        QStringList values{GameAttributeName(row, specialization)};
        if (gain && gain->increment > 0 && result.FightSeconds() > 0) {
            const double dps = gain->stats.mean_damage / result.FightSeconds();
            const double delta = dps - result.MeanDps();
            const auto perPoint = QString::number(delta / gain->increment, 'f', detailed ? 4 : 2);
            const auto percent = result.MeanDps() > 0 ? QString::number(delta * 100 / result.MeanDps(), 'f', 2) + '%' : QStringLiteral("--");
            if (detailed) values << QString::number(gain->increment) << QString::number(gain->stats.count)
                                 << QString::number(dps, 'f', 1) << QString::number(delta, 'f', 1);
            values << perPoint << percent;
        } else {
            while (values.size() < (detailed ? 6 : 2)) values << QStringLiteral("--");
            values << MissingGainText(row, result);
        }
        for (int column = 0; column < values.size(); ++column) {
            auto *item = new QTableWidgetItem(values[column]);
            item->setData(Qt::UserRole, QString::fromLatin1(GameAttributeRows[row].key));
            item->setToolTip(gain ? QStringLiteral("%1：增加 %2 点%3的模拟收益；不是增加 1% 面板数值。")
                                 .arg(GainComparisonName(result.gainMode)).arg(gain->increment).arg(gain->attribute)
                                 : MissingGainText(row, result));
            if (column) item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            table->setItem(row, column, item);
        }
    }
}

inline constexpr int AttributeRowHeight = 19;
inline constexpr int AttributeHeaderHeight = 28;
inline constexpr int AttributeHeaderSpacing = 4;
inline constexpr int AttributeFooterHeight = 34;
inline constexpr int AttributeActionHeight = 26;

class AttributeComparisonRow final : public QWidget {
public:
    std::function<void(int)> hovered;
    explicit AttributeComparisonRow(int index, QWidget *parent = nullptr) : QWidget(parent), index_(index) {
        setFixedHeight(AttributeRowHeight);
    }
    void SetHighlighted(bool highlighted) {
        if (highlighted_ == highlighted) return;
        highlighted_ = highlighted;
        setProperty("comparisonHighlighted", highlighted);
        update();
    }
protected:
    void paintEvent(QPaintEvent *) override {
        if (!highlighted_) return;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#2d2d33"));
        painter.drawRoundedRect(QRectF(rect()), 4, 4);
    }
    void enterEvent(QEnterEvent *event) override { if (hovered) hovered(index_); QWidget::enterEvent(event); }
    void leaveEvent(QEvent *event) override { if (hovered) hovered(-1); QWidget::leaveEvent(event); }
    void hideEvent(QHideEvent *event) override { if (highlighted_ && hovered) hovered(-1); QWidget::hideEvent(event); }
private:
    int index_;
    bool highlighted_ = false;
};

class AttributeActionRow final : public QWidget {
public:
    using QWidget::QWidget;
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setPen(QColor("#333339"));
        painter.drawLine(0, 0, width(), 0);
    }
};

inline QHBoxLayout *AttributeModuleHeader(QWidget *owner, QVBoxLayout *layout, const QString &title) {
    auto *header = new QWidget(owner);
    header->setObjectName(QStringLiteral("attributeModuleHeader"));
    header->setFixedHeight(AttributeHeaderHeight);
    auto *row = new QHBoxLayout(header);
    row->setContentsMargins(2, 0, 2, 0);
    row->setSpacing(6);
    auto *label = new QLabel(title, header);
    label->setObjectName(QStringLiteral("attributeModuleTitle"));
    label->setStyleSheet(QStringLiteral("font-weight: 600; color: #92929d;"));
    row->addWidget(label);
    row->addStretch();
    layout->addWidget(header);
    return row;
}
inline QVBoxLayout *AttributeModuleBody(QWidget *owner, QVBoxLayout *layout) {
    auto *body = new QWidget(owner);
    body->setObjectName(QStringLiteral("attributeModuleBody"));
    body->setProperty("statModule", true);
    body->setAttribute(Qt::WA_StyledBackground);
    auto *content = new QVBoxLayout(body);
    content->setContentsMargins(12, 6, 12, 6);
    content->setSpacing(0);
    layout->addWidget(body, 1);
    return content;
}
inline QHBoxLayout *AttributeModuleFooter(QWidget *owner, QVBoxLayout *layout) {
    auto *footer = new AttributeActionRow(owner);
    footer->setObjectName(QStringLiteral("attributeModuleActions"));
    footer->setFixedHeight(AttributeFooterHeight);
    auto *row = new QHBoxLayout(footer);
    row->setContentsMargins(0, 5, 0, 0);
    row->setSpacing(6);
    layout->addWidget(footer);
    return row;
}

class GameAttributePanel final : public QWidget {
public:
    QPushButton *edit;
    QToolButton *equipment;
    QHBoxLayout *tools;
    std::function<void(int)> hoverChanged;
    explicit GameAttributePanel(QWidget *parent = nullptr) : QWidget(parent) {
        setObjectName(QStringLiteral("gameAttributePanel"));
        setProperty("statSection", true);
        setAttribute(Qt::WA_StyledBackground);
        auto *section = new QVBoxLayout(this);
        section->setContentsMargins(0, 0, 0, 0);
        section->setSpacing(AttributeHeaderSpacing);
        auto *header = AttributeModuleHeader(this, section, QStringLiteral("属性"));
        equipment = new QToolButton(this);
        equipment->setObjectName(QStringLiteral("openEquipment"));
        equipment->setText(QStringLiteral("配装…"));
        equipment->setToolTip(QStringLiteral("打开独立配装窗口，不切走属性与收益"));
        header->addWidget(equipment);
        auto *layout = AttributeModuleBody(this, section);
        for (int i = 0; i < int(GameAttributeRows.size()); ++i) {
            auto *row = rows_[i] = new AttributeComparisonRow(i, this);
            row->hovered = [this](int index) { if (hoverChanged) hoverChanged(index); };
            row->setObjectName(QStringLiteral("gameAttributeRow_%1").arg(QString::fromLatin1(GameAttributeRows[i].key)));
            row->setProperty("attributeKey", QString::fromLatin1(GameAttributeRows[i].key));
            auto *cells = new QHBoxLayout(row);
            cells->setContentsMargins(0, 0, 0, 0); cells->setSpacing(6);
            names_[i] = new QLabel(row);
            names_[i]->setMinimumWidth(56);
            values_[i] = new QLabel(QStringLiteral("--"), row);
            values_[i]->setMinimumWidth(88);
            values_[i]->setTextFormat(Qt::RichText);
            values_[i]->setObjectName(QStringLiteral("gameAttributeValue_%1").arg(QString::fromLatin1(GameAttributeRows[i].key)));
            values_[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            values_[i]->setTextInteractionFlags(Qt::TextSelectableByMouse);
            cells->addWidget(names_[i]); cells->addWidget(values_[i], 1);
            layout->addWidget(row);
        }
        layout->addStretch();
        tools = AttributeModuleFooter(this, layout);
        edit = new QPushButton(QStringLiteral("编辑数值"), this);
        edit->setObjectName(QStringLiteral("editGameAttributes"));
        edit->setProperty("moduleAction", true);
        tools->addWidget(edit, 1);
    }
    void HighlightRow(int index) { for (int i = 0; i < int(rows_.size()); ++i) rows_[i]->SetHighlighted(i == index); }
    void SetValues(Specialization specialization, const std::array<QString, 10> &values,
                   const std::array<QString, 10> &changes = {}, bool included = true) {
        const auto modeTip = included
            ? QStringLiteral("已计入所选常驻增益。括号为相对未计入时的净变化：绿色增加，红色减少；百分比属性显示百分点变化，按面板精度取整。")
            : QStringLiteral("未计入常驻增益的基础面板；已选增益仍参与模拟，右侧收益也仍按实际模拟配置计算。");
        for (int i = 0; i < int(GameAttributeRows.size()); ++i) {
            names_[i]->setText(GameAttributeName(i, specialization));
            const auto delta = included ? changes[i] : QString{};
            values_[i]->setProperty("displayValue", values[i]);
            values_[i]->setProperty("permanentDelta", delta);
            values_[i]->setText(delta.isEmpty() ? values[i] : QStringLiteral("<span style=\"color:%2\">(%3)</span> %1")
                .arg(values[i].toHtmlEscaped(), QString::fromLatin1(delta.startsWith('-') ? theme::Error : theme::Success), delta.toHtmlEscaped()));
            values_[i]->setAccessibleName(names_[i]->text());
            values_[i]->setAccessibleDescription((delta.isEmpty() ? QString{} : '(' + delta + ") ") + values[i] + '\n' + modeTip);
            values_[i]->setToolTip(modeTip);
        }
        values_[0]->setToolTip(modeTip + QStringLiteral("\n导入属性中已经计入攻击和会心的主属性。未记录时总值显示 --，括号仅为已知的常驻主属性增量，不从攻击反推；请通过导入属性补充。"));
        values_[2]->setToolTip(QStringLiteral("当前旧规则引擎仅判定命中和会心，按固定命中处理，不提供可调整的命中等级。"));
        for (int i = 3; i <= 7; ++i) values_[i]->setToolTip(modeTip + QStringLiteral("\n按模拟器当前旧规则折算的静态面板百分比，不含战斗中的临时增益。"));
    }
private:
    std::array<AttributeComparisonRow *, 10> rows_{};
    std::array<QLabel *, 10> names_{}, values_{};
};

class AttributeGainsPanel final : public QWidget {
public:
    QToolButton *calculate;
    std::function<void(int)> hoverChanged;
    explicit AttributeGainsPanel(QWidget *parent = nullptr) : QWidget(parent) {
        setObjectName(QStringLiteral("attributeGainsModule"));
        setProperty("statSection", true);
        setAttribute(Qt::WA_StyledBackground);
        setMinimumWidth(208);
        auto *section = new QVBoxLayout(this);
        section->setContentsMargins(0, 0, 0, 0);
        section->setSpacing(AttributeHeaderSpacing);
        auto *header = AttributeModuleHeader(this, section, QStringLiteral("收益"));
        auto *units = new QLabel(QStringLiteral("秒伤 / 提升"), this);
        units->setProperty("muted", true);
        units->setToolTip(QStringLiteral("左列为所选比较增量带来的秒伤增量，右列为总提升比例。"));
        header->addWidget(units);
        mode_ = new SegmentedTabBar(this);
        mode_->setObjectName(QStringLiteral("attributeGainMode"));
        mode_->SetSegmentHeight(AttributeActionHeight);
        mode_->setUsesScrollButtons(false);
        mode_->addTab(GainComparisonName(GainComparisonMode::SameValue));
        mode_->addTab(QStringLiteral("同费用"));
        mode_->setAccessibleName(QStringLiteral("收益比较方式"));
        for (int i = 0; i < 2; ++i)
            mode_->setTabToolTip(i, GainComparisonDescription(static_cast<GainComparisonMode>(i)) +
                QStringLiteral("左列为秒伤增量，右列为总提升比例。选中右侧“计算”后，开始模拟时计算该模式。"));
        mode_->setCurrentIndex(QSettings().value(QStringLiteral("interface/gainComparisonMode"), 0).toInt() == 1 ? 1 : 0);
        auto *layout = AttributeModuleBody(this, section);
        for (int i = 0; i < int(GameAttributeRows.size()); ++i) {
            auto *row = rows_[i] = new AttributeComparisonRow(i, this);
            row->hovered = [this](int index) { if (hoverChanged) hoverChanged(index); };
            row->setObjectName(QStringLiteral("gameAttributeGainRow_%1").arg(QString::fromLatin1(GameAttributeRows[i].key)));
            row->setProperty("attributeKey", QString::fromLatin1(GameAttributeRows[i].key));
            auto *stack = stacks_[i] = new QStackedLayout(row);
            stack->setContentsMargins(0, 0, 0, 0);
            auto *numbers = new QWidget(row);
            auto *cells = new QHBoxLayout(numbers);
            cells->setContentsMargins(0, 0, 0, 0); cells->setSpacing(6);
            gains_[i] = new QLabel(QStringLiteral("--"), row);
            gains_[i]->setObjectName(QStringLiteral("gameAttributeGain_%1").arg(QString::fromLatin1(GameAttributeRows[i].key)));
            gains_[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            changes_[i] = new QLabel(row);
            changes_[i]->setObjectName(QStringLiteral("gameAttributeGainChange_%1").arg(QString::fromLatin1(GameAttributeRows[i].key)));
            changes_[i]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            changes_[i]->setProperty("muted", true);
            cells->addWidget(gains_[i], 1); cells->addWidget(changes_[i], 1);
            stack->addWidget(numbers);
            auto *state = new QWidget(row);
            auto *stateLayout = new QHBoxLayout(state);
            stateLayout->setContentsMargins(0, 0, 0, 0);
            status_[i] = new QLabel(state);
            status_[i]->setObjectName(QStringLiteral("gameAttributeGainStatus_%1").arg(QString::fromLatin1(GameAttributeRows[i].key)));
            status_[i]->setProperty("gainStatus", true);
            status_[i]->setAlignment(Qt::AlignCenter);
            stateLayout->addWidget(status_[i], 0, Qt::AlignCenter);
            stack->addWidget(state);
            layout->addWidget(row);
        }
        layout->addStretch();
        auto *footer = AttributeModuleFooter(this, layout);
        footer->addWidget(mode_);
        footer->addStretch();
        calculate = new MagpieCheckButton(this);
        calculate->setObjectName(QStringLiteral("calculateInlineGains"));
        calculate->setText(QStringLiteral("计算"));
        calculate->setAccessibleName(QStringLiteral("模拟时自动计算属性收益"));
        calculate->setChecked(QSettings().value(QStringLiteral("interface/autoCalculateGains"), false).toBool());
        footer->addWidget(calculate);
        connect(calculate, &QAbstractButton::toggled, this, [this](bool checked) {
            QSettings().setValue(QStringLiteral("interface/autoCalculateGains"), checked);
            Refresh();
        });
        connect(mode_, &QTabBar::currentChanged, this, [this] {
            QSettings().setValue(QStringLiteral("interface/gainComparisonMode"), mode_->currentIndex());
            Refresh();
        });
        Clear(Specialization::TaiXu);
    }
    GainComparisonMode Mode() const { return static_cast<GainComparisonMode>(mode_->currentIndex()); }
    void SetResult(const SimulationResult &result, Specialization specialization) {
        if (!result.gainsRequested) { Clear(specialization); return; }
        specialization_ = specialization;
        results_[result.gainMode == GainComparisonMode::SameCost ? 1 : 0] = result;
        Refresh();
    }
    void Clear(Specialization specialization) {
        specialization_ = specialization;
        results_ = {};
        Refresh();
    }
    void HighlightRow(int index) { for (int i = 0; i < int(rows_.size()); ++i) rows_[i]->SetHighlighted(i == index); }
private:
    void Refresh() {
        const auto &cached = results_[mode_->currentIndex()];
        SimulationResult empty;
        empty.gainMode = Mode();
        const auto &result = cached ? *cached : empty;
        const auto description = GainComparisonDescription(Mode());
        calculate->setToolTip(QStringLiteral("选中后，点击开始模拟时一并计算%1收益；不选中则只模拟基准战斗。\n"
            "切换此开关不会启动任务；不作用于断点调试和单次记录。收益共需三组战斗，结果按比较模式分别保留。\n%2")
            .arg(GainComparisonName(Mode()), description));
        for (int i = 0; i < int(GameAttributeRows.size()); ++i) {
            const auto *gain = GameAttributeGain(result, i);
            if (gain && (gain->increment <= 0 || result.FightSeconds() <= 0)) gain = nullptr;
            gains_[i]->setText(gain ? QString::number(gain->stats.mean_damage / result.FightSeconds() - result.MeanDps(), 'f', 2) : QStringLiteral("--"));
            changes_[i]->setText(gain && result.MeanDps() > 0
                ? QString::number((gain->stats.mean_damage / result.FightSeconds() / result.MeanDps() - 1) * 100, 'f', 2) + '%'
                : QStringLiteral("--"));
            const bool state = !gain && (i == 0 || i == 2 || (result.gainsRequested && !result.Complete()));
            status_[i]->setText(MissingGainText(i, result));
            stacks_[i]->setCurrentIndex(state ? 1 : 0);
            gains_[i]->setStyleSheet(gain ? QStringLiteral("color: #ededf1;") : QStringLiteral("color: #92929d;"));
            const auto name = GameAttributeName(i, specialization_);
            rows_[i]->setAccessibleName(name + QStringLiteral("收益"));
            const auto tip = (gain ? QStringLiteral("%1 · %2：增加 %3 点%4。左列为秒伤增量，右列为总提升比例。")
                .arg(GainComparisonName(Mode()), name).arg(gain->increment).arg(gain->attribute)
                : name + QStringLiteral("：") + MissingGainText(i, result)) + '\n' + description;
            rows_[i]->setToolTip(tip); gains_[i]->setToolTip(tip); changes_[i]->setToolTip(tip); status_[i]->setToolTip(tip);
            rows_[i]->setAccessibleDescription(tip);
        }
    }
    SegmentedTabBar *mode_;
    Specialization specialization_ = Specialization::TaiXu;
    std::array<std::optional<SimulationResult>, 2> results_{};
    std::array<AttributeComparisonRow *, 10> rows_{};
    std::array<QStackedLayout *, 10> stacks_{};
    std::array<QLabel *, 10> gains_{}, changes_{}, status_{};
};
} // namespace desktop
