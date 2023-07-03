/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-04 01:22:40
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "LineEdit.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QIntValidator>
#include <QPainter>

#include "ThemeColors.h"

LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    // 创建一个阴影效果对象
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setColor(Qt::black);     // 设置阴影的颜色
    shadowEffect->setBlurRadius(5);        // 设置阴影的模糊半径
    shadowEffect->setOffset(0, 0);         // 设置阴影的偏移量
    this->setGraphicsEffect(shadowEffect); // 为按钮应用阴影效果

    this->setStyleSheet(
        QString("QLineEdit{"
                "background-color: rgb(%1);"
                "border: none;"
                "color: rgb(%2);"
                "selection-background-color: rgb(56, 60, 67);"
                "selection-color: rgb(%3);"
                "font-size: 10pt;"
                "}"
                "QLineEdit::hover{"
                "background-color: rgb(35, 38, 46);"
                "}"
                "QLineEdit::focus{"
                "background-color: rgb(35, 38, 46);"
                "}"
                "QLineEdit::disabled{"
                "background-color: rgb(%4);"
                "color: rgb(%5);"
                "}")
            .arg(TO_STR(COLOR_BACKGROUND_HIGHLIGHT))
            .arg(TO_STR(COLOR_FOCUS))
            .arg(TO_STR(COLOR_FOCUS))
            .arg(TO_STR(COLOR_BACKGROUND_HIGHLIGHT))
            .arg(TO_STR(COLOR_HIGHLIGHT)));

    this->setValidator(new QIntValidator(1, 9999999, this));
}
