#ifndef DESKTOP_RUNTIME_BACKEND_H
#define DESKTOP_RUNTIME_BACKEND_H
#include <algorithm>
#include <array>
#include <memory>
#include <span>
#include <type_traits>
#include <vector>

#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"

#include "runtime_catalog.h"

namespace desktop {
// Attribute gains share one execution plan for every specialization.  The
// critical, strain and haste entries are the requested rotation-sensitive
// set; the remaining gains only change the formula for recorded events.
enum class AttributeGainKind : std::uint8_t {
    Attack,
    Critical,
    CriticalPower,
    Overcome,
    Strain,
    Surplus,
    Haste,
    Weapon,
};

struct AttributeGainSpec {
    AttributeGainKind kind;
    const char *name;
    bool rerun_rotation;
};

inline constexpr std::array<AttributeGainSpec, 8> AttributeGains{{
    {AttributeGainKind::Attack, "基础攻击", false},
    {AttributeGainKind::Critical, "会心等级", true},
    {AttributeGainKind::CriticalPower, "会效等级", false},
    {AttributeGainKind::Overcome, "破防等级", false},
    {AttributeGainKind::Strain, "无双等级", true},
    {AttributeGainKind::Surplus, "破招等级", false},
    {AttributeGainKind::Haste, "加速等级", true},
    {AttributeGainKind::Weapon, "武器伤害", false},
}};

inline constexpr std::array<int, 5> DamageOnlyGainIndexes{0, 2, 3, 5, 7};
inline constexpr std::array<int, 3> RotationGainIndexes{1, 4, 6};

// Dispatch once at the desktop boundary; each fight retains its concrete rules type.
inline auto MakeRules(const rt::tai_xu::Config &config)
{
    return rt::tai_xu::Rules(rt::tai_xu::Prepare(config));
}

inline auto MakeRules(const rt::mo_wen::Config &config)
{
    return rt::mo_wen::Rules(rt::mo_wen::Prepare(config));
}

// The desktop uses one gain pipeline for every specialization. Rules only
// provide their configuration and prepared damage implementation.
inline auto PrepareConfig(const rt::tai_xu::Config &config) { return rt::tai_xu::Prepare(config); }
inline auto PrepareConfig(const rt::mo_wen::Config &config) { return rt::mo_wen::Prepare(config); }

template <typename Config>
Config ApplyAttributeGain(Config config, AttributeGainKind kind)
{
    auto &a = config.attributes;
    switch (kind) {
        case AttributeGainKind::Attack: a.attack_base += 100; break;
        case AttributeGainKind::Critical: a.crit += 100; break;
        case AttributeGainKind::CriticalPower: a.crit_power += 100; break;
        case AttributeGainKind::Overcome:
            if constexpr (requires { a.overcome_base; }) {
                if (a.overcome_base >= 0) a.overcome_base += 100;
                else a.overcome += 100;
            } else a.overcome += 100;
            break;
        case AttributeGainKind::Strain: a.strain += 100; break;
        case AttributeGainKind::Surplus: a.surplus += 100; break;
        case AttributeGainKind::Haste: a.haste += 100; break;
        case AttributeGainKind::Weapon: a.weapon += 100; break;
    }
    return config;
}

template <typename Config>
Config ApplyAttributeGain(Config config, int index)
{
    if (index < 0 || index >= static_cast<int>(AttributeGains.size()))
        throw std::out_of_range("invalid attribute gain index");
    return ApplyAttributeGain(std::move(config), AttributeGains[static_cast<std::size_t>(index)].kind);
}

template <typename Prepared>
struct DamageReplay {
    std::vector<std::shared_ptr<const Prepared>> prepared;
    using Version = std::conditional_t<std::is_same_v<Prepared, rt::tai_xu::Prepared>,
                                       rt::tai_xu::AttributeVersion, std::uint8_t>;
    std::vector<std::vector<Version>> versions;

    std::size_t Count() const { return prepared.size(); }

    template <typename Simulation>
    void operator()(const Simulation &simulation, std::span<JX3DPS::value_t> output)
    {
        std::fill(output.begin(), output.end(), JX3DPS::value_t{});
        const auto &hits = simulation.Log().Intents();
        if constexpr (std::is_same_v<Prepared, rt::tai_xu::Prepared>) {
            const auto &base = simulation.GetRules().Versions();
            versions.resize(prepared.size());
            for (std::size_t i = 0; i < prepared.size(); ++i) {
                const auto &data = prepared[i];
                auto &mapped = versions[i];
                mapped.clear();
                mapped.reserve(base.Size());
                for (const auto &version : base)
                    mapped.push_back(data->Resolve(version.profile, version.team));
            }
            for (const auto &hit : hits) {
                const bool critical = hit.outcome == JX3DPS::RollResult::DOUBLE;
                for (std::size_t i = 0; i < prepared.size(); ++i)
                    output[i] += prepared[i]->Damage(versions[i][hit.snapshot_version],
                                                     versions[i][hit.live_version], hit.effect, critical);
            }
        } else {
            for (const auto &hit : hits)
                for (std::size_t i = 0; i < prepared.size(); ++i)
                    output[i] += prepared[i]->Reduce(hit);
        }
    }
};

template <typename Config>
auto MakeDamageReplay(const Config &config, std::span<const int> indexes)
{
    using Prepared = std::remove_const_t<std::remove_reference_t<decltype(*PrepareConfig(config))>>;
    DamageReplay<Prepared> replay;
    replay.prepared.reserve(indexes.size());
    for (const auto index : indexes)
        replay.prepared.push_back(PrepareConfig(ApplyAttributeGain(config, index)));
    return replay;
}
} // namespace desktop
#endif
