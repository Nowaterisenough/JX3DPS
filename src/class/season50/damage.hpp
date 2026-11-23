#pragma once
#include <cmath>

#include "src/core/runtime/attack_coefficient.hpp"
#include "src/core/runtime/damage_log.hpp"

#include "book.hpp"

namespace JX3DPS::runtime::season50 {
constexpr int Level = 50;

constexpr int RatingScale(int level)
{
    return 33 * level - 660;
}

inline int Rating(value_t value, double divisor)
{
    return static_cast<int>(value * 1024. / (RatingScale(Level) * divisor));
}

// Same sequence as the native direct/DOT damage reducer: base, skill modifiers,
// mitigation, critical outcome, level, strain, NPC and vulnerability.
inline value_t Damage(const Config &cfg, const DamageIntent &hit, const Stats &buffs, const Stats &live)
{
    const auto &skill = cfg.book->hits[hit.skill];
    const auto &a     = cfg.attributes;
    const auto &t     = cfg.target;
    if (skill.fixed_equipment_damage) {
        const auto level_adjusted = hit.base * (100 - 5 * (t.level - Level)) / 100;
        return level_adjusted * (1024 + live.fragile) / 1024;
    }
    const double divisor = skill.physical ? 89.92 : 97.04;
    const int channel = buffs.channel_override >= 0 ? buffs.channel_override : skill.Channel(hit.level);
    const double coefficient = skill.ticks ? DotAttackCoefficient(channel, skill.ticks, skill.interval, divisor)
                                          : DirectAttackCoefficient(channel, divisor);
    const auto panel_attack = (a.attack_base + buffs.attack_flat) * (1024 + buffs.attack) / 1024 + a.attack_by_class + buffs.class_attack_flat;
    const auto attack       = panel_attack * skill.inherited_attack_bp / 10000;
    value_t    raw          = hit.base + static_cast<value_t>(attack * coefficient) + a.weapon * skill.weapon / 1024;
    raw                     = raw * (1024 + buffs.damage) / 1024 * hit.sub;
    const auto flat_shield = std::max(0, t.shield + live.shield_flat);
    const auto base_shield = flat_shield * (1024 + live.target_shield_percent) / 1024;
    const auto shield       = base_shield - base_shield * std::clamp(live.ignore, 0, 1024) / 1024;
    const auto reduction    = static_cast<int>(shield * 1024. / (shield + 10.912 * RatingScale(t.level)));
    const int  overcome     = 1024 + Rating((a.overcome + live.overcome_flat) * (1024 + live.overcome) / 1024, 10.483);
    raw                     = raw * (overcome - overcome * reduction / 1024) / 1024;
    if (hit.outcome == RollResult::DOUBLE) {
        raw = raw * 7 / 4 + raw * (std::min(1280, Rating(a.crit_power, 3.54) + buffs.power) + buffs.uncapped_power) / 1024;
    }
    raw = raw * (100 - 5 * (t.level - Level)) / 100;
    raw = raw * (1024 + Rating((a.strain + buffs.strain_flat) * (1024 + buffs.strain_rating) / 1024, 7.117) + buffs.strain) / 1024;
    raw = raw * (1024 + buffs.pve) / 1024;
    return raw * (1024 + live.fragile) / 1024 * (1024 + buffs.final_damage) / 1024;
}

inline void Add(Stats &to, const Stats &from)
{
    to.attack             += from.attack;
    to.critical           += from.critical;
    to.power              += from.power;
    to.damage             += from.damage;
    to.pve                += from.pve;
    to.ignore             += from.ignore;
    to.strain             += from.strain;
    to.fragile            += from.fragile;
    to.overcome           += from.overcome;
    to.strain_rating      += from.strain_rating;
    to.shield_flat        += from.shield_flat;
    to.target_shield_percent += from.target_shield_percent;
    if (from.channel_override >= 0) to.channel_override = from.channel_override;
    to.uncapped_power     += from.uncapped_power;
    to.final_damage       += from.final_damage;
    to.overcome_flat      += from.overcome_flat;
    to.attack_flat        += from.attack_flat;
    to.class_attack_flat  += from.class_attack_flat;
    to.strain_flat        += from.strain_flat;
}
} // namespace JX3DPS::runtime::season50
