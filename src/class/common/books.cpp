#include <set>
#include <cmath>
#include <stdexcept>

#include "book.hpp"
#include "damage.hpp"
#include "permanents.hpp"

namespace JX3DPS::runtime::season50 {
// CMake discovers the individual school factories from src/class/*.cpp.
#define JX3DPS_SCHOOL(name) Book name();
#include "school_factories.inc"
#undef JX3DPS_SCHOOL

std::span<const Book> Books()
{
    static const auto books = [] {
        static constexpr auto factories = std::array{
#define JX3DPS_SCHOOL(name) &name,
#include "school_factories.inc"
#undef JX3DPS_SCHOOL
        };
        std::vector<Book> result;
        result.reserve(factories.size());
        for (auto create : factories) result.push_back(create());
        std::ranges::sort(result, {}, [](const Book &b) { return std::pair{b.display_order, b.code}; });
        std::set<std::string_view> keys;
        for (const auto &b : result) {
            if (b.code.empty() || !keys.insert(b.code).second || (!b.alias.empty() && !keys.insert(b.alias).second))
                throw std::logic_error("missing or duplicate school code: " + std::string(b.code));
        }
        return result;
    }();
    return books;
}

const Book *Find(std::string_view code)
{
    for (const auto &b : Books()) {
        if (b.code == code || (!b.alias.empty() && b.alias == code)) return &b;
    }
    return nullptr;
}

Preparation Prepare(const Config &input)
{
    if (!input.book) {
        throw std::invalid_argument("missing specialization");
    }
    Config config = input;
    for (auto &recipe : config.recipes) {
        for (const auto &[old_name, new_name] : config.book->recipe_aliases)
            if (recipe == old_name) { recipe = new_name; break; }
    }
    if (config.gear_score && (*config.gear_score < 0 || *config.gear_score > 1000000))
        throw std::invalid_argument("invalid gear score");
    if (config.raid_full) config.permanents.merge(FullRaidPermanents(*config.book));
    ApplyPermanents(*config.book, config.attributes, config.base_major.value_or(config.book->default_major), config.permanents, config.raid_full);
    if (config.target.level < 50 || config.target.level > 54 || config.target.shield < 0 || config.target.shield > 100000 ||
        !std::isfinite(config.target.life) || config.target.life < 0.01 || config.target.life > 1 ||
        !std::isfinite(config.target.distance) || config.target.distance < 0 || config.target.distance > 1000)
    {
        throw std::invalid_argument("invalid level-50 target");
    }
    const auto &a = config.attributes;
    for (auto v : { a.attack_base, a.attack_by_class, a.weapon, a.crit, a.crit_power, a.overcome, a.strain, a.surplus, a.haste })
    {
        if (v < 0 || v > 1000000) {
            throw std::invalid_argument("invalid level-50 attribute");
        }
    }
    if (config.delay < 0 || config.delay > 160 || config.target.vulnerable < 0 || config.target.vulnerable > 10000 ||
        !std::isfinite(config.player_life) || config.player_life < 0 || config.player_life > 1 ||
        a.ignore_shield < 0 || a.ignore_shield > 1024 ||
        (config.initial_qidian &&
         (config.book->resources.empty() || *config.initial_qidian < config.book->resources.front().low ||
          *config.initial_qidian > config.book->resources.front().high)))
    {
        throw std::invalid_argument("invalid initial combat settings");
    }
    for (auto bonus :
         { a.attack_percent, a.overcome_percent, a.haste_percent, a.crit_basis_points, a.crit_power_percent, a.damage_percent, a.strain_percent, a.pve_percent })
    {
        if (bonus < 0 || bonus > 10000) {
            throw std::invalid_argument("invalid attribute bonus");
        }
    }
    ValidateTargetEvents(config.target_events);
    const auto &b        = *config.book;
    const auto  validate = [](const auto &known, const auto &selected) {
        for (const auto &name : selected) {
            if (std::ranges::find(known, name) == known.end()) {
                throw std::invalid_argument("unsupported selection: " + name);
            }
        }
    };
    validate(b.talents, config.talents);
    validate(b.recipes, config.recipes);
    std::unordered_map<std::string_view, int> recipe_counts;
    for (const auto &[name, group] : b.recipe_groups) {
        if (std::ranges::count(config.recipes, name) > 1) {
            throw std::invalid_argument("重复秘籍：" + std::string(name));
        }
        if (std::ranges::find(config.recipes, name) != config.recipes.end() && ++recipe_counts[group] > 4) {
            throw std::invalid_argument("同一招式最多选四本秘籍：" + std::string(group));
        }
    }
    validate(b.equipment, config.equipment);
    std::vector<std::string_view> occupied;
    for (const auto &option : config.equipment) {
        const auto slot = EquipmentSlot(b, option);
        if (slot.empty()) {
            continue;
        }
        if (std::ranges::count(occupied, slot) >= (slot == "戒指" ? 2 : 1) || std::ranges::count(config.equipment, option) > 1) {
            throw std::invalid_argument("装备部位重复或超出可穿戴数量");
        }
        occupied.push_back(slot);
    }
    const auto legendary = std::ranges::find_if(LegendaryWeapons, [&](const auto &tier) {
        return std::ranges::contains(config.equipment, tier.name);
    });
    const bool has_legendary = legendary != std::end(LegendaryWeapons);
    const auto enabled = [&](std::string_view choice) {
        if (has_legendary && (choice == Legendary || (!b.weapon_effect.empty() && choice == b.weapon_effect))) return true;
        return choice.empty() || std::ranges::find(config.talents, choice) != config.talents.end() ||
               std::ranges::find(config.recipes, choice) != config.recipes.end() ||
               std::ranges::find(config.equipment, choice) != config.equipment.end();
    };
    auto p       = std::make_shared<Prepared>();
    // Flat item ratings are added before percentage modifiers. Lock the necklace and crown choices on entering combat.
    if (enabled("残阳深")) { config.attributes.overcome += 219; config.attributes.crit += 219; }
    if (enabled("陌微尘")) config.attributes.strain += 313;
    if (enabled("无观冠·适应之力")) {
        auto &higher = config.attributes.overcome >= config.attributes.crit ? config.attributes.overcome : config.attributes.crit;
        higher += 563;
    }
    if (enabled("无观项链·破防转攻击")) config.attributes.attack_base += std::min<value_t>(68, a.overcome / 10 * 2);
    if (enabled("无观项链·会心转无双")) config.attributes.strain += std::min<value_t>(470, a.crit / 10 * 2);
    p->config    = config;
    p->resources = b.resources;
    if (has_legendary) p->legendary_strain_per_stack = legendary->strain_per_stack;
    const auto aura = [&](std::string_view name) { return static_cast<Slot>(std::ranges::find(b.buffs, name, &BuffDescriptor::name) - b.buffs.begin()); };
    const auto hit_slot = [&](std::string_view name) { return static_cast<Slot>(std::ranges::find(b.skills, name, &SkillDescriptor::name) - b.skills.begin()); };
    if (enabled("晚山青")) p->bracer_hit = hit_slot("巽·晚山青");
    if (config.raid_full) {
        const auto &formation = config.permanents.at("阵法");
        if (formation == "凌雪阵（常驻）") p->raid_formation = Prepared::RaidFormation::LingXue;
        if (formation == "蓬莱阵" && formation == b.raid.own_formation) p->raid_formation = Prepared::RaidFormation::PengLai;
        if (formation == "易筋阵（常驻）") p->raid_formation = Prepared::RaidFormation::YiJin;
        p->raid_pollen = hit_slot("逐云寒蕊");
        p->raid_pollen_damage = static_cast<int>(100 * std::max(18., config.gear_score.value_or(b.raid.gear_score) * .0020416667 - 22.9166667));
        p->raid_hanxiao_stacks = b.raid.hanxiao_stacks;
    }
    if (enabled("动寒星") || enabled("满庭月") || enabled("影凋零") || enabled("月照庭")) p->speed_dot = aura("速·震");
    if (enabled("无观鞋·会心")) p->shoe_crit = aura("无观鞋·会心");
    if (enabled("无观鞋·破防")) p->shoe_overcome = aura("无观鞋·破防");
    if (enabled("无观囊·伤害13档")) p->trinket_hit = hit_slot("无观·荒·13档");
    if (enabled("无观囊·伤害14档")) p->trinket_hit = hit_slot("无观·荒·14档");
    if (enabled("疏雨落")) p->ring_overcome = aura("疏雨落");
    if (enabled("落潮平")) p->ring_crit = aura("落潮平");
    if (enabled("见天心")) { p->adaptive_crit = aura("见天心·会心"); p->adaptive_overcome = aura("见天心·破防"); }
    if (enabled("无观囊·加速攻击")) {
        p->attack_trinket = aura("无观囊·攻击");
        p->attack_trinket_duration = a.haste >= 2024 ? 176 : a.haste >= 1452 ? 160 : a.haste >= 928 ? 136 : a.haste >= 445 ? 120 : 96;
    }
    for (std::size_t i = 0; i < std::size(DamageEnchants); ++i) {
        const auto &enchant = DamageEnchants[i];
        if (!enabled(enchant.name)) {
            continue;
        }
        auto &proc = enchant.slot == "伤腕" ? p->wrist_proc : p->shoe_proc;
        proc       = b.enchant_hits[i];
    }
    if (enabled(BeltEnchant)) {
        p->belt_low  = b.belt_low;
        p->belt_high = b.belt_high;
    }
    if (enabled(WaterWeapon)) {
        p->water = b.water;
    }
    p->crit_basis_points = static_cast<int>(a.crit * 10000. / (9.609 * 990)) + a.crit_basis_points;
    p->haste    = 1. + std::min(256, static_cast<int>(a.haste * 1024. / (10.21 * 990)) + a.haste_percent) / 1024.;
    p->stance_haste = b.stance_haste;
    auto select = [&](auto &out, const auto &in) {
        auto &steps = out.emplace_back();
        for (const auto &s : in) {
            if (enabled(s.choice) && enabled(s.other_choice)) {
                steps.push_back(s);
            }
        }
    };
    for (const auto &buff : b.buffs) {
        p->aura_stack_caps.push_back(buff.max_stacks);
        p->base_intervals.push_back(buff.interval);
    }
    for (const auto &aura : b.auras) {
        p->dot_effects.push_back(aura.damage);
        p->dot_ticks.push_back(aura.damage == INVALID_SLOT
                                   ? 0
                                   : b.hits[aura.damage].ticks +
                                         (!aura.extra_tick_talent.empty() && enabled(aura.extra_tick_talent) ? aura.extra_ticks : 0));
        select(p->pulses, aura.pulse);
        select(p->expires, aura.expire);
    }
    if (p->speed_dot != INVALID_SLOT) {
        const auto h = a.haste;
        p->dot_ticks[p->speed_dot] = h <= 444 ? 12 + 3 * h / 444 : h <= 928 ? 15 + 2 * (h - 444) / 484 :
                                     h <= 1451 ? 17 + 3 * (h - 928) / 523 : h <= 2024 ? 20 + 2 * (h - 1451) / 573 : 22;
    }
    for (std::size_t i = 0; i < b.actions.size(); ++i) {
        const Action          *active = &b.actions[i];
        const SkillDescriptor *skill  = &b.skills[i];
        for (const auto &v : b.variants) {
            if (v.slot == i && enabled(v.talent) && enabled(v.other_talent)) {
                active = &v.action;
                skill  = &v.skill;
            }
        }
        p->actions.push_back(active);
        p->skills.push_back(*skill);
        p->gcd_reduction.push_back(0);
        p->cooldown_reduction.push_back(0);
        p->ranges.push_back(active->range);
        const auto &action = *active;
        p->enabled_actions.push_back(enabled(action.choice));
        p->mana_cost_quarters.push_back(action.minimum_mana_quarters);
        select(p->impacts, action.impact);
        select(p->pre_cast, action.pre_cast);
        select(p->post_cast, action.post_cast);
    }
    for (const auto &h : b.hits) {
        select(p->pre_damage, h.pre_damage);
        select(p->post_damage, h.post_damage);
        auto &mods    = p->modifiers.emplace_back();
        auto &passive = p->passive.emplace_back(h.passive);
        Add(passive,
            Stats{ .attack   = a.attack_percent,
                   .power    = a.crit_power_percent,
                   .damage   = a.damage_percent,
                   .pve      = a.pve_percent,
                   .ignore   = a.ignore_shield,
                   .strain   = a.strain_percent,
                   .fragile  = config.target.vulnerable,
                   .overcome = a.overcome_percent });
        for (const auto &m : h.modifiers) {
            if (config.raid_full && m.replaced_by_raid)
                continue; // The maintained raid debuff supersedes the player's copy.
            if (enabled(m.choice)) {
                if (m.aura == INVALID_SLOT && m.resource == INVALID_SLOT && m.channel_scale == 1. && m.stats.channel_override < 0) {
                    Add(passive, m.stats);
                } else {
                    mods.push_back(m);
                }
            }
        }
    }
    if (b.adjust) {
        b.adjust(*p);
    }
    for (const auto &timing : b.timing_adjustments) {
        if (enabled(timing.choice)) {
            auto &skill = p->skills[timing.skill];
            if (p->actions[timing.skill]->stance_cooldown) p->cooldown_reduction[timing.skill] -= timing.cooldown;
            else skill.cooldown = std::max(0, skill.cooldown + timing.cooldown);
            skill.cast_time = std::max(0, skill.cast_time + timing.cast_time);
        }
    }
    return p;
}
} // namespace JX3DPS::runtime::season50
