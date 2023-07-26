/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2023-04-08 02:23:52
 * @Update      : NoWats
 * @LastTime    : 2023-04-13 02:29:07
 * @FilePath    : \LegionTDSim\apps\LegionTDSim\Widget.cpp
 */

#include "Widget.h"

#include <fstream>

#include <QEvent>
#include <QEventLoop>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QPainter>
#include <QTimer>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <ThreadPool/ThreadPool.h>

#include "Core/JX3DPS.h"

#include "Button.h"
#include "CheckBox.h"
#include "ComboBox1.h"
#include "DataBars.h"
#include "FramelessWidget.h"
#include "GroupBox.h"
#include "ImportWidget.h"
#include "LineEdit.h"
#include "PlainTextEdit.h"
#include "ProgressBar.h"
#include "SpinBox.h"
#include "Splitter.h"
#include "StatsWidget.h"
#include "TabWidget.h"

void Permanent2Json(const std::vector<ComboBox *> &comboBoxes, const std::vector<CheckBox *> &checkBoxes, nlohmann::json &json)
{
    for (auto &comboBox : comboBoxes) {
        for (auto &items : comboBox->GetItemInfo().subItems) {
            nlohmann::json obj;
            obj["type"]  = items.first;
            obj["value"] = items.second;
            json["AttributeAdd"].push_back(obj);
        }
    }
    for (auto &checkBox : checkBoxes) {
        if (checkBox->isChecked()) {
            for (auto &items : checkBox->GetItemInfo().subItems) {
                nlohmann::json obj;
                obj["type"]  = items.first;
                obj["value"] = items.second;
                json["AttributeAdd"].push_back(obj);
            }
        }
    }
    json["TeamCore"] = comboBoxes[0]->GetItemInfo().id;

}

void Attr2Json(const JX3DPS::Attr &attr, nlohmann::json &json)
{
    json["Agility"]                    = attr.GetAgility();
    json["Spirit"]                     = attr.GetSpirit();
    json["Strength"]                   = attr.GetStrength();
    json["Spunk"]                      = attr.GetSpunk();
    json["PhysicsAttackBase"]          = attr.GetPhysicsAttackBase();
    json["PhysicsCriticalStrike"]      = attr.GetPhysicsCriticalStrike();
    json["PhysicsCriticalStrikePower"] = attr.GetPhysicsCriticalStrikePower();
    json["PhysicsOvercomeBase"]        = attr.GetPhysicsOvercomeBase();
    json["MagicAttackBase"]            = attr.GetMagicAttackBase();
    json["MagicCriticalStrike"]        = attr.GetMagicCriticalStrike();
    json["MagicCriticalStrikePower"]   = attr.GetMagicCriticalStrikePower();
    json["MagicOvercomeBase"]          = attr.GetMagicOvercomeBase();
    json["Haste"]                      = attr.GetHaste();
    json["Strain"]                     = attr.GetStrain();
    json["Surplus"]                    = attr.GetSurplus();
    json["WeaponAttack"]               = attr.GetWeaponAttack();
}

void Secrets2Json(const std::unordered_map<std::string, std::list<SecretCheckBox *>> &secretWidgets,
                  nlohmann::json                                                     &json)
{
    for (auto &secret : secretWidgets) {
        for (auto &secretWidget : secret.second) {
            json[secret.first].push_back(secretWidget->IsChecked());
        }
    }
}

void SetEffects2Json(const std::vector<CheckBox *> &checkBoxes, nlohmann::json &json)
{
    json["EnchantWrist"]  = checkBoxes[0]->isChecked();
    json["EnchantShoes"]  = checkBoxes[1]->isChecked();
    json["EnchantBelt"]   = checkBoxes[2]->isChecked();
    json["EnchantJacket"] = checkBoxes[3]->isChecked();
    json["EnchantHat"]    = checkBoxes[4]->isChecked();
    json["WeaponCW"]      = checkBoxes[5]->isChecked();
    json["WeaponWater"]      = checkBoxes[6]->isChecked();
    json["ClassSetBuff"]  = checkBoxes[7]->isChecked();
    json["ClassSetSkill"] = checkBoxes[8]->isChecked();
}

