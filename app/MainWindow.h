#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QGraphicsOpacityEffect>

struct BonusSettings
{
    double bonus = 0.0;
    int count = 0;
    bool enable = false;
    bool checked = false;

    BonusSettings() {}

    BonusSettings(double bonus, int count, bool enable, bool checked) :
        bonus(bonus), count(count), enable(enable), checked(checked)
    {}
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int height READ GetHeight WRITE SetHeight)
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int  GetHeight();
    void SetHeight(int h);

private slots:
    void on_tabWidget_currentChanged(int index);

    void on_pushButton_Agility_clicked();

    void on_pushButton_Strength_clicked();

    void on_pushButton_Attack_clicked();

    void on_pushButton_CriticalStrike_clicked();

    void on_pushButton_CriticalStrikePower_clicked();

    void on_pushButton_Overcome_clicked();

    void ChangeEnableAgility(bool enable);

    void ChangeEnableStrength(bool enable);

    void ChangeEnableAttack(bool enable);

    void ChangeEnableCriticalStrike(bool enable);

    void ChangeEnableCriticalStrikePower(bool enable);

    void ChangeEnableHaste(bool enable);

    void ChangeEnableOvercome(bool enable);

    void ChangeEnableSurplus(bool enable);

    void ChangeEnableStrain(bool enable);

    void ChangeEnableWeaponAttack(bool enable);

    void on_spinBox_BonusAgility_valueChanged(int arg1);

    void on_spinBox_BonusStrength_valueChanged(int arg1);

    void EquipEnable(int);

    void UpdateBonus();

private:
    Ui::MainWindow *ui;

    double m_tabExpansionSpeed = 1.4;

    BonusSettings m_bonusSettingsAgility = BonusSettings(0.0, 0, true, false);
    BonusSettings m_bonusSettingsSpirit = BonusSettings(0.0, 0, false, false);
    BonusSettings m_bonusSettingsStrength = BonusSettings(0.0, 0, true, false);
    BonusSettings m_bonusSettingsSpunk = BonusSettings(0.0, 0, false, false);
    BonusSettings m_bonusSettingsPhysicsAttack = BonusSettings(0.0, 0, true, true);
    BonusSettings m_bonusSettingsMagicAttack = BonusSettings(0.0, 0, false, false);
    BonusSettings m_bonusSettingsPhysicsCriticalStrike = BonusSettings(0.0, 0, true, true);
    BonusSettings m_bonusSettingsMagicCriticalStrike = BonusSettings(0.0, 0, false, false);
    BonusSettings m_bonusSettingsPhysicsCriticalStrikePower = BonusSettings(0.0, 0, true, true);
    BonusSettings m_bonusSettingsMagicCriticalStrikePower = BonusSettings(0.0, 0, false, false);
    BonusSettings m_bonusSettingsPhysicsOvercome = BonusSettings(0.0, 0, true, true);
    BonusSettings m_bonusSettingsMagicOvercome = BonusSettings(0.0, 0, false, false);

};
#endif // MAINWINDOW_H
