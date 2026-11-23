#ifndef JX3DPS_MO_WEN_RUNTIME_DATA_HPP
#define JX3DPS_MO_WEN_RUNTIME_DATA_HPP

#include <bit>
#include <memory>

#include "src/core/runtime/attack_coefficient.hpp"
#include "src/core/runtime/target_timeline.hpp"

#include "skills.hpp"

namespace JX3DPS::runtime::mo_wen {

enum Talent : unsigned
{
    HAO_ZHONG        = 1u << 0,
    FEI_FAN          = 1u << 1,
    CHANG_QING       = 1u << 2,
    XIAN_FENG_TALENT = 1u << 3,
    LIU_ZHAO_TALENT  = 1u << 4,
    HAO_QING         = 1u << 5,
    SHI_XIANG        = 1u << 6,
    ZHI_ZHI          = 1u << 7,
    KE_MENG          = 1u << 8,
    ZHENG_MING       = 1u << 9,
    MING_JIN_TALENT  = 1u << 10,
    XIU_QI           = 1u << 11,
    YUN_HAN_TALENT   = 1u << 12,
    CAN_LIAN_TALENT  = 1u << 13,
    ZHENG_LV_HE_MING = 1u << 14
};

enum Recipe : unsigned
{
    GONG_CRIT2    = 1u << 0,
    GONG_CRIT3    = 1u << 1,
    GONG_CRIT4    = 1u << 2,
    GONG_DAMAGE3  = 1u << 3,
    GONG_DAMAGE4  = 1u << 4,
    GONG_PREPARE1 = 1u << 5,
    GONG_PREPARE2 = 1u << 6,
    GONG_RANGE    = 1u << 7,
    SHANG_CRIT2   = 1u << 8,
    SHANG_CRIT3   = 1u << 9,
    SHANG_CRIT4   = 1u << 10,
    SHANG_DAMAGE3 = 1u << 11,
    SHANG_DAMAGE4 = 1u << 12,
    SHANG_DAMAGE5 = 1u << 13,
    ZHI_CRIT2     = 1u << 14,
    ZHI_CRIT3     = 1u << 15,
    ZHI_CRIT4     = 1u << 16,
    ZHI_DAMAGE3   = 1u << 17,
    ZHI_DAMAGE4   = 1u << 18,
    ZHI_RANGE     = 1u << 19,
    YU_CRIT3      = 1u << 20,
    YU_CRIT4      = 1u << 21,
    YU_DAMAGE3    = 1u << 22,
    YU_DAMAGE4    = 1u << 23
};

enum EquipmentEffect : unsigned
{
    ENCHANT_SHOES = 1u << 0,
    ENCHANT_BELT  = 1u << 1,
    ENCHANT_WRIST = 1u << 2,
    SET_ATTRIBUTE = 1u << 3,
    WEAPON_CW     = 1u << 4,
    WEAPON_WATER  = 1u << 5,
    SET_SKILL     = 1u << 6
};

enum TeamEffect : unsigned
{
    YOU_REN   = 1u << 0,
    JING_MIAO = 1u << 1
};
enum class Style : std::uint8_t
{
    YANG_CHUN_BAI_XUE,
    GAO_SHAN_LIU_SHUI
};

struct Attributes
{
    value_t attack_base = 10000, attack_by_class = 2500, weapon = 2000, crit = 20000, crit_power = 15000,
            overcome = 25000, strain = 10000, surplus = 3000;
    value_t haste          = 0;
    int     attack_percent = 0, overcome_percent = 0, haste_percent = 0, crit_basis_points = 0, crit_power_percent = 0,
            damage_percent = 0, strain_percent = 0, pve_percent = 0, ignore_shield = 0;
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
    Attributes               attributes;
    Target                   target;
    unsigned                 talents = 0, equipment = 0, team_effects = 0;
    std::array<unsigned, 4>  recipes{};
    Style                    initial_style  = Style::GAO_SHAN_LIU_SHUI;
    int                      initial_yingzi = 0;
    tick_t                   delay          = 0;
    std::vector<TargetEvent> target_events;
};

enum Effect : std::uint16_t
{
    E_GONG,
    E_BIAN_GONG,
    E_SHANG,
    E_JUE,
    E_ZHI,
    E_BIAN_ZHI,
    E_YU,
    E_YANG_CHUN,
    E_YANG_CHUN_EXECUTE,
    E_WU_YIN,
    E_SURPLUS0,
    E_SURPLUS1,
    E_SURPLUS2,
    E_SURPLUS3,
    E_SHANG_DOT,
    E_JUE_DOT,
    E_YING_ZI,
    E_XIAN_FENG,
    E_LIU_ZHAO,
    E_LIU_ZHAO_SURPLUS,
    E_GONG_CW,
    E_BIAN_GONG_CW,
    E_YU_CW,
    E_SHEN_BING,
    E_SHEN_BING2,
    E_SHEN_BING3,
    E_ZHI_YIN,
    E_ZHI_YIN2,
    E_ZHI_YIN3,
    E_ZERO_GAO_SHAN,
    E_ZERO_YANG_CHUN,
    E_ZERO_SHU_YING,
    E_ZERO_GU_YING,
    E_ZERO_ZHENG_LV1,
    E_ZERO_ZHENG_LV2,
    E_ZERO_ZHENG_LV3,
    E_SHOES,
    E_WRIST,
    E_COUNT
};
enum class FormulaKind : std::uint8_t
{
    DIRECT,
    DOT,
    SURPLUS,
    ZERO
};

struct Formula
{
    Slot          skill = INVALID_SLOT;
    int           fixed = 0, channel = 0;
    FormulaKind   kind      = FormulaKind::ZERO;
    int           dot_count = 6, dot_interval = 48;
    std::uint16_t sub = 0, level = 0;
};

inline constexpr auto Formulas = [] {
    std::array<Formula, E_COUNT> f{};
    f[E_GONG]                    = { GONG, 180, 277, FormulaKind::DIRECT };
    f[E_BIAN_GONG]               = { BIAN_GONG, 180, 277, FormulaKind::DIRECT };
    f[E_SHANG]                   = { SHANG, 16, 108, FormulaKind::DIRECT };
    f[E_JUE]                     = { JUE, 16, 108, FormulaKind::DIRECT };
    f[E_ZHI]                     = { ZHI, 200, 124, FormulaKind::DIRECT };
    f[E_BIAN_ZHI]                = { BIAN_ZHI, 200, 124, FormulaKind::DIRECT };
    f[E_YU]                      = { YU, 149, 208, FormulaKind::DIRECT };
    f[E_YANG_CHUN]               = { YANG_CHUN, 80, 89, FormulaKind::DIRECT };
    f[E_YANG_CHUN_EXECUTE]       = f[E_YANG_CHUN];
    f[E_YANG_CHUN_EXECUTE].level = 1;
    f[E_WU_YIN]                  = { WU_YIN, 0, 4, FormulaKind::DIRECT };
    constexpr std::array RATIOS{ 0.25 * 0.5 * 1.3 * 1.2 * 0.5 * 1.11,
                                 0.30 * 0.5 * 1.3 * 1.2 * 0.5 * 1.11,
                                 0.36 * 0.5 * 1.3 * 1.2 * 0.5 * 1.11,
                                 0.4 * 0.9 * 0.7 };
    for (unsigned i = 0; i < 4; ++i) {
        f[E_SURPLUS0 + i] = {
            i == 3 ? ZHENG_MING_DAMAGE : PO_ZHAO,
            0,
            static_cast<int>(1024 * 1024 * (RATIOS[i] - 1)),
            FormulaKind::SURPLUS,
            6,
            48,
            0,
            static_cast<std::uint16_t>(i)
        };
    }
    f[E_SHANG_DOT] = { SHANG_DAMAGE, 19, 263, FormulaKind::DOT };
    f[E_JUE_DOT]   = { JUE_DAMAGE, 19, 257, FormulaKind::DOT };
    f[E_YING_ZI]   = { SHADOW_DAMAGE, 180, 61, FormulaKind::DIRECT, 6, 48, 1 };
    f[E_XIAN_FENG] = { XIAN_FENG_DAMAGE, 41, 48, FormulaKind::DIRECT };
    f[E_LIU_ZHAO]  = { LIU_ZHAO_DAMAGE, 120, static_cast<int>(61 * 1.25), FormulaKind::DIRECT };
    f[E_LIU_ZHAO_SURPLUS] = { LIU_ZHAO_SURPLUS, 0, static_cast<int>(1024 * 1024 * ((0.125 * 0.5 * 1.3 * 1.2 * 0.5 * 1.11) - 1)), FormulaKind::SURPLUS };
    f[E_GONG_CW]      = { GONG_CW_DAMAGE, 0, 277, FormulaKind::DIRECT, 6, 48, 1 };
    f[E_BIAN_GONG_CW] = { BIAN_GONG_CW_DAMAGE, 0, 277, FormulaKind::DIRECT, 6, 48, 1 };
    f[E_YU_CW]        = { YU_CW_DAMAGE, 21, 50, FormulaKind::DIRECT, 6, 48, 1 };
    for (unsigned i = 0; i < 3; ++i) {
        f[E_SHEN_BING + i] = {
            SHEN_BING_DAMAGE,
            58,
            618 * static_cast<int>(i + 1),
            FormulaKind::DOT,
            10,
            48,
            0,
            static_cast<std::uint16_t>(i)
        };
        f[E_ZHI_YIN + i] = {
            ZHI_YIN_DAMAGE,
            43,
            std::array{ 1786, 1453, 775 }
            [i],
            FormulaKind::DIRECT,
            6,
            48,
            0,
            static_cast<std::uint16_t>(i)
        };
    }
    for (unsigned i = 0; i < 7; ++i) {
        f[E_ZERO_GAO_SHAN + i].skill =
            std::array<Slot, 7>{ GAO_SHAN, YANG_CHUN, SHU_YING, GU_YING, ZHENG_LV1, ZHENG_LV2, ZHENG_LV3 }[i];
    }
    f[E_SHOES] = { SHOES_DAMAGE, 48, 100, FormulaKind::DIRECT };
    f[E_WRIST] = { WRIST_DAMAGE, 48, 90, FormulaKind::DIRECT };
    return f;
}();

// This bounded aura profile changes only on buff transitions.
inline constexpr unsigned YunHanMask = 7, CanLianProfile = 8, MingJinProfile = 16;
inline constexpr unsigned YangChunUnit = 32, YangChunMask = 96, SetProfile = 128, YouRenProfile = 256;
inline constexpr unsigned ProfileCount = 256;
inline constexpr unsigned WaterUnit = 512, WaterMask = 15 * WaterUnit, BeltUnit = 8192, BeltMask = 3 * BeltUnit;

constexpr unsigned ChanceProfile(unsigned p)
{
    return (((p & SetProfile) != 0u) ? 1 : 0) | (((p & YouRenProfile) != 0u) ? 2 : 0);
}

struct DamageGrouping
{
    struct Key
    {
        std::uint16_t profile;
        std::uint16_t effect_roll;
        std::int32_t  extra_power;
        bool          operator==(const Key &) const = default;
    };

