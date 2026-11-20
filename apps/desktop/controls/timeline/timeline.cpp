#include "timeline.h"

#include <cmath>

#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QTimer>
#include <QtMath>
#include <QToolTip>
#include <QWheelEvent>

#include "../theme/workbench_theme.h"

// ============================================================================
// TimelinePrivate
// ============================================================================

class TimelinePrivate
{
public:
    explicit TimelinePrivate(Timeline *q) : q_ptr(q) { }

    // 数据
    QVector<Timeline::EventItem> events;
    QVector<Timeline::BuffItem>  buffs;

    // 布局参数
    int thumbnailHeight{ 60 }; // 缩略图高度
    int mainViewHeight{ 100 }; // 主视图高度
    int buffAreaHeight{ 0 };   // Buff区域总高度（根据buff数量动态计算）
    int buffRowHeight{ 30 };   // 每个buff行的高度
    int spacing{ 10 };         // 间距

    // 时间范围
    int   totalDuration{ 0 }; // 总时长（毫秒）
    int   viewStartMs{ 0 };   // 当前可见范围起点
    int   viewEndMs{ 10000 }; // 当前可见范围终点
    qreal zoomLevel{ 1.0 };   // 缩放级别 (1.0 = 100%)

    // 交互状态
    bool    dragging{ false };
    bool    draggingIndicator{ false }; // 是否正在拖动缩略图指示器
    int     dragStartX{ 0 };
    int     dragStartViewMs{ 0 };
    QPoint  pressPosition;
    int     pressedEventIndex = -1;
    bool    movedSincePress   = false;
    QPoint  cursorPosition{ -1, -1 };
    double  cursorMs = -1;
    int     hoveredEventIndex{ -1 };        // 当前悬停的事件索引 (-1表示无)
    qreal   hoverScale{ 1.0 };              // 悬停图标的缩放比例 (1.0 - 1.15)
    QTimer *hoverAnimationTimer{ nullptr }; // 悬停动画定时器

    // 区域矩形
    QRect thumbnailRect;
    QRect mainViewRect;
    QRect buffAreaRect;     // Buff区域矩形
    QRect visibleIndicator; // 缩略图中的可见区域指示器

private:
    Timeline *q_ptr;
    Q_DECLARE_PUBLIC(Timeline)
};

// ============================================================================
// Timeline Implementation
// ============================================================================

Timeline::Timeline(QWidget *parent) : QWidget(parent), d_ptr(new TimelinePrivate(this))
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
            background-color: #1a1a1e;
        }
    )");

    // 初始化悬停动画定时器
    d->hoverAnimationTimer = new QTimer(this);
    d->hoverAnimationTimer->setInterval(16); // 约60fps
    connect(d->hoverAnimationTimer, &QTimer::timeout, this, [this]() {
        Q_D(Timeline);
        const qreal TARGET_SCALE = 1.15; // 目标缩放比例
        const qreal SPEED        = 0.15; // 动画速度

        if (d->hoveredEventIndex >= 0) {
            // 放大
            if (d->hoverScale < TARGET_SCALE) {
                d->hoverScale += SPEED;
                if (d->hoverScale >= TARGET_SCALE) {
                    d->hoverScale = TARGET_SCALE;
                    d->hoverAnimationTimer->stop(); // 动画完成，停止定时器
                }
                update();
            } else {
                d->hoverAnimationTimer->stop(); // 已经达到目标，停止定时器
            }
        } else {
            // 缩小
            if (d->hoverScale > 1.0) {
                d->hoverScale -= SPEED;
                if (d->hoverScale <= 1.0) {
                    d->hoverScale = 1.0;
                    d->hoverAnimationTimer->stop();
                }
                update();
            } else {
                d->hoverAnimationTimer->stop(); // 已经恢复，停止定时器
            }
        }
    });
}

void Timeline::SetEvents(const QVector<EventItem> &events)
{
    Q_D(Timeline);
    d->events            = events;
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
    d->viewEndMs   = d->totalDuration;

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
    int required_height = d->thumbnailHeight + d->spacing + d->mainViewHeight;
    if (!buffs.isEmpty()) {
        required_height += d->spacing + (buffs.size() * d->buffRowHeight);
    }
    setMinimumHeight(qMax(210, required_height + 2));

    UpdateLayout();
    update();
}

