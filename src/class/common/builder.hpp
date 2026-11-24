#pragma once

#include "book.hpp"

namespace JX3DPS::runtime::season50 {
class Builder
{
    Book b_;

    Slot skill(std::string_view name)
    {
        for (Slot i = 0; i < b_.skills.size(); ++i) {
            if (b_.skills[i].name == name) {
                return i;
            }
        }
        b_.skills.push_back({ 0, name, 0, 0, 0 });
        b_.hits.emplace_back();
        b_.actions.emplace_back();
        b_.skill_aliases.push_back(name);
        return static_cast<Slot>(b_.skills.size() - 1);
    }

public:
    Builder(std::string_view code, std::string_view name, std::string_view macro)
    {
        b_.code          = code;
        b_.name          = name;
        b_.default_macro = macro;
    }

    Book &definition() { return b_; }

    void resource(Resource r) { b_.resources.push_back(r); }

    void adjust(void (*fn)(Prepared &)) { b_.adjust = fn; }

    void default_talents(std::initializer_list<std::string_view> values) { b_.default_talents = values; }

    void preset(int major, Attributes attributes, std::initializer_list<std::string_view> recipes)
    {
        b_.default_major      = major;
        b_.default_attributes = attributes;
        b_.default_recipes    = recipes;
    }

    void state(BuffDescriptor d, bool target, bool hasted = false, bool keep_next = false)
    {
        b_.buffs.push_back(d);
        b_.buff_aliases.push_back(d.name);
        b_.auras.push_back({ target, INVALID_SLOT, hasted, keep_next, {} });
    }

    Slot buff(std::string_view n) const
    {
        for (Slot i = 0; i < b_.buffs.size(); ++i) {
            if (b_.buffs[i].name == n) {
                return i;
            }
        }
        throw std::logic_error(std::string("unknown state: ") + std::string(n));
    }

    Slot effect(std::string_view n) { return skill(n); }

    void dot(std::string_view name, std::string_view damage) { b_.auras[buff(name)].damage = skill(damage); }

    void accumulate_ticks(std::string_view aura, int limit) { b_.auras[buff(aura)].max_remaining_ticks = limit; }

    void independent_dot_hits(std::string_view aura) { b_.auras[buff(aura)].separate_stack_hits = true; }

    void extra_tick(std::string_view name, std::string_view talent, int count = 1)
    {
        auto &aura             = b_.auras[buff(name)];
        aura.extra_tick_talent = talent;
        aura.extra_ticks       = count;
    }

    void tick_schedule(std::string_view aura, int first, int padding)
    {
        auto &a            = b_.auras[buff(aura)];
        a.first_tick       = first;
        a.interval_padding = padding;
    }

    void charge_variant(std::string_view n, std::string_view talent, int cd, int charges)
    {
        auto slot  = skill(n);
        auto a     = b_.actions[slot];
        auto d     = b_.skills[slot];
        a.charges  = charges;
        d.cooldown = cd;
        b_.variants.push_back({ slot, talent, d, std::move(a) });
    }

    void pulse(std::string_view name, std::initializer_list<Step> s) { b_.auras[buff(name)].pulse = s; }
    void on_expiry(std::string_view name, std::initializer_list<Step> steps) { b_.auras[buff(name)].expire = steps; }
    void stance_haste(std::string_view stance, int bonus, std::string_view swing = {})
    {
        b_.haste_stance = buff(stance);
        b_.stance_haste = bonus;
        if (!swing.empty()) b_.auras[buff(swing)].stance_timing = true;
    }
    void damage(std::uint64_t id, std::string_view name, Hit h)
    {
        const auto slot    = skill(name);
        // Composite combat-log keys are not game skill IDs.
        b_.skills[slot].id = id <= std::numeric_limits<int>::max() ? static_cast<int>(id) : 0;
        b_.hits[slot]      = std::move(h);
    }

    void initial(std::string_view state) { b_.initial_auras.push_back(buff(state)); }