    static Key MakeKey(const DamageIntent &hit)
    {
        const bool         CRITICAL       = hit.outcome == RollResult::DOUBLE;
        // YouRen only changes the roll probability. The recorded roll already includes it.
        constexpr unsigned DAMAGE_PROFILE = (ProfileCount - 1) | WaterMask | BeltMask;
        static_assert(2 * E_COUNT <= 65536);
        return { static_cast<std::uint16_t>(hit.snapshot & DAMAGE_PROFILE),
                 static_cast<std::uint16_t>((2 * hit.effect) + static_cast<int>(CRITICAL)),
                 CRITICAL ? static_cast<std::int32_t>(hit.base) : 0 };
    }

    static std::size_t Hash(Key key)
    {
        auto bits  = std::bit_cast<std::uint64_t>(key);
        bits      ^= bits >> 33;
        bits      *= 0xff51afd7ed558ccdULL;
        return static_cast<std::size_t>(bits ^ (bits >> 32));
    }
};

struct DamageInput
{
    value_t attack, crit_power, overcome, strain, surplus;
    int     damage_percent, crit_power_percent, strain_percent, pve_percent;
    int     ignore_shield, effect_ignore_shield;
};

inline value_t BeforeCritical(const Formula &f, const DamageInput &a, const Target &t)
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
                ? DotAttackCoefficient(f.channel, f.dot_count, f.dot_interval, 12)
                : DirectAttackCoefficient(f.channel, 12);
        raw = f.fixed + static_cast<value_t>(a.attack * coefficient);
    }
    raw                 = raw * (1024 + a.damage_percent) / 1024;
    auto shield         = t.shield - (t.shield * a.ignore_shield / 1024);
    shield              = std::max<value_t>(0, shield - (shield * a.effect_ignore_shield / 1024));
    const int OVERCOME  = static_cast<int>((a.overcome * 1024 / (9.530 * 8250)) + 1024);
    const int REDUCTION = static_cast<int>(shield * 1024 / (shield + (5.091 * ((450 * t.level) - 45750))));
    return raw * (OVERCOME - (OVERCOME * REDUCTION / 1024)) / 1024;
}

