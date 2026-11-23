#ifndef JX3DPS_TAI_XU_RUNTIME_DATA_HPP
#define JX3DPS_TAI_XU_RUNTIME_DATA_HPP

#include <bit>
#include <memory>
#include <utility>

#include "src/core/runtime/attack_coefficient.hpp"
#include "src/core/runtime/attribute_versions.hpp"
#include "src/core/runtime/target_timeline.hpp"

#include "skills.hpp"

namespace JX3DPS::runtime::tai_xu {

enum Talent : unsigned
{
    DIE_REN_TALENT  = 1 << 0,
    WU_YI           = 1 << 1,
    SHEN_MAI        = 1 << 2,
    RUO_SHUI        = 1 << 3,
    FENG_SHI_TALENT = 1 << 4,
    WU_YU           = 1 << 5,
    HUAN_YUE        = 1 << 6,
    QIE_YU          = 1 << 7,
    XIN_GU          = 1 << 8,
    TONG_GEN        = 1 << 9,
    FU_YIN          = 1 << 10,
    GU_CHANG        = 1 << 11,
    QI_SHENG        = 1 << 12,
    CHANG_SHENG     = 1 << 13,
    XUAN_MEN        = 1 << 14,
    LIE_YUN         = 1 << 15,
    XU_JI           = 1 << 16,
    JIAN_RU         = 1 << 17,
    JING_HUA_TALENT = 1 << 18
};

inline constexpr unsigned SupportedTalents = (1 << 19) - 1;

enum Recipe : unsigned
{
    CRIT2               = 1 << 0,
    CRIT3               = 1 << 1,
    CRIT4               = 1 << 2,
    DAMAGE3             = 1 << 3,
    DAMAGE4             = 1 << 4,
    DAMAGE5             = 1 << 5,
    QIDIAN              = 1 << 6,
    COOLDOWN            = 1 << 7,
    PREPARE1            = 1 << 8,
    PREPARE2            = 1 << 9,
    PREPARE3            = 1 << 10,
    RANGE1              = 1 << 11,
    RANGE2              = 1 << 12,
    RANGE3              = 1 << 13,
    DAMAGE40            = 1 << 14,
    DAMAGE60            = 1 << 15,
    DOT                 = 1 << 16,
    YUN_ZHONG_SUI_XING  = 1 << 17,
    YUN_ZHONG_SHENG_TAI = 1 << 18,
    YUN_ZHONG_TUN_RI    = 1 << 19
};

enum EquipmentEffect : unsigned
{
    WEAPON_CW     = 1 << 0,
    SET_ATTRIBUTE = 1 << 1
};

enum TeamEffect : unsigned
{
    YOU_REN = 1 << 0
};

// Resolved combat attributes, after equipment/class calculations. Percent ints
// use 1024; crit basis points use 10000. Attack remains split for Purple's buff.
struct Attributes
{
    value_t attack_base = 10000, attack_by_class = 2500, weapon = 2000;
    int     attack_percent = 0;
    value_t crit = 20000, crit_power = 15000, overcome = 25000, strain = 10000, surplus = 3000;
    // A negative base selects the legacy resolved-overcome input. XuanMen needs the split.
    value_t overcome_base = -1, overcome_by_class = 0;
    int     overcome_percent = 0;
    value_t haste            = 0;
    int     haste_percent = 0, crit_basis_points = 0, crit_power_percent = 0;
    int     damage_percent = 0, strain_percent = 0, pve_percent = 0, ignore_shield = 0;
};

struct Target
{
    int     level      = 124;
    value_t shield     = 30000;
    int     vulnerable = 0;
    double  life = 1.0, distance = 4.0;
};
struct Module;

struct Config
{
    using RuntimeModule = Module;
    Attributes                     attributes;
    Target                         target;
    unsigned                       talents      = 0;
    unsigned                       equipment    = 0;
    unsigned                       team_effects = 0;
    std::array<unsigned, 3>        recipes{}; // WuWo, BaHuang, SanHuan.
    unsigned                       shengtaiji_recipes = 0;
    unsigned                       renjian_recipes    = 0;
    int                            initial_qidian     = 10;
    tick_t                         delay              = 0; // Fixed input delay; legacy random delay is not migrated.
    std::vector<team::Application> team_buffs;
    std::size_t                    attribute_version_capacity = 4096;
    std::vector<TargetEvent>       target_events;
};

enum class FormulaKind : std::uint8_t
{
    DIRECT,
    SURPLUS,
    DOT,
    ZERO
};

struct Formula
{
    Slot          skill = ZI_QI;
    std::uint16_t sub = 0, level = 0;
    int           fixed = 0, weapon = 0, channel = 0, count = 1;
    FormulaKind   kind      = FormulaKind::ZERO;
    int           dot_count = 8, dot_interval = 48;
};

inline constexpr std::uint16_t WuWoEffect = 0, FengWuWoEffect = 10, BaHuangEffect = 20, BaHuangExtraEffect = 21,
                               SanHuanEffect = 31, SanChaiEffect = 32, PoZhaoEffect = 33, DieRenEffect = 36, ZeroEffect = 99,
                               SuiXingEffect = 100, ShengTaiEffect = 101, TunRiEffect = 102, ChiYingEffect = 103,
                               WanJianEffect = 104, RenJianEffect = 105, RenJianSuiXingEffect = 106, RenJianDotEffect = 107,
                               JianRuEffect = 108, JianRuActiveEffect = 109, YunZhongSuiXingEffect = 110,
                               YunZhongShengTaiEffect = 111, YunZhongTunRiEffect = 112, JingHuaEffect = 113, JingHuaSuiEffect = 114,
                               BaHuangCwEffect = 115, WeaponCwExtraEffect = 116, WanXiangEffect = 117, EffectCount = 120;

enum Profile : std::uint16_t
{
    PURPLE_PROFILE        = 1,
    SUI_XING_PROFILE      = 2,
    QI_SHENG_PROFILE      = 4,
    LIVE_GU_CHANG_PROFILE = 8,
    LIE_YUN_PROFILE       = 16,
    XUAN_MEN_PROFILE_UNIT = 32,
    XUAN_MEN_PROFILE_MASK = 96,
    SET_ATTRIBUTE_PROFILE = 128
};

inline constexpr unsigned LiveProfileMask = LIVE_GU_CHANG_PROFILE | XUAN_MEN_PROFILE_MASK;
inline constexpr unsigned ProfileCount    = 256;
inline constexpr auto     Formulas        = [] {
    std::array<Formula, EffectCount> result{};
    // Explicitly materialize defaults for zero effects (also on GCC 16).
    result.fill(Formula{});
    for (int q = 1; q <= 10; ++q) {
        const Formula F{ WU_WO,
                         0,
                         static_cast<std::uint16_t>(q - 1),
                         ((2696 * q / 10 / 12) + ((2696 + 286) * q / 10 / 12)) / 2,
                         2048,
                         static_cast<int>(19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * q),
                         1,
                         FormulaKind::DIRECT };
        result[WuWoEffect + q - 1] = result[FengWuWoEffect + q - 1] = F;
    }
    result[BaHuangEffect] = {
        BA_HUANG, 0,  0, static_cast<int>((((1801 / 15 * 1.1 * 1.05) + ((1801 + 130) / 15 * 1.1 * 1.05)) / 2) + 2080),
        0,        16, 1, FormulaKind::DIRECT
    };
    for (int i = 0; i < 10; ++i) {
        const int D                    = 10 - i;
        result[BaHuangExtraEffect + i] = {
            BA_HUANG,
            1,
            static_cast<std::uint16_t>(i),
            ((690 / 3 / D) + (690 / 3 / D) + (70 / 3 / D)) / 2,
            2048,
            static_cast<int>((128 + (16 * (i + 1))) * 1.1 * 1.1 * 1.05),
            1,
            FormulaKind::DIRECT
        };
    }
    result[SanHuanEffect] = { SAN_HUAN, 0,
                              0,        ((1233 / 10) + ((1233 + 137) / 10)) / 2,
                              1024,     static_cast<int>(120 * 1.1 * 1.05 * 1.1),
                              1,        FormulaKind::DIRECT };
    result[SanChaiEffect] = { SAN_CHAI, 0, 0, 0, 1024, 16, 1, FormulaKind::DIRECT };
    constexpr std::array RATIOS{ 0.065, 0.13, 0.23 };
    for (int i = 0; i < 3; ++i) {
        result[PoZhaoEffect + i] = { PO_ZHAO, 0, static_cast<std::uint16_t>(i), 0, 0, static_cast<int>(1024 * 1024 * (RATIOS[i] - 1)), 1, FormulaKind::SURPLUS };
    }
    for (int stack = 1; stack <= 7; ++stack) {
        for (int count = 0; count <= 8; ++count) {
            result[DieRenEffect + ((stack - 1) * 9) + count] = {
                DIE_REN_DAMAGE,
                static_cast<std::uint16_t>(count == 1 ? 0 : 1),
                static_cast<std::uint16_t>(stack - 1),
                10,
                0,
                static_cast<int>(58 * 1.15 * 1.1 * 1.1) * stack,
                count,
                FormulaKind::DOT
            };
        }
    }
    result[SuiXingEffect].skill  = SUI_XING;
    result[ShengTaiEffect].skill = SHENG_TAI;
    result[TunRiEffect].skill    = TUN_RI;
    result[ChiYingEffect]        = { CHI_YING_DAMAGE, 0, 0, (40 + 40 + 17) / 2, 0, 127, 1, FormulaKind::DIRECT };
    result[WanJianEffect]        = {
        WAN_JIAN, 0,
        0,        static_cast<int>(((1132 / 10 * 1.5) + ((1132 / 10 * 1.5) + (113 / 10 * 1.5))) / 2),
        0,        static_cast<int>(100 * 1.1 * 1.25 * 1.05 * 1.5 * 1.1),
        1,        FormulaKind::DIRECT
    };
    result[RenJianEffect]              = { REN_JIAN, 0, 0, 1273 / 20, 0, 40, 1, FormulaKind::DIRECT };
    result[RenJianSuiXingEffect]       = result[RenJianEffect];
    result[RenJianSuiXingEffect].skill = REN_JIAN_SUI_XING;
    result[RenJianDotEffect]           = { REN_JIAN_DAMAGE, 0, 0, 16, 0, 40, 1, FormulaKind::DOT, 4, 48 };
    result[JianRuEffect]               = {
        JIAN_RU_DAMAGE,     0, 0, (77 + 77 + 25) / 2, 0, static_cast<int>(84 * 1.2 * 1.1 * 2 * 1.6774), 1,
        FormulaKind::DIRECT
    };
    result[JianRuActiveEffect] = {
        JIAN_RU_DAMAGE, 1, 0, (77 + 77 + 25) / 2, 0, static_cast<int>(280 * 1.1 * 2 * 1.6774), 1, FormulaKind::DIRECT
    };
    result[YunZhongSuiXingEffect]  = { YUN_ZHONG_DAMAGE,   0, 0, (40 + 40 + 17) / 2, 0, static_cast<int>(70 * 1.1), 1,
                                       FormulaKind::DIRECT };
    result[YunZhongShengTaiEffect] = result[YunZhongSuiXingEffect];
    result[YunZhongShengTaiEffect].sub = 1;
    result[YunZhongTunRiEffect]        = result[YunZhongSuiXingEffect];
    result[YunZhongTunRiEffect].sub    = 2;
    result[JingHuaEffect]              = {
        JING_HUA, 0, 0, (113 + 113 + 67) / 2, 1024, static_cast<int>(620 * 1.1 * 1.1), 1, FormulaKind::DIRECT
    };
    result[JingHuaSuiEffect]    = { JING_HUA, 1, 0, 200, 0, static_cast<int>(200 * 1.1 * 1.1), 1, FormulaKind::DIRECT };
    result[BaHuangCwEffect]     = result[BaHuangExtraEffect + 9];
    result[WeaponCwExtraEffect] = { BA_HUANG, 2, 0, (20 + 20 + 2) / 2, 0, 65, 1, FormulaKind::DIRECT };
    for (int stack = 1; stack <= 3; ++stack) {
        result[WanXiangEffect + stack -
               1] = { WAN_XIANG_DAMAGE, 0, static_cast<std::uint16_t>(stack - 1), 10, 0, static_cast<int>(205 * stack * 1.8), 1, FormulaKind::DOT, 10, 48 };
    }
    return result;
}();
static_assert(Formulas[ZeroEffect].skill == ZI_QI && Formulas[ZeroEffect].kind == FormulaKind::ZERO &&
              Formulas[SuiXingEffect].kind == FormulaKind::ZERO);

struct DamageInput
{
    value_t attack, weapon, crit_power, overcome, strain, surplus;
    int     damage_percent, crit_power_percent, strain_percent, pve_percent, ignore_shield;
};

struct AttributeVersion
{
    DamageInput     input{};
    double          critical_chance = 0;
    int             mitigation = 0, critical_power = 0, strain_factor = 0, vulnerable = 0;
    std::uint16_t   profile = 0;
    // Keep the timeline aura inputs so a damage-only replay can resolve the
    // same snapshot under a changed base attribute configuration.
    team::Modifiers team;
};

inline value_t CalculateDamage(const Formula &f, const DamageInput &a, const Target &t, bool critical)
{
    if (f.kind == FormulaKind::ZERO) {
        return 0;
    }
    value_t raw = 0;
    if (f.kind == FormulaKind::SURPLUS) {
        raw = static_cast<value_t>(a.surplus * (((f.channel + static_cast<int>(f.channel < 0)) / 1024) + 1024) / 1024 * 13.192);
    } else {
        const double coefficient =
            f.kind == FormulaKind::DOT
                ? DotAttackCoefficient(f.channel, f.dot_count, f.dot_interval, 10)
                : DirectAttackCoefficient(f.channel, 10);
        raw = f.fixed + static_cast<value_t>(a.attack * coefficient) + (a.weapon * f.weapon / 1024);
    }
    raw                   = raw * (1024 + a.damage_percent) / 1024;
    raw                  *= f.count; // Detonation multiplies before mitigation and each rounding step.
    const auto SHIELD     = std::max<value_t>(0, t.shield - (t.shield * a.ignore_shield / 1024));
    const int  OVERCOME   = static_cast<int>((a.overcome * 1024 / (9.530 * 8250)) + 1024);
    const int  REDUCTION  = static_cast<int>(SHIELD * 1024 / (SHIELD + (5.091 * ((450 * t.level) - 45750))));
    raw                   = raw * (OVERCOME - (OVERCOME * REDUCTION / 1024)) / 1024;
    if (critical) {
        const int POWER = static_cast<int>(a.crit_power * 1024 / (3.335 * 8250));
        raw             = static_cast<value_t>(raw + (raw * 0.75) + (raw * (POWER + a.crit_power_percent) / 1024));
    }
    const int DELTA  = 120 - t.level;
    raw              = static_cast<value_t>(raw * (1.0 + (DELTA * (DELTA < 0 ? .05 : .15))));
    const int STRAIN = static_cast<int>(a.strain * 1024 / (9.189 * 8250)) + 1024 + a.strain_percent;
    raw              = raw * STRAIN / 1024;
    raw              = raw * (1024 + a.pve_percent) / 1024;
    return raw * (1024 + t.vulnerable) / 1024;
}

struct Prepared
{
    Config config;
    double haste = 1;
    tick_t gcd = 24, sanhuan_cd = 32, sanchai_cd = 22, dot_interval = 48;
    tick_t bahuang_cd = 192, purple_cd = 1200;
    tick_t wanjian_cd = 192, renjian_cd = 320;
    int    renjian_range = 6, dot_max_stacks = 5;
    tick_t field_prepare = 16, shengtaiji_prepare = 16, jinghua_prepare = 48, shengtaiji_cd = 160, suixing_duration = 384;
    int                              purple_charges = 2;
    value_t                          max_damage     = 0;
    std::array<Formula, EffectCount> formulas       = Formulas;
    std::array<int, EffectCount>     damage_bonus{}, power_bonus{};
    std::array<double, EffectCount>  coefficients{}, chance_bonus{};
    std::array<value_t, EffectCount> surplus_damage{};
    double                           level_multiplier = 1;

