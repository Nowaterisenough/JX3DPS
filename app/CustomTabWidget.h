#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H


#include <QObject>
#include <QWidget>
#include <QTabWidget>
#include <QTabBar>
#include <QCheckBox>

class CustomTabBar : public QTabBar
{
    Q_OBJECT
public:
    CustomTabBar(QWidget *parent = nullptr);

private slots:
    void CheckBoxChange(int index);

signals:
    void EquipEnable(int);

private:
    QCheckBox *m_checkBox = new QCheckBox(this);
};

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    CustomTabWidget(QWidget *parent = nullptr);

signals:
    void EquipEnable(int);

private:
    CustomTabBar *m_tabBar = new CustomTabBar(this);
};

#endif // CUSTOMTABWIDGET_H
