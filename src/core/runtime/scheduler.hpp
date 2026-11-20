#ifndef JX3DPS_RUNTIME_SCHEDULER_HPP
#define JX3DPS_RUNTIME_SCHEDULER_HPP

#include <optional>
#include <stdexcept>

#include "../runtime.hpp"

#include "fight_timing.hpp"

namespace JX3DPS::runtime {

enum class TimerKind : std::uint8_t
{
    SKILL_COOLDOWN,
    SKILL_PREPARE,
    SKILL_CASTING,
    BUFF_DURATION,
    BUFF_INTERVAL,
    MACRO_WAKE,
    FIELD,
    RESOURCE_REGEN,
    TEAM_APPLICATION,
    TARGET_CHANGE
};

struct TimerHandle
{
    TimerKind kind   = TimerKind::SKILL_COOLDOWN;
    Slot      slot   = INVALID_SLOT;
    bool      target = false;
};

// One heap entry per timer. Re-arming neither allocates nor retains stale
// entries, even when a buff is refreshed millions of times before expiry.
template <bool Measure>
class BasicDeadlineQueue
{
public:
    using TimerId                          = std::uint32_t;
    static constexpr TimerId INVALID_TIMER = std::numeric_limits<TimerId>::max();

    BasicDeadlineQueue()                           = default;
    BasicDeadlineQueue(const BasicDeadlineQueue &) = default;

    BasicDeadlineQueue(BasicDeadlineQueue &&other) noexcept { Swap(other); }

    BasicDeadlineQueue &operator=(BasicDeadlineQueue other) noexcept
    {
        Swap(other);
        return *this;
    }

    TimerId Create(TimerHandle handle, std::uint16_t priority = 0)
    {
        if (timers_.size() >= INVALID_TIMER) {
            throw std::length_error("too many timers");
        }
        const auto ID = static_cast<TimerId>(timers_.size());
        // Each registered timer can occupy at most one heap slot. Size the
        // storage here so rearming only writes the final entry in SiftUp.
        heap_.resize(timers_.size() + 1);
        timers_.push_back({ 0, INVALID_TIMER, priority, handle });
        return ID;
    }

    void Arm(TimerId id, tick_t delay)
    {
        if (delay < 0 || delay > std::numeric_limits<tick_t>::max() - current_) {
            throw std::out_of_range("timer delay is out of range");
        }
        ArmAt(id, current_ + delay);
    }

    void ArmAt(TimerId id, tick_t deadline)
    {
        const PhaseScope<Measure> PHASE(FightPhase::QUEUE, "DeadlineQueue::ArmAt");
        if (deadline < current_) {
            throw std::out_of_range("timer is in the past");
        }
        auto &timer     = timers_.at(id);
        timer.order     = sequence_++;
        // The common case fits in one comparison. Saturation preserves ordering
        // across long fights; equal saturated keys use the full insertion order.
        const auto RANK = std::min(timer.order, std::uint64_t{ 0xffff });
        const Entry ENTRY{ (static_cast<std::uint64_t>(deadline) << 32) | (static_cast<std::uint64_t>(timer.priority) << 16) | RANK, id };
        const auto POS = timer.position;
        if (POS == INVALID_TIMER) {
            SiftUp(pending_++, ENTRY);
        } else if (ENTRY.key < heap_[POS].key) {
            SiftUp(POS, ENTRY);
        } else {
            SiftDown(POS, ENTRY);
        }
    }

    void Cancel(TimerId id)
    {
        const PhaseScope<Measure> PHASE(FightPhase::QUEUE, "DeadlineQueue::Cancel");
        auto                     &timer = timers_.at(id);
        if (timer.position != INVALID_TIMER) {
            Remove(timer.position);
        }
    }

    std::optional<tick_t> NextDelta() const
    {
        const PhaseScope<Measure> FRAME("DeadlineQueue::NextDelta");
        if (pending_ == 0u) {
            return std::nullopt;
        }
        return std::max<tick_t>(0, heap_.front().Deadline() - current_);
    }

