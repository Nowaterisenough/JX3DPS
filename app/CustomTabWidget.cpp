#include "CustomTabWidget.h"

CustomTabBar::CustomTabBar(QWidget *parent) : QTabBar(parent)
{
    connect(this, QOverload<int>::of(&QTabBar::currentChanged),
            this, QOverload<int>::of(&CustomTabBar::CheckBoxChange));
    connect(m_checkBox, QOverload<int>::of(&QCheckBox::stateChanged),
            this, QOverload<int>::of(&CustomTabBar::EquipEnable));
}

void CustomTabBar::CheckBoxChange(int index)
{
    if (index == 2) {
        m_checkBox->setGeometry(135, 5, 15, 15);
    } else {
        m_checkBox->setGeometry(135, 6, 15, 15);
    }
}

CustomTabWidget::CustomTabWidget(QWidget *parent) : QTabWidget(parent)
{
    this->setTabBar(m_tabBar);
    connect(m_tabBar, QOverload<int>::of(&CustomTabBar::EquipEnable),
            this, QOverload<int>::of(&CustomTabWidget::EquipEnable));
}

