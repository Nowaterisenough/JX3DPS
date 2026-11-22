#pragma once

#include <map>
#include <memory>
#include <span>

#include "src/core/runtime/specialization.hpp"
#include "src/core/runtime/target_timeline.hpp"

namespace JX3DPS::runtime::season50 {

// All times are frames. Percent modifiers use 1024, critical chance uses 10000.
struct Attributes
{
    value_t attack_base = 10000, attack_by_class = 0, weapon = 0;
    value_t crit = 0, crit_power = 0, overcome = 0, strain = 0, surplus = 0, haste = 0;
    int     attack_percent = 0, overcome_percent = 0, haste_percent = 0, crit_basis_points = 0, crit_power_percent = 0,
            damage_percent = 0, strain_percent = 0, pve_percent = 0, ignore_shield = 0;
};

struct Target
{
    int    level = 54, shield = 6592;
    double life = 1., distance = 4.;
    int    vulnerable = 0;
};
struct Book;
struct Module;

struct Config
{
    using RuntimeModule = Module;
    const Book                        *book{};
    tick_t                             delay = 0;
    std::optional<int>                 initial_qidian;
    Attributes                         attributes;
    Target                             target;
    std::vector<TargetEvent>           target_events;
    std::vector<std::string>           talents;
    std::vector<std::string>           recipes;
    std::vector<std::string>           equipment;
    std::map<std::string, std::string> permanents;
    std::optional<int>                 base_major;
    bool                               precombat = false;
    double                             player_life = 1.;
};

enum class Op
{
    Hit,
    HitsPerPoint,
    HitsPerStack,
    SpendStackGroups,
    RecordDistinct,
    RecordPoisonFromDots,
    SuppressCooldown,
    StackRankHit,
    StackStageHit,
    PulseHit,
    ProcHit,
    DelayedHit,
    Add,
    AddOnCrit,
    ResourceAura,
    ResourceStacks,
    Remove,
    Burst,
    AdvanceTicks,
    AdvanceAndRestartTicks,
    ManaQuarters,
    ExtraTick,
    SpendMeridians,
    Resource,
    ResourceHit,
    CycleHit,
    ResetCd,
    RollResetCd,
    ShortenCd,
    ExtendAura,
    ProlongAura,
    SpawnField,
    ExtendLatestField,
    RetimePulse,
    BuildCharge,
    ChanceAura,
    ConvertStacks,
    ToggleAura,
    FillPair,
    BreakPair,
    SpendTierHit,
    SpendForDuration,
    RefundRemainingDuration,
    RemainingTimeHit,
    SpendPairForAura,
    SpendCharge,
    GrantSeal,
    SpendSeals,
    CompressPeriodic,
    ShortenRemainingCd,
    CrossbowVolley,
    ApproachTarget,
    RefreshTwoDots,
    RefreshExisting,
    ShortenGcd,
    Neutralize
};

struct Step
{
    Op               op;
    Slot             slot;
    int              amount    = 1;
    Slot             condition = INVALID_SLOT; // Active aura, optionally inverted.
    bool             absent    = false;
    std::string_view choice{}; // Talent or recipe gating this effect.
    Slot             extra    = INVALID_SLOT;
    Slot             resource = INVALID_SLOT;
    int              minimum = 0, maximum = 100;
    int              life_low = -1, life_high = 10000; // (low, high], in basis points
    bool             captured     = false;
    Slot             neutral_buff = INVALID_SLOT, neutral_dot = INVALID_SLOT;
    int              range        = 0;
    int              exact_stacks = 0;
    int              min_stacks   = 0;
    tick_t           duration     = 0;
    std::string_view other_choice{};
    Slot             repeat_aura = INVALID_SLOT;
    bool             empowered_channel = false;
    tick_t           minimum_aura_time = 0;
    unsigned         clear_repeated = 0; // Identities forgotten on a duplicate, zero keeps the set.
    int              rank_offset = 0; // Added to the stack count at launch for a delayed hit.
    Slot             blocked_by = INVALID_SLOT;
};

struct Resource
{
    std::string_view name;
    int              initial, low, high;
    tick_t           regen_interval  = 0;
    int              regen           = 0;
    Slot             on_spend        = INVALID_SLOT;
    int              spent_per_point = 1;
    std::string_view spend_talent{};
    Slot             spent_hit   = INVALID_SLOT;
    tick_t           first_regen = 0;
    bool             mana        = false;
};

struct Stats
{
    int    attack = 0, critical = 0, power = 0, damage = 0, pve = 0, ignore = 0, strain = 0, fragile = 0, overcome = 0;
    int    strain_rating = 0, shield_flat = 0;
    double channel_multiplier              = 1.0;
    int    uncapped_power                  = 0;
    int    final_damage                    = 0;
    int    overcome_flat                   = 0;
    int    attack_flat                     = 0;
    int    class_attack_flat               = 0;
    int    strain_flat                     = 0;
    bool   operator==(const Stats &) const = default;
};

struct Modifier
{
    Slot             aura;
    int              stacks;
    Stats            stats;
    std::string_view choice{};
    Slot             resource = INVALID_SLOT;
    int              low = 0, high = 0;
};

struct Hit
{
    int                   fixed = 0, spread = 0;
    double                channel = 0;
    int                   weapon  = 0;
    int                   ticks = 0, interval = 0;
    bool                  physical = true;
    Stats                 passive{};
    std::vector<Modifier> modifiers;
    std::vector<int>      rank_channels;
    int                   inherited_attack_bp = 10000;
    std::vector<Step>     on_hit;
    bool                  fixed_equipment_damage = false;
    bool                  triggers_equipment     = true;
    std::vector<std::pair<int, int>> rank_base_spread;
};

struct Aura
{
    bool              target         = false;
    Slot              damage         = INVALID_SLOT;
    bool              hasted         = false;
    bool              keep_next_tick = false;
    // Periodic non-damage actions, e.g. recover rage and lose one stack.
    std::vector<Step> pulse;
    std::string_view  extra_tick_talent{};
    int               extra_ticks = 1;
    int               first_tick = 0, interval_padding = 0;
    bool              stance_timing = false;
    int               max_remaining_ticks = 0;
    bool              separate_stack_hits = false;
    std::vector<Step> expire;
};

struct Action
{
    Slot              resource = INVALID_SLOT;
    int               cost     = 0;
    Slot              required = INVALID_SLOT, forbidden = INVALID_SLOT;
    std::string_view  choice{};
    std::vector<Step> impact;
    int               pulses = 1, spacing = 0;
    Slot              free_cd           = INVALID_SLOT;
    bool              hasted_cd         = false;
    tick_t            lock              = 0; // Minimum action lock independent of the shared GCD.
    tick_t            chain_window      = 0;
    int               channel_cost_step = 0, extra_pulses = 0, max_cost_steps = 0;
    Slot              finisher  = INVALID_SLOT;
    int               charges   = 1;
    Slot              gcd_group = 0;
    int               range     = 0;
    Slot              capture = INVALID_SLOT, instant = INVALID_SLOT;
    bool              cooldown_on_finish = false;
    std::vector<Step> start, finish;
    // Sample once at cast start, before spending sword dance or style.
    Slot              timing_resource   = INVALID_SLOT;
    int               empowered_spacing = 0;
    bool              pulses_per_point  = false;
    bool              fixed_cast_time   = false;
    Slot              also_forbidden    = INVALID_SLOT;
    int               execute_below     = -1;
    int               points_per_pulse  = 1;
    Slot              alternate_instant = INVALID_SLOT;
    Slot              free_cost         = INVALID_SLOT;
    tick_t            channel_duration  = 0;
    int               instant_at        = 0;
    Slot              channel_buff      = INVALID_SLOT;
    int               buff_pulses       = 0;
    bool              ground_gcd = false;
    Slot              clear_on_channel_end = INVALID_SLOT;
    Slot              refund_channel_point = INVALID_SLOT;
    bool              distribute_pulses = false; // Round the full channel once and distribute leftover frames.
    bool              later_remainder = false;
    Slot              check_gcd_group = INVALID_SLOT;
    bool              shared_repayment = false;
    int               minimum_mana_quarters = 0;
    Slot              cooldown_aura = INVALID_SLOT;
    tick_t            aura_cooldown = 0;
    Slot              alternate_gcd_aura = INVALID_SLOT, alternate_gcd_group = 0;
    tick_t            minimum_aura_time = 0;
    Slot              defer_swing = INVALID_SLOT;
    bool              precombat_cast = false;
    int               required_layers = 1;
    bool              stance_cast = false;
    bool              stance_cooldown = false;
    bool              cast_while_channeling = false;
    bool              release_charged = false;
    Slot              weapon_cd = INVALID_SLOT, weapon_free_cost = INVALID_SLOT;
};

struct CastVariant
{
    Slot             slot;
    std::string_view talent;
    SkillDescriptor  skill;
    Action           action;
    std::string_view other_talent{};
};

struct EnchantTier
{
    std::string_view name, slot;
    int              skill_id, base;
};

inline constexpr EnchantTier DamageEnchants[] = {
    { "昆吾·弦刃",      "伤腕", 37562, 13524 },
    { "昆吾·弦刃·英雄", "伤腕", 37562, 19745 },
    { "刃凌",           "伤鞋", 37561, 9017  },
    { "刃凌·英雄",      "伤鞋", 37561, 13164 }
};
inline constexpr std::string_view BeltEnchant = "伤腰·不稳定增伤", WindPendant = "风特效腰坠",
                                  WaterWeapon = "水特效武器·灭虚", ClassSet = "门派四件套";

struct LegendaryTier { std::string_view name; int strain_per_stack; };
inline constexpr LegendaryTier LegendaryWeapons[]{
    {"橙武·960品", 57}, {"橙武·1030品", 64}, {"橙武·1110品", 72}, {"橙武·1170品", 77}, {"橙武·1240品", 84}
};
inline constexpr std::string_view Legendary = "50级橙武"; // Internal choice shared by all five item tiers.

struct GearOption { std::string_view name, slot; };
inline constexpr GearOption InstanceEquipment[]{
    {"残阳深", "腰带"}, {"见天心", "腰带"}, {"晚山青", "护腕"},
    {"陌微尘", "戒指"}, {"疏雨落", "戒指"}, {"落潮平", "戒指"},
    {"无观冠·适应之力", "帽子"}, {"无观囊·加速攻击", "暗器"},
    {"无观项链·破防转攻击", "项链"}, {"无观项链·会心转无双", "项链"},
    {"动寒星", "护腕"}, {"满庭月", "护腕"}, {"影凋零", "腰带"}, {"月照庭", "腰带"},
    {"无观囊·伤害13档", "暗器"}, {"无观囊·伤害14档", "暗器"},
    {"无观鞋·会心", "鞋子"}, {"无观鞋·破防", "鞋子"}
};

inline constexpr bool IsSchoolWeaponEffect(std::string_view name)
{
    return name == "神兵附伤" || name == "橙武孤锋" || name == "神兵·羽" || name == "鉴魄证身" || name == "月弦激星";
}

inline std::string_view EquipmentSlot(std::string_view name)
{
    for (const auto &option : InstanceEquipment) if (option.name == name) return option.slot;
    for (const auto &enchant : DamageEnchants) {
        if (enchant.name == name) {
            return enchant.slot;
        }
    }
    if (name == WaterWeapon || IsSchoolWeaponEffect(name) || std::ranges::find(LegendaryWeapons, name, &LegendaryTier::name) != std::end(LegendaryWeapons))
    {
        return "武器";
    }
    return {};
}

inline bool DefaultPrecombat(std::string_view code)
{
    constexpr std::string_view schools[]{ "baj", "bxj", "dj", "fsj", "gfj", "hjy", "lhj", "mw", "shxj", "tlgd", "txj", "wf", "xcj", "yjj", "ztg", "zxg" };
    return std::ranges::find(schools, code) != std::end(schools);
}

struct Prepared;

struct RecipeTiming
{
    std::string_view choice;
    Slot skill;
    int cooldown, cast_time;
};

struct Book
{
    std::string_view              code, name, default_macro;
    Attributes                    default_attributes;
    int                           default_major = 0;
    std::vector<std::string_view> default_recipes, default_talents, default_equipment;
    std::array<Slot, 4>           enchant_hits{};
    Slot                          belt_low = INVALID_SLOT, belt_high = INVALID_SLOT, water = INVALID_SLOT;
    Slot                          legendary_strain = INVALID_SLOT;
    std::vector<Resource>         resources;
    std::vector<SkillDescriptor>  skills; // Casts and damage identities share a name registry.
    std::vector<BuffDescriptor>   buffs;
    std::vector<Action>           actions;
    std::vector<CastVariant>      variants;
    std::vector<Hit>              hits;
    std::vector<Aura>             auras;
    std::vector<Slot>             castable;
    std::vector<std::string_view> skill_aliases, buff_aliases;
    std::vector<std::string_view> talents, recipes, equipment, preset_only_recipes;
    std::vector<std::pair<std::string_view, std::string_view>> recipe_groups;
    std::vector<RecipeTiming>                                  recipe_timing;
    std::map<std::string_view, std::string_view>                single_target_recipe_notes;
    std::vector<Slot>                                          initial_auras;
    Slot                                                       haste_stance = INVALID_SLOT;
    int                                                        stance_haste = 0;
    void (*adjust)(Prepared &) = nullptr;

