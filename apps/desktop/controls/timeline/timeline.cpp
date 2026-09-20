#include "timeline.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainterPath>
#include <QScrollBar>
#include <QDebug>
#include <QtMath>
#include <QTimer>
#include <QApplication>
#include <QToolTip>
#include <cmath>

// ============================================================================
// TimelinePrivate
// ============================================================================

class TimelinePrivate
{
public:
    explicit TimelinePrivate(Timeline *q) :
        q_ptr(q),
        thumbnailHeight(60),
        mainViewHeight(100),  // 减小主视图高度，从180减到100
        buffAreaHeight(0),     // 默认不显示buff区域
        buffRowHeight(30),
        spacing(10),
        totalDuration(0),
        viewStartMs(0),
        viewEndMs(10000),
        zoomLevel(1.0),
        dragging(false),
        draggingIndicator(false),
        dragStartX(0),
        dragStartViewMs(0),
        hoveredEventIndex(-1),
        hoverScale(1.0),
        hoverAnimationTimer(nullptr)
    {
    }

    // 数据
    QVector<Timeline::EventItem> events;
    QVector<Timeline::BuffItem> buffs;

    // 布局参数
    int thumbnailHeight;   // 缩略图高度
    int mainViewHeight;    // 主视图高度
    int buffAreaHeight;    // Buff区域总高度（根据buff数量动态计算）
    int buffRowHeight;     // 每个buff行的高度
    int spacing;           // 间距

    // 时间范围
    int totalDuration;     // 总时长（毫秒）
    int viewStartMs;       // 当前可见范围起点
    int viewEndMs;         // 当前可见范围终点
    qreal zoomLevel;       // 缩放级别 (1.0 = 100%)

    // 交互状态
    bool dragging;
    bool draggingIndicator;  // 是否正在拖动缩略图指示器
    int dragStartX;
    int dragStartViewMs;
    QPoint pressPosition;
    int pressedEventIndex = -1;
    bool movedSincePress = false;
    QPoint cursorPosition{-1, -1};
    double cursorMs = -1;
    int hoveredEventIndex;   // 当前悬停的事件索引 (-1表示无)
    qreal hoverScale;        // 悬停图标的缩放比例 (1.0 - 1.15)
    QTimer *hoverAnimationTimer; // 悬停动画定时器

    // 区域矩形
    QRect thumbnailRect;
    QRect mainViewRect;
    QRect buffAreaRect;      // Buff区域矩形
    QRect visibleIndicator;  // 缩略图中的可见区域指示器

private:
    Timeline *q_ptr;
    Q_DECLARE_PUBLIC(Timeline)
};

// ============================================================================
// Timeline Implementation
// ============================================================================

Timeline::Timeline(QWidget *parent) :
    QWidget(parent),
    d_ptr(new TimelinePrivate(this))
{
    SetupUI();
}

Timeline::~Timeline() = default;

void Timeline::SetupUI()
{
    Q_D(Timeline);

    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMouseTracking(true);

    // 设置样式
    setStyleSheet(R"(
        Timeline {
            background-color: #1e1e1e;
        }
    )");

    // 初始化悬停动画定时器
    d->hoverAnimationTimer = new QTimer(this);
    d->hoverAnimationTimer->setInterval(16);  // 约60fps
    connect(d->hoverAnimationTimer, &QTimer::timeout, this, [this]() {
        Q_D(Timeline);
        const qreal targetScale = 1.15;  // 目标缩放比例
        const qreal speed = 0.15;         // 动画速度

        if (d->hoveredEventIndex >= 0) {
            // 放大
            if (d->hoverScale < targetScale) {
                d->hoverScale += speed;
                if (d->hoverScale >= targetScale) {
                    d->hoverScale = targetScale;
                    d->hoverAnimationTimer->stop();  // 动画完成，停止定时器
                }
                update();
            } else {
                d->hoverAnimationTimer->stop();  // 已经达到目标，停止定时器
            }
        } else {
            // 缩小
            if (d->hoverScale > 1.0) {
                d->hoverScale -= speed;
                if (d->hoverScale <= 1.0) {
                    d->hoverScale = 1.0;
                    d->hoverAnimationTimer->stop();
                }
                update();
            } else {
                d->hoverAnimationTimer->stop();  // 已经恢复，停止定时器
            }
        }
    });
}

