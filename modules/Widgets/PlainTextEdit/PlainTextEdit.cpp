/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-09 05:19:24
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "PlainTextEdit.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QIntValidator>
#include <QPainter>

#include "Common/ThemeColors.h"

PlainTextEdit::PlainTextEdit(QWidget *parent) : QPlainTextEdit(parent)
{
    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setStyleSheet(
        QString("QPlainTextEdit {"
                "background-color: rgb(%1);"
                "border: 2px solid rgb(%2);"
                "border-radius: 5px;"
                "color: rgb(%3);"
                "padding: 5px;"
                "font-size: 13px;"
                "selection-background-color: rgba(120, 198, 255, 80);"
                "}"
                "QPlainTextEdit:hover {"
                "border: 1px solid rgb(%4);"
                "}"
                "QPlainTextEdit:focus {"
                "border: 1px solid rgb(%5);"
                "}")
            .arg(TO_STR(COLOR_BACKGROUND_HIGHLIGHT))
            .arg(TO_STR(COLOR_BACKGROUND_HIGHLIGHT))
            .arg(TO_STR(COLOR_HIGHLIGHT))
            .arg(TO_STR(COLOR_ACTIVE))
            .arg(TO_STR(COLOR_ACTIVE)));

    int     index = QFontDatabase::addApplicationFont(":/resources/Monaco.ttf");
    QString str   = QFontDatabase::applicationFontFamilies(index).at(0);
    str.append(",NoWatsFont");
    QFont font(str);
    this->setFont(font);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWordWrapMode(QTextOption::NoWrap);
    installEventFilter(this);
}
