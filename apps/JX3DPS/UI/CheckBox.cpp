/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-18 00:28:20
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

    painter.drawRect(6, 6, this->width() - 12, this->width() - 12);

    if (this->isChecked()) {
        // 画个对号
        painter.setPen(QPen(QColor(26, 159, 255), 2.5));
        // painter.setBrush(QColor(26, 159, 255));
        int width = this->width() - 12;

        painter.drawLines(QVector<QLineF>()
                          << QLineF(6 + 1, 6 + width / 2, 6 + width / 2 - 1, 6 + width - 2)
                          << QLineF(6 + width / 2 - 1, 6 + width - 2, 6 + width, 6 + width / 2 - 3));

        // painter.drawLines(QVector<QLineF>()
        //                   << QLineF(6, 6 + width / 2, 6 + width / 2 - 1, 6 + width - 1)
        //                   << QLineF(6 + width / 2 - 1, 6 + width - 1, 6 + width, 6 + width / 2 - 4));
    }
}

void CheckBox::enterEvent(QEnterEvent *event)
{
    m_color = QColor(35, 38, 46);
    update();
}

void CheckBox::leaveEvent(QEvent *event)
{
    m_color = QColor(43, 46, 50);
    update();
}

void CheckBox::mousePressEvent(QMouseEvent *event)
{
    this->setChecked(!this->isChecked());
    update();
   // QCheckBox::mousePressEvent(event);
}
