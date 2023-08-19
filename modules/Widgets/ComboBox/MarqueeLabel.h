/**
 * Project: JX3DPS
 * File: MarqueeLabel.h
 * Description: 
 * Created Date: 2023-07-16 14:10:09
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-16 15:08:37
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef MARQUEELABEL_H
#define MARQUEELABEL_H

#include <QBasicTimer>
#include <QLabel>

class MarqueeLabel : public QLabel
{
    Q_OBJECT

public:
    enum Direction
    {
        RIGHT_TO_LEFT = 1,
        LEFT_TO_RIGHT = 2
    };

    explicit MarqueeLabel(QWidget *parent = nullptr);
    explicit MarqueeLabel(const QString &text, QWidget *parent = nullptr);

    // 滚动方向
    Direction GetDirection() const;
    void      SetDirection(Direction direction);

    // 刷新间隔
    int  GetInterval() const;
    void SetInterval(int interval);

public slots:
    // 开始滚动
    void Start();

    // 重置
    void Reset();

protected:
    // basictimer定时器触发
    virtual void timerEvent(QTimerEvent *event) override;

    // 绘制
    virtual void paintEvent(QPaintEvent *event) override;

    // 大小变化时重新计算
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    // 滚动前后停滞时间
    int m_startTimes = 0;
    int m_endTimes   = 0;

    // 刷新间隔
    int m_interval = 20;

    // 偏移量
    int m_offset = 0;

    // 文本宽度
    int m_textWidth = 0;

    // 控件宽度
    int m_labelWidth = 0;

    // 默认右往左
    Direction m_direction = RIGHT_TO_LEFT;

    // 滚动定时器 也可以使用QTimer QTimeLine QAnimation等实现
    QBasicTimer m_timer;
};

#endif // MARQUEELABEL_H
