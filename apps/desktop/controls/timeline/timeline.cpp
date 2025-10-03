#include "timeline.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainterPath>
#include <QScrollBar>
#include <QDebug>
#include <QtMath>

// ============================================================================
// TimelinePrivate
// ============================================================================

class TimelinePrivate
{
public:
    explicit TimelinePrivate(Timeline *q) :
        q_ptr(q),
        thumbnailHeight(60),
        mainViewHeight(120),
        spacing(10),
        totalDuration(0),
        viewStartMs(0),
        viewEndMs(10000),
        zoomLevel(1.0),
        dragging(false),
        draggingIndicator(false),
        dragStartX(0),
        dragStartViewMs(0),
        hoveredEventIndex(-1)
    {
    }

    // 数据
    QVector<Timeline::EventItem> events;

    // 布局参数
    int thumbnailHeight;   // 缩略图高度
    int mainViewHeight;    // 主视图高度
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
    int hoveredEventIndex;   // 当前悬停的事件索引 (-1表示无)

    // 区域矩形
    QRect thumbnailRect;
    QRect mainViewRect;
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
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMouseTracking(true);

    // 设置样式
    setStyleSheet(R"(
        Timeline {
            background-color: #1e1e1e;
        }
    )");
}

void Timeline::SetEvents(const QVector<EventItem> &events)
{
    Q_D(Timeline);
    d->events = events;

    // 计算总时长
    d->totalDuration = 0;
    for (const auto &event : events) {
        if (event.timestamp > d->totalDuration) {
            d->totalDuration = event.timestamp;
        }
    }

    // 初始化视图范围
    d->viewStartMs = 0;
    d->viewEndMs = d->totalDuration;

    UpdateLayout();
    update();
}

void Timeline::Clear()
{
    Q_D(Timeline);
    d->events.clear();
    d->totalDuration = 0;
    d->viewStartMs = 0;
    d->viewEndMs = 10000;
    update();
}

void Timeline::SetTimeRange(int startMs, int endMs)
{
    Q_D(Timeline);
    d->totalDuration = qMax(endMs, d->totalDuration);
    update();
}

void Timeline::ZoomIn()
{
    Q_D(Timeline);
    d->zoomLevel *= 1.2;
    int duration = d->viewEndMs - d->viewStartMs;
    int newDuration = duration / 1.2;
    int center = (d->viewStartMs + d->viewEndMs) / 2;

    d->viewStartMs = center - newDuration / 2;
    d->viewEndMs = center + newDuration / 2;

    UpdateLayout();
    update();
    emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
}

void Timeline::ZoomOut()
{
    Q_D(Timeline);
    d->zoomLevel /= 1.2;
    int duration = d->viewEndMs - d->viewStartMs;
    int newDuration = duration * 1.2;
    int center = (d->viewStartMs + d->viewEndMs) / 2;

    d->viewStartMs = qMax(0, center - newDuration / 2);
    d->viewEndMs = qMin(d->totalDuration, center + newDuration / 2);

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

    // 缩略图区域（上方）
    d->thumbnailRect = QRect(0, 0, w, d->thumbnailHeight);

    // 主视图区域（下方）
    d->mainViewRect = QRect(0, d->thumbnailHeight + d->spacing, w, h - d->thumbnailHeight - d->spacing);

    // 计算可见区域指示器
    if (d->totalDuration > 0) {
        qreal startRatio = qreal(d->viewStartMs) / d->totalDuration;
        qreal endRatio = qreal(d->viewEndMs) / d->totalDuration;

        int indicatorX = startRatio * w;
        int indicatorWidth = (endRatio - startRatio) * w;

        d->visibleIndicator = QRect(indicatorX, 0, indicatorWidth, d->thumbnailHeight);
    }
}