    void cast(std::string_view name, int cd, int gcd, int prepare, Action a, std::vector<Step> steps)
    {
        const auto slot  = skill(name);
        auto      &d     = b_.skills[slot];
        d.cooldown       = cd;
        d.gcd            = gcd;
        d.cast_time      = prepare;
        a.impact         = std::move(steps);
        b_.actions[slot] = std::move(a);
        b_.castable.push_back(slot);
    }

    Step hit(std::string_view n, int count = 1) { return { Op::Hit, skill(n), count }; }

    Step hits_per_point(std::string_view n, int resource) { return { Op::HitsPerPoint, skill(n), resource }; }

    Step hits_per_stack(std::string_view n, std::string_view aura)
    {
        return { Op::HitsPerStack, skill(n), buff(aura) };
    }

    void pre_damage(std::string_view name, std::initializer_list<Step> steps)
    {
        auto &out = b_.hits.at(skill(name)).pre_damage;
        out.insert(out.end(), steps.begin(), steps.end());
    }

    void post_damage(std::string_view n, std::initializer_list<Step> steps)
    {
        auto &out = b_.hits.at(skill(n)).post_damage;
        out.insert(out.end(), steps.begin(), steps.end());
    }

    void pre_cast(std::string_view name, std::initializer_list<Step> steps)
    {
        const auto slot   = skill(name);
        const auto append = [&](Action &a) {
            a.pre_cast.insert(a.pre_cast.end(), steps.begin(), steps.end());
        };
        append(b_.actions[slot]);
        for (auto &v : b_.variants) {
            if (v.slot == slot) {
                append(v.action);
            }
        }
    }

    Step build_charge(std::string_view meter, int threshold, std::string_view charge)
    {
        Step s{ Op::BuildCharge, buff(meter), threshold };
        s.extra = buff(charge);
        return s;
    }

    Step shorten(std::string_view name, int frames) { return { Op::ShortenCd, skill(name), frames }; }

    Step at_least(std::string_view aura, int layers, Step step)
    {
        step.condition  = buff(aura);
        step.min_stacks = layers;
        return step;
    }

    Step convert(std::string_view from, std::string_view to)
    {
        return { Op::ConvertStacks, buff(from), 1, INVALID_SLOT, false, {}, buff(to) };
    }

    void repeat_hits(std::string_view action, std::string_view effect, int first, int spacing, int count, std::string_view choice = {})
    {
        for (int i = 0; i < count; ++i) {
            const int  frame = first + i * spacing;
            const auto step  = frame ? later(effect, frame) : hit(effect);
            b_.actions[skill(action)].impact.push_back(selected(choice, step));
        }
    }

    void post_cast(std::string_view name, std::initializer_list<Step> steps)
    {
        const auto slot   = skill(name);
        const auto append = [&](Action &a) {
            a.post_cast.insert(a.post_cast.end(), steps.begin(), steps.end());
        };
        append(b_.actions[slot]);
        for (auto &v : b_.variants) {
            if (v.slot == slot) {
                append(v.action);
            }
        }
    }

    Step stacks(std::string_view n, int count, Step step)
    {
        step.condition    = buff(n);
        step.exact_stacks = count;
        return step;
    }

    void variant(std::string_view name, std::string_view talent, int cd, int prepare, Action action, std::vector<Step> impact, std::string_view other = {})
    {
        const auto slot       = skill(name);
        auto       descriptor = b_.skills.at(slot);
        descriptor.cooldown   = cd;
        descriptor.cast_time  = prepare;
        action.impact         = std::move(impact);
        b_.variants.push_back({ slot, talent, descriptor, std::move(action), other });
    }

    void defer_swing(std::string_view name, std::string_view timer)
    {
        const auto slot = skill(name), aura = buff(timer);
        b_.actions[slot].defer_swing = aura;
        for (auto &v : b_.variants) if (v.slot == slot) v.action.defer_swing = aura;
    }

