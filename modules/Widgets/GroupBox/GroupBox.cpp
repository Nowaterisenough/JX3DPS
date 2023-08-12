/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 02:43:16
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "GroupBox.h"

#include <QEvent>
#include <QPainter>

#include "Common/ThemeColors.h"

GroupBox::GroupBox(const QString &name, QWidget *parent) : QGroupBox(name, parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void GroupBox::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.setFont(QFont(painter.font().family(), 11));
    // 画一个方框
    painter.setPen(QPen(QColor(COLOR_BACKGROUND_HIGHLIGHT), 1));
    painter.setBrush(Qt::NoBrush);

    painter.drawLines(
        QVector<QLineF>()
        << QLineF(10, painter.fontMetrics().height() / 2, 0, painter.fontMetrics().height() / 2)
        << QLineF(0, painter.fontMetrics().height() / 2, 0, this->height() - 1)
        << QLineF(0, this->height() - 1, this->width() - 1, this->height() - 1)
        << QLineF(this->width() - 1, this->height() - 1, this->width() - 1, painter.fontMetrics().height() / 2)
        << QLineF(this->width() - 1,
                  painter.fontMetrics().height() / 2,
                  10 + painter.fontMetrics().horizontalAdvance(this->title()) + 10,
                  painter.fontMetrics().height() / 2));

    painter.setPen(QPen(QColor(COLOR_HIGHLIGHT)));
    painter.setBrush(Qt::NoBrush);

    painter.drawText(10,
                     0,
                     painter.fontMetrics().horizontalAdvance(this->title()) + 10,
                     painter.fontMetrics().height(),
                     Qt::AlignCenter,
                     this->title());
    painter.drawText(10,
                     0,
                     painter.fontMetrics().horizontalAdvance(this->title()) + 10,
                     painter.fontMetrics().height(),
                     Qt::AlignCenter,
                     this->title());
}

void GroupBox::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event); // QGroupBox的鼠标释放事件不能正常传递给父类，需要手动调用，避免无法拖动窗口
}
