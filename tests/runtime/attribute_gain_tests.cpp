#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <new>
#include <stdexcept>

#include "src/class/common/runtime_rules.hpp"
#include "src/engine/scenarios.hpp"
#include "src/core/runtime/attribute_gains.hpp"

#include "check.hpp"

using namespace JX3DPS;
using namespace JX3DPS::runtime;

static bool        count_allocations = false;
static std::size_t allocations       = 0;

void *operator new(std::size_t size)
{
    if (count_allocations) {
        ++allocations;
    }
    if (auto *p = std::malloc((size != 0u) ? size : 1)) {
        return p;
    }
    throw std::bad_alloc();
}

void *operator new[](std::size_t size)
{
    return ::operator new(size);
}

void operator delete(void *p) noexcept
{
    std::free(p);
}

void operator delete[](void *p) noexcept
{
    std::free(p);
}

void operator delete(void *p, std::size_t) noexcept
{
    std::free(p);
}

void operator delete[](void *p, std::size_t) noexcept
{
    std::free(p);
}

template <class Exception, class F>
void Throws(F &&function)
{
    try {
        function();
    } catch (const Exception &) {
        return;
    }
    CHECK(false);
}

void SameStats(const BatchStats &actual, const BatchStats &expected)
{
    CHECK(actual.count == expected.count);
    CHECK(actual.checksum == expected.checksum);
    CHECK(actual.min_damage == expected.min_damage);
    CHECK(actual.max_damage == expected.max_damage);
    CHECK(std::abs(actual.mean_damage - expected.mean_damage) <= std::max(1.0, expected.mean_damage) * 1e-12);
    CHECK(std::abs(actual.m2_damage - expected.m2_damage) <= std::max(1.0, expected.m2_damage) * 1e-10);
}

void StatisticsKeys()
{
    DamageStatistics statistics;
    statistics.Reserve(16);
    DamageIntent hit;
    hit.skill            = 1;
    hit.target           = 2;
    hit.effect           = 3;
    hit.level            = 4;
    hit.sub              = 5;
    hit.snapshot         = 6;
    hit.snapshot_version = 7;
    hit.live_version     = 8;
    hit.base             = 9;
    hit.coefficient      = 10;
    statistics.Record(hit);
    auto same       = hit;
    same.frame      = 30;
    same.sequence   = 100;
    same.random_key = 999;
    statistics.Record(same);
    CHECK(statistics.Entries().size() == 1 && statistics.Entries()[0].count == 2);
    auto distinct = [&](auto change) {
        auto other = hit;
        change(other);
        statistics.Record(other);
    };
    distinct([](auto &h) { ++h.skill; });
    distinct([](auto &h) { ++h.target; });
    distinct([](auto &h) { ++h.effect; });
    distinct([](auto &h) { ++h.level; });
    distinct([](auto &h) { ++h.sub; });
    distinct([](auto &h) { ++h.snapshot; });
    distinct([](auto &h) { ++h.snapshot_version; });
    distinct([](auto &h) { ++h.live_version; });
    distinct([](auto &h) { ++h.base; });
    distinct([](auto &h) { ++h.coefficient; });
    distinct([](auto &h) { h.outcome = RollResult::DOUBLE; });
    CHECK(statistics.Entries().size() == 12);
    statistics.Clear();
    CHECK(statistics.Entries().empty());
    statistics.Record(hit);
    CHECK(statistics.Entries().size() == 1 && statistics.Entries()[0].count == 1);
    statistics.Reserve(1);
    statistics.Record(hit);
    statistics.Record(hit);
    Throws<std::length_error>([&] { distinct([](auto &h) { ++h.base; }); });
    statistics.Clear();
    hit.coefficient = 0;
    statistics.Record(hit);
    hit.coefficient = -0.0;
    statistics.Record(hit);
    CHECK(statistics.Entries().size() == 1 && statistics.Entries()[0].count == 2);

    statistics.Reserve(512);
    statistics.Clear();
    // Many different keys exercise collision resolution as well as resets.
    for (unsigned round = 0; round < 3; ++round) {
        for (unsigned i = 0; i < 512; ++i) {
            hit.base             = i;
            hit.snapshot_version = i % 11;
            hit.live_version     = i % 13;
            statistics.Record(hit);
        }
    }
    CHECK(statistics.Entries().size() == 512);
    for (const auto &entry : statistics.Entries()) {
        CHECK(entry.count == 3);
    }
    Throws<std::invalid_argument>([&] { statistics.Reserve(0); });
}

