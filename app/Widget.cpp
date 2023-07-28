/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2023-04-08 02:23:52
 * @Update      : NoWats
 * @LastTime    : 2023-04-13 02:29:07
 * @FilePath    : \LegionTDSim\apps\LegionTDSim\Widget.cpp
 */

#include "Widget.h"

#include <QLayout>
#include <QPainter>

#include <spdlog/spdlog.h>

#include "FramelessWidget.h"
#include "TabWidget.h"
#include "ThemeColors.h"
#include "ThreadPool.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    m_frameless = new FramelessWidget(this);
    m_frameless->SetWidget(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFixedSize(300, 500);

    ThreadPool::Instance()->Enqueue([]() { qDebug() << "Hello World!"; });

    QGridLayout *layout    = new QGridLayout(this);
    TabWidget   *tabWidget = new TabWidget(this);

    tabWidget->AddTab("宏");
    tabWidget->AddTab("爆发宏2");
    tabWidget->AddTab("爆发宏1");
    tabWidget->AddTab("技能序列88");
    tabWidget->AddTab(">");

    layout->addWidget(tabWidget);

    spdlog::info("Hello World!");
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(COLOR_BACKGROUND_BASE));
    painter.drawRoundedRect(this->rect(), 10, 10);
}