void Timeline::Clear()
{
    Q_D(Timeline);
    d->events.clear();
    d->cursorPosition = { -1, -1 };
    d->cursorMs       = -1;
    emit CursorLeft();
    d->buffs.clear();
    d->totalDuration     = 0;
    d->viewStartMs       = 0;
    d->viewEndMs         = 10000;
    d->hoveredEventIndex = -1;
    d->pressedEventIndex = -1;
    d->dragging          = false;
    d->draggingIndicator = false;
    d->hoverScale        = 1.0;
    d->hoverAnimationTimer->stop();
    d->visibleIndicator = QRect();
    setMinimumHeight(210);
    setCursor(Qt::ArrowCursor);
    UpdateLayout();
    update();
}

void Timeline::SetTimeRange(int start_ms, int end_ms)
{
    Q_D(Timeline);
    if (start_ms < 0 || end_ms <= start_ms) {
        return;
    }
    d->totalDuration = qMax(end_ms, d->totalDuration);
    d->viewStartMs   = start_ms;
    d->viewEndMs     = end_ms;
    UpdateLayout();
    update();
    emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
}

void Timeline::ZoomIn()
{
    Q_D(Timeline);
    if (d->totalDuration <= 0) {
        return;
    }
    d->zoomLevel           *= 1.2;
    const int DURATION      = d->viewEndMs - d->viewStartMs;
    int       new_duration  = qMax(1, static_cast<int>(DURATION / 1.2));
    const int CENTER        = (d->viewStartMs + d->viewEndMs) / 2;

    new_duration   = qMin(new_duration, d->totalDuration);
    d->viewStartMs = qBound(0, CENTER - (new_duration / 2), d->totalDuration - new_duration);
    d->viewEndMs   = d->viewStartMs + new_duration;

    UpdateLayout();
    update();
    emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
}

void Timeline::ZoomOut()
{
    Q_D(Timeline);
    if (d->totalDuration <= 0) {
        return;
    }
    d->zoomLevel           /= 1.2;
    const int DURATION      = d->viewEndMs - d->viewStartMs;
    const int NEW_DURATION  = qMin(d->totalDuration, qMax(DURATION + 1, static_cast<int>(DURATION * 1.2)));
    const int CENTER        = (d->viewStartMs + d->viewEndMs) / 2;

    d->viewStartMs = qBound(0, CENTER - (NEW_DURATION / 2), d->totalDuration - NEW_DURATION);
    d->viewEndMs   = d->viewStartMs + NEW_DURATION;

    UpdateLayout();
    update();
    emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
}

void Timeline::ResetZoom()
{
    Q_D(Timeline);
    d->zoomLevel   = 1.0;
    d->viewStartMs = 0;
    d->viewEndMs   = d->totalDuration;

    UpdateLayout();
    update();
    emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
}

void Timeline::UpdateLayout()
{
    Q_D(Timeline);

    const int W = width();

    // 计算buff区域高度
    d->buffAreaHeight = d->buffs.isEmpty() ? 0 : d->buffs.size() * d->buffRowHeight;

    // 缩略图区域（上方）
    d->thumbnailRect = QRect(0, 0, W, d->thumbnailHeight);

    // 主视图区域（中间）
    const int MAIN_VIEW_TOP = d->thumbnailHeight + d->spacing;
    const int LABEL_WIDTH   = d->buffs.isEmpty() ? 0 : qMin(40, W / 3);
    d->mainViewRect         = QRect(LABEL_WIDTH, MAIN_VIEW_TOP, W - LABEL_WIDTH, d->mainViewHeight);

    // Buff区域（下方）
    if (d->buffAreaHeight > 0) {
        const int BUFF_AREA_TOP = MAIN_VIEW_TOP + d->mainViewHeight + d->spacing;
        d->buffAreaRect         = QRect(0, BUFF_AREA_TOP, W, d->buffAreaHeight);
    } else {
        d->buffAreaRect = QRect();
    }

    // 计算可见区域指示器
    if (d->totalDuration > 0) {
        const qreal START_RATIO = qreal(d->viewStartMs) / d->totalDuration;
        const qreal END_RATIO   = qreal(d->viewEndMs) / d->totalDuration;

        const int INDICATOR_X     = START_RATIO * W;
        const int INDICATOR_WIDTH = (END_RATIO - START_RATIO) * W;

        d->visibleIndicator = QRect(INDICATOR_X, 0, INDICATOR_WIDTH, d->thumbnailHeight);
    }
    UpdateCursor(d->cursorPosition);
}

