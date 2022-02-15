/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-07 01:11:57
 * @Update      : NoWats
 * @LastTime    : 2022-02-15 23:05:39
 * @FilePath    : \JX3DPS\app\JX3DPS Application\Widget.cpp
 */

#include "Widget.h"

#include <array>
#include <unordered_map>

#include <QApplication>
#include <QLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QPropertyAnimation>

#include "JX3DPS/JX3DPS.h"

#pragma execution_character_set("utf-8")

constexpr int    JX3DPS_UI_HEIGHT    = 31;
constexpr double TAB_EXPANSION_SPEED = 1.4;
constexpr int    TAB_HEIGHT[5]       = {714, 844, 600, 600, 250};

constexpr char *ATTR_NAME[10] =
    {"身法", "外功攻击", "外功会心", "外功会心效果", "加速", "外功破防", "破招", "无双", "武器伤害"};

const std::unordered_map<std::string, std::string> BRANCH = {{"local", "Local"},
                                                             {"develop", "Dev"},
                                                             {"master", "Release"}};

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    qApp->setFont(QFont("NoWatsFont", 10));
    // qApp->setFont(QFont("微软雅黑", 9.5));
    this->setWindowTitle(JX3DPS::Name());
    this->setFixedWidth(465);
    this->setFixedHeight(TAB_HEIGHT[0]);

    QGridLayout *layout    = new QGridLayout(this);
    QHBoxLayout *layout1   = new QHBoxLayout(this);
    QVBoxLayout *layout2   = new QVBoxLayout(this);
    QTabWidget  *tabWidget = new QTabWidget(this);
    layout->addLayout(layout1, 0, 0);
    layout->addLayout(layout2, 0, 1);
    layout->addWidget(tabWidget, 1, 0, 1, 2);
    layout->setContentsMargins(10, 10, 8, 9);

    QWidget     *widgetAbout = new QWidget(tabWidget);
    QGridLayout *layoutAbout = new QGridLayout(widgetAbout);
    QLabel      *label       = new QLabel(widgetAbout);
    layoutAbout->addWidget(label);
    label->setText(QString("版本: %1.%2")
                       .arg(JX3DPS::Version())
                       .arg(QString::fromStdString(BRANCH.at(JX3DPS::Branch()))));
    label->setAlignment(Qt::AlignCenter);

    QWidget     *widgetAttribute   = new QWidget(tabWidget);
    QGridLayout *layoutAttribute   = new QGridLayout(widgetAttribute);
    QGroupBox   *groupBoxAttribute = new QGroupBox("属性面板 - 收益", widgetAttribute);
    QGroupBox   *groupBoxOthers    = new QGroupBox("其他", widgetAttribute);
    layoutAttribute->addWidget(groupBoxAttribute);
    layoutAttribute->addWidget(groupBoxOthers, 1, 0);
    groupBoxAttribute->setContentsMargins(10, 20, 10, 11);

    QGridLayout *layoutAttribute2 = new QGridLayout(groupBoxAttribute);
    layoutAttribute2->setContentsMargins(0, 0, 0, 0);

    QVector<QPushButton *> btnAttrs;
    QVector<QLineEdit *>   lineEditAttrs;
    QVector<QSpinBox *>    spinBoxAttrs;
    QVector<QSpinBox *>    spinBoxAttrBonus;
    QVector<QLineEdit *>   lineEditAttrBonus;
    btnAttrs.resize(9);
    lineEditAttrs.resize(8);
    spinBoxAttrs.resize(10);
    spinBoxAttrBonus.resize(9);
    lineEditAttrBonus.resize(9);
    for (int i = 0; i < 8; i++) {
        btnAttrs[i]          = new QPushButton(ATTR_NAME[i], groupBoxAttribute);
        lineEditAttrs[i]     = new QLineEdit(groupBoxAttribute);
        spinBoxAttrs[i]      = new QSpinBox(groupBoxAttribute);
        spinBoxAttrBonus[i]  = new QSpinBox(groupBoxAttribute);
        lineEditAttrBonus[i] = new QLineEdit(groupBoxAttribute);
        layoutAttribute2->addWidget(btnAttrs[i], i, 0);
        layoutAttribute2->addWidget(lineEditAttrs[i], i, 1);
        layoutAttribute2->addWidget(spinBoxAttrs[i], i, 2);
        layoutAttribute2->addWidget(spinBoxAttrBonus[i], i, 4);
        layoutAttribute2->addWidget(lineEditAttrBonus[i], i, 5);

        btnAttrs[i]->setFixedSize(98, JX3DPS_UI_HEIGHT);
        lineEditAttrs[i]->setFixedSize(64, JX3DPS_UI_HEIGHT - 2);
        spinBoxAttrs[i]->setFixedSize(64, JX3DPS_UI_HEIGHT - 2);
        spinBoxAttrs[i]->setMaximum(99999);
        spinBoxAttrBonus[i]->setFixedSize(64, JX3DPS_UI_HEIGHT - 2);
        lineEditAttrBonus[i]->setFixedSize(64, JX3DPS_UI_HEIGHT - 2);
    }
    btnAttrs[8]          = new QPushButton(ATTR_NAME[8], groupBoxAttribute);
    spinBoxAttrs[8]      = new QSpinBox(groupBoxAttribute);
    spinBoxAttrs[9]      = new QSpinBox(groupBoxAttribute);
    spinBoxAttrBonus[8]  = new QSpinBox(groupBoxAttribute);
    lineEditAttrBonus[8] = new QLineEdit(groupBoxAttribute);
    QSpacerItem *spacer  = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    layoutAttribute2->addWidget(btnAttrs[8], 8, 0);
    layoutAttribute2->addWidget(spinBoxAttrs[8], 8, 1);
    layoutAttribute2->addWidget(spinBoxAttrs[9], 8, 2);
    layoutAttribute2->addItem(spacer, 8, 3);
    layoutAttribute2->addWidget(spinBoxAttrBonus[8], 8, 4);
    layoutAttribute2->addWidget(lineEditAttrBonus[8], 8, 5);

    btnAttrs[8]->setFixedSize(98, JX3DPS_UI_HEIGHT);
    spinBoxAttrs[8]->setFixedSize(64, JX3DPS_UI_HEIGHT - 2);
    spinBoxAttrs[9]->setFixedSize(64, JX3DPS_UI_HEIGHT - 2);
    spinBoxAttrs[8]->setMaximum(99999);
    spinBoxAttrBonus[8]->setFixedSize(64, JX3DPS_UI_HEIGHT - 2);
    lineEditAttrBonus[8]->setFixedSize(64, JX3DPS_UI_HEIGHT - 2);

    tabWidget->addTab(widgetAttribute, "属性");
    tabWidget->addTab(new QWidget(this), "配装  ");
    tabWidget->addTab(new QWidget(this), "秘籍奇穴");
    tabWidget->addTab(new QWidget(this), "宏");
    tabWidget->addTab(widgetAbout, "关于");

    connect(tabWidget,
            QOverload<int>::of(&QTabWidget::currentChanged),
            this,
            QOverload<int>::of(&Widget::ChangeHeight));

    QGroupBox *groupBox  = new QGroupBox("设置", this);
    QGroupBox *groupBox2 = new QGroupBox("输出", this);
    layout1->addWidget(groupBox);
    layout1->addWidget(groupBox2);

    groupBox->setContentsMargins(10, 20, 10, 11);
    groupBox->setFixedSize(216, 99);
    groupBox2->setContentsMargins(10, 20, 10, 8);
    groupBox2->setFixedSize(92, 99);

    QPushButton *btn     = new QPushButton("模拟", this);
    QPushButton *btn2    = new QPushButton("", this);
    QSpacerItem *spacer2 = new QSpacerItem(10, 7, QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout2->addItem(spacer2);
    layout2->addWidget(btn);
    layout2->addWidget(btn2);

    btn->setFixedHeight(55);
    btn2->setFixedHeight(JX3DPS_UI_HEIGHT);

    QGridLayout *layout3               = new QGridLayout(groupBox);
    QComboBox   *comboxJX3Class        = new QComboBox(groupBox);
    QPushButton *btnSimulateCount      = new QPushButton("次数", groupBox);
    QPushButton *btnSimulateDelay      = new QPushButton("延迟", groupBox);
    QLineEdit   *lineEditSimulateCount = new QLineEdit(groupBox);
    QLineEdit   *lineEditDelayMin      = new QLineEdit(groupBox);
    QLineEdit   *lineEditDelayMax      = new QLineEdit(groupBox);
    layout3->setContentsMargins(0, 0, 0, 0);
    layout3->addWidget(comboxJX3Class, 0, 0, 2, 1);
    layout3->addWidget(btnSimulateCount, 0, 1);
    layout3->addWidget(btnSimulateDelay, 1, 1);
    layout3->addWidget(lineEditSimulateCount, 0, 2, 1, 2);
    layout3->addWidget(lineEditDelayMin, 1, 2);
    layout3->addWidget(lineEditDelayMax, 1, 3);

    comboxJX3Class->setFixedSize(66, 66);
    btnSimulateCount->setFixedSize(46, JX3DPS_UI_HEIGHT);
    btnSimulateDelay->setFixedSize(46, JX3DPS_UI_HEIGHT);
    lineEditSimulateCount->setFixedSize(72, JX3DPS_UI_HEIGHT - 2);
    lineEditDelayMin->setFixedSize(33, JX3DPS_UI_HEIGHT - 2);
    lineEditDelayMax->setFixedSize(33, JX3DPS_UI_HEIGHT - 2);

    QGridLayout *layout4     = new QGridLayout(groupBox2);
    QLineEdit   *lineEditDPS = new QLineEdit(groupBox2);
    QPushButton *btnStats    = new QPushButton("详细统计", groupBox2);
    layout4->setContentsMargins(0, 0, 0, 0);
    layout4->addWidget(lineEditDPS, 0, 1);
    layout4->addWidget(btnStats, 1, 1);
    lineEditDPS->setFixedSize(72, JX3DPS_UI_HEIGHT - 2);
    btnStats->setFixedSize(72, JX3DPS_UI_HEIGHT);

    lineEditDPS->setPlaceholderText("DPS期望");
    lineEditDPS->setAlignment(Qt::AlignCenter);
    lineEditDPS->setReadOnly(true);
}

Widget::~Widget() {}

int Widget::GetHeight() const
{
    return 0;
}

void Widget::SetHeight(int h)
{
    this->setFixedHeight(h);
}

void Widget::ChangeHeight(int index)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "height");
    animation->setDuration(
        static_cast<int>(qAbs(this->height() - TAB_HEIGHT[index]) / TAB_EXPANSION_SPEED));
    animation->setStartValue(this->height());
    animation->setEndValue(TAB_HEIGHT[index]);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}