#pragma once

#include <condition_variable>
#include <mutex>
#include <thread>

#include "fight_timing.hpp"

namespace JX3DPS::runtime {

// The sampler only advances a clock. Workers capture their own active call
// path at the next marked boundary; paths remain thread-local until join.
class BatchProfile
{
public:
    explicit BatchProfile(unsigned count) :
        workers(count), sampler_([this] {
            std::uint32_t    random = 42;
            std::unique_lock lock(mutex_);
            while (!stop_) {
                random         = (random * 1664525u) + 1013904223u;
                const auto DUE = std::chrono::steady_clock::now() + std::chrono::microseconds(1000 + (random >> 22));
                if (wake_.wait_until(lock, DUE, [this] { return stop_; })) {
                    break;
                }
                tick.fetch_add(1, std::memory_order_relaxed);
            }
        })
    {
    }

    ~BatchProfile()
    {
        {
            const std::lock_guard LOCK(mutex_);
            stop_ = true;
        }
        wake_.notify_one();
        sampler_.join();
    }

    std::vector<WorkerProfile> workers;
    std::atomic<std::uint64_t> tick{ 0 };

private:
    std::mutex              mutex_;
    std::condition_variable wake_;
    bool                    stop_ = false;
    std::thread             sampler_;
};

template <bool Measure>
struct WorkerMeasurement
{
    WorkerMeasurement(BatchProfile *, unsigned, const char *) { }

    void Finish() { }
};

template <>
class WorkerMeasurement<true>
{
public:
    WorkerMeasurement(BatchProfile *profile, unsigned index, const char *stage) :
        worker_(profile->workers[index]), root_{ nullptr, stage, FightPhase::OTHER }, started_(Clock::now())
    {
        worker_.tick          = &profile->tick;
        worker_.lastTick      = profile->tick.load(std::memory_order_relaxed);
        worker_.top           = &root_;
        WorkerProfile::active = &worker_;
    }

    ~WorkerMeasurement() { Finish(); }

    WorkerMeasurement(const WorkerMeasurement &) = delete;

    void Finish()
    {
        if (worker_.top == nullptr) {
            return;
        }
        worker_.Poll();
        worker_.elapsed       += Clock::now() - started_;
        worker_.top            = nullptr;
        WorkerProfile::active  = nullptr;
    }

private:
    using Clock = std::chrono::steady_clock;
    WorkerProfile    &worker_;
    ProfileFrame      root_;
    Clock::time_point started_;
};

} // namespace JX3DPS::runtime