#include <iostream>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    m_frameless = new FramelessWidget(this);
    m_frameless->SetWidget(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFixedSize(1000, 770);

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

    Button *button = new Button(widget);
    button->setText("开始模拟");
    button->setFont(QFont("NoWatsFont", 14));
    button->setFixedSize(156, 44);
    button->SetButtonColor(QColor(COLOR_BUTTON_GREEN_HOVER), QColor(COLOR_BUTTON_GREEN_NORMAL));

    GroupBox *groupBoxSetting = new GroupBox("设置", widget);
    InitWidgetSetting(groupBoxSetting);

    GroupBox *groupBoxOut = new GroupBox("输出", widget);
    InitWidgetOut(groupBoxOut);

    TabWidget *tabWidgetAttrAndEchant = new TabWidget(widget);
    tabWidgetAttrAndEchant->AddTab("属性");
    InitWidgetAttr(tabWidgetAttrAndEchant->Widget(0));
    tabWidgetAttrAndEchant->AddTab("配装");
    InitWidgetEchant(tabWidgetAttrAndEchant->Widget(1));
    CheckBox *checkBox = new CheckBox(widget);
    checkBox->setGeometry(100, 139, 22, 22);

    TabWidget *tabWidgetAttrGain = new TabWidget(widget);
    tabWidgetAttrGain->AddTab("收益");
    InitWidgetAttrGain(tabWidgetAttrGain->Widget(0));
    tabWidgetAttrGain->AddTab("帮助");

    TabWidget *tabWidgetTalent = new TabWidget(widget);
    tabWidgetTalent->AddTab("奇穴");
    InitWidgetTalent(tabWidgetTalent->Widget(0));
    tabWidgetTalent->AddTab("秘籍");
    InitWidgetSecret(tabWidgetTalent->Widget(1));

    TabWidget *tabWidgetPermanent = new TabWidget(widget);
    tabWidgetPermanent->AddTab("常驻增益");
    InitWidgetPermanent(tabWidgetPermanent->Widget(0));

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
    splitter->addWidget(tabWidgetEvent);

    groupBoxSetting->setFixedWidth(228);
    tabWidgetAttrAndEchant->setFixedWidth(228);
    groupBoxOut->setFixedWidth(156);
    tabWidgetAttrGain->setFixedWidth(156);
    tabWidgetAttrGain->setFixedHeight(371);
    tabWidgetPermanent->setFixedWidth(220);

    QSpacerItem *verticalSpacer3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    QGridLayout *gLayout = new QGridLayout(widget);
    gLayout->setContentsMargins(12, 5, 12, 12);
    gLayout->setSpacing(10);
    gLayout->addWidget(groupBoxSetting, 0, 0, 2, 1);
    gLayout->addWidget(button, 0, 1, 1, 1);
    gLayout->addWidget(groupBoxOut, 1, 1, 1, 1);
    gLayout->addWidget(tabWidgetAttrAndEchant, 2, 0, 2, 1);
    gLayout->addWidget(tabWidgetAttrGain, 2, 1, 1, 1);
    gLayout->addWidget(tabWidgetPermanent, 0, 2, 3, 1);
    gLayout->addWidget(tabWidgetTalent, 3, 1, 1, 2);
    gLayout->addWidget(splitter, 0, 3, 4, 1);

    gLayout->addItem(verticalSpacer3, 3, 0, 1, 1);

    connect(button, &Button::clicked, this, [=]() {
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

        std::vector<int> talents;
        for (auto &talent : m_talentWidgets) {
            talents.push_back(talent->GetId());
        }
        json["talents"] = nlohmann::json(talents);

        nlohmann::json jsonAttr;
        Attr2Json(m_attr, jsonAttr);
        json["attr"] = jsonAttr;

        nlohmann::json jsonSecrets;
        Secrets2Json(m_secretWidgets, jsonSecrets);
        json["secrets"] = jsonSecrets;

        nlohmann::json jsonSetEffects;
        SetEffects2Json(m_setEffectWidgets, jsonSetEffects);
        json["set_effects"] = jsonSetEffects;

        json["delay_min"] = static_cast<LineEdit *>(m_lineEditDelayMin)->text().toInt();
        json["delay_max"] = static_cast<LineEdit *>(m_lineEditDelayMax)->text().toInt();

        nlohmann::json jsonPermanent;
        Permanent2Json(m_permanentComboBoxes, m_permanentCheckBoxes, jsonPermanent);
        json["Permanent"] = jsonPermanent;

        m_progressBar = new ProgressBar(nullptr);
        m_progressBar->setAttribute(Qt::WA_DeleteOnClose);
        m_progressBar->show();

        ThreadPool::Instance()->Enqueue([=]() {
            char *result = new char[1024 * 1024];

            JX3DPSSimulate(json.dump().c_str(), result, this, [](void *obj, double arg) {
                static_cast<Widget *>(obj)->SetProgress(arg);
            });
            std::string str = result;
            delete[] result;
            nlohmann::json res = nlohmann::json::parse(str);
            QMetaObject::invokeMethod(this, [=, this] {
                emit Signal_UpdateGains(res["gains"]);
                emit Signal_UpdateStats(res);
                int  avg = res["gains"]["空"];
                static_cast<LineEdit *>(m_lineEditDPS)->setText(QString::number(avg, 10));
            });
        });
    });

    emit Signal_UpdateAttr();
}

void Widget::SetProgress(double value)
{
    QMetaObject::invokeMethod(this, [=, this] { m_progressBar->SetProgress(value); });
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor(64, 64, 64));
    painter.setBrush(QColor(COLOR_BACKGROUND_BASE));
    painter.drawRoundedRect(this->rect(), 10, 10);
}

bool LoadConfig(const std::string &path, nlohmann::json &json)
{
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        return false;
    }

    ifs >> json;
    ifs.close();
    return true;
}

void ParseJson2Talents(const nlohmann::json &json, JX3DPS::Class classType, std::vector<std::vector<TalentInfo>> &talents)
{
    for (auto &item : json["class"]) {
        if (item["id"].get<int>() == static_cast<int>(classType)) {
            for (int i = 1; i <= 12; ++i) {
                std::vector<TalentInfo> talentInfos;
                for (auto &talentInfo : item["talents"][std::to_string(i)]) {
                    TalentInfo info;
                    info.id     = talentInfo["id"].get<int>();
                    info.iconId = talentInfo["icon"].get<int>();
                    info.name   = talentInfo["name"].get<std::string>();
                    info.type   = talentInfo["type"].get<std::string>();
                    info.desc   = talentInfo["desc"].get<std::string>();
                    talentInfos.push_back(info);
                }
                talents.push_back(talentInfos);
            }
            break;
        }
    }
}

void ParseJson2Secrets(const nlohmann::json                                     &json,
                       JX3DPS::Class                                             classType,
                       std::unordered_map<std::string, std::vector<SecretInfo>> &secrets)
{
    for (auto &item : json["class"]) {
        if (item["id"].get<int>() == static_cast<int>(classType)) {
            for (auto &secretInfo : item["secrets"].items()) {
                std::string key = secretInfo.key();
                for (auto &secret : secretInfo.value()) {
                    SecretInfo info;
                    info.iconId = secret["icon"].get<int>();
                    info.desc   = secret["desc"].get<std::string>();
                    info.name   = secret["name"].get<std::string>();
                    secrets[key].push_back(info);
                }
            }
            break;
        }
    }
}

void ParseJson2SkillExprs(const nlohmann::json                             &json,
                          JX3DPS::Class                                     classType,
                          std::vector<std::pair<std::string, std::string>> &exprs)
{
    exprs.clear();
    for (auto &item : json["class"]) {
        if (item["id"].get<int>() == static_cast<int>(classType)) {
            for (auto &iter : item["default_macros"]) {
                exprs.push_back(std::pair<std::string, std::string>(item["name"].get<std::string>(), ""));
                for (auto &it : iter["expr"]) {
                    exprs.back().second.append(it.get<std::string>());
                    exprs.back().second.append("\n");
                }
            }
        }
    }
}

void ParseJson2EventExprs(const nlohmann::json                             &json,
                          JX3DPS::Class                                     classType,
                          std::vector<std::pair<std::string, std::string>> &exprs)
{
    exprs.clear();
    for (auto &item : json["class"]) {
        if (item["id"].get<int>() == static_cast<int>(classType)) {
            for (auto &iter : item["default_events"]) {
                exprs.push_back(std::pair<std::string, std::string>(item["name"].get<std::string>(), ""));
                for (auto &it : iter["expr"]) {
                    exprs.back().second.append(it.get<std::string>());
                    exprs.back().second.append("\n");
                }
            }
        }
    }
}