    explicit Prepared(Config input) : config(std::move(input))
    {
        Validate();
        std::stable_sort(config.team_buffs.begin(), config.team_buffs.end(), [](const auto &a, const auto &b) {
            return a.frame < b.frame;
        });
        const auto &a = config.attributes;
        haste         = 1024.0 / (static_cast<int>(a.haste / (11.695 * 8250) * 1024) + a.haste_percent + 1024);
        gcd           = static_cast<int>(24 * haste + config.delay);
        sanhuan_cd = static_cast<int>((32 - ((config.recipes[SAN_HUAN] & COOLDOWN) ? 8 : 0) + (Has(HUAN_YUE) ? 64 : 0)) * haste);
        sanchai_cd     = static_cast<int>(22 * haste);
        dot_interval   = static_cast<int>(48 * haste);
        bahuang_cd     = 192 - ((config.recipes[BA_HUANG] & COOLDOWN) ? 16 : 0);
        wanjian_cd     = Has(XU_JI) ? 160 : 192;
        renjian_cd     = (config.renjian_recipes & COOLDOWN) ? 240 : 320;
        renjian_range  = 6 + std::popcount(config.renjian_recipes & (RANGE1 | RANGE2 | RANGE3));
        dot_max_stacks = Has(LIE_YUN) ? 7 : 5;
        purple_cd      = Has(RUO_SHUI) ? 960 : 1200;
        purple_charges = Has(RUO_SHUI) ? 3 : 2;
        field_prepare  = static_cast<int>(16 * haste);
        shengtaiji_prepare =
            static_cast<int>((16 - 2 * std::popcount(config.shengtaiji_recipes & (PREPARE1 | PREPARE2 | PREPARE3))) * haste);
        shengtaiji_cd    = (Has(XUAN_MEN) ? 400 : 160) - ((config.shengtaiji_recipes & COOLDOWN) ? 32 : 0);
        suixing_duration = Has(FU_YIN) ? 576 : 384;
        if (gcd < 1 || sanhuan_cd < 1 || sanchai_cd < 1 || dot_interval < 1) {
            throw std::invalid_argument("haste produces a zero-length timer");
        }
        team::Modifiers maximum;
        const int       DELTA = 120 - config.target.level;
        level_multiplier      = 1.0 + (DELTA * (DELTA < 0 ? .05 : .15));
        for (const auto &d : team::Definitions) {
            maximum.Add(d.modifiers, d.max_stacks);
        }
        for (unsigned effect = 0; effect < EffectCount; ++effect) {
            auto &formula = formulas[effect];
            if (effect >= WanXiangEffect && effect < WanXiangEffect + 3) {
                formula.dot_interval = dot_interval;
            }
            if (formula.skill == DIE_REN_DAMAGE && Has(XU_JI)) {
                formula.channel = static_cast<int>(formula.channel * 1.2);
            }
            const auto STATS     = Input(effect, 0);
            damage_bonus[effect] = STATS.damage_percent - a.damage_percent;
            power_bonus[effect]  = STATS.crit_power_percent - a.crit_power_percent;
            chance_bonus[effect] = ExtraChance(effect) * 1.0 / 10000;
            coefficients[effect] =
                formula.kind == FormulaKind::DOT
                    ? DotAttackCoefficient(formula.channel, formula.dot_count, formula.dot_interval, 10)
                    : DirectAttackCoefficient(formula.channel, 10);
            if (formula.kind == FormulaKind::SURPLUS) {
                surplus_damage[effect] =
                    static_cast<value_t>(a.surplus * (((formula.channel + static_cast<int>(formula.channel < 0)) / 1024) + 1024) /
                                         1024 * 13.192);
            }
            auto target   = TargetFor(maximum);
            target.shield = 0;
            max_damage = std::max(max_damage, CalculateDamage(formula, Input(effect, ProfileCount - 1, maximum), target, true));
        }
    }

