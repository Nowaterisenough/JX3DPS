#include "history_panel.h"

#include <cmath>

#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

#include "game_icons.h"

namespace {
void TrimGrid(QGridLayout *layout, int count)
{
    for (int i = count; i < layout->count(); ++i) {
        layout->itemAt(i)->widget()->hide();
    }
    layout->parentWidget()->setVisible(count > 0);
}

QString Remaining(int deadline, int now)
{
    if (deadline == std::numeric_limits<int>::max()) {
        return QStringLiteral("不可用 / 常驻");
    }
    return QStringLiteral("%1 秒").arg(std::max(0, deadline - now) / 16.0, 0, 'f', 4);
}

void Icon(QGridLayout *layout, int index, desktop::Specialization specialization, const QString &name, const QString &badge, const QString &tip)
{
    QToolButton *button;
    if (index < layout->count()) {
        button = static_cast<QToolButton *>(layout->itemAt(index)->widget());
    } else {
        button = new QToolButton;
        button->setIconSize({ 36, 36 });
        button->setFixedSize(42, 42);
        button->setAutoRaise(true);
        layout->addWidget(button, index / 5, index % 5);
    }
    const auto key = QString::number(static_cast<int>(specialization)) + name + ':' + badge;
    if (button->property("iconKey").toString() != key) {
        auto pixmap = desktop::GameIcon(specialization, name).pixmap(36, 36);
        if (!badge.isEmpty() && !pixmap.isNull()) {
            QPainter    painter(&pixmap);
            const QRect label(0, 21, 36, 15);
            painter.fillRect(label, QColor(0, 0, 0, 185));
            painter.setPen(Qt::white);
            auto font = painter.font();
            font.setPixelSize(11);
            font.setBold(true);
            painter.setFont(font);
            painter.drawText(label.adjusted(0, 0, -2, 0), Qt::AlignRight | Qt::AlignVCenter, badge);
        }
        button->setIcon(QIcon(pixmap));
        button->setProperty("iconKey", key);
    }
    button->show();
    button->setAccessibleName(name);
    button->setToolTip(tip);
}
} // namespace

HistoryPanel::HistoryPanel(QWidget *parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("timelineHistory"));
    setMinimumWidth(265);
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 4, 4, 4);
    m_time = new QLabel(QStringLiteral("时间点属性"));
    m_time->setObjectName(QStringLiteral("historyTime"));
    m_time->setStyleSheet(QStringLiteral("font-size: 16px; color: #efbf5f;"));
    layout->addWidget(m_time);
    m_status = new QLabel;
    m_status->setWordWrap(true);
    layout->addWidget(m_status);
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    layout->addWidget(scroll, 1);
    m_content  = new QWidget;
    auto *body = new QVBoxLayout(m_content);
    body->setContentsMargins(0, 0, 6, 0);
    body->setAlignment(Qt::AlignTop);
    m_resources = new QLabel;
    m_resources->setWordWrap(true);
    body->addWidget(m_resources);
    auto *stats  = new QGroupBox(QStringLiteral("即时属性"));
    m_attributes = new QFormLayout(stats);
    m_attributes->setVerticalSpacing(3);
    stats->setToolTip(
        QStringLiteral("显示本场战斗的通用面板属性，包含当时动态增益；技能专属加成及持续伤害、影子快照不计入此面板。"));
    body->addWidget(stats);
    auto grid = [&](const QString &title) {
        auto *group  = new QGroupBox(title);
        auto *result = new QGridLayout(group);
        result->setSpacing(2);
        result->setContentsMargins(5, 14, 5, 5);
        result->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        body->addWidget(group);
        return result;
    };
    m_self   = grid(QStringLiteral("自身增益"));
    m_target = grid(QStringLiteral("目标增益"));
    m_skills = grid(QStringLiteral("技能冷却 / 充能"));
    scroll->setWidget(m_content);
    Clear();
}

void HistoryPanel::Clear()
{
    m_history.Clear();
    m_frame = -1;
    m_time->setText(QStringLiteral("时间点属性"));
    m_status->setText(QStringLiteral("记录一场战斗，移动鼠标查看时间轴上的状态。"));
    m_content->hide();
}

void HistoryPanel::Append(const desktop::HistoryChunk &chunk)
{
    m_history.Append(chunk);
    if (m_frame < 0) {
        m_frame = 0;
    }
    Refresh();
}

void HistoryPanel::Inspect(double milliseconds)
{
    const int frame = static_cast<int>(std::floor(milliseconds * 16 / 1000 + 1e-9));
    if (frame == m_frame) {
        return;
    }
    m_frame = frame;
    Refresh();
}