bool Widget::InitClass(JX3DPS::Class classType)
{
    m_progressBar = new ProgressBar(nullptr);
    m_progressBar->setAttribute(Qt::WA_DeleteOnClose);
    m_progressBar->show();
    m_progressBar->SetLoadMode();
    connect(this, &Widget::Signal_UpdateProgress, m_progressBar, &ProgressBar::SetProgress);

    nlohmann::json json;
    if (!LoadConfig(".\\config.json", json)) {
        return false;
    }

    m_attr = JX3DPS::Attr(classType);

    ThreadPool::Instance()->Enqueue([=, this] {
        QEventLoop loop;

        QTimer timer;
        int    value = 0;
        connect(&timer, &QTimer::timeout, [&]() {
            emit Signal_UpdateProgress(value / 100.0);
            value++;
            if (value == 100) {
                loop.quit(); // 当进度设置到100后，停止事件循环
            }
        });
        timer.start(40);
        loop.exec();
    });

    std::vector<std::vector<TalentInfo>> talents;
    ParseJson2Talents(json, classType, talents);

    for (int i = 0; i < 12; ++i) {
        for (auto &talentInfo : talents[i]) {
            m_talentWidgets[i]->AddItem(talentInfo);
        }
    }

    std::unordered_map<std::string, std::vector<SecretInfo>> secrets;
    ParseJson2Secrets(json, classType, secrets);

    int index = 0;
    for (auto &secret : secrets) {
        static_cast<VerticalTabWidget *>(m_tabWidgetSecrets)->AddTab(QString::fromStdString(secret.first));
        QGridLayout *gLayoutSecret =
            new QGridLayout(static_cast<VerticalTabWidget *>(m_tabWidgetSecrets)->Widget(index));

        for (int i = 0; i < secret.second.size(); ++i) {

            SecretCheckBox *checkBox =
                new SecretCheckBox(secret.second[i],
                                   static_cast<VerticalTabWidget *>(m_tabWidgetSecrets)->Widget(index));
            gLayoutSecret->addWidget(checkBox, i / 3, i % 3, 1, 1);
            m_secretWidgets[secret.first].push_back(checkBox);
        }
        index++;
    }

    ParseJson2SkillExprs(json, classType, m_skills);
    LoadSkills(0);

    ParseJson2EventExprs(json, classType, m_events);
    LoadEvents(0);

    nlohmann::json jsonClass;
    for (auto &item : json["class"]) {
        if (item["id"].get<int>() == static_cast<int>(classType)) {
            jsonClass = item;
            break;
        }
    }

    emit Signal_UpdatePermanent(jsonClass);

    m_progressBar->SetProgress(1.0);
}

void Widget::LoadSkills(int index)
{
    static_cast<PlainTextEdit *>(m_plainTextEditSkill)
        ->appendPlainText(QString::fromStdString(m_skills[index].second));
}

void Widget::LoadEvents(int index)
{
    static_cast<PlainTextEdit *>(m_plainTextEditEvent)
        ->appendPlainText(QString::fromStdString(m_events[index].second));
}

void Widget::InitWidgetSetting(QWidget *parent)
{
    ComboBoxClass *comboBoxClass = new ComboBoxClass(parent);
    comboBoxClass->setFixedSize(54, 54);

    TextButton *textButtonSimulateCount = new TextButton(parent);
    textButtonSimulateCount->setText("模拟次数");
    textButtonSimulateCount->setFixedSize(62, 26);

    m_lineEditSimulateCount = new LineEdit(parent);
    m_lineEditSimulateCount->setFixedSize(62, 26);
    static_cast<LineEdit *>(m_lineEditSimulateCount)->setText("1");

    // TextButton *textButtonSimulateTime = new TextButton(parent);
    // textButtonSimulateTime->setText("模拟时间");
    // textButtonSimulateTime->setFixedSize(62, 26);

    // LineEdit *lineEditSimulateTime = new LineEdit(parent);
    // lineEditSimulateTime->setFixedSize(48, 21);

    CheckBox *checkBoxDebug = new CheckBox(parent);
    checkBoxDebug->setFixedHeight(22);
    checkBoxDebug->setText("调试");

    TextButton *textButtonDelay = new TextButton(parent);
    textButtonDelay->setText("延迟波动");
    textButtonDelay->setFixedSize(62, 26);

    m_lineEditDelayMin = new LineEdit(parent);
    m_lineEditDelayMin->setFixedSize(62, 26);
    static_cast<LineEdit *>(m_lineEditDelayMin)->setText("40");

    m_lineEditDelayMax = new LineEdit(parent);
    m_lineEditDelayMax->setFixedSize(62, 26);
    static_cast<LineEdit *>(m_lineEditDelayMax)->setText("75");

    QGridLayout *gLayout = new QGridLayout(parent);

    gLayout->addWidget(comboBoxClass, 0, 2, 2, 1);
    gLayout->addWidget(checkBoxDebug, 2, 2, 1, 1);
    gLayout->addWidget(textButtonSimulateCount, 0, 0, 1, 1);
    gLayout->addWidget(m_lineEditSimulateCount, 0, 1, 1, 1);
    // gLayout->addWidget(textButtonSimulateTime, 1, 0, 1, 1);
    // gLayout->addWidget(lineEditSimulateTime, 1, 1, 1, 1);
    gLayout->addWidget(textButtonDelay, 1, 0, 1, 1);
    gLayout->addWidget(m_lineEditDelayMin, 1, 1, 1, 1);
    gLayout->addWidget(m_lineEditDelayMax, 2, 1, 1, 1);

    connect(comboBoxClass, &ComboBoxClass::Signal_CurrentClassChanged, this, [=](int index) {
        InitClass(static_cast<JX3DPS::Class>(index + 1));
    });

    connect(checkBoxDebug, &QCheckBox::stateChanged, [=](int checked) {
        if (checked) {
            spdlog::default_logger()->set_level(spdlog::level::debug);
            spdlog::flush_on(spdlog::level::debug);
        } else {
            spdlog::default_logger()->set_level(spdlog::level::info);
            spdlog::flush_on(spdlog::level::info);
        }
    });
}

void Widget::InitWidgetOut(QWidget *parent)
{
    TextButton *textButtonDPS = new TextButton(parent);
    textButtonDPS->setText("DPS 期望");
    textButtonDPS->setFixedSize(62, 26);

    m_lineEditDPS = new LineEdit(parent);
    m_lineEditDPS->setFixedHeight(26);

    QGridLayout *gLayout = new QGridLayout(parent);

    gLayout->addWidget(textButtonDPS, 0, 0, 1, 1);
    gLayout->addWidget(m_lineEditDPS, 0, 1, 1, 1);

    m_statsWidget = new StatsWidget();

    connect(this,
            QOverload<const nlohmann::json &>::of(&Widget::Signal_UpdateStats),
            static_cast<StatsWidget *>(m_statsWidget),
            QOverload<const nlohmann::json &>::of(&StatsWidget::Signal_UpdateStats));

    connect(textButtonDPS, &TextButton::clicked, this, [=]() { m_statsWidget->show(); });
}

