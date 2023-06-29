/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-29 20:55:40
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QCheckBox>
#include <QColor>

#include "ThemeColors.h"

class CheckBox : public QCheckBox
{
    Q_OBJECT

public:
    CheckBox(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

private:
    QColor m_color = QColor(COLOR_BACKGROUND_HIGHLIGHT);

};

#endif // CHECKBOX_H