    bool Has(Talent talent) const { return (config.talents & talent) != 0; }

    bool HasEquipment(EquipmentEffect effect) const { return (config.equipment & effect) != 0; }

    bool HasTeam(TeamEffect effect) const { return (config.team_effects & effect) != 0; }

    DamageInput Input(unsigned effect, unsigned snapshot, const team::Modifiers &team = {}) const
    {
        auto a            = config.attributes;
        a.attack_percent += team.attack_percent;
        if (a.overcome_base >= 0) {
            a.overcome_base += team.overcome_base;
        } else {
            a.overcome += team.overcome_base;
        }
        a.overcome_percent += team.overcome_percent;
        a.strain           += team.strain;
        a.ignore_shield    += team.ignore_shield;
        const auto &f       = Formulas[effect];
        const int   ATTACK_BONUS =
            (((snapshot & PURPLE_PROFILE) != 0u) ? 256 : 0) + (((snapshot & QI_SHENG_PROFILE) != 0u) ? 102 : 0);
        const int  POWER_BONUS = (((snapshot & PURPLE_PROFILE) != 0u) ? 256 : 0) +
                                 (((snapshot & SUI_XING_PROFILE) != 0u) ? (Has(FU_YIN) ? 205 : 102) : 0) +
                                 (((snapshot & LIE_YUN_PROFILE) != 0u) ? 154 : 0);
        const auto OVERCOME =
            a.overcome_base < 0
                ? a.overcome
                : (a.overcome_base *
                   (1024 + a.overcome_percent + (204 * ((snapshot & XUAN_MEN_PROFILE_MASK) / XUAN_MEN_PROFILE_UNIT))) / 1024) +
                      a.overcome_by_class;
        DamageInput result{
            (a.attack_base * (1024 + a.attack_percent + ATTACK_BONUS) / 1024) + a.attack_by_class,
            a.weapon,
            a.crit_power,
            OVERCOME,
            a.strain,
            a.surplus,
            a.damage_percent,
            a.crit_power_percent + POWER_BONUS + (((snapshot & SET_ATTRIBUTE_PROFILE) != 0u) ? 41 : 0),
            a.strain_percent,
            a.pve_percent,
            a.ignore_shield + (((snapshot & LIVE_GU_CHANG_PROFILE) != 0u) ? 614 : 0)
        };
        if (f.skill <= SAN_HUAN) {
            const auto R = config.recipes[f.skill];
            result.damage_percent +=
                (((R & DAMAGE3) != 0u) ? 31 : 0) + (((R & DAMAGE4) != 0u) ? 41 : 0) + (((R & DAMAGE5) != 0u) ? 51 : 0);
        }
        if (f.skill == WU_WO) {
            if (Has(TONG_GEN)) {
                result.damage_percent += 102;
            }
            if (Has(WU_YI) && f.level >= 5) {
                result.crit_power_percent += 307;
            }
            if (effect >= FengWuWoEffect) {
                result.damage_percent += 307;
            }
        }
        if (f.skill == SAN_HUAN && Has(XIN_GU)) {
            result.crit_power_percent += 102;
        }
        if (f.skill == SAN_CHAI) {
            result.damage_percent += 205;
        }
        if ((f.skill == WU_WO || f.skill == BA_HUANG) && ((config.equipment & WEAPON_CW) != 0u)) {
            result.damage_percent += 51;
        }
        if (f.skill == WAN_JIAN) {
            if (Has(TONG_GEN)) {
                result.damage_percent += 102;
            }
            if (Has(XU_JI)) {
                result.crit_power_percent += 1102; // Both old additions modify power.
            }
        }
        if (IsRenJian(f.skill)) {
            result.damage_percent += (((config.renjian_recipes & DAMAGE40) != 0u) ? 409 : 0) +
                                     (((config.renjian_recipes & DAMAGE60) != 0u) ? 614 : 0);
        }
        return result;
    }

