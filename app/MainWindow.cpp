#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QThread>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("JX3DPS");
    this->setFixedWidth(444);
    this->setFixedHeight(367);

    // 信号槽 - 收益计算激活
    connect(ui->lineEdit_BonusAgility, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableAgility));
    connect(ui->lineEdit_BonusStrength, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableStrength));
    connect(ui->lineEdit_BonusAttack, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableAttack));
    connect(ui->lineEdit_BonusCriticalStrike, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableCriticalStrike));
    connect(ui->lineEdit_BonusCriticalStrikePower, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableCriticalStrikePower));
    connect(ui->lineEdit_BonusHaste, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableHaste));
    connect(ui->lineEdit_BonusOvercome, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableOvercome));
    connect(ui->lineEdit_BonusSurplus, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableSurplus));
    connect(ui->lineEdit_BonusStrain, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableStrain));
    connect(ui->lineEdit_BonusWeaponAttack, QOverload<bool>::of(&CustomLineEdit::ChangeEnable), this, QOverload<bool>::of(&MainWindow::ChangeEnableWeaponAttack));

    // 信号槽 - 配装模式激活
    connect(ui->tabWidget, QOverload<int>::of(&CustomTabWidget::EquipEnable), this, QOverload<int>::of(&MainWindow::EquipEnable));


    ui->lineEdit_CriticalStrikePower->setText("109.32%");
//    ui->checkBox_Agility->setChecked(true);
//    ui->checkBox_Attack->setChecked(true);
//    ui->checkBox_CriticalStrike->setChecked(true);
//    ui->checkBox_CriticalStrikePower->setChecked(true);
//    ui->checkBox_Overcome->setChecked(true);
//    ui->checkBox_Surplus->setChecked(true);
//    ui->checkBox_Strain->setChecked(true);
//    ui->checkBox_WeaponAttack->setChecked(true);

    ui->lineEdit_BonusStrength->setEnabled(false);
    ui->spinBox_BonusStrength->setEnabled(false);
    ui->lineEdit_BonusHaste->setEnabled(false);
    ui->spinBox_BonusHaste->setEnabled(false);

    ui->lineEdit_BonusAgility->SetValue(0.0052);
    ui->lineEdit_BonusStrength->SetValue(0.0014);
    ui->lineEdit_BonusAttack->SetValue(0.0161);
    ui->lineEdit_BonusCriticalStrike->SetValue(0.0083);
    ui->lineEdit_BonusCriticalStrikePower->SetValue(0.02);
    m_bonusSettingsPhysicsCriticalStrike.bonus = 0.0083;
    m_bonusSettingsPhysicsCriticalStrikePower.bonus = 0.02;
    //ui->label->setPixmap(QPixmap("D:\\Pic.png").copy(2000, 600, 1350, 675).scaledToWidth(416));

}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::GetHeight()
{
    return 0;
}

void MainWindow::SetHeight(int h)
{
    this->setFixedHeight(h);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (index == 0) {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "height");
        animation->setDuration(static_cast<int>(qAbs(this->height() - 367) / m_tabExpansionSpeed));
        animation->setStartValue(this->height());
        animation->setEndValue(367);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    } else if (index == 1) {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "height");
        animation->setDuration(static_cast<int>(qAbs(this->height() - 714) / m_tabExpansionSpeed));
        animation->setStartValue(this->height());
        animation->setEndValue(714);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    } else if (index == 2) {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "height");
        animation->setDuration(static_cast<int>(qAbs(this->height() - 844) / m_tabExpansionSpeed));
        animation->setStartValue(this->height());
        animation->setEndValue(844);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void MainWindow::on_pushButton_Agility_clicked()
{
    if (m_bonusSettingsAgility.enable) {
        m_bonusSettingsAgility.enable = false;
        m_bonusSettingsSpirit.enable = true;
        ui->pushButton_Agility->setText("根骨");
        ui->spinBox_BonusAgility->setValue(m_bonusSettingsSpirit.count);
        ui->lineEdit_BonusAgility->SetValue(m_bonusSettingsSpirit.bonus);
        ChangeEnableAgility(m_bonusSettingsSpirit.checked);
    } else {
        m_bonusSettingsAgility.enable = true;
        m_bonusSettingsSpirit.enable = false;
        ui->pushButton_Agility->setText("身法");
        ui->spinBox_BonusAgility->setValue(m_bonusSettingsAgility.count);
        ui->lineEdit_BonusAgility->SetValue(m_bonusSettingsAgility.bonus);
        ChangeEnableAgility(m_bonusSettingsAgility.checked);
    }
}

