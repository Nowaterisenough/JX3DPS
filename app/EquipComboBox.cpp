#include "EquipComboBox.h"

EquipComboBox::EquipComboBox(QWidget *parent) : QComboBox(parent)
{
    setStyleSheet("QComboBox::drop-down{""border-style: none;}");
}