    Target TargetFor(const team::Modifiers &team) const
    {
        auto result        = config.target;
        result.shield      = std::max<value_t>(0, result.shield + team.shield);
        result.vulnerable += team.vulnerable;
        return result;
    }

    value_t Damage(unsigned profile, unsigned effect, bool critical) const
    {
        const auto VERSION = Resolve(profile);
        return Damage(VERSION, VERSION, effect, critical);
    }

    AttributeVersion Resolve(unsigned profile, const team::Modifiers &team = {}) const
    {
        const auto input     = Input(ZeroEffect, profile, team);
        const auto target    = TargetFor(team);
        const auto SHIELD    = std::max<value_t>(0, target.shield - (target.shield * input.ignore_shield / 1024));
        const int  OVERCOME  = static_cast<int>((input.overcome * 1024 / (9.530 * 8250)) + 1024);
        const int  REDUCTION = static_cast<int>(SHIELD * 1024 / (SHIELD + (5.091 * ((450 * target.level) - 45750))));
        return { input,
                 Chance(ZeroEffect, profile, team.crit_basis_points),
                 OVERCOME - (OVERCOME * REDUCTION / 1024),
                 static_cast<int>(input.crit_power * 1024 / (3.335 * 8250)),
                 static_cast<int>(input.strain * 1024 / (9.189 * 8250)) + 1024 + input.strain_percent,
                 target.vulnerable,
                 static_cast<std::uint16_t>(profile),
                 team };
    }

