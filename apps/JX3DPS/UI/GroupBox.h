/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-18 01:12:49
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef GROUPBOX_H
#define GROUPBOX_H

#include <QGroupBox>

class GroupBox : public QGroupBox
{
    Q_OBJECT

public:
    GroupBox(const QString &name, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // GROUPBOX_H
