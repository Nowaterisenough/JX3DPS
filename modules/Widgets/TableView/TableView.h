/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-13 02:50:30
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __TABLE_VIEW_H__
#define __TABLE_VIEW_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_TABLE_VIEW // CMake add_definitions
#        ifdef __GNUC__
#            define TABLE_VIEW_API __attribute__((dllexport))
#        else
#            define TABLE_VIEW_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define TABLE_VIEW_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_TABLE_VIEW_LIB
#                define TABLE_VIEW_API __declspec(dllimport)
#            else
#                define TABLE_VIEW_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_TABLE_VIEW
#    define TABLE_VIEW_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define TABLE_VIEW_API     __attribute__((visibility("default")))
#        define TABLE_VIEW_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define TABLE_VIEW_API
#        define TABLE_VIEW_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define TABLE_VIEW_API     __attribute__((visibility("default")))
#    define TABLE_VIEW_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QTableView>

class TABLE_VIEW_API TableView : public QTableView
{
    Q_OBJECT

public:
    TableView(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // __TABLE_VIEW_H__
