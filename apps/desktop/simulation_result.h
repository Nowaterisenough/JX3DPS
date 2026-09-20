#ifndef DESKTOP_SIMULATION_RESULT_H
#define DESKTOP_SIMULATION_RESULT_H

#include <QMetaType>
#include <QString>
#include <QVector>
#include "src/core/runtime/batch.hpp"

// Emitted once after a run; no per-fight Qt objects are created by batch workers.
struct AttributeGain {
    QString attribute;
    int increment = 100;
    JX3DPS::runtime::BatchStats stats;
};
struct SimulationResult {
    JX3DPS::runtime::BatchStats stats;
    JX3DPS::runtime::BatchOptions options;
    double elapsedSeconds = 0;
    bool trace = false;
    QVector<AttributeGain> gains;
    bool gainsRequested = false;

    bool Complete() const { return stats.count == options.iterations && (!gainsRequested || gains.size() == 8); }
    double FightSeconds() const { return options.duration / 16.0; }
    double MeanDps() const { return stats.mean_damage / FightSeconds(); }
};

Q_DECLARE_METATYPE(SimulationResult)

#endif
