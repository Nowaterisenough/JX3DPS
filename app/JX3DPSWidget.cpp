/**
 * Project: JX3DPS
 * File: JX3DPSWidget.cpp
 * Description:
 * Created Date: 2023-08-06 06:46:22
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-08 07:10:19
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
#include "TabWidget/TabWidget.h"

#include "JX3DPS.h"

JX3DPSWidget::JX3DPSWidget(QWidget *parent)
{
    QString title = QString("%1  %3").arg(APP_NAME).arg(JX3DPSVersion());
    this->SetTitle(title);

    this->setFixedHeight(848);
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
    buttonSimulate->setFixedHeight(52);
    buttonSimulate->setFont(QFont(buttonSimulate->font().family(), 14));

    tabWidgetAttribute->AddTab("属性");
    tabWidgetAttribute->AddTab("配装");

    tabWidgetGains->AddTab("收益");
    tabWidgetGains->setFixedHeight(428);

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
    layout->addWidget(buttonSimulate, 0, 1, 1, 2);
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

    tabWidgetSkills->AddTab("宏");
    tabWidgetEvents->AddTab("事件");

    InitWidgetSetting(groupBoxSetting);

    InitWidgetOut(groupBoxOut);

    InitWidgetAttribute(tabWidgetAttribute->Widget(0));

    InitWidgetGains(tabWidgetGains->Widget(0));

    InitWidgetTalents(tabWidgetTalentsRecipes->Widget(0));

    InitWidgetPermanents(groupBoxPermanents);
}

JX3DPSWidget::~JX3DPSWidget() { }

void JX3DPSWidget::InitWidgetSetting(QWidget *parent)
{
    TextButton *textButtonSimulateCount = new TextButton(parent);
    textButtonSimulateCount->setFixedSize(64, 26);
    textButtonSimulateCount->setText("模拟次数");

    LineEdit *lineEditSimulateCount = new LineEdit(parent);
    lineEditSimulateCount->setFixedSize(64, 26);
    lineEditSimulateCount->setText("1000");

    TextButton *textButtonDelay = new TextButton(parent);
    textButtonDelay->setFixedSize(64, 26);
    textButtonDelay->setText("延迟波动");

    LineEdit *lineEditDelayMin = new LineEdit(parent);
    lineEditDelayMin->setFixedSize(64, 26);
    lineEditDelayMin->setText("40");

    LineEdit *lineEditDelayMax = new LineEdit(parent);
    lineEditDelayMax->setFixedSize(64, 26);
    lineEditDelayMax->setText("75");

    ComboBox *comboBoxClass = new ComboBox(ComboBoxType::ICON_MODE, parent);
    comboBoxClass->setFixedSize(54, 54);

    CheckBox *checkBoxDebug = new CheckBox(parent);
    checkBoxDebug->setFixedHeight(22);
    checkBoxDebug->setText("调试");

    QGridLayout *gLayout = new QGridLayout(parent);
    gLayout->addWidget(textButtonSimulateCount, 0, 0, 1, 1);
    gLayout->addWidget(lineEditSimulateCount, 0, 1, 1, 1);
    gLayout->addWidget(textButtonDelay, 1, 0, 1, 1);
    gLayout->addWidget(lineEditDelayMin, 1, 1, 1, 1);
    gLayout->addWidget(lineEditDelayMax, 2, 1, 1, 1);
    gLayout->addWidget(comboBoxClass, 0, 2, 2, 1);
    gLayout->addWidget(checkBoxDebug, 2, 2, 1, 1);

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
}

void JX3DPSWidget::InitWidgetOut(QWidget *parent)
{
    TextButton *textButtonDPS = new TextButton(parent);
    textButtonDPS->setText("DPS");
    textButtonDPS->setFixedSize(25, 26);

    LineEdit *lineEditDPS = new LineEdit(parent);
    lineEditDPS->setFixedSize(66, 26);

    Button *buttonStats = new Button(parent);
    buttonStats->setText("统计");
    buttonStats->setFixedSize(60, 26);
    buttonStats->setFont(QFont(buttonStats->font().family(), 10.5));

    QGridLayout *gLayout = new QGridLayout(parent);

    gLayout->addWidget(textButtonDPS, 0, 0, 1, 1);
    gLayout->addWidget(lineEditDPS, 0, 1, 1, 1);
    gLayout->addWidget(buttonStats, 0, 2, 1, 1);
}

void JX3DPSWidget::InitWidgetAttribute(QWidget *parent)
{
    QList<JX3DPS::Attribute::Type> attributeTypes = {
        JX3DPS::Attribute::Type::AGILITY_BASE,
        JX3DPS::Attribute::Type::STRENGTH_BASE,
        JX3DPS::Attribute::Type::SPIRIT_BASE,
        JX3DPS::Attribute::Type::SPUNK_BASE,
        JX3DPS::Attribute::Type::ATTACK_POWER_BASE,
        JX3DPS::Attribute::Type::CRITICAL_STRIKE,
        JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER,
        JX3DPS::Attribute::Type::OVERCOME_BASE,
        JX3DPS::Attribute::Type::HASTE_BASE,
        JX3DPS::Attribute::Type::STRAIN_BASE,
        JX3DPS::Attribute::Type::SURPLUS_VALUE_BASE,
    };

    QList<QString> attributeNames = { "身法", "力道", "根骨", "元气", "攻击", "会心",
                                      "会效", "破防", "加速", "无双", "破招", "武器伤害" };

    QList<QString> types = { "外功", "内功" };

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
         { "外功攻击", JX3DPS::Attribute::Type::PHYSICS_ATTACK_POWER_BASE },
         { "内功攻击", JX3DPS::Attribute::Type::MAGIC_ATTACK_POWER_BASE },
         { "外功会心", JX3DPS::Attribute::Type::PHYSICS_CRITICAL_STRIKE },
         { "内功会心", JX3DPS::Attribute::Type::MAGIC_CRITICAL_STRIKE },
         { "外功会效", JX3DPS::Attribute::Type::PHYSICS_CRITICAL_STRIKE_POWER },
         { "内功会效", JX3DPS::Attribute::Type::MAGIC_CRITICAL_STRIKE_POWER },
         { "外功破防", JX3DPS::Attribute::Type::PHYSICS_OVERCOME_BASE },
         { "内功破防", JX3DPS::Attribute::Type::MAGIC_OVERCOME_BASE },
         { "加速", JX3DPS::Attribute::Type::HASTE_BASE },
         { "无双", JX3DPS::Attribute::Type::STRAIN_BASE },
         { "破招", JX3DPS::Attribute::Type::SURPLUS_VALUE_BASE },
         { "武器伤害", JX3DPS::Attribute::Type::WEAPON_DAMAGE_BASE }}
    };

    int index = 0;
    for (const auto &type : attributeTypes) {
        TextButton *textButton = new TextButton(parent);
        QString     str        = "";
        if (index >= 4 && index <= 7) {
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
        textButton->setFixedSize(64, 26);
        LineEdit *lineEdit = new LineEdit(parent);
        lineEdit->setFixedSize(64, 26);
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setReadOnly(true);
        SpinBox *spinBox = new SpinBox(parent);
        spinBox->setFixedSize(64, 26);

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
    textButtonWeapon->setFixedSize(64, 26);
    SpinBox *spinBoxWeaponMin = new SpinBox(parent);
    spinBoxWeaponMin->setFixedSize(64, 26);
    SpinBox *spinBoxWeaponMax = new SpinBox(parent);
    spinBoxWeaponMax->setFixedSize(64, 26);

    connect(this, &JX3DPSWidget::Signal_UpdateAttribute, this, [=] {
        attributeLineEdits[JX3DPS::Attribute::Type::AGILITY_BASE]->UpdateValue(attribute->GetAgility());
        attributeSpinBoxes[JX3DPS::Attribute::Type::AGILITY_BASE]->UpdateValue(attribute->GetAgility());
        attributeSpinBoxes[JX3DPS::Attribute::Type::AGILITY_BASE]->setRange(attribute->GetAgilityBaseByClass());

        attributeLineEdits[JX3DPS::Attribute::Type::STRENGTH_BASE]->UpdateValue(attribute->GetStrength());
        attributeSpinBoxes[JX3DPS::Attribute::Type::STRENGTH_BASE]->UpdateValue(attribute->GetStrength());
        attributeSpinBoxes[JX3DPS::Attribute::Type::STRENGTH_BASE]->setRange(attribute->GetStrengthBaseByClass());

        attributeLineEdits[JX3DPS::Attribute::Type::SPIRIT_BASE]->UpdateValue(attribute->GetSpirit());
        attributeSpinBoxes[JX3DPS::Attribute::Type::SPIRIT_BASE]->UpdateValue(attribute->GetSpirit());
        attributeSpinBoxes[JX3DPS::Attribute::Type::SPIRIT_BASE]->setRange(attribute->GetSpiritBaseByClass());

        attributeLineEdits[JX3DPS::Attribute::Type::SPUNK_BASE]->UpdateValue(attribute->GetSpunk());
        attributeSpinBoxes[JX3DPS::Attribute::Type::SPUNK_BASE]->UpdateValue(attribute->GetSpunk());
        attributeSpinBoxes[JX3DPS::Attribute::Type::SPUNK_BASE]->setRange(attribute->GetSpunkBaseByClass());

        if (attributeTextButtons[JX3DPS::Attribute::Type::ATTACK_POWER_BASE]->text().contains(types[0]))
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

        if (attributeTextButtons[JX3DPS::Attribute::Type::CRITICAL_STRIKE]->text().contains(types[0]))
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

        if (attributeTextButtons[JX3DPS::Attribute::Type::CRITICAL_STRIKE_POWER]->text().contains(types[0]))
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

        if (attributeTextButtons[JX3DPS::Attribute::Type::OVERCOME_BASE]->text().contains(types[0]))
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

void JX3DPSWidget::InitWidgetEquipEffects(QWidget *parent)
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
    gLayout->addWidget(checkBoxEnchantJacket, 3, 0, 1, 1);
    gLayout->addWidget(checkBoxEnchantHat, 4, 0, 1, 1);

    gLayout->addWidget(checkBoxClassSetBuff, 0, 1, 1, 1);
    gLayout->addWidget(checkBoxClassSetSkill, 1, 1, 1, 1);
}

void JX3DPSWidget::InitWidgetGains(QWidget *parent)
{
    QList<JX3DPS::Attribute::Type> attributeTypes = {
        JX3DPS::Attribute::Type::AGILITY_BASE,
        JX3DPS::Attribute::Type::STRENGTH_BASE,
        JX3DPS::Attribute::Type::SPIRIT_BASE,
        JX3DPS::Attribute::Type::SPUNK_BASE,
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
        dataBar->setFixedHeight(26);

        gLayout->addWidget(dataBar, index, 0, 1, 1);

        attributeDataBars.insert(type, dataBar);

        index++;
    }

    QSpacerItem *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
    gLayout->addItem(spacerItem, index + 1, 0, 1, 1);
}

void JX3DPSWidget::InitWidgetTalents(QWidget *parent)
{
    std::vector<ComboBox *> talentsComboBoxes;
    QGridLayout            *gLayout = new QGridLayout(parent);
    for (int i = 0; i < 12; ++i) {
        ComboBox *comboBox = new ComboBox(ComboBoxType::ICON_AND_NAME_MODE, parent);
        comboBox->setFixedSize(54, 72);

        gLayout->addWidget(comboBox, i / 6, i % 6, 1, 1);

        talentsComboBoxes.push_back(comboBox);
    }
}

void JX3DPSWidget::InitWidgetPermanents(QWidget *parent)
{
    QGridLayout *gLayout = new QGridLayout(parent);

    std::vector<ComboBox *>  permanentComboBoxes;
    std::vector<std::string> permanents = { "阵眼",     "食品增强", "食品辅助",
                                            "药品增强", "药品辅助", "家园炊事",
                                            "家园酿造", "武器磨石" };

    for (int i = 0; i < permanents.size(); ++i) {
        ComboBox *comboBox = new ComboBox(ComboBoxType::DETAILED_MODE, parent);
        comboBox->setFixedSize(200, 48);
        comboBox->SetItemSize(200, 48);
        gLayout->addWidget(comboBox, i, 0, 1, 2);

        ComboBoxItemInfo itemInfo;
        itemInfo.name = permanents[i];
        comboBox->AddItem(itemInfo);

        permanentComboBoxes.push_back(comboBox);
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
}
