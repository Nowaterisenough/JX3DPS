#pragma once

#include <optional>
#include <span>

#include "../runtime.hpp"

namespace JX3DPS::runtime {
inline constexpr std::size_t MaximumTargetEvents = 256;
inline constexpr tick_t      MaximumFlowFrame    = 86400 * FRAMES_PER_SECOND;

struct TargetEvent
{
    tick_t             frame = 0;
    std::optional<int> life_basis_points;
    std::optional<int> distance_hundredths;
    bool               operator==(const TargetEvent &) const = default;
};

inline void ValidateTargetEvents(std::span<const TargetEvent> events)
{
    if (events.size() > MaximumTargetEvents) {
        throw std::invalid_argument("too many target timeline events");
    }
    tick_t previous = 0;
    for (const auto &event : events) {
        if (event.frame <= 0 || event.frame > MaximumFlowFrame || event.frame < previous ||
            (!event.life_basis_points && !event.distance_hundredths) ||
            (event.life_basis_points && (*event.life_basis_points < 100 || *event.life_basis_points > 10000)) ||
            (event.distance_hundredths && (*event.distance_hundredths < 0 || *event.distance_hundredths > 100000)))
        {
            throw std::invalid_argument("invalid target timeline event");
        }
        previous = event.frame;
    }
}

// One reusable timer per fight, no allocations or source parsing during dispatch.
class TargetTimeline
{
public:
    template <class Engine>
    void Setup(Engine &engine, std::span<const TargetEvent> events)
    {
        if (!events.empty()) {
            timer_ = engine.Timers().Create({ TimerKind::TARGET_CHANGE }, 0);
        }
    }

    template <class Engine>
    void Reset(Engine &engine, std::span<const TargetEvent> events)
    {
        cursor_ = 0;
        if (!events.empty()) {
            engine.Timers().ArmAt(timer_, events.front().frame);
        }
    }

    template <class Engine>
    void Dispatch(Engine &engine, std::span<const TargetEvent> events)
    {
        auto &state = engine.MutableState();
        while (cursor_ < events.size() && events[cursor_].frame == state.now) {
            const auto &event = events[cursor_++];
            if (event.life_basis_points) {
                engine.Log().RecordTarget(state.now, MutationKind::TARGET_LIFE, *event.life_basis_points);
                state.target_life = *event.life_basis_points / 10000.0;
            }
            if (event.distance_hundredths) {
                engine.Log().RecordTarget(state.now, MutationKind::TARGET_DISTANCE, *event.distance_hundredths);
                state.target_distance = *event.distance_hundredths / 100.0;
            }
        }
        if (cursor_ < events.size()) {
            engine.Timers().ArmAt(timer_, events[cursor_].frame);
        }
    }

private:
    DeadlineQueue::TimerId timer_  = DeadlineQueue::INVALID_TIMER;
    std::size_t            cursor_ = 0;
};
} // namespace JX3DPS::runtime