void Timeline::DrawThumbnail(QPainter &painter)
{
    Q_D(Timeline);

    painter.save();
    painter.setClipRect(d->thumbnailRect);

    // 背景
    painter.fillRect(d->thumbnailRect, QColor(desktop::theme::Surface));

    // 绘制宏覆盖带（底部）
    const int MACRO_BAR_HEIGHT = 8;
    const int MACRO_BAR_Y      = d->thumbnailRect.bottom() - MACRO_BAR_HEIGHT - 2;

    if (d->totalDuration > 0 && !d->events.isEmpty()) {
        painter.setPen(Qt::NoPen);

        // 收集宏覆盖区间
        QString current_macro;
        int     segment_start = 0;
        QColor  current_color;

        for (int i = 0; i < d->events.size(); ++i) {
            const auto &event = d->events[i];

            // 检测宏切换
            if (event.macroName != current_macro) {
                // 绘制上一个宏的区间
                if (!current_macro.isEmpty() && i > 0) {
                    const qreal START_RATIO = qreal(segment_start) / d->totalDuration;
                    const qreal END_RATIO   = qreal(event.timestamp) / d->totalDuration;
                    const int   X1          = START_RATIO * width();
                    const int   X2          = END_RATIO * width();

                    painter.setBrush(current_color);
                    painter.drawRect(X1, MACRO_BAR_Y, X2 - X1, MACRO_BAR_HEIGHT);
                }

                // 开始新宏区间
                current_macro = event.macroName;
                current_color = event.macroColor;
                segment_start = event.timestamp;
            }
        }

        // 绘制最后一个宏区间
        if (!current_macro.isEmpty()) {
            const qreal START_RATIO = qreal(segment_start) / d->totalDuration;
            const int   X1          = START_RATIO * width();
            const int   X2          = width();

            painter.setBrush(current_color);
            painter.drawRect(X1, MACRO_BAR_Y, X2 - X1, MACRO_BAR_HEIGHT);
        }
    }

    // 绘制所有事件（缩略图 - 使用小图标）
    if (d->totalDuration > 0) {
        painter.setPen(Qt::NoPen);
        const int MINI_ICON_SIZE = 16;                                                      // 缩略图中的小图标尺寸
        const int ICON_Y = d->thumbnailRect.top() + ((d->thumbnailRect.height() - 12) / 2); // 调整图标位置，为底部宏条留空间

        for (const auto &event : d->events) {
            const qreal RATIO = qreal(event.timestamp) / d->totalDuration;
            const int   X     = RATIO * width();

            if (!event.icon.isNull()) {
                // 绘制小图标
                const QPixmap SCALED_ICON =
                    event.icon.scaled(MINI_ICON_SIZE, MINI_ICON_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(X - (MINI_ICON_SIZE / 2), ICON_Y - (MINI_ICON_SIZE / 2), MINI_ICON_SIZE, MINI_ICON_SIZE, SCALED_ICON);
            } else {
                // 无图标时绘制小竖线
                painter.setPen(QPen(event.color, 2));
                painter.drawLine(X, d->thumbnailRect.top() + 10, X, MACRO_BAR_Y - 2);
                painter.setPen(Qt::NoPen);
            }
        }
    }

    // 绘制可见区域指示器
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(100, 100, 100, 80));
    painter.drawRect(d->visibleIndicator);

    painter.setPen(QPen(desktop::theme::AccentColor(this), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(d->visibleIndicator);

    // 边框
    painter.setPen(QColor(desktop::theme::Border));
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
    painter.fillRect(d->mainViewRect, QColor(desktop::theme::Surface));

    // 绘制时间刻度（顶部，以秒为单位）
    const int VIEW_DURATION = d->viewEndMs - d->viewStartMs;
    if (VIEW_DURATION > 0) {
        const double DESIRED   = qMax(1.0, VIEW_DURATION * 70.0 / qMax(1, d->mainViewRect.width()));
        const double MAGNITUDE = std::pow(10.0, std::floor(std::log10(DESIRED)));
        const int    TICK_INTERVAL =
            static_cast<int>(MAGNITUDE * (DESIRED <= MAGNITUDE       ? 1
                                          : DESIRED <= 2 * MAGNITUDE ? 2
                                          : DESIRED <= 5 * MAGNITUDE ? 5
                                                                     : 10));

        painter.setPen(QColor(desktop::theme::Muted));
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);

        for (int ms = (d->viewStartMs / TICK_INTERVAL) * TICK_INTERVAL; ms <= d->viewEndMs; ms += TICK_INTERVAL) {
            const qreal RATIO = qreal(ms - d->viewStartMs) / VIEW_DURATION;
            const int   X     = d->mainViewRect.left() + (RATIO * d->mainViewRect.width());

            // 刻度线（从顶部向下）
            painter.drawLine(X, d->mainViewRect.top(), X, d->mainViewRect.top() + 20);

            // 时间标签（顶部）
            const QString TIME_LABEL = QString::number(ms / 1000.0, 'f', TICK_INTERVAL < 1000 ? 3 : 0) + QStringLiteral("秒");
            const int LABEL_X =
                qBound(d->mainViewRect.left(), X - 30, qMax(d->mainViewRect.left(), d->mainViewRect.right() - 59));
            painter.drawText(LABEL_X, d->mainViewRect.top() + 5, 60, 15, Qt::AlignCenter, TIME_LABEL);
        }
    }

    // 绘制所有事件图标（正常状态，跳过hover的）
    painter.setPen(Qt::NoPen);
    const int ICON_SIZE = 40;
    const int ICON_Y    = d->mainViewRect.top() + 30;

    if (VIEW_DURATION <= 0) {
        painter.restore();
        return;
    }
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

        const qreal RATIO = qreal(event.timestamp - d->viewStartMs) / VIEW_DURATION;
        const int   X     = d->mainViewRect.left() + (RATIO * d->mainViewRect.width());

        // 绘制事件图标
        if (!event.icon.isNull()) {
            // 裁剪掉图标最外3圈像素，并绘制
            const QRect source_rect(3, 3, event.icon.width() - 6, event.icon.height() - 6);
            const QRect target_rect(X - (ICON_SIZE / 2) + 3, ICON_Y + 3, ICON_SIZE - 6, ICON_SIZE - 6);
            painter.drawPixmap(target_rect, event.icon, source_rect);

            // 绘制灰黑色边框（1像素宽，在裁剪后图标的内侧）
            painter.setPen(QPen(QColor(desktop::theme::Border), 1));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(target_rect.adjusted(0.5, 0.5, -0.5, -0.5), 3, 3);
            painter.setPen(Qt::NoPen);
        } else {
            // 无图标时绘制简单标记
            painter.setBrush(event.color);
            painter.drawRect(X - 2, ICON_Y, 4, 40);
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
        painter.fillRect(d->buffAreaRect, QColor(desktop::theme::Surface));

        const int VIEW_DURATION = d->viewEndMs - d->viewStartMs;

        // 绘制每个buff行
        for (int i = 0; i < d->buffs.size(); ++i) {
            const auto &buff  = d->buffs[i];
            const int   ROW_Y = d->buffAreaRect.top() + (i * d->buffRowHeight);

            // 绘制buff图标和名称
            if (!buff.icon.isNull()) {
                const int     BUFF_ICON_SIZE = d->buffRowHeight - 6;
                const QPixmap SCALED_ICON =
                    buff.icon.scaled(BUFF_ICON_SIZE, BUFF_ICON_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                painter.drawPixmap(5, ROW_Y + 3, BUFF_ICON_SIZE, BUFF_ICON_SIZE, SCALED_ICON);
            }

            // 绘制覆盖条区域（从buff名称右侧开始）
            const int BAR_START_X = d->mainViewRect.left();
            const int BAR_WIDTH   = d->mainViewRect.width();
            const int BAR_Y       = ROW_Y + 5;
            const int bar_height  = d->buffRowHeight - 10;

            // 绘制背景条
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(desktop::theme::Border));
            painter.drawRect(BAR_START_X, BAR_Y, BAR_WIDTH, bar_height);

            // 绘制buff覆盖时间段
            if (VIEW_DURATION > 0) {
                painter.setBrush(buff.color);

                for (const auto &segment : buff.segments) {
                    // 只绘制可见范围内的部分
                    const double SEG_START = qMax(segment.startMs, double(d->viewStartMs));
                    const double SEG_END   = qMin(segment.endMs, double(d->viewEndMs));

                    if (SEG_START < SEG_END) {
                        const qreal START_RATIO = qreal(SEG_START - d->viewStartMs) / VIEW_DURATION;
                        const qreal END_RATIO   = qreal(SEG_END - d->viewStartMs) / VIEW_DURATION;

                        const int X1 = BAR_START_X + (START_RATIO * BAR_WIDTH);
                        const int X2 = BAR_START_X + (END_RATIO * BAR_WIDTH);
                        const int W  = X2 - X1;

                        // 绘制覆盖条
                        painter.setPen(Qt::NoPen);
                        painter.setBrush(buff.color);
                        painter.drawRect(X1, BAR_Y, W, bar_height);
                        if (W >= 20) {
                            painter.setPen(QColor(10, 14, 20));
                            painter.drawText(QRect(X1, BAR_Y, W, bar_height), Qt::AlignCenter, QString::number(segment.stacks));
                        }
                    }
                }
            }

            // 绘制行分割线
            if (i < d->buffs.size() - 1) {
                painter.setPen(QColor("#2c2c32"));
                painter.drawLine(d->buffAreaRect.left(), ROW_Y + d->buffRowHeight, d->buffAreaRect.right(), ROW_Y + d->buffRowHeight);
            }
        }

        // 边框
        painter.setPen(QColor(desktop::theme::Border));
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
        const int X      = d->cursorPosition.x();
        const int BOTTOM = d->buffAreaRect.isValid() ? d->buffAreaRect.bottom() : d->mainViewRect.bottom();
        painter.setPen(QPen(QColor(desktop::theme::Success), 1));
        painter.drawLine(X, d->mainViewRect.top(), X, BOTTOM);
        const QString TIME =
            QStringLiteral("%1 秒 · 第 %2 帧").arg(d->cursorMs / 1000.0, 0, 'f', 4).arg(static_cast<int>(d->cursorMs * 16 / 1000));
        const int   WIDTH = painter.fontMetrics().horizontalAdvance(TIME) + 16;
        const int   left  = qBound(0, X - (WIDTH / 2), qMax(0, this->width() - WIDTH));
        const QRect LABEL(left, d->mainViewRect.top() - 27, WIDTH, 25);
        painter.fillRect(LABEL, QColor(desktop::theme::Raised));
        painter.drawText(LABEL, Qt::AlignCenter, TIME);
    }
    const int DURATION = d->viewEndMs - d->viewStartMs;
    if (d->hoveredEventIndex >= 0 && d->hoveredEventIndex < d->events.size() && DURATION > 0) {
        const auto &event = d->events[d->hoveredEventIndex];
        if (event.timestamp >= d->viewStartMs && event.timestamp <= d->viewEndMs) {
            const int X =
                d->mainViewRect.left() + (double(event.timestamp - d->viewStartMs) / DURATION * d->mainViewRect.width());
            const int   SIZE = 40 * d->hoverScale;
            const QRect ICON_RECT(X - (SIZE / 2), d->mainViewRect.top() + 30 - ((SIZE - 40) / 2), SIZE, SIZE);
            painter.setClipRect(d->mainViewRect);
            painter.drawPixmap(ICON_RECT, event.icon);
            painter.setPen(QPen(QColor(desktop::theme::Success), 2));
            painter.drawRect(ICON_RECT);
        }
    }
    painter.restore();
}

void Timeline::DrawHoverTooltip(QPainter &painter)
{
    Q_D(Timeline);
    const double FRAME_MS = 62.5; // JX3逻辑帧：16帧/秒 = 62.5毫秒/帧

    if (d->hoveredEventIndex >= 0 && d->hoveredEventIndex < d->events.size()) {
        const auto &hovered_event = d->events[d->hoveredEventIndex];
        const int   VIEW_DURATION = d->viewEndMs - d->viewStartMs;

        if (hovered_event.timestamp >= d->viewStartMs && hovered_event.timestamp <= d->viewEndMs && VIEW_DURATION > 0) {
            const qreal RATIO     = qreal(hovered_event.timestamp - d->viewStartMs) / VIEW_DURATION;
            const int   HOVER_X   = d->mainViewRect.left() + (RATIO * d->mainViewRect.width());
            const int   ICON_SIZE = 40;
            const int   ICON_Y    = d->mainViewRect.top() + 30;

            // 收集80%以上重叠的事件
            QVector<int> overlapped_indices;
            overlapped_indices.append(d->hoveredEventIndex);

            for (int i = 0; i < d->events.size(); ++i) {
                if (i == d->hoveredEventIndex) {
                    continue;
                }
                const auto &other_event = d->events[i];

                if (other_event.timestamp < d->viewStartMs || other_event.timestamp > d->viewEndMs) {
                    continue;
                }

                const qreal OTHER_RATIO = qreal(other_event.timestamp - d->viewStartMs) / VIEW_DURATION;
                const int   OTHER_X     = d->mainViewRect.left() + (OTHER_RATIO * d->mainViewRect.width());

                // 计算重叠度
                const int   DISTANCE      = qAbs(HOVER_X - OTHER_X);
                const qreal OVERLAP_RATIO = 1.0 - (qreal(DISTANCE) / ICON_SIZE);

                if (OVERLAP_RATIO >= 0.8) {
                    overlapped_indices.append(i);
                }
            }

            // 构建浮窗内容（不显示图标，只显示文字）
            QStringList tooltip_lines;
            for (const int IDX : overlapped_indices) {
                const auto   &evt = d->events[IDX];
                const QString LINE =
                    QString("%1 | %2 秒 (%3 帧) | 伤害:%4 | %5")
                        .arg(evt.name)
                        .arg(evt.timestamp / 1000.0, 0, 'f', 2)
                        .arg(qRound(evt.timestamp / FRAME_MS))
                        .arg(evt.damage)
                        .arg(evt.rollResult == 1   ? "普通"
                             : evt.rollResult == 2 ? "会心"
                             : evt.rollResult == 3 ? "识破"
                                                   : "未命中");
                tooltip_lines.append(LINE);
            }

            // 计算浮窗尺寸
            QFont popup_font = painter.font();
            popup_font.setPointSize(9);
            painter.setFont(popup_font);
            const QFontMetrics FM(popup_font);

            int max_width = 0;
            for (const QString &line : tooltip_lines) {
                max_width = qMax(max_width, FM.horizontalAdvance(line));
            }

            const int POPUP_WIDTH  = max_width + 20;
            const int popup_height = (FM.height() * tooltip_lines.size()) + 16;

            // 计算浮窗位置（在图标正下方，居中对齐）
            int       popup_x     = HOVER_X - (POPUP_WIDTH / 2);
            const int SCALED_SIZE = ICON_SIZE * d->hoverScale;
            int       popup_y     = ICON_Y + SCALED_SIZE + 10; // 在放大图标下方

            // 边界检查
            if (popup_x < d->mainViewRect.left()) {
                popup_x = d->mainViewRect.left() + 5;
            }
            if (popup_x + POPUP_WIDTH > d->mainViewRect.right()) {
                popup_x = d->mainViewRect.right() - POPUP_WIDTH - 5;
            }
            if (popup_y + popup_height > height()) {
                popup_y = ICON_Y - popup_height - 10; // 移到图标上方
            }

            const QRect popup_rect(popup_x, popup_y, POPUP_WIDTH, popup_height);

            painter.save();

            // 绘制渐变阴影（多层，增加层次感，更浅）
            painter.setPen(Qt::NoPen);
            for (int i = 5; i > 0; --i) {
                const int ALPHA = 10 + ((5 - i) * 8); // 更浅的渐变透明度
                painter.setBrush(QColor(0, 0, 0, ALPHA));
                painter.drawRect(popup_rect.adjusted(-i, -i, i, i));
            }

            // 绘制浮窗背景
            painter.setBrush(QColor(35, 35, 39, 250));
            painter.drawRect(popup_rect);

            // 绘制边框（深灰色，0.5像素宽）
            painter.setPen(QPen(QColor(desktop::theme::Border), 0.5));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(popup_rect);

            // 绘制文字（高亮悬停的那一行，居中对齐）
            int text_y = popup_y + 8 + FM.ascent();
            for (int i = 0; i < tooltip_lines.size(); ++i) {
                // 悬停的事件用金色高亮
                if (overlapped_indices[i] == d->hoveredEventIndex) {
                    painter.setPen(QColor(desktop::theme::Text));
                } else {
                    painter.setPen(QColor(desktop::theme::Muted));
                }
                painter.drawText(popup_x + 10, text_y, tooltip_lines[i]);
                text_y += FM.height();
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
    const int DURATION = d->viewEndMs - d->viewStartMs;
    if (DURATION <= 0 || !d->mainViewRect.contains(position)) {
        return -1;
    }
    // Match icon drawing order, including several hits at the same timestamp.
    for (int index = static_cast<int>(d->events.size()) - 1; index >= 0; --index) {
        const auto TIMESTAMP = d->events[index].timestamp;
        if (TIMESTAMP < d->viewStartMs || TIMESTAMP > d->viewEndMs) {
            continue;
        }
        const int X = d->mainViewRect.left() + (qreal(TIMESTAMP - d->viewStartMs) / DURATION * d->mainViewRect.width());
        if (QRect(X - 20, d->mainViewRect.top() + 30, 40, 40).contains(position)) {
            return index;
        }
    }
    return -1;
}

void Timeline::mousePressEvent(QMouseEvent *event)
{
    Q_D(Timeline);

    if (event->button() == Qt::LeftButton) {
        d->pressPosition     = event->pos();
        d->pressedEventIndex = EventAt(event->pos());
        d->movedSincePress   = false;
        // 点击缩略图
        if (d->thumbnailRect.contains(event->pos())) {
            // 判断是否点击在可见指示器上
            if (d->visibleIndicator.contains(event->pos())) {
                // 拖动指示器
                d->draggingIndicator = true;
                d->dragStartX        = event->pos().x();
                d->dragStartViewMs   = d->viewStartMs;
                setCursor(Qt::ClosedHandCursor);
            } else {
                // 点击缩略图空白区域 - 跳转到对应位置
                const qreal RATIO         = qreal(event->pos().x()) / width();
                const int   CENTER_MS     = RATIO * d->totalDuration;
                const int   HALF_DURATION = (d->viewEndMs - d->viewStartMs) / 2;

                d->viewStartMs = qMax(0, CENTER_MS - HALF_DURATION);
                d->viewEndMs   = qMin(d->totalDuration, CENTER_MS + HALF_DURATION);

                UpdateLayout();
                update();
                emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
            }
        }
        // 点击主视图 - 开始拖动或选择事件
        else if (d->mainViewRect.contains(event->pos()))
        {
            d->dragging        = true;
            d->dragStartX      = event->pos().x();
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
    {
        d->movedSincePress = true;
    }

    // 拖动缩略图指示器
    if (d->draggingIndicator) {
        const int DELTA_X       = event->pos().x() - d->dragStartX;
        const int VIEW_DURATION = d->viewEndMs - d->viewStartMs;

        // 根据拖动距离计算时间偏移（缩略图映射到整个时间轴）
        const qreal DELTA_RATIO = qreal(DELTA_X) / width();
        const int   DELTA_MS    = DELTA_RATIO * d->totalDuration;

        d->viewStartMs = d->dragStartViewMs + DELTA_MS;
        d->viewEndMs   = d->viewStartMs + VIEW_DURATION;

        // 边界限制
        if (d->viewStartMs < 0) {
            d->viewStartMs = 0;
            d->viewEndMs   = VIEW_DURATION;
        }
        if (d->viewEndMs > d->totalDuration) {
            d->viewEndMs   = d->totalDuration;
            d->viewStartMs = d->totalDuration - VIEW_DURATION;
        }

        UpdateLayout();
        update();
    }
    // 拖动主视图
    else if (d->dragging)
    {
        const int DELTA_X       = event->pos().x() - d->dragStartX;
        const int VIEW_DURATION = d->viewEndMs - d->viewStartMs;

        // 根据拖动距离计算时间偏移
        const qreal DELTA_RATIO = qreal(-DELTA_X) / d->mainViewRect.width();
        const int   DELTA_MS    = DELTA_RATIO * VIEW_DURATION;

        d->viewStartMs = d->dragStartViewMs + DELTA_MS;
        d->viewEndMs   = d->viewStartMs + VIEW_DURATION;

        // 边界限制
        if (d->viewStartMs < 0) {
            d->viewStartMs = 0;
            d->viewEndMs   = VIEW_DURATION;
        }
        if (d->viewEndMs > d->totalDuration) {
            d->viewEndMs   = d->totalDuration;
            d->viewStartMs = d->totalDuration - VIEW_DURATION;
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
            const int OLD_HOVERED_INDEX = d->hoveredEventIndex;
            d->hoveredEventIndex        = EventAt(event->pos());

            // 如果悬停状态改变，触发重绘和动画
            if (OLD_HOVERED_INDEX != d->hoveredEventIndex) {
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
        const int ROW = (event->pos().y() - d->buffAreaRect.top()) / d->buffRowHeight;
        QString   tip = d->buffs[ROW].name;
        for (const auto &segment : d->buffs[ROW].segments) {
            if (d->cursorMs >= segment.startMs && d->cursorMs < segment.endMs) {
                tip += QStringLiteral("\n%1 层 | %2 至 %3 秒")
                           .arg(segment.stacks)
                           .arg(segment.startMs / 1000.0, 0, 'f', 4)
                           .arg(segment.endMs / 1000.0, 0, 'f', 4);
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
    const int BOTTOM = d->buffAreaRect.isValid() ? d->buffAreaRect.bottom() : d->mainViewRect.bottom();
    const QRect AREA(d->mainViewRect.left(), d->mainViewRect.top(), d->mainViewRect.width(), BOTTOM - d->mainViewRect.top() + 1);
    if (!AREA.contains(position) || d->viewEndMs <= d->viewStartMs) {
        if (d->cursorMs >= 0) {
            emit CursorLeft();
        }
        d->cursorMs = -1;
        return;
    }
    d->cursorMs =
        d->viewStartMs + ((position.x() - AREA.left()) * double(d->viewEndMs - d->viewStartMs) / qMax(1, AREA.width()));
    emit CursorTimeChanged(d->cursorMs);
}

void Timeline::leaveEvent(QEvent *event)
{
    Q_D(Timeline);
    d->cursorPosition    = { -1, -1 };
    d->cursorMs          = -1;
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
        const bool WAS_DRAGGING = d->dragging || d->draggingIndicator;
        const int  SELECTED     = d->pressedEventIndex;
        const bool CLICKED = d->dragging && !d->movedSincePress && SELECTED >= 0 &&
                             (event->pos() - d->pressPosition).manhattanLength() < QApplication::startDragDistance() &&
                             EventAt(event->pos()) == SELECTED;

        d->dragging          = false;
        d->draggingIndicator = false;
        d->pressedEventIndex = -1;
        setCursor(Qt::ArrowCursor);

        if (CLICKED) {
            const auto SELECTED_EVENT = d->events[SELECTED];
            emit       EventClicked(SELECTED, SELECTED_EVENT);
            return;
        }
        if (WAS_DRAGGING) {
            emit VisibleRangeChanged(d->viewStartMs, d->viewEndMs);
        }
    }
}
