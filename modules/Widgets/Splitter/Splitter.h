/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 03:49:20
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __SPLITTER_H__
#define __SPLITTER_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_SPLITTER // CMake add_definitions
#        ifdef __GNUC__
#            define SPLITTER_API __attribute__((dllexport))
#        else
#            define SPLITTER_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define SPLITTER_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_SPLITTER_LIB
#                define SPLITTER_API __declspec(dllimport)
#            else
#                define SPLITTER_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_SPLITTER
#    define SPLITTER_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define SPLITTER_API     __attribute__((visibility("default")))
#        define SPLITTER_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define SPLITTER_API
#        define SPLITTER_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define SPLITTER_API     __attribute__((visibility("default")))
#    define SPLITTER_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QSplitter>

class SPLITTER_API Splitter : public QSplitter
{
    Q_OBJECT

public:
    Splitter(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // __SPLITTER_H__
