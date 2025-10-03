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
    };

    // 设置时间轴数据
    void SetEvents(const QVector<EventItem> &events);

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

    QScopedPointer<TimelinePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Timeline)
};

#endif // TIMELINE_H
