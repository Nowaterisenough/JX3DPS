/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-16 18:51:51
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "CheckBox.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QPainter>

#include "Common/ThemeColors.h"

CheckBox::CheckBox(QWidget *parent) : QCheckBox(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(5);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    // 设置样式表
    this->setStyleSheet(
        "QToolTip { color: white; background-color: rgb(23, 29, 37); border: none; }");
}

void CheckBox::SetItemInfo(const ItemInfo &itemInfo)
{
    m_itemInfo = itemInfo;
    this->setToolTip(itemInfo.description);
}

CheckBox::ItemInfo CheckBox::GetItemInfo() const
{
    return m_itemInfo;
}

void CheckBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setPen(QPen(QColor(56, 60, 67), 1));

    if (m_hovered) {
        painter.setBrush(QColor(35, 38, 46));
    } else {
        painter.setBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT));
    }

    painter.drawRect(3, 7, this->height() - 12, this->height() - 12);

    if (this->isChecked()) {
        // 画个对号
        painter.setPen(QPen(QColor(COLOR_ACTIVE), 2.5));
        int width = this->height() - 12;

        painter.drawLines(QVector<QLineF>()
                          << QLineF(3 + 1, 7 + width / 2, 3 + width / 2 - 1, 7 + width - 2)
                          << QLineF(3 + width / 2 - 1, 7 + width - 2, 3 + width, 7 + width / 2 - 3));
    }

    if (m_hovered) {
        painter.setPen(QPen(QColor(COLOR_HIGHLIGHT), 1));
    } else {
        painter.setPen(QPen(QColor(COLOR_INACTIVE), 1));
    }

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
    m_hovered = true;
    update();
}

void CheckBox::leaveEvent(QEvent *event)
{
    m_hovered = false;
    update();
}

void CheckBox::mousePressEvent(QMouseEvent *event)
{
    this->setChecked(!this->isChecked());
    update();
    // QCheckBox::mousePressEvent(event);
}

CheckBoxIcon::CheckBoxIcon(QWidget *parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);

        // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(4);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    // 设置样式表
    this->setStyleSheet(
        "QToolTip { color: white; background-color: rgb(23, 29, 37); border: none; }");
}

void CheckBoxIcon::SetItemInfo(const CheckBox::ItemInfo &itemInfo)
{
    m_itemInfo = itemInfo;
    this->setToolTip(m_itemInfo.name + "\n" + m_itemInfo.description);
    m_pixmap = QPixmap(m_itemInfo.iconPath);
}

CheckBox::ItemInfo CheckBoxIcon::GetItemInfo() const
{
    return m_itemInfo;
}

void CheckBoxIcon::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    painter.setPen(QPen(Qt::black));
    if (m_hovered) {
        painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_BASE)));
    } else {
        painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT)));
    }
    painter.drawRect(this->rect());

    // 画一个框
    if (m_pixmap.isNull()) {
        return;
    }
    int border = 3;
    painter.setPen(QPen(Qt::black));

    painter.setBrush(QBrush(QColor(COLOR_BACKGROUND_HIGHLIGHT)));

    painter.drawRect(border, border, this->height() - border * 2, this->height() - border * 2);

    QIcon icon(m_pixmap.copy(m_pixmap.width() / 12,
                             m_pixmap.width() / 12,
                             m_pixmap.width() / 12 * 10,
                             m_pixmap.width() / 12 * 10));

    QIcon::Mode mode = (m_hovered || this->isChecked()) ? QIcon::Normal : QIcon::Disabled;
    painter.drawPixmap(border, border, icon.pixmap(this->size(), mode).scaledToWidth(this->height() - border * 2));
}

void CheckBoxIcon::enterEvent(QEnterEvent *event)
{
    m_hovered = true;
    QCheckBox::enterEvent(event);
}

void CheckBoxIcon::leaveEvent(QEvent *event)
{
    m_hovered = false;
    QCheckBox::leaveEvent(event);
}

void CheckBoxIcon::mousePressEvent(QMouseEvent *event)
{
    this->setChecked(!this->isChecked());
    update();
}
