#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/attribute_gains.hpp"
#include <cstdlib>
#include <iostream>
#include <new>

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;
namespace mw = JX3DPS::runtime::mo_wen;

#define CHECK(condition) do { if (!(condition)) { std::cerr << __LINE__ << ": " #condition "\n"; std::exit(1); } } while (false)
static bool count_allocations = false;
static std::size_t allocations = 0;
void *operator new(std::size_t size) {
    if (count_allocations) ++allocations;
    if (auto *p = std::malloc(size ? size : 1)) return p;
    throw std::bad_alloc();
}
void *operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { std::free(p); }
void operator delete[](void *p, std::size_t) noexcept { std::free(p); }

template <class Exception, class F>
void Throws(F &&function) {
    try { function(); } catch (const Exception &) { return; }
    CHECK(false);
}

void SameStats(const BatchStats &actual, const BatchStats &expected) {
    CHECK(actual.count == expected.count);
    CHECK(actual.checksum == expected.checksum);
    CHECK(actual.min_damage == expected.min_damage);
    CHECK(actual.max_damage == expected.max_damage);
    CHECK(std::abs(actual.mean_damage - expected.mean_damage) <= std::max(1.0, expected.mean_damage) * 1e-12);
    CHECK(std::abs(actual.m2_damage - expected.m2_damage) <= std::max(1.0, expected.m2_damage) * 1e-10);
}

void StatisticsKeys() {
    DamageStatistics statistics;
    statistics.Reserve(16);
    DamageIntent hit;
    hit.skill = 1; hit.target = 2; hit.effect = 3; hit.level = 4; hit.sub = 5;
    hit.snapshot = 6; hit.snapshot_version = 7; hit.live_version = 8;
    hit.base = 9; hit.coefficient = 10;
    statistics.Record(hit);
    auto same = hit;
    same.frame = 30; same.sequence = 100; same.random_key = 999;
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
    for (unsigned round = 0; round < 3; ++round)
        for (unsigned i = 0; i < 512; ++i) {
            hit.base = i;
            hit.snapshot_version = i % 11;
            hit.live_version = i % 13;
            statistics.Record(hit);
        }
    CHECK(statistics.Entries().size() == 512);
    for (const auto &entry : statistics.Entries()) CHECK(entry.count == 3);
    Throws<std::invalid_argument>([&] { statistics.Reserve(0); });
}

// A third module proves that the framework has no Tai Xu/Mo Wen dispatch or
// hidden full simulations for the five damage-only gains.
namespace probe {
struct Module;
struct Counters {
    std::atomic<unsigned> starts{0};
    std::array<std::atomic<unsigned>, 4> stages{};
    BatchControl *control = nullptr;
    unsigned cancel_at = 0;
};
struct Config {
    using RuntimeModule = Module;
    struct Attributes {
        value_t attack_base = 1, crit = 0, crit_power = 0, overcome = 0;
        value_t strain = 0, surplus = 0, haste = 0, weapon = 0;
    } attributes;
    std::shared_ptr<Counters> counters = std::make_shared<Counters>();
};
struct Rules {
    std::shared_ptr<const Config> config;
    bool casted = false;
    explicit Rules(std::shared_ptr<const Config> data) : config(std::move(data)) {}
    template <class Engine> void Setup(Engine &) {}
    template <class Engine> void Reset(Engine &, std::uint64_t) {
        casted = false;
        auto &c = *config->counters;
        const auto &a = config->attributes;
        ++c.stages[a.crit ? 1 : a.strain ? 2 : a.haste ? 3 : 0];
        if (++c.starts == c.cancel_at && c.control) c.control->cancel_requested = true;
    }
    template <class Engine> bool Cast(Engine &engine, const MacroLine &) {
        if (casted) return false;
        casted = true;
        for (unsigned i = 0; i < 3; ++i)
            engine.Log().Record(0, 0, 0, 0, i, 1, 0, RollResult::HIT);
        return true;
    }
    template <class Engine> void OnTimer(Engine &, TimerHandle, DeadlineQueue::TimerId) {}
    value_t Reduce(const DamageIntent &hit) const {
        const auto &a = config->attributes;
        return hit.base + a.attack_base + a.crit + a.crit_power + a.overcome + a.strain + a.surplus + a.haste + a.weapon;
    }
};
struct DamageReducer : Rules {
    using Rules::Rules;
    void Setup(std::size_t) {}
    void Bind(const Rules &) {}
};
struct Module {
    using Rules = probe::Rules;
    using DamageReducer = probe::DamageReducer;
    static auto Prepare(const Config &config) { return std::make_shared<const Config>(config); }
};
}

