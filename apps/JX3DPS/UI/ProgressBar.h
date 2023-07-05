/**
 * Project: JX3DPS
 * File: ProgressBar.h
 * Description:
 * Created Date: 2023-06-30 23:40:42
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-05 11:23:38
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QDialog>

#include "FramelessWidget.h"

class ProgressBar : public QDialog
{
    Q_OBJECT

public:
    ProgressBar(QWidget *parent = nullptr);

    double GetProgress() const;
    void   SetProgress(double progress);

    void SetLoadMode();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    FramelessWidget *m_frameless = nullptr;
    double           m_progress  = 0.0;

    bool m_loadMode = false;
};

#endif // PROGRESSBAR_H