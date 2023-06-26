/**
 * Project: JX3DPS
 * File: SpinBox.cpp
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-26 00:50:20
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

PlainTextEdit::PlainTextEdit(QWidget *parent) : QPlainTextEdit(parent)
{
    this->setAttribute(Qt::WA_NoMousePropagation, true);
    this->setStyleSheet("QPlainTextEdit {"
                        "background-color: rgb(43, 46, 50);"
                        "border: 2px solid rgb(43, 46, 50);"
                        "border-radius: 5px;"
                        "color: rgb(208, 211, 212);"
                        "padding: 5px;"
                        "font-size: 13px;"
                        "selection-color: rgb(13, 19, 27);"
                        "selection-background-color: rgb(120, 198, 255);"
                        "}"
                        "QPlainTextEdit:hover {"
                        "border: 1px solid rgb(26, 159, 255);"
                        "}"
                        "QPlainTextEdit:focus {"
                        "border: 1px solid rgb(26, 159, 255);"
                        "}");

    int     index = QFontDatabase::addApplicationFont(":/resources/Monaco.ttf");
    QString str   = QFontDatabase::applicationFontFamilies(index).at(0);
    str.append(",Microsoft YaHei");
    QFont font(str);
    this->setFont(font);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWordWrapMode(QTextOption::NoWrap);
}
