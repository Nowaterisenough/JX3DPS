#ifndef DESKTOP_SIMULATION_RESULT_H
#define DESKTOP_SIMULATION_RESULT_H

#include <QMetaType>
#include <QString>
#include <QVector>
#include "src/core/runtime/batch.hpp"
#include "attribute_gain_mode.h"

// Emitted once after a run; no per-fight Qt objects are created by batch workers.
struct AttributeGain {
    QString attribute;
    int increment = 100;
    JX3DPS::runtime::BatchStats stats;
};
struct SkillStatistics {
    int skill = 0;
    QString name;
    qint64 damage = 0;
    quint64 count = 0, hit = 0, critical = 0, insight = 0, miss = 0;
};
struct FightHotspots {
    struct Call {
        QString function;
        unsigned parent = 0;
        quint64 inclusive = 0, self = 0;
    };
    QVector<Call> calls;
    struct Worker { quint64 fights, samplePoints; };
    QVector<Worker> workers;
    quint64 fights = 0, samplePoints = 0;
    double workerMs = 0;
    std::array<double, static_cast<std::size_t>(JX3DPS::runtime::FightPhase::Count)> phaseUs{};
};
struct SimulationResult {
    JX3DPS::runtime::BatchStats stats;
    JX3DPS::runtime::BatchOptions options;
    double elapsedSeconds = 0;
    bool trace = false;
    QVector<AttributeGain> gains;
    bool gainsRequested = false;
    QVector<SkillStatistics> skills;
    bool skillsCollected = false;
    desktop::GainComparisonMode gainMode = desktop::GainComparisonMode::SameValue;
    std::optional<FightHotspots> hotspots;

    bool Complete() const { return stats.count == options.iterations && (!gainsRequested || gains.size() == 8); }
    double FightSeconds() const { return options.duration / 16.0; }
    double MeanDps() const { return stats.mean_damage / FightSeconds(); }
};

Q_DECLARE_METATYPE(SimulationResult)

#endif
