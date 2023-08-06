/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 04:14:41
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __LINEEDIT_H__
#define __LINEEDIT_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_LINE_EDIT // CMake add_definitions
#        ifdef __GNUC__
#            define LINE_EDIT_API __attribute__((dllexport))
#        else
#            define LINE_EDIT_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define LINE_EDIT_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_LINE_EDIT_LIB
#                define LINE_EDIT_API __declspec(dllimport)
#            else
#                define LINE_EDIT_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_LINE_EDIT
#    define LINE_EDIT_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define LINE_EDIT_API     __attribute__((visibility("default")))
#        define LINE_EDIT_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define LINE_EDIT_API
#        define LINE_EDIT_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define LINE_EDIT_API     __attribute__((visibility("default")))
#    define LINE_EDIT_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QLineEdit>

class LINE_EDIT_API LineEdit : public QLineEdit
{
    Q_OBJECT

public:
    LineEdit(QWidget *parent = nullptr);

    void UpdateValue(int value);
    void UpdateValueFloat(float value);
};

#endif // __LINEEDIT_H__
