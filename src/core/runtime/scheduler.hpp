#ifndef JX3DPS_RUNTIME_SCHEDULER_HPP
#define JX3DPS_RUNTIME_SCHEDULER_HPP

#include "../runtime.hpp"
#include <optional>
#include <stdexcept>

namespace JX3DPS::runtime {

enum class TimerKind : std::uint8_t { SkillCooldown, SkillPrepare, SkillCasting, BuffDuration, BuffInterval, MacroWake, Field, ResourceRegen };
struct TimerHandle {
    TimerKind kind = TimerKind::SkillCooldown;
    Slot slot = INVALID_SLOT;
    bool target = false;
};

// One heap entry per timer. Re-arming neither allocates nor retains stale
// entries, even when a buff is refreshed millions of times before expiry.
class DeadlineQueue {
public:
    using TimerId = std::uint32_t;
    static constexpr TimerId INVALID_TIMER = std::numeric_limits<TimerId>::max();

    TimerId Create(TimerHandle handle, std::uint16_t priority = 0) {
        if (timers_.size() >= INVALID_TIMER) throw std::length_error("too many timers");
        auto id = static_cast<TimerId>(timers_.size());
        timers_.push_back({handle, 0, 0, INVALID_TIMER, priority});
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
        if (deadline < current_) throw std::out_of_range("timer is in the past");
        auto &timer = timers_.at(id);
        timer.deadline = deadline;
        timer.order = sequence_++;
        if (timer.position == INVALID_TIMER) {
            timer.position = static_cast<TimerId>(heap_.size());
            heap_.push_back(id);
        }
        Repair(timer.position);
    }
    void Cancel(TimerId id) {
        auto &timer = timers_.at(id);
        if (timer.position != INVALID_TIMER) Remove(timer.position);
    }
    std::optional<tick_t> NextDelta() const {
        if (heap_.empty()) return std::nullopt;
        return std::max<tick_t>(0, timers_[heap_.front()].deadline - current_);
    }
    void Advance(tick_t delta) {
        if (delta < 0 || delta > std::numeric_limits<tick_t>::max() - current_) {
            throw std::out_of_range("clock advance is out of range");
        }
        if (auto next = NextDelta(); next && delta > *next) throw std::logic_error("cannot skip a pending event");
        current_ += delta;
    }

    template <typename Fn>
    void DispatchDue(Fn &&fn, std::size_t budget = 100000) {
        std::size_t dispatched = 0;
        while (!heap_.empty() && timers_[heap_.front()].deadline <= current_) {
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
        TimerHandle handle;
        tick_t deadline;
        std::uint64_t order;
        TimerId position;
        std::uint16_t priority;
    };
    bool Before(TimerId lhs, TimerId rhs) const {
        const auto &a = timers_[lhs];
        const auto &b = timers_[rhs];
        if (a.deadline != b.deadline) return a.deadline < b.deadline;
        if (a.priority != b.priority) return a.priority < b.priority;
        return a.order < b.order;
    }
    void Swap(TimerId a, TimerId b) {
        std::swap(heap_[a], heap_[b]);
        timers_[heap_[a]].position = a;
        timers_[heap_[b]].position = b;
    }
    void Repair(TimerId pos) {
        while (pos > 0 && Before(heap_[pos], heap_[(pos - 1) / 2])) {
            auto parent = (pos - 1) / 2;
            Swap(pos, parent);
            pos = parent;
        }
        for (;;) {
            const auto left = std::size_t(pos) * 2 + 1;
            if (left >= heap_.size()) break;
            auto best = static_cast<TimerId>(left);
            if (left + 1 < heap_.size() && Before(heap_[left + 1], heap_[left])) ++best;
            if (!Before(heap_[best], heap_[pos])) break;
            Swap(pos, best);
            pos = best;
        }
    }
    void Remove(TimerId pos) {
        const auto removed = heap_[pos];
        const auto moved = heap_.back();
        heap_.pop_back();
        timers_[removed].position = INVALID_TIMER;
        if (pos < heap_.size()) {
            heap_[pos] = moved;
            timers_[moved].position = pos;
            Repair(pos);
        }
    }

    tick_t current_ = 0;
    std::uint64_t sequence_ = 0;
    std::vector<Timer> timers_;
    std::vector<TimerId> heap_;
};

} // namespace JX3DPS::runtime
#endif
