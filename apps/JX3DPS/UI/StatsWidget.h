/**
 * Project: JX3DPS
 * File: ProgressBar.h
 * Description:
 * Created Date: 2023-06-30 23:40:42
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-13 23:49:27
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QDialog>

#include <nlohmann/json.hpp>

#include "FramelessWidget.h"

class StatsWidget : public QDialog
{
    Q_OBJECT

public:
    StatsWidget(QWidget *parent = nullptr);

signals:
    void Signal_UpdateStats(const nlohmann::json &stats);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    FramelessWidget *m_frameless = nullptr;

    nlohmann::json m_stats;
};

#endif // STATSWIDGET_H