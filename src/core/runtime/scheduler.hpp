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
        heap_.reserve(timers_.size() + 1);
        timers_.push_back({0, INVALID_TIMER, priority, handle});
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
        timer.order = sequence_++;
        // The common case fits in one comparison. Saturation preserves ordering
        // across long fights; equal saturated keys use the full insertion order.
        const auto rank = std::min(timer.order, std::uint64_t{0xffff});
        const Entry entry{(static_cast<std::uint64_t>(deadline) << 32) |
                          (static_cast<std::uint64_t>(timer.priority) << 16) | rank, id};
        const auto pos = timer.position;
        if (pos == INVALID_TIMER) {
            const auto end = static_cast<TimerId>(heap_.size());
            heap_.push_back(entry);
            SiftUp(end, entry);
        } else if (entry.key < heap_[pos].key) {
            SiftUp(pos, entry);
        } else {
            SiftDown(pos, entry);
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
        return std::max<tick_t>(0, heap_.front().Deadline() - current_);
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
        while (!heap_.empty() && heap_.front().Deadline() <= current_) {
            if (dispatched++ == budget) throw std::runtime_error("same-frame event budget exceeded");
            const auto id = heap_.front().id;
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
        std::uint64_t order;
        TimerId position;
        std::uint16_t priority;
        TimerHandle handle;
    };
    struct Entry {
        std::uint64_t key; // Absolute frame, priority, then saturated insertion order.
        TimerId id;
        tick_t Deadline() const { return static_cast<tick_t>(key >> 32); }
    };
    bool Before(Entry a, Entry b) const {
        if (a.key != b.key) return a.key < b.key;
        return timers_[a.id].order < timers_[b.id].order;
    }
    void Place(TimerId pos, Entry entry) {
        heap_[pos] = entry;
        timers_[entry.id].position = pos;
    }
    void SiftUp(TimerId pos, Entry entry) {
        PhaseScope<Measure> frame("DeadlineQueue::SiftUp");
        while (pos > 0) {
            const auto parent = (pos - 1) / 2;
            const auto parent_entry = heap_[parent];
            if (!Before(entry, parent_entry)) break;
            Place(pos, parent_entry);
            pos = parent;
        }
        Place(pos, entry);
    }
    void SiftDown(TimerId pos, Entry entry) {
        PhaseScope<Measure> frame("DeadlineQueue::SiftDown");
        while (pos < heap_.size() / 2) {
            auto child = pos * 2 + 1;
            if (child + 1 < heap_.size() && Before(heap_[child + 1], heap_[child])) ++child;
            const auto child_entry = heap_[child];
            if (!Before(child_entry, entry)) break;
            Place(pos, child_entry);
            pos = child;
        }
        Place(pos, entry);
    }
    void Remove(TimerId pos) {
        PhaseScope<Measure> frame("DeadlineQueue::Remove");
        const auto removed = heap_[pos];
        const auto moved = heap_.back();
        heap_.pop_back();
        timers_[removed.id].position = INVALID_TIMER;
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
    std::vector<Entry> heap_;
};

using DeadlineQueue = BasicDeadlineQueue<false>;

} // namespace JX3DPS::runtime
#endif