    void passive(std::string_view name, std::string_view talent, Stats stats, double channel_scale = 1.)
    {
        b_.hits.at(skill(name)).modifiers.push_back({ .aura = INVALID_SLOT, .stacks = 0, .stats = stats, .choice = talent, .channel_scale = channel_scale });
    }

    void legendary_damage(std::initializer_list<std::string_view> hits)
    {
        for (auto name : hits) passive(name, Legendary, { .damage = 51 });
    }

    void no_cooldown_during(std::string_view name, std::string_view window, bool instant = false, bool capture = false)
    {
        const auto slot = skill(name), aura = buff(window);
        const auto set = [&](Action &action) {
            action.bypass_cooldown_aura = aura;
            if (instant) action.alternate_instant = aura;
            if (capture) action.capture = aura;
        };
        set(b_.actions[slot]);
        for (auto &v : b_.variants) if (v.slot == slot) set(v.action);
    }

    void resource_modifier(std::string_view name, Slot resource, int low, int high, Stats stats, std::string_view talent, double channel_scale = 1.)
    {
        pre_damage(name, { INVALID_SLOT, 0, stats, talent, resource, low, high, channel_scale });
    }

    void pre_damage(std::string_view name, Modifier mod) { b_.hits.at(skill(name)).modifiers.push_back(mod); }

    // All combat effects currently defined in this school, including later child hits registered before this call.
    void school_modifier(Modifier modifier)
    {
        for (auto &hit : b_.hits) {
            if (hit.channel || hit.weapon || hit.fixed || !hit.rank_channels.empty()) hit.modifiers.push_back(modifier);
        }
    }

    void inherit_recipes(std::string_view name, std::string_view other)
    {
        for (const auto &mod : b_.hits.at(skill(other)).modifiers) {
            if (mod.aura == INVALID_SLOT && std::ranges::find(b_.recipes, mod.choice) != b_.recipes.end()) {
                pre_damage(name, mod);
            }
        }
    }

    Step later(std::string_view n, int frames) { return { Op::DelayedHit, skill(n), frames }; }

    Step proc(std::string_view n, int per1024) { return { Op::ProcHit, skill(n), per1024 }; }

    Step ranked_pulse(std::string_view n) { return { Op::PulseHit, skill(n) }; }

    void ranks(std::string_view n, std::initializer_list<int> channels)
    {
        b_.hits.at(skill(n)).rank_channels = channels;
    }

    void pet_attack(std::string_view n, int basis_points)
    {
        auto &hit               = b_.hits.at(skill(n));
        hit.inherited_attack_bp = basis_points;
        hit.triggers_equipment  = false;
    }

    void rank_bases(std::string_view name, std::initializer_list<std::pair<int, int>> values)
    {
        b_.hits[skill(name)].rank_base_spread = values;
    }
    void secondary_damage(std::uint64_t id, std::string_view name, Hit hit)
    {
        hit.triggers_equipment = false;
        damage(id, name, std::move(hit));
    }

    Step apply(std::string_view n, int count = 1) { return { Op::Add, buff(n), count }; }
    Step refresh_existing(std::string_view n) { return { Op::RefreshExisting, buff(n) }; }

    Step apply_for(std::string_view n, tick_t duration)
    {
        auto step     = apply(n);
        step.duration = duration;
        return step;
    }

    Step resource_aura(std::string_view n, int resource) { return { Op::ResourceAura, buff(n), resource }; }

    Step resource_stacks(std::string_view n, int resource) { return { Op::ResourceStacks, buff(n), resource }; }

    Step remove(std::string_view n, int count = 1) { return { Op::Remove, buff(n), count }; }

    Step burst(std::string_view n) { return { Op::Burst, buff(n) }; }

    Step advance(std::string_view n, int ticks) { return { Op::AdvanceTicks, buff(n), ticks }; }

    Step extra(std::string_view n) { return { Op::ExtraTick, buff(n) }; }


    Step on_crit(std::string_view n) { return { Op::AddOnCrit, buff(n) }; }

    Step within(int range, Step s)
    {
        s.range = range;
        return s;
    }

