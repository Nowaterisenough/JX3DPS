#include "player_state_panel.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>

struct PlayerStatePanel::Impl
{
    PlayerState state = {};
    bool hasData = false;

    static void DrawProgressBar(QPainter &painter, int x, int y, int width, int height,
                               double percent, const QColor &color, const QString &label);
};

PlayerStatePanel::PlayerStatePanel(QWidget *parent)
    : QWidget(parent)
    , d(std::make_unique<Impl>())
{
    setMinimumSize(250, 300);
    setMinimumHeight(300);
}

PlayerStatePanel::~PlayerStatePanel() = default;

void PlayerStatePanel::UpdateState(const PlayerState &state)
{
    d->state = state;
    d->hasData = true;
    update();
}

void PlayerStatePanel::SetCursorTime(int frame, double seconds)
{
    if (!d->hasData) return;
    d->state.currentFrame = frame;
    d->state.currentSeconds = seconds;
    update();
}

void PlayerStatePanel::Clear()
{
    d->hasData = false;
    d->state = {};
    update();
}

void PlayerStatePanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景
    painter.fillRect(rect(), QColor(30, 30, 30));

    if (!d->hasData) {
        painter.setPen(QColor(150, 150, 150));
        painter.drawText(rect(), Qt::AlignCenter, "等待调试...");
        return;
    }

    int y = 20;
    const int lineHeight = 22;
    const int leftMargin = 10;

    // 标题
    QFont titleFont = painter.font();
    titleFont.setBold(true);
    titleFont.setPointSize(10);
    painter.setFont(titleFont);
    painter.setPen(QColor(100, 200, 255));
    painter.drawText(leftMargin, y, "玩家状态");
    y += lineHeight + 5;

    // 普通字体
    QFont normalFont = painter.font();
    normalFont.setBold(false);
    normalFont.setPointSize(9);
    painter.setFont(normalFont);

    // 时间信息
    painter.setPen(QColor(180, 180, 180));
    painter.drawText(leftMargin, y, QString("时间：%1 秒（第 %2 帧）")
        .arg(d->state.currentSeconds, 0, 'f', 2)
        .arg(d->state.currentFrame));
    y += lineHeight;

    // 当前宏
    if (!d->state.currentMacro.isEmpty()) {
        painter.drawText(leftMargin, y, QString("宏: %1").arg(d->state.currentMacro));
        y += lineHeight;
    }

    // 当前技能
    if (!d->state.lastSkill.isEmpty()) {
        painter.setPen(QColor(255, 200, 100));
        painter.drawText(leftMargin, y, QString("技能: %1").arg(d->state.lastSkill));
        y += lineHeight;
    }

    y += 5;

    // 生命值和内力
    Impl::DrawProgressBar(painter, leftMargin, y, width() - 20, 20,
                          d->state.lifePercent, QColor(200, 50, 50), "生命");
    y += 30;

    Impl::DrawProgressBar(painter, leftMargin, y, width() - 20, 20,
                          d->state.manaPercent, QColor(50, 100, 200), "内力");
    y += 35;

    // 资源信息
    painter.setPen(QColor(180, 180, 180));
    painter.drawText(leftMargin, y, QString("气点 %1   怒气 %2   能量 %3")
        .arg(d->state.qidian).arg(d->state.rage).arg(d->state.energy));
    y += lineHeight + 5;

    // 目标信息
    painter.setPen(QColor(100, 200, 255));
    titleFont.setPointSize(9);
    painter.setFont(titleFont);
    painter.drawText(leftMargin, y, "目标");
    y += lineHeight;

    painter.setFont(normalFont);
    painter.setPen(QColor(180, 180, 180));
    painter.drawText(leftMargin, y, QString("目标编号：%1").arg(d->state.targetId));
    y += lineHeight;

    Impl::DrawProgressBar(painter, leftMargin, y, width() - 20, 20,
                          d->state.targetLifePercent, QColor(150, 100, 50), "血量");
}

void PlayerStatePanel::Impl::DrawProgressBar(QPainter &painter, int x, int y, int width, int height,
                                             double percent, const QColor &color, const QString &label)
{
    // 背景
    painter.fillRect(x, y, width, height, QColor(50, 50, 50));

    // 进度条
    int fillWidth = static_cast<int>(width * percent);
    painter.fillRect(x, y, fillWidth, height, color);

    // 边框
    painter.setPen(QColor(80, 80, 80));
    painter.drawRect(x, y, width, height);

    // 文本
    painter.setPen(QColor(255, 255, 255));
    QString text = QString("%1: %2%").arg(label).arg(static_cast<int>(percent * 100));
    painter.drawText(x, y, width, height, Qt::AlignCenter, text);
}
