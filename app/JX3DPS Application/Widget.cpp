/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-07 01:11:57
 * @Update      : NoWats
 * @LastTime    : 2022-02-07 01:58:22
 * @FilePath    : \JX3DPS\app\JX3DPS Application\Widget.cpp
 */

#include "Widget.h"

#include <QPushButton>

#include "JX3DPS/JX3DPS.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle("JX3DPS");
    QPushButton *btn = new QPushButton(JX3DPS::Simulator::Version(), this);
    QPushButton *btn2 = new QPushButton(JX3DPS::Simulator::Branch(), this);
}

Widget::~Widget()
{
}