void Timeline::SetEvents(const QVector<EventItem> &events)
{
    Q_D(Timeline);
    d->events = events;
    d->hoveredEventIndex = -1;
    d->pressedEventIndex = -1;

    // 计算总时长
    d->totalDuration = 0;
    for (const auto &event : events) {
        if (event.timestamp > d->totalDuration) {
            d->totalDuration = qCeil(event.timestamp);
        }
    }

    // 初始化视图范围
    d->viewStartMs = 0;
    d->viewEndMs = d->totalDuration;

    UpdateLayout();
    update();
}

void Timeline::AddEvent(const EventItem &event)
{
    Q_D(Timeline);
    d->events.append(event);

    // 更新总时长
    if (event.timestamp > d->totalDuration) {
        d->totalDuration = qCeil(event.timestamp);
        // 自动扩展视图范围以包含新事件
        if (event.timestamp > d->viewEndMs) {
            d->viewEndMs = qCeil(event.timestamp);
        }
    }

    UpdateLayout();
    update();
}

void Timeline::SetBuffs(const QVector<BuffItem> &buffs)
{
    Q_D(Timeline);
    d->buffs = buffs;

    // 根据buff数量调整控件最小高度
    int requiredHeight = d->thumbnailHeight + d->spacing + d->mainViewHeight;
    if (!buffs.isEmpty()) {
        requiredHeight += d->spacing + buffs.size() * d->buffRowHeight;
    }
    setMinimumHeight(qMax(210, requiredHeight + 2));

    UpdateLayout();
    update();
}

void Timeline::Clear()
{
    Q_D(Timeline);
    d->events.clear();
    d->cursorPosition = {-1, -1};
    d->cursorMs = -1;
    emit CursorLeft();
    d->buffs.clear();
    d->totalDuration = 0;
    d->viewStartMs = 0;
    d->viewEndMs = 10000;
    d->hoveredEventIndex = -1;
    d->pressedEventIndex = -1;
    d->dragging = false;
    d->draggingIndicator = false;
    d->hoverScale = 1.0;
    d->hoverAnimationTimer->stop();
    d->visibleIndicator = QRect();
    setMinimumHeight(210);
    setCursor(Qt::ArrowCursor);
    UpdateLayout();
    update();
}

void Timeline::SetTimeRange(int startMs, int endMs)
{
    Q_D(Timeline);
    if (startMs < 0 || endMs <= startMs) return;
    d->totalDuration = qMax(endMs, d->totalDuration);
    d->viewStartMs = startMs;
    d->viewEndMs = endMs;
    UpdateLayout();
    update();
    emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
}

void Timeline::ZoomIn()
{
    Q_D(Timeline);
    if (d->totalDuration <= 0) return;
    d->zoomLevel *= 1.2;
    int duration = d->viewEndMs - d->viewStartMs;
    int newDuration = qMax(1, static_cast<int>(duration / 1.2));
    int center = (d->viewStartMs + d->viewEndMs) / 2;

    newDuration = qMin(newDuration, d->totalDuration);
    d->viewStartMs = qBound(0, center - newDuration / 2, d->totalDuration - newDuration);
    d->viewEndMs = d->viewStartMs + newDuration;

    UpdateLayout();
    update();
    emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
}

void Timeline::ZoomOut()
{
    Q_D(Timeline);
    if (d->totalDuration <= 0) return;
    d->zoomLevel /= 1.2;
    int duration = d->viewEndMs - d->viewStartMs;
    int newDuration = qMin(d->totalDuration, qMax(duration + 1, static_cast<int>(duration * 1.2)));
    int center = (d->viewStartMs + d->viewEndMs) / 2;

    d->viewStartMs = qBound(0, center - newDuration / 2, d->totalDuration - newDuration);
    d->viewEndMs = d->viewStartMs + newDuration;

    UpdateLayout();
    update();
    emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
}

void Timeline::ResetZoom()
{
    Q_D(Timeline);
    d->zoomLevel = 1.0;
    d->viewStartMs = 0;
    d->viewEndMs = d->totalDuration;

    UpdateLayout();
    update();
    emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
}

