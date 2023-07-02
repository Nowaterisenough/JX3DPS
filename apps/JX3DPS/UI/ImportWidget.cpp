/**
 * Project: JX3DPS
 * File: ImportWidget.cpp
 * Description:
 * Created Date: 2023-07-02 17:47:40
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-02 19:26:34
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "ImportWidget.h"

#include <QLayout>
#include <QPainter>

#include "Button.h"
#include "PlainTextEdit.h"

ImportWidget::ImportWidget(QWidget *parent) : QDialog(parent)
{
    m_frameless = new FramelessWidget(this);
    m_frameless->SetWidget(this);

    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setFixedSize(530, 500);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setModal(true);

    PlainTextEdit *textEdit = new PlainTextEdit(this);
    textEdit->setWordWrapMode(QTextOption::NoWrap);
    Button *button = new Button(this);
    button->setFixedHeight(30);
    button->setFont(QFont("微软雅黑", 11));
    button->setText("应用");
    
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(textEdit, 0, 0, 1, 2);
    layout->addWidget(button, 1, 0, 1, 2);

    connect(button, &Button::clicked, this, &ImportWidget::close);
}

void ImportWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(COLOR_BACKGROUND_BASE));
    painter.drawRoundedRect(this->rect(), 20, 20);
}
