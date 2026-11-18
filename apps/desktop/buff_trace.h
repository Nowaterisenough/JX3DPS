#ifndef DESKTOP_BUFF_TRACE_H
#define DESKTOP_BUFF_TRACE_H

#include "runtime_catalog.h"
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QVector>
#include <vector>

namespace JX3DPS::runtime { struct State; struct StateMutation; }

struct BuffEvent {
    int frame = 0;
    quint32 sequence = 0;
    int slot = 0;
    bool target = false;
    QString name;
    int stacks = 0;
    int expiresAt = 0;
    int tickAt = 0;
    bool initial = false;
    bool operator==(const BuffEvent &) const = default;
};
using BuffEvents = QVector<BuffEvent>;
Q_DECLARE_METATYPE(BuffEvents)

BuffEvents InitialBuffEvents(const JX3DPS::runtime::State &state, desktop::Specialization specialization = desktop::Specialization::TaiXu);
BuffEvents ReadBuffEvents(const std::vector<JX3DPS::runtime::StateMutation> &mutations, std::size_t &cursor, desktop::Specialization specialization = desktop::Specialization::TaiXu);

struct BuffInterval {
    int start = 0, end = 0, stacks = 0;
    bool operator==(const BuffInterval &) const = default;
};
struct BuffChange {
    BuffEvent event;
    int before = 0;
    QString kind;
};
struct BuffCoverage {
    BuffEvent latest;
    QVector<BuffInterval> intervals;
    qint64 activeFrames = 0;
    qint64 stackFrames = 0;
    int peakStacks = 0;
    int changes = 0;
};

// Desktop-only projection of the existing log; the simulation owns all state.
class BuffTrace {
public:
    void Reset(int duration);
    void Append(const BuffEvents &events);
    void Observe(int frame);
    int ObservedFrame() const { return m_observed; }
    int Duration() const { return m_duration; }
    const QVector<BuffChange> &Changes() const { return m_changes; }
    QVector<BuffCoverage> Coverage() const;

private:
    QMap<int, BuffCoverage> m_tracks;
    QVector<BuffChange> m_changes;
    int m_duration = 0;
    int m_observed = 0;
    static void Extend(BuffCoverage &track, int until);
};

#endif