bool ParseJson2Attr(const nlohmann::json &json, JX3DPS::Attr &attr)
{
    try {
        nlohmann::json j = json;
        attr.SetAgilityBase(j.at("Agility").get<int>());
        attr.SetSpiritBase(j.at("Spirit").get<int>());
        attr.SetStrengthBase(j.at("Strength").get<int>());
        attr.SetSpunkBase(j.at("Spunk").get<int>());

        attr.SetPhysicsAttackBaseFromCustom(j.at("PhysicsAttackPowerBase").get<int>() -
                                            attr.GetPhysicsAttackBaseFromMajor());

        attr.SetBoxPhysicsCriticalStrikePercent(j.at("PhysicsCriticalStrikeRate").get<double>());
        attr.SetBoxPhysicsCriticalStrikePowerPercent(j.at("PhysicsCriticalDamagePowerPercent").get<double>());

        attr.SetPhysicsOvercomeBaseFromCustom(j.at("PhysicsOvercome").get<int>() - attr.GetPhysicsOvercomeBaseFromMajor() -
                                              attr.GetPhysicsOvercomeFromClass());
        attr.SetHaste(j.at("Haste").get<int>());
        attr.SetStrain(j.at("Strain").get<int>());
        attr.SetSurplusBase(j.at("SurplusValue").get<int>());
        attr.SetWeaponAttack(j.at("MeleeWeaponDamage").get<int>(),
                             j.at("MeleeWeaponDamage").get<int>() + j.at("MeleeWeaponDamageRand").get<int>());

    } catch (const std::exception &e) {
        spdlog::error("属性解析失败 {}", e.what());
        return false;
    }
    return true;
}