void ExecutionAndCancellation() {
    CompileOptions compile;
    compile.skill_count = 1;
    auto [macro, errors] = MacroCompiler::Compile("/cast action", compile);
    CHECK(errors.empty());
    BatchOptions options{7, 11, 64, 3, 16, 16};
    probe::Config config;
    BatchControl control;
    const auto result = RunAttributeGainsControlled(macro.program, options, control, config);
    CHECK(config.counters->starts == options.iterations * 4);
    CHECK(control.completed == options.iterations * AttributeGainSimulationStages<probe::Config>);
    CHECK(result.primary.mean_damage == 6);
    for (const auto &stage : config.counters->stages) CHECK(stage == options.iterations);
    for (const auto &gain : result.gains) CHECK(gain && gain->count == options.iterations && gain->mean_damage == 306);

    options.workers = 1;
    for (const auto stop : {3u, 9u, 16u}) {
        config.counters = std::make_shared<probe::Counters>();
        config.counters->control = &control;
        config.counters->cancel_at = stop;
        control.cancel_requested = false;
        const auto partial = RunAttributeGainsControlled(macro.program, options, control, config);
        CHECK(config.counters->starts == stop && control.completed == stop);
        CHECK(partial.primary.count == std::min<std::uint64_t>(stop, options.iterations));
        unsigned completed_gains = 0;
        for (const auto &gain : partial.gains) completed_gains += gain.has_value();
        CHECK(completed_gains == (stop < 7 ? 0u : stop < 14 ? 5u : 6u));
    }
    config.counters = std::make_shared<probe::Counters>();
    control.cancel_requested = true;
    const auto cancelled = RunAttributeGainsControlled(macro.program, options, control, config);
    CHECK(cancelled.primary.count == 0 && control.completed == 0 && config.counters->starts == 0);
    control.cancel_requested = false;
    options.damage_capacity = 1;
    Throws<std::length_error>([&] { RunAttributeGainsControlled(macro.program, options, control, config); });
    CHECK(control.cancel_requested && control.completed == 0);
    auto invalid = DefaultGainIncrements;
    invalid[6] = 0;
    Throws<std::invalid_argument>([&] {
        RunAttributeGainsControlled(macro.program, options, control, config, detail::TotalDamageReducer{}, invalid);
    });
    Throws<std::invalid_argument>([&] { ApplyAttributeGain(config, 0, -1); });
}