namespace probe {
struct Module;

struct Counters
{
    std::atomic<unsigned>                starts{ 0 };
    std::array<std::atomic<unsigned>, 4> stages{};
    BatchControl                        *control   = nullptr;
    unsigned                             cancel_at = 0;
};

struct Config
{
    using RuntimeModule = Module;

    struct Attributes
    {
        value_t attack_base = 1, crit = 0, crit_power = 0, overcome = 0;
        value_t strain = 0, surplus = 0, haste = 0, weapon = 0;
    } attributes;

    std::shared_ptr<Counters> counters = std::make_shared<Counters>();
};

struct Rules
{
    std::shared_ptr<const Config> config;
    bool                          casted = false;

    explicit Rules(std::shared_ptr<const Config> data) : config(std::move(data)) { }

    template <class Engine>
    void Setup(Engine &)
    {
    }

    template <class Engine>
    void Reset(Engine &, std::uint64_t)
    {
        casted        = false;
        auto       &c = *config->counters;
        const auto &a = config->attributes;
        ++c.stages[a.crit ? 1 : a.strain ? 2 : a.haste ? 3 : 0];
        if (++c.starts == c.cancel_at && c.control) {
            c.control->cancel_requested = true;
        }
    }

    template <class Engine>
    bool Cast(Engine &engine, const MacroLine &)
    {
        if (casted) {
            return false;
        }
        casted = true;
        for (unsigned i = 0; i < 3; ++i) {
            engine.Log().Record(0, 0, 0, 0, i, 1, 0, RollResult::HIT);
        }
        return true;
    }

    template <class Engine>
    void OnTimer(Engine &, TimerHandle, DeadlineQueue::TimerId)
    {
    }

    value_t Reduce(const DamageIntent &hit) const
    {
        const auto &a = config->attributes;
        return hit.base + a.attack_base + a.crit + a.crit_power + a.overcome + a.strain + a.surplus + a.haste + a.weapon;
    }
};

struct DamageReducer : Rules
{
    using Rules::Rules;

    void Setup(std::size_t) { }

    void Bind(const Rules &) { }
};

struct Module
{
    using Rules         = probe::Rules;
    using DamageReducer = probe::DamageReducer;

    static auto Prepare(const Config &config) { return std::make_shared<const Config>(config); }
};
} // namespace probe

void ExecutionAndCancellation()
{
    CompileOptions compile;
    compile.skill_count  = 1;
    auto [macro, errors] = MacroCompiler::Compile("/cast action", compile);
    CHECK(errors.empty());
    BatchOptions  options{ 7, 11, 64, 3, 16, 16 };
    probe::Config config;
    BatchControl  control;
    const auto    result = RunAttributeGainsControlled(macro.program, options, control, config);
    CHECK(config.counters->starts == options.iterations * 4);
    CHECK(control.completed == options.iterations * AttributeGainSimulationStages<probe::Config>);
    CHECK(result.primary.mean_damage == 6);
    for (const auto &stage : config.counters->stages) {
        CHECK(stage == options.iterations);
    }
    for (const auto &gain : result.gains) {
        CHECK(gain && gain->count == options.iterations && gain->mean_damage == 306);
    }

    options.workers = 1;
    for (const auto STOP : { 3u, 9u, 16u }) {
        config.counters            = std::make_shared<probe::Counters>();
        config.counters->control   = &control;
        config.counters->cancel_at = STOP;
        control.cancel_requested   = false;
        const auto PARTIAL         = RunAttributeGainsControlled(macro.program, options, control, config);
        CHECK(config.counters->starts == STOP && control.completed == STOP);
        CHECK(PARTIAL.primary.count == std::min<std::uint64_t>(STOP, options.iterations));
        unsigned completed_gains = 0;
        for (const auto &gain : PARTIAL.gains) {
            completed_gains += static_cast<unsigned int>(gain.has_value());
        }
        CHECK(completed_gains == (STOP < 7 ? 0u : STOP < 14 ? 5u : 6u));
    }
    config.counters          = std::make_shared<probe::Counters>();
    control.cancel_requested = true;
    const auto CANCELLED     = RunAttributeGainsControlled(macro.program, options, control, config);
    CHECK(CANCELLED.primary.count == 0 && control.completed == 0 && config.counters->starts == 0);
    control.cancel_requested = false;
    options.damage_capacity  = 1;
    Throws<std::length_error>([&] { RunAttributeGainsControlled(macro.program, options, control, config); });
    CHECK(control.cancel_requested && control.completed == 0);
    auto invalid = DefaultGainIncrements;
    invalid[6]   = 0;
    Throws<std::invalid_argument>([&] {
        RunAttributeGainsControlled(macro.program, options, control, config, detail::TotalDamageReducer{}, invalid);
    });
    Throws<std::invalid_argument>([&] { ApplyAttributeGain(config, 0, -1); });
}

