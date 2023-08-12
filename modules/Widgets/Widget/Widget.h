/**
 * Project: JX3DPS
 * File: Widget.h
 * Description:
 * Created Date: 2023-06-07 01:15:59
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-12 06:20:16
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __WIDGET_H__
#define __WIDGET_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_WIDGET // CMake add_definitions
#        ifdef __GNUC__
#            define WIDGET_API __attribute__((dllexport))
#        else
#            define WIDGET_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define WIDGET_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_WIDGET_LIB
#                define WIDGET_API __declspec(dllimport)
#            else
#                define WIDGET_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_WIDGET
#    define WIDGET_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define WIDGET_API     __attribute__((visibility("default")))
#        define WIDGET_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define WIDGET_API
#        define WIDGET_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define WIDGET_API     __attribute__((visibility("default")))
#    define WIDGET_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QPushButton>
#include <QWidget>

#include "Common/ThemeColors.h"
#include "FramelessWidget/FramelessWidget.h"

class WIDGET_PRIVATE Title : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)

public:
    Title(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color = QColor(COLOR_INACTIVE);
};

class WIDGET_PRIVATE CloseButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(QColor textColor READ GetTextColor WRITE SetTextColor)

public:
    CloseButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    QColor GetTextColor() const;
    void   SetTextColor(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color     = QColor(COLOR_BACKGROUND_BASE);
    QColor m_textColor = QColor(COLOR_INACTIVE);
};

class WIDGET_API Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

    void SetTitle(const QString &title);

protected:
    void paintEvent(QPaintEvent *event) override;

    QWidget *centralWidget = nullptr;

private:
    FramelessWidget *m_frameless = nullptr;
    Title           *m_title     = nullptr;
};

#endif // __WIDGET_H__