void HistoryPanel::Refresh()
{
    m_time->setText(QStringLiteral("%1 秒 · 第 %2 帧").arg(m_frame / 16.0, 0, 'f', 4).arg(m_frame));
    const auto snapshot = m_history.At(m_frame);
    m_content->setVisible(snapshot.has_value());
    if (!snapshot) {
        m_status->setText(m_history.Empty()
                              ? QStringLiteral("尚无战斗记录。")
                              : QStringLiteral("此时间尚未模拟，已记录至 %1 秒。").arg(m_history.ObservedFrame() / 16.0, 0, 'f', 4));
        return;
    }
    m_status->setText(desktop::ClassName(m_history.Class()) +
                      (m_history.Finished() ? QStringLiteral(" · 本场战斗历史状态") : QStringLiteral(" · 调试记录（当前帧仅含已执行部分）")));
    const auto &state = snapshot->state;
    const auto &a     = snapshot->attributes;
    QString     resource =
        m_history.Class() == desktop::Specialization::TaiXu
            ? QStringLiteral("气点 %1 / 10").arg(state.qidian)
            : QStringLiteral("%1 · 影子 %2 / 6")
                  .arg(a.style == 1 ? QStringLiteral("高山流水") : QStringLiteral("阳春白雪"))
                  .arg(a.shadows);
    if (state.preparing_skill != desktop::rt::INVALID_SLOT) {
        resource += QStringLiteral("\n运功 %1 · %2")
                        .arg(desktop::SkillName(m_history.Class(), state.preparing_skill),
                             Remaining(state.skill_prepare_at[state.preparing_skill], m_frame));
    }
    if (state.casting_skill != desktop::rt::INVALID_SLOT) {
        resource += QStringLiteral("\n引导 %1 · %2")
                        .arg(desktop::SkillName(m_history.Class(), state.casting_skill),
                             Remaining(state.skill_casting_at[state.casting_skill], m_frame));
    }
    m_resources->setText(resource);
    auto percent = [](double value) {
        return QStringLiteral("%1 %").arg(value * 100, 0, 'f', 2);
    };
    const std::pair<QString, QString> values[] = {
        { QStringLiteral("攻击"),       QString::number(a.attack)                                                                       },
        { QStringLiteral("武器伤害"),   QString::number(a.weapon)                                                                       },
        { QStringLiteral("会心几率"),   percent(a.criticalChance)                                                                       },
        { QStringLiteral("会心效果"),   percent(a.criticalMultiplier)                                                                   },
        { QStringLiteral("破防等级"),   QString::number(a.overcome)                                                                     },
        { QStringLiteral("无双"),       percent(static_cast<int>(a.strain * 1024 / (9.189 * 8250)) / 1024.0 + a.strainPercent / 1024.0) },
        { QStringLiteral("破招等级"),   QString::number(a.surplus)                                                                      },
        { QStringLiteral("加速"),       percent(1.0 / a.haste - 1)                                                                      },
        { QStringLiteral("伤害加成"),   percent(a.damagePercent / 1024.0)                                                               },
        { QStringLiteral("非侠士加成"), percent(a.pvePercent / 1024.0)                                                                  },
        { QStringLiteral("无视防御"),   percent(a.ignoreShield / 1024.0)                                                                }
    };
    int row = 0;
    for (const auto &[label, value] : values) {
        if (row < m_attributes->rowCount()) {
            static_cast<QLabel *>(m_attributes->itemAt(row, QFormLayout::FieldRole)->widget())->setText(value);
        } else {
            auto *field = new QLabel(value);
            field->setAlignment(Qt::AlignRight);
            m_attributes->addRow(label, field);
        }
        ++row;
    }
    int        selfIndex = 0, targetIndex = 0, skillIndex = 0;
    const auto catalog = desktop::Describe(m_history.Class());
    for (desktop::rt::Slot slot = 0; slot < catalog.buffs.size(); ++slot) {
        for (const bool target : { false, true }) {
            const int stacks = (target ? state.target_buff_stacks : state.self_buff_stacks)[slot];
            if (!stacks) {
                continue;
            }
            const int  expires = (target ? state.target_buff_expires_at : state.self_buff_expires_at)[slot];
            const auto name    = desktop::DisplayName(catalog.buffs[slot].name);
            const auto duration =
                !expires || expires == std::numeric_limits<int>::max() ? QStringLiteral("常驻") : Remaining(expires, m_frame);
            Icon(target ? m_target : m_self,
                 target ? targetIndex++ : selfIndex++,
                 m_history.Class(),
                 name,
                 QString::number(stacks),
                 QStringLiteral("%1\n%2 层 · 剩余 %3").arg(name).arg(stacks).arg(duration));
        }
    }
    TrimGrid(m_self, selfIndex);
    TrimGrid(m_target, targetIndex);
    for (const auto slot : catalog.castable) {
        const auto name  = desktop::SkillName(m_history.Class(), slot);
        const int  ready = state.SkillReadyAt(slot);
        const auto badge = ready == std::numeric_limits<int>::max() ? QStringLiteral("--")
                           : ready > m_frame                        ? QString::number((ready - m_frame) / 16.0, 'f', 1)
                                                                    : QString{};
        Icon(m_skills,
             skillIndex++,
             m_history.Class(),
             name,
             badge,
             QStringLiteral("%1\n冷却（含公共冷却）：%2\n充能：%3").arg(name, Remaining(ready, m_frame)).arg(state.skill_energy[slot]));
    }
    TrimGrid(m_skills, skillIndex);
}