inline value_t AfterCritical(value_t raw, int power, bool critical, int strain, int pve, int vulnerable, double level)
{
    if (critical) {
        raw = static_cast<value_t>(raw + (raw * .75) + (raw * power / 1024));
    }
    raw = static_cast<value_t>(raw * level);
    raw = raw * strain / 1024;
    raw = raw * (1024 + pve) / 1024;
    return raw * (1024 + vulnerable) / 1024;
}

inline value_t CalculateDamage(const Formula &f, const DamageInput &a, const Target &t, bool critical)
{
    const int POWER  = static_cast<int>(a.crit_power * 1024 / (3.335 * 8250)) + a.crit_power_percent;
    const int STRAIN = static_cast<int>(a.strain * 1024 / (9.189 * 8250)) + 1024 + a.strain_percent;
    const int DELTA  = 120 - t.level;
    return AfterCritical(BeforeCritical(f, a, t), POWER, critical, STRAIN, a.pve_percent, t.vulnerable, 1.0 + (DELTA * (DELTA < 0 ? .05 : .15)));
}

struct Timing
{
    double haste = 1;
    tick_t gcd = 24, short_gcd = 24, gong = 28, zhi = 16, dot = 48, shadow = 32, wuyin = 16;
};

struct Prepared
{
    Config                                                   config;
    std::array<Timing, 2>                                    timing{};
    tick_t                                                   yu_cooldown = 96, yangchun_cooldown = 48;
    std::array<Formula, E_COUNT>                             formulas = Formulas;
    std::vector<std::array<std::array<value_t, 2>, E_COUNT>> damage;
    std::vector<std::array<value_t, E_COUNT>>                before_crit;
    std::array<std::array<int, E_COUNT>, ProfileCount>       power{};
    std::array<std::array<double, E_COUNT>, 4>               critical_chance{};
    int                                                      strain          = 1024;
    double                                                   level           = 1;
    value_t                                                  max_damage      = 0;
    value_t                                                  max_before_crit = 0;
    int                                                      max_power       = 0;
    unsigned                                                 water_profiles  = 1;