    Step life(int lo, int hi, Step s)
    {
        s.life_low  = lo;
        s.life_high = hi;
        return s;
    }

    Step gain(int slot, int amount) { return { Op::Resource, static_cast<Slot>(slot), amount }; }

    Step cycle(std::string_view first, int count) { return { Op::CycleHit, skill(first), count }; }

    Step tier(std::string_view first, int resource) { return { Op::ResourceHit, skill(first), resource }; }

    Step reset(std::string_view n) { return { Op::ResetCd, skill(n) }; }


    Step when(std::string_view n, Step s, bool absent = false)
    {
        s.condition = buff(n);
        s.absent    = absent;
        return s;
    }

    Step selected(std::string_view n, Step s)
    {
        s.other_choice = s.choice;
        s.choice       = n;
        return s;
    }

    Step captured(Step s)
    {
        s.captured = true;
        return s;
    }

    Action &base_action(std::string_view name) { return b_.actions[skill(name)]; }

    Step at(int resource, int low, int high, Step s)
    {
        s.resource = static_cast<Slot>(resource);
        s.minimum  = low;
        s.maximum  = high;
        return s;
    }

    void choices(std::initializer_list<std::string_view> talents,
                 std::initializer_list<std::string_view> recipes,
                 std::initializer_list<std::string_view> equipment = {})
    {
        b_.talents   = talents;
        b_.recipes   = recipes;
        b_.equipment = equipment;
    }

    void recipe_group(std::string_view name, std::string_view group) { b_.recipe_groups.emplace_back(name, group); }

    void recipe(std::string_view group, std::string_view name, Stats delta, std::initializer_list<std::string_view> hits, double channel_scale = 1.)
    {
        if (std::ranges::find(b_.recipes, name) == b_.recipes.end()) {
            b_.recipes.push_back(name);
            recipe_group(name, group);
        }
        for (const auto hit : hits) {
            auto found = std::ranges::find(b_.skills, hit, &SkillDescriptor::name);
            if (found == b_.skills.end()) {
                throw std::logic_error("recipe references an unknown damage event");
            }
            b_.hits.at(found - b_.skills.begin()).modifiers.push_back({ .aura = INVALID_SLOT, .stacks = 0, .stats = delta, .choice = name, .channel_scale = channel_scale });
        }
    }

    void single_target_recipe(std::string_view group, std::string_view name, std::string_view note)
    {
        recipe(group, name, {}, {});
        b_.single_target_recipe_notes.emplace(name, note);
    }
    void preset_recipe(std::string_view group, std::string_view name)
    {
        recipe(group, name, {}, {});
        b_.preset_only_recipes.push_back(name);
    }

    void timing_adjustment(std::string_view talent, std::string_view action, int cooldown, int cast_time = 0)
    {
        b_.timing_adjustments.push_back({ talent, skill(action), cooldown, cast_time });
    }

    void timing_recipe(std::string_view group, std::string_view name, std::string_view action, int cooldown, int cast_time)
    {
        recipe(group, name, {}, {});
        timing_adjustment(name, action, cooldown, cast_time);
    }

    void alias(std::string_view name, std::string_view alias) { b_.skill_aliases.at(skill(name)) = alias; }

    void set_bonus(Stats bonus, std::initializer_list<std::string_view> names, double channel_scale = 1.)
    {
        if (std::ranges::find(b_.equipment, ClassSet) == b_.equipment.end()) {
            b_.equipment.push_back(ClassSet);
        }
        for (auto name : names) {
            passive(name, ClassSet, bonus, channel_scale);
        }
    }

    void default_gear(std::initializer_list<std::string_view> names) { b_.default_equipment = names; }

