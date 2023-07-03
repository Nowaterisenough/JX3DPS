/**
 * Project: JX3DPS
 * File: SpinBox.h
 * Description:
 * Created Date: 2023-07-03 21:37:34
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-04 00:59:06
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef SPINBOX_H
#define SPINBOX_H

#include <QStyleOptionSpinBox>
#include <QStylePainter>
#include <QtWidgets>

class SpinBox : public QWidget
{
    Q_OBJECT

public:
    explicit SpinBox(QWidget *parent = nullptr);
    int value() const;
    void setValue(int value);
    void setRange(int min, int max);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    //void leaveEvent(QEvent *event) override;

private:
    int m_value;
    int m_min;
    int m_max;
    bool m_dragging;
    QPoint m_dragStartPos;
};



#endif // SPINBOX_H
