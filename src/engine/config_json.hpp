#pragma once
#include <nlohmann/json.hpp>

#include "src/class/common/book.hpp"

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
    if (e.player_life_basis_points) j["player_life"] = *e.player_life_basis_points;
    if (e.incoming_damage_basis_points) j["incoming_damage"] = *e.incoming_damage_basis_points;
}

inline void from_json(const nlohmann::json &j, TargetEvent &e)
{
    e = {};
    j.at("frame").get_to(e.frame);
    if (j.contains("life")) {
        e.life_basis_points = j.at("life").get<int>();
    }
    if (j.contains("distance")) {
        e.distance_hundredths = j.at("distance").get<int>();
    }
    if (j.contains("player_life")) e.player_life_basis_points = j.at("player_life").get<int>();
    if (j.contains("incoming_damage")) e.incoming_damage_basis_points = j.at("incoming_damage").get<int>();
}

namespace season50 {
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    Attributes,
    attack_base,
    attack_by_class,
    weapon,
    crit,
    crit_power,
    overcome,
    strain,
    surplus,
    haste,
    attack_percent,
    overcome_percent,
    haste_percent,
    crit_basis_points,
    crit_power_percent,
    damage_percent,
    strain_percent,
    pve_percent,
    ignore_shield)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Target, level, shield, life, distance, vulnerable)

inline void to_json(nlohmann::json &j, const Config &c)
{
    j = {
        { "attributes",    c.attributes    },
        { "target",        c.target        },
        { "player_life",   c.player_life   },
        { "target_events", c.target_events },
        { "talents",       c.talents       },
        { "recipes",       c.recipes       },
        { "equipment",     c.equipment     },
        { "precombat",     c.precombat     },
        { "raid_full",     c.raid_full     },
        { "permanents",    c.permanents    },
        { "delay",         c.delay         }
    };
    if (c.base_major) j["base_major"] = *c.base_major;
    if (c.gear_score) j["gear_score"] = *c.gear_score;
    if (c.initial_qidian) {
        j["initial_qidian"] = *c.initial_qidian;
    }
}

inline void from_json(const nlohmann::json &j, Config &c)
{
    if (!j.is_object()) {
        throw std::invalid_argument("expected level-50 configuration object");
    }
    for (const auto &[key, unused] : j.items()) {
        if (key != "attributes" && key != "target" && key != "player_life" && key != "target_events" && key != "talents" && key != "recipes" &&
            key != "equipment" && key != "delay" && key != "initial_qidian" && key != "permanents" && key != "base_major" && key != "precombat" && key != "raid_full" && key != "gear_score")
        {
            throw std::invalid_argument("unsupported level-50 config field: " + key);
        }
    }
    if (j.contains("delay")) {
        j.at("delay").get_to(c.delay);
    }
    if (j.contains("precombat")) j.at("precombat").get_to(c.precombat);
    if (j.contains("raid_full")) j.at("raid_full").get_to(c.raid_full);
    if (j.contains("gear_score")) {
        const auto &score = j.at("gear_score");
        if (!score.is_number_integer() || score < 0 || score > 1000000) throw std::invalid_argument("invalid gear score");
        c.gear_score = score.get<int>();
    }
    if (j.contains("player_life")) j.at("player_life").get_to(c.player_life);
    if (j.contains("initial_qidian")) {
        c.initial_qidian = j.at("initial_qidian").get<int>();
    }
    if (j.contains("attributes")) {
        const auto &overrides = j.at("attributes");
        if (!overrides.is_object()) throw std::invalid_argument("expected level-50 attributes object");
        nlohmann::json attributes = c.attributes;
        attributes.update(overrides);
        attributes.get_to(c.attributes);
    }
    if (j.contains("target")) {
        j.at("target").get_to(c.target);
    }
    if (j.contains("target_events")) {
        j.at("target_events").get_to(c.target_events);
    }
    if (j.contains("talents")) {
        j.at("talents").get_to(c.talents);
    }
    if (j.contains("recipes")) {
        j.at("recipes").get_to(c.recipes);
    }
    if (j.contains("equipment")) {
        j.at("equipment").get_to(c.equipment);
    }
    if (j.contains("permanents")) {
        const auto &choices = j.at("permanents");
        if (!choices.is_object()) throw std::invalid_argument("expected permanent selections by category");
        c.permanents = choices.get<std::map<std::string, std::string>>();
    }
    if (j.contains("base_major")) {
        const auto &v = j.at("base_major");
        if (!v.is_number_integer() || v < 0 || v > 1000000) throw std::invalid_argument("invalid base major attribute");
        c.base_major = v.get<int>();
    }
}
} // namespace season50
} // namespace JX3DPS::runtime
