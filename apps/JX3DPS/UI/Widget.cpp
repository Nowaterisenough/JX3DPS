﻿/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2023-04-08 02:23:52
 * @Update      : NoWats
 * @LastTime    : 2023-04-13 02:29:07
 * @FilePath    : \LegionTDSim\apps\LegionTDSim\Widget.cpp
 */

#include "Widget.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QPainter>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <ThreadPool/ThreadPool.h>

#include "Core/JX3DPS.h"

#include "Button.h"
#include "CheckBox.h"
#include "ComboBox.h"
#include "FramelessWidget.h"
#include "GroupBox.h"
#include "LineEdit.h"
#include "PlainTextEdit.h"
#include "Splitter.h"
#include "TabWidget.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    m_frameless = new FramelessWidget(this);
    m_frameless->SetWidget(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFixedSize(610, 680);

    QGridLayout *layout = new QGridLayout(this);

    TextButton *name = new TextButton(this);
    name->setText("JX3DPS Simulator");

    TextButton *version = new TextButton(this);
    version->setText(QString(JX3DPSVersion()));

    CloseButton *closeButton = new CloseButton(this);
    closeButton->setFixedSize(30, 30);
    closeButton->setToolTip("关闭");
    connect(closeButton, &QPushButton::clicked, this, &Widget::close);

    QWidget *widget = new QWidget(this);

    QSpacerItem *horizontalSpacer1 = new QSpacerItem(3, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
    QSpacerItem *horizontalSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    layout->addItem(horizontalSpacer1, 0, 0, 1, 1);
    layout->addWidget(name, 0, 1, 1, 1);
    layout->addWidget(version, 0, 2, 1, 1);
    layout->addItem(horizontalSpacer2, 0, 3, 1, 1);
    layout->addWidget(closeButton, 0, 4, 1, 1);
    layout->addWidget(widget, 1, 0, 1, 5);

    GreenButton *greenButton = new GreenButton(widget);
    greenButton->setText("开始模拟");
    greenButton->setFont(QFont("Microsoft YaHei", 14));
    greenButton->setFixedSize(134, 44);

    GroupBox *groupBoxSetting = new GroupBox("设置", widget);
    InitWidgetSetting(groupBoxSetting);

    GroupBox *groupBoxOut = new GroupBox("输出", widget);
    InitWidgetOut(groupBoxOut);

    TabWidget *tabWidgetAttrAndEchant = new TabWidget(widget);
    tabWidgetAttrAndEchant->AddTab("属性");
    InitWidgetAttr(tabWidgetAttrAndEchant->Widget(0));
    tabWidgetAttrAndEchant->AddTab("配装");
    InitWidgetEchant(tabWidgetAttrAndEchant->Widget(1));
    CheckBox *checkBoxSecret = new CheckBox(widget);
    checkBoxSecret->setGeometry(95, 127, 22, 22);

    TabWidget *tabWidgetAttrGain = new TabWidget(widget);
    tabWidgetAttrGain->AddTab("收益");
    InitWidgetAttrGain(tabWidgetAttrGain->Widget(0));
    tabWidgetAttrGain->AddTab("帮助");

    TabWidget *tabWidgetTalent = new TabWidget(widget);
    tabWidgetTalent->AddTab("奇穴");
    InitWidgetTalent(tabWidgetTalent->Widget(0));
    tabWidgetTalent->AddTab("秘籍");
    InitWidgetSecret(tabWidgetTalent->Widget(1));
    tabWidgetTalent->AddTab("临时增益");

    TabWidget *tabWidgetSkill = new TabWidget(widget);
    tabWidgetSkill->AddTab("宏");
    InitWidgetSkill(tabWidgetSkill->Widget(0));
    tabWidgetSkill->AddTab("帮助");
    InitWidgetSkillHelp(tabWidgetSkill->Widget(1));

    TabWidget *tabWidgetEvent = new TabWidget(widget);
    tabWidgetEvent->AddTab("事件");
    InitWidgetEvent(tabWidgetEvent->Widget(0));
    tabWidgetEvent->AddTab("事件可视化");
    tabWidgetEvent->AddTab("帮助");

    Splitter *splitter = new Splitter(widget);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(tabWidgetSkill);
    QWidget *spacer = new QWidget(widget);
    spacer->setFixedHeight(10); // 设置间距的宽度
    splitter->addWidget(spacer);
    splitter->addWidget(tabWidgetEvent);

    groupBoxSetting->setFixedWidth(188);
    tabWidgetAttrAndEchant->setFixedWidth(188);
    groupBoxOut->setFixedWidth(134);
    tabWidgetAttrGain->setFixedWidth(134);
    tabWidgetAttrGain->setFixedHeight(312);

    QSpacerItem *verticalSpacer3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    QGridLayout *gLayout = new QGridLayout(widget);
    gLayout->setContentsMargins(12, 5, 12, 12);
    gLayout->setSpacing(10);
    gLayout->addWidget(groupBoxSetting, 0, 0, 2, 1);
    gLayout->addWidget(greenButton, 0, 1, 1, 1);
    gLayout->addWidget(groupBoxOut, 1, 1, 1, 1);
    gLayout->addWidget(tabWidgetAttrAndEchant, 2, 0, 2, 1);
    gLayout->addWidget(tabWidgetAttrGain, 2, 1, 1, 1);
    gLayout->addWidget(splitter, 0, 2, 3, 1);
    gLayout->addWidget(tabWidgetTalent, 3, 1, 1, 2);

    gLayout->addItem(verticalSpacer3, 3, 0, 1, 1);

    connect(greenButton, &GreenButton::clicked, this, [=]() {
        nlohmann::json json;

        nlohmann::json jsonObj;
        QString        text  = static_cast<PlainTextEdit *>(m_plainTextEditSkill)->toPlainText();
        QStringList    lines = text.split("\n", Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            jsonObj.push_back(line.toStdString());
        }
        json["skills"] = jsonObj;

        jsonObj.clear();
        text  = static_cast<PlainTextEdit *>(m_plainTextEditEvent)->toPlainText();
        lines = text.split("\n", Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            jsonObj.push_back(line.toStdString());
        }
        json["events"] = jsonObj;

        int simIterations      = static_cast<LineEdit *>(m_lineEditSimulateCount)->text().toInt();
        json["sim_iterations"] = simIterations;

        std::vector<int> talents = { 1401, 1403, 1409, 1411, 1415, 1420,
                                     1424, 1430, 1433, 1436, 1444, 1448 };
        json["talents"]          = nlohmann::json(talents);

        std::string str = R"({
                    "Vitality": 38099,
                    "Agility": 5674,
                    "Spirit": 41,
                    "Spunk": 41,
                    "Strength": 41,
                    "PhysicsAttackPowerBase": 21983,
                    "PhysicsAttackPower": 30211,
                    "PhysicsCriticalStrikeRate": 0.2541002893573723,
                    "PhysicsCriticalDamagePowerPercent": 1.8277066012448322,
                    "PhysicsOvercomePercent": 0.3735317498171643,
                    "StrainPercent": 0.37304154835986375,
                    "HastePercent": 0.008281187246557064,
                    "SurplusValue": 11959,
                    "MaxHealth": 532708,
                    "PhysicsShieldPercent": 0.06539867266729343,
                    "LunarShieldPercent": 0.05702643085264617,
                    "ToughnessDefCriticalPercent": 0,
                    "DecriticalDamagePercent": 0.1,
                    "MeleeWeaponAttackSpeed": 21,
                    "MeleeWeaponDamage": 2248,
                    "MeleeWeaponDamageRand": 1498,
                    "PhysicsOvercome": 29368,
                    "Strain": 28280,
                    "Haste": 799
                    }
                )";

        json["attr"] = nlohmann::json::parse(str);

        std::string str2 = R"({
                    "无我无剑": [
                        false,
                        true,
                        true,
                        false,
                        true,
                        true,
                        false
                    ],
                    "八荒归元": [
                        true,
                        true,
                        true,
                        false,
                        true
                    ],
                    "三环套月": [
                        true,
                        false,
                        false,
                        true,
                        false,
                        true,
                        true
                    ],
                    "人剑合一": [
                        false,
                        false,
                        false,
                        false,
                        true,
                        true,
                        true,
                        true
                    ],
                    "生太极": [
                        true,
                        true,
                        true,
                        true,
                        false,
                        false,
                        false
                    ]
                })";

        json["secrets"] = nlohmann::json::parse(str2);

        ThreadPool::Instance()->Enqueue([=]() {
            char *result = new char[1024];
            JX3DPSSimulate(json.dump().c_str(), result);
            std::string str = result;
            delete[] result;
            nlohmann::json res = nlohmann::json::parse(str);
            str                = res["avg"].dump();
            QMetaObject::invokeMethod(this, [=, this] {
                static_cast<LineEdit *>(m_lineEditDPS)->setText(QString::fromStdString(str));
            });
        });
    });
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(23, 29, 37));
    painter.drawRoundedRect(this->rect(), 10, 10);
}

