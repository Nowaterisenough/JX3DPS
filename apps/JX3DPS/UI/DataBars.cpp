/**
 * Project: JX3DPS
 * File: DataBars.cpp
 * Description:
 * Created Date: 2022-01-26 21:34:09
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-08 01:48:04
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "DataBars.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPropertyAnimation>

#include "BlurImage.hpp"
#include "ThemeColors.h"

double                DataBars::s_valueMax = 0.00;
double                DataBars::s_valueMin = 0.00;
std::list<DataBars *> DataBars::s_lineEdits;

DataBars::DataBars(QWidget *parent) : QLineEdit(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setReadOnly(true);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(5);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    s_lineEdits.push_back(this);
    connect(this, &QLineEdit::textChanged, [=](const QString &text) {
        if (text.isEmpty()) {
            return;
        }
        double value = text.toDouble();
        SetValue(value);
    });

    // m_gradient.setColorAt(0, m_color0);  // 渐变起始颜色
    // m_gradient.setColorAt(0.6, m_color1);   // 渐变结束颜色
    // m_gradient.setColorAt(1.0, m_color2);   // 渐变结束颜色
}

DataBars::~DataBars()
{
    for (auto it = s_lineEdits.begin(); it != s_lineEdits.end();) {
        if (*it == this) {
            it = s_lineEdits.erase(it);
            break;
        }
        ++it;
    }
}

double DataBars::GetValue() const
{
    return m_value;
}

void DataBars::SetValue(double value)
{
    if (value < 0.0) {
        m_value = 0.0;
        this->text().clear();
    } else {
        m_value = value;
    }
    UpdateValue();
}

void DataBars::UpdateValue()
{
    s_valueMax = std::numeric_limits<double>::min();
    s_valueMin = std::numeric_limits<double>::max();
    for (auto &lineEdit : s_lineEdits) {
        if (lineEdit->GetValue() > s_valueMax && lineEdit->isEnabled()) {
            s_valueMax = lineEdit->GetValue();
        }
        if (lineEdit->GetValue() < s_valueMin && lineEdit->isEnabled()) {
            s_valueMin = lineEdit->GetValue();
        }
    }

    for (auto &lineEdit : s_lineEdits) {
        double lastRate = lineEdit->GetRate();
        if (lineEdit->isEnabled() == false) {
            lastRate = 0.0;
        }

        double rate = (lineEdit->GetValue() - s_valueMin) / (s_valueMax - s_valueMin);

        QPropertyAnimation *animation = new QPropertyAnimation(lineEdit, "rate");
        animation->setDuration(350);
        animation->setStartValue(lastRate);
        animation->setEndValue(rate);
        animation->setEasingCurve(QEasingCurve::OutBack);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

double DataBars::GetRate() const
{
    return m_rate;
}

void DataBars::SetRate(double rate)
{
    m_rate = rate;
}

double DataBars::GetOpacity() const
{
    return m_opacity;
}

void DataBars::SetOpacity(double opacity)
{
    m_opacity = opacity;
}

void DataBars::paintEvent(QPaintEvent *e)
{
    // QLineEdit::paintEvent(e);
    QPainter painter(this);
    int      width = GetWidth(m_rate, this->width() - 5);

    painter.setPen(QColor(COLOR_BACKGROUND_HIGHLIGHT));
    painter.setBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT));
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
    if (this->isEnabled()) { // 可用
        painter.setPen(QColor(COLOR_BACKGROUND_HIGHLIGHT));
        painter.setBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT));
        painter.drawRect(2, 2, this->width() - 5, this->height() - 5);

        if (width > 0) {
            painter.setPen(GetColor(m_rate));
            painter.setBrush(GetColor(m_rate));

            QRect rect(2, 2, width, this->height() - 5);

            // QImage blur(this->rect().size(), QImage::Format_ARGB32);
            // blur.fill(QColor(0, 0, 0, 0));
            // QPainter blurPainter(&blur);
            // blurPainter.setRenderHint(QPainter::Antialiasing);
            // blurPainter.setPen(GetColor(m_rate));
            // blurPainter.setBrush(GetColor(m_rate));
            // blurPainter.drawRect(rect);
            // blurPainter.end();

            // qt_blurImage(blur, 2.0, true, false);
            // painter.drawImage(this->rect(), blur);

            painter.drawRect(rect);
        }
        painter.setPen(QColor(COLOR_HIGHLIGHT));
    } else {
        painter.setPen(QColor(COLOR_BACKGROUND_PRIMARY));
        painter.setBrush(QColor(COLOR_BACKGROUND_PRIMARY));
        painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
        painter.setPen(QColor(COLOR_BACKGROUND_PRIMARY));
    }
    // painter.setPen(QColor(COLOR_BACKGROUND_HIGHLIGHT));
    // painter.setBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT));

    painter.drawText(2,
                     2,
                     this->width() - 5,
                     this->height() - 5,
                     Qt::AlignRight | Qt::AlignVCenter,
                     QString::number(m_value * 100, 'f', 4) + "%");
    painter.drawText(2,
                     2,
                     this->width() - 5,
                     this->height() - 5,
                     Qt::AlignRight | Qt::AlignVCenter,
                     QString::number(m_value * 100, 'f', 4) + "%");

    // hover
    painter.setOpacity(m_opacity);
    if (m_isPressed) {
        painter.setPen(QColor(COLOR_BACKGROUND_HIGHLIGHT));
        painter.setBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT));
    } else {
        painter.setPen(QColor(COLOR_BACKGROUND_HIGHLIGHT));
        painter.setBrush(QColor(COLOR_BACKGROUND_PRIMARY));
    }
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

    if (this->isEnabled()) {
        if (m_isPressed) {
            painter.setBrush(Qt::black);
        } else {
            painter.setBrush(QColor(COLOR_BACKGROUND_PRIMARY));
        }
        if (width > 0) {
            painter.drawRect(2, 2, width, this->height() - 5);
        }
        painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
    } else {
        if (m_isPressed) {
            painter.setPen(QColor(34, 39, 47));
            painter.setBrush(Qt::black);
        } else {
            painter.setPen(QColor(34, 39, 47));
            painter.setBrush(QColor(COLOR_BACKGROUND_PRIMARY));
        }
        painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
    }
    painter.setPen(QColor(COLOR_FOCUS));
    if (this->isEnabled()) {
        painter.drawText(2, 2, this->width() - 5, this->height() - 5, Qt::AlignHCenter | Qt::AlignVCenter, "禁用");
    } else {
        painter.drawText(2, 2, this->width() - 5, this->height() - 5, Qt::AlignHCenter | Qt::AlignVCenter, "启用");
    }
}

bool DataBars::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::MouseButtonPress:
        {
            m_isPressed = true;
            if (!this->isEnabled()) {
                m_rate = 0.0;
            }
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            m_isPressed = false;
            this->setEnabled(!this->isEnabled());
            UpdateValue();
            emit Signal_EnableChanged(this->isEnabled());
            break;
        }
        case QEvent::Enter:
        {
            QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity");
            animation->setDuration(150);
            animation->setStartValue(m_opacity);
            animation->setEndValue(1.0);
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
            break;
        }
        case QEvent::Leave:
        {
            QPropertyAnimation *animation = new QPropertyAnimation(this, "opacity");
            animation->setDuration(150);
            animation->setStartValue(m_opacity);
            animation->setEndValue(0.0);
            animation->setEasingCurve(QEasingCurve::InOutQuad);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
            break;
        }
        default: break;
    }
    return QLineEdit::event(event);
}

void DataBars::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
}

QColor DataBars::GetColor(const double rate)
{
    if (rate > 1.0) {
        return m_color2;
    } else if (rate > 0.6) {
        double colorRate = (rate - 0.6) / 0.4;

        int r = m_color1.red() + static_cast<int>((m_color2.red() - m_color1.red()) * colorRate);
        int g = m_color1.green() + static_cast<int>((m_color2.green() - m_color1.green()) * colorRate);
        int b = m_color1.blue() + static_cast<int>((m_color2.blue() - m_color1.blue()) * colorRate);
        return QColor(r, g, b);
    } else {
        double colorRate = rate / 0.6;

        int r = m_color0.red() + static_cast<int>((m_color1.red() - m_color0.red()) * colorRate);
        int g = m_color0.green() + static_cast<int>((m_color1.green() - m_color0.green()) * colorRate);
        int b = m_color0.blue() + static_cast<int>((m_color1.blue() - m_color0.blue()) * colorRate);
        return QColor(r, g, b);
    }
}

int DataBars::GetWidth(const double rate, const int width)
{
    update();
    return static_cast<int>(width * rate);
}