template <class Config>
void CompareGains(const Config &config, std::string_view source, CompileOptions compile,
                  const AttributeGainIncrements &increments = DefaultGainIncrements) {
    auto [macro, errors] = MacroCompiler::Compile(source, compile);
    CHECK(errors.empty());
    BatchOptions options{17, 931, 16 * 65, 1, 16000, 48000};
    BatchControl control;
    const auto single = RunAttributeGainsControlled(macro.program, options, control, config, detail::TotalDamageReducer{}, increments);
    CHECK(control.completed == options.iterations * 3);
    SameStats(single.primary, RunBatch(macro.program, options, LoadSpecialization(config)));
    for (std::size_t i = 0; i < single.gains.size(); ++i) {
        CHECK(single.gains[i]);
        const auto direct = RunBatch(macro.program, options, LoadSpecialization(ApplyAttributeGain(config, static_cast<int>(i), increments[i])));
        SameStats(*single.gains[i], direct);
    }
    options.workers = 4;
    const auto parallel = RunAttributeGainsControlled(macro.program, options, control, config, detail::TotalDamageReducer{}, increments);
    CHECK(control.completed == options.iterations * 3);
    SameStats(parallel.primary, single.primary);
    for (std::size_t i = 0; i < single.gains.size(); ++i) {
        CHECK(parallel.gains[i]);
        SameStats(*parallel.gains[i], *single.gains[i]);
    }

    using Rules = typename Config::RuntimeModule::Rules;
    Simulation<Rules> simulation(macro.program, LoadSpecialization(config), options.damage_capacity, options.mutation_capacity);
    DamageGainObserver<Config> observer(config, DamageOnlyGainIndexes<Config>, increments);
    observer.Setup(simulation);
    std::array<std::array<value_t, DamageOnlyGainIndexes<Config>.size()>, 4> replay_damage{};
    allocations = 0;
    count_allocations = true;
    for (unsigned i = 0; i < replay_damage.size(); ++i) {
        simulation.Start(options.duration, IterationSeed(options.seed, i));
        simulation.Run();
        observer(simulation, replay_damage[i]);
    }
    count_allocations = false;
    CHECK(allocations == 0);
    for (std::size_t j = 0; j < DamageOnlyGainIndexes<Config>.size(); ++j) {
        const auto index = DamageOnlyGainIndexes<Config>[j];
        const auto changed = ApplyAttributeGain(config, index, increments[index]);
        Simulation<Rules> direct(macro.program, LoadSpecialization(changed), options.damage_capacity, options.mutation_capacity);
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
    for (const auto &entry : statistics.Entries()) count += entry.count;
    CHECK(count == simulation.Log().Size());
    CHECK(statistics.Entries().size() < count);
    Throws<std::invalid_argument>([&] {
        DamageGainObserver<Config> invalid(config, RotationGainIndexes<Config>);
    });
}

void GameModules() {
    constexpr auto rotation = "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [qidian>=8] wuwu\n/cast bahuang\n"
                              "/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";
    tx::Config tai_xu;
    CompareGains(tai_xu, rotation, tx::MacroOptions(false));
    CompareGains(tai_xu, rotation, tx::MacroOptions(false), {398, 883, 883, 883, 883, 883, 883, 597});
    tai_xu.talents = tx::DieRenTalent | tx::WuYi | tx::ShenMai | tx::XuanMen | tx::LieYun | tx::GuChang;
    tai_xu.equipment = tx::WeaponCW | tx::SetAttribute;
    tai_xu.attributes.overcome_base = 20000;
    tai_xu.attributes.overcome_by_class = 5000;
    tai_xu.recipes[tx::WuWo] = tx::Crit2 | tx::Damage4 | tx::Damage5;
    tai_xu.team_buffs = {{tx::team::JiLei, 0, 96, 1}, {tx::team::JiLei, 160, 96, 1},
        {tx::team::NongMei, 48, 96, 1}, {tx::team::HaoLingSanJun, 16, 0, 48},
        {tx::team::PoFeng, 32, 160, 1}, {tx::team::JieHuo, 96, 160, 1}};
    CompareGains(tai_xu, "/scast shengtaiji\n" + std::string(rotation), tx::MacroOptions(false));

    constexpr auto mo_wen_rotation = "/cast [tnobuff:shang] shang\n/cast [tnobuff:jue] jue\n/cast shuying\n"
                                    "/cast gaoshan\n/cast [buff:qufeng=4] yu\n/cast bianzhi\n/cast biangong";
    mw::Config mo_wen;
    CompareGains(mo_wen, mo_wen_rotation, mw::MacroOptions(false));
    CompareGains(mo_wen, mo_wen_rotation, mw::MacroOptions(false), {475, 883, 883, 883, 883, 883, 883, 597});
    mo_wen.talents = mw::HaoZhong | mw::FeiFan | mw::ShiXiang | mw::ZhengLvHeMing | mw::LiuZhaoTalent | mw::KeMeng;
    mo_wen.equipment = mw::EnchantShoes | mw::EnchantWrist | mw::EnchantBelt | mw::SetAttribute | mw::SetSkill |
        mw::WeaponCW | mw::WeaponWater;
    mo_wen.team_effects = mw::YouRen | mw::JingMiao;
    mo_wen.recipes[0] = mw::GongCrit2 | mw::GongDamage4 | mw::GongPrepare1;
    CompareGains(mo_wen, "/scast zhenglv1\n" + std::string(mo_wen_rotation), mw::MacroOptions(false));
    // The catalog keeps legacy Chinese names for the two target DOTs.
    const auto options = mw::MacroOptions(false);
    CHECK(options.resolve_buff(mw::Skills.entries[mw::Shang].name) == mw::ShangDot);
    CHECK(options.resolve_buff(mw::Skills.entries[mw::Jue].name) == mw::JueDot);
}

int main() {
    StatisticsKeys();
    ExecutionAndCancellation();
    GameModules();
    std::cout << "attribute gain tests passed: module-specific simulation stages, statistics reduction, game-module parity, cancellation, worker parity, zero reduction allocations\n";
}
