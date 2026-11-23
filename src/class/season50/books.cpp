#include <cmath>
#include <stdexcept>

#include "book.hpp"
#include "damage.hpp"
#include "permanents.hpp"

namespace JX3DPS::runtime::season50 {
namespace {
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

    void on_hit(std::string_view n, std::initializer_list<Step> steps)
    {
        auto &out = b_.hits.at(skill(n)).on_hit;
        out.insert(out.end(), steps.begin(), steps.end());
    }

    void on_cast(std::string_view name, std::initializer_list<Step> steps)
    {
        const auto slot   = skill(name);
        const auto append = [&](Action &a) {
            a.start.insert(a.start.end(), steps.begin(), steps.end());
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

    void after_cast(std::string_view name, std::initializer_list<Step> steps)
    {
        const auto slot   = skill(name);
        const auto append = [&](Action &a) {
            a.finish.insert(a.finish.end(), steps.begin(), steps.end());
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

    void passive(std::string_view name, std::string_view talent, Stats stats)
    {
        b_.hits.at(skill(name)).modifiers.push_back({ INVALID_SLOT, 0, stats, talent });
    }

    void legendary_damage(std::initializer_list<std::string_view> hits)
    {
        for (auto name : hits) passive(name, Legendary, { .damage = 51 });
    }

    void no_cooldown_during(std::string_view name, std::string_view window, bool instant = false, bool capture = false)
    {
        const auto slot = skill(name), aura = buff(window);
        const auto set = [&](Action &action) {
            action.weapon_cd = aura;
            if (instant) action.alternate_instant = aura;
            if (capture) action.capture = aura;
        };
        set(b_.actions[slot]);
        for (auto &v : b_.variants) if (v.slot == slot) set(v.action);
    }

    void resource_modifier(std::string_view name, Slot resource, int low, int high, Stats stats, std::string_view talent)
    {
        modifier(name, { INVALID_SLOT, 0, stats, talent, resource, low, high });
    }

    void modifier(std::string_view name, Modifier mod) { b_.hits.at(skill(name)).modifiers.push_back(mod); }

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
                modifier(name, mod);
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

    Step meridians(std::string_view hit, std::string_view rupture, std::string_view aura)
    {
        Step step{ Op::SpendMeridians, skill(hit) };
        step.extra        = skill(rupture);
        step.neutral_buff = buff(aura);
        return step;
    }

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

    Step temperature(int delta, std::string_view event, std::string_view boost = {}, std::string_view dot = {})
    {
        Step step{ Op::Neutralize, 0, delta, INVALID_SLOT, false, {}, skill(event) };
        if (!boost.empty()) {
            step.neutral_buff = buff(boost);
        }
        if (!dot.empty()) {
            step.neutral_dot = buff(dot);
        }
        return step;
    }

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

    void phases(std::string_view n, std::initializer_list<Step> start, std::initializer_list<Step> finish)
    {
        auto &a  = b_.actions[skill(n)];
        a.start  = start;
        a.finish = finish;
    }

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

    void recipe(std::string_view group, std::string_view name, Stats delta, std::initializer_list<std::string_view> hits)
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
            b_.hits.at(found - b_.skills.begin()).modifiers.push_back({ INVALID_SLOT, 0, delta, name });
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

    void timing_recipe(std::string_view group, std::string_view name, std::string_view action, int cooldown, int cast_time)
    {
        recipe(group, name, {}, {});
        b_.recipe_timing.push_back({ name, skill(action), cooldown, cast_time });
    }

    void alias(std::string_view name, std::string_view alias) { b_.skill_aliases.at(skill(name)) = alias; }

    void set_bonus(Stats bonus, std::initializer_list<std::string_view> names)
    {
        if (std::ranges::find(b_.equipment, ClassSet) == b_.equipment.end()) {
            b_.equipment.push_back(ClassSet);
        }
        for (auto name : names) {
            passive(name, ClassSet, bonus);
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

#include "specializations.inc"

} // namespace

std::span<const Book> Books()
{
    static const auto books = BuildBooks();
    return books;
}

const Book *Find(std::string_view code)
{
    if (code == "tx50") {
        code = "tx";
    } else if (code == "mw50") {
        code = "mw";
    }
    for (const auto &b : Books()) {
        if (b.code == code) {
            return &b;
        }
    }
    return nullptr;
}

Preparation Prepare(const Config &input)
{
    if (!input.book) {
        throw std::invalid_argument("missing specialization");
    }
    Config config = input;
    // Accept season presets saved before the two 返闭惊魂 tooltip values were corrected.
    if (config.book->code == "txj") {
        for (auto &recipe : config.recipes) {
            if (recipe == "返闭惊魂·调息减16帧") recipe = "返闭惊魂·调息减3秒";
            if (recipe == "返闭惊魂·调息减32帧") recipe = "返闭惊魂·调息减4秒";
        }
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
        const auto slot = EquipmentSlot(option);
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
        if (has_legendary && (choice == Legendary || IsSchoolWeaponEffect(choice))) return true;
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
        if (formation == "蓬莱阵" && b.code == "lhj") p->raid_formation = Prepared::RaidFormation::PengLai;
        if (formation == "易筋阵（常驻）") p->raid_formation = Prepared::RaidFormation::YiJin;
        const int default_score = b.code == "tx" ? 24440 : b.code == "fsj" ? 24030 : b.code == "fysj" ? 17785 : 17188;
        p->raid_pollen = hit_slot("逐云寒蕊");
        p->raid_pollen_damage = static_cast<int>(100 * std::max(18., config.gear_score.value_or(default_score) * .0020416667 - 22.9166667));
        p->raid_hanxiao_stacks = b.code == "mw" ? 1 : 100;
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
    for (const auto &buff : b.buffs) p->aura_stack_caps.push_back(buff.max_stacks);
    for (const auto &aura : b.auras) {
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
        select(p->starts, action.start);
        select(p->finishes, action.finish);
    }
    for (const auto &h : b.hits) {
        select(p->on_hit, h.on_hit);
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
            if (config.raid_full && m.aura != INVALID_SLOT && m.resource == INVALID_SLOT &&
                ((b.code == "axzy" && b.buffs[m.aura].name == "破风" && m.stats.shield_flat == -280) ||
                 (b.code == "fsj" && b.buffs[m.aura].name == "虚弱" && m.stats.ignore == 51)))
                continue; // The maintained raid debuff supersedes the player's copy.
            if (enabled(m.choice)) {
                if (m.aura == INVALID_SLOT && m.resource == INVALID_SLOT) {
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
    for (const auto &timing : b.recipe_timing) {
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
