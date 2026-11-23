#pragma once

#include <optional>
#include <utility>

#include "batch.hpp"
#include "damage_statistics.hpp"
#include "specialization.hpp"

namespace JX3DPS::runtime {

enum class AttributeGainKind : std::uint8_t
{
    ATTACK,
    CRITICAL,
    CRITICAL_POWER,
    OVERCOME,
    STRAIN,
    SURPLUS,
    HASTE,
    WEAPON
};

struct AttributeGainSpec
{
    AttributeGainKind kind;
    bool              rerun_rotation;
};

inline constexpr std::array<AttributeGainSpec, 8> AttributeGains{
    { { AttributeGainKind::ATTACK, false },
     { AttributeGainKind::CRITICAL, true },
     { AttributeGainKind::CRITICAL_POWER, false },
     { AttributeGainKind::OVERCOME, false },
     { AttributeGainKind::STRAIN, true },
     { AttributeGainKind::SURPLUS, false },
     { AttributeGainKind::HASTE, true },
     { AttributeGainKind::WEAPON, false } }
};

template <class Config>
constexpr bool RerunsRotation(AttributeGainSpec gain)
{
    using Module = typename Config::RuntimeModule;
    if constexpr (requires { Module::replay_strain; }) {
        if (gain.kind == AttributeGainKind::STRAIN) {
            return !Module::replay_strain;
        }
    }
    return gain.rerun_rotation;
}

template <class Config, bool Rerun>
consteval auto GainIndexes()
{
    constexpr std::size_t count = [] {
        std::size_t result = 0;
        for (const auto &gain : AttributeGains) {
            result += RerunsRotation<Config>(gain) == Rerun;
        }
        return result;
    }();
    std::array<int, count> result{};
    std::size_t            cursor = 0;
    for (std::size_t i = 0; i < AttributeGains.size(); ++i) {
        if (RerunsRotation<Config>(AttributeGains[i]) == Rerun) {
            result[cursor++] = static_cast<int>(i);
        }
    }
    return result;
}
template <class Config>
inline constexpr auto DamageOnlyGainIndexes = GainIndexes<Config, false>();
template <class Config>
inline constexpr auto RotationGainIndexes = GainIndexes<Config, true>();
template <class Config>
inline constexpr std::uint64_t AttributeGainSimulationStages = 1 + RotationGainIndexes<Config>.size();

template <class Config>
bool RerunsRotation(const Config &config, AttributeGainSpec gain)
{
    if constexpr (requires { config.HasAdaptiveOvercome(); }) {
        if (gain.kind == AttributeGainKind::OVERCOME && config.HasAdaptiveOvercome()) return true;
    }
    return RerunsRotation<Config>(gain);
}

template <class Config>
std::uint64_t GainSimulationStages(const Config &config)
{
    return 1 + std::ranges::count_if(AttributeGains, [&](auto gain) { return RerunsRotation(config, gain); });
}
inline constexpr int           AttributeGainIncrement        = 100;
using AttributeGainIncrements                                = std::array<int, AttributeGains.size()>;
inline constexpr AttributeGainIncrements DefaultGainIncrements{ 100, 100, 100, 100, 100, 100, 100, 100 };

template <class Config>
Config ApplyAttributeGain(Config config, AttributeGainKind kind, int increment = AttributeGainIncrement)
{
    if (increment <= 0) {
        throw std::invalid_argument("attribute gain increment must be positive");
    }
    auto &a = config.attributes;
    switch (kind) {
        case AttributeGainKind::ATTACK: a.attack_base += increment; break;
        case AttributeGainKind::CRITICAL: a.crit += increment; break;
        case AttributeGainKind::CRITICAL_POWER: a.crit_power += increment; break;
        case AttributeGainKind::OVERCOME:
            if constexpr (requires { a.overcome_base; }) {
                if (a.overcome_base >= 0) {
                    a.overcome_base += increment;
                } else {
                    a.overcome += increment;
                }
            } else {
                a.overcome += increment;
            }
            break;
        case AttributeGainKind::STRAIN: a.strain += increment; break;
        case AttributeGainKind::SURPLUS: a.surplus += increment; break;
        case AttributeGainKind::HASTE: a.haste += increment; break;
        case AttributeGainKind::WEAPON: a.weapon += increment; break;
        default: throw std::out_of_range("invalid attribute gain kind");
    }
    return config;
}

template <class Config>
Config ApplyAttributeGain(Config config, int index, int increment = AttributeGainIncrement)
{
    if (index < 0 || std::cmp_greater_equal(index, AttributeGains.size())) {
        throw std::out_of_range("invalid attribute gain index");
    }
    return ApplyAttributeGain(std::move(config), AttributeGains[index].kind, increment);
}

template <class Config>
class DamageGainObserver
{
public:
    using Reducer = typename Config::RuntimeModule::DamageReducer;