void Widget::InitWidgetSetting(QWidget *parent)
{
    ComboBoxClass *comboBoxClass = new ComboBoxClass(parent);
    comboBoxClass->setFixedSize(48, 48);

    TextButton *textButtonSimulateCount = new TextButton(parent);
    textButtonSimulateCount->setText("模拟次数");
    textButtonSimulateCount->setFixedSize(60, 21);

    m_lineEditSimulateCount = new LineEdit(parent);
    m_lineEditSimulateCount->setFixedSize(48, 21);

    TextButton *textButtonSimulateTime = new TextButton(parent);
    textButtonSimulateTime->setText("模拟时间");
    textButtonSimulateTime->setFixedSize(60, 21);

    LineEdit *lineEditSimulateTime = new LineEdit(parent);
    lineEditSimulateTime->setFixedSize(48, 21);

    TextButton *textButtonDelay = new TextButton(parent);
    textButtonDelay->setText("延迟波动");
    textButtonDelay->setFixedSize(60, 21);

    LineEdit *lineEditDelayMin = new LineEdit(parent);
    lineEditDelayMin->setFixedSize(48, 21);

    LineEdit *lineEditDelayMax = new LineEdit(parent);
    lineEditDelayMax->setFixedSize(48, 21);

    QGridLayout *gLayout = new QGridLayout(parent);

    gLayout->addWidget(comboBoxClass, 0, 2, 2, 1);
    gLayout->addWidget(textButtonSimulateCount, 0, 0, 1, 1);
    gLayout->addWidget(m_lineEditSimulateCount, 0, 1, 1, 1);
    gLayout->addWidget(textButtonSimulateTime, 1, 0, 1, 1);
    gLayout->addWidget(lineEditSimulateTime, 1, 1, 1, 1);
    gLayout->addWidget(textButtonDelay, 2, 0, 1, 1);
    gLayout->addWidget(lineEditDelayMin, 2, 1, 1, 1);
    gLayout->addWidget(lineEditDelayMax, 2, 2, 1, 1);
}