void Widget::InitWidgetAttr(QWidget *parent)
{
    TextButton *textButtonAgilityOrSpirit = new TextButton(parent);
    textButtonAgilityOrSpirit->setText("身法");
    textButtonAgilityOrSpirit->setFixedSize(62, 26);
    LineEdit *lineEditAgilityOrSpirit = new LineEdit(parent);
    lineEditAgilityOrSpirit->setFixedSize(62, 26);
    lineEditAgilityOrSpirit->setAlignment(Qt::AlignRight);
    lineEditAgilityOrSpirit->setReadOnly(true);
    SpinBox *spinBoxAgilityOrSpirit = new SpinBox(parent);
    spinBoxAgilityOrSpirit->setFixedSize(62, 26);

    TextButton *textButtonStrengthOrSpunk = new TextButton(parent);
    textButtonStrengthOrSpunk->setFixedSize(62, 26);
    textButtonStrengthOrSpunk->setText("力道");
    LineEdit *lineEditStrengthOrSpunk = new LineEdit(parent);
    lineEditStrengthOrSpunk->setFixedSize(62, 26);
    lineEditStrengthOrSpunk->setAlignment(Qt::AlignRight);
    lineEditStrengthOrSpunk->setReadOnly(true);
    SpinBox *spinBoxStrengthOrSpunk = new SpinBox(parent);
    spinBoxStrengthOrSpunk->setFixedSize(62, 26);

    TextButton *textButtonAttack = new TextButton(parent);
    textButtonAttack->setFixedSize(62, 26);
    textButtonAttack->setText("外功攻击");
    LineEdit *lineEditAttack = new LineEdit(parent);
    lineEditAttack->setFixedSize(62, 26);
    lineEditAttack->setAlignment(Qt::AlignRight);
    lineEditAttack->setReadOnly(true);
    SpinBox *spinBoxAttack = new SpinBox(parent);
    spinBoxAttack->setFixedSize(62, 26);

    TextButton *textButtonCritical = new TextButton(parent);
    textButtonCritical->setFixedSize(62, 26);
    textButtonCritical->setText("外功会心");
    LineEdit *lineEditCritical = new LineEdit(parent);
    lineEditCritical->setFixedSize(62, 26);
    lineEditCritical->setAlignment(Qt::AlignRight);
    lineEditCritical->setReadOnly(true);
    SpinBox *spinBoxCritical = new SpinBox(parent);
    spinBoxCritical->setFixedSize(62, 26);

    TextButton *textButtonCriticalPower = new TextButton(parent);
    textButtonCriticalPower->setFixedSize(62, 26);
    textButtonCriticalPower->setText("外功会效");
    LineEdit *lineEditCriticalPower = new LineEdit(parent);
    lineEditCriticalPower->setFixedSize(62, 26);
    lineEditCriticalPower->setAlignment(Qt::AlignRight);
    lineEditCriticalPower->setReadOnly(true);
    SpinBox *spinBoxCriticalPower = new SpinBox(parent);
    spinBoxCriticalPower->setFixedSize(62, 26);

    TextButton *textButtonHaste = new TextButton(parent);
    textButtonHaste->setFixedSize(62, 26);
    textButtonHaste->setText("加速");
    LineEdit *lineEditHaste = new LineEdit(parent);
    lineEditHaste->setFixedSize(62, 26);
    lineEditHaste->setAlignment(Qt::AlignRight);
    lineEditHaste->setReadOnly(true);
    SpinBox *spinBoxHaste = new SpinBox(parent);
    spinBoxHaste->setFixedSize(62, 26);

    TextButton *textButtonOvercome = new TextButton(parent);
    textButtonOvercome->setFixedSize(62, 26);
    textButtonOvercome->setText("外功破防");
    LineEdit *lineEditOvercome = new LineEdit(parent);
    lineEditOvercome->setFixedSize(62, 26);
    lineEditOvercome->setAlignment(Qt::AlignRight);
    lineEditOvercome->setReadOnly(true);
    SpinBox *spinBoxOvercome = new SpinBox(parent);
    spinBoxOvercome->setFixedSize(62, 26);

    TextButton *textButtonStrain = new TextButton(parent);
    textButtonStrain->setFixedSize(62, 26);
    textButtonStrain->setText("无双");
    LineEdit *lineEditStrain = new LineEdit(parent);
    lineEditStrain->setFixedSize(62, 26);
    lineEditStrain->setAlignment(Qt::AlignRight);
    lineEditStrain->setReadOnly(true);
    SpinBox *spinBoxStrain = new SpinBox(parent);
    spinBoxStrain->setFixedSize(62, 26);

    TextButton *textButtonSurplus = new TextButton(parent);
    textButtonSurplus->setFixedSize(62, 26);
    textButtonSurplus->setText("破招");
    LineEdit *lineEditSurplus = new LineEdit(parent);
    lineEditSurplus->setFixedSize(62, 26);
    lineEditSurplus->setAlignment(Qt::AlignRight);
    lineEditSurplus->setReadOnly(true);
    SpinBox *spinBoxSurplus = new SpinBox(parent);
    spinBoxSurplus->setFixedSize(62, 26);

    TextButton *textButtonWeaponAttack = new TextButton(parent);
    textButtonWeaponAttack->setFixedSize(62, 26);
    textButtonWeaponAttack->setText("武器伤害");
    SpinBox *spinBoxWeaponAttackMin = new SpinBox(parent);
    spinBoxWeaponAttackMin->setFixedSize(62, 26);
    SpinBox *spinBoxWeaponAttackMax = new SpinBox(parent);
    spinBoxWeaponAttackMax->setFixedSize(62, 26);

    connect(spinBoxAgilityOrSpirit, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        if (textButtonAgilityOrSpirit->text() == "身法") {
            m_attr.SetAgilityBase(value);
        } else {
            m_attr.SetSpiritBase(value);
        }
        emit Signal_UpdateAttr();
    });

    connect(spinBoxStrengthOrSpunk, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        if (textButtonStrengthOrSpunk->text() == "力道") {
            m_attr.SetStrengthBase(value);
        } else {
            m_attr.SetSpunkBase(value);
        }
        emit Signal_UpdateAttr();
    });

    connect(spinBoxAttack, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        if (textButtonAttack->text() == "外功攻击") {
            m_attr.SetPhysicsAttackBase(value);
        } else {
            m_attr.SetMagicAttackBase(value);
        }
        emit Signal_UpdateAttr();
    });

    connect(spinBoxCritical, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        if (textButtonCritical->text() == "外功会心") {
            m_attr.SetPhysicsCriticalStrike(value);
        } else {
            m_attr.SetMagicCriticalStrike(value);
        }
        emit Signal_UpdateAttr();
    });

    connect(spinBoxCriticalPower, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        if (textButtonCriticalPower->text() == "外功会效") {
            m_attr.SetPhysicsCriticalStrikePower(value);
        } else {
            m_attr.SetMagicCriticalStrikePower(value);
        }
        emit Signal_UpdateAttr();
    });

    connect(spinBoxHaste, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        m_attr.SetHaste(value);

        emit Signal_UpdateAttr();
    });

    connect(spinBoxOvercome, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        if (textButtonOvercome->text() == "外功破防") {
            m_attr.SetPhysicsOvercomeBase(value);
        } else {
            m_attr.SetMagicOvercomeBase(value);
        }
        emit Signal_UpdateAttr();
    });

    connect(spinBoxStrain, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        m_attr.SetStrain(value);
        emit Signal_UpdateAttr();
    });

    connect(spinBoxSurplus, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        m_attr.SetSurplusBase(value);
        emit Signal_UpdateAttr();
    });

    connect(spinBoxWeaponAttackMin, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        m_attr.SetWeaponAttack(value, spinBoxWeaponAttackMax->value());
        emit Signal_UpdateAttr();
    });

    connect(spinBoxWeaponAttackMax, &SpinBox::Signal_UpdateValue, this, [=](int value) {
        m_attr.SetWeaponAttack(spinBoxWeaponAttackMin->value(), value);
        emit Signal_UpdateAttr();
    });

    QGridLayout *gLayout = new QGridLayout(parent);

    gLayout->addWidget(textButtonAgilityOrSpirit, 0, 0, 1, 1);
    gLayout->addWidget(lineEditAgilityOrSpirit, 0, 1, 1, 1);
    gLayout->addWidget(spinBoxAgilityOrSpirit, 0, 2, 1, 1);
    gLayout->addWidget(textButtonStrengthOrSpunk, 1, 0, 1, 1);
    gLayout->addWidget(lineEditStrengthOrSpunk, 1, 1, 1, 1);
    gLayout->addWidget(spinBoxStrengthOrSpunk, 1, 2, 1, 1);
    gLayout->addWidget(textButtonAttack, 2, 0, 1, 1);
    gLayout->addWidget(lineEditAttack, 2, 1, 1, 1);
    gLayout->addWidget(spinBoxAttack, 2, 2, 1, 1);
    gLayout->addWidget(textButtonCritical, 3, 0, 1, 1);
    gLayout->addWidget(lineEditCritical, 3, 1, 1, 1);
    gLayout->addWidget(spinBoxCritical, 3, 2, 1, 1);
    gLayout->addWidget(textButtonCriticalPower, 4, 0, 1, 1);
    gLayout->addWidget(lineEditCriticalPower, 4, 1, 1, 1);
    gLayout->addWidget(spinBoxCriticalPower, 4, 2, 1, 1);
    gLayout->addWidget(textButtonHaste, 5, 0, 1, 1);
    gLayout->addWidget(lineEditHaste, 5, 1, 1, 1);
    gLayout->addWidget(spinBoxHaste, 5, 2, 1, 1);
    gLayout->addWidget(textButtonOvercome, 6, 0, 1, 1);
    gLayout->addWidget(lineEditOvercome, 6, 1, 1, 1);
    gLayout->addWidget(spinBoxOvercome, 6, 2, 1, 1);
    gLayout->addWidget(textButtonStrain, 7, 0, 1, 1);
    gLayout->addWidget(lineEditStrain, 7, 1, 1, 1);
    gLayout->addWidget(spinBoxStrain, 7, 2, 1, 1);
    gLayout->addWidget(textButtonSurplus, 8, 0, 1, 1);
    gLayout->addWidget(lineEditSurplus, 8, 1, 1, 1);
    gLayout->addWidget(spinBoxSurplus, 8, 2, 1, 1);
    gLayout->addWidget(textButtonWeaponAttack, 9, 0, 1, 1);
    gLayout->addWidget(spinBoxWeaponAttackMin, 9, 1, 1, 1);
    gLayout->addWidget(spinBoxWeaponAttackMax, 9, 2, 1, 1);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    gLayout->addItem(verticalSpacer, 10, 0, 1, 1);

    GroupBox *groupBoxAttrSetEffect = new GroupBox("装备效果", parent);
    InitWidgetSetEffect(groupBoxAttrSetEffect);
    gLayout->addWidget(groupBoxAttrSetEffect, 11, 0, 1, 3);

    Button *buttonImportJX3Box = new Button(parent);
    buttonImportJX3Box->setFont(QFont("NoWatsFont", 11));
    buttonImportJX3Box->setFixedHeight(35);
    buttonImportJX3Box->setText("导入魔盒属性");

    gLayout->addWidget(buttonImportJX3Box, 12, 0, 1, 3);

    connect(buttonImportJX3Box, &Button::clicked, this, [=] {
        ImportWidget *importWidget = new ImportWidget();
        importWidget->show();
        importWidget->setAttribute(Qt::WA_DeleteOnClose);

        connect(importWidget, &ImportWidget::Signal_Import, this, [=](const nlohmann::json &json) {
            m_json    = json;
            int index = 0;
            for (auto &talent : json["TalentCode"]) {
                m_talentWidgets[index++]->SetTalent(QString::fromStdString(talent["name"].get<std::string>()));
            }

            ParseJson2Attr(json, m_attr);

            emit Signal_UpdateAttr();
        });
    });

    connect(this, &Widget::Signal_UpdateAttr, this, [=] {
        if (textButtonAgilityOrSpirit->text() == "身法") {
            spinBoxAgilityOrSpirit->UpdateValue(m_attr.GetAgility());
            lineEditAgilityOrSpirit->UpdateValue(m_attr.GetAgility());
        } else {
            spinBoxAgilityOrSpirit->UpdateValue(m_attr.GetSpirit());
            lineEditAgilityOrSpirit->UpdateValue(m_attr.GetSpirit());
        }

        if (textButtonStrengthOrSpunk->text() == "力道") {
            spinBoxStrengthOrSpunk->UpdateValue(m_attr.GetStrength());
            lineEditStrengthOrSpunk->UpdateValue(m_attr.GetStrength());
        } else {
            spinBoxStrengthOrSpunk->UpdateValue(m_attr.GetSpunk());
            lineEditStrengthOrSpunk->UpdateValue(m_attr.GetSpunk());
        }

        if (textButtonAttack->text() == "外功攻击") {
            spinBoxAttack->setRange(m_attr.GetPhysicsAttackBaseMinimum());
            spinBoxAttack->UpdateValue(m_attr.GetPhysicsAttackFromBase());
            lineEditAttack->UpdateValue(m_attr.GetPhysicsAttack());

        } else {
            spinBoxAttack->setRange(m_attr.GetMagicAttackBaseMinimum());
            spinBoxAttack->UpdateValue(m_attr.GetMagicAttackFromBase());
            lineEditAttack->UpdateValue(m_attr.GetMagicAttack());
        }

        if (textButtonCritical->text() == "外功会心") {
            spinBoxCritical->setRange(m_attr.GetPhysicsCriticalStrikeMinimum());
            spinBoxCritical->UpdateValue(m_attr.GetPhysicsCriticalStrike());
            lineEditCritical->UpdateValueFloat(m_attr.GetPhysicsCriticalStrikePercent());

        } else {
            spinBoxCritical->setRange(m_attr.GetMagicCriticalStrikeMinimum());
            spinBoxCritical->UpdateValue(m_attr.GetMagicCriticalStrike());
            lineEditCritical->UpdateValueFloat(m_attr.GetMagicCriticalStrikePercent());
        }

        if (textButtonCriticalPower->text() == "外功会效") {
            spinBoxCriticalPower->UpdateValue(m_attr.GetPhysicsCriticalStrikePower());
            lineEditCriticalPower->UpdateValueFloat(m_attr.GetPhysicsCriticalStrikePowerPercent());
        } else {
            spinBoxCriticalPower->UpdateValue(m_attr.GetMagicCriticalStrikePower());
            lineEditCriticalPower->UpdateValueFloat(m_attr.GetMagicCriticalStrikePowerPercent());
        }

        spinBoxHaste->UpdateValue(m_attr.GetHaste());
        lineEditHaste->UpdateValueFloat(m_attr.GetHastePercentVisible());

        if (textButtonOvercome->text() == "外功破防") {
            spinBoxOvercome->setRange(m_attr.GetPhysicsOvercomeBaseMinimum());
            spinBoxOvercome->UpdateValue(m_attr.GetPhysicsOvercomeBase());
            lineEditOvercome->UpdateValueFloat(m_attr.GetPhysicsOvercomePercent());

        } else {
            spinBoxOvercome->setRange(m_attr.GetMagicOvercomeBaseMinimum());
            spinBoxOvercome->UpdateValue(m_attr.GetMagicAttackBase());
            lineEditOvercome->UpdateValueFloat(m_attr.GetMagicOvercomePercent());
        }

        spinBoxStrain->UpdateValue(m_attr.GetStrain());
        lineEditStrain->UpdateValueFloat(m_attr.GetStrainPercent());

        spinBoxSurplus->UpdateValue(m_attr.GetSurplusBase());
        lineEditSurplus->UpdateValue(m_attr.GetSurplus());

        spinBoxWeaponAttackMin->UpdateValue(m_attr.GetWeaponAttackLower());
        spinBoxWeaponAttackMax->UpdateValue(m_attr.GetWeaponAttackUpper());
    });
}