void Timeline::UpdateLayout()
{
    Q_D(Timeline);

    int w = width();
    int h = height();

    // 计算buff区域高度
    d->buffAreaHeight = d->buffs.isEmpty() ? 0 : d->buffs.size() * d->buffRowHeight;

    // 缩略图区域（上方）
    d->thumbnailRect = QRect(0, 0, w, d->thumbnailHeight);

    // 主视图区域（中间）
    int mainViewTop = d->thumbnailHeight + d->spacing;
    const int labelWidth = d->buffs.isEmpty() ? 0 : qMin(40, w / 3);
    d->mainViewRect = QRect(labelWidth, mainViewTop, w - labelWidth, d->mainViewHeight);

    // Buff区域（下方）
    if (d->buffAreaHeight > 0) {
        int buffAreaTop = mainViewTop + d->mainViewHeight + d->spacing;
        d->buffAreaRect = QRect(0, buffAreaTop, w, d->buffAreaHeight);
    } else {
        d->buffAreaRect = QRect();
    }

    // 计算可见区域指示器
    if (d->totalDuration > 0) {
        qreal startRatio = qreal(d->viewStartMs) / d->totalDuration;
        qreal endRatio = qreal(d->viewEndMs) / d->totalDuration;

        int indicatorX = startRatio * w;
        int indicatorWidth = (endRatio - startRatio) * w;

        d->visibleIndicator = QRect(indicatorX, 0, indicatorWidth, d->thumbnailHeight);
    }
    UpdateCursor(d->cursorPosition);
}