void Widget::InitWidgetOut(QWidget *parent)
{

    TextButton *textButtonDPS = new TextButton(parent);
    textButtonDPS->setText("DPS 期望");
    textButtonDPS->setFixedSize(60, 21);

    m_lineEditDPS = new LineEdit(parent);
    m_lineEditDPS->setFixedSize(48, 21);

    // GreenButton *greenButtonDPS = new GreenButton(groupBoxDPS);
    // greenButtonDPS->setText("详情统计");
    // greenButtonDPS->setFont(QFont("Microsoft YaHei", 11));
    // greenButtonDPS->setFixedSize(110, 27);

    QGridLayout *gLayout = new QGridLayout(parent);

    gLayout->addWidget(textButtonDPS, 0, 0, 1, 1);
    gLayout->addWidget(m_lineEditDPS, 0, 1, 1, 1);
}

void Widget::InitWidgetAttr(QWidget *parent)
{

    TextButton *textButtonAgilityOrSpirit = new TextButton(parent);
    textButtonAgilityOrSpirit->setText("身法");
    textButtonAgilityOrSpirit->setFixedSize(60, 21);
    LineEdit *lineEditAgilityOrSpirit = new LineEdit(parent);
    lineEditAgilityOrSpirit->setFixedSize(48, 21);
    LineEdit *lineEditAgilityOrSpirit2 = new LineEdit(parent);
    lineEditAgilityOrSpirit2->setFixedSize(48, 21);

    TextButton *textButtonStrengthOrSpunk = new TextButton(parent);
    textButtonStrengthOrSpunk->setFixedSize(60, 21);
    textButtonStrengthOrSpunk->setText("力道");
    LineEdit *lineEditStrengthOrSpunk = new LineEdit(parent);
    lineEditStrengthOrSpunk->setFixedSize(48, 21);
    LineEdit *lineEditStrengthOrSpunk2 = new LineEdit(parent);
    lineEditStrengthOrSpunk2->setFixedSize(48, 21);

    TextButton *textButtonAttack = new TextButton(parent);
    textButtonAttack->setFixedSize(60, 21);
    textButtonAttack->setText("外功攻击");
    LineEdit *lineEditAttack = new LineEdit(parent);
    lineEditAttack->setFixedSize(48, 21);
    LineEdit *lineEditAttack2 = new LineEdit(parent);
    lineEditAttack2->setFixedSize(48, 21);

    TextButton *textButtonCritical = new TextButton(parent);
    textButtonCritical->setFixedSize(60, 21);
    textButtonCritical->setText("外功会心");
    LineEdit *lineEditCritical = new LineEdit(parent);
    lineEditCritical->setFixedSize(48, 21);
    LineEdit *lineEditCritical2 = new LineEdit(parent);
    lineEditCritical2->setFixedSize(48, 21);

    TextButton *textButtonCriticalPower = new TextButton(parent);
    textButtonCriticalPower->setFixedSize(60, 21);
    textButtonCriticalPower->setText("外功会效");
    LineEdit *lineEditCriticalPower = new LineEdit(parent);
    lineEditCriticalPower->setFixedSize(48, 21);
    LineEdit *lineEditCriticalPower2 = new LineEdit(parent);
    lineEditCriticalPower2->setFixedSize(48, 21);

    TextButton *textButtonHaste = new TextButton(parent);
    textButtonHaste->setFixedSize(60, 21);
    textButtonHaste->setText("加速");
    LineEdit *lineEditHaste = new LineEdit(parent);
    lineEditHaste->setFixedSize(48, 21);
    LineEdit *lineEditHaste2 = new LineEdit(parent);
    lineEditHaste2->setFixedSize(48, 21);

    TextButton *textButtonOvercome = new TextButton(parent);
    textButtonOvercome->setFixedSize(60, 21);
    textButtonOvercome->setText("外功破防");
    LineEdit *lineEditOvercome = new LineEdit(parent);
    lineEditOvercome->setFixedSize(48, 21);
    LineEdit *lineEditOvercome2 = new LineEdit(parent);
    lineEditOvercome2->setFixedSize(48, 21);

    TextButton *textButtonStrain = new TextButton(parent);
    textButtonStrain->setFixedSize(60, 21);
    textButtonStrain->setText("外功无双");
    LineEdit *lineEditStrain = new LineEdit(parent);
    lineEditStrain->setFixedSize(48, 21);
    LineEdit *lineEditStrain2 = new LineEdit(parent);
    lineEditStrain2->setFixedSize(48, 21);

    TextButton *textButtonSurplus = new TextButton(parent);
    textButtonSurplus->setFixedSize(60, 21);
    textButtonSurplus->setText("破招");
    LineEdit *lineEditSurplus = new LineEdit(parent);
    lineEditSurplus->setFixedSize(48, 21);
    LineEdit *lineEditSurplus2 = new LineEdit(parent);
    lineEditSurplus2->setFixedSize(48, 21);

    TextButton *textButtonWeaponAttack = new TextButton(parent);
    textButtonWeaponAttack->setFixedSize(60, 21);
    textButtonWeaponAttack->setText("武器伤害");
    LineEdit *lineEditWeaponAttack = new LineEdit(parent);
    lineEditWeaponAttack->setFixedSize(48, 21);
    LineEdit *lineEditWeaponAttack2 = new LineEdit(parent);
    lineEditWeaponAttack2->setFixedSize(48, 21);

    QGridLayout *gLayout = new QGridLayout(parent);

    gLayout->addWidget(textButtonAgilityOrSpirit, 0, 0, 1, 1);
    gLayout->addWidget(lineEditAgilityOrSpirit, 0, 1, 1, 1);
    gLayout->addWidget(lineEditAgilityOrSpirit2, 0, 2, 1, 1);
    gLayout->addWidget(textButtonStrengthOrSpunk, 1, 0, 1, 1);
    gLayout->addWidget(lineEditStrengthOrSpunk, 1, 1, 1, 1);
    gLayout->addWidget(lineEditStrengthOrSpunk2, 1, 2, 1, 1);
    gLayout->addWidget(textButtonAttack, 2, 0, 1, 1);
    gLayout->addWidget(lineEditAttack, 2, 1, 1, 1);
    gLayout->addWidget(lineEditAttack2, 2, 2, 1, 1);
    gLayout->addWidget(textButtonCritical, 3, 0, 1, 1);
    gLayout->addWidget(lineEditCritical, 3, 1, 1, 1);
    gLayout->addWidget(lineEditCritical2, 3, 2, 1, 1);
    gLayout->addWidget(textButtonCriticalPower, 4, 0, 1, 1);
    gLayout->addWidget(lineEditCriticalPower, 4, 1, 1, 1);
    gLayout->addWidget(lineEditCriticalPower2, 4, 2, 1, 1);
    gLayout->addWidget(textButtonHaste, 5, 0, 1, 1);
    gLayout->addWidget(lineEditHaste, 5, 1, 1, 1);
    gLayout->addWidget(lineEditHaste2, 5, 2, 1, 1);
    gLayout->addWidget(textButtonOvercome, 6, 0, 1, 1);
    gLayout->addWidget(lineEditOvercome, 6, 1, 1, 1);
    gLayout->addWidget(lineEditOvercome2, 6, 2, 1, 1);
    gLayout->addWidget(textButtonStrain, 7, 0, 1, 1);
    gLayout->addWidget(lineEditStrain, 7, 1, 1, 1);
    gLayout->addWidget(lineEditStrain2, 7, 2, 1, 1);
    gLayout->addWidget(textButtonSurplus, 8, 0, 1, 1);
    gLayout->addWidget(lineEditSurplus, 8, 1, 1, 1);
    gLayout->addWidget(lineEditSurplus2, 8, 2, 1, 1);
    gLayout->addWidget(textButtonWeaponAttack, 9, 0, 1, 1);
    gLayout->addWidget(lineEditWeaponAttack, 9, 1, 1, 1);
    gLayout->addWidget(lineEditWeaponAttack2, 9, 2, 1, 1);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    gLayout->addItem(verticalSpacer, 10, 0, 1, 1);

    GroupBox *groupBoxAttrSetEffect = new GroupBox("装备效果", parent);

    gLayout->addWidget(groupBoxAttrSetEffect, 11, 0, 1, 3);
}