    explicit Prepared(Config input) : config(std::move(input))
    {
        Validate();
        const auto &a = config.attributes;
        for (unsigned i = 0; i < timing.size(); ++i) {
            auto &t = timing[i];
            t.haste = 1024.0 / (static_cast<int>(a.haste / (11.695 * 8250) * 1024) + a.haste_percent + 1024 + ((i != 0u) ? 205 : 0));
            t.gcd       = static_cast<int>((24 * t.haste) + config.delay);
            t.short_gcd = static_cast<int>(24 * t.haste);
            t.gong = static_cast<int>((28 - (2 * std::popcount(config.recipes[0] & (GONG_PREPARE1 | GONG_PREPARE2)))) * t.haste);
            t.zhi    = static_cast<int>(16 * t.haste);
            t.dot    = static_cast<int>(48 * t.haste);
            t.shadow = static_cast<int>(32 * t.haste);
            t.wuyin  = static_cast<int>(16 * t.haste);
            if (std::min({ t.gcd, t.short_gcd, t.gong, t.zhi, t.dot, t.shadow, t.wuyin }) < 1) {
                throw std::invalid_argument("haste produces a zero-length Mo Wen timer");
            }
        }
        yu_cooldown       = Has(MING_JIN_TALENT) ? 176 : 96;
        yangchun_cooldown = Has(CAN_LIAN_TALENT) ? 16 : 48;
        for (const auto effect : { E_SHANG, E_SHANG_DOT }) {
            auto &channel = formulas[effect].channel;
            if ((config.recipes[1] & SHANG_DAMAGE3) != 0u) {
                channel = static_cast<int>(channel * 1.03);
            }
            if ((config.recipes[1] & SHANG_DAMAGE4) != 0u) {
                channel = static_cast<int>(channel * 1.04);
            }
            if ((config.recipes[1] & SHANG_DAMAGE5) != 0u) {
                channel = static_cast<int>(channel * 1.05);
            }
        }
        strain                       = static_cast<int>(a.strain * 1024 / (9.189 * 8250)) + 1024 + a.strain_percent;
        const int DELTA              = 120 - config.target.level;
        level                        = 1.0 + (DELTA * (DELTA < 0 ? .05 : .15));
        water_profiles               = HasEquipment(WEAPON_WATER) ? 11 : 1;
        const unsigned profile_count = ProfileCount * water_profiles * (HasEquipment(ENCHANT_BELT) ? 3 : 1);
        damage.resize(profile_count);
        before_crit.resize(profile_count);
        for (unsigned p = 0; p < profile_count; ++p) {
            for (unsigned e = 0; e < E_COUNT; ++e) {
                const unsigned ENCODED = (p % ProfileCount) + ((p / ProfileCount % water_profiles) * WaterUnit) +
                                         ((p / ProfileCount / water_profiles) * BeltUnit);
                const auto     in      = Input(e, ENCODED);
                before_crit[p][e]      = BeforeCritical(formulas[e], in, config.target);
                max_before_crit        = std::max(max_before_crit, before_crit[p][e]);
                if (p < ProfileCount) {
                    power[p][e] = static_cast<int>(in.crit_power * 1024 / (3.335 * 8250)) + in.crit_power_percent;
                    max_power   = std::max(max_power, power[p][e]);
                }
            }
        }
        ValidateCapacity(1);
        for (unsigned p = 0; p < profile_count; ++p) {
            for (unsigned e = 0; e < E_COUNT; ++e) {
                for (unsigned crit = 0; crit < 2; ++crit) {
                    damage[p][e][crit] =
                        AfterCritical(before_crit[p][e],
                                      power[p % ProfileCount][e],
                                      crit != 0,
                                      strain,
                                      config.attributes.pve_percent,
                                      config.target.vulnerable,
                                      level);
                    max_damage = std::max(max_damage, damage[p][e][crit]);
                }
            }
        }
        for (unsigned p = 0; p < 4; ++p) {
            for (unsigned e = 0; e < E_COUNT; ++e) {
                critical_chance[p][e] = Chance(e, p);
            }
        }
    }