void Timeline::DrawThumbnail(QPainter &painter)
{
    Q_D(Timeline);

    painter.save();
    painter.setClipRect(d->thumbnailRect);

    // 背景
    painter.fillRect(d->thumbnailRect, QColor(30, 30, 30));

        // 绘制宏覆盖带（底部）
        const int macroBarHeight = 8;
        const int macroBarY = d->thumbnailRect.bottom() - macroBarHeight - 2;

        if (d->totalDuration > 0 && !d->events.isEmpty()) {
            painter.setPen(Qt::NoPen);

            // 收集宏覆盖区间
            QString currentMacro;
            int segmentStart = 0;
            QColor currentColor;

            for (int i = 0; i < d->events.size(); ++i) {
                const auto &event = d->events[i];

                // 检测宏切换
                if (event.macroName != currentMacro) {
                    // 绘制上一个宏的区间
                    if (!currentMacro.isEmpty() && i > 0) {
                        qreal startRatio = qreal(segmentStart) / d->totalDuration;
                        qreal endRatio = qreal(event.timestamp) / d->totalDuration;
                        int x1 = startRatio * width();
                        int x2 = endRatio * width();

                        painter.setBrush(currentColor);
                        painter.drawRect(x1, macroBarY, x2 - x1, macroBarHeight);
                    }

                    // 开始新宏区间
                    currentMacro = event.macroName;
                    currentColor = event.macroColor;
                    segmentStart = event.timestamp;
                }
            }

            // 绘制最后一个宏区间
            if (!currentMacro.isEmpty()) {
                qreal startRatio = qreal(segmentStart) / d->totalDuration;
                int x1 = startRatio * width();
                int x2 = width();

                painter.setBrush(currentColor);
                painter.drawRect(x1, macroBarY, x2 - x1, macroBarHeight);
            }
        }

        // 绘制所有事件（缩略图 - 使用小图标）
        if (d->totalDuration > 0) {
            painter.setPen(Qt::NoPen);
            const int miniIconSize = 16;  // 缩略图中的小图标尺寸
            const int iconY = d->thumbnailRect.top() + (d->thumbnailRect.height() - 12) / 2;  // 调整图标位置，为底部宏条留空间

            for (const auto &event : d->events) {
                qreal ratio = qreal(event.timestamp) / d->totalDuration;
                int x = ratio * width();

                if (!event.icon.isNull()) {
                    // 绘制小图标
                    QPixmap scaledIcon = event.icon.scaled(miniIconSize, miniIconSize,
                                                           Qt::KeepAspectRatio,
                                                           Qt::SmoothTransformation);
                    painter.drawPixmap(x - miniIconSize / 2, iconY - miniIconSize / 2,
                                      miniIconSize, miniIconSize, scaledIcon);
                } else {
                    // 无图标时绘制小竖线
                    painter.setPen(QPen(event.color, 2));
                    painter.drawLine(x, d->thumbnailRect.top() + 10, x, macroBarY - 2);
                    painter.setPen(Qt::NoPen);
                }
            }
        }

        // 绘制可见区域指示器
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(100, 100, 100, 80));
        painter.drawRect(d->visibleIndicator);

        painter.setPen(QPen(QColor(117, 190, 255), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(d->visibleIndicator);

    // 边框
    painter.setPen(QColor(60, 60, 60));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(d->thumbnailRect);

    painter.restore();
}

void Timeline::DrawMainView(QPainter &painter)
{
    Q_D(Timeline);

    painter.save();
    painter.setClipRect(d->mainViewRect);

    // 背景
    painter.fillRect(d->mainViewRect, QColor(40, 40, 40));

    // 绘制时间刻度（顶部，以秒为单位）
    int viewDuration = d->viewEndMs - d->viewStartMs;
    if (viewDuration > 0) {
        const double desired = qMax(1.0, viewDuration * 70.0 / qMax(1, d->mainViewRect.width()));
        const double magnitude = std::pow(10.0, std::floor(std::log10(desired)));
        const int tickInterval = static_cast<int>(magnitude * (desired <= magnitude ? 1 : desired <= 2 * magnitude ? 2 : desired <= 5 * magnitude ? 5 : 10));

        painter.setPen(QColor(165, 170, 180));
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);

        for (int ms = (d->viewStartMs / tickInterval) * tickInterval; ms <= d->viewEndMs; ms += tickInterval) {
            qreal ratio = qreal(ms - d->viewStartMs) / viewDuration;
            int x = d->mainViewRect.left() + ratio * d->mainViewRect.width();

            // 刻度线（从顶部向下）
            painter.drawLine(x, d->mainViewRect.top(), x, d->mainViewRect.top() + 20);

            // 时间标签（顶部）
            QString timeLabel = QString::number(ms / 1000.0, 'f', tickInterval < 1000 ? 3 : 0) + QStringLiteral("秒");
            const int labelX = qBound(d->mainViewRect.left(), x - 30, qMax(d->mainViewRect.left(), d->mainViewRect.right() - 59));
            painter.drawText(labelX, d->mainViewRect.top() + 5, 60, 15, Qt::AlignCenter, timeLabel);
        }
    }

    // 绘制所有事件图标（正常状态，跳过hover的）
    painter.setPen(Qt::NoPen);
    const int iconSize = 40;
    const int iconY = d->mainViewRect.top() + 30;

    if (viewDuration <= 0) { painter.restore(); return; }
    for (int i = 0; i < d->events.size(); ++i) {
        // 跳过悬停的图标（稍后单独绘制）
        if (i == d->hoveredEventIndex) {
            continue;
        }

        const auto &event = d->events[i];

        // 只绘制可见范围内的事件（性能优化）
        if (event.timestamp < d->viewStartMs || event.timestamp > d->viewEndMs) {
            continue;
        }

        qreal ratio = qreal(event.timestamp - d->viewStartMs) / viewDuration;
        int x = d->mainViewRect.left() + ratio * d->mainViewRect.width();

        // 绘制事件图标
        if (!event.icon.isNull()) {
            // 裁剪掉图标最外3圈像素，并绘制
            QRect sourceRect(3, 3, event.icon.width() - 6, event.icon.height() - 6);
            QRect targetRect(x - iconSize / 2 + 3, iconY + 3, iconSize - 6, iconSize - 6);
            painter.drawPixmap(targetRect, event.icon, sourceRect);

            // 绘制灰黑色边框（1像素宽，在裁剪后图标的内侧）
            painter.setPen(QPen(QColor(60, 60, 60), 1));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(targetRect.adjusted(0.5, 0.5, -0.5, -0.5), 3, 3);
            painter.setPen(Qt::NoPen);
        } else {
            // 无图标时绘制简单标记
            painter.setBrush(event.color);
            painter.drawRect(x - 2, iconY, 4, 40);
        }
    }

    painter.restore();
}

