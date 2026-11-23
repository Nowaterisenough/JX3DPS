#include "buff_trace.h"

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "src/core/runtime/damage_log.hpp"

namespace rt = JX3DPS::runtime;

namespace {
QString BuffName(int slot, desktop::Specialization specialization)
{
    const auto NAME = desktop::Describe(specialization).buffs[slot].name;
    return desktop::DisplayName(NAME);
}
} // namespace

BuffEvents InitialBuffEvents(const rt::State &state, desktop::Specialization specialization)
{
    BuffEvents events;
    for (int slot = 0; std::cmp_less(slot, desktop::Describe(specialization).buffs.size()); ++slot) {
        for (const bool target : { false, true }) {
            const auto STACKS = (target ? state.target_buff_stacks : state.self_buff_stacks).at(slot);
            if (STACKS == 0) {
                continue;
            }
            events.push_back({ state.now,
                               0,
                               slot,
                               target,
                               BuffName(slot, specialization),
                               STACKS,
                               (target ? state.target_buff_expires_at : state.self_buff_expires_at).at(slot),
                               (target ? state.target_buff_tick_at : state.self_buff_tick_at).at(slot),
                               true });
        }
    }
    return events;
}

BuffEvents ReadBuffEvents(const std::vector<rt::StateMutation> &mutations, std::size_t &cursor, desktop::Specialization specialization)
{
    BuffEvents events;
    for (; cursor < mutations.size(); ++cursor) {
        const auto &mutation = mutations[cursor];
        if (mutation.kind != rt::MutationKind::BUFF) {
            continue;
        }
        events.push_back({ mutation.frame,
                           mutation.sequence,
                           mutation.slot,
                           mutation.target,
                           BuffName(mutation.slot, specialization),
                           mutation.c,
                           mutation.a,
                           mutation.b,
                           false });
    }
    return events;
}

void BuffTrace::Reset(int duration)
{
    m_tracks.clear();
    m_changes.clear();
    m_duration = std::max(0, duration);
    m_observed = 0;
}

void BuffTrace::Extend(BuffCoverage &track, int until)
{
    const auto &event = track.latest;
    const int   END   = event.expiresAt > 0 ? std::min(until, event.expiresAt) : until;
    if ((event.stacks == 0) || END <= event.frame) {
        return;
    }
    const auto FRAMES   = END - event.frame;
    track.activeFrames += FRAMES;
    track.stackFrames  += static_cast<qint64>(FRAMES) * event.stacks;
    if (!track.intervals.isEmpty() && track.intervals.back().end == event.frame &&
        track.intervals.back().stacks == event.stacks)
    {
        track.intervals.back().end = END;
    } else {
        track.intervals.push_back({ event.frame, END, event.stacks });
    }
}

void BuffTrace::Append(const BuffEvents &events)
{
    for (const auto &event : events) {
        if (event.frame < m_observed || event.frame > m_duration || event.stacks < 0) {
            throw std::invalid_argument("buff events must be within the unobserved fight interval");
        }
        if (!m_changes.isEmpty() &&
            (event.frame < m_changes.back().event.frame ||
             (!event.initial && !m_changes.back().event.initial && event.sequence <= m_changes.back().event.sequence)))
        {
            throw std::invalid_argument("buff event sequence must be increasing");
        }
        auto      &track    = m_tracks[(event.slot * 2) + static_cast<int>(event.target)];
        const auto PREVIOUS = track.latest;
        Extend(track, event.frame);
        QString kind;
        if (event.initial) {
            kind = QStringLiteral("初始");
        } else if (PREVIOUS.stacks == 0 && event.stacks > 0) {
            kind = QStringLiteral("获得");
        } else if (PREVIOUS.stacks > 0 && event.stacks == 0) {
            kind = QStringLiteral("消失");
        } else if (PREVIOUS.stacks != event.stacks) {
            kind = QStringLiteral("层数");
        } else if (PREVIOUS.expiresAt != event.expiresAt) {
            kind = QStringLiteral("刷新");
        } else {
            kind = QStringLiteral("计时更新");
        }
        m_changes.push_back({ event, PREVIOUS.stacks, kind });
        track.latest     = event;
        track.peakStacks = std::max(track.peakStacks, event.stacks);
        if (!event.initial) {
            ++track.changes;
        }
    }
}

void BuffTrace::Observe(int frame)
{
    const int next = std::clamp(frame, m_observed, m_duration);
    if (!m_changes.isEmpty() && next < m_changes.back().event.frame) {
        throw std::invalid_argument("buff observation precedes the latest event");
    }
    m_observed = next;
}

QVector<BuffCoverage> BuffTrace::Coverage() const
{
    QVector<BuffCoverage> result;
    result.reserve(m_tracks.size());
    for (const auto &track : m_tracks) {
        auto current = track;
        Extend(current, m_observed);
        result.push_back(std::move(current));
    }
    return result;
}
