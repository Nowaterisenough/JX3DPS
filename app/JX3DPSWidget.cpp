/**
 * Project: JX3DPS
 * File: JX3DPSWidget.cpp
 * Description:
 * Created Date: 2023-08-06 06:46:22
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 04:18:54
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "JX3DPSWidget.h"

#include <QLayout>

#include "Button/Button.h"
#include "CheckBox/CheckBox.h"
#include "GroupBox/GroupBox.h"
#include "LineEdit/LineEdit.h"
#include "Splitter/Splitter.h"
#include "TabWidget/TabWidget.h"

#include "JX3DPS.h"

JX3DPSWidget::JX3DPSWidget(QWidget *parent)
{
    QString title = QString("%1  %3").arg(APP_NAME).arg(JX3DPSVersion());
    this->SetTitle(title);

    GroupBox  *groupBoxSetting         = new GroupBox("设置", this->centralWidget);
    TabWidget *tabWidgetAttribute      = new TabWidget(this->centralWidget);
    Button    *buttonSimulate          = new Button(this->centralWidget);
    GroupBox  *groupBoxOut             = new GroupBox("输出", this->centralWidget);
    TabWidget *tabWidgetGains          = new TabWidget(this->centralWidget);
    TabWidget *tabWidgetTalentsRecipes = new TabWidget(this->centralWidget);
    GroupBox  *groupBoxPermanents = new GroupBox("常驻增益", this->centralWidget);
    Splitter  *splitter           = new Splitter(this->centralWidget);
    TabWidget *tabWidgetSkills    = new TabWidget(this->centralWidget);
    TabWidget *tabWidgetEvents    = new TabWidget(this->centralWidget);

    buttonSimulate->setText("开始模拟");
    buttonSimulate->SetButtonColor(QColor(COLOR_BUTTON_GREEN_HOVER), QColor(COLOR_BUTTON_GREEN_NORMAL));
    buttonSimulate->setFixedSize(156, 44);
    buttonSimulate->setFont(QFont(buttonSimulate->font().family(), 14));

    tabWidgetAttribute->AddTab("属性");
    tabWidgetAttribute->AddTab("配装");

    tabWidgetGains->AddTab("收益");

    tabWidgetTalentsRecipes->AddTab("奇穴");
    tabWidgetTalentsRecipes->AddTab("秘籍");

    tabWidgetSkills->AddTab("宏");
    tabWidgetSkills->SetAddButtonVisible(true);
    tabWidgetSkills->setFixedWidth(300);

    tabWidgetEvents->AddTab("事件");

    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(tabWidgetSkills);
    splitter->addWidget(tabWidgetEvents);

    QGridLayout *layout = new QGridLayout(this->centralWidget);
    layout->addWidget(groupBoxSetting, 0, 0, 2, 1);
    layout->addWidget(tabWidgetAttribute, 2, 0, 2, 1);
    layout->addWidget(buttonSimulate, 0, 1, 1, 1);
    layout->addWidget(groupBoxOut, 1, 1, 1, 1);
    layout->addWidget(tabWidgetGains, 2, 1, 1, 1);
    layout->addWidget(tabWidgetTalentsRecipes, 3, 1, 1, 2);
    layout->addWidget(groupBoxPermanents, 0, 2, 3, 1);
    layout->addWidget(splitter, 0, 3, 4, 1);

    InitWidgetSetting(groupBoxSetting);
}

JX3DPSWidget::~JX3DPSWidget() { }

void JX3DPSWidget::InitWidgetSetting(QWidget *parent)
{
    TextButton *textButtonSimulateCount = new TextButton(parent);
    textButtonSimulateCount->setText("模拟次数");
    textButtonSimulateCount->setFixedSize(62, 26);

    QGridLayout *gLayout = new QGridLayout(parent);
    gLayout->addWidget(textButtonSimulateCount, 0, 0, 1, 1);
}