    value_t Damage(const AttributeVersion &frozen, const AttributeVersion &live, unsigned effect, bool critical) const
    {
        const auto &f = formulas[effect];
        if (f.kind == FormulaKind::ZERO) {
            return 0;
        }
        value_t raw =
            f.kind == FormulaKind::SURPLUS
                ? surplus_damage[effect]
                : f.fixed + static_cast<value_t>(frozen.input.attack * coefficients[effect]) + (live.input.weapon * f.weapon / 1024);
        raw  = raw * (1024 + frozen.input.damage_percent + damage_bonus[effect]) / 1024;
        raw *= f.count;
        raw  = raw * live.mitigation / 1024;
        if (critical) {
            raw = static_cast<value_t>(raw + (raw * .75) +
                                       (raw * (frozen.critical_power + frozen.input.crit_power_percent + power_bonus[effect]) / 1024));
        }
        raw = static_cast<value_t>(raw * level_multiplier);
        raw = raw * frozen.strain_factor / 1024;
        raw = raw * (1024 + live.input.pve_percent) / 1024;
        return raw * (1024 + live.vulnerable) / 1024;
    }

    void ValidateCapacity(std::size_t capacity) const
    {
        if ((max_damage != 0) && std::cmp_greater(capacity, std::numeric_limits<value_t>::max() / max_damage)) {
            throw std::invalid_argument("damage capacity and attributes could overflow the fight total");
        }
    }

