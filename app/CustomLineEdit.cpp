/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 15:05:30
 * @FilePath    : \JX3DPS\app\CustomLineEdit.cpp
 */
#include "CustomLineEdit.h"

#include <QPainter>
#include <QPropertyAnimation>
#include <QEvent>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

double CustomLineEdit::s_valueMax = 0.02;
double CustomLineEdit::s_valueMin = 0.00;

CustomLineEdit::CustomLineEdit(QWidget *parent) : QLineEdit(parent)
{
}

void CustomLineEdit::SetValueMax(const double max)
{
    s_valueMax = max;
}

void CustomLineEdit::SetValueMin(const double min)
{
    s_valueMin = min;
}

double CustomLineEdit::GetValue() const
{
    return m_value;
}

void CustomLineEdit::SetValue(double value)
{
    if (value < 0.0) {
        value = m_value;
    }
    if (s_valueMax < s_valueMin) {
        return;
    }
    double r = m_rate;
    double rate = (value - s_valueMin) / (s_valueMax - s_valueMin);
    if (s_valueMax == s_valueMin) {
        rate = 0.0;
    }
    rate = rate < 0.0 ? 0.0 : rate;
    rate = rate > 1.0 ? 1.0 : rate;

    m_rate = rate;
    QPropertyAnimation *animation = new QPropertyAnimation(this, "rate");
    animation->setDuration(350);
    animation->setStartValue(r);
    animation->setEndValue(rate);
    animation->setEasingCurve(QEasingCurve::OutBack);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    m_value = value;
}

double CustomLineEdit::GetRate() const
{
    return m_rate;
}

void CustomLineEdit::SetRate(double rate)
{
    m_rate = rate;
}

double CustomLineEdit::GetOpacity() const
{
    return m_opacity;
}

void CustomLineEdit::SetOpacity(double opacity)
{
    m_opacity = opacity;
}

void CustomLineEdit::paintEvent(QPaintEvent *e)
{
    QLineEdit::paintEvent(e);
    QPainter painter(this);
    int width = GetWidth(m_rate, this->width() - 5);
    if (this->isEnabled()) {
        painter.setPen(Qt::white);
        painter.setBrush(Qt::white);
        painter.drawRect(2, 2, this->width() - 5, this->height() - 5);

        painter.setPen(GetColor(m_rate));
        painter.setBrush(GetColor(m_rate));

        if (width > 0) {
            painter.drawRect(2, 2, width, this->height() - 5);
        }
        painter.setPen(Qt::black);
        painter.drawText(2, 2, this->width() - 5, this->height() - 5, Qt::AlignRight | Qt::AlignVCenter, QString::number(s_valueMin + m_rate * (s_valueMax - s_valueMin) * 100, 'f', 2) + "%");
    } else {
        painter.setPen(QColor(240, 240, 240));
        painter.setBrush(QColor(240, 240, 240));
        painter.drawRect(2, 2, this->width() - 5, this->height() - 5);

        painter.setPen(QColor(109, 109, 109));
        painter.drawText(2, 2, this->width() - 5, this->height() - 5, Qt::AlignRight | Qt::AlignVCenter, QString::number(m_value * 100, 'f', 2) + "%");
    }

    painter.setOpacity(m_opacity);
    if (m_isPressed) {
        painter.setPen(QColor(0, 85, 155));
        painter.setBrush(QColor(204, 228, 247));
    } else {
        painter.setPen(QColor(12, 123, 212));
        painter.setBrush(QColor(229, 241, 251));
    }
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

    if (this->isEnabled()) {
        if (m_isPressed) {
            painter.setBrush(QColor(0, 85, 155));
        } else {
            painter.setBrush(QColor(12, 123, 212));
        }
        if (width > 0) {
            painter.drawRect(2, 2, width, this->height() - 5);
        }
    } else {
        if (m_isPressed) {
            painter.setPen(QColor(0, 85, 155));
            painter.setBrush(QColor(204, 228, 247));
        } else {
            painter.setPen(QColor(12, 123, 212));
            painter.setBrush(QColor(229, 241, 251));
        }
        painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
    }
    painter.setPen(Qt::black);
    if (this->isEnabled()) {
        painter.drawText(2, 2, this->width() - 5, this->height() - 5, Qt::AlignHCenter | Qt::AlignVCenter, "禁用"/*QString::number(s_valueMin + m_rate * (s_valueMax - s_valueMin) * 100, 'f', 2) + "%"*/);
    } else {
        painter.drawText(2, 2, this->width() - 5, this->height() - 5, Qt::AlignHCenter | Qt::AlignVCenter, "启用"/*QString::number(m_value * 100, 'f', 2) + "%"*/);
    }
}

bool CustomLineEdit::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::MouseButtonPress : {
        m_isPressed = true;
        break;
    }
    case QEvent::MouseButtonRelease : {
        m_isPressed = false;
        m_rate = 0.0;   // 动态效果
        this->setEnabled(!this->isEnabled());
        emit ChangeEnable(this->isEnabled());
        this->SetValue(m_value);    // 动态效果
        break;
    }
    case QEvent::Enter : {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity");
        animation->setDuration(150);
        animation->setStartValue(m_opacity);
        animation->setEndValue(1.0);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        break;
    }
    case QEvent::Leave : {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity");
        animation->setDuration(150);
        animation->setStartValue(m_opacity);
        animation->setEndValue(0.0);
        animation->setEasingCurve(QEasingCurve::InOutQuad);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
        break;
    }
    default:
        break;
    }
    return QLineEdit::event(event);
}

QColor CustomLineEdit::GetColor(const double rate)
{
    if (rate > 1.0) {
        return QColor(112, 173, 71);
    } else if (rate > 5.0 / 7) {
        double colorRate = rate * 3.5 - 2.5;
        int r = 255 + static_cast<int>((112 - 255) * colorRate);
        int g = 192 + static_cast<int>((173 - 192) * colorRate);
        int b = 0 + static_cast<int>((71 - 0) * colorRate);
        return QColor(r, g, b);
    } else {
        double colorRate = rate * 7 / 5;
        int r = 192 + static_cast<int>((255 - 192) * colorRate);
        int g = 0 + static_cast<int>((192 - 0) * colorRate);
        int b = 0 + static_cast<int>((0 - 0) * colorRate);
        return QColor(r, g, b);
    }
}

int CustomLineEdit::GetWidth(const double rate, int width)
{
    update();
    return static_cast<int>(width * rate);
}