void Widget::InitWidgetEchant(QWidget *parent) { }

void Widget::InitWidgetAttrGain(QWidget *parent)
{

    LineEdit *lineEditAttrGainAgilityOrSpirit = new LineEdit(parent);
    lineEditAttrGainAgilityOrSpirit->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainAgilityOrSpirit2 = new LineEdit(parent);
    lineEditAttrGainAgilityOrSpirit2->setFixedSize(48, 21);

    LineEdit *lineEditAttrGainStrengthOrSpunk = new LineEdit(parent);
    lineEditAttrGainStrengthOrSpunk->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainStrengthOrSpunk2 = new LineEdit(parent);
    lineEditAttrGainStrengthOrSpunk2->setFixedSize(48, 21);

    LineEdit *lineEditAttrGainAttack = new LineEdit(parent);
    lineEditAttrGainAttack->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainAttack2 = new LineEdit(parent);
    lineEditAttrGainAttack2->setFixedSize(48, 21);

    LineEdit *lineEditAttrGainCritical = new LineEdit(parent);
    lineEditAttrGainCritical->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainCritical2 = new LineEdit(parent);
    lineEditAttrGainCritical2->setFixedSize(48, 21);

    LineEdit *lineEditAttrGainCriticalPower = new LineEdit(parent);
    lineEditAttrGainCriticalPower->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainCriticalPower2 = new LineEdit(parent);
    lineEditAttrGainCriticalPower2->setFixedSize(48, 21);

    LineEdit *lineEditAttrGainHaste = new LineEdit(parent);
    lineEditAttrGainHaste->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainHaste2 = new LineEdit(parent);
    lineEditAttrGainHaste2->setFixedSize(48, 21);

    LineEdit *lineEditAttrGainOvercome = new LineEdit(parent);
    lineEditAttrGainOvercome->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainOvercome2 = new LineEdit(parent);
    lineEditAttrGainOvercome2->setFixedSize(48, 21);

    LineEdit *lineEditAttrGainStrain = new LineEdit(parent);
    lineEditAttrGainStrain->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainStrain2 = new LineEdit(parent);
    lineEditAttrGainStrain2->setFixedSize(48, 21);

    LineEdit *lineEditAttrGainSurplus = new LineEdit(parent);
    lineEditAttrGainSurplus->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainSurplus2 = new LineEdit(parent);
    lineEditAttrGainSurplus2->setFixedSize(48, 21);

    LineEdit *lineEditAttrGainWeaponAttack = new LineEdit(parent);
    lineEditAttrGainWeaponAttack->setFixedSize(48, 21);
    LineEdit *lineEditAttrGainWeaponAttack2 = new LineEdit(parent);
    lineEditAttrGainWeaponAttack2->setFixedSize(48, 21);

    QGridLayout *gLayout = new QGridLayout(parent);
    gLayout->addWidget(lineEditAttrGainAgilityOrSpirit, 0, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainAgilityOrSpirit2, 0, 1, 1, 1);
    gLayout->addWidget(lineEditAttrGainStrengthOrSpunk, 1, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainStrengthOrSpunk2, 1, 1, 1, 1);
    gLayout->addWidget(lineEditAttrGainAttack, 2, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainAttack2, 2, 1, 1, 1);
    gLayout->addWidget(lineEditAttrGainCritical, 3, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainCritical2, 3, 1, 1, 1);
    gLayout->addWidget(lineEditAttrGainCriticalPower, 4, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainCriticalPower2, 4, 1, 1, 1);
    gLayout->addWidget(lineEditAttrGainHaste, 5, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainHaste2, 5, 1, 1, 1);
    gLayout->addWidget(lineEditAttrGainOvercome, 6, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainOvercome2, 6, 1, 1, 1);
    gLayout->addWidget(lineEditAttrGainStrain, 7, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainStrain2, 7, 1, 1, 1);
    gLayout->addWidget(lineEditAttrGainSurplus, 8, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainSurplus2, 8, 1, 1, 1);
    gLayout->addWidget(lineEditAttrGainWeaponAttack, 9, 0, 1, 1);
    gLayout->addWidget(lineEditAttrGainWeaponAttack2, 9, 1, 1, 1);
}