void Widget::InitWidgetEchant(QWidget *parent) { }

void Widget::InitWidgetAttrGain(QWidget *parent)
{
    DataBars *dataBarAgilityOrSpirit = new DataBars(parent);
    dataBarAgilityOrSpirit->setFixedHeight(26);

    DataBars *dataBarStrengthOrSpunk = new DataBars(parent);
    dataBarStrengthOrSpunk->setFixedHeight(26);

    DataBars *dataBarAttack = new DataBars(parent);
    dataBarAttack->setFixedHeight(26);

    DataBars *dataBarCritical = new DataBars(parent);
    dataBarCritical->setFixedHeight(26);

    DataBars *dataBarCriticalPower = new DataBars(parent);
    dataBarCriticalPower->setFixedHeight(26);

    DataBars *dataBarHaste = new DataBars(parent);
    dataBarHaste->setFixedHeight(26);

    DataBars *dataBarOvercome = new DataBars(parent);
    dataBarOvercome->setFixedHeight(26);

    DataBars *dataBarStrain = new DataBars(parent);
    dataBarStrain->setFixedHeight(26);

    DataBars *dataBarSurplus = new DataBars(parent);
    dataBarSurplus->setFixedHeight(26);

    DataBars *dataBarWeaponAttack = new DataBars(parent);
    dataBarWeaponAttack->setFixedHeight(26);

    QGridLayout *gLayout = new QGridLayout(parent);

    gLayout->addWidget(dataBarAgilityOrSpirit, 0, 0, 1, 1);
    gLayout->addWidget(dataBarStrengthOrSpunk, 1, 0, 1, 1);
    gLayout->addWidget(dataBarAttack, 2, 0, 1, 1);
    gLayout->addWidget(dataBarCritical, 3, 0, 1, 1);
    gLayout->addWidget(dataBarCriticalPower, 4, 0, 1, 1);
    gLayout->addWidget(dataBarHaste, 5, 0, 1, 1);
    gLayout->addWidget(dataBarOvercome, 6, 0, 1, 1);
    gLayout->addWidget(dataBarStrain, 7, 0, 1, 1);
    gLayout->addWidget(dataBarSurplus, 8, 0, 1, 1);
    gLayout->addWidget(dataBarWeaponAttack, 9, 0, 1, 1);

    connect(this, &Widget::Signal_UpdateGains, this, [=](const nlohmann::json &json) {
        dataBarAttack->SetValue(json["外功攻击"]);
        dataBarOvercome->SetValue(json["外功破防"]);
        dataBarCritical->SetValue(json["外功会心"]);
        dataBarCriticalPower->SetValue(json["外功会效"]);
        dataBarStrain->SetValue(json["无双"]);
        dataBarSurplus->SetValue(json["破招"]);
        dataBarWeaponAttack->SetValue(json["武器伤害"]);
    });
}

