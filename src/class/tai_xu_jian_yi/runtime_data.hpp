#ifndef JX3DPS_TAI_XU_RUNTIME_DATA_HPP
#define JX3DPS_TAI_XU_RUNTIME_DATA_HPP

#include "src/core/runtime/descriptors.hpp"
#include "src/core/runtime/attribute_versions.hpp"
#include "team_buffs.hpp"
#include <bit>
#include <memory>

namespace JX3DPS::runtime::tai_xu {

// Behavior is pinned to ba960f7 (level 120); external IDs only label reports.
enum SkillSlot : Slot { WuWo, BaHuang, SanHuan, SanChai, ZiQi, PoZhao, DieRenDamage,
    SuiXing, ShengTai, TunRi, ChiYingDamage, WanJian, RenJian, RenJianSuiXing, RenJianDamage, JianRuDamage, YunZhongDamage, JingHua, WanXiangDamage, SkillCount };
enum BuffSlot : Slot { DieRen, Purple, FengShi, SuiXingBuff, TunRiBuff, QiShengBuff,
    ChiYingBuff, FieldSuiXing, FieldShengTai, FieldTunRi, XuanMenBuff, LieYunBuff,
    HiddenLieYun, RenJianDot, JianRuBuff, YunZhongSuiXingBuff, YunZhongShengTaiBuff, YunZhongTunRiBuff,
    JingHuaBuff, WeaponCwBuff, WanXiangBuff, SetAttributeBuff, YouRenBuff, TeamBuffBegin,
    BuffCount = TeamBuffBegin + team::Count };
inline constexpr std::array<Slot, 12> CastableSkills{WuWo, BaHuang, SanHuan, SanChai, ZiQi, SuiXing, ShengTai, TunRi, WanJian, RenJian, RenJianSuiXing, JingHua};
inline constexpr bool IsFieldSkill(Slot skill) { return skill >= SuiXing && skill <= TunRi; }
inline constexpr bool IsRenJian(Slot skill) { return skill == RenJian || skill == RenJianSuiXing; }
inline constexpr bool IsCastable(Slot skill) { return skill <= ZiQi || IsFieldSkill(skill) || skill == WanJian || IsRenJian(skill) || skill == JingHua; }
inline constexpr bool UsesGcd(Slot skill) { return skill <= SanHuan || IsFieldSkill(skill) || skill == WanJian || IsRenJian(skill) || skill == JingHua; }
inline constexpr DescriptorTable<SkillDescriptor, SkillCount> Skills{{{
    {32696, "\xe6\x97\xa0\xe6\x88\x91\xe6\x97\xa0\xe5\x89\x91", 0, 24, 0},
    {32882, "\xe5\x85\xab\xe8\x8d\x92\xe5\xbd\x92\xe5\x85\x83", 192, 24, 0},
    {32700, "\xe4\xb8\x89\xe7\x8e\xaf\xe5\xa5\x97\xe6\x9c\x88", 32, 24, 0},
    {32704, "\xe4\xb8\x89\xe6\x9f\xb4\xe5\x89\x91\xe6\xb3\x95", 22, 0, 0},
    {32715, "\xe7\xb4\xab\xe6\xb0\x94\xe4\xb8\x9c\xe6\x9d\xa5", 1200, 0, 0},
    {32897, "\xe7\xa0\xb4\xe6\x8b\x9b", 0, 0, 0},
    {9745, "\xe5\x8f\xa0\xe5\x88\x83", 0, 0, 0},
    {32708, "\xe7\xa2\x8e\xe6\x98\x9f\xe8\xbe\xb0", 160, 24, 16},
    {32711, "\xe7\x94\x9f\xe5\xa4\xaa\xe6\x9e\x81", 160, 24, 16},
    {32707, "\xe5\x90\x9e\xe6\x97\xa5\xe6\x9c\x88", 160, 24, 16},
    {28626, "\xe6\x8c\x81\xe7\x9b\x88", 0, 0, 0},
    {32703, "\xe4\xb8\x87\xe5\x89\x91\xe5\xbd\x92\xe5\xae\x97", 192, 24, 0},
    {32705, "\xe4\xba\xba\xe5\x89\x91\xe5\x90\x88\xe4\xb8\x80", 320, 24, 0},
    {-6, "\xe4\xba\xba\xe5\x89\x91\xe5\x90\x88\xe4\xb8\x80\xc2\xb7\xe7\xa9\xba\xe7\x88\x86\xc2\xb7\xe7\xa2\x8e\xe6\x98\x9f\xe8\xbe\xb0", 320, 24, 0},
    {9732, "\xe4\xba\xba\xe5\x89\x91\xe5\x90\x88\xe4\xb8\x80\xc2\xb7" "DOT", 0, 0, 0},
    {25766, "\xe5\x89\x91\xe5\x85\xa5", 0, 0, 0},
    {-8, "\xe4\xba\x91\xe4\xb8\xad\xe5\x89\x91", 0, 0, 0},
    {32898, "\xe9\x95\x9c\xe8\x8a\xb1\xe5\xbd\xb1", 0, 24, 48},
    {18931, "\xe4\xb8\x87\xe8\xb1\xa1\xe5\xbd\x92\xe5\x85\x83", 0, 0, 0}
}}};
inline constexpr auto Buffs = [] {
DescriptorTable<BuffDescriptor, BuffCount> result{{{
    {9745, Skills.entries[DieRenDamage].name, 384, 48, 7},
    {19267, Skills.entries[ZiQi].name, 160, 16, 1},
    {17932, "\xe9\xa3\x8e\xe9\x80\x9d", 80, 0, 1},
    // Negative IDs are internal labels where the pinned source has no game ID.
    {-1, Skills.entries[SuiXing].name, 64, 0, 1},
    {-2, Skills.entries[TunRi].name, 20, 0, 1},
    {23393, "\xe6\x9c\x9f\xe5\xa3\xb0", 48, 0, 1},
    {28626, Skills.entries[ChiYingDamage].name, 240, 0, 3},
    {-3, "\xe6\xb0\x94\xe5\x9c\xba\xc2\xb7\xe7\xa2\x8e\xe6\x98\x9f\xe8\xbe\xb0", 384, 48, 3},
    {-4, "\xe6\xb0\x94\xe5\x9c\xba\xc2\xb7\xe7\x94\x9f\xe5\xa4\xaa\xe6\x9e\x81", 384, 48, 3},
    {-5, "\xe6\xb0\x94\xe5\x9c\xba\xc2\xb7\xe5\x90\x9e\xe6\x97\xa5\xe6\x9c\x88", 384, 20, 3},
    {23367, "\xe7\x8e\x84\xe9\x97\xa8", 640, 0, 3},
    {25711, "\xe8\xa3\x82\xe4\xba\x91", 16, 0, 1},
    {-7, "\xe9\x9a\x90\xe8\x97\x8f\xc2\xb7\xe8\xa3\x82\xe4\xba\x91", 384, 8, 1},
    {9732, Skills.entries[RenJian].name, 192, 48, 1},
    {25766, "\xe5\x89\x91\xe5\x85\xa5", 98, 16, 1},
    {-9, "\xe4\xba\x91\xe4\xb8\xad\xe5\x89\x91\xc2\xb7\xe7\xa2\x8e\xe6\x98\x9f\xe8\xbe\xb0", 128, 16, 1},
    {-10, "\xe4\xba\x91\xe4\xb8\xad\xe5\x89\x91\xc2\xb7\xe7\x94\x9f\xe5\xa4\xaa\xe6\x9e\x81", 128, 16, 1},
    {-11, "\xe4\xba\x91\xe4\xb8\xad\xe5\x89\x91\xc2\xb7\xe5\x90\x9e\xe6\x97\xa5\xe6\x9c\x88", 128, 16, 1},
    {28598, "\xe9\x95\x9c\xe8\x8a\xb1\xe5\xbd\xb1", 128, 0, 1},
    {-12, "\xe6\xa9\x99\xe6\xad\xa6\xe7\x89\xb9\xe6\x95\x88\xc2\xb7" "CW", 96, 0, 1},
    {18931, "\xe4\xb8\x87\xe8\xb1\xa1\xe5\xbd\x92\xe5\x85\x83", 480, 48, 3},
    {-13, "\xe5\xa5\x97\xe8\xa3\x85\xc2\xb7\xe5\x89\x91\xe9\xb8\xa3", 96, 0, 1},
    {-14, "\xe6\xb8\xb8\xe5\x88\x83", 320, 0, 5}
}}};
    for (Slot i=0; i<team::Count; ++i) {
        const auto &d = team::Definitions[i];
        result.entries[TeamBuffBegin+i] = {-100-static_cast<int>(i), d.name, d.duration, 0, d.max_stacks};
    }
    return result;
}();
inline constexpr std::array<std::string_view, SkillCount> SkillAliases{
    "wuwu", "bahuang", "sanhuan", "sanchai", "ziqi", "pozhao", "dieren", "suixing", "shengtaiji", "tunriyue", "chiying", "wanjian", "renjian", "renjian_suixing", "renjian_dot", "jianru", "yunzhongjian", "jinghua", "wanxiang"};
inline constexpr auto BuffAliases = [] {
std::array<std::string_view, BuffCount> result{
    "dieren", "ziqi", "fengshi", "suixing", "tunriyue", "qisheng", "chiying", "field_suixing", "field_shengtaiji", "field_tunriyue", "xuanmen", "lieyun", "hidden_lieyun", "renjian", "jianru", "yunzhong_suixing", "yunzhong_shengtaiji", "yunzhong_tunriyue", "jinghua", "weapon_cw", "wanxiang", "set_attribute", "you_ren"};
    for (Slot i=0; i<team::Count; ++i) result[TeamBuffBegin+i] = team::Definitions[i].alias;
    return result;
}();
static_assert(Skills.Valid() && Buffs.Valid());

inline CompileOptions MacroOptions(bool debug = true) {
    CompileOptions options;
    options.skill_count = SkillCount;
    options.buff_count = BuffCount;
    options.keep_debug_info = debug;
    options.resolve_skill = [](std::string_view name) {
        for (Slot s : CastableSkills)
            if (name == Skills.entries[s].name || name == SkillAliases[s]) return s;
        return INVALID_SLOT;
    };
    options.resolve_buff = [](std::string_view name) {
        for (Slot b = 0; b < BuffCount; ++b)
            if (name == Buffs.entries[b].name || name == BuffAliases[b]) return b;
        return INVALID_SLOT;
    };
    return options;
}

enum Talent : unsigned {
    DieRenTalent = 1 << 0, WuYi = 1 << 1, ShenMai = 1 << 2, RuoShui = 1 << 3,
    FengShiTalent = 1 << 4, WuYu = 1 << 5, HuanYue = 1 << 6, QieYu = 1 << 7,
    XinGu = 1 << 8, TongGen = 1 << 9, FuYin = 1 << 10, GuChang = 1 << 11,
    QiSheng = 1 << 12, ChangSheng = 1 << 13, XuanMen = 1 << 14, LieYun = 1 << 15, XuJi = 1 << 16, JianRu = 1 << 17, JingHuaTalent = 1 << 18
};
inline constexpr unsigned SupportedTalents = (1 << 19) - 1;
enum Recipe : unsigned {
    Crit2 = 1 << 0, Crit3 = 1 << 1, Crit4 = 1 << 2,
    Damage3 = 1 << 3, Damage4 = 1 << 4, Damage5 = 1 << 5,
    Qidian = 1 << 6, Cooldown = 1 << 7, Prepare1 = 1 << 8, Prepare2 = 1 << 9, Prepare3 = 1 << 10,
    Range1 = 1 << 11, Range2 = 1 << 12, Range3 = 1 << 13,
    Damage40 = 1 << 14, Damage60 = 1 << 15, Dot = 1 << 16,
    YunZhongSuiXing = 1 << 17, YunZhongShengTai = 1 << 18, YunZhongTunRi = 1 << 19
};
enum EquipmentEffect : unsigned { WeaponCW = 1 << 0, SetAttribute = 1 << 1 };
enum TeamEffect : unsigned { YouRen = 1 << 0 };

// Resolved combat attributes, after equipment/class calculations. Percent ints
// use 1024; crit basis points use 10000. Attack remains split for Purple's buff.
struct Attributes {
    value_t attack_base = 10000, attack_by_class = 2500, weapon = 2000;
    int attack_percent = 0;
    value_t crit = 20000, crit_power = 15000, overcome = 25000, strain = 10000, surplus = 3000;
    // A negative base selects the legacy resolved-overcome input. XuanMen needs the split.
    value_t overcome_base = -1, overcome_by_class = 0;
    int overcome_percent = 0;
    value_t haste = 0;
    int haste_percent = 0, crit_basis_points = 0, crit_power_percent = 0;
    int damage_percent = 0, strain_percent = 0, pve_percent = 0, ignore_shield = 0;
};
struct Target {
    int level = 124;
    value_t shield = 30000;
    int vulnerable = 0;
    double life = 1.0, distance = 4.0;
};
struct Config {
    Attributes attributes;
    Target target;
    unsigned talents = 0;
    unsigned equipment = 0;
    unsigned team_effects = 0;
    std::array<unsigned, 3> recipes{}; // WuWo, BaHuang, SanHuan.
    unsigned shengtaiji_recipes = 0;
    unsigned renjian_recipes = 0;
    int initial_qidian = 10;
    tick_t delay = 0; // Fixed input delay; legacy random delay is not migrated.
    std::vector<team::Application> team_buffs;
    std::size_t attribute_version_capacity = 4096;
};

enum class FormulaKind : std::uint8_t { Direct, Surplus, Dot, Zero };
struct Formula {
    Slot skill = ZiQi;
    std::uint16_t sub = 0, level = 0;
    int fixed = 0, weapon = 0, channel = 0, count = 1;
    FormulaKind kind = FormulaKind::Zero;
    int dot_count = 8, dot_interval = 48;
};
inline constexpr std::uint16_t WuWoEffect = 0, FengWuWoEffect = 10, BaHuangEffect = 20,
    BaHuangExtraEffect = 21, SanHuanEffect = 31, SanChaiEffect = 32, PoZhaoEffect = 33,
    DieRenEffect = 36, ZeroEffect = 99, SuiXingEffect = 100, ShengTaiEffect = 101,
    TunRiEffect = 102, ChiYingEffect = 103, WanJianEffect = 104, RenJianEffect = 105,
    RenJianSuiXingEffect = 106, RenJianDotEffect = 107, JianRuEffect = 108, JianRuActiveEffect = 109,
    YunZhongSuiXingEffect = 110, YunZhongShengTaiEffect = 111, YunZhongTunRiEffect = 112,
    JingHuaEffect = 113, JingHuaSuiEffect = 114, BaHuangCwEffect = 115,
    WeaponCwExtraEffect = 116, WanXiangEffect = 117, EffectCount = 120;
enum Profile : std::uint16_t { PurpleProfile = 1, SuiXingProfile = 2, QiShengProfile = 4,
    LiveGuChangProfile = 8, LieYunProfile = 16, XuanMenProfileUnit = 32, XuanMenProfileMask = 96,
    SetAttributeProfile = 128 };
inline constexpr unsigned LiveProfileMask = LiveGuChangProfile | XuanMenProfileMask;
inline constexpr unsigned ProfileCount = 256;
inline constexpr auto Formulas = [] {
    std::array<Formula, EffectCount> result{};
    // Explicitly materialize defaults for zero effects (also on GCC 16).
    result.fill(Formula{});
    for (int q = 1; q <= 10; ++q) {
        Formula f{WuWo, 0, static_cast<std::uint16_t>(q - 1),
            (2696*q/10/12 + (2696+286)*q/10/12)/2, 2048,
            static_cast<int>(19*1.1*1.1*1.1*1.1*1.05*q), 1, FormulaKind::Direct};
        result[WuWoEffect + q - 1] = result[FengWuWoEffect + q - 1] = f;
    }
    result[BaHuangEffect] = {BaHuang, 0, 0,
        static_cast<int>((1801/15*1.1*1.05 + (1801+130)/15*1.1*1.05)/2 + 2080),
        0, 16, 1, FormulaKind::Direct};
    for (int i = 0; i < 10; ++i) {
        int d = 10 - i;
        result[BaHuangExtraEffect + i] = {BaHuang, 1, static_cast<std::uint16_t>(i),
            (690/3/d + 690/3/d + 70/3/d)/2, 2048,
            static_cast<int>((128+16*(i+1))*1.1*1.1*1.05), 1, FormulaKind::Direct};
    }
    result[SanHuanEffect] = {SanHuan, 0, 0, (1233/10+(1233+137)/10)/2, 1024,
        static_cast<int>(120*1.1*1.05*1.1), 1, FormulaKind::Direct};
    result[SanChaiEffect] = {SanChai, 0, 0, 0, 1024, 16, 1, FormulaKind::Direct};
    constexpr std::array ratios{0.065, 0.13, 0.23};
    for (int i = 0; i < 3; ++i)
        result[PoZhaoEffect + i] = {PoZhao, 0, static_cast<std::uint16_t>(i), 0, 0,
            static_cast<int>(1024*1024*(ratios[i]-1)), 1, FormulaKind::Surplus};
    for (int stack = 1; stack <= 7; ++stack)
        for (int count = 0; count <= 8; ++count)
            result[DieRenEffect + (stack-1)*9 + count] = {DieRenDamage,
                static_cast<std::uint16_t>(count == 1 ? 0 : 1), static_cast<std::uint16_t>(stack-1),
                10, 0, static_cast<int>(58*1.15*1.1*1.1)*stack, count, FormulaKind::Dot};
    result[SuiXingEffect].skill = SuiXing;
    result[ShengTaiEffect].skill = ShengTai;
    result[TunRiEffect].skill = TunRi;
    result[ChiYingEffect] = {ChiYingDamage, 0, 0, (40+40+17)/2, 0, 127, 1, FormulaKind::Direct};
    result[WanJianEffect] = {WanJian, 0, 0,
        static_cast<int>((1132/10*1.5 + (1132/10*1.5 + 113/10*1.5))/2), 0,
        static_cast<int>(100*1.1*1.25*1.05*1.5*1.1), 1, FormulaKind::Direct};
    result[RenJianEffect] = {RenJian, 0, 0, 1273/20, 0, 40, 1, FormulaKind::Direct};
    result[RenJianSuiXingEffect] = result[RenJianEffect];
    result[RenJianSuiXingEffect].skill = RenJianSuiXing;
    result[RenJianDotEffect] = {RenJianDamage, 0, 0, 16, 0, 40, 1, FormulaKind::Dot, 4, 48};
    result[JianRuEffect] = {JianRuDamage, 0, 0,
        (77+77+25)/2, 0, static_cast<int>(84*1.2*1.1*2*1.6774), 1, FormulaKind::Direct};
    result[JianRuActiveEffect] = {JianRuDamage, 1, 0,
        (77+77+25)/2, 0, static_cast<int>(280*1.1*2*1.6774), 1, FormulaKind::Direct};
    result[YunZhongSuiXingEffect] = {YunZhongDamage, 0, 0, (40+40+17)/2, 0, static_cast<int>(70*1.1), 1, FormulaKind::Direct};
    result[YunZhongShengTaiEffect] = result[YunZhongSuiXingEffect];
    result[YunZhongShengTaiEffect].sub = 1;
    result[YunZhongTunRiEffect] = result[YunZhongSuiXingEffect];
    result[YunZhongTunRiEffect].sub = 2;
    result[JingHuaEffect] = {JingHua, 0, 0, (113+113+67)/2, 1024, static_cast<int>(620*1.1*1.1), 1, FormulaKind::Direct};
    result[JingHuaSuiEffect] = {JingHua, 1, 0, 200, 0, static_cast<int>(200*1.1*1.1), 1, FormulaKind::Direct};
    result[BaHuangCwEffect] = result[BaHuangExtraEffect + 9];
    result[WeaponCwExtraEffect] = {BaHuang, 2, 0, (20+20+2)/2, 0, 65, 1, FormulaKind::Direct};
    for (int stack = 1; stack <= 3; ++stack)
        result[WanXiangEffect + stack - 1] = {WanXiangDamage, 0, static_cast<std::uint16_t>(stack-1),
            10, 0, static_cast<int>(205*stack*1.8), 1, FormulaKind::Dot, 10, 48};
    return result;
}();
static_assert(Formulas[ZeroEffect].skill == ZiQi && Formulas[ZeroEffect].kind == FormulaKind::Zero &&
    Formulas[SuiXingEffect].kind == FormulaKind::Zero);

struct DamageInput {
    value_t attack, weapon, crit_power, overcome, strain, surplus;
    int damage_percent, crit_power_percent, strain_percent, pve_percent, ignore_shield;
};
struct AttributeVersion {
    DamageInput input;
    double critical_chance = 0;
    int mitigation = 0, critical_power = 0, strain_factor = 0, vulnerable = 0;
    std::uint16_t profile = 0;
    // Keep the timeline aura inputs so a damage-only replay can resolve the
    // same snapshot under a changed base attribute configuration.
    team::Modifiers team;
};
inline value_t CalculateDamage(const Formula &f, const DamageInput &a, const Target &t, bool critical) {
    if (f.kind == FormulaKind::Zero) return 0;
    value_t raw;
    if (f.kind == FormulaKind::Surplus) {
        raw = static_cast<value_t>(a.surplus * ((f.channel + (f.channel < 0)) / 1024 + 1024) / 1024 * 13.192);
    } else {
        const double coefficient = f.kind == FormulaKind::Dot
            ? f.channel * std::max(16, f.dot_count*f.dot_interval/12) * 1.0 / f.dot_count / 10 / 16 / 16
            : std::max(f.channel, 16) * 1.0 / 10 / 16;
        raw = f.fixed + static_cast<value_t>(a.attack * coefficient) + a.weapon * f.weapon / 1024;
    }
    raw = raw * (1024 + a.damage_percent) / 1024;
    raw *= f.count; // Detonation multiplies before mitigation and each rounding step.
    const auto shield = std::max<value_t>(0, t.shield - t.shield * a.ignore_shield / 1024);
    const int overcome = static_cast<int>(a.overcome * 1024 / (9.530 * 8250) + 1024);
    const int reduction = static_cast<int>(shield * 1024 / (shield + 5.091 * (450*t.level-45750)));
    raw = raw * (overcome - overcome * reduction / 1024) / 1024;
    if (critical) {
        const int power = static_cast<int>(a.crit_power * 1024 / (3.335 * 8250));
        raw = static_cast<value_t>(raw + raw * 0.75 + raw * (power + a.crit_power_percent) / 1024);
    }
    const int delta = 120 - t.level;
    raw = static_cast<value_t>(raw * (1.0 + delta * (delta < 0 ? .05 : .15)));
    const int strain = static_cast<int>(a.strain * 1024 / (9.189 * 8250)) + 1024 + a.strain_percent;
    raw = raw * strain / 1024;
    raw = raw * (1024 + a.pve_percent) / 1024;
    return raw * (1024 + t.vulnerable) / 1024;
}

struct Prepared {
    Config config;
    double haste = 1;
    tick_t gcd = 24, sanhuan_cd = 32, sanchai_cd = 22, dot_interval = 48;
    tick_t bahuang_cd = 192, purple_cd = 1200;
    tick_t wanjian_cd = 192, renjian_cd = 320;
    int renjian_range = 6, dot_max_stacks = 5;
    tick_t field_prepare = 16, shengtaiji_prepare = 16, jinghua_prepare = 48, shengtaiji_cd = 160, suixing_duration = 384;
    int purple_charges = 2;
    value_t max_damage = 0;
    std::array<Formula, EffectCount> formulas = Formulas;
    std::array<int, EffectCount> damage_bonus{}, power_bonus{};
    std::array<double, EffectCount> coefficients{}, chance_bonus{};
    std::array<value_t, EffectCount> surplus_damage{};
    double level_multiplier = 1;