void Timeline::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    Q_D(Timeline);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制缩略图
    {
        painter.save();
        painter.setClipRect(d->thumbnailRect);

        // 背景
        painter.fillRect(d->thumbnailRect, QColor(30, 30, 30));

        // 绘制所有事件（缩略图 - 使用小图标）
        if (d->totalDuration > 0) {
            painter.setPen(Qt::NoPen);
            const int miniIconSize = 16;  // 缩略图中的小图标尺寸
            const int iconY = d->thumbnailRect.center().y();

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
                    painter.drawLine(x, d->thumbnailRect.top() + 10, x, d->thumbnailRect.bottom() - 10);
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

    // 绘制主视图
    {
        painter.save();
        painter.setClipRect(d->mainViewRect);

        // 背景
        painter.fillRect(d->mainViewRect, QColor(40, 40, 40));

        // 绘制时间刻度
        int viewDuration = d->viewEndMs - d->viewStartMs;
        if (viewDuration > 0) {
            // 计算合适的刻度间隔
            int tickInterval = 1000; // 1秒
            if (viewDuration > 60000) tickInterval = 5000;  // 5秒
            if (viewDuration > 300000) tickInterval = 10000; // 10秒

            painter.setPen(QColor(80, 80, 80));
            QFont font = painter.font();
            font.setPointSize(8);
            painter.setFont(font);

            for (int ms = (d->viewStartMs / tickInterval) * tickInterval; ms <= d->viewEndMs; ms += tickInterval) {
                qreal ratio = qreal(ms - d->viewStartMs) / viewDuration;
                int x = d->mainViewRect.left() + ratio * d->mainViewRect.width();

                // 刻度线
                painter.drawLine(x, d->mainViewRect.bottom() - 20, x, d->mainViewRect.bottom());

                // 时间标签
                QString timeLabel = QString::number(ms / 1000.0, 'f', 1) + "s";
                painter.drawText(x - 20, d->mainViewRect.bottom() - 5, 40, 15, Qt::AlignCenter, timeLabel);
            }
        }

        // 绘制可见范围内的事件
        painter.setPen(Qt::NoPen);
        for (int i = 0; i < d->events.size(); ++i) {
            const auto &event = d->events[i];

            // 只绘制可见范围内的事件（性能优化）
            if (event.timestamp < d->viewStartMs || event.timestamp > d->viewEndMs) {
                continue;
            }

            qreal ratio = qreal(event.timestamp - d->viewStartMs) / viewDuration;
            int x = d->mainViewRect.left() + ratio * d->mainViewRect.width();

            // 绘制事件图标
            if (!event.icon.isNull()) {
                int iconSize = 40;
                int y = d->mainViewRect.top() + 10;

                // 悬停高亮效果
                bool isHovered = (i == d->hoveredEventIndex);

                // 绘制图标（悬停时稍微放大）
                if (isHovered) {
                    int hoveredSize = iconSize + 6;
                    QPixmap scaledIcon = event.icon.scaled(hoveredSize, hoveredSize,
                                                           Qt::KeepAspectRatio,
                                                           Qt::SmoothTransformation);

                    // 悬停外圈高亮
                    painter.setPen(QPen(QColor(255, 255, 255, 120), 2));
                    painter.setBrush(Qt::NoBrush);
                    painter.drawEllipse(QPoint(x, y + iconSize / 2), hoveredSize / 2 + 2, hoveredSize / 2 + 2);

                    painter.setPen(Qt::NoPen);
                    painter.drawPixmap(x - hoveredSize / 2, y - 3, hoveredSize, hoveredSize, scaledIcon);
                } else {
                    painter.drawPixmap(x - iconSize / 2, y, iconSize, iconSize, event.icon);
                }
            } else {
                // 无图标时绘制简单标记
                painter.setBrush(event.color);
                painter.drawRect(x - 2, d->mainViewRect.top() + 10, 4, 40);
            }
        }

        // 绘制悬停事件的信息浮窗
        if (d->hoveredEventIndex >= 0 && d->hoveredEventIndex < d->events.size()) {
            const auto &event = d->events[d->hoveredEventIndex];

            if (event.timestamp >= d->viewStartMs && event.timestamp <= d->viewEndMs) {
                qreal ratio = qreal(event.timestamp - d->viewStartMs) / viewDuration;
                int iconX = d->mainViewRect.left() + ratio * d->mainViewRect.width();
                int iconY = d->mainViewRect.top() + 10;

                // 浮窗内容
                QString tooltipText = QString("%1\n时间: %2s\n伤害: %3\n结果: %4")
                    .arg(event.name)
                    .arg(event.timestamp / 1000.0, 0, 'f', 2)
                    .arg(event.damage)
                    .arg(event.rollResult == 1 ? "普通" : event.rollResult == 2 ? "会心" : "识破");

                QFont font = painter.font();
                font.setPointSize(9);
                painter.setFont(font);

                QFontMetrics fm(font);
                QStringList lines = tooltipText.split('\n');
                int maxWidth = 0;
                for (const QString &line : lines) {
                    maxWidth = qMax(maxWidth, fm.horizontalAdvance(line));
                }
                int tooltipWidth = maxWidth + 20;
                int tooltipHeight = fm.height() * lines.size() + 16;

                // 计算浮窗位置（避免超出边界）
                int tooltipX = iconX + 30;
                int tooltipY = iconY - tooltipHeight / 2;

                if (tooltipX + tooltipWidth > d->mainViewRect.right()) {
                    tooltipX = iconX - tooltipWidth - 10;
                }
                if (tooltipY < d->mainViewRect.top()) {
                    tooltipY = d->mainViewRect.top() + 5;
                }
                if (tooltipY + tooltipHeight > d->mainViewRect.bottom()) {
                    tooltipY = d->mainViewRect.bottom() - tooltipHeight - 5;
                }

                QRect tooltipRect(tooltipX, tooltipY, tooltipWidth, tooltipHeight);

                // 绘制浮窗背景（带阴影）
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(0, 0, 0, 180));
                painter.drawRoundedRect(tooltipRect.adjusted(2, 2, 2, 2), 6, 6);

                painter.setBrush(QColor(45, 45, 48));
                painter.drawRoundedRect(tooltipRect, 6, 6);

                // 绘制边框
                painter.setPen(QColor(80, 80, 80));
                painter.setBrush(Qt::NoBrush);
                painter.drawRoundedRect(tooltipRect, 6, 6);

                // 绘制文字
                painter.setPen(QColor(220, 220, 220));
                int textY = tooltipY + 12;
                for (const QString &line : lines) {
                    painter.drawText(tooltipX + 10, textY, line);
                    textY += fm.height();
                }
            }
        }

        // 边框
        painter.setPen(QColor(60, 60, 60));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(d->mainViewRect);

        painter.restore();
    }
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

void Timeline::mousePressEvent(QMouseEvent *event)
{
    Q_D(Timeline);

    if (event->button() == Qt::LeftButton) {
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
            int viewDuration = d->viewEndMs - d->viewStartMs;
            int oldHoveredIndex = d->hoveredEventIndex;
            d->hoveredEventIndex = -1;

            if (viewDuration > 0) {
                const int iconSize = 40;
                const int iconY = d->mainViewRect.top() + 10;

                for (int i = 0; i < d->events.size(); ++i) {
                    const auto &evt = d->events[i];

                    // 跳过不可见事件
                    if (evt.timestamp < d->viewStartMs || evt.timestamp > d->viewEndMs) {
                        continue;
                    }

                    qreal ratio = qreal(evt.timestamp - d->viewStartMs) / viewDuration;
                    int iconX = d->mainViewRect.left() + ratio * d->mainViewRect.width();

                    // 检测鼠标是否在图标圆形区域内
                    QPoint iconCenter(iconX, iconY + iconSize / 2);
                    int dx = event->pos().x() - iconCenter.x();
                    int dy = event->pos().y() - iconCenter.y();
                    int distSq = dx * dx + dy * dy;
                    int radiusSq = (iconSize / 2) * (iconSize / 2);

                    if (distSq <= radiusSq) {
                        d->hoveredEventIndex = i;
                        break;
                    }
                }
            }

            // 如果悬停状态改变，触发重绘
            if (oldHoveredIndex != d->hoveredEventIndex) {
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
}

void Timeline::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(Timeline);

    if (event->button() == Qt::LeftButton) {
        bool wasDragging = d->dragging || d->draggingIndicator;

        d->dragging = false;
        d->draggingIndicator = false;
        setCursor(Qt::ArrowCursor);

        if (wasDragging) {
            emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
        }
    }
}
