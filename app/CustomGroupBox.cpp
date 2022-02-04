#include "CustomGroupBox.h"

#include <QPainter>

CustomGroupBox::CustomGroupBox(QWidget *parent) : QGroupBox(parent)
{

}

void CustomGroupBox::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(QColor(160, 160, 160));
    painter.drawLine(244, 37, 244, this->height() - 30);
//    painter.setPen(Qt::black);
//    painter.drawText(QRectF(242, 37, 250, 47), "收\n益");
    QGroupBox::paintEvent(e);
}