void Timeline::DrawBuffArea(QPainter &painter)
{
    Q_D(Timeline);

    if (!d->buffs.isEmpty() && d->buffAreaRect.isValid()) {
        painter.save();
        painter.setClipRect(d->buffAreaRect);

        // 背景
        painter.fillRect(d->buffAreaRect, QColor(35, 35, 35));

        int viewDuration = d->viewEndMs - d->viewStartMs;

        // 绘制每个buff行
        for (int i = 0; i < d->buffs.size(); ++i) {
            const auto &buff = d->buffs[i];
            int rowY = d->buffAreaRect.top() + i * d->buffRowHeight;

            // 绘制buff图标和名称
            if (!buff.icon.isNull()) {
                int buffIconSize = d->buffRowHeight - 6;
                QPixmap scaledIcon = buff.icon.scaled(buffIconSize, buffIconSize,
                                                       Qt::KeepAspectRatio,
                                                       Qt::SmoothTransformation);
                painter.drawPixmap(5, rowY + 3, buffIconSize, buffIconSize, scaledIcon);
            }

            // 绘制覆盖条区域（从buff名称右侧开始）
            int barStartX = d->mainViewRect.left();
            int barWidth = d->mainViewRect.width();
            int barY = rowY + 5;
            int barHeight = d->buffRowHeight - 10;

            // 绘制背景条
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(60, 60, 60));
            painter.drawRect(barStartX, barY, barWidth, barHeight);

            // 绘制buff覆盖时间段
            if (viewDuration > 0) {
                painter.setBrush(buff.color);

                for (const auto &segment : buff.segments) {
                    // 只绘制可见范围内的部分
                    const double segStart = qMax(segment.startMs, double(d->viewStartMs));
                    const double segEnd = qMin(segment.endMs, double(d->viewEndMs));

                    if (segStart < segEnd) {
                        qreal startRatio = qreal(segStart - d->viewStartMs) / viewDuration;
                        qreal endRatio = qreal(segEnd - d->viewStartMs) / viewDuration;

                        int x1 = barStartX + startRatio * barWidth;
                        int x2 = barStartX + endRatio * barWidth;
                        int w = x2 - x1;

                        // 绘制覆盖条
                        painter.setPen(Qt::NoPen);
                        painter.setBrush(buff.color);
                        painter.drawRect(x1, barY, w, barHeight);
                        if (w >= 20) {
                            painter.setPen(QColor(10, 14, 20));
                            painter.drawText(QRect(x1, barY, w, barHeight), Qt::AlignCenter, QString::number(segment.stacks));
                        }
                    }
                }
            }

            // 绘制行分割线
            if (i < d->buffs.size() - 1) {
                painter.setPen(QColor(50, 50, 50));
                painter.drawLine(d->buffAreaRect.left(), rowY + d->buffRowHeight,
                               d->buffAreaRect.right(), rowY + d->buffRowHeight);
            }
        }

        // 边框
        painter.setPen(QColor(60, 60, 60));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(d->buffAreaRect);

        painter.restore();
    }
}

void Timeline::DrawHoverEffects(QPainter &painter)
{
    Q_D(Timeline);
    painter.save();
    if (d->cursorMs >= 0) {
        const int x = d->cursorPosition.x();
        const int bottom = d->buffAreaRect.isValid() ? d->buffAreaRect.bottom() : d->mainViewRect.bottom();
        painter.setPen(QPen(QColor(239, 191, 95), 1));
        painter.drawLine(x, d->mainViewRect.top(), x, bottom);
        const QString time = QStringLiteral("%1 秒 · 第 %2 帧")
            .arg(d->cursorMs / 1000.0, 0, 'f', 4).arg(static_cast<int>(d->cursorMs * 16 / 1000));
        const int width = painter.fontMetrics().horizontalAdvance(time) + 16;
        const int left = qBound(0, x - width / 2, qMax(0, this->width() - width));
        const QRect label(left, d->mainViewRect.top() - 27, width, 25);
        painter.fillRect(label, QColor(35, 39, 46));
        painter.drawText(label, Qt::AlignCenter, time);
    }
    const int duration = d->viewEndMs - d->viewStartMs;
    if (d->hoveredEventIndex >= 0 && d->hoveredEventIndex < d->events.size() && duration > 0) {
        const auto &event = d->events[d->hoveredEventIndex];
        if (event.timestamp >= d->viewStartMs && event.timestamp <= d->viewEndMs) {
            const int x = d->mainViewRect.left() + double(event.timestamp - d->viewStartMs) / duration * d->mainViewRect.width();
            const int size = 40 * d->hoverScale;
            const QRect iconRect(x - size / 2, d->mainViewRect.top() + 30 - (size - 40) / 2, size, size);
            painter.setClipRect(d->mainViewRect);
            painter.drawPixmap(iconRect, event.icon);
            painter.setPen(QPen(QColor(239, 191, 95), 2));
            painter.drawRect(iconRect);
        }
    }
    painter.restore();
}