    bool Has(Talent t) const { return (config.talents & t) != 0; }

    bool HasEquipment(EquipmentEffect e) const { return (config.equipment & e) != 0; }

    bool HasTeam(TeamEffect e) const { return (config.team_effects & e) != 0; }

    DamageInput Input(unsigned effect, unsigned profile) const
    {
        const auto &a            = config.attributes;
        const int   ATTACK_BONUS = ((profile & CanLianProfile) != 0u) ? 102 : 0;
        const int   WATER        = static_cast<int>((profile & WaterMask) / WaterUnit);
        const int   belt         = static_cast<int>((profile & BeltMask) / BeltUnit);
        DamageInput in{
            ((a.attack_base + (88 * WATER)) * (1024 + a.attack_percent + ATTACK_BONUS) / 1024) + a.attack_by_class,
            a.crit_power,
            a.overcome * (1024 + a.overcome_percent) / 1024,
            a.strain,
            a.surplus,
            a.damage_percent + (51 * static_cast<int>(profile & YunHanMask)) +
                (belt == 2   ? 51
                 : belt == 1 ? 10
                             : 0),
            a.crit_power_percent + (((profile & SetProfile) != 0u) ? 41 : 0),
            a.strain_percent,
            a.pve_percent,
            a.ignore_shield,
            0
        };
        if (effect <= E_YU || effect == E_SHANG_DOT || effect == E_JUE_DOT) {
            in.damage_percent += ((profile & MingJinProfile) != 0u) ? 205 : 0;
        }
        if (effect == E_GONG || effect == E_BIAN_GONG) {
            in.damage_percent +=
                (((config.recipes[0] & GONG_DAMAGE3) != 0u) ? 31 : 0) + (((config.recipes[0] & GONG_DAMAGE4) != 0u) ? 41 : 0);
            if (Has(HAO_ZHONG)) {
                in.crit_power_percent += 102;
            }
        }
        if (effect == E_ZHI || effect == E_BIAN_ZHI) {
            in.damage_percent +=
                (((config.recipes[2] & ZHI_DAMAGE3) != 0u) ? 31 : 0) + (((config.recipes[2] & ZHI_DAMAGE4) != 0u) ? 41 : 0);
            if (Has(FEI_FAN)) {
                in.damage_percent += effect == E_ZHI ? 102 : 152;
            }
        }
        if (effect == E_GONG || effect == E_BIAN_GONG || effect == E_ZHI || effect == E_BIAN_ZHI) {
            if (HasEquipment(SET_SKILL)) {
                in.damage_percent += 102;
            }
            if (HasEquipment(WEAPON_CW)) {
                in.damage_percent += 51;
            }
        }
        if (effect == E_GONG || effect == E_ZHI || effect == E_GONG_CW) {
            in.effect_ignore_shield += 307 * static_cast<int>((profile & YangChunMask) / YangChunUnit);
        }
        if (effect == E_YU) {
            in.damage_percent +=
                (((config.recipes[3] & YU_DAMAGE3) != 0u) ? 31 : 0) + (((config.recipes[3] & YU_DAMAGE4) != 0u) ? 41 : 0);
        }
        if ((effect == E_YU || effect == E_YU_CW) && Has(SHI_XIANG)) {
            in.effect_ignore_shield += 614;
        }
        if ((effect == E_GONG_CW || effect == E_BIAN_GONG_CW) && Has(HAO_ZHONG)) {
            in.crit_power_percent += 102;
        }
        if (effect == E_YANG_CHUN_EXECUTE) {
            in.damage_percent += 410;
        }
        if (effect == E_YING_ZI && Has(KE_MENG)) {
            in.crit_power_percent += 102;
        }
        if (effect == E_LIU_ZHAO) {
            in.effect_ignore_shield = 922;
        }
        return in;
    }

