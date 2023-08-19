/**
 * Project: JX3DPS
 * File: TestUI.cpp
 * Description:
 * Created Date: 2023-08-09 03:08:00
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-09 17:16:02
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "TestUI.h"

#include "ComboBox/ComboBox.h"

TestUI::TestUI(QWidget *parent) : Widget(parent)
{
    QGridLayout *layout = new QGridLayout(this->centralWidget);

    ComboBox *comboBox1 = new ComboBox(this->centralWidget);
    ComboBox *comboBox2 = new ComboBox(this->centralWidget);
    ComboBox *comboBox3 = new ComboBox(this->centralWidget);
    ComboBox *comboBox4 = new ComboBox(this->centralWidget);
    ComboBox *comboBox5 = new ComboBox(this->centralWidget);

    comboBox1->setFixedSize(50, 50);
    comboBox2->setFixedSize(50, 50);
    comboBox3->setFixedSize(50, 70);
    comboBox4->setFixedSize(200, 50);
    comboBox5->setFixedSize(200, 50);

    comboBox1->SetType(ComboBox::Type::DEFAULT);
    comboBox2->SetType(ComboBox::Type::ICON);
    comboBox3->SetType(ComboBox::Type::ICON_NAME);
    comboBox4->SetType(ComboBox::Type::DETAILED);
    comboBox5->SetType(ComboBox::Type::DETAILED);

    ComboBox::ItemInfo itemInfo1;
    itemInfo1.name = "测试";
    itemInfo1.iconPath = "C:\\Users\\NoWat\\Project\\JX3DPS3\\resources\\pics\\JX3\\莫问.png";

    comboBox1->SetItemSize(200, 50);
    comboBox2->SetItemSize(200, 50);
    comboBox3->SetItemSize(200, 50);
    comboBox4->SetItemSize(200, 50);
    comboBox5->SetItemSize(200, 50);
    
    comboBox1->AddItem(itemInfo1);
    comboBox2->AddItem(itemInfo1);
    comboBox3->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);
    itemInfo1.iconPath = "";
    comboBox1->AddItem(itemInfo1);
    comboBox2->AddItem(itemInfo1);
    comboBox3->AddItem(itemInfo1);
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);

    itemInfo1.name = "太虚剑意";
    itemInfo1.iconPath = "C:\\Users\\NoWat\\Project\\JX3DPS3\\resources\\pics\\JX3\\太虚剑意.png";
    
    comboBox1->AddItem(itemInfo1);
    comboBox2->AddItem(itemInfo1);
    comboBox3->AddItem(itemInfo1);
    
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);

    itemInfo1.name = "紫霞功";
    itemInfo1.iconPath = "C:\\Users\\NoWat\\Project\\JX3DPS3\\resources\\pics\\JX3\\紫霞功.png";
    itemInfo1.description = "C:\\Users\\NoWat\\Project\\JX3DPS3\\resources\\pics\\JX3\\紫霞功.png";
    
    comboBox5->AddItem(itemInfo1);
    comboBox4->AddItem(itemInfo1);
    comboBox5->AddItem(itemInfo1);
    comboBox1->AddItem(itemInfo1);
    comboBox2->AddItem(itemInfo1);
    comboBox3->AddItem(itemInfo1);
    

    layout->addWidget(comboBox1, 0, 0, 1, 1);
    layout->addWidget(comboBox2, 1, 0, 1, 1);
    layout->addWidget(comboBox3, 2, 0, 1, 1);
    layout->addWidget(comboBox4, 3, 0, 1, 1);
    layout->addWidget(comboBox5, 4, 0, 1, 1);
}

TestUI::~TestUI() { }