void Timeline::DrawHoverTooltip(QPainter &painter)
{
    Q_D(Timeline);
    const double FRAME_MS = 62.5;  // JX3逻辑帧：16帧/秒 = 62.5毫秒/帧

    if (d->hoveredEventIndex >= 0 && d->hoveredEventIndex < d->events.size()) {
        const auto &hoveredEvent = d->events[d->hoveredEventIndex];
        int viewDuration = d->viewEndMs - d->viewStartMs;

        if (hoveredEvent.timestamp >= d->viewStartMs && hoveredEvent.timestamp <= d->viewEndMs && viewDuration > 0) {
            qreal ratio = qreal(hoveredEvent.timestamp - d->viewStartMs) / viewDuration;
            int hoverX = d->mainViewRect.left() + ratio * d->mainViewRect.width();
            const int iconSize = 40;
            const int iconY = d->mainViewRect.top() + 30;

            // 收集80%以上重叠的事件
            QVector<int> overlappedIndices;
            overlappedIndices.append(d->hoveredEventIndex);

            for (int i = 0; i < d->events.size(); ++i) {
                if (i == d->hoveredEventIndex) continue;
                const auto &otherEvent = d->events[i];

                if (otherEvent.timestamp < d->viewStartMs || otherEvent.timestamp > d->viewEndMs) {
                    continue;
                }

                qreal otherRatio = qreal(otherEvent.timestamp - d->viewStartMs) / viewDuration;
                int otherX = d->mainViewRect.left() + otherRatio * d->mainViewRect.width();

                // 计算重叠度
                int distance = qAbs(hoverX - otherX);
                qreal overlapRatio = 1.0 - qreal(distance) / iconSize;

                if (overlapRatio >= 0.8) {
                    overlappedIndices.append(i);
                }
            }

            // 构建浮窗内容（不显示图标，只显示文字）
            QStringList tooltipLines;
            for (int idx : overlappedIndices) {
                const auto &evt = d->events[idx];
                QString line = QString("%1 | %2 秒 (%3 帧) | 伤害:%4 | %5")
                    .arg(evt.name)
                    .arg(evt.timestamp / 1000.0, 0, 'f', 2)
                    .arg(qRound(evt.timestamp / FRAME_MS))
                    .arg(evt.damage)
                    .arg(evt.rollResult == 1 ? "普通" : evt.rollResult == 2 ? "会心" : evt.rollResult == 3 ? "识破" : "未命中");
                tooltipLines.append(line);
            }

            // 计算浮窗尺寸
            QFont popupFont = painter.font();
            popupFont.setPointSize(9);
            painter.setFont(popupFont);
            QFontMetrics fm(popupFont);

            int maxWidth = 0;
            for (const QString &line : tooltipLines) {
                maxWidth = qMax(maxWidth, fm.horizontalAdvance(line));
            }

            int popupWidth = maxWidth + 20;
            int popupHeight = fm.height() * tooltipLines.size() + 16;

            // 计算浮窗位置（在图标正下方，居中对齐）
            int popupX = hoverX - popupWidth / 2;
            int scaledSize = iconSize * d->hoverScale;
            int popupY = iconY + scaledSize + 10;  // 在放大图标下方

            // 边界检查
            if (popupX < d->mainViewRect.left()) {
                popupX = d->mainViewRect.left() + 5;
            }
            if (popupX + popupWidth > d->mainViewRect.right()) {
                popupX = d->mainViewRect.right() - popupWidth - 5;
            }
            if (popupY + popupHeight > height()) {
                popupY = iconY - popupHeight - 10;  // 移到图标上方
            }

            QRect popupRect(popupX, popupY, popupWidth, popupHeight);

            painter.save();

            // 绘制渐变阴影（多层，增加层次感，更浅）
            painter.setPen(Qt::NoPen);
            for (int i = 5; i > 0; --i) {
                int alpha = 10 + (5 - i) * 8; // 更浅的渐变透明度
                painter.setBrush(QColor(0, 0, 0, alpha));
                painter.drawRect(popupRect.adjusted(-i, -i, i, i));
            }

            // 绘制浮窗背景
            painter.setBrush(QColor(45, 45, 48, 240));
            painter.drawRect(popupRect);

            // 绘制边框（深灰色，0.5像素宽）
            painter.setPen(QPen(QColor(120, 120, 120), 0.5));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(popupRect);

            // 绘制文字（高亮悬停的那一行，居中对齐）
            int textY = popupY + 8 + fm.ascent();
            for (int i = 0; i < tooltipLines.size(); ++i) {
                // 悬停的事件用金色高亮
                if (overlappedIndices[i] == d->hoveredEventIndex) {
                    painter.setPen(QColor(255, 215, 0));
                } else {
                    painter.setPen(QColor(200, 200, 200));
                }
                painter.drawText(popupX + 10, textY, tooltipLines[i]);
                textY += fm.height();
            }

            painter.restore();
        }
    }
}