void MainWindow::on_pushButton_Strength_clicked()
{
    if (m_bonusSettingsStrength.enable) {
        m_bonusSettingsStrength.enable = false;
        m_bonusSettingsSpunk.enable = true;
        ui->pushButton_Strength->setText("元气");
        ui->spinBox_BonusStrength->setValue(m_bonusSettingsSpunk.count);
        ui->lineEdit_BonusStrength->SetValue(m_bonusSettingsSpunk.bonus);
        ChangeEnableStrength(m_bonusSettingsSpunk.checked);
    } else {
        m_bonusSettingsStrength.enable = true;
        m_bonusSettingsSpunk.enable = false;
        ui->pushButton_Strength->setText("力道");
        ui->spinBox_BonusStrength->setValue(m_bonusSettingsStrength.count);
        ui->lineEdit_BonusStrength->SetValue(m_bonusSettingsStrength.bonus);
        ChangeEnableStrength(m_bonusSettingsStrength.checked);
    }
}

void MainWindow::on_pushButton_Attack_clicked()
{
    if (m_bonusSettingsPhysicsAttack.enable) {
        m_bonusSettingsPhysicsAttack.enable = false;
        m_bonusSettingsMagicAttack.enable = true;
        ui->pushButton_Attack->setText("内功攻击");
        ui->spinBox_BonusAttack->setValue(m_bonusSettingsMagicAttack.count);
        ui->lineEdit_BonusAttack->SetValue(m_bonusSettingsMagicAttack.bonus);
        ChangeEnableAttack(m_bonusSettingsMagicAttack.checked);
    } else {
        m_bonusSettingsPhysicsAttack.enable = true;
        m_bonusSettingsMagicAttack.enable = false;
        ui->pushButton_Attack->setText("外功攻击");
        ui->spinBox_BonusAttack->setValue(m_bonusSettingsPhysicsAttack.count);
        ui->lineEdit_BonusAttack->SetValue(m_bonusSettingsPhysicsAttack.bonus);
        ChangeEnableAttack(m_bonusSettingsPhysicsAttack.checked);
    }
}

void MainWindow::on_pushButton_CriticalStrike_clicked()
{
    if (m_bonusSettingsPhysicsCriticalStrike.enable) {
        m_bonusSettingsPhysicsCriticalStrike.enable = false;
        m_bonusSettingsMagicCriticalStrike.enable = true;
        ui->pushButton_CriticalStrike->setText("内功会心");
        ui->spinBox_BonusCriticalStrike->setValue(m_bonusSettingsMagicCriticalStrike.count);
        ui->lineEdit_BonusCriticalStrike->SetValue(m_bonusSettingsMagicCriticalStrike.bonus);
        ChangeEnableCriticalStrike(m_bonusSettingsMagicCriticalStrike.checked);
    } else {
        m_bonusSettingsPhysicsCriticalStrike.enable = true;
        m_bonusSettingsMagicCriticalStrike.enable = false;
        ui->pushButton_CriticalStrike->setText("外功会心");
        ui->spinBox_BonusCriticalStrike->setValue(m_bonusSettingsPhysicsCriticalStrike.count);
        ui->lineEdit_BonusCriticalStrike->SetValue(m_bonusSettingsPhysicsCriticalStrike.bonus);
        ChangeEnableCriticalStrike(m_bonusSettingsPhysicsCriticalStrike.checked);
    }
}

void MainWindow::on_pushButton_CriticalStrikePower_clicked()
{
    if (m_bonusSettingsPhysicsCriticalStrikePower.enable) {
        m_bonusSettingsPhysicsCriticalStrikePower.enable = false;
        m_bonusSettingsMagicCriticalStrikePower.enable = true;
        ui->pushButton_CriticalStrikePower->setText("内功会心效果");
        ui->spinBox_BonusCriticalStrikePower->setValue(m_bonusSettingsMagicCriticalStrikePower.count);
        ui->lineEdit_BonusCriticalStrikePower->SetValue(m_bonusSettingsMagicCriticalStrikePower.bonus);
        ChangeEnableCriticalStrikePower(m_bonusSettingsMagicCriticalStrikePower.checked);
    } else {
        m_bonusSettingsPhysicsCriticalStrikePower.enable = true;
        m_bonusSettingsMagicCriticalStrikePower.enable = false;
        ui->pushButton_CriticalStrikePower->setText("外功会心效果");
        ui->spinBox_BonusCriticalStrikePower->setValue(m_bonusSettingsPhysicsCriticalStrikePower.count);
        ui->lineEdit_BonusCriticalStrikePower->SetValue(m_bonusSettingsPhysicsCriticalStrikePower.bonus);
        ChangeEnableCriticalStrikePower(m_bonusSettingsPhysicsCriticalStrikePower.checked);
    }
}