    double Chance(unsigned effect, unsigned profile) const
    {
        const auto &a     = config.attributes;
        int         bonus = a.crit_basis_points + (((profile & 1) != 0u) ? 400 : 0) + (((profile & 2) != 0u) ? 500 : 0);
        if (effect == E_GONG || effect == E_BIAN_GONG) {
            bonus += (((config.recipes[0] & GONG_CRIT2) != 0u) ? 200 : 0) +
                     (((config.recipes[0] & GONG_CRIT3) != 0u) ? 300 : 0) + (((config.recipes[0] & GONG_CRIT4) != 0u) ? 400 : 0);
            if (Has(HAO_ZHONG)) {
                bonus += 1000;
            }
        }
        if (effect == E_SHANG || effect == E_SHANG_DOT) {
            bonus += (((config.recipes[1] & SHANG_CRIT2) != 0u) ? 200 : 0) +
                     (((config.recipes[1] & SHANG_CRIT3) != 0u) ? 300 : 0) +
                     (((config.recipes[1] & SHANG_CRIT4) != 0u) ? 400 : 0);
        }
        if (effect == E_ZHI || effect == E_BIAN_ZHI) {
            bonus += (((config.recipes[2] & ZHI_CRIT2) != 0u) ? 200 : 0) +
                     (((config.recipes[2] & ZHI_CRIT3) != 0u) ? 300 : 0) + (((config.recipes[2] & ZHI_CRIT4) != 0u) ? 400 : 0);
        }
        if (effect == E_YU) {
            bonus += (((config.recipes[3] & YU_CRIT3) != 0u) ? 300 : 0) + (((config.recipes[3] & YU_CRIT4) != 0u) ? 400 : 0);
        }
        if (effect == E_YING_ZI && Has(KE_MENG)) {
            bonus += 1000;
        }
        return (a.crit / (9.530 * 8250)) + (bonus / 10000.0);
    }

    value_t Reduce(unsigned snapshot, unsigned effect, bool critical, value_t extra_power) const
    {
        const auto PROFILE = CacheIndex(snapshot);
        // Legacy MiaoYi adds power even at the stack cap. Capture its unbounded
        // remainder in the payload instead of growing a cache dimension per fight.
        if (!critical || extra_power == 0) {
            return damage[PROFILE][effect][static_cast<std::size_t>(critical)];
        }
        return AfterCritical(before_crit[PROFILE][effect],
                             power[snapshot & (ProfileCount - 1)][effect] + static_cast<int>(extra_power),
                             true,
                             strain,
                             config.attributes.pve_percent,
                             config.target.vulnerable,
                             level);
    }

    value_t Reduce(const DamageIntent &hit) const
    {
        return Reduce(hit.snapshot, hit.effect, hit.outcome == RollResult::DOUBLE, hit.base);
    }

    unsigned CacheIndex(unsigned profile) const
    {
        return (profile & (ProfileCount - 1)) +
               (ProfileCount * (((profile & WaterMask) / WaterUnit) + (water_profiles * ((profile & BeltMask) / BeltUnit))));
    }