void Timeline::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    DrawThumbnail(painter);
    DrawMainView(painter);
    DrawBuffArea(painter);
    DrawHoverEffects(painter);
    DrawHoverTooltip(painter);
}

void Timeline::resizeEvent(QResizeEvent *event)
{
    UpdateLayout();
    QWidget::resizeEvent(event);
}

void Timeline::wheelEvent(QWheelEvent *event)
{
    Q_D(Timeline);

    // 滚轮缩放
    if (event->angleDelta().y() > 0) {
        ZoomIn();
    } else {
        ZoomOut();
    }

    event->accept();
}

int Timeline::EventAt(const QPoint &position) const
{
    Q_D(const Timeline);
    const int duration = d->viewEndMs - d->viewStartMs;
    if (duration <= 0 || !d->mainViewRect.contains(position)) return -1;
    // Match icon drawing order, including several hits at the same timestamp.
    for (int index = static_cast<int>(d->events.size()) - 1; index >= 0; --index) {
        const auto timestamp = d->events[index].timestamp;
        if (timestamp < d->viewStartMs || timestamp > d->viewEndMs) continue;
        const int x = d->mainViewRect.left() + qreal(timestamp - d->viewStartMs) / duration * d->mainViewRect.width();
        if (QRect(x - 20, d->mainViewRect.top() + 30, 40, 40).contains(position)) return index;
    }
    return -1;
}

void Timeline::mousePressEvent(QMouseEvent *event)
{
    Q_D(Timeline);

    if (event->button() == Qt::LeftButton) {
        d->pressPosition = event->pos();
        d->pressedEventIndex = EventAt(event->pos());
        d->movedSincePress = false;
        // 点击缩略图
        if (d->thumbnailRect.contains(event->pos())) {
            // 判断是否点击在可见指示器上
            if (d->visibleIndicator.contains(event->pos())) {
                // 拖动指示器
                d->draggingIndicator = true;
                d->dragStartX = event->pos().x();
                d->dragStartViewMs = d->viewStartMs;
                setCursor(Qt::ClosedHandCursor);
            } else {
                // 点击缩略图空白区域 - 跳转到对应位置
                qreal ratio = qreal(event->pos().x()) / width();
                int centerMs = ratio * d->totalDuration;
                int halfDuration = (d->viewEndMs - d->viewStartMs) / 2;

                d->viewStartMs = qMax(0, centerMs - halfDuration);
                d->viewEndMs = qMin(d->totalDuration, centerMs + halfDuration);

                UpdateLayout();
                update();
                emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
            }
        }
        // 点击主视图 - 开始拖动或选择事件
        else if (d->mainViewRect.contains(event->pos())) {
            d->dragging = true;
            d->dragStartX = event->pos().x();
            d->dragStartViewMs = d->viewStartMs;
            setCursor(Qt::ClosedHandCursor);
        }
    }
}