    void Advance(tick_t delta)
    {
        const PhaseScope<Measure> FRAME("DeadlineQueue::Advance");
        if (delta < 0 || delta > std::numeric_limits<tick_t>::max() - current_) {
            throw std::out_of_range("clock advance is out of range");
        }
        if (auto next = NextDelta(); next && delta > *next) {
            throw std::logic_error("cannot skip a pending event");
        }
        current_ += delta;
    }

    template <typename Fn>
    void DispatchDue(Fn &&fn, std::size_t budget = 100000)
    {
        const PhaseScope<Measure> FRAME("DeadlineQueue::DispatchDue");
        std::size_t               dispatched = 0;
        while (pending_ && heap_.front().Deadline() <= current_) {
            if (dispatched++ == budget) {
                throw std::runtime_error("same-frame event budget exceeded");
            }
            const auto ID     = heap_.front().id;
            const auto HANDLE = timers_[ID].handle;
            Remove(0);
            fn(HANDLE, ID);
        }
    }

    tick_t Now() const { return current_; }

    std::size_t Pending() const { return pending_; }

    void Reset()
    {
        current_  = 0;
        sequence_ = 0;
        pending_  = 0;
        for (auto &timer : timers_) {
            timer.position = INVALID_TIMER;
        }
    }

private:
    void Swap(BasicDeadlineQueue &other) noexcept
    {
        std::swap(current_, other.current_);
        std::swap(sequence_, other.sequence_);
        std::swap(pending_, other.pending_);
        timers_.swap(other.timers_);
        heap_.swap(other.heap_);
    }

    struct Timer
    {
        std::uint64_t order{};
        TimerId       position{};
        std::uint16_t priority{};
        TimerHandle   handle;
    };

    struct Entry
    {
        std::uint64_t key; // Absolute frame, priority, then saturated insertion order.
        TimerId       id;

        tick_t Deadline() const { return static_cast<tick_t>(key >> 32); }
    };

    bool Before(Entry a, Entry b) const
    {
        if (a.key != b.key) {
            return a.key < b.key;
        }
        return timers_[a.id].order < timers_[b.id].order;
    }

    void Place(TimerId pos, Entry entry)
    {
        heap_[pos]                 = entry;
        timers_[entry.id].position = pos;
    }

    void SiftUp(TimerId pos, Entry entry)
    {
        const PhaseScope<Measure> FRAME("DeadlineQueue::SiftUp");
        while (pos > 0) {
            const auto parent       = (pos - 1) / 2;
            const auto PARENT_ENTRY = heap_[parent];
            if (!Before(entry, PARENT_ENTRY)) {
                break;
            }
            Place(pos, PARENT_ENTRY);
            pos = parent;
        }
        Place(pos, entry);
    }

    void SiftDown(TimerId pos, Entry entry)
    {
        const PhaseScope<Measure> FRAME("DeadlineQueue::SiftDown");
        while (pos < pending_ / 2) {
            auto child = (pos * 2) + 1;
            if (child + 1 < pending_ && Before(heap_[child + 1], heap_[child])) {
                ++child;
            }
            const auto CHILD_ENTRY = heap_[child];
            if (!Before(CHILD_ENTRY, entry)) {
                break;
            }
            Place(pos, CHILD_ENTRY);
            pos = child;
        }
        Place(pos, entry);
    }

    void Remove(TimerId pos)
    {
        const PhaseScope<Measure> FRAME("DeadlineQueue::Remove");
        const auto                REMOVED = heap_[pos];
        const auto                MOVED   = heap_[--pending_];
        timers_[REMOVED.id].position      = INVALID_TIMER;
        if (pos < pending_) {
            if (pos > 0 && Before(MOVED, heap_[(pos - 1) / 2])) {
                SiftUp(pos, MOVED);
            } else {
                SiftDown(pos, MOVED);
            }
        }
    }

    tick_t             current_  = 0;
    TimerId            pending_  = 0;
    std::uint64_t      sequence_ = 0;
    std::vector<Timer> timers_;
    std::vector<Entry> heap_;
};

using DeadlineQueue = BasicDeadlineQueue<false>;

} // namespace JX3DPS::runtime
#endif
