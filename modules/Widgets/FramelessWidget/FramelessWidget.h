/**
 * Project: 
 * File: FramelessWidget.h
 * Description:
 * Created Date: 2023-06-16 08:21:30
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-21 14:38:41
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __FRAMELESS_WIDGET_H__
#define __FRAMELESS_WIDGET_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_FRAMELESS_WIDGET // CMake add_definitions
#        ifdef __GNUC__
#            define FRAMELESS_WIDGET_API __attribute__((dllexport))
#        else
#            define FRAMELESS_WIDGET_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define FRAMELESS_WIDGET_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_FRAMELESS_WIDGET_LIB
#                define FRAMELESS_WIDGET_API __declspec(dllimport)
#            else
#                define FRAMELESS_WIDGET_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_FRAMELESS_WIDGET
#    define FRAMELESS_WIDGET_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define FRAMELESS_WIDGET_API     __attribute__((visibility("default")))
#        define FRAMELESS_WIDGET_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define FRAMELESS_WIDGET_API
#        define FRAMELESS_WIDGET_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define FRAMELESS_WIDGET_API     __attribute__((visibility("default")))
#    define FRAMELESS_WIDGET_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QWidget>

class FRAMELESS_WIDGET_API FramelessWidget : public QObject
{
    Q_OBJECT

public:
    explicit FramelessWidget(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    // 边距+可移动+可拉伸
    int  padding;
    bool moveEnable;
    bool resizeEnable;

    // 无边框窗体
    QWidget *widget;

    // 鼠标是否按下+按下坐标+按下时窗体区域
    bool   mousePressed;
    QPoint mousePoint;
    QRect  mouseRect;

    // 鼠标是否按下某个区域+按下区域的大小
    // 依次为 左侧+右侧+上侧+下侧+左上侧+右上侧+左下侧+右下侧
    QList<bool>  pressedArea;
    QList<QRect> pressedRect;

    // 记录是否最小化
    bool            isMin;
    // 存储窗体默认的属性
    Qt::WindowFlags flags;

public slots:

    // 设置边距
    void SetPadding(int padding);

    // 设置是否可拖动+拉伸
    void SetMoveEnable(bool moveEnable);

    void SetResizeEnable(bool resizeEnable);

    // 修复部分控件不能自动识别 MouseButtonRelease 的bug
    void SetMousePressed(bool mousePressed);

    // 设置要无边框的窗体
    void SetWidget(QWidget *widget);
};

#endif // __FRAMELESS_WIDGET_H__
