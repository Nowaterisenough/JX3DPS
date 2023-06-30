/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-30 13:36:09
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "CheckBox.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QIntValidator>
#include <QPainter>

#include "ThemeColors.h"

CheckBox::CheckBox(QWidget *parent) : QCheckBox(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(5);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果
}

void CheckBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setPen(QPen(QColor(56, 60, 67), 1));
    painter.setBrush(m_color);

    painter.drawRect(3, 7, this->height() - 12, this->height() - 12);

    if (this->isChecked()) {
        // 画个对号
        painter.setPen(QPen(QColor(COLOR_ACTIVE), 2.5));
        int width = this->height() - 12;

        painter.drawLines(QVector<QLineF>()
                          << QLineF(3 + 1, 7 + width / 2, 3 + width / 2 - 1, 7 + width - 2)
                          << QLineF(3 + width / 2 - 1, 7 + width - 2, 3 + width, 7 + width / 2 - 3));
    }

    painter.setPen(QPen(QColor(COLOR_INACTIVE), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawText(3 + this->height() - 12 + 6,
                     0,
                     this->width() - (3 + this->height() - 12 + 6),
                     this->height(),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     this->text());
    painter.drawText(3 + this->height() - 12 + 6,
                     0,
                     this->width() - (3 + this->height() - 12 + 6),
                     this->height(),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     this->text());
}

void CheckBox::enterEvent(QEnterEvent *event)
{
    m_color = QColor(35, 38, 46);
    update();
}

void CheckBox::leaveEvent(QEvent *event)
{
    m_color = QColor(COLOR_BACKGROUND_HIGHLIGHT);
    update();
}

void CheckBox::mousePressEvent(QMouseEvent *event)
{
    this->setChecked(!this->isChecked());
    update();
    // QCheckBox::mousePressEvent(event);
}
