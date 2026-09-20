#include "combat_history.h"

namespace desktop {
void CombatHistory::Clear()
{
    m_checkpoints.clear();
    m_mutations.clear();
    m_attributes.clear();
    m_tail     = {};
    m_observed = 0;
    m_finished = false;
}

void CombatHistory::Append(const HistoryChunk &chunk)
{
    if (chunk.initial) {
        Clear();
        m_specialization = chunk.specialization;
        m_tail           = *chunk.initial;
        m_checkpoints.push_back({ 0, m_tail });
    }
    if (Empty()) {
        return;
    }
    for (const auto &mutation : chunk.mutations) {
        m_mutations.push_back(mutation);
        rt::DamageIntentLog::Apply(m_tail, mutation);
        m_tail.now = mutation.frame;
        if (m_mutations.size() % 256 == 0) {
            m_checkpoints.push_back({ m_mutations.size(), m_tail });
        }
    }
    m_attributes.insert(m_attributes.end(), chunk.attributes.begin(), chunk.attributes.end());
    m_observed = chunk.observedFrame;
    m_finished = chunk.finished;
}

std::optional<HistoricalState> CombatHistory::At(int frame) const
{
    if (Empty() || frame < 0 || frame > m_observed || m_attributes.empty()) {
        return {};
    }
    const auto  end = std::upper_bound(m_mutations.begin(), m_mutations.end(), frame, [](int f, const auto &mutation) {
        return f < mutation.frame;
    });
    const auto  count      = static_cast<std::size_t>(end - m_mutations.begin());
    const auto &checkpoint = m_checkpoints[count / 256];
    HistoricalState result{ checkpoint.state, {} };
    for (auto i = checkpoint.count; i < count; ++i) {
        rt::DamageIntentLog::Apply(result.state, m_mutations[i]);
    }
    const auto attributes = std::upper_bound(m_attributes.begin(), m_attributes.end(), frame, [](int f, const auto &sample) {
        return f < sample.frame;
    });
    if (attributes == m_attributes.begin()) {
        return {};
    }
    result.attributes       = std::prev(attributes)->values;
    result.state.last_skill = result.attributes.lastSkill;
    result.state.now        = frame;
    return result;
}
} // namespace desktop