void MainWindow::on_pushButton_Overcome_clicked()
{
    if (m_bonusSettingsPhysicsOvercome.enable) {
        m_bonusSettingsPhysicsOvercome.enable = false;
        m_bonusSettingsMagicOvercome.enable = true;
        ui->pushButton_Overcome->setText("内功破防");
        ui->spinBox_BonusOvercome->setValue(m_bonusSettingsMagicOvercome.count);
        ui->lineEdit_BonusOvercome->SetValue(m_bonusSettingsMagicOvercome.bonus);
        ChangeEnableOvercome(m_bonusSettingsMagicOvercome.checked);
    } else {
        m_bonusSettingsPhysicsOvercome.enable = true;
        m_bonusSettingsMagicOvercome.enable = false;
        ui->pushButton_Overcome->setText("外功破防");
        ui->spinBox_BonusOvercome->setValue(m_bonusSettingsPhysicsOvercome.count);
        ui->lineEdit_BonusOvercome->SetValue(m_bonusSettingsPhysicsOvercome.bonus);
        ChangeEnableOvercome(m_bonusSettingsPhysicsOvercome.checked);
    }
}

void MainWindow::ChangeEnableAgility(bool enable)
{
    ui->lineEdit_BonusAgility->setEnabled(enable);
    ui->spinBox_BonusAgility->setEnabled(enable);
    if (m_bonusSettingsAgility.enable) {
        m_bonusSettingsAgility.checked = enable;
    } else {
        m_bonusSettingsSpirit.checked = enable;
    }
    UpdateBonus();
}

void MainWindow::ChangeEnableStrength(bool enable)
{
    ui->lineEdit_BonusStrength->setEnabled(enable);
    ui->spinBox_BonusStrength->setEnabled(enable);
    if (m_bonusSettingsStrength.enable) {
        m_bonusSettingsStrength.checked = enable;
    } else {
        m_bonusSettingsSpunk.checked = enable;
    }
    UpdateBonus();
}

void MainWindow::ChangeEnableAttack(bool enable)
{
    ui->lineEdit_BonusAttack->setEnabled(enable);
    ui->spinBox_BonusAttack->setEnabled(enable);
    if (m_bonusSettingsPhysicsAttack.enable) {
        m_bonusSettingsPhysicsAttack.checked = enable;
    } else {
        m_bonusSettingsMagicAttack.checked = enable;
    }
    UpdateBonus();
}

void MainWindow::ChangeEnableCriticalStrike(bool enable)
{
    ui->lineEdit_BonusCriticalStrike->setEnabled(enable);
    ui->spinBox_BonusCriticalStrike->setEnabled(enable);
    if (m_bonusSettingsPhysicsCriticalStrike.enable) {
        m_bonusSettingsPhysicsCriticalStrike.checked = enable;
    } else {
        m_bonusSettingsMagicCriticalStrike.checked = enable;
    }
    UpdateBonus();
}

void MainWindow::ChangeEnableCriticalStrikePower(bool enable)
{
    ui->lineEdit_BonusCriticalStrikePower->setEnabled(enable);
    ui->spinBox_BonusCriticalStrikePower->setEnabled(enable);
    if (m_bonusSettingsPhysicsCriticalStrikePower.enable) {
        m_bonusSettingsPhysicsCriticalStrikePower.checked = enable;
    } else {
        m_bonusSettingsMagicCriticalStrikePower.checked = enable;
    }
    UpdateBonus();
}

void MainWindow::ChangeEnableHaste(bool enable)
{
    ui->lineEdit_BonusHaste->setEnabled(enable);
    ui->spinBox_BonusHaste->setEnabled(enable);
    UpdateBonus();
}

void MainWindow::ChangeEnableOvercome(bool enable)
{
    ui->lineEdit_BonusOvercome->setEnabled(enable);
    ui->spinBox_BonusOvercome->setEnabled(enable);
    if (m_bonusSettingsPhysicsOvercome.enable) {
        m_bonusSettingsPhysicsOvercome.checked = enable;
    } else {
        m_bonusSettingsMagicOvercome.checked = enable;
    }
    UpdateBonus();
}

void MainWindow::ChangeEnableSurplus(bool enable)
{
    ui->lineEdit_BonusSurplus->setEnabled(enable);
    ui->spinBox_BonusSurplus->setEnabled(enable);
    UpdateBonus();
}

void MainWindow::ChangeEnableStrain(bool enable)
{
    ui->lineEdit_BonusStrain->setEnabled(enable);
    ui->spinBox_BonusStrain->setEnabled(enable);
    UpdateBonus();
}

void MainWindow::ChangeEnableWeaponAttack(bool enable)
{
    ui->lineEdit_BonusWeaponAttack->setEnabled(enable);
    ui->spinBox_BonusWeaponAttack->setEnabled(enable);
    UpdateBonus();
}

void MainWindow::on_spinBox_BonusAgility_valueChanged(int arg1)
{
    if (m_bonusSettingsAgility.enable) {
        m_bonusSettingsAgility.count = arg1;
    } else {
        m_bonusSettingsSpirit.count = arg1;
    }
}