    explicit Prepared(Config input) : config(std::move(input)) {
        Validate();
        std::stable_sort(config.team_buffs.begin(), config.team_buffs.end(),
            [](const auto &a, const auto &b) { return a.frame < b.frame; });
        const auto &a = config.attributes;
        haste = 1024.0 / (static_cast<int>(a.haste / (11.695*8250) * 1024) + a.haste_percent + 1024);
        gcd = static_cast<int>(24 * haste + config.delay);
        sanhuan_cd = static_cast<int>((32 - ((config.recipes[SanHuan]&Cooldown) ? 8 : 0) + (Has(HuanYue) ? 64 : 0))*haste);
        sanchai_cd = static_cast<int>(22*haste);
        dot_interval = static_cast<int>(48*haste);
        bahuang_cd = 192 - ((config.recipes[BaHuang]&Cooldown) ? 16 : 0);
        wanjian_cd = Has(XuJi) ? 160 : 192;
        renjian_cd = (config.renjian_recipes & Cooldown) ? 240 : 320;
        renjian_range = 6 + std::popcount(config.renjian_recipes & (Range1|Range2|Range3));
        dot_max_stacks = Has(LieYun) ? 7 : 5;
        purple_cd = Has(RuoShui) ? 960 : 1200;
        purple_charges = Has(RuoShui) ? 3 : 2;
        field_prepare = static_cast<int>(16*haste);
        shengtaiji_prepare = static_cast<int>((16-2*std::popcount(config.shengtaiji_recipes & (Prepare1|Prepare2|Prepare3)))*haste);
        shengtaiji_cd = (Has(XuanMen) ? 400 : 160) - ((config.shengtaiji_recipes & Cooldown) ? 32 : 0);
        suixing_duration = Has(FuYin) ? 576 : 384;
        if (gcd < 1 || sanhuan_cd < 1 || sanchai_cd < 1 || dot_interval < 1)
            throw std::invalid_argument("haste produces a zero-length timer");
        team::Modifiers maximum;
        const int delta = 120-config.target.level;
        level_multiplier = 1.0+delta*(delta < 0 ? .05 : .15);
        for (const auto &d : team::Definitions) maximum.Add(d.modifiers, d.max_stacks);
        for (unsigned effect = 0; effect < EffectCount; ++effect) {
                auto &formula = formulas[effect];
                if (effect >= WanXiangEffect && effect < WanXiangEffect + 3)
                    formula.dot_interval = dot_interval;
                if (formula.skill == DieRenDamage && Has(XuJi)) formula.channel = static_cast<int>(formula.channel*1.2);
                const auto stats = Input(effect, 0);
                damage_bonus[effect] = stats.damage_percent - a.damage_percent;
                power_bonus[effect] = stats.crit_power_percent - a.crit_power_percent;
                chance_bonus[effect] = ExtraChance(effect)*1.0/10000;
                coefficients[effect] = formula.kind == FormulaKind::Dot
                    ? formula.channel * std::max(16, formula.dot_count*formula.dot_interval/12) * 1.0 / formula.dot_count / 10 / 16 / 16
                    : std::max(formula.channel, 16) * 1.0 / 10 / 16;
                if (formula.kind == FormulaKind::Surplus)
                    surplus_damage[effect] = static_cast<value_t>(a.surplus * ((formula.channel + (formula.channel < 0)) / 1024 + 1024) / 1024 * 13.192);
                auto target = TargetFor(maximum);
                target.shield = 0;
                max_damage = std::max(max_damage, CalculateDamage(formula, Input(effect, ProfileCount-1, maximum), target, true));
        }
    }
    bool Has(Talent talent) const { return (config.talents & talent) != 0; }
    bool HasEquipment(EquipmentEffect effect) const { return (config.equipment & effect) != 0; }
    bool HasTeam(TeamEffect effect) const { return (config.team_effects & effect) != 0; }
    DamageInput Input(unsigned effect, unsigned snapshot, const team::Modifiers &team = {}) const {
        auto a = config.attributes;
        a.attack_percent += team.attack_percent;
        if (a.overcome_base >= 0) a.overcome_base += team.overcome_base;
        else a.overcome += team.overcome_base;
        a.overcome_percent += team.overcome_percent;
        a.strain += team.strain;
        a.ignore_shield += team.ignore_shield;
        const auto &f = Formulas[effect];
        const int attack_bonus = ((snapshot & PurpleProfile) ? 256 : 0) + ((snapshot & QiShengProfile) ? 102 : 0);
        const int power_bonus = ((snapshot & PurpleProfile) ? 256 : 0) + ((snapshot & SuiXingProfile) ? (Has(FuYin) ? 205 : 102) : 0) + ((snapshot & LieYunProfile) ? 154 : 0);
        const auto overcome = a.overcome_base < 0 ? a.overcome :
            a.overcome_base*(1024+a.overcome_percent+204*((snapshot & XuanMenProfileMask)/XuanMenProfileUnit))/1024 + a.overcome_by_class;
        DamageInput result{a.attack_base*(1024+a.attack_percent+attack_bonus)/1024 + a.attack_by_class,
            a.weapon, a.crit_power, overcome, a.strain, a.surplus, a.damage_percent,
            a.crit_power_percent+power_bonus+((snapshot & SetAttributeProfile) ? 41 : 0), a.strain_percent, a.pve_percent,
            a.ignore_shield+((snapshot & LiveGuChangProfile) ? 614 : 0)};
        if (f.skill <= SanHuan) {
            const auto r = config.recipes[f.skill];
            result.damage_percent += ((r&Damage3) ? 31 : 0) + ((r&Damage4) ? 41 : 0) + ((r&Damage5) ? 51 : 0);
        }
        if (f.skill == WuWo) {
            if (Has(TongGen)) result.damage_percent += 102;
            if (Has(WuYi) && f.level >= 5) result.crit_power_percent += 307;
            if (effect >= FengWuWoEffect) result.damage_percent += 307;
        }
        if (f.skill == SanHuan && Has(XinGu)) result.crit_power_percent += 102;
        if (f.skill == SanChai) result.damage_percent += 205;
        if ((f.skill == WuWo || f.skill == BaHuang) && (config.equipment & WeaponCW)) result.damage_percent += 51;
        if (f.skill == WanJian) {
            if (Has(TongGen)) result.damage_percent += 102;
            if (Has(XuJi)) result.crit_power_percent += 1102; // Both old additions modify power.
        }
        if (IsRenJian(f.skill)) {
            result.damage_percent += ((config.renjian_recipes & Damage40) ? 409 : 0)
                + ((config.renjian_recipes & Damage60) ? 614 : 0);
        }
        return result;
    }
    Target TargetFor(const team::Modifiers &team) const {
        auto result = config.target;
        result.shield = std::max<value_t>(0, result.shield + team.shield);
        result.vulnerable += team.vulnerable;
        return result;
    }
    value_t Damage(unsigned profile, unsigned effect, bool critical) const {
        const auto version = Resolve(profile);
        return Damage(version, version, effect, critical);
    }
    AttributeVersion Resolve(unsigned profile, const team::Modifiers &team = {}) const {
        const auto input = Input(ZeroEffect, profile, team);
        const auto target = TargetFor(team);
        const auto shield = std::max<value_t>(0, target.shield-target.shield*input.ignore_shield/1024);
        const int overcome = static_cast<int>(input.overcome*1024/(9.530*8250)+1024);
        const int reduction = static_cast<int>(shield*1024/(shield+5.091*(450*target.level-45750)));
        return {input, Chance(ZeroEffect, profile, team.crit_basis_points), overcome-overcome*reduction/1024,
            static_cast<int>(input.crit_power*1024/(3.335*8250)),
            static_cast<int>(input.strain*1024/(9.189*8250))+1024+input.strain_percent,
            target.vulnerable, static_cast<std::uint16_t>(profile), team};
    }
    value_t Damage(const AttributeVersion &frozen, const AttributeVersion &live, unsigned effect, bool critical) const {
        const auto &f = formulas[effect];
        if (f.kind == FormulaKind::Zero) return 0;
        value_t raw = f.kind == FormulaKind::Surplus ? surplus_damage[effect] :
            f.fixed + static_cast<value_t>(frozen.input.attack*coefficients[effect]) + live.input.weapon*f.weapon/1024;
        raw = raw*(1024+frozen.input.damage_percent+damage_bonus[effect])/1024;
        raw *= f.count;
        raw = raw*live.mitigation/1024;
        if (critical) raw = static_cast<value_t>(raw+raw*.75 +
            raw*(frozen.critical_power+frozen.input.crit_power_percent+power_bonus[effect])/1024);
        raw = static_cast<value_t>(raw*level_multiplier);
        raw = raw*frozen.strain_factor/1024;
        raw = raw*(1024+live.input.pve_percent)/1024;
        return raw*(1024+live.vulnerable)/1024;
    }
    int ExtraChance(unsigned effect) const {
        const auto &f = Formulas[effect];
        int extra = 0;
        if (f.skill <= SanHuan) {
            const auto r = config.recipes[f.skill];
            extra = ((r&Crit2) ? 200 : 0) + ((r&Crit3) ? 300 : 0) + ((r&Crit4) ? 400 : 0);
        }
        if (f.skill == WuWo && f.level >= 5 && Has(WuYi)) extra += 1000;
        if (f.skill == SanHuan && Has(XinGu)) extra += 1000;
        return extra;
    }
    double Chance(unsigned effect, unsigned snapshot, int team_crit = 0) const {
        const auto &a = config.attributes;
        // Keep the legacy addition order at probability boundaries.
        const int bonus = ((snapshot & PurpleProfile) ? 2500 : 0) + ((snapshot & SuiXingProfile) ? (Has(FuYin) ? 1000 : 500) : 0)
            + 300*((snapshot & XuanMenProfileMask)/XuanMenProfileUnit) + ((snapshot & SetAttributeProfile) ? 400 : 0);
        return a.crit/(9.530*8250) + (a.crit_basis_points+bonus+team_crit)*1.0/10000 + ExtraChance(effect)*1.0/10000;
    }
private:
    void Validate() const {
        if (!config.attribute_version_capacity || config.attribute_version_capacity > 10000000 || config.team_buffs.size() > 100000)
            throw std::invalid_argument("unsupported attribute version capacity or team schedule size");
        for (const auto &app : config.team_buffs) {
            team::Validate(app);
            if (team::Definitions[app.kind].modifiers.overcome_percent && config.attributes.overcome_base < 0)
                throw std::invalid_argument("team overcome percent requires explicit overcome_base");
        }
        if (config.talents & ~SupportedTalents) throw std::invalid_argument("unmigrated talent flags");
        if (config.equipment & ~(WeaponCW|SetAttribute)) throw std::invalid_argument("unmigrated equipment effects");
        if (config.team_effects & ~YouRen) throw std::invalid_argument("unmigrated team effects");
        if (config.shengtaiji_recipes & ~(Prepare1|Prepare2|Prepare3|Cooldown))
            throw std::invalid_argument("unsupported ShengTaiJi recipe");
        if ((config.renjian_recipes & ~(Cooldown|Range1|Range2|Range3|Damage40|Damage60|Dot|YunZhongSuiXing|YunZhongShengTai|YunZhongTunRi)) || std::popcount(config.renjian_recipes) > 4)
            throw std::invalid_argument("unsupported RenJian recipe or more than four recipes");
        constexpr std::array<unsigned, 3> allowed{127, Damage3|Damage4|Damage5|Qidian|Cooldown, 191};
        for (unsigned i = 0; i < 3; ++i)
            if ((config.recipes[i] & ~allowed[i]) || std::popcount(config.recipes[i]) > 4)
                throw std::invalid_argument("unsupported recipes or more than four recipes on a skill");
        const auto &a = config.attributes;
        if (a.overcome_base < -1 || a.overcome_base > 1000000 || a.overcome_by_class < 0 || a.overcome_by_class > 1000000 ||
            (a.overcome_base < 0 && (Has(XuanMen) || a.overcome_by_class || a.overcome_percent)))
            throw std::invalid_argument("XuanMen/split overcome requires explicit overcome_base and valid overcome_by_class");
        for (auto v : {a.attack_base, a.attack_by_class, a.weapon, a.crit, a.crit_power, a.overcome, a.strain, a.surplus, a.haste})
            if (v < 0 || v > 1000000) throw std::invalid_argument("combat attribute outside supported range");
        for (auto v : {a.attack_percent, a.overcome_percent, a.haste_percent, a.crit_basis_points, a.crit_power_percent,
                      a.damage_percent, a.strain_percent, a.pve_percent, config.target.vulnerable})
            if (v < 0 || v > 10000) throw std::invalid_argument("attribute bonus outside supported range");
        if (a.ignore_shield < 0 || a.ignore_shield > 1024 || config.target.shield < 0 || config.target.shield > 10000000 ||
            config.target.level < 110 || config.target.level > 130 ||
            !std::isfinite(config.target.life) || config.target.life < .01 || config.target.life > 1 ||
            !std::isfinite(config.target.distance) || config.target.distance < 0 ||
            config.initial_qidian < 0 || config.initial_qidian > 10 || config.delay < 0 || config.delay > 160)
            throw std::invalid_argument("unsupported target, resource or delay configuration");
    }
};

inline std::shared_ptr<const Prepared> Prepare(Config config = {}) {
    return std::make_shared<const Prepared>(std::move(config));
}

} // namespace JX3DPS::runtime::tai_xu
#endif
