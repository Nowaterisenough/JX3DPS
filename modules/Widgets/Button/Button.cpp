/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-30 19:08:01
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Button.h"

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
    m_hoverColor  = hover;
    this->update();
}

void Button::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setBrush(QBrush(m_color));
    painter.setPen(QPen(m_color));
    painter.drawRoundedRect(this->rect(), 1, 1);

    painter.setPen(QPen(m_textColor));
    painter.setFont(QFont(painter.font().family(), painter.font().pointSize()));
    painter.drawText(this->rect(), Qt::AlignCenter, this->text());
    painter.drawText(this->rect(), Qt::AlignCenter, this->text());
}

void Button::enterEvent(QEnterEvent *event)
{
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
    animation2->setEndValue(QColor(193, 255, 203));
    animation2->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    animation2->start(QAbstractAnimation::DeleteWhenStopped);
}

CloseButton::CloseButton(QWidget *parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
}

QColor CloseButton::GetColor() const
{
    return m_color;
}

void CloseButton::SetColor(QColor color)
{
    m_color = color;
    this->update();
}

QColor CloseButton::GetTextColor() const
{
    return m_textColor;
}

void CloseButton::SetTextColor(QColor color)
{
    m_textColor = color;
    this->update();
}

void CloseButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setBrush(QBrush(m_color));
    painter.setPen(QColor(64, 64, 64));

    // 画一个只有右上角为(10, 10)的圆角矩形
    painter.drawRoundedRect(this->rect(), 10, 10);
    painter.setPen(QPen(m_color));
    painter.drawRect(this->rect().x(), this->rect().y(), this->rect().width() - 10, this->rect().height() - 10);
    painter.drawRect(this->rect().x(), this->rect().y() + 10, this->rect().width(), this->rect().height() - 10);
    painter.setPen(QColor(64, 64, 64));
    painter.drawLine(this->rect().x(), this->rect().y(), this->rect().width() - 10, this->rect().y());
    painter.drawLine(this->rect().x() + this->rect().width(),
                     this->rect().y() + 10,
                     this->rect().x() + this->rect().width(),
                     this->rect().y() + this->rect().height());

    painter.setPen(QPen(m_textColor, 1.5));
    painter.setBrush(m_textColor);

    // 在正中心画一个X
    painter.drawLine(this->rect().center().x() - 4,
                     this->rect().center().y() - 4,
                     this->rect().center().x() + 4,
                     this->rect().center().y() + 4);
    painter.drawLine(this->rect().center().x() - 4,
                     this->rect().center().y() + 4,
                     this->rect().center().x() + 4,
                     this->rect().center().y() - 4);
}

void CloseButton::enterEvent(QEnterEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(150);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(226, 42, 39));
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *animation2 = new QPropertyAnimation(this, "textColor");
    animation2->setDuration(150);
    animation2->setStartValue(m_textColor);
    animation2->setEndValue(QColor(COLOR_FOCUS));
    animation2->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    animation2->start(QAbstractAnimation::DeleteWhenStopped);
}

void CloseButton::leaveEvent(QEvent *event)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "color");
    animation->setDuration(200);
    animation->setStartValue(m_color);
    animation->setEndValue(QColor(COLOR_BACKGROUND_BASE));
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *animation2 = new QPropertyAnimation(this, "textColor");
    animation2->setDuration(150);
    animation2->setStartValue(m_textColor);
    animation2->setEndValue(QColor(COLOR_INACTIVE));
    animation2->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
    animation2->start(QAbstractAnimation::DeleteWhenStopped);
}