void Widget::InitWidgetTalent(QWidget *parent)
{

    // for (int i = 0; i < 12; ++i) {
    //     ComboBoxTalent *talent = new ComboBoxTalent(parent);
    //     talent->setFixedSize(52, 62);
    //     gLayout->addWidget(talent, i / 6, i % 6, 1, 1);
    // }
}

void Widget::InitWidgetSecret(QWidget *parent)
{

    QGridLayout       *gLayout   = new QGridLayout(parent);
    VerticalTabWidget *tabWidget = new VerticalTabWidget(parent);
    tabWidget->AddTab("无我无剑");
    tabWidget->AddTab("八荒归元");
    tabWidget->AddTab("三环套月");
    tabWidget->AddTab("人剑合一");
    tabWidget->AddTab("生太极");
    gLayout->addWidget(tabWidget, 0, 0, 1, 1);
}

void Widget::InitWidgetSkill(QWidget *parent)
{
    m_plainTextEditSkill = new PlainTextEdit(parent);
    QGridLayout *gLayout = new QGridLayout(parent);
    gLayout->setContentsMargins(0, 0, 0, 0);
    gLayout->addWidget(m_plainTextEditSkill, 0, 0, 1, 1);

    static_cast<PlainTextEdit *>(m_plainTextEditSkill)
        ->appendPlainText("/cast [qidian<10&bufftime:玄门>20] 紫气东来\n"
                          "/cast 三环套月\n"
                          "/cast [buff:风逝] 人剑合一\n"
                          "/cast [bufftime:玄门>35] 碎星辰\n"
                          "/cast [buff:风逝|buff:持盈] 无我无剑\n"
                          "/cast 八荒归元\n"
                          "/cast 吞日月\n"
                          "/cast 三柴剑法\n");
}

