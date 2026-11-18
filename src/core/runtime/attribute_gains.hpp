#pragma once

#include "batch.hpp"
#include "damage_statistics.hpp"
#include "specialization.hpp"
#include <optional>

namespace JX3DPS::runtime {

enum class AttributeGainKind : std::uint8_t {
    Attack, Critical, CriticalPower, Overcome, Strain, Surplus, Haste, Weapon
};
struct AttributeGainSpec {
    AttributeGainKind kind;
    bool rerun_rotation;
};
inline constexpr std::array<AttributeGainSpec, 8> AttributeGains{{
    {AttributeGainKind::Attack, false}, {AttributeGainKind::Critical, true},
    {AttributeGainKind::CriticalPower, false}, {AttributeGainKind::Overcome, false},
    {AttributeGainKind::Strain, true}, {AttributeGainKind::Surplus, false},
    {AttributeGainKind::Haste, true}, {AttributeGainKind::Weapon, false}
}};

template <class Config>
constexpr bool RerunsRotation(AttributeGainSpec gain) {
    using Module = typename Config::RuntimeModule;
    if constexpr (requires { Module::replay_strain; }) {
        if (gain.kind == AttributeGainKind::Strain) return !Module::replay_strain;
    }
    return gain.rerun_rotation;
}

template <class Config, bool Rerun>
consteval auto GainIndexes() {
    constexpr std::size_t count = [] {
        std::size_t result = 0;
        for (const auto &gain : AttributeGains) result += RerunsRotation<Config>(gain) == Rerun;
        return result;
    }();
    std::array<int, count> result{};
    std::size_t cursor = 0;
    for (std::size_t i = 0; i < AttributeGains.size(); ++i)
        if (RerunsRotation<Config>(AttributeGains[i]) == Rerun) result[cursor++] = static_cast<int>(i);
    return result;
}
template <class Config> inline constexpr auto DamageOnlyGainIndexes = GainIndexes<Config, false>();
template <class Config> inline constexpr auto RotationGainIndexes = GainIndexes<Config, true>();
template <class Config> inline constexpr std::uint64_t AttributeGainSimulationStages = 1 + RotationGainIndexes<Config>.size();
inline constexpr int AttributeGainIncrement = 100;
using AttributeGainIncrements = std::array<int, AttributeGains.size()>;
inline constexpr AttributeGainIncrements DefaultGainIncrements{100, 100, 100, 100, 100, 100, 100, 100};

template <class Config>
Config ApplyAttributeGain(Config config, AttributeGainKind kind, int increment = AttributeGainIncrement) {
    if (increment <= 0) throw std::invalid_argument("attribute gain increment must be positive");
    auto &a = config.attributes;
    switch (kind) {
        case AttributeGainKind::Attack: a.attack_base += increment; break;
        case AttributeGainKind::Critical: a.crit += increment; break;
        case AttributeGainKind::CriticalPower: a.crit_power += increment; break;
        case AttributeGainKind::Overcome:
            if constexpr (requires { a.overcome_base; }) {
                if (a.overcome_base >= 0) a.overcome_base += increment;
                else a.overcome += increment;
            } else a.overcome += increment;
            break;
        case AttributeGainKind::Strain: a.strain += increment; break;
        case AttributeGainKind::Surplus: a.surplus += increment; break;
        case AttributeGainKind::Haste: a.haste += increment; break;
        case AttributeGainKind::Weapon: a.weapon += increment; break;
        default: throw std::out_of_range("invalid attribute gain kind");
    }
    return config;
}
template <class Config>
Config ApplyAttributeGain(Config config, int index, int increment = AttributeGainIncrement) {
    if (index < 0 || index >= static_cast<int>(AttributeGains.size()))
        throw std::out_of_range("invalid attribute gain index");
    return ApplyAttributeGain(std::move(config), AttributeGains[index].kind, increment);
}

template <class Config>
class DamageGainObserver {
public:
    using Reducer = typename Config::RuntimeModule::DamageReducer;
    DamageGainObserver(const Config &config, std::span<const int> indexes,
                       const AttributeGainIncrements &increments = DefaultGainIncrements) {
        reducers_.reserve(indexes.size());
        for (const auto index : indexes) {
            if (index < 0 || index >= static_cast<int>(AttributeGains.size()) || RerunsRotation<Config>(AttributeGains[index]))
                throw std::invalid_argument("rotation-sensitive attribute requires simulation");
            reducers_.emplace_back(PrepareConfiguration(ApplyAttributeGain(config, index, increments[index])));
        }
    }
    std::size_t Count() const { return reducers_.size(); }
    template <class Simulation>
    void Setup(const Simulation &simulation) {
        const auto capacity = simulation.Log().DamageCapacity();
        statistics_.Reserve(capacity);
        for (auto &reducer : reducers_) reducer.Setup(capacity);
    }
    template <class Simulation>
    void operator()(const Simulation &simulation, std::span<value_t> output) {
        if (output.size() != reducers_.size()) throw std::invalid_argument("invalid damage gain output size");
        statistics_.Collect(simulation.Log().Intents());
        for (std::size_t i = 0; i < reducers_.size(); ++i) {
            auto &reducer = reducers_[i];
            reducer.Bind(simulation.GetRules());
            output[i] = statistics_.Reduce(reducer);
        }
    }

private:
    std::vector<Reducer> reducers_;
    DamageStatistics statistics_;
};

struct AttributeGainBatch {
    BatchStats primary;
    std::array<std::optional<BatchStats>, AttributeGains.size()> gains;
};

template <class Config, class Reducer = detail::TotalDamageReducer>
AttributeGainBatch RunAttributeGainsControlled(const MacroProgram &program, const BatchOptions &options,
                                              BatchControl &control, const Config &config, Reducer reducer = {},
                                              const AttributeGainIncrements &increments = DefaultGainIncrements) {
    for (const auto increment : increments)
        if (increment <= 0) throw std::invalid_argument("attribute gain increment must be positive");
    control.completed.store(0, std::memory_order_relaxed);
    auto rules = LoadSpecialization(config);
    DamageGainObserver<Config> observer(config, DamageOnlyGainIndexes<Config>, increments);
    const auto observed_count = observer.Count();
    auto combined = detail::RunBatchObservedImpl(program, options, control, rules, observed_count, 1,
                                                 std::move(observer), std::move(reducer));
    AttributeGainBatch result{combined.first, {}};
    if (result.primary.count != options.iterations) return result;
    for (std::size_t i = 0; i < DamageOnlyGainIndexes<Config>.size(); ++i)
        result.gains[DamageOnlyGainIndexes<Config>[i]] = combined.second[i];
    for (const auto index : RotationGainIndexes<Config>) {
        if (control.cancel_requested.load(std::memory_order_relaxed)) break;
        const auto changed_rules = LoadSpecialization(ApplyAttributeGain(config, index, increments[index]));
        const auto gain = detail::RunBatchImpl<true>(program, options, changed_rules, &control);
        if (gain.count == options.iterations) result.gains[index] = gain;
    }
    return result;
}

} // namespace JX3DPS::runtime
