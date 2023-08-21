/**
 * Project: JX3DPS
 * File: ProgressBar.h
 * Description:
 * Created Date: 2023-06-30 23:40:42
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-13 12:35:24
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_STATS_WIDGET_H__
#define __JX3DPS_STATS_WIDGET_H__

#include <QDialog>

#include <nlohmann/json.hpp>

#include "Widget/Widget.h"

namespace JX3DPS {

namespace Simulator {

class StatsWidget : public Widget
{
    Q_OBJECT

public:
    StatsWidget(QWidget *parent = nullptr);

signals:
    void Signal_UpdateStats(const nlohmann::ordered_json &stats);
    void Signal_UpdateTargetStats();
    void Signal_UpdateTargetEffectStats(const std::string &targetName, long long targetDamage);
    void Signal_UpdateTargetEffectRollStats(const std::string &targetName,
                                            const std::string &effectName,
                                            long long          effectDamage,
                                            int                effectCount);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // FramelessWidget *m_frameless = nullptr;

    nlohmann::ordered_json m_stats;

    int m_simIterations = 0;
};

} // namespace Simulator

} // namespace JX3DPS

#endif // __JX3DPS_STATS_WIDGET_H__