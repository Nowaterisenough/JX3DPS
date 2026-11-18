#ifndef DESKTOP_COMBAT_HISTORY_H
#define DESKTOP_COMBAT_HISTORY_H

#include <optional>

#include <QMetaType>

#include "runtime_catalog.h"

namespace desktop {
struct ActorAttributes
{
    JX3DPS::value_t attack = 0, weapon = 0, overcome = 0, strain = 0, surplus = 0;
    double          criticalChance = 0, criticalMultiplier = 1.75, haste = 1;
    int             damagePercent = 0, strainPercent = 0, pvePercent = 0, ignoreShield = 0;
    rt::Slot        lastSkill = rt::INVALID_SLOT;
    int             style = -1, shadows = 0;
    bool            operator==(const ActorAttributes &) const = default;
};

struct AttributeSample
{
    int             frame = 0;
    ActorAttributes values;
    bool            operator==(const AttributeSample &) const = default;
};

// Incremental, value-owned packets; no Qt GUI objects cross the worker boundary.
struct HistoryChunk
{
    Specialization                 specialization = Specialization::TaiXu;
    std::optional<rt::State>       initial;
    std::vector<rt::StateMutation> mutations;
    std::vector<AttributeSample>   attributes;
    int                            observedFrame = 0;
    bool                           finished      = false;
};

struct HistoricalState
{
    rt::State       state;
    ActorAttributes attributes;
};

class CombatHistory
{
public:
    void                           Clear();
    void                           Append(const HistoryChunk &chunk);
    std::optional<HistoricalState> At(int frame) const;

    int ObservedFrame() const { return m_observed; }

    Specialization Class() const { return m_specialization; }

    bool Empty() const { return m_checkpoints.empty(); }

    bool Finished() const { return m_finished; }

private:
    struct Checkpoint
    {
        std::size_t count;
        rt::State   state;
    };

    Specialization                 m_specialization = Specialization::TaiXu;
    int                            m_observed       = 0;
    bool                           m_finished       = false;
    rt::State                      m_tail;
    std::vector<Checkpoint>        m_checkpoints;
    std::vector<rt::StateMutation> m_mutations;
    std::vector<AttributeSample>   m_attributes;
};
} // namespace desktop
Q_DECLARE_METATYPE(desktop::HistoryChunk)
#endif
