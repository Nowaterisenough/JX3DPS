#ifndef JX3DPS_RUNTIME_SCHEDULER_HPP
#define JX3DPS_RUNTIME_SCHEDULER_HPP

#include "../runtime.hpp"
#include "fight_timing.hpp"
#include <optional>
#include <stdexcept>

namespace JX3DPS::runtime {

enum class TimerKind : std::uint8_t { SkillCooldown, SkillPrepare, SkillCasting, BuffDuration, BuffInterval, MacroWake, Field, ResourceRegen, TeamApplication, TargetChange };
struct TimerHandle {
    TimerKind kind = TimerKind::SkillCooldown;
    Slot slot = INVALID_SLOT;
    bool target = false;
};

// One heap entry per timer. Re-arming neither allocates nor retains stale
// entries, even when a buff is refreshed millions of times before expiry.
template<bool Measure>
class BasicDeadlineQueue {
public:
    using TimerId = std::uint32_t;
    static constexpr TimerId INVALID_TIMER = std::numeric_limits<TimerId>::max();

    TimerId Create(TimerHandle handle, std::uint16_t priority = 0) {
        if (timers_.size() >= INVALID_TIMER) throw std::length_error("too many timers");
        auto id = static_cast<TimerId>(timers_.size());
        timers_.push_back({priority, 0, INVALID_TIMER, handle});
        heap_.reserve(timers_.size());
        return id;
    }

    void Arm(TimerId id, tick_t delay) {
        if (delay < 0 || delay > std::numeric_limits<tick_t>::max() - current_) {
            throw std::out_of_range("timer delay is out of range");
        }
        ArmAt(id, current_ + delay);
    }
    void ArmAt(TimerId id, tick_t deadline) {
        PhaseScope<Measure> phase(FightPhase::Queue, "DeadlineQueue::ArmAt");
        if (deadline < current_) throw std::out_of_range("timer is in the past");
        auto &timer = timers_.at(id);
        const auto previous = timer.deadline_priority;
        timer.deadline_priority = (static_cast<std::uint64_t>(deadline) << 16) | static_cast<std::uint16_t>(previous);
        timer.order = sequence_++;
        if (timer.position == INVALID_TIMER) {
            const auto pos = static_cast<TimerId>(heap_.size());
            heap_.push_back(id);
            SiftUp(pos, id);
        } else if (timer.deadline_priority < previous) {
            SiftUp(timer.position, id);
        } else {
            // Equal deadlines get a later insertion order when re-armed.
            SiftDown(timer.position, id);
        }
    }
    void Cancel(TimerId id) {
        PhaseScope<Measure> phase(FightPhase::Queue, "DeadlineQueue::Cancel");
        auto &timer = timers_.at(id);
        if (timer.position != INVALID_TIMER) Remove(timer.position);
    }
    std::optional<tick_t> NextDelta() const {
        PhaseScope<Measure> frame("DeadlineQueue::NextDelta");
        if (heap_.empty()) return std::nullopt;
        return std::max<tick_t>(0, timers_[heap_.front()].Deadline() - current_);
    }
    void Advance(tick_t delta) {
        PhaseScope<Measure> frame("DeadlineQueue::Advance");
        if (delta < 0 || delta > std::numeric_limits<tick_t>::max() - current_) {
            throw std::out_of_range("clock advance is out of range");
        }
        if (auto next = NextDelta(); next && delta > *next) throw std::logic_error("cannot skip a pending event");
        current_ += delta;
    }

    template <typename Fn>
    void DispatchDue(Fn &&fn, std::size_t budget = 100000) {
        PhaseScope<Measure> frame("DeadlineQueue::DispatchDue");
        std::size_t dispatched = 0;
        while (!heap_.empty() && timers_[heap_.front()].Deadline() <= current_) {
            if (dispatched++ == budget) throw std::runtime_error("same-frame event budget exceeded");
            const auto id = heap_.front();
            const auto handle = timers_[id].handle;
            Remove(0);
            fn(handle, id);
        }
    }
    tick_t Now() const { return current_; }
    std::size_t Pending() const { return heap_.size(); }
    void Reset() {
        current_ = 0;
        sequence_ = 0;
        heap_.clear();
        for (auto &timer : timers_) timer.position = INVALID_TIMER;
    }

private:
    struct Timer {
        // Unsigned (absolute frame, priority), in lexicographic order.
        std::uint64_t deadline_priority;
        std::uint64_t order;
        TimerId position;
        TimerHandle handle;
        tick_t Deadline() const { return static_cast<tick_t>(deadline_priority >> 16); }
    };
    bool Before(TimerId lhs, TimerId rhs) const {
        const auto &a = timers_[lhs];
        const auto &b = timers_[rhs];
        if (a.deadline_priority != b.deadline_priority) return a.deadline_priority < b.deadline_priority;
        return a.order < b.order;
    }
    void Place(TimerId pos, TimerId id) {
        heap_[pos] = id;
        timers_[id].position = pos;
    }
    void SiftUp(TimerId pos, TimerId id) {
        PhaseScope<Measure> frame("DeadlineQueue::SiftUp");
        while (pos > 0) {
            const auto parent = (pos - 1) / 2;
            const auto parent_id = heap_[parent];
            if (!Before(id, parent_id)) break;
            Place(pos, parent_id);
            pos = parent;
        }
        Place(pos, id);
    }
    void SiftDown(TimerId pos, TimerId id) {
        PhaseScope<Measure> frame("DeadlineQueue::SiftDown");
        while (pos < heap_.size() / 2) {
            auto child = pos * 2 + 1;
            if (child + 1 < heap_.size() && Before(heap_[child + 1], heap_[child])) ++child;
            const auto child_id = heap_[child];
            if (!Before(child_id, id)) break;
            Place(pos, child_id);
            pos = child;
        }
        Place(pos, id);
    }
    void Remove(TimerId pos) {
        PhaseScope<Measure> frame("DeadlineQueue::Remove");
        const auto removed = heap_[pos];
        const auto moved = heap_.back();
        heap_.pop_back();
        timers_[removed].position = INVALID_TIMER;
        if (pos < heap_.size()) {
            if (pos > 0 && Before(moved, heap_[(pos - 1) / 2]))
                SiftUp(pos, moved);
            else
                SiftDown(pos, moved);
        }
    }

    tick_t current_ = 0;
    std::uint64_t sequence_ = 0;
    std::vector<Timer> timers_;
    std::vector<TimerId> heap_;
};

using DeadlineQueue = BasicDeadlineQueue<false>;

} // namespace JX3DPS::runtime
#endif
