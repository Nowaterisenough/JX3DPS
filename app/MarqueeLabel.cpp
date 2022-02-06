/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 15:05:31
 * @FilePath    : \JX3DPS\app\MarqueeLabel.cpp
 */
#include "MarqueeLabel.h"

#include <QTimerEvent>
#include <QPainter>
#include <QResizeEvent>

MarqueeLabel::MarqueeLabel(QWidget *parent) : QLabel(parent)
{

}

MarqueeLabel::Direction MarqueeLabel::GetDirection() const
{
    return m_direction;
}

void MarqueeLabel::SetDirection(MarqueeLabel::Direction direction)
{
    if (this->m_direction != direction) {
        this->m_direction = direction;
        m_offset = 0;
    }
}

int MarqueeLabel::GetInterval() const
{
    return m_interval;
}

void MarqueeLabel::SetInterval(int interval)
{
    if (this->m_interval != interval) {
        this->m_interval = interval;
        m_timer.start(interval, this);
    }
}

void MarqueeLabel::Start()
{
    m_timer.start(m_interval, this);
}

void MarqueeLabel::Reset()
{
    m_timer.stop();
    m_offset = this->width();
}

void MarqueeLabel::timerEvent(QTimerEvent *event)
{
    if (m_startTimes >= 20) {
        m_startTimes = 0;
    } else if (m_offset == this->width()) {
        m_startTimes++;
        update();
        return;
    }
    if (event->timerId() == m_timer.timerId()) {
        m_offset++;
        if (m_offset > m_textWidth + 5) {
            if (m_endTimes < 30) {
                m_endTimes++;
                m_offset--;
            } else {
                m_endTimes = 0;
                m_offset = this->width();
            }
        }
        update();
    } else {
        QLabel::timerEvent(event);
    }
}

void MarqueeLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int textWidth = painter.fontMetrics().horizontalAdvance(text());
    int textHeight = painter.fontMetrics().height();
    int textY = (height() + textHeight) / 2 - painter.fontMetrics().descent();
    if (!m_timer.isActive()) {
        m_offset = this->width();
    } else if (m_textWidth != textWidth && textWidth > 0) {
        m_textWidth = textWidth;
        m_offset = this->width();
    }
    if (m_direction == RIGHT_TO_LEFT) {
        painter.drawText(m_labelWidth - m_offset, textY, text());
    } else {
        painter.drawText(m_offset - m_textWidth, textY, text());
    }
}

void MarqueeLabel::resizeEvent(QResizeEvent *event)
{
    int oldWidth = event->oldSize().width();
    int newWidth = event->size().width();
    if (newWidth > 10) {
        m_labelWidth = newWidth;
        // 新宽度更小, 则重置偏移
        if (newWidth < oldWidth) {
            m_offset = 0;
        }
    }
    QLabel::resizeEvent(event);
}
