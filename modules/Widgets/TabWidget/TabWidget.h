/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-12 06:15:30
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __TAB_WIDGET_H__
#define __TAB_WIDGET_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_TAB_WIDGET // CMake add_definitions
#        ifdef __GNUC__
#            define TAB_WIDGET_API __attribute__((dllexport))
#        else
#            define TAB_WIDGET_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define TAB_WIDGET_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_TAB_WIDGET_LIB
#                define TAB_WIDGET_API __declspec(dllimport)
#            else
#                define TAB_WIDGET_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_TAB_WIDGET
#    define TAB_WIDGET_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define TAB_WIDGET_API     __attribute__((visibility("default")))
#        define TAB_WIDGET_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define TAB_WIDGET_API
#        define TAB_WIDGET_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define TAB_WIDGET_API     __attribute__((visibility("default")))
#    define TAB_WIDGET_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QLineEdit>
#include <QPushButton>
#include <QStringList>
#include <QWidget>

#include "Common/ThemeColors.h"
#include "Widget/Widget.h"

class TAB_WIDGET_API RenameLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    RenameLineEdit(QWidget *parent = nullptr);
};

class TAB_WIDGET_API RenameWidget : public Widget
{
    Q_OBJECT

public:
    RenameWidget(QWidget *parent = nullptr);

    void SetText(const QString &text);

signals:
    void Signal_Rename(const QString &text);
    void Signal_Delete();

private:
    RenameLineEdit *m_lineEdit = nullptr;
};

class TAB_WIDGET_PRIVATE Tab : public QWidget
{
    Q_OBJECT

public:
    Tab(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

class TAB_WIDGET_PRIVATE TabButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(int line READ GetLine WRITE SetLine)

public:
    TabButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    int  GetLine() const;
    void SetLine(int line);

signals:
    void Signal_Delete();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QColor m_color;
    int    m_line = 0;
};

class TAB_WIDGET_API TabWidget : public QWidget
{
    Q_OBJECT

public:
    TabWidget(QWidget *parent = nullptr);

    void AddTab(const QString &text);
    Tab *Widget(int index);

    void SetAddButtonVisible(bool visible);

    QList<QPair<TabButton *, Tab *>> &Tabs();
    int Count() const;
    void Clear();

signals:
    void Signal_AddTab();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QList<QPair<TabButton *, Tab *>> m_tabs;

    QWidget *m_widget = nullptr;

    TabButton *m_tabButton = nullptr;
};

#endif // __TAB_WIDGET_H__
