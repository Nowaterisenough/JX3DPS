/**
 * Project: 
 * File: Widget.h
 * Description:
 * Created Date: 2023-06-07 01:15:59
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-19 06:32:58
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_WIDGET_H__
#define __JX3DPS_WIDGET_H__

#include <QWidget>

class FramelessWidget;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    FramelessWidget *m_frameless = nullptr;
};

#endif // __JX3DPS_WIDGET_H__
