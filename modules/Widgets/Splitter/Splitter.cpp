/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 03:53:37
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Splitter.h"

#include <QEvent>
#include <QIntValidator>
#include <QPainter>
#include <QSplitterHandle>

#include "Common/ThemeColors.h"

Splitter::Splitter(QWidget *parent) : QSplitter(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setHandleWidth(10);
}

void Splitter::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QList<QSplitterHandle *> handles = this->findChildren<QSplitterHandle *>();
    for (QSplitterHandle *handle : handles) {
        QPalette handlePalette = handle->palette();
        handlePalette.setColor(QPalette::Window, QColor(COLOR_BACKGROUND_BASE));
        handle->setPalette(handlePalette);
    }

    QSplitter::paintEvent(event);
}