    DamageGainObserver(const Config &config, std::span<const int> indexes, const AttributeGainIncrements &increments = DefaultGainIncrements)
    {
        reducers_.reserve(indexes.size());
        for (const auto INDEX : indexes) {
            if (INDEX < 0 || std::cmp_greater_equal(INDEX, AttributeGains.size()) || RerunsRotation(config, AttributeGains[INDEX]))
            {
                throw std::invalid_argument("rotation-sensitive attribute requires simulation");
            }
            reducers_.emplace_back(PrepareConfiguration(ApplyAttributeGain(config, INDEX, increments[INDEX])));
        }
    }

    std::size_t Count() const { return reducers_.size(); }

    template <class Simulation>
    void Setup(const Simulation &simulation)
    {
        const auto CAPACITY = simulation.Log().DamageCapacity();
        statistics_.Reserve(CAPACITY);
        for (auto &reducer : reducers_) {
            reducer.Setup(CAPACITY);
        }
    }

    template <class Simulation>
    void operator()(const Simulation &simulation, std::span<value_t> output)
    {
        if (output.size() != reducers_.size()) {
            throw std::invalid_argument("invalid damage gain output size");
        }
        statistics_.Collect(simulation.Log().Intents());
        for (std::size_t i = 0; i < reducers_.size(); ++i) {
            auto &reducer = reducers_[i];
            reducer.Bind(simulation.GetRules());
            output[i] = statistics_.Reduce(reducer);
        }
    }

private:
    std::vector<Reducer>                                             reducers_;
    decltype(MakeDamageStatistics<typename Config::RuntimeModule>()) statistics_;
};

struct AttributeGainBatch
{
    BatchStats                                                   primary;
    std::array<std::optional<BatchStats>, AttributeGains.size()> gains;
};

template <class Config, class Reducer = detail::TotalDamageReducer, bool Measure = false>
AttributeGainBatch RunAttributeGainsControlled(
    const MacroProgram            &program,
    const BatchOptions            &options,
    BatchControl                  &control,
    const Config                  &config,
    Reducer                        reducer    = {},
    const AttributeGainIncrements &increments = DefaultGainIncrements,
    BatchProfile                  *profile    = nullptr)
{
    for (const auto increment : increments) {
        if (increment <= 0) {
            throw std::invalid_argument("attribute gain increment must be positive");
        }
    }
    control.completed.store(0, std::memory_order_relaxed);
    const auto                 RULES = LoadSpecialization(config);
    std::vector<int> replay_indexes, rerun_indexes;
    for (int i = 0; i < static_cast<int>(AttributeGains.size()); ++i) {
        (RerunsRotation(config, AttributeGains[i]) ? rerun_indexes : replay_indexes).push_back(i);
    }
    DamageGainObserver<Config> observer(config, replay_indexes, increments);
    const auto                 observed_count = observer.Count();
    auto                       combined =
        detail::RunBatchObservedImpl<Measure>(program, options, control, RULES, observed_count, 1, std::move(observer), std::move(reducer), profile);
    AttributeGainBatch result{ combined.first, {} };
    if (result.primary.count != options.iterations) {
        return result;
    }
    for (std::size_t i = 0; i < replay_indexes.size(); ++i) {
        result.gains[replay_indexes[i]] = combined.second[i];
    }
    for (const auto INDEX : rerun_indexes) {
        if (control.cancel_requested.load(std::memory_order_relaxed)) {
            break;
        }
        const auto  CHANGED_RULES = LoadSpecialization(ApplyAttributeGain(config, INDEX, increments[INDEX]));
        const char *stage =
            AttributeGains[INDEX].kind == AttributeGainKind::CRITICAL ? "会心重跑"
            : AttributeGains[INDEX].kind == AttributeGainKind::HASTE
                ? "加速重跑"
                : "属性重跑";
        const auto GAIN =
            detail::RunBatchImpl<true, Measure>(program, options, CHANGED_RULES, &control, detail::TotalDamageReducer{}, profile, stage);
        if (GAIN.count == options.iterations) {
            result.gains[INDEX] = GAIN;
        }
    }
    return result;
}

} // namespace JX3DPS::runtime