void MainWindow::on_spinBox_BonusStrength_valueChanged(int arg1)
{
    if (m_bonusSettingsStrength.enable) {
        m_bonusSettingsStrength.count = arg1;
    } else {
        m_bonusSettingsSpunk.count = arg1;
    }
}

void MainWindow::EquipEnable(int enable)
{
    if (enable == Qt::Checked) {
        ui->spinBox_Agility->setReadOnly(true);
        ui->spinBox_Strength->setReadOnly(true);
        ui->spinBox_Attack->setReadOnly(true);
        ui->spinBox_CriticalStrike->setReadOnly(true);
        ui->spinBox_CriticalStrikePower->setReadOnly(true);
        ui->spinBox_Haste->setReadOnly(true);
        ui->spinBox_Overcome->setReadOnly(true);
        ui->spinBox_Surplus->setReadOnly(true);
        ui->spinBox_Strain->setReadOnly(true);
        ui->spinBox_WeaponAttackMin->setReadOnly(true);
        ui->spinBox_WeaponAttackMax->setReadOnly(true);
    } else {
        ui->spinBox_Agility->setReadOnly(false);
        ui->spinBox_Strength->setReadOnly(false);
        ui->spinBox_Attack->setReadOnly(false);
        ui->spinBox_CriticalStrike->setReadOnly(false);
        ui->spinBox_CriticalStrikePower->setReadOnly(false);
        ui->spinBox_Haste->setReadOnly(false);
        ui->spinBox_Overcome->setReadOnly(false);
        ui->spinBox_Surplus->setReadOnly(false);
        ui->spinBox_Strain->setReadOnly(false);
        ui->spinBox_WeaponAttackMin->setReadOnly(false);
        ui->spinBox_WeaponAttackMax->setReadOnly(false);
    }
}

void MainWindow::UpdateBonus()
{
    double max = 0.0;
    if (ui->lineEdit_BonusAgility->isEnabled()) {
        max = ui->lineEdit_BonusAgility->GetValue() > max ? ui->lineEdit_BonusAgility->GetValue() : max;
    }
    if (ui->lineEdit_BonusStrength->isEnabled()) {
        max = ui->lineEdit_BonusStrength->GetValue() > max ? ui->lineEdit_BonusStrength->GetValue() : max;
    }
    if (ui->lineEdit_BonusAttack->isEnabled()) {
        max = ui->lineEdit_BonusAttack->GetValue() > max ? ui->lineEdit_BonusAttack->GetValue() : max;
    }
    if (ui->lineEdit_BonusCriticalStrike->isEnabled()) {
        max = ui->lineEdit_BonusCriticalStrike->GetValue() > max ? ui->lineEdit_BonusCriticalStrike->GetValue() : max;
    }
    if (ui->lineEdit_BonusCriticalStrikePower->isEnabled()) {
        max = ui->lineEdit_BonusCriticalStrikePower->GetValue() > max ? ui->lineEdit_BonusCriticalStrikePower->GetValue() : max;
    }
    if (ui->lineEdit_BonusHaste->isEnabled()) {
        max = ui->lineEdit_BonusHaste->GetValue() > max ? ui->lineEdit_BonusHaste->GetValue() : max;
    }
    if (ui->lineEdit_BonusOvercome->isEnabled()) {
        max = ui->lineEdit_BonusOvercome->GetValue() > max ? ui->lineEdit_BonusOvercome->GetValue() : max;
    }
    if (ui->lineEdit_BonusSurplus->isEnabled()) {
        max = ui->lineEdit_BonusSurplus->GetValue() > max ? ui->lineEdit_BonusSurplus->GetValue() : max;
    }
    if (ui->lineEdit_BonusStrain->isEnabled()) {
        max = ui->lineEdit_BonusStrain->GetValue() > max ? ui->lineEdit_BonusStrain->GetValue() : max;
    }
    if (ui->lineEdit_BonusWeaponAttack->isEnabled()) {
        max = ui->lineEdit_BonusWeaponAttack->GetValue() > max ? ui->lineEdit_BonusWeaponAttack->GetValue() : max;
    }
    CustomLineEdit::SetValueMax(max);
    ui->lineEdit_BonusAgility->SetValue();
    ui->lineEdit_BonusStrength->SetValue();
    ui->lineEdit_BonusAttack->SetValue();
    ui->lineEdit_BonusCriticalStrike->SetValue();
    ui->lineEdit_BonusCriticalStrikePower->SetValue();
    ui->lineEdit_BonusHaste->SetValue();
    ui->lineEdit_BonusOvercome->SetValue();
    ui->lineEdit_BonusSurplus->SetValue();
    ui->lineEdit_BonusStrain->SetValue();
    ui->lineEdit_BonusWeaponAttack->SetValue();
}

