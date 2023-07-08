/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-08 07:09:09
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

class TableView : public QTableView
{
    Q_OBJECT

public:
    TableView(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // TABLEVIEW_H
