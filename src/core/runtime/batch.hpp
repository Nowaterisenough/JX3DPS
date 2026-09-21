#ifndef JX3DPS_RUNTIME_BATCH_HPP
#define JX3DPS_RUNTIME_BATCH_HPP

#include "simulation.hpp"
#include <atomic>
#include <exception>
#include <span>
#include <thread>

namespace JX3DPS::runtime {

inline std::uint64_t IterationSeed(std::uint64_t base, std::uint64_t iteration) {
    auto value = base + 0x9e3779b97f4a7c15ULL * (iteration + 1);
    value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

struct BatchStats {
    std::uint64_t count = 0;
    double mean_damage = 0;
    double m2_damage = 0;
    value_t min_damage = std::numeric_limits<value_t>::max();
    value_t max_damage = 0;
    std::uint64_t checksum = 0;

    void Add(value_t damage, std::uint64_t iteration) {
        ++count;
        const double delta = static_cast<double>(damage) - mean_damage;
        mean_damage += delta / static_cast<double>(count);
        m2_damage += delta * (static_cast<double>(damage) - mean_damage);
        min_damage = std::min(min_damage, damage);
        max_damage = std::max(max_damage, damage);
        checksum ^= IterationSeed(static_cast<std::uint64_t>(damage), iteration);
    }
    void Merge(const BatchStats &other) {
        if (other.count == 0) return;
        if (count == 0) { *this = other; return; }
        const auto combined = count + other.count;
        const double delta = other.mean_damage - mean_damage;
        m2_damage += other.m2_damage + delta * delta * static_cast<double>(count) * static_cast<double>(other.count) / static_cast<double>(combined);
        mean_damage += delta * static_cast<double>(other.count) / static_cast<double>(combined);
        count = combined;
        min_damage = std::min(min_damage, other.min_damage);
        max_damage = std::max(max_damage, other.max_damage);
        checksum ^= other.checksum;
    }
    double PopulationDeviation() const { return count ? std::sqrt(m2_damage / static_cast<double>(count)) : 0; }
};

struct BatchOptions {
    std::uint64_t iterations = 1;
    std::uint64_t seed = 0;
    tick_t duration = 4800;
    unsigned workers = 1;
    std::size_t damage_capacity = 4096;
    std::size_t mutation_capacity = 8192;
};

// Owned by the caller for the duration of a controlled batch. Progress counts
// only completed fights; cancellation takes effect at the next fight boundary.
struct BatchControl {
    alignas(64) std::atomic<bool> cancel_requested{false};
    alignas(64) std::atomic<std::uint64_t> completed{0};
};

namespace detail {
template <bool Controlled, typename Rules>
BatchStats RunBatchImpl(const MacroProgram &program, const BatchOptions &options, const Rules &rules,
                       BatchControl *control) {
    if (!options.iterations || !options.workers || options.duration <= 0) {
        throw std::invalid_argument("batch iterations, workers and duration must be positive");
    }
    const auto worker_count = static_cast<unsigned>(std::min<std::uint64_t>(options.workers, options.iterations));
    struct alignas(64) WorkerResult { BatchStats stats; std::exception_ptr error; };
    std::vector<WorkerResult> results(worker_count);
    auto worker = [&](unsigned index) {
        std::uint64_t pending = 0;
        try {
            if constexpr (Controlled) {
                if (control->cancel_requested.load(std::memory_order_relaxed)) return;
            }
            Simulation<Rules> simulation(program, rules, options.damage_capacity, options.mutation_capacity);
            const auto quotient = options.iterations / worker_count;
            const auto remainder = options.iterations % worker_count;
            const auto start = quotient * index + std::min<std::uint64_t>(index, remainder);
            const auto end = start + quotient + (index < remainder ? 1 : 0);
            for (auto i = start; i < end; ++i) {
                if constexpr (Controlled) {
                    if (control->cancel_requested.load(std::memory_order_relaxed)) break;
                }
                simulation.Start(options.duration, IterationSeed(options.seed, i));
                simulation.Run();
                results[index].stats.Add(simulation.TotalDamage(), i);
                if constexpr (Controlled) {
                    if (++pending == 32) {
                        control->completed.fetch_add(pending, std::memory_order_relaxed);
                        pending = 0;
                    }
                }
            }
        } catch (...) {
            results[index].error = std::current_exception();
            if constexpr (Controlled) control->cancel_requested.store(true, std::memory_order_relaxed);
        }
        if constexpr (Controlled) control->completed.fetch_add(pending, std::memory_order_relaxed);
    };
    {
        std::vector<std::jthread> threads;
        threads.reserve(worker_count - 1);
        for (unsigned i = 1; i < worker_count; ++i) threads.emplace_back(worker, i);
        worker(0);
    }
    BatchStats combined;
    for (const auto &result : results) {
        if (result.error) std::rethrow_exception(result.error);
        combined.Merge(result.stats);
    }
    return combined;
}

// Run one primary batch while allowing the caller to reduce each completed
// fight's event log into additional statistics. The observer receives a
// reusable output span, so replay paths do not allocate per event or fight.
template <typename Rules, typename Observer>
std::pair<BatchStats, std::vector<BatchStats>> RunBatchObservedImpl(
    const MacroProgram &program, const BatchOptions &options, BatchControl &control,
    const Rules &rules, std::size_t observed_count, std::uint64_t progress_units,
    Observer observer) {
    if (!options.iterations || !options.workers || options.duration <= 0 || !progress_units)
        throw std::invalid_argument("batch iterations, workers and duration must be positive");
    const auto worker_count = static_cast<unsigned>(std::min<std::uint64_t>(options.workers, options.iterations));
    struct alignas(64) WorkerResult {
        BatchStats primary;
        std::vector<BatchStats> observed;
        std::exception_ptr error;
    };
    std::vector<WorkerResult> results(worker_count);
    for (auto &result : results) result.observed.resize(observed_count);
    auto worker = [&](unsigned index) {
        std::uint64_t pending = 0;
        try {
            if (control.cancel_requested.load(std::memory_order_relaxed)) return;
            Simulation<Rules> simulation(program, rules, options.damage_capacity, options.mutation_capacity);
            const auto quotient = options.iterations / worker_count;
            const auto remainder = options.iterations % worker_count;
            const auto start = quotient * index + std::min<std::uint64_t>(index, remainder);
            const auto end = start + quotient + (index < remainder ? 1 : 0);
            auto local_observer = observer;
            std::vector<value_t> values(observed_count);
            for (auto i = start; i < end; ++i) {
                if (control.cancel_requested.load(std::memory_order_relaxed)) break;
                simulation.Start(options.duration, IterationSeed(options.seed, i));
                simulation.Run();
                results[index].primary.Add(simulation.TotalDamage(), i);
                if (observed_count) {
                    local_observer(simulation, std::span<value_t>(values));
                    for (std::size_t j = 0; j < observed_count; ++j)
                        results[index].observed[j].Add(values[j], i);
                }
                pending += progress_units;
                if (pending >= 32) {
                    control.completed.fetch_add(pending, std::memory_order_relaxed);
                    pending = 0;
                }
            }
        } catch (...) {
            results[index].error = std::current_exception();
            control.cancel_requested.store(true, std::memory_order_relaxed);
        }
        if (pending) control.completed.fetch_add(pending, std::memory_order_relaxed);
    };
    {
        std::vector<std::jthread> threads;
        threads.reserve(worker_count - 1);
        for (unsigned i = 1; i < worker_count; ++i) threads.emplace_back(worker, i);
        worker(0);
    }
    BatchStats primary;
    std::vector<BatchStats> observed(observed_count);
    for (const auto &result : results) {
        if (result.error) std::rethrow_exception(result.error);
        primary.Merge(result.primary);
        for (std::size_t j = 0; j < observed_count; ++j) observed[j].Merge(result.observed[j]);
    }
    return {primary, std::move(observed)};
}
} // namespace detail

// Each worker reuses its simulator and log. The CLI specialization contains no
// progress/cancellation loads, and neither path retains multiple fight logs.
template <typename Rules>
BatchStats RunBatch(const MacroProgram &program, const BatchOptions &options, const Rules &rules = {}) {
    return detail::RunBatchImpl<false>(program, options, rules, nullptr);
}

template <typename Rules>
BatchStats RunBatchControlled(const MacroProgram &program, const BatchOptions &options,
                             BatchControl &control, const Rules &rules = {}) {
    control.completed.store(0, std::memory_order_relaxed);
    return detail::RunBatchImpl<true>(program, options, rules, &control);
}

template <typename Rules, typename Observer>
std::pair<BatchStats, std::vector<BatchStats>> RunBatchControlledObserved(
    const MacroProgram &program, const BatchOptions &options, BatchControl &control,
    const Rules &rules, std::size_t observed_count, std::uint64_t progress_units,
    Observer observer) {
    control.completed.store(0, std::memory_order_relaxed);
    return detail::RunBatchObservedImpl(program, options, control, rules, observed_count, progress_units,
                                        std::move(observer));
}

} // namespace JX3DPS::runtime
#endif