    int ExtraChance(unsigned effect) const
    {
        const auto &f     = Formulas[effect];
        int         extra = 0;
        if (f.skill <= SAN_HUAN) {
            const auto R = config.recipes[f.skill];
            extra = (((R & CRIT2) != 0u) ? 200 : 0) + (((R & CRIT3) != 0u) ? 300 : 0) + (((R & CRIT4) != 0u) ? 400 : 0);
        }
        if (f.skill == WU_WO && f.level >= 5 && Has(WU_YI)) {
            extra += 1000;
        }
        if (f.skill == SAN_HUAN && Has(XIN_GU)) {
            extra += 1000;
        }
        return extra;
    }

    double Chance(unsigned effect, unsigned snapshot, int team_crit = 0) const
    {
        const auto &a = config.attributes;
        // Keep the legacy addition order at probability boundaries.
        const int   BONUS =
            (((snapshot & PURPLE_PROFILE) != 0u) ? 2500 : 0) +
            (((snapshot & SUI_XING_PROFILE) != 0u) ? (Has(FU_YIN) ? 1000 : 500) : 0) +
            (300 * ((snapshot & XUAN_MEN_PROFILE_MASK) / XUAN_MEN_PROFILE_UNIT)) +
            (((snapshot & SET_ATTRIBUTE_PROFILE) != 0u) ? 400 : 0);
        return (a.crit / (9.530 * 8250)) + ((a.crit_basis_points + BONUS + team_crit) * 1.0 / 10000) +
               (ExtraChance(effect) * 1.0 / 10000);
    }

private:
    void Validate() const
    {
        ValidateTargetEvents(config.target_events);
        if ((config.attribute_version_capacity == 0u) || config.attribute_version_capacity > 10000000 ||
            config.team_buffs.size() > 100000)
        {
            throw std::invalid_argument("unsupported attribute version capacity or team schedule size");
        }
        for (const auto &app : config.team_buffs) {
            team::Validate(app);
            if ((team::Definitions[app.kind].modifiers.overcome_percent != 0) && config.attributes.overcome_base < 0) {
                throw std::invalid_argument("team overcome percent requires explicit overcome_base");
            }
        }
        if ((config.talents & ~SupportedTalents) != 0u) {
            throw std::invalid_argument("unmigrated talent flags");
        }
        if ((config.equipment & ~(WEAPON_CW | SET_ATTRIBUTE)) != 0u) {
            throw std::invalid_argument("unmigrated equipment effects");
        }
        if ((config.team_effects & ~YOU_REN) != 0u) {
            throw std::invalid_argument("unmigrated team effects");
        }
        if ((config.shengtaiji_recipes & ~(PREPARE1 | PREPARE2 | PREPARE3 | COOLDOWN)) != 0u) {
            throw std::invalid_argument("unsupported ShengTaiJi recipe");
        }
        if (((config.renjian_recipes & ~(COOLDOWN | RANGE1 | RANGE2 | RANGE3 | DAMAGE40 | DAMAGE60 | DOT |
                                         YUN_ZHONG_SUI_XING | YUN_ZHONG_SHENG_TAI | YUN_ZHONG_TUN_RI)) != 0u) ||
            std::popcount(config.renjian_recipes) > 4)
        {
            throw std::invalid_argument("unsupported RenJian recipe or more than four recipes");
        }
        constexpr std::array<unsigned, 3> ALLOWED{ 127, DAMAGE3 | DAMAGE4 | DAMAGE5 | QIDIAN | COOLDOWN, 191 };
        for (unsigned i = 0; i < 3; ++i) {
            if (((config.recipes[i] & ~ALLOWED[i]) != 0u) || std::popcount(config.recipes[i]) > 4) {
                throw std::invalid_argument("unsupported recipes or more than four recipes on a skill");
            }
        }
        const auto &a = config.attributes;
        if (a.overcome_base < -1 || a.overcome_base > 1000000 || a.overcome_by_class < 0 || a.overcome_by_class > 1000000 ||
            (a.overcome_base < 0 && (Has(XUAN_MEN) || (a.overcome_by_class != 0) || (a.overcome_percent != 0))))
        {
            throw std::invalid_argument(
                "XuanMen/split overcome requires explicit overcome_base and valid overcome_by_class");
        }
        for (const auto v :
             { a.attack_base, a.attack_by_class, a.weapon, a.crit, a.crit_power, a.overcome, a.strain, a.surplus, a.haste })
        {
            if (v < 0 || v > 1000000) {
                throw std::invalid_argument("combat attribute outside supported range");
            }
        }
        for (const auto v :
             { a.attack_percent,
               a.overcome_percent,
               a.haste_percent,
               a.crit_basis_points,
               a.crit_power_percent,
               a.damage_percent,
               a.strain_percent,
               a.pve_percent,
               config.target.vulnerable })
        {
            if (v < 0 || v > 10000) {
                throw std::invalid_argument("attribute bonus outside supported range");
            }
        }
        if (a.ignore_shield < 0 || a.ignore_shield > 1024 || config.target.shield < 0 ||
            config.target.shield > 10000000 || config.target.level < 110 || config.target.level > 130 ||
            !std::isfinite(config.target.life) || config.target.life < .01 || config.target.life > 1 ||
            !std::isfinite(config.target.distance) || config.target.distance < 0 || config.initial_qidian < 0 ||
            config.initial_qidian > 10 || config.delay < 0 || config.delay > 160)
        {
            throw std::invalid_argument("unsupported target, resource or delay configuration");
        }
    }
};

inline std::shared_ptr<const Prepared> Prepare(Config config = {})
{
    return std::make_shared<const Prepared>(std::move(config));
}

} // namespace JX3DPS::runtime::tai_xu
#endif