    SkillCatalog Catalog() const
    {
        SkillCatalog result{ skills, buffs, castable, skill_aliases, buff_aliases };
        for (std::size_t i = 0; i < resources.size(); ++i) {
            result.resources.at(i) = { resources[i].name, resources[i].low < 0 };
        }
        return result;
    }
};

std::span<const Book> Books();
const Book           *Find(std::string_view code);

struct Prepared
{
    Config                             config;
    std::vector<const Action *>        actions;
    std::vector<SkillDescriptor>       skills;
    std::vector<Resource>              resources;
    std::vector<Step>                  opening;
    // Pre-pull placements whose first periodic tick is still pending at frame zero.
    std::vector<std::pair<Slot, tick_t>> opening_auras_elapsed, opening_cooldowns;
    tick_t                             opening_gcd = 0;
    Slot                               distant_target_aura = INVALID_SLOT;
    double                             distant_threshold = 0.;
    tick_t                             distant_linger = 0;
    std::vector<bool>                  enabled_actions;
    std::vector<std::vector<Step>>     impacts, starts, finishes, pulses, on_hit, expires;
    std::vector<std::vector<Modifier>> modifiers;
    std::vector<Stats>                 passive;
    std::vector<int>                   dot_ticks, aura_stack_caps;
    std::vector<int>                   gcd_reduction, mana_cost_quarters, ranges, cooldown_reduction;
    int                                crit_basis_points;
    double                             haste;
    int                                stance_haste = 0;
    int                                legendary_strain_per_stack = 0;
    Slot bracer_hit = INVALID_SLOT, ring_crit = INVALID_SLOT, ring_overcome = INVALID_SLOT;
    Slot adaptive_crit = INVALID_SLOT, adaptive_overcome = INVALID_SLOT, attack_trinket = INVALID_SLOT;
    tick_t attack_trinket_duration = 0;
    Slot speed_dot = INVALID_SLOT, trinket_hit = INVALID_SLOT;
    Slot shoe_crit = INVALID_SLOT, shoe_overcome = INVALID_SLOT;
    Slot wrist_proc = INVALID_SLOT, shoe_proc = INVALID_SLOT, belt_low = INVALID_SLOT, belt_high = INVALID_SLOT, water = INVALID_SLOT;
};

using Preparation = std::shared_ptr<const Prepared>;
Preparation Prepare(const Config &);
} // namespace JX3DPS::runtime::season50