void Timeline::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(Timeline);
    d->cursorPosition = event->pos();
    if ((d->dragging || d->draggingIndicator) &&
        (event->pos() - d->pressPosition).manhattanLength() >= QApplication::startDragDistance())
        d->movedSincePress = true;

    // 拖动缩略图指示器
    if (d->draggingIndicator) {
        int deltaX = event->pos().x() - d->dragStartX;
        int viewDuration = d->viewEndMs - d->viewStartMs;

        // 根据拖动距离计算时间偏移（缩略图映射到整个时间轴）
        qreal deltaRatio = qreal(deltaX) / width();
        int deltaMs = deltaRatio * d->totalDuration;

        d->viewStartMs = d->dragStartViewMs + deltaMs;
        d->viewEndMs = d->viewStartMs + viewDuration;

        // 边界限制
        if (d->viewStartMs < 0) {
            d->viewStartMs = 0;
            d->viewEndMs = viewDuration;
        }
        if (d->viewEndMs > d->totalDuration) {
            d->viewEndMs = d->totalDuration;
            d->viewStartMs = d->totalDuration - viewDuration;
        }

        UpdateLayout();
        update();
    }
    // 拖动主视图
    else if (d->dragging) {
        int deltaX = event->pos().x() - d->dragStartX;
        int viewDuration = d->viewEndMs - d->viewStartMs;

        // 根据拖动距离计算时间偏移
        qreal deltaRatio = qreal(-deltaX) / d->mainViewRect.width();
        int deltaMs = deltaRatio * viewDuration;

        d->viewStartMs = d->dragStartViewMs + deltaMs;
        d->viewEndMs = d->viewStartMs + viewDuration;

        // 边界限制
        if (d->viewStartMs < 0) {
            d->viewStartMs = 0;
            d->viewEndMs = viewDuration;
        }
        if (d->viewEndMs > d->totalDuration) {
            d->viewEndMs = d->totalDuration;
            d->viewStartMs = d->totalDuration - viewDuration;
        }

        UpdateLayout();
        update();
    }
    // 鼠标悬停效果
    else {
        // 检测是否悬停在缩略图指示器上
        if (d->visibleIndicator.contains(event->pos())) {
            setCursor(Qt::OpenHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }

        // 检测是否悬停在事件图标上
        if (d->mainViewRect.contains(event->pos())) {
            int oldHoveredIndex = d->hoveredEventIndex;
            d->hoveredEventIndex = EventAt(event->pos());

            // 如果悬停状态改变，触发重绘和动画
            if (oldHoveredIndex != d->hoveredEventIndex) {
                if (d->hoveredEventIndex >= 0 && !d->hoverAnimationTimer->isActive()) {
                    d->hoverAnimationTimer->start();
                }
                update();
            }
        } else {
            // 鼠标离开主视图区域
            if (d->hoveredEventIndex != -1) {
                d->hoveredEventIndex = -1;
                update();
            }
        }
    }
    UpdateCursor(event->pos());
    if (d->buffAreaRect.contains(event->pos()) && !d->dragging) {
        const int row = (event->pos().y() - d->buffAreaRect.top()) / d->buffRowHeight;
        QString tip = d->buffs[row].name;
        for (const auto &segment : d->buffs[row].segments) {
            if (d->cursorMs >= segment.startMs && d->cursorMs < segment.endMs) {
                tip += QStringLiteral("\n%1 层 | %2 至 %3 秒")
                    .arg(segment.stacks).arg(segment.startMs / 1000.0, 0, 'f', 4).arg(segment.endMs / 1000.0, 0, 'f', 4);
                break;
            }
        }
        QToolTip::showText(event->globalPosition().toPoint(), tip, this);
    } else {
        QToolTip::hideText();
    }
    update();
}

void Timeline::UpdateCursor(const QPoint &position)
{
    Q_D(Timeline);
    const int bottom = d->buffAreaRect.isValid() ? d->buffAreaRect.bottom() : d->mainViewRect.bottom();
    const QRect area(d->mainViewRect.left(), d->mainViewRect.top(), d->mainViewRect.width(), bottom - d->mainViewRect.top() + 1);
    if (!area.contains(position) || d->viewEndMs <= d->viewStartMs) {
        if (d->cursorMs >= 0) emit CursorLeft();
        d->cursorMs = -1;
        return;
    }
    d->cursorMs = d->viewStartMs + (position.x() - area.left()) * double(d->viewEndMs - d->viewStartMs) / qMax(1, area.width());
    emit CursorTimeChanged(d->cursorMs);
}

void Timeline::leaveEvent(QEvent *event)
{
    Q_D(Timeline);
    d->cursorPosition = {-1, -1};
    d->cursorMs = -1;
    d->hoveredEventIndex = -1;
    d->hoverAnimationTimer->stop();
    d->hoverScale = 1.0;
    QToolTip::hideText();
    emit CursorLeft();
    update();
    QWidget::leaveEvent(event);
}

void Timeline::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(Timeline);

    if (event->button() == Qt::LeftButton) {
        bool wasDragging = d->dragging || d->draggingIndicator;
        const int selected = d->pressedEventIndex;
        const bool clicked = d->dragging && !d->movedSincePress && selected >= 0 &&
            (event->pos() - d->pressPosition).manhattanLength() < QApplication::startDragDistance() &&
            EventAt(event->pos()) == selected;

        d->dragging = false;
        d->draggingIndicator = false;
        d->pressedEventIndex = -1;
        setCursor(Qt::ArrowCursor);

        if (clicked) {
            const auto selectedEvent = d->events[selected];
            emit EventClicked(selected, selectedEvent);
            return;
        }
        if (wasDragging) {
            emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
        }
    }
}
