#include "history_panel.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

#include "game_icons.h"
#include "src/class/common/damage.hpp"

namespace {
void TrimGrid(const QGridLayout *layout, int count)
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
    QToolButton *button = nullptr;
    if (index < layout->count()) {
        button = static_cast<QToolButton *>(layout->itemAt(index)->widget());
    } else {
        button = new QToolButton;
        button->setIconSize({ 36, 36 });
        button->setFixedSize(42, 42);
        button->setAutoRaise(true);
        layout->addWidget(button, index / 5, index % 5);
    }
    const auto KEY = QString::number(static_cast<int>(specialization)) + name + ':' + badge;
    if (button->property("iconKey").toString() != KEY) {
        auto pixmap = desktop::GameIcon(specialization, name).pixmap(36, 36);
        if (!badge.isEmpty() && !pixmap.isNull()) {
            QPainter    painter(&pixmap);
            const QRect LABEL(0, 21, 36, 15);
            painter.fillRect(LABEL, QColor(0, 0, 0, 185));
            painter.setPen(Qt::white);
            auto font = painter.font();
            font.setPixelSize(11);
            font.setBold(true);
            painter.setFont(font);
            painter.drawText(LABEL.adjusted(0, 0, -2, 0), Qt::AlignRight | Qt::AlignVCenter, badge);
        }
        button->setIcon(QIcon(pixmap));
        button->setProperty("iconKey", KEY);
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
    m_time->setStyleSheet(QStringLiteral("font-size: 16px; color: #ededf1;"));
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
    const auto GRID = [&](const QString &title) {
        auto *group  = new QGroupBox(title);
        auto *result = new QGridLayout(group);
        result->setSpacing(2);
        result->setContentsMargins(5, 14, 5, 5);
        result->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        body->addWidget(group);
        return result;
    };
    m_self   = GRID(QStringLiteral("自身增益"));
    m_target = GRID(QStringLiteral("目标增益"));
    m_skills = GRID(QStringLiteral("技能冷却 / 充能"));
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
    const int FRAME = static_cast<int>(std::floor((milliseconds * 16 / 1000) + 1e-9));
    if (FRAME == m_frame) {
        return;
    }
    m_frame = FRAME;
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
    QString     resource;
    if (static_cast<int>(m_history.Class()) >= 0) {
        const int   values[]{ state.qidian, state.energy, state.rage };
        const auto &slotsList = School(m_history.Class()).resources;
        QStringList parts;
        for (std::size_t i = 0; i < slotsList.size(); ++i) {
            parts << QStringLiteral("%1 %2 / %3").arg(desktop::Text(slotsList[i].name)).arg(values[i]).arg(slotsList[i].high);
        }
        resource = parts.join(" · ");

    }
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
    resource +=
        QStringLiteral("\n目标气血 %1% · 距离 %2").arg(state.target_life * 100, 0, 'f', 2).arg(state.target_distance, 0, 'f', 2);
    m_resources->setText(resource);
    const auto percent = [](double value) {
        return QStringLiteral("%1 %").arg(value * 100, 0, 'f', 2);
    };
    const std::pair<QString, QString> VALUES[] = {
        { QStringLiteral("攻击"),       QString::number(a.attack)                                                                           },
        { QStringLiteral("武器伤害"),   QString::number(a.weapon)                                                                           },
        { QStringLiteral("会心几率"),   percent(a.criticalChance)                                                                           },
        { QStringLiteral("会心效果"),   percent(a.criticalMultiplier)                                                                       },
        { QStringLiteral("破防等级"),   QString::number(a.overcome)                                                                         },
        { QStringLiteral("无双"),       percent((JX3DPS::runtime::season50::Rating(a.strain, 7.117) / 1024.0) + (a.strainPercent / 1024.0)) },
        { QStringLiteral("破招等级"),   QString::number(a.surplus)                                                                          },
        { QStringLiteral("加速"),       percent(a.haste - 1.0)                                                                        },
        { QStringLiteral("伤害加成"),   percent(a.damagePercent / 1024.0)                                                                   },
        { QStringLiteral("非侠士加成"), percent(a.pvePercent / 1024.0)                                                                      },
        { QStringLiteral("无视防御"),   percent(a.ignoreShield / 1024.0)                                                                    }
    };
    int row = 0;
    for (const auto &[LABEL, value] : VALUES) {
        if (row < m_attributes->rowCount()) {
            static_cast<QLabel *>(m_attributes->itemAt(row, QFormLayout::FieldRole)->widget())->setText(value);
        } else {
            auto *field = new QLabel(value);
            field->setAlignment(Qt::AlignRight);
            m_attributes->addRow(LABEL, field);
        }
        ++row;
    }
    int        self_index   = 0;
    int        target_index = 0;
    int        skill_index  = 0;
    const auto CATALOG      = desktop::Describe(m_history.Class());
    for (desktop::rt::Slot slot = 0; slot < CATALOG.buffs.size(); ++slot) {
        for (const bool target : { false, true }) {
            const int STACKS = (target ? state.target_buff_stacks : state.self_buff_stacks)[slot];
            if (STACKS == 0) {
                continue;
            }
            const int  EXPIRES = (target ? state.target_buff_expires_at : state.self_buff_expires_at)[slot];
            const auto NAME    = desktop::DisplayName(CATALOG.buffs[slot].name);
            const auto DURATION =
                (EXPIRES == 0) || EXPIRES == std::numeric_limits<int>::max() ? QStringLiteral("常驻") : Remaining(EXPIRES, m_frame);
            Icon(target ? m_target : m_self,
                 target ? target_index++ : self_index++,
                 m_history.Class(),
                 NAME,
                 QString::number(STACKS),
                 QStringLiteral("%1\n%2 层 · 剩余 %3").arg(NAME).arg(STACKS).arg(DURATION));
        }
    }
    TrimGrid(m_self, self_index);
    TrimGrid(m_target, target_index);
    for (const auto slot : CATALOG.castable) {
        const auto NAME  = desktop::SkillName(m_history.Class(), slot);
        const int  READY = state.SkillReadyAt(slot);
        const auto BADGE = READY == std::numeric_limits<int>::max() ? QStringLiteral("--")
                           : READY > m_frame                        ? QString::number((READY - m_frame) / 16.0, 'f', 1)
                                                                    : QString{};
        Icon(m_skills,
             skill_index++,
             m_history.Class(),
             NAME,
             BADGE,
             QStringLiteral("%1\n冷却（含公共冷却）：%2\n充能：%3").arg(NAME, Remaining(READY, m_frame)).arg(state.skill_energy[slot]));
    }
    TrimGrid(m_skills, skill_index);
}