template <class Config>
void CompareGains(const Config &config, std::string_view source, CompileOptions compile, const AttributeGainIncrements &increments = DefaultGainIncrements)
{
    auto [macro, errors] = MacroCompiler::Compile(source, compile);
    CHECK(errors.empty());
    BatchOptions options{ 17, 931, 16 * 65, 1, 16000, 48000 };
    BatchControl control;
    const auto   SINGLE =
        RunAttributeGainsControlled(macro.program, options, control, config, detail::TotalDamageReducer{}, increments);
    CHECK(control.completed == options.iterations * GainSimulationStages(config));
    SameStats(SINGLE.primary, RunBatch(macro.program, options, LoadSpecialization(config)));
    for (std::size_t i = 0; i < SINGLE.gains.size(); ++i) {
        CHECK(SINGLE.gains[i]);
        const auto direct =
            RunBatch(macro.program, options, LoadSpecialization(ApplyAttributeGain(config, static_cast<int>(i), increments[i])));
        SameStats(*SINGLE.gains[i], direct);
    }
    options.workers = 4;
    const auto PARALLEL =
        RunAttributeGainsControlled(macro.program, options, control, config, detail::TotalDamageReducer{}, increments);
    CHECK(control.completed == options.iterations * GainSimulationStages(config));
    SameStats(PARALLEL.primary, SINGLE.primary);
    for (std::size_t i = 0; i < SINGLE.gains.size(); ++i) {
        CHECK(PARALLEL.gains[i]);
        SameStats(*PARALLEL.gains[i], *SINGLE.gains[i]);
    }

    using Rules = typename Config::RuntimeModule::Rules;
    Simulation<Rules> simulation(macro.program, LoadSpecialization(config), options.damage_capacity, options.mutation_capacity);
    DamageGainObserver<Config> observer(config, DamageOnlyGainIndexes<Config>, increments);
    observer.Setup(simulation);
    std::array<std::array<value_t, DamageOnlyGainIndexes<Config>.size()>, 4> replay_damage{};
    // Grow the fight's snapshot and timer storage before measuring steady-state replay.
    for (unsigned i = 0; i < replay_damage.size(); ++i) {
        simulation.Start(options.duration, IterationSeed(options.seed, i));
        simulation.Run();
        observer(simulation, replay_damage[i]);
    }
    allocations       = 0;
    count_allocations = true;
    for (unsigned i = 0; i < replay_damage.size(); ++i) {
        simulation.Start(options.duration, IterationSeed(options.seed, i));
        simulation.Run();
        observer(simulation, replay_damage[i]);
    }
    count_allocations = false;
    CHECK(allocations == 0);
    for (std::size_t j = 0; j < DamageOnlyGainIndexes<Config>.size(); ++j) {
        const auto INDEX   = DamageOnlyGainIndexes<Config>[j];
        const auto CHANGED = ApplyAttributeGain(config, INDEX, increments[INDEX]);
        Simulation<Rules> direct(macro.program, LoadSpecialization(CHANGED), options.damage_capacity, options.mutation_capacity);
        for (std::size_t i = 0; i < replay_damage.size(); ++i) {
            direct.Start(options.duration, IterationSeed(options.seed, i));
            direct.Run();
            CHECK(replay_damage[i][j] == direct.TotalDamage());
        }
    }
    DamageStatistics statistics;
    statistics.Reserve(options.damage_capacity);
    statistics.Collect(simulation.Log().Intents());
    std::size_t count = 0;
    for (const auto &entry : statistics.Entries()) {
        count += entry.count;
    }
    CHECK(count == simulation.Log().Size());
    CHECK(statistics.Entries().size() < count);
    Throws<std::invalid_argument>([&] { const DamageGainObserver<Config> INVALID(config, RotationGainIndexes<Config>); });
}

void GameModules()
{
    constexpr auto ROTATION = "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [qidian>=8] wuwu\n/cast bahuang\n"
                              "/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";
    for (auto code : {"tx", "mw"}) {
        auto cfg = JX3DPS::engine::Scenario(code);
        CompareGains(cfg, cfg.book->default_macro, CatalogMacroOptions(cfg.book->Catalog(), false));
    }
}

int main()
{
    StatisticsKeys();
    ExecutionAndCancellation();
    GameModules();
    std::cout << "attribute gain tests passed: module-specific simulation stages, statistics reduction, game-module "
                 "parity, cancellation, worker parity, zero reduction allocations\n";
}