void Widget::InitWidgetTalent(QWidget *parent)
{
    QGridLayout *gLayout = new QGridLayout(parent);
    gLayout->setContentsMargins(10, 10, 10, 4);

    for (int i = 0; i < 12; ++i) {
        TalentWidget *talent = new TalentWidget(parent);
        m_talentWidgets.push_back(talent);
        gLayout->addWidget(talent, i / 6, i % 6, 1, 1);
    }
}

void Widget::InitWidgetSecret(QWidget *parent)
{
    QGridLayout *gLayout = new QGridLayout(parent);
    m_tabWidgetSecrets   = new VerticalTabWidget(parent);

    gLayout->addWidget(m_tabWidgetSecrets, 0, 0, 1, 1);
}

void Widget::InitWidgetSkill(QWidget *parent)
{
    m_plainTextEditSkill = new PlainTextEdit(parent);
    QGridLayout *gLayout = new QGridLayout(parent);
    gLayout->setContentsMargins(0, 0, 0, 0);
    gLayout->addWidget(m_plainTextEditSkill, 0, 0, 1, 1);
}

void Widget::InitWidgetSkillHelp(QWidget *parent) { }

void Widget::InitWidgetEvent(QWidget *parent)
{
    m_plainTextEditEvent = new PlainTextEdit(parent);
    QGridLayout *gLayout = new QGridLayout(parent);
    gLayout->setContentsMargins(0, 0, 0, 0);
    gLayout->addWidget(m_plainTextEditEvent, 0, 0, 1, 1);
}

void Widget::InitWidgetSetEffect(QWidget *parent)
{
    QGridLayout *gLayout = new QGridLayout(parent);

    CheckBox *checkBoxEnchantWrist  = new CheckBox(parent);
    CheckBox *checkBoxEnchantShoes  = new CheckBox(parent);
    CheckBox *checkBoxEnchantBelt   = new CheckBox(parent);
    CheckBox *checkBoxEnchantJacket = new CheckBox(parent);
    CheckBox *checkBoxEnchantHat    = new CheckBox(parent);
    CheckBox *checkBoxWeaponCW      = new CheckBox(parent);
    CheckBox *checkBoxWeaponWater      = new CheckBox(parent);
    CheckBox *checkBoxClassSetBuff  = new CheckBox(parent);
    CheckBox *checkBoxClassSetSkill = new CheckBox(parent);

    m_setEffectWidgets.push_back(checkBoxEnchantWrist);
    m_setEffectWidgets.push_back(checkBoxEnchantShoes);
    m_setEffectWidgets.push_back(checkBoxEnchantBelt);
    m_setEffectWidgets.push_back(checkBoxEnchantJacket);
    m_setEffectWidgets.push_back(checkBoxEnchantHat);
    m_setEffectWidgets.push_back(checkBoxWeaponCW);
    m_setEffectWidgets.push_back(checkBoxWeaponWater);
    m_setEffectWidgets.push_back(checkBoxClassSetBuff);
    m_setEffectWidgets.push_back(checkBoxClassSetSkill);

    checkBoxEnchantWrist->setText("大附魔·腕");
    checkBoxEnchantShoes->setText("大附魔·鞋");
    checkBoxEnchantBelt->setText("大附魔·腰");
    checkBoxEnchantJacket->setText("大附魔·衣");
    checkBoxEnchantHat->setText("大附魔·帽");
    checkBoxWeaponCW->setText("大橙武");
    checkBoxWeaponWater->setText("水特效");
    checkBoxClassSetBuff->setText("套装·属性");
    checkBoxClassSetSkill->setText("套装·技能");

    checkBoxEnchantWrist->setFixedHeight(22);
    checkBoxEnchantShoes->setFixedHeight(22);
    checkBoxEnchantBelt->setFixedHeight(22);
    checkBoxEnchantJacket->setFixedHeight(22);
    checkBoxEnchantHat->setFixedHeight(22);
    checkBoxWeaponCW->setFixedHeight(22);
    checkBoxWeaponWater->setFixedHeight(22);
    checkBoxClassSetBuff->setFixedHeight(22);
    checkBoxClassSetSkill->setFixedHeight(22);

    gLayout->addWidget(checkBoxEnchantWrist, 0, 0, 1, 1);
    gLayout->addWidget(checkBoxEnchantShoes, 1, 0, 1, 1);
    gLayout->addWidget(checkBoxEnchantBelt, 2, 0, 1, 1);
    gLayout->addWidget(checkBoxEnchantJacket, 3, 0, 1, 1);
    gLayout->addWidget(checkBoxEnchantHat, 4, 0, 1, 1);

    gLayout->addWidget(checkBoxWeaponCW, 0, 1, 1, 1);
    gLayout->addWidget(checkBoxWeaponWater, 1, 1, 1, 1);
    gLayout->addWidget(checkBoxClassSetBuff, 2, 1, 1, 1);
    gLayout->addWidget(checkBoxClassSetSkill, 3, 1, 1, 1);
}

#include "BaseWidgets/ComboBox.h"

