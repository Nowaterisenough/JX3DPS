#pragma once
#include <nlohmann/json.hpp>

#include "scenarios.hpp"

namespace JX3DPS::runtime {
inline void to_json(nlohmann::json &j, const TargetEvent &e)
{
    j = {
        { "frame", e.frame }
    };
    if (e.life_basis_points) {
        j["life"] = *e.life_basis_points;
    }
    if (e.distance_hundredths) {
        j["distance"] = *e.distance_hundredths;
    }
}

inline void from_json(const nlohmann::json &j, TargetEvent &e)
{
    j.at("frame").get_to(e.frame);
    if (j.contains("life")) {
        e.life_basis_points = j.at("life").get<int>();
    }
    if (j.contains("distance")) {
        e.distance_hundredths = j.at("distance").get<int>();
    }
}

namespace tai_xu::team {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Application, kind, frame, duration, stacks)
} // namespace tai_xu::team

#define JX3_COMMON_ATTRS                                                                                                        \
    attack_base, attack_by_class, weapon, crit, crit_power, overcome, strain, surplus, haste, attack_percent, overcome_percent, \
        haste_percent, crit_basis_points, crit_power_percent, damage_percent, strain_percent, pve_percent, ignore_shield

namespace tai_xu {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Attributes, JX3_COMMON_ATTRS, overcome_base, overcome_by_class)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Target, level, shield, vulnerable, life, distance)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    Config,
    attributes,
    target,
    talents,
    equipment,
    team_effects,
    recipes,
    shengtaiji_recipes,
    renjian_recipes,
    initial_qidian,
    delay,
    team_buffs,
    attribute_version_capacity,
    target_events)
} // namespace tai_xu

namespace mo_wen {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Attributes, JX3_COMMON_ATTRS)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Target, level, shield, vulnerable, life, distance)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Config, attributes, target, talents, equipment, team_effects, recipes, initial_style, initial_yingzi, delay, target_events)
} // namespace mo_wen

#undef JX3_COMMON_ATTRS
} // namespace JX3DPS::runtime
