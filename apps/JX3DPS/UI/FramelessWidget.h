/**
 * Project: JX3DPS
 * File: FramelessWidget.h
 * Description:
 * Created Date: 2023-06-16 08:21:30
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-16 08:36:34
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

#include <QWidget>

class FramelessWidget : public QObject
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

#endif // FRAMELESSWIDGET_H