void Widget::InitWidgetPermanent(QWidget *parent)
{
    QGridLayout *gLayout = new QGridLayout(parent);

    std::vector<std::string> permanents = { "阵眼",     "食品增强", "食品辅助", "药品增强",
                                            "药品辅助", "家园炊事", "家园酿造" };

    for (int i = 0; i < 7; ++i) {
        ComboBox *comboBox = new ComboBox(ComboBoxType::DETAILED_MODE, this);
        comboBox->setFixedHeight(48);
        comboBox->SetItemSize(201, 48);
        gLayout->addWidget(comboBox, i, 0, 1, 2);

        ItemInfo itemInfo;
        itemInfo.name = permanents[i];
        comboBox->AddItem(itemInfo);

        m_permanentComboBoxes.push_back(comboBox);
    }

    connect(this, &Widget::Signal_UpdatePermanent, this, [=](const nlohmann::json &json) {
        for (auto &item : json["Permanent"]["TeamCore"]) {
            
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : item["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.id   = item["id"].get<int>();
            itemInfo.name = item["name"].get<std::string>();
            itemInfo.desc = item["desc"].get<std::string>();
            itemInfo.icon = std::format(":/resources/pics/JX3/Icons/{}.png", item["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentComboBoxes[0]->AddItem(itemInfo);
        }
        for (auto &item : json["Permanent"]["FoodEnhance"]) {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : item["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name = item["name"].get<std::string>();
            itemInfo.desc = item["desc"].get<std::string>();
            itemInfo.icon = std::format(":/resources/pics/JX3/Icons/{}.png", item["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentComboBoxes[1]->AddItem(itemInfo);
        }
        for (auto &item : json["Permanent"]["FoodSupport"]) {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : item["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name = item["name"].get<std::string>();
            itemInfo.desc = item["desc"].get<std::string>();
            itemInfo.icon = std::format(":/resources/pics/JX3/Icons/{}.png", item["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentComboBoxes[2]->AddItem(itemInfo);
        }
        for (auto &item : json["Permanent"]["MedEnhance"]) {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : item["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name = item["name"].get<std::string>();
            itemInfo.desc = item["desc"].get<std::string>();
            itemInfo.icon = std::format(":/resources/pics/JX3/Icons/{}.png", item["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentComboBoxes[3]->AddItem(itemInfo);
        }
        for (auto &item : json["Permanent"]["MedSupport"]) {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : item["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name = item["name"].get<std::string>();
            itemInfo.desc = item["desc"].get<std::string>();
            itemInfo.icon = std::format(":/resources/pics/JX3/Icons/{}.png", item["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentComboBoxes[4]->AddItem(itemInfo);
        }
        for (auto &item : json["Permanent"]["HomeCook"]) {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : item["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name = item["name"].get<std::string>();
            itemInfo.desc = item["desc"].get<std::string>();
            itemInfo.icon = std::format(":/resources/pics/JX3/Icons/{}.png", item["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentComboBoxes[5]->AddItem(itemInfo);
        }
        for (auto &item : json["Permanent"]["HomeWine"]) {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : item["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name = item["name"].get<std::string>();
            itemInfo.desc = item["desc"].get<std::string>();
            itemInfo.icon = std::format(":/resources/pics/JX3/Icons/{}.png", item["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentComboBoxes[6]->AddItem(itemInfo);
        }
        {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : json["Permanent"]["WeaponWhetstone"].front()["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name = json["Permanent"]["WeaponWhetstone"].front()["name"].get<std::string>();
            itemInfo.desc = json["Permanent"]["WeaponWhetstone"].front()["desc"].get<std::string>();
            itemInfo.icon = std::format(":/resources/pics/JX3/Icons/{}.png",
                                        json["Permanent"]["WeaponWhetstone"].front()["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentCheckBoxes[0]->SetItemInfo(itemInfo);
        }
        {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : json["Permanent"]["Others"].front()["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name     = json["Permanent"]["Others"][0]["name"].get<std::string>();
            itemInfo.desc     = json["Permanent"]["Others"][0]["desc"].get<std::string>();
            itemInfo.icon     = std::format(":/resources/pics/JX3/Icons/{}.png",
                                        json["Permanent"]["Others"][0]["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentCheckBoxes[1]->SetItemInfo(itemInfo);
        }
        {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : json["Permanent"]["Others"].front()["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name     = json["Permanent"]["Others"][1]["name"].get<std::string>();
            itemInfo.desc     = json["Permanent"]["Others"][1]["desc"].get<std::string>();
            itemInfo.icon     = std::format(":/resources/pics/JX3/Icons/{}.png",
                                        json["Permanent"]["Others"][1]["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentCheckBoxes[2]->SetItemInfo(itemInfo);
        }
        {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : json["Permanent"]["Others"].front()["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name     = json["Permanent"]["Others"][2]["name"].get<std::string>();
            itemInfo.desc     = json["Permanent"]["Others"][2]["desc"].get<std::string>();
            itemInfo.icon     = std::format(":/resources/pics/JX3/Icons/{}.png",
                                        json["Permanent"]["Others"][2]["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentCheckBoxes[3]->SetItemInfo(itemInfo);
        }
        {
            std::vector<std::pair<std::string, int>> its;
            for (auto &it : json["Permanent"]["Others"].front()["attr"]) {
                its.push_back({ it["type"].get<std::string>(), it["value"].get<int>() });
            }
            ItemInfo itemInfo;
            itemInfo.name     = json["Permanent"]["Others"][3]["name"].get<std::string>();
            itemInfo.desc     = json["Permanent"]["Others"][3]["desc"].get<std::string>();
            itemInfo.icon     = std::format(":/resources/pics/JX3/Icons/{}.png",
                                        json["Permanent"]["Others"][3]["icon"].get<int>());
            itemInfo.subItems = its;
            m_permanentCheckBoxes[4]->SetItemInfo(itemInfo);
        }
    });

    m_permanentCheckBoxes.push_back(new CheckBox(parent));
    m_permanentCheckBoxes.push_back(new CheckBox(parent));
    m_permanentCheckBoxes.push_back(new CheckBox(parent));
    m_permanentCheckBoxes.push_back(new CheckBox(parent));
    m_permanentCheckBoxes.push_back(new CheckBox(parent));

    m_permanentCheckBoxes[0]->setText("武器熔锭");
    m_permanentCheckBoxes[1]->setText("玉笛谁家听落梅");
    m_permanentCheckBoxes[2]->setText("同泽宴");
    m_permanentCheckBoxes[3]->setText("炼狱水煮鱼");
    m_permanentCheckBoxes[4]->setText("蒸鱼菜盘");

    m_permanentCheckBoxes[0]->setFixedHeight(22);
    m_permanentCheckBoxes[1]->setFixedHeight(22);
    m_permanentCheckBoxes[2]->setFixedHeight(22);
    m_permanentCheckBoxes[3]->setFixedHeight(22);
    m_permanentCheckBoxes[4]->setFixedHeight(22);

    gLayout->addWidget(m_permanentCheckBoxes[0], 7, 0, 1, 1);
    gLayout->addWidget(m_permanentCheckBoxes[1], 8, 0, 1, 1);
    gLayout->addWidget(m_permanentCheckBoxes[2], 8, 1, 1, 1);
    gLayout->addWidget(m_permanentCheckBoxes[3], 9, 0, 1, 1);
    gLayout->addWidget(m_permanentCheckBoxes[4], 9, 1, 1, 1);
}