    std::size_t CacheBytes() const
    {
        return (damage.size() * sizeof(damage[0])) + (before_crit.size() * sizeof(before_crit[0])) + sizeof(power) +
               sizeof(critical_chance);
    }

    void ValidateCapacity(std::size_t capacity) const
    {
        // Each MiaoYi grant emits five intents and adds at most 410 power.
        const long double EXTRA = (Has(ZHENG_LV_HE_MING) ? (82.0L * capacity) + 410 : 0) + (HasTeam(JING_MIAO) ? 205 : 0);
        const auto limit   = static_cast<long double>(std::numeric_limits<value_t>::max());
        const auto CHECKED = [&](long double v) {
            if (v >= limit) {
                throw std::invalid_argument("Mo Wen attributes and log capacity could overflow damage");
            }
            return v;
        };
        const auto CRIT_POWER = max_power + EXTRA;
        if (CRIT_POWER > std::numeric_limits<int>::max()) {
            throw std::invalid_argument("Mo Wen log capacity could overflow accumulated critical power");
        }
        auto upper = CHECKED((max_before_crit * 1.75L) + (CHECKED(max_before_crit * CRIT_POWER) / 1024) + 1);
        upper      = CHECKED((upper * level) + 1);
        upper      = (CHECKED(upper * strain) / 1024) + 1;
        upper      = (CHECKED(upper * (1024 + config.attributes.pve_percent)) / 1024) + 1;
        upper      = (CHECKED(upper * (1024 + config.target.vulnerable)) / 1024) + 1;
        CHECKED(upper * capacity);
    }

    void Validate() const
    {
        ValidateTargetEvents(config.target_events);
        const auto &a = config.attributes;
        for (const value_t v :
             { a.attack_base, a.attack_by_class, a.weapon, a.crit, a.crit_power, a.overcome, a.strain, a.surplus, a.haste })
        {
            if (v < 0 || v > 1000000) {
                throw std::invalid_argument("Mo Wen attribute outside 0..1000000");
            }
        }
        for (const int v :
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
                throw std::invalid_argument("Mo Wen bonus outside 0..10000");
            }
        }
        if (a.ignore_shield < 0 || a.ignore_shield > 1024) {
            throw std::invalid_argument("invalid shield ignore");
        }
        if (config.target.level < 110 || config.target.level > 130 || config.target.shield < 0 || config.target.shield > 10000000 ||
            !std::isfinite(config.target.distance) || config.target.distance < 0 || config.target.distance > 1000 ||
            !std::isfinite(config.target.life) || config.target.life < .01 || config.target.life > 1)
        {
            throw std::invalid_argument("invalid Mo Wen target");
        }
        if (((config.talents & ~((1u << 15) - 1)) != 0u) || ((config.team_effects & ~(YOU_REN | JING_MIAO)) != 0u)) {
            throw std::invalid_argument("unknown Mo Wen talent or team effect");
        }
        if ((config.equipment &
             ~(SET_ATTRIBUTE | WEAPON_CW | SET_SKILL | ENCHANT_SHOES | ENCHANT_WRIST | ENCHANT_BELT | WEAPON_WATER)) != 0u)
        {
            throw std::invalid_argument("unknown Mo Wen equipment effect");
        }
        if (config.delay < 0 || config.delay > 160 || config.initial_yingzi < 0 || config.initial_yingzi > 6 ||
            (config.initial_style != Style::YANG_CHUN_BAI_XUE && config.initial_style != Style::GAO_SHAN_LIU_SHUI))
        {
            throw std::invalid_argument("invalid initial Mo Wen state");
        }
        constexpr std::array<unsigned, 4> MASKS{ 255, (1u << 14) - 256, ((1u << 20) - 1) ^ ((1u << 14) - 1), ((1u << 24) - 1) ^ ((1u << 20) - 1) };
        for (unsigned i = 0; i < 4; ++i) {
            if (((config.recipes[i] & ~MASKS[i]) != 0u) || std::popcount(config.recipes[i]) > 4) {
                throw std::invalid_argument("invalid Mo Wen recipe selection");
            }
        }
    }
};

inline std::shared_ptr<const Prepared> Prepare(Config config = {})
{
    return std::make_shared<const Prepared>(std::move(config));
}
} // namespace JX3DPS::runtime::mo_wen
#endif
