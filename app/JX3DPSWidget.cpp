/**
 * Project: JX3DPS
 * File: JX3DPS::Simulator::Widget.cpp
 * Description:
 * Created Date: 2023-08-06 06:46:22
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-11 06:46:26
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "JX3DPSWidget.h"

#include <QLayout>

#include <spdlog/spdlog.h>

#include <Attribute/Attribute.hpp>

#include "Button/Button.h"
#include "CheckBox/CheckBox.h"
#include "ComboBox/ComboBox.h"
#include "DataBar/DataBar.h"
#include "GroupBox/GroupBox.h"
#include "LineEdit/LineEdit.h"
#include "PlainTextEdit/PlainTextEdit.h"
#include "SpinBox/SpinBox.h"
#include "Splitter/Splitter.h"
#include "StackWidget/StackWidget.h"

#include "JX3DPS.h"
#include "JX3DPSJsonParser.h"

const char *const CONFIG_PATH = "./config.json";

JX3DPS::Simulator::Widget::Widget(QWidget *parent)
{
    QString title = QString("%1  %3").arg(APP_NAME).arg(JX3DPSVersion());
    this->SetTitle(title);

    this->setFixedHeight(814);
    this->setMinimumWidth(1000);

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
    buttonSimulate->setFixedHeight(40);
    buttonSimulate->setFixedWidth(160);
    buttonSimulate->setFont(QFont(buttonSimulate->font().family(), 13));

    tabWidgetAttribute->AddTab("属性");
    tabWidgetAttribute->AddTab("配装");

    tabWidgetGains->AddTab("收益");

    tabWidgetTalentsRecipes->AddTab("奇穴");
    tabWidgetTalentsRecipes->AddTab("秘籍");

    tabWidgetSkills->SetAddButtonVisible(true);

    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(tabWidgetSkills);
    splitter->addWidget(tabWidgetEvents);

    QGridLayout *layout = new QGridLayout(this->centralWidget);
    layout->setSpacing(10);
    layout->addWidget(groupBoxSetting, 0, 0, 2, 1);
    layout->addWidget(tabWidgetAttribute, 2, 0, 2, 1);
    layout->addWidget(buttonSimulate, 0, 1, 1, 2, Qt::AlignCenter);
    layout->addWidget(groupBoxOut, 1, 1, 1, 2);
    layout->addWidget(tabWidgetGains, 2, 1, 1, 2);
    layout->addWidget(tabWidgetTalentsRecipes, 3, 1, 1, 3);
    layout->addWidget(groupBoxPermanents, 0, 3, 3, 1);
    layout->addWidget(splitter, 0, 4, 4, 1);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 0);
    layout->setColumnStretch(3, 0);
    layout->setColumnStretch(4, 1);

    connect(tabWidgetSkills, &TabWidget::Signal_AddTab, this, [=]() {
        QWidget *widget = tabWidgetSkills->Widget(tabWidgetSkills->Count() - 1);

        PlainTextEdit *plainTextEdit = new PlainTextEdit(widget);
        QGridLayout   *layout        = new QGridLayout(widget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(plainTextEdit);
    });

    connect(tabWidgetEvents, &TabWidget::Signal_AddTab, this, [=]() {
        QWidget *widget = tabWidgetEvents->Widget(tabWidgetEvents->Count() - 1);

        PlainTextEdit *plainTextEdit = new PlainTextEdit(widget);
        QGridLayout   *layout        = new QGridLayout(widget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(plainTextEdit);
    });

    InitWidgetSetting(groupBoxSetting);

    InitWidgetOut(groupBoxOut);

    InitWidgetAttribute(tabWidgetAttribute->Widget(0));

    InitWidgetGains(tabWidgetGains->Widget(0));

    InitWidgetTalents(tabWidgetTalentsRecipes->Widget(0));

    InitWidgetRecipes(tabWidgetTalentsRecipes->Widget(1));

    InitWidgetPermanents(groupBoxPermanents);

    InitWidgetSkills(tabWidgetSkills);

    InitWidgetEvents(tabWidgetEvents);
}

JX3DPS::Simulator::Widget::~Widget() { }

void JX3DPS::Simulator::Widget::InitWidgetSetting(QWidget *parent)
{
    ComboBox *comboBoxClass = new ComboBox(parent);
    comboBoxClass->SetType(ComboBox::Type::DETAILED);
    comboBoxClass->setFixedHeight(54);
    comboBoxClass->SetItemSize(204, 54);

    ComboBox::ItemInfo itemInfo;
    itemInfo.name = "心法";
    comboBoxClass->AddItem(itemInfo);

    TextButton *textButtonSimulateCount = new TextButton(parent);
    textButtonSimulateCount->setFixedSize(64, 28);
    textButtonSimulateCount->setText("模拟次数");

    LineEdit *lineEditSimulateCount = new LineEdit(parent);
    lineEditSimulateCount->setFixedSize(64, 28);
    lineEditSimulateCount->setText("1000");

    TextButton *textButtonDelay = new TextButton(parent);
    textButtonDelay->setFixedSize(64, 28);
    textButtonDelay->setText("延迟波动");

    LineEdit *lineEditDelayMin = new LineEdit(parent);
    lineEditDelayMin->setFixedSize(64, 28);
    lineEditDelayMin->setText("40");

    LineEdit *lineEditDelayMax = new LineEdit(parent);
    lineEditDelayMax->setFixedSize(64, 28);
    lineEditDelayMax->setText("75");

    JsonParser::LoadConfig(CONFIG_PATH, m_config);

    std::list<ComboBox::ItemInfo> itemInfos;
    JsonParser::ParseJsonToClassTypeItemInfos(m_config, itemInfos);

    for (const auto &itemInfo : itemInfos) {
        comboBoxClass->AddItem(itemInfo);
    }

    CheckBox *checkBoxDebug = new CheckBox(parent);
    checkBoxDebug->setFixedHeight(22);
    checkBoxDebug->setText("调试");

    QGridLayout *gLayout = new QGridLayout(parent);
    gLayout->addWidget(comboBoxClass, 0, 0, 1, 3);
    gLayout->addWidget(textButtonSimulateCount, 1, 0, 1, 1);
    gLayout->addWidget(lineEditSimulateCount, 1, 1, 1, 2);
    gLayout->addWidget(textButtonDelay, 2, 0, 1, 1);
    gLayout->addWidget(lineEditDelayMin, 2, 1, 1, 1);
    gLayout->addWidget(lineEditDelayMax, 2, 2, 1, 1);

    gLayout->addWidget(checkBoxDebug, 1, 2, 1, 1);

    connect(checkBoxDebug, &QCheckBox::stateChanged, [=](int checked) {
        if (checked) {
            spdlog::default_logger()->set_level(spdlog::level::debug);
            spdlog::flush_on(spdlog::level::debug);

            lineEditSimulateCount->setText("1");
            lineEditSimulateCount->setReadOnly(true);
        } else {
            spdlog::default_logger()->set_level(spdlog::level::info);
            spdlog::flush_on(spdlog::level::info);

            lineEditSimulateCount->setText("1000");
            lineEditSimulateCount->setReadOnly(false);
        }
    });

    connect(comboBoxClass, &ComboBox::Signal_CurrentItemChanged, [=](const ComboBox::ItemInfo &itemInfo) {
        if (itemInfo.name == "心法") {
            return;
        }
        JX3DPS::ClassType type = JX3DPS::GetClassType(itemInfo.name.toStdString());

        emit Signal_UpdateClassType(type);
    });
}

void JX3DPS::Simulator::Widget::InitWidgetOut(QWidget *parent)
{
    TextButton *textButtonDPS = new TextButton(parent);
    textButtonDPS->setText("DPS 期望");
    textButtonDPS->setFixedHeight(28);

    LineEdit *lineEditDPS = new LineEdit(parent);
    lineEditDPS->setFixedSize(75, 28);
    lineEditDPS->setReadOnly(true);

    QSpacerItem *spacerItem = new QSpacerItem(0, 2, QSizePolicy::Expanding, QSizePolicy::Fixed);

    Button *buttonStats = new Button(parent);
    buttonStats->setText("数据统计");
    buttonStats->setFixedHeight(36);
    buttonStats->setFont(QFont(buttonStats->font().family(), 11));

    QGridLayout *gLayout = new QGridLayout(parent);

    gLayout->addWidget(textButtonDPS, 0, 0, 1, 1);
    gLayout->addWidget(lineEditDPS, 0, 1, 1, 1);
    gLayout->addItem(spacerItem, 1, 0, 1, 2);
    gLayout->addWidget(buttonStats, 2, 0, 1, 2);
}

void JX3DPS::Simulator::Widget::InitWidgetAttribute(QWidget *parent)
{

    QList<JX3DPS::Attribute::Type> attributeTypes = {
        JX3DPS::Attribute::Type::DEFAULT,
        JX3DPS::Attribute::Type::ATTACK_POWER_BASE,
        JX3DPS::Attribute::Type::CRITICAL_STRIKE,
        JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER,
        JX3DPS::Attribute::Type::OVERCOME_BASE,
        JX3DPS::Attribute::Type::HASTE_BASE,
        JX3DPS::Attribute::Type::STRAIN_BASE,
        JX3DPS::Attribute::Type::SURPLUS_VALUE_BASE,
    };

    QList<QString> attributeNames = { "身法", "攻击", "会心", "会效",    "破防",
                                      "加速", "无双", "破招", "武器伤害" };

    QList<QString> types = { "内功", "外功" };

    QMap<JX3DPS::Attribute::Type, TextButton *> attributeTextButtons;
    QMap<JX3DPS::Attribute::Type, LineEdit *>   attributeLineEdits;
    QMap<JX3DPS::Attribute::Type, SpinBox *>    attributeSpinBoxes;

    JX3DPS::Attribute *attribute = new JX3DPS::Attribute;

    attribute->SetClassType(JX3DPS::ClassType::MO_WEN);

    QGridLayout *gLayout = new QGridLayout(parent);

    static const std::unordered_map<std::string_view, JX3DPS::Attribute::Type> &ATTRIBUTE_TYPE_HASH = {
        {{ "身法", JX3DPS::Attribute::Type::AGILITY_BASE },
         { "力道", JX3DPS::Attribute::Type::STRENGTH_BASE },
         { "根骨", JX3DPS::Attribute::Type::SPIRIT_BASE },
         { "元气", JX3DPS::Attribute::Type::SPUNK_BASE },
         { "攻击", JX3DPS::Attribute::Type::ATTACK_POWER_BASE },
         { "会心", JX3DPS::Attribute::Type::CRITICAL_STRIKE },
         { "会效", JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER },
         { "破防", JX3DPS::Attribute::Type::OVERCOME_BASE },
         { "加速", JX3DPS::Attribute::Type::HASTE_BASE },
         { "无双", JX3DPS::Attribute::Type::STRAIN_BASE },
         { "破招", JX3DPS::Attribute::Type::SURPLUS_VALUE_BASE },
         { "武器伤害", JX3DPS::Attribute::Type::WEAPON_DAMAGE_BASE },
         { "外功攻击", JX3DPS::Attribute::Type::PHYSICS_ATTACK_POWER_BASE },
         { "内功攻击", JX3DPS::Attribute::Type::MAGIC_ATTACK_POWER_BASE },
         { "外功会心", JX3DPS::Attribute::Type::PHYSICS_CRITICAL_STRIKE },
         { "内功会心", JX3DPS::Attribute::Type::MAGIC_CRITICAL_STRIKE },
         { "外功会效", JX3DPS::Attribute::Type::PHYSICS_CRITICAL_STRIKE_POWER },
         { "内功会效", JX3DPS::Attribute::Type::MAGIC_CRITICAL_STRIKE_POWER },
         { "外功破防", JX3DPS::Attribute::Type::PHYSICS_OVERCOME_BASE },
         { "内功破防", JX3DPS::Attribute::Type::MAGIC_OVERCOME_BASE }}
    };

    int index = 0;
    for (const auto &type : attributeTypes) {
        TextButton *textButton = new TextButton(parent);
        QString     str        = "";
        if (index >= 1 && index <= 4) {
            str = types[0];
            connect(textButton, &TextButton::clicked, [=]() {
                if (textButton->text() == types[0] + attributeNames[index]) {
                    textButton->setText(types[1] + attributeNames[index]);
                } else {
                    textButton->setText(types[0] + attributeNames[index]);
                }
                emit Signal_UpdateAttribute();
            });
        }

        textButton->setText(str + attributeNames[index]);
        textButton->setFixedSize(64, 28);
        LineEdit *lineEdit = new LineEdit(parent);
        lineEdit->setFixedSize(64, 28);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setReadOnly(true);
        SpinBox *spinBox = new SpinBox(parent);
        spinBox->setFixedSize(64, 28);

        gLayout->addWidget(textButton, index, 0, 1, 1);
        gLayout->addWidget(lineEdit, index, 1, 1, 1);
        gLayout->addWidget(spinBox, index, 2, 1, 1);

        attributeTextButtons.insert(type, textButton);
        attributeLineEdits.insert(type, lineEdit);
        attributeSpinBoxes.insert(type, spinBox);

        connect(spinBox, &SpinBox::Signal_UpdateValue, this, [=](int value) {
            JX3DPS::Attribute::Type t = ATTRIBUTE_TYPE_HASH.at(textButton->text().toStdString());
            attribute->SetAttributeInitial(t, value);
            emit Signal_UpdateAttribute();
        });

        index++;
    }

    TextButton *textButtonWeapon = new TextButton(parent);
    textButtonWeapon->setText(attributeNames[index]);
    textButtonWeapon->setFixedSize(64, 28);
    SpinBox *spinBoxWeaponMin = new SpinBox(parent);
    spinBoxWeaponMin->setFixedSize(64, 28);
    SpinBox *spinBoxWeaponMax = new SpinBox(parent);
    spinBoxWeaponMax->setFixedSize(64, 28);

    attributeTextButtons.insert(JX3DPS::Attribute::Type::WEAPON_DAMAGE_BASE, textButtonWeapon);
    attributeSpinBoxes.insert(JX3DPS::Attribute::Type::WEAPON_DAMAGE_BASE, spinBoxWeaponMin);
    attributeSpinBoxes.insert(JX3DPS::Attribute::Type::WEAPON_DAMAGE_RAND, spinBoxWeaponMax);

    connect(spinBoxWeaponMin, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        JX3DPS::Attribute::Type t = JX3DPS::Attribute::Type::WEAPON_DAMAGE_BASE;
        attribute->SetAttributeInitial(t, value);
        emit Signal_UpdateAttribute();
    });

    connect(spinBoxWeaponMax, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        JX3DPS::Attribute::Type t = JX3DPS::Attribute::Type::WEAPON_DAMAGE_RAND;
        attribute->SetAttributeInitial(t, value - attribute->GetWeaponDamageBase());
        emit Signal_UpdateAttribute();
    });

    connect(this, &JX3DPS::Simulator::Widget::Signal_UpdateAttribute, this, [=] {
        if (attributeTextButtons[JX3DPS::Attribute::Type::DEFAULT]->text() == "身法") {
            attributeLineEdits[JX3DPS::Attribute::Type::DEFAULT]->UpdateValue(attribute->GetAgility());
            attributeSpinBoxes[JX3DPS::Attribute::Type::DEFAULT]->UpdateValue(attribute->GetAgility());
            attributeSpinBoxes[JX3DPS::Attribute::Type::DEFAULT]->setRange(attribute->GetAgilityBaseByClass());
        } else if (attributeTextButtons[JX3DPS::Attribute::Type::DEFAULT]->text() ==
                   "力道")
        {
            attributeLineEdits[JX3DPS::Attribute::Type::DEFAULT]->UpdateValue(attribute->GetStrength());
            attributeSpinBoxes[JX3DPS::Attribute::Type::DEFAULT]->UpdateValue(attribute->GetStrength());
            attributeSpinBoxes[JX3DPS::Attribute::Type::DEFAULT]->setRange(attribute->GetStrengthBaseByClass());
        } else if (attributeTextButtons[JX3DPS::Attribute::Type::DEFAULT]->text() ==
                   "根骨")
        {
            attributeLineEdits[JX3DPS::Attribute::Type::DEFAULT]->UpdateValue(attribute->GetSpirit());
            attributeSpinBoxes[JX3DPS::Attribute::Type::DEFAULT]->UpdateValue(attribute->GetSpirit());
            attributeSpinBoxes[JX3DPS::Attribute::Type::DEFAULT]->setRange(attribute->GetSpiritBaseByClass());
        } else {
            attributeLineEdits[JX3DPS::Attribute::Type::DEFAULT]->UpdateValue(attribute->GetSpunk());
            attributeSpinBoxes[JX3DPS::Attribute::Type::DEFAULT]->UpdateValue(attribute->GetSpunk());
            attributeSpinBoxes[JX3DPS::Attribute::Type::DEFAULT]->setRange(attribute->GetSpunkBaseByClass());
        }

        if (attributeTextButtons[JX3DPS::Attribute::Type::ATTACK_POWER_BASE]->text().contains(types[1]))
        {
            attributeLineEdits[JX3DPS::Attribute::Type::ATTACK_POWER_BASE]->UpdateValue(
                attribute->GetPhysicsAttackPower());
            attributeSpinBoxes[JX3DPS::Attribute::Type::ATTACK_POWER_BASE]->UpdateValue(
                attribute->GetPhysicsAttackPowerBase());
            attributeSpinBoxes[JX3DPS::Attribute::Type::ATTACK_POWER_BASE]->setRange(
                attribute->GetPhysicsAttackPowerBaseByClass());
        } else {
            attributeLineEdits[JX3DPS::Attribute::Type::ATTACK_POWER_BASE]->UpdateValue(
                attribute->GetMagicAttackPower());
            attributeSpinBoxes[JX3DPS::Attribute::Type::ATTACK_POWER_BASE]->UpdateValue(
                attribute->GetMagicAttackPowerBase());
            attributeSpinBoxes[JX3DPS::Attribute::Type::ATTACK_POWER_BASE]->setRange(
                attribute->GetMagicAttackPowerBaseByClass());
        }

        if (attributeTextButtons[JX3DPS::Attribute::Type::CRITICAL_STRIKE]->text().contains(types[1]))
        {
            attributeLineEdits[JX3DPS::Attribute::Type::CRITICAL_STRIKE]->UpdateValueFloat(
                attribute->GetPhysicsCriticalStrikePercent());
            attributeSpinBoxes[JX3DPS::Attribute::Type::CRITICAL_STRIKE]->UpdateValue(
                attribute->GetPhysicsCriticalStrike());
            attributeSpinBoxes[JX3DPS::Attribute::Type::CRITICAL_STRIKE]->setRange(
                attribute->GetPhysicsCriticalStrikeMinimum());
        } else {
            attributeLineEdits[JX3DPS::Attribute::Type::CRITICAL_STRIKE]->UpdateValueFloat(
                attribute->GetMagicCriticalStrikePercent());
            attributeSpinBoxes[JX3DPS::Attribute::Type::CRITICAL_STRIKE]->UpdateValue(
                attribute->GetMagicCriticalStrike());
            attributeSpinBoxes[JX3DPS::Attribute::Type::CRITICAL_STRIKE]->setRange(
                attribute->GetMagicCriticalStrikeMinimum());
        }

        if (attributeTextButtons[JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER]->text().contains(types[1]))
        {
            attributeLineEdits[JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER]->UpdateValueFloat(
                attribute->GetPhysicsCriticalStrikePowerPercent());
            attributeSpinBoxes[JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER]->UpdateValue(
                attribute->GetPhysicsCriticalStrikePower());
        } else {
            attributeLineEdits[JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER]->UpdateValueFloat(
                attribute->GetMagicCriticalStrikePowerPercent());
            attributeSpinBoxes[JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER]->UpdateValue(
                attribute->GetMagicCriticalStrikePower());
        }

        if (attributeTextButtons[JX3DPS::Attribute::Type::OVERCOME_BASE]->text().contains(types[1]))
        {
            attributeLineEdits[JX3DPS::Attribute::Type::OVERCOME_BASE]->UpdateValueFloat(
                attribute->GetPhysicsOvercomePercent());
            attributeSpinBoxes[JX3DPS::Attribute::Type::OVERCOME_BASE]->UpdateValue(
                attribute->GetPhysicsOvercomeBase());
            attributeSpinBoxes[JX3DPS::Attribute::Type::OVERCOME_BASE]->setRange(
                attribute->GetPhysicsOvercomeBaseMinimum());
        } else {
            attributeLineEdits[JX3DPS::Attribute::Type::OVERCOME_BASE]->UpdateValueFloat(
                attribute->GetMagicOvercomePercent());
            attributeSpinBoxes[JX3DPS::Attribute::Type::OVERCOME_BASE]->UpdateValue(
                attribute->GetMagicOvercomeBase());
            attributeSpinBoxes[JX3DPS::Attribute::Type::OVERCOME_BASE]->setRange(attribute->GetMagicOvercomeBaseMinimum());
        }

        attributeLineEdits[JX3DPS::Attribute::Type::HASTE_BASE]->UpdateValueFloat(
            attribute->GetHasteVisiblePercent());
        attributeSpinBoxes[JX3DPS::Attribute::Type::HASTE_BASE]->UpdateValue(attribute->GetHasteBase());

        attributeLineEdits[JX3DPS::Attribute::Type::STRAIN_BASE]->UpdateValueFloat(
            attribute->GetStrainPercent());
        attributeSpinBoxes[JX3DPS::Attribute::Type::STRAIN_BASE]->UpdateValue(attribute->GetStrainBase());

        attributeLineEdits[JX3DPS::Attribute::Type::SURPLUS_VALUE_BASE]->UpdateValue(
            attribute->GetSurplusValueBase());
        attributeSpinBoxes[JX3DPS::Attribute::Type::SURPLUS_VALUE_BASE]->UpdateValue(
            attribute->GetSurplusValueBase());

        attributeSpinBoxes[JX3DPS::Attribute::Type::WEAPON_DAMAGE_BASE]->UpdateValue(
            attribute->GetWeaponDamageBase());
        attributeSpinBoxes[JX3DPS::Attribute::Type::WEAPON_DAMAGE_RAND]->UpdateValue(
            attribute->GetWeaponDamageBase() + attribute->GetWeaponDamageRand());
    });

    emit Signal_UpdateAttribute();

    connect(this, &JX3DPS::Simulator::Widget::Signal_UpdateClassType, [=](JX3DPS::ClassType type) {
        for (int index = 1; index <= 4; ++index) {
            attributeTextButtons[attributeTypes[index]]->setText(types[static_cast<int>(type) % 2] +
                                                                 attributeNames[index]);
        }

        if (JX3DPS::Attribute::MAJOR[static_cast<int>(type)][static_cast<int>(JX3DPS::Attribute::MajorType::AGILITY)])
        {
            attributeTextButtons[JX3DPS::Attribute::Type::DEFAULT]->setText("身法");
        } else if (JX3DPS::Attribute::MAJOR[static_cast<int>(type)][static_cast<int>(JX3DPS::Attribute::MajorType::STRENGTH)])
        {
            attributeTextButtons[JX3DPS::Attribute::Type::DEFAULT]->setText("力道");
        } else if (JX3DPS::Attribute::MAJOR[static_cast<int>(type)][static_cast<int>(JX3DPS::Attribute::MajorType::SPIRIT)])
        {
            attributeTextButtons[JX3DPS::Attribute::Type::DEFAULT]->setText("根骨");
        } else {
            attributeTextButtons[JX3DPS::Attribute::Type::DEFAULT]->setText("元气");
        }

        *attribute = JX3DPS::Attribute();
        attribute->SetClassType(type);

        std::unordered_map<std::string, int> attributes;
        JsonParser::ParseJsonToDefaultAttribute(m_config, type, attributes);

        for (auto &[attributeName, value] : attributes) {
            Attribute::Type type = Attribute::AttributeType(attributeName);

            attribute->SetAttributeInitial(type, value);
        }

        emit Signal_UpdateAttribute();
    });

    gLayout->addWidget(textButtonWeapon, index, 0, 1, 1);
    gLayout->addWidget(spinBoxWeaponMin, index, 1, 1, 1);
    gLayout->addWidget(spinBoxWeaponMax, index, 2, 1, 1);

    QSpacerItem *spacerItem =
        new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    gLayout->addItem(spacerItem, ++index, 0, 1, 3);

    GroupBox *groupBoxEquipEffect = new GroupBox("装备效果", parent);

    InitWidgetEquipEffects(groupBoxEquipEffect);

    gLayout->addWidget(groupBoxEquipEffect, ++index, 0, 1, 3);
    gLayout->setRowStretch(index, 0);

    QSpacerItem *spacerItem2 =
        new QSpacerItem(0, 5, QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    gLayout->addItem(spacerItem2, ++index, 0, 1, 3);

    Button *buttonImport = new Button(parent);
    buttonImport->setFixedHeight(35);
    buttonImport->setText("导入魔盒属性");
    buttonImport->setFont(QFont(buttonImport->font().family(), 11));

    gLayout->addWidget(buttonImport, ++index, 0, 1, 3);
}

void JX3DPS::Simulator::Widget::InitWidgetEquipEffects(QWidget *parent)
{
    QGridLayout *gLayout = new QGridLayout(parent);

    std::vector<CheckBox *> setEffectWidgets;

    CheckBox *checkBoxEnchantWrist  = new CheckBox(parent);
    CheckBox *checkBoxEnchantShoes  = new CheckBox(parent);
    CheckBox *checkBoxEnchantBelt   = new CheckBox(parent);
    CheckBox *checkBoxEnchantJacket = new CheckBox(parent);
    CheckBox *checkBoxEnchantHat    = new CheckBox(parent);

    CheckBox *checkBoxClassSetBuff  = new CheckBox(parent);
    CheckBox *checkBoxClassSetSkill = new CheckBox(parent);

    ComboBox *comboBoxWeapon = new ComboBox(parent);

    comboBoxWeapon->setFixedHeight(36);
    comboBoxWeapon->SetItemSize(184, 36);
    comboBoxWeapon->SetType(ComboBox::Type::DETAILED);

    ComboBox::ItemInfo itemInfo;
    itemInfo.name = "武器效果";
    comboBoxWeapon->AddItem(itemInfo);
    itemInfo.name = "武器效果·橙武";
    comboBoxWeapon->AddItem(itemInfo);
    itemInfo.name = "武器效果·水特效";
    comboBoxWeapon->AddItem(itemInfo);

    setEffectWidgets.push_back(checkBoxEnchantWrist);
    setEffectWidgets.push_back(checkBoxEnchantShoes);
    setEffectWidgets.push_back(checkBoxEnchantBelt);
    setEffectWidgets.push_back(checkBoxEnchantJacket);
    setEffectWidgets.push_back(checkBoxEnchantHat);

    setEffectWidgets.push_back(checkBoxClassSetBuff);
    setEffectWidgets.push_back(checkBoxClassSetSkill);

    checkBoxEnchantWrist->setText("大附魔·腕");
    checkBoxEnchantShoes->setText("大附魔·鞋");
    checkBoxEnchantBelt->setText("大附魔·腰");
    checkBoxEnchantJacket->setText("大附魔·衣");
    checkBoxEnchantHat->setText("大附魔·帽");
    checkBoxClassSetBuff->setText("套装·属性");
    checkBoxClassSetSkill->setText("套装·技能");

    checkBoxEnchantWrist->setFixedHeight(22);
    checkBoxEnchantShoes->setFixedHeight(22);
    checkBoxEnchantBelt->setFixedHeight(22);
    checkBoxEnchantJacket->setFixedHeight(22);
    checkBoxEnchantHat->setFixedHeight(22);

    checkBoxClassSetBuff->setFixedHeight(22);
    checkBoxClassSetSkill->setFixedHeight(22);

    gLayout->addWidget(checkBoxEnchantWrist, 0, 0, 1, 1);
    gLayout->addWidget(checkBoxEnchantShoes, 1, 0, 1, 1);
    gLayout->addWidget(checkBoxEnchantBelt, 2, 0, 1, 1);
    gLayout->addWidget(checkBoxEnchantJacket, 0, 1, 1, 1);
    gLayout->addWidget(checkBoxEnchantHat, 1, 1, 1, 1);

    gLayout->addWidget(checkBoxClassSetBuff, 2, 1, 1, 1);
    gLayout->addWidget(checkBoxClassSetSkill, 3, 1, 1, 1);
    gLayout->addWidget(comboBoxWeapon, 4, 0, 1, 2);
}

void JX3DPS::Simulator::Widget::InitWidgetGains(QWidget *parent)
{
    QList<JX3DPS::Attribute::Type> attributeTypes = {
        JX3DPS::Attribute::Type::AGILITY_BASE,
        JX3DPS::Attribute::Type::ATTACK_POWER_BASE,
        JX3DPS::Attribute::Type::CRITICAL_STRIKE,
        JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER,
        JX3DPS::Attribute::Type::OVERCOME_BASE,
        JX3DPS::Attribute::Type::HASTE_BASE,
        JX3DPS::Attribute::Type::STRAIN_BASE,
        JX3DPS::Attribute::Type::SURPLUS_VALUE_BASE,
        JX3DPS::Attribute::Type::WEAPON_DAMAGE_BASE,
    };

    QMap<JX3DPS::Attribute::Type, DataBar *> attributeDataBars;

    QGridLayout *gLayout = new QGridLayout(parent);

    int index = 0;
    for (const auto &type : attributeTypes) {
        DataBar *dataBar = new DataBar(parent);
        dataBar->setFixedHeight(28);

        gLayout->addWidget(dataBar, index, 0, 1, 1);

        attributeDataBars.insert(type, dataBar);

        index++;
    }

    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    gLayout->addItem(spacerItem, index, 0, 1, 1);
}

void JX3DPS::Simulator::Widget::InitWidgetTalents(QWidget *parent)
{
    std::vector<ComboBox *> talentsComboBoxes;
    QGridLayout            *gLayout = new QGridLayout(parent);
    QVector<QString>        nums    = { "一", "二", "三", "四", "五",   "六",
                                        "七", "八", "九", "十", "十一", "十二" };
    for (int i = 0; i < 12; ++i) {
        ComboBox *comboBox = new ComboBox(parent);
        comboBox->SetType(ComboBox::Type::ICON_NAME);
        comboBox->setFixedSize(54, 78);
        comboBox->SetItemSize(200, 54);

        ComboBox::ItemInfo itemInfo;
        itemInfo.name = QString("第%1重").arg(nums[i]);
        comboBox->AddItem(itemInfo);

        gLayout->addWidget(comboBox, i / 6, i % 6, 1, 1);

        talentsComboBoxes.push_back(comboBox);
    }

    connect(this, &JX3DPS::Simulator::Widget::Signal_UpdateClassType, [=](JX3DPS::ClassType type) {
        std::vector<std::list<ComboBox::ItemInfo>> talents;
        std::list<std::string>                     defaults;
        JsonParser::ParseJsonToTalentItemInfos(m_config, type, talents, defaults);
        for (int i = 0; i < 12; ++i) {
            talentsComboBoxes[i]->Clear();
            ComboBox::ItemInfo itemInfo;
            itemInfo.name = QString("第%1重").arg(nums[i]);
            talentsComboBoxes[i]->AddItem(itemInfo);

            for (const auto &itemInfo : talents[i]) {
                talentsComboBoxes[i]->AddItem(itemInfo);

                for (auto &defaultName : defaults) {
                    if (defaultName == itemInfo.name.toStdString()) {
                        talentsComboBoxes[i]->SetView(itemInfo);
                    }
                }
            }
        }
    });
}

void JX3DPS::Simulator::Widget::InitWidgetRecipes(QWidget *parent)
{
    QGridLayout *gLayout = new QGridLayout(parent);

    StackWidget *stackWidget = new StackWidget(parent);

    gLayout->addWidget(stackWidget, 0, 0, 1, 1);

    connect(this, &JX3DPS::Simulator::Widget::Signal_UpdateClassType, [=](JX3DPS::ClassType type) {
        stackWidget->Clear();

        std::unordered_map<std::string, std::list<CheckBox::ItemInfo>> recipes;
        std::list<std::string>                                         defaults;
        JsonParser::ParseJsonToRecipeItemInfos(m_config, type, recipes, defaults);

        int index = 0;
        for (const auto &[name, itemInfos] : recipes) {
            stackWidget->AddTab(name.c_str());
            QGridLayout *layout = new QGridLayout(stackWidget->Widget(index));
            int          idx    = 0;
            for (const auto &itemInfo : itemInfos) {
                CheckBoxIcon *checkBoxIcon = new CheckBoxIcon(parent);
                checkBoxIcon->SetItemInfo(itemInfo);
                checkBoxIcon->setFixedSize(42, 42);
                layout->addWidget(checkBoxIcon, idx / 4, idx % 4, 1, 1);

                for (const auto &defaultName : defaults) {
                    if (defaultName == itemInfo.name.toStdString()) {
                        checkBoxIcon->setChecked(true);
                    }
                }
                
                idx++;
            }
            index++;
        }
    });
}

void JX3DPS::Simulator::Widget::InitWidgetPermanents(QWidget *parent)
{
    QGridLayout *gLayout = new QGridLayout(parent);

    std::unordered_map<std::string, ComboBox *> permanentComboBoxes;
    std::vector<std::string> permanents = { "阵眼",     "食品增强", "食品辅助",
                                            "药品增强", "药品辅助", "家园炊事",
                                            "家园酿造", "武器磨石" };

    std::vector<std::string> permanentTexts = {
        "TeamCore",   "FoodEnhance", "FoodSupport", "MedEnhance",
        "MedSupport", "HomeCook",    "HomeWine",    "WeaponWhetstone"
    };

    for (int i = 0; i < permanents.size(); ++i) {
        ComboBox *comboBox = new ComboBox(parent);
        comboBox->SetType(ComboBox::Type::DETAILED);
        comboBox->setFixedSize(200, 48);
        comboBox->SetItemSize(200, 48);
        gLayout->addWidget(comboBox, i, 0, 1, 2);

        ComboBox::ItemInfo itemInfo;
        itemInfo.name = permanents[i].c_str();
        comboBox->AddItem(itemInfo);

        permanentComboBoxes.emplace(permanentTexts[i], comboBox);
    }

    std::vector<CheckBox *> permanentCheckBoxes;

    permanentCheckBoxes.push_back(new CheckBox(parent));
    permanentCheckBoxes.push_back(new CheckBox(parent));
    permanentCheckBoxes.push_back(new CheckBox(parent));
    permanentCheckBoxes.push_back(new CheckBox(parent));

    permanentCheckBoxes[0]->setText("玉笛谁家听落梅");
    permanentCheckBoxes[1]->setText("同泽宴");
    permanentCheckBoxes[2]->setText("炼狱水煮鱼");
    permanentCheckBoxes[3]->setText("蒸鱼菜盘");

    permanentCheckBoxes[0]->setFixedHeight(22);
    permanentCheckBoxes[1]->setFixedHeight(22);
    permanentCheckBoxes[2]->setFixedHeight(22);
    permanentCheckBoxes[3]->setFixedHeight(22);

    gLayout->addWidget(permanentCheckBoxes[0], 8, 0, 1, 1);
    gLayout->addWidget(permanentCheckBoxes[1], 8, 1, 1, 1);
    gLayout->addWidget(permanentCheckBoxes[2], 9, 0, 1, 1);
    gLayout->addWidget(permanentCheckBoxes[3], 9, 1, 1, 1);

    connect(this, &JX3DPS::Simulator::Widget::Signal_UpdateClassType, [=](JX3DPS::ClassType type) {
        std::unordered_map<std::string, CheckBox::ItemInfo>            permanents1;
        std::unordered_map<std::string, std::list<ComboBox::ItemInfo>> permanents2;
        JsonParser::ParseJsonToPermanents(m_config, type, permanents1, permanents2);

        for (auto &[name, itemInfos] : permanents2) {
            permanentComboBoxes.at(name)->Clear();
            for (int i = 0; i < permanents.size(); ++i) {
                if (name == permanentTexts[i]) {
                    ComboBox::ItemInfo info;
                    info.name = permanents[i].c_str();
                    permanentComboBoxes.at(name)->AddItem(info);
                }
            }

            for (const auto &itemInfo : itemInfos) {
                permanentComboBoxes.at(name)->AddItem(itemInfo);
            }
        }

        permanentCheckBoxes[0]->SetItemInfo(permanents1["玉笛谁家听落梅"]);
        permanentCheckBoxes[1]->SetItemInfo(permanents1["同泽宴"]);
        permanentCheckBoxes[2]->SetItemInfo(permanents1["炼狱水煮鱼"]);
        permanentCheckBoxes[3]->SetItemInfo(permanents1["蒸鱼菜盘"]);
    });
}

void JX3DPS::Simulator::Widget::InitWidgetSkills(TabWidget *parent)
{
    parent->AddTab("宏");
    connect(this, &JX3DPS::Simulator::Widget::Signal_UpdateClassType, [=](JX3DPS::ClassType type) {
        parent->Clear();
        std::list<std::pair<std::string, std::list<std::string>>> skills;
        JsonParser::ParseJsonToSkills(m_config, type, skills);
        int index = 0;
        for (auto &[name, exprs] : skills) {
            parent->AddTab(name.c_str());
            PlainTextEdit *text = parent->Widget(index)->findChild<PlainTextEdit *>();
            for (const auto &expr : exprs) {
                text->appendHtml(expr.c_str());
            }
            index++;
        }
    });
}

void JX3DPS::Simulator::Widget::InitWidgetEvents(TabWidget *parent)
{
    parent->AddTab("事件");
    connect(this, &JX3DPS::Simulator::Widget::Signal_UpdateClassType, [=](JX3DPS::ClassType type) {
        std::list<std::string> events;
        JsonParser::ParseJsonToEvents(m_config, type, events);
        PlainTextEdit *text = parent->Widget(0)->findChild<PlainTextEdit *>();
        text->clear();
        for (auto &expr : events) {
            text->appendHtml(expr.c_str());
        }
    });
}
