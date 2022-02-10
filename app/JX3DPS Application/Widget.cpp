/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-07 01:11:57
 * @Update      : NoWats
 * @LastTime    : 2022-02-09 01:13:05
 * @FilePath    : \JX3DPS\app\JX3DPS Application\Widget.cpp
 */

#include "Widget.h"

#include <QApplication>
#include <QLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QPushButton>
#include <QComboBox>

#include "JX3DPS/JX3DPS.h"

constexpr int JX3DPS_UI_HEIGHT = 31;

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    qApp->setFont(QFont("NoWatsFont", 9.5));
    // qApp->setFont(QFont("微软雅黑", 9.5));
    this->setWindowTitle(JX3DPS::Name());

    QGridLayout *layout    = new QGridLayout(this);
    QHBoxLayout *layout1   = new QHBoxLayout(this);
    QVBoxLayout *layout2   = new QVBoxLayout(this);
    QTabWidget  *tabWidget = new QTabWidget(this);
    layout->addLayout(layout1, 0, 0);
    layout->addLayout(layout2, 0, 1);
    layout->addWidget(tabWidget, 1, 0, 1, 2);

    tabWidget->addTab(new QWidget(this), "关于");

    QGroupBox *groupBox  = new QGroupBox("设置", this);
    QGroupBox *groupBox2 = new QGroupBox("输出", this);
    layout1->addWidget(groupBox);
    layout1->addWidget(groupBox2);

    groupBox->setContentsMargins(10, 20, 10, 11);

    QPushButton *btn  = new QPushButton(JX3DPS::Version(), this);
    QPushButton *btn2 = new QPushButton(JX3DPS::Branch(), this);
    layout2->addWidget(btn);
    layout2->addWidget(btn2);

    btn->setFixedHeight(JX3DPS_UI_HEIGHT);
    // btn2->setFixedHeight(53);

    QGridLayout *layout3          = new QGridLayout(groupBox);
    QComboBox   *comboxJX3Class   = new QComboBox(groupBox);
    QPushButton *btnSimulateCount = new QPushButton("次数", groupBox);
    QPushButton *btnSimulateDelay = new QPushButton("延迟", groupBox);
    layout3->addWidget(comboxJX3Class, 0, 0, 2, 1);
    layout3->addWidget(btnSimulateCount, 0, 1);
    layout3->addWidget(btnSimulateDelay, 1, 1);
    layout3->setContentsMargins(0, 0, 0, 0);

    comboxJX3Class->setFixedSize(66, 66);
    btnSimulateCount->setFixedHeight(JX3DPS_UI_HEIGHT);
    btnSimulateDelay->setFixedHeight(JX3DPS_UI_HEIGHT);
}

Widget::~Widget() {}
