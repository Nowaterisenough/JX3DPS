/**
 * Project: JX3DPS
 * File: ProgressBar.h
 * Description:
 * Created Date: 2023-06-30 23:40:42
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-12 01:59:47
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __PROGRESS_BAR_H__
#define __PROGRESS_BAR_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_PROGRESS_BAR // CMake add_definitions
#        ifdef __GNUC__
#            define PROGRESS_BAR_API __attribute__((dllexport))
#        else
#            define PROGRESS_BAR_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define PROGRESS_BAR_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_PROGRESS_BAR_LIB
#                define PROGRESS_BAR_API __declspec(dllimport)
#            else
#                define PROGRESS_BAR_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_PROGRESS_BAR
#    define PROGRESS_BAR_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define PROGRESS_BAR_API     __attribute__((visibility("default")))
#        define PROGRESS_BAR_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define PROGRESS_BAR_API
#        define PROGRESS_BAR_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define PROGRESS_BAR_API     __attribute__((visibility("default")))
#    define PROGRESS_BAR_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QDialog>

#include "FramelessWidget/FramelessWidget.h"

class PROGRESS_BAR_API ProgressBar : public QDialog
{
    Q_OBJECT

public:
    ProgressBar(QWidget *parent = nullptr);

    double GetProgress() const;

public slots:
    void SetProgress(double progress);

    void SetProgress(double progress, const QString &text);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    FramelessWidget *m_frameless = nullptr;
    double           m_progress  = 0.0;
    QString          m_text;
};

#endif // __PROGRESS_BAR_H__