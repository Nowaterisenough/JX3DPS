/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-17 16:47:50
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Splitter.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QIntValidator>
#include <QPainter>

Splitter::Splitter(QWidget *parent) : QSplitter(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setHandleWidth(0);
}

void Splitter::paintEvent(QPaintEvent *event) { }
