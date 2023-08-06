/**
 * Project: JX3DPS
 * File: JX3DPSWidget.cpp
 * Description:
 * Created Date: 2023-08-06 06:46:22
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-06 23:34:30
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "JX3DPSWidget.h"

#include <QLayout>

#include "TabWidget/TabWidget.h"

#include "JX3DPS.h"

JX3DPSWidget::JX3DPSWidget(QWidget *parent)
{
    QString title = QString("%1  %3").arg(APP_NAME).arg(JX3DPSVersion());
    this->SetTitle(title);

    QGridLayout *layout = new QGridLayout(this->centralWidget);

    TabWidget *tabWidget = new TabWidget(this->centralWidget);
    tabWidget->AddTab("紫气爆发宏");
    tabWidget->AddTab("常规宏");
    tabWidget->AddTab("常规宏");

    layout->addWidget(tabWidget);
}

JX3DPSWidget::~JX3DPSWidget() { }