    Book finish()
    {
        state({29608, "神兵·无双", 96, 0, 5}, false);
        b_.legendary_strain = buff("神兵·无双");
        for (const auto &tier : LegendaryWeapons) b_.equipment.push_back(tier.name);
        for (const auto &item : InstanceEquipment) b_.equipment.push_back(item.name);
        state({0, "见天心·会心", 320, 0, 1}, false);
        state({0, "见天心·破防", 320, 0, 1}, false);
        state({0, "疏雨落", 160, 0, 1}, false);
        state({0, "落潮平", 160, 0, 1}, false);
        state({34831, "无观囊·攻击", 96, 0, 1}, false);
        state({29524, "无观鞋·会心", 160, 0, 1}, false);
        state({29526, "无观鞋·破防", 160, 0, 1}, false);
        state({31978, "速·震", 192, 16, 1}, true);
        secondary_damage(42768, "速·震", {.fixed = 1375, .ticks = 12, .fixed_equipment_damage = true});
        dot("速·震", "速·震");
        secondary_damage(38966, "无观·荒·13档", {.fixed = 33638, .fixed_equipment_damage = true});
        secondary_damage(38966, "无观·荒·14档", {.fixed = 39244, .fixed_equipment_damage = true});
        Hit bracer{};
        bracer.fixed = 46000;
        bracer.fixed_equipment_damage = true;
        secondary_damage(40789, "巽·晚山青", bracer);
        secondary_damage(29541, "逐云寒蕊", {.fixed_equipment_damage = true});
        state({ 15455, "腰大附魔·1%", 128, 0, 1 }, false);
        state({ 15455, "腰大附魔·5%", 128, 0, 1 }, false);
        state({ 29268, "风·灭虚", 240, 0, 1 }, false);
        state({ 4761, "水·灭虚", 96, 0, 10 }, false);
        b_.water        = buff("水·灭虚");
        b_.belt_low     = buff("腰大附魔·1%");
        b_.belt_high    = buff("腰大附魔·5%");
        const auto wind = buff("风·灭虚");
        for (auto &hit : b_.hits) {
            hit.modifiers.push_back({ b_.belt_low, 0, { .final_damage = 10 }, BeltEnchant });
            hit.modifiers.push_back({ b_.belt_high, 0, { .final_damage = 51 }, BeltEnchant });
            hit.modifiers.push_back({ wind, 0, { .overcome_flat = 931 }, WindPendant });
        }
        for (std::size_t i = 0; i < std::size(DamageEnchants); ++i) {
            const auto &enchant = DamageEnchants[i];
            Hit         hit{};
            hit.fixed                  = enchant.base;
            hit.fixed_equipment_damage = true;
            hit.triggers_equipment     = false;
            damage(enchant.skill_id, enchant.name, std::move(hit));
            b_.enchant_hits[i] = effect(enchant.name);
            b_.equipment.push_back(enchant.name);
        }
        b_.equipment.push_back(BeltEnchant);
        b_.equipment.push_back(WindPendant);
        b_.equipment.push_back(WaterWeapon);
        cast("使用风腰坠", 2880, 0, 0, { .choice = WindPendant }, { apply("风·灭虚") });
        if (b_.default_talents.empty()) {
            b_.default_talents = b_.talents;
        }
        if (b_.resources.size() > 3) {
            throw std::logic_error("too many specialization resources");
        }
        for (auto slot : b_.castable) {
            const auto &spell = b_.skills[slot];
            const auto &a     = b_.actions[slot];
            if ((spell.gcd == 0 && spell.cooldown == 0 && spell.cast_time == 0 && a.pulses == 1) || a.gcd_group >= 8 ||
                a.charges < 1 || (a.pulses > 1 && a.spacing <= 0))
            {
                throw std::logic_error(std::string("invalid cast timing: ") + std::string(spell.name));
            }
        }
        for (std::size_t i = 0; i < b_.auras.size(); ++i) {
            if (b_.auras[i].damage != INVALID_SLOT && (!b_.buffs[i].interval || !b_.hits.at(b_.auras[i].damage).ticks))
            {
                throw std::logic_error(std::string("invalid periodic damage: ") + std::string(b_.buffs[i].name));
            }
        }
        return std::move(b_);
    }
};
} // namespace JX3DPS::runtime::season50
