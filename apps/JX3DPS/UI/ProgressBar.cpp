/**
 * Project: JX3DPS
 * File: ProgressBar.cpp
 * Description:
 * Created Date: 2023-06-30 23:42:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-13 21:34:38
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "ProgressBar.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

#include "BlurImage.hpp"
#include "ThemeColors.h"

ProgressBar::ProgressBar(QWidget *parent) : QDialog(parent)
{
    m_frameless = new FramelessWidget(this);
    m_frameless->SetWidget(this);

    this->setAttribute(Qt::WA_TranslucentBackground);

    this->setFixedSize(400, 80);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setModal(true);
}

void ProgressBar::SetProgress(double progress)
{
    if (progress == 1.0) {
        this->close();
    } else {
        m_progress = progress;
        this->update();
    }
}

void ProgressBar::SetLoadMode()
{
    m_loadMode = true;
}

double ProgressBar::GetProgress() const
{
    return m_progress;
}

void ProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(COLOR_BACKGROUND_BASE));
    painter.drawRoundedRect(this->rect(), 20, 20);

    QLinearGradient gradient(20, 0, 380, 0);
    gradient.setColorAt(0.0, QColor(0, 210, 242, 230));
    gradient.setColorAt(0.5, QColor(0, 129, 245, 230));
    gradient.setColorAt(1.0, QColor(69, 70, 246, 230));

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidth(1);
    pen.setBrush(QColor(COLOR_BACKGROUND_PRIMARY));

    painter.setPen(pen);
    painter.setBrush(QColor(COLOR_BACKGROUND_PRIMARY));
    painter.drawRoundedRect(20, 20, 360, 10, 5, 5);

    pen.setBrush(gradient);
    QImage blur(this->rect().size(), QImage::Format_ARGB32);
    blur.fill(QColor(0, 0, 0, 0));
    QPainter blurPainter(&blur);
    blurPainter.setRenderHint(QPainter::Antialiasing);
    blurPainter.setPen(pen);
    blurPainter.setBrush(gradient);
    blurPainter.drawRoundedRect(20, 20, 360 * m_progress, 10, 5, 5);
    blurPainter.end();

    qt_blurImage(blur, 40.0, false, false);
    painter.drawImage(this->rect(), blur);

    painter.setPen(pen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(20, 20, 360 * m_progress, 10, 5, 5);

    painter.setPen(QPen(QColor(COLOR_HIGHLIGHT)));
    painter.setBrush(Qt::NoBrush);
    painter.setFont(QFont("NoWatsFont", 11));

    double percentage = m_progress * 100;
    QByteArray byteArray = QByteArray::number(percentage, 'f', 0); // 转换为整数形式的字符串


    QString text = m_loadMode ? "心法配置加载中..." : "正在模拟中...";

    painter.drawText(0,
                     20,
                     this->width(),
                     this->height() - 20,
                     Qt::AlignCenter,
                     QString("%1%  %2").arg(byteArray.constData()).arg(text));
    painter.drawText(0,
                     20,
                     this->width(),
                     this->height() - 20,
                     Qt::AlignCenter,
                     QString("%1%  %2").arg(byteArray.constData()).arg(text));
}
