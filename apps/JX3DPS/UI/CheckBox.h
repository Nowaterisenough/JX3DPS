/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-18 00:27:05
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
    QColor m_color = QColor(43, 46, 50);

};

#endif // CHECKBOX_H