void Widget::InitWidgetSkillHelp(QWidget *parent) { }

void Widget::InitWidgetEvent(QWidget *parent)
{
    m_plainTextEditEvent = new PlainTextEdit(parent);
    QGridLayout *gLayout = new QGridLayout(parent);
    gLayout->setContentsMargins(0, 0, 0, 0);
    gLayout->addWidget(m_plainTextEditEvent, 0, 0, 1, 1);

    static_cast<PlainTextEdit *>(m_plainTextEditEvent)
        ->appendPlainText(
            "00:00 /add_buff id=0 name=玄门 stack_num=3 duration=35\n"
            "00:00 /add_buff id=0 name=气场·碎星辰 stack_num=1 duration=22\n"
            "00:00 /add_target id=1 level=124 shield=27550 distance=3\n"
            "00:00 /set_target id=1\n"
            "01:30 /set_target id=1 life=0.9\n"
            "03:00 /set_target id=1 life=0.8\n"
            "04:30 /set_target id=1 life=0.7\n"
            "06:00 /set_target id=1 life=0.6\n"
            "07:30 /set_target id=1 life=0.5\n"
            "07:30 /add_target id=2 level=124 shield=27550 distance=3\n"
            "07:30 /set_target id=2\n"
            "08:00 /set_target id=2 dead\n"
            "08:00 /stop\n"
            "09:00 /continue\n"
            "09:00 /add_buff id=0 name=玄门 stack_num=3 duration=35\n"
            "09:00 /set_target id=1\n"
            "09:00 /set_target id=1 life=0.4\n"
            "10:30 /set_target id=1 life=0.3\n"
            "12:00 /set_target id=1 life=0.2\n"
            "13:30 /set_target id=1 life=0.1\n"
            "15:00 /end\n");
}