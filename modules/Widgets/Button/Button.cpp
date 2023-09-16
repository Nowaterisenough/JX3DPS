/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-08 07:05:13
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Button.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPropertyAnimation>

TextButton::TextButton(QWidget *parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
}

QColor TextButton::GetColor() const
{
    return m_color;
}

void TextButton::SetColor(QColor color)
{
    m_color = color;
    this->update();
}

void TextButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(QPen(m_color));
    painter.setFont(QFont(painter.font().family(), painter.font().pointSize()));
    painter.drawText(this->rect(), Qt::AlignCenter, this->text());
    painter.drawText(this->rect(), Qt::AlignCenter, this->text());
}

void TextButton::enterEvent(QEnterEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(150);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(COLOR_FOCUS));
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    this->setCursor(Qt::PointingHandCursor);
}

void TextButton::leaveEvent(QEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(200);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(COLOR_INACTIVE));
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

Button::Button(QWidget *parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);

    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black); // 设置阴影的颜色
    shadowEffect->setBlurRadius(10);   // 设置阴影的模糊半径

    shadowEffect->setOffset(0, 0); // 设置阴影的偏移量

    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果
}

QColor Button::GetColor() const
{
    return m_color;
}

void Button::SetColor(QColor color)
{
    m_color = color;
    this->update();
}

QColor Button::GetTextColor() const
{
    return m_textColor;
}

void Button::SetTextColor(QColor color)
{
    m_textColor = color;
    this->update();
}

void Button::SetButtonColor(const QColor &hover, const QColor &normal)
{
    m_color = m_normalColor = normal;
    m_hoverColor            = hover;
    this->update();
}

void Button::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    if (this->isEnabled()) {
        painter.setBrush(QBrush(m_color));
        painter.setPen(QPen(m_color));
        painter.drawRoundedRect(this->rect(), 1, 1);

        painter.setPen(QPen(m_textColor));
        painter.setFont(QFont(painter.font().family(), painter.font().pointSize()));
        painter.drawText(this->rect(), Qt::AlignCenter, this->text());
        painter.drawText(this->rect(), Qt::AlignCenter, this->text());
    } else {
        painter.setBrush(QBrush(QColor(100, 100, 100)));
        painter.setPen(QPen(QColor(100, 100, 100)));
        painter.drawRoundedRect(this->rect(), 1, 1);

        painter.setPen(QPen(QColor(180, 180, 180)));
        painter.setFont(QFont(painter.font().family(), painter.font().pointSize()));
        painter.drawText(this->rect(), Qt::AlignCenter, this->text());
        painter.drawText(this->rect(), Qt::AlignCenter, this->text());
    }
}

void Button::enterEvent(QEnterEvent *event)
{
    if (this->isEnabled()) {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
        animation->setDuration(150);
        animation->setStartValue(m_color);
        animation->setEndValue(m_hoverColor);
        animation->setEasingCurve(QEasingCurve::InOutQuad);

        QPropertyAnimation *animation2 = new QPropertyAnimation(this, "textColor");
        animation2->setDuration(150);
        animation2->setStartValue(m_textColor);
        animation2->setEndValue(QColor(COLOR_FOCUS));
        animation2->setEasingCurve(QEasingCurve::InOutQuad);

        animation->start(QAbstractAnimation::DeleteWhenStopped);
        animation2->start(QAbstractAnimation::DeleteWhenStopped);

        this->setCursor(Qt::PointingHandCursor);
    }
}

void Button::leaveEvent(QEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(200);
    animation->setStartValue(m_color);
    animation->setEndValue(m_normalColor);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *animation2 = new QPropertyAnimation(this, "textColor");
    animation2->setDuration(150);
    animation2->setStartValue(m_textColor);
    animation2->setEndValue(QColor(220, 220, 220));
    animation2->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    animation2->start(QAbstractAnimation::DeleteWhenStopped);
}
