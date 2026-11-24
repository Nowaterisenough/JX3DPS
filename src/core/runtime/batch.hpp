#ifndef JX3DPS_RUNTIME_BATCH_HPP
#define JX3DPS_RUNTIME_BATCH_HPP

#include <atomic>
#include <exception>
#include <span>
#include <thread>

#include "batch_profile.hpp"
#include "batch_statistics.hpp"
#include "simulation.hpp"

namespace JX3DPS::runtime {

struct BatchOptions
{
    std::uint64_t iterations        = 1;
    std::uint64_t seed              = 0;
    tick_t        duration          = 4800;
    unsigned      workers           = 1;
    std::size_t   damage_capacity   = 4096;
    std::size_t   mutation_capacity = 8192;
    bool          random_seed       = false; // Draw an independent seed for every fight.
};

inline std::uint64_t FightSeed(const BatchOptions &options, std::uint64_t iteration)
{
    return options.random_seed ? RandomSeed() : IterationSeed(options.seed, iteration);
}

// Owned by the caller for the duration of a controlled batch. Progress counts
// only completed fights; cancellation takes effect at the next fight boundary.
struct BatchControl
{
    alignas(64) std::atomic<bool> cancel_requested{ false };
    alignas(64) std::atomic<std::uint64_t> completed{ 0 };
};

namespace detail {
enum class BatchAssignment
{
    FIXED_RANGES,
    SHARED_BLOCKS
};

struct TotalDamageReducer
{
    template <class Simulation>
    value_t operator()(Simulation &simulation, unsigned) const
    {
        return simulation.TotalDamage();
    }
};

template <bool Controlled, bool Measure = false, BatchAssignment Assignment = BatchAssignment::FIXED_RANGES, typename Rules, typename Reducer = TotalDamageReducer>
BatchStats RunBatchImpl(const MacroProgram &program,
                        const BatchOptions &options,
                        const Rules        &rules,
                        BatchControl       *control,
                        Reducer             reducer = {},
                        BatchProfile       *profile = nullptr,
                        const char         *stage   = "基准战斗")
{
    if (!options.iterations || !options.workers || options.duration <= 0) {
        throw std::invalid_argument("batch iterations, workers and duration must be positive");
    }
    const auto worker_count = static_cast<unsigned>(std::min<std::uint64_t>(options.workers, options.iterations));
    // Give each worker its first block. For longer runs, distribute subsequent
    // blocks on demand and merge in block order so floating-point statistics
    // are reproducible regardless of which worker finishes first.
    unsigned   blocks       = worker_count;
    if constexpr (Assignment == BatchAssignment::SHARED_BLOCKS) {
        constexpr unsigned TARGET_SIZE = 128;
        constexpr unsigned MAX_BLOCKS  = 1024;
        if (worker_count > 1) {
            const auto WANTED = 1 + ((options.iterations - 1) / TARGET_SIZE);
            blocks = std::max(worker_count, static_cast<unsigned>(std::min<std::uint64_t>(WANTED, MAX_BLOCKS)));
        }
    }
    std::atomic<std::uint64_t> next_block{ worker_count };
    const auto                 quotient  = options.iterations / blocks;
    const auto                 REMAINDER = options.iterations % blocks;

    struct alignas(64) BlockResult
    {
        BatchStats stats;
    };

    std::vector<BlockResult>        results(blocks);
    std::vector<std::exception_ptr> errors(worker_count);
    const auto                      WORKER = [&](unsigned index) {
        std::uint64_t pending = 0;
        try {
            if constexpr (Controlled) {
                if (control->cancel_requested.load(std::memory_order_relaxed)) {
                    return;
                }
            }
            const WorkerMeasurement<Measure> measurement(profile, index, stage);
            Simulation<Rules, false, Measure> simulation(program, rules, options.damage_capacity, options.mutation_capacity);
            const auto LOCAL_REDUCER = reducer;
            for (std::uint64_t block = index; block < blocks; block = next_block.fetch_add(1, std::memory_order_relaxed))
            {
                const auto start = (quotient * block) + std::min(block, REMAINDER);
                const auto END   = start + quotient + (block < REMAINDER);
                auto      &stats = results[block].stats;
                auto       i     = start;
                for (; i < END; ++i) {
                    if constexpr (Controlled) {
                        if (control->cancel_requested.load(std::memory_order_relaxed)) {
                            break;
                        }
                    }
                    simulation.Start(options.duration, FightSeed(options, i));
                    simulation.Run();
                    stats.Add(LOCAL_REDUCER(simulation, index), i);
                    if constexpr (Measure) {
                        ++profile->workers[index].fights;
                    }
                    if constexpr (Controlled) {
                        if (++pending == 32) {
                            control->completed.fetch_add(pending, std::memory_order_relaxed);
                            pending = 0;
                        }
                    }
                }
                if (i != END || blocks == worker_count) {
                    break;
                }
            }
        } catch (...) {
            errors[index] = std::current_exception();
            if constexpr (Controlled) {
                control->cancel_requested.store(true, std::memory_order_relaxed);
            }
        }
        if constexpr (Controlled) {
            control->completed.fetch_add(pending, std::memory_order_relaxed);
        }
    };
    {
        std::vector<std::jthread> threads;
        threads.reserve(worker_count - 1);
        for (unsigned i = 1; i < worker_count; ++i) {
            threads.emplace_back(WORKER, i);
        }
        WORKER(0);
    }
    BatchStats combined;
    for (const auto &error : errors) {
        if (error) {
            std::rethrow_exception(error);
        }
    }
    for (const auto &result : results) {
        combined.Merge(result.stats);
    }
    return combined;
}

// Run one primary batch while allowing the caller to reduce each completed
// fight's event log into additional statistics. The observer receives a
// reusable output span, so replay paths do not allocate per event or fight.
template <bool Measure = false, typename Rules, typename Observer, typename Reducer = TotalDamageReducer>
std::pair<BatchStats, std::vector<BatchStats>> RunBatchObservedImpl(
    const MacroProgram &program,
    const BatchOptions &options,
    BatchControl       &control,
    const Rules        &rules,
    std::size_t         observed_count,
    std::uint64_t       progress_units,
    Observer            observer,
    Reducer             reducer = {},
    BatchProfile       *profile = nullptr)
{
    if (!options.iterations || !options.workers || options.duration <= 0 || !progress_units) {
        throw std::invalid_argument("batch iterations, workers and duration must be positive");
    }
    const auto worker_count = static_cast<unsigned>(std::min<std::uint64_t>(options.workers, options.iterations));

    struct alignas(64) WorkerResult
    {
        BatchStats              primary;
        std::vector<BatchStats> observed;
        std::exception_ptr      error;
    };

    std::vector<WorkerResult> results(worker_count);
    for (auto &result : results) {
        result.observed.resize(observed_count);
    }
    const auto WORKER = [&](unsigned index) {
        std::uint64_t pending = 0;
        try {
            if (control.cancel_requested.load(std::memory_order_relaxed)) {
                return;
            }
            const WorkerMeasurement<Measure> measurement(profile, index, "基准战斗");
            Simulation<Rules, false, Measure> simulation(program, rules, options.damage_capacity, options.mutation_capacity);
            const auto quotient       = options.iterations / worker_count;
            const auto REMAINDER      = options.iterations % worker_count;
            const auto start          = (quotient * index) + std::min<std::uint64_t>(index, REMAINDER);
            const auto END            = start + quotient + (index < REMAINDER ? 1 : 0);
            auto       local_observer = observer;
            const auto LOCAL_REDUCER  = reducer;
            if constexpr (requires { local_observer.Setup(simulation); }) {
                local_observer.Setup(simulation);
            }
            std::vector<value_t> values(observed_count);
            for (auto i = start; i < END; ++i) {
                if (control.cancel_requested.load(std::memory_order_relaxed)) {
                    break;
                }
                simulation.Start(options.duration, FightSeed(options, i));
                simulation.Run();
                results[index].primary.Add(LOCAL_REDUCER(simulation, index), i);
                if (observed_count) {
                    const PhaseScope<Measure> STAGE(FightPhase::GAIN_REPLAY, "DamageGainObserver");
                    local_observer(simulation, std::span<value_t>(values));
                    for (std::size_t j = 0; j < observed_count; ++j) {
                        results[index].observed[j].Add(values[j], i);
                    }
                }
                if constexpr (Measure) {
                    ++profile->workers[index].fights;
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
        if (pending) {
            control.completed.fetch_add(pending, std::memory_order_relaxed);
        }
    };
    {
        std::vector<std::jthread> threads;
        threads.reserve(worker_count - 1);
        for (unsigned i = 1; i < worker_count; ++i) {
            threads.emplace_back(WORKER, i);
        }
        WORKER(0);
    }
    BatchStats              primary;
    std::vector<BatchStats> observed(observed_count);
    for (const auto &result : results) {
        if (result.error) {
            std::rethrow_exception(result.error);
        }
        primary.Merge(result.primary);
        for (std::size_t j = 0; j < observed_count; ++j) {
            observed[j].Merge(result.observed[j]);
        }
    }
    return { primary, std::move(observed) };
}
} // namespace detail

// Each worker reuses its simulator and log. The CLI specialization contains no
// progress/cancellation loads, and neither path retains multiple fight logs.
template <typename Rules>
BatchStats RunBatch(const MacroProgram &program, const BatchOptions &options, const Rules &rules = {})
{
    return detail::RunBatchImpl<false, false, detail::BatchAssignment::SHARED_BLOCKS>(program, options, rules, nullptr);
}

template <typename Rules>
BatchStats RunBatchControlled(const MacroProgram &program, const BatchOptions &options, BatchControl &control, const Rules &rules = {})
{
    control.completed.store(0, std::memory_order_relaxed);
    return detail::RunBatchImpl<true, false, detail::BatchAssignment::SHARED_BLOCKS>(program, options, rules, &control);
}

template <typename Rules, typename Observer>
std::pair<BatchStats, std::vector<BatchStats>> RunBatchControlledObserved(
    const MacroProgram &program,
    const BatchOptions &options,
    BatchControl       &control,
    const Rules        &rules,
    std::size_t         observed_count,
    std::uint64_t       progress_units,
    Observer            observer)
{
    control.completed.store(0, std::memory_order_relaxed);
    return detail::RunBatchObservedImpl(program, options, control, rules, observed_count, progress_units, std::move(observer));
}

// The reducer runs once per completed fight and can aggregate into worker-local
// storage while reducing damage. It receives the worker index, not a fight id.
template <typename Rules, typename Reducer>
BatchStats RunBatchControlledWithReducer(const MacroProgram &program, const BatchOptions &options, BatchControl &control, const Rules &rules, Reducer reducer)
{
    control.completed.store(0, std::memory_order_relaxed);
    return detail::RunBatchImpl<true>(program, options, rules, &control, std::move(reducer));
}

} // namespace JX3DPS::runtime
#endif
