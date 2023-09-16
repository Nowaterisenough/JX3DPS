/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-08 05:59:24
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_BUTTON // CMake add_definitions
#        ifdef __GNUC__
#            define BUTTON_API __attribute__((dllexport))
#        else
#            define BUTTON_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define BUTTON_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_BUTTON_LIB
#                define BUTTON_API __declspec(dllimport)
#            else
#                define BUTTON_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_BUTTON
#    define BUTTON_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define BUTTON_API     __attribute__((visibility("default")))
#        define BUTTON_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define BUTTON_API
#        define BUTTON_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define BUTTON_API     __attribute__((visibility("default")))
#    define BUTTON_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QColor>
#include <QPushButton>

#include "Common/ThemeColors.h"

class BUTTON_API TextButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)

public:
    TextButton(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_color = QColor(COLOR_INACTIVE);
};

class BUTTON_API Button : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ GetColor WRITE SetColor)
    Q_PROPERTY(QColor textColor READ GetTextColor WRITE SetTextColor)

public:
    Button(QWidget *parent = nullptr);

    QColor GetColor() const;
    void   SetColor(QColor color);

    QColor GetTextColor() const;
    void   SetTextColor(QColor color);

    void SetButtonColor(const QColor &hover, const QColor &normal);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QColor m_textColor   = QColor(220, 220, 220);
    QColor m_hoverColor  = QColor(COLOR_BUTTON_BLUE_HOVER);
    QColor m_normalColor = QColor(COLOR_BUTTON_BLUE_NORMAL);
    QColor m_color       = m_normalColor;
};

#endif // __BUTTON_H__
