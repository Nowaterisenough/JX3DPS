/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 02:11:53
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __GROUP_BOX_H__
#define __GROUP_BOX_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_GROUP_BOX // CMake add_definitions
#        ifdef __GNUC__
#            define GROUP_BOX_API __attribute__((dllexport))
#        else
#            define GROUP_BOX_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define GROUP_BOX_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_GROUP_BOX_LIB
#                define GROUP_BOX_API __declspec(dllimport)
#            else
#                define GROUP_BOX_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_GROUP_BOX
#    define GROUP_BOX_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define GROUP_BOX_API     __attribute__((visibility("default")))
#        define GROUP_BOX_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define GROUP_BOX_API
#        define GROUP_BOX_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define GROUP_BOX_API     __attribute__((visibility("default")))
#    define GROUP_BOX_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QGroupBox>

class GROUP_BOX_API GroupBox : public QGroupBox
{
    Q_OBJECT

public:
    GroupBox(const QString &name, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // __GROUP_BOX_H__
