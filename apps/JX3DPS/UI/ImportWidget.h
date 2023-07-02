/**
 * Project: JX3DPS
 * File: Import.h
 * Description:
 * Created Date: 2023-07-02 17:46:07
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-02 18:29:14
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef IMPORTWIDGET_H
#define IMPORTWIDGET_H

#include <QDialog>

#include "FramelessWidget.h"

class ImportWidget : public QDialog
{
    Q_OBJECT

public:
    ImportWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    FramelessWidget *m_frameless = nullptr;
};

#endif // IMPORTWIDGET_H