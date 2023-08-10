/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-10 22:50:55
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __STACK_WIDGET_H__
#define __STACK_WIDGET_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_STACK_WIDGET // CMake add_definitions
#        ifdef __GNUC__
#            define STACK_WIDGET_API __attribute__((dllexport))
#        else
#            define STACK_WIDGET_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define STACK_WIDGET_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_STACK_WIDGET_LIB
#                define STACK_WIDGET_API __declspec(dllimport)
#            else
#                define STACK_WIDGET_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_STACK_WIDGET
#    define STACK_WIDGET_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define STACK_WIDGET_API     __attribute__((visibility("default")))
#        define STACK_WIDGET_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define STACK_WIDGET_API
#        define STACK_WIDGET_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define STACK_WIDGET_API     __attribute__((visibility("default")))
#    define STACK_WIDGET_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QPushButton>
#include <QStringList>
#include <QWidget>

#include "Common/ThemeColors.h"

class Stack : public QWidget
{
    Q_OBJECT

public:
    Stack(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

class StackButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(int line READ GetLine WRITE SetLine)

public:
    StackButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    int  GetLine() const;
    void SetLine(int line);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color = QColor(COLOR_HIGHLIGHT);
    int    m_line  = 0;
};

class STACK_WIDGET_API StackWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int dx READ GetDx WRITE SetDx)

public:
    StackWidget(QWidget *parent = nullptr);

    void   AddTab(const QString &text);
    Stack *Widget(int index);
    void   Resize();
    void   AnimatedResize(int index);
    void   Clear();

    int  GetDx() const;
    void SetDx(int dx);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QList<QPair<StackButton *, Stack *>> m_tabs;

    int  m_dx           = 0;
    int  m_lastIndex    = 0;     // 上一个选中的索引
    int  m_currentIndex = 0;     // 当前选中的索引
    bool m_flag         = false; // 是否正在动画
};

#endif // __STACK_WIDGET_H__
