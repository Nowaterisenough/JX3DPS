/**
 * Project: JX3DPS
 * File: SpinBox.h
 * Description:
 * Created Date: 2023-07-03 21:37:34
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 04:54:44
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __SPIN_BOX_H__
#define __SPIN_BOX_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_SPIN_BOX // CMake add_definitions
#        ifdef __GNUC__
#            define SPIN_BOX_API __attribute__((dllexport))
#        else
#            define SPIN_BOX_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define SPIN_BOX_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_SPIN_BOX_LIB
#                define SPIN_BOX_API __declspec(dllimport)
#            else
#                define SPIN_BOX_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_SPIN_BOX
#    define SPIN_BOX_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define SPIN_BOX_API     __attribute__((visibility("default")))
#        define SPIN_BOX_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define SPIN_BOX_API
#        define SPIN_BOX_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define SPIN_BOX_API     __attribute__((visibility("default")))
#    define SPIN_BOX_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QStyleOptionSpinBox>
#include <QStylePainter>
#include <QtWidgets>

class SPIN_BOX_API SpinBox : public QWidget
{
    Q_OBJECT

public:
    explicit SpinBox(QWidget *parent = nullptr);
    int  value() const;
    void setValue(int value);
    void setRange(int min, int max = 99999);

    void UpdateValue(int value);

signals:
    void Signal_UpdateValue(int value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    // void leaveEvent(QEvent *event) override;

private:
    int    m_value;
    int    m_min;
    int    m_max = 99999;
    bool   m_dragging;
    QPoint m_dragStartPos;
};

#endif // __SPIN_BOX_H__
