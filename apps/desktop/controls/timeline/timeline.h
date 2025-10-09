#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include <QVector>
#include <QColor>
#include <QPixmap>

class TimelinePrivate;

/**
 * @brief 高性能时间轴控件
 *
 * 特性：
 * - 缩略图预览（上方小窗）
 * - 主视图（下方大窗，可滚动缩放）
 * - 技能事件显示（图标 + 时间戳）
 * - 优化渲染性能（只绘制可见区域）
 * - 平滑缩放和拖动
 */
class Timeline : public QWidget
{
    Q_OBJECT

public:
    explicit Timeline(QWidget *parent = nullptr);
    ~Timeline() override;

    // 事件项数据结构
    struct EventItem {
        int timestamp;      // 时间戳（毫秒）
        QString name;       // 技能名称
        QPixmap icon;       // 技能图标
        QColor color;       // 颜色标记
        int damage;         // 伤害值
        int rollResult;     // 会心结果 (1=普通, 2=会心, 3=识破)
        QString macroName;  // 所属宏名称
        QColor macroColor;  // 宏的颜色标记
    };

    // Buff覆盖区间数据结构
    struct BuffSegment {
        int startMs;        // 起始时间（毫秒）
        int endMs;          // 结束时间（毫秒）
    };

    // Buff项数据结构
    struct BuffItem {
        QString name;                    // Buff名称
        QPixmap icon;                    // Buff图标
        QColor color;                    // 覆盖条颜色
        QVector<BuffSegment> segments;   // 覆盖时间段列表
    };

    // 设置时间轴数据
    void SetEvents(const QVector<EventItem> &events);

    // 添加单个事件（用于增量添加）
    void AddEvent(const EventItem &event);

    // 设置Buff数据
    void SetBuffs(const QVector<BuffItem> &buffs);

    // 清空数据
    void Clear();

    // 设置时间范围（毫秒）
    void SetTimeRange(int startMs, int endMs);

    // 缩放控制
    void ZoomIn();
    void ZoomOut();
    void ResetZoom();

signals:
    // 事件被点击
    void EventClicked(int index, const EventItem &item);

    // 时间范围改变（用户缩放或滚动）
    void VisibleRangeChanged(int startMs, int endMs);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void SetupUI();
    void UpdateLayout();

    // Paint helpers (拆分自paintEvent)
    void DrawThumbnail(QPainter &painter);
    void DrawMainView(QPainter &painter);
    void DrawBuffArea(QPainter &painter);
    void DrawHoverEffects(QPainter &painter);
    void DrawHoverTooltip(QPainter &painter);

    QScopedPointer<TimelinePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Timeline)
};

#endif // TIMELINE_H
