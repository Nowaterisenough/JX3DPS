#pragma once

#include "src/core/runtime/specialization.hpp"

#include "team_buffs.hpp"

namespace JX3DPS::runtime::tai_xu {

// Behavior is pinned to ba960f7 (level 120); external IDs only label reports.
enum SkillSlot : Slot
{
    WU_WO,
    BA_HUANG,
    SAN_HUAN,
    SAN_CHAI,
    ZI_QI,
    PO_ZHAO,
    DIE_REN_DAMAGE,
    SUI_XING,
    SHENG_TAI,
    TUN_RI,
    CHI_YING_DAMAGE,
    WAN_JIAN,
    REN_JIAN,
    REN_JIAN_SUI_XING,
    REN_JIAN_DAMAGE,
    JIAN_RU_DAMAGE,
    YUN_ZHONG_DAMAGE,
    JING_HUA,
    WAN_XIANG_DAMAGE,
    SKILL_COUNT
};

enum BuffSlot : Slot
{
    DIE_REN,
    PURPLE,
    FENG_SHI,
    SUI_XING_BUFF,
    TUN_RI_BUFF,
    QI_SHENG_BUFF,
    CHI_YING_BUFF,
    FIELD_SUI_XING,
    FIELD_SHENG_TAI,
    FIELD_TUN_RI,
    XUAN_MEN_BUFF,
    LIE_YUN_BUFF,
    HIDDEN_LIE_YUN,
    REN_JIAN_DOT,
    JIAN_RU_BUFF,
    YUN_ZHONG_SUI_XING_BUFF,
    YUN_ZHONG_SHENG_TAI_BUFF,
    YUN_ZHONG_TUN_RI_BUFF,
    JING_HUA_BUFF,
    WEAPON_CW_BUFF,
    WAN_XIANG_BUFF,
    SET_ATTRIBUTE_BUFF,
    YOU_REN_BUFF,
    TEAM_BUFF_BEGIN,
    BUFF_COUNT = TEAM_BUFF_BEGIN + team::COUNT
};

inline constexpr std::array<Slot, 12> CastableSkills{
    WU_WO,  BA_HUANG, SAN_HUAN, SAN_CHAI,          ZI_QI,   SUI_XING, SHENG_TAI,
    TUN_RI, WAN_JIAN, REN_JIAN, REN_JIAN_SUI_XING, JING_HUA
};

constexpr bool IsFieldSkill(Slot skill)
{
    return skill >= SUI_XING && skill <= TUN_RI;
}

constexpr bool IsRenJian(Slot skill)
{
    return skill == REN_JIAN || skill == REN_JIAN_SUI_XING;
}

constexpr bool IsCastable(Slot skill)
{
    return skill <= ZI_QI || IsFieldSkill(skill) || skill == WAN_JIAN || IsRenJian(skill) || skill == JING_HUA;
}

constexpr bool UsesGcd(Slot skill)
{
    return skill <= SAN_HUAN || IsFieldSkill(skill) || skill == WAN_JIAN || IsRenJian(skill) || skill == JING_HUA;
}

inline constexpr DescriptorTable<SkillDescriptor, SKILL_COUNT> Skills{
    { { { 32696, "无我无剑", 0, 24, 0 },
        { 32882, "八荒归元", 192, 24, 0 },
        { 32700, "三环套月", 32, 24, 0 },
        { 32704, "三柴剑法", 22, 0, 0 },
        { 32715, "紫气东来", 1200, 0, 0 },
        { 32897, "破招", 0, 0, 0 },
        { 9745, "叠刃", 0, 0, 0 },
        { 32708, "碎星辰", 160, 24, 16 },
        { 32711, "生太极", 160, 24, 16 },
        { 32707, "吞日月", 160, 24, 16 },
        { 28626, "持盈", 0, 0, 0 },
        { 32703, "万剑归宗", 192, 24, 0 },
        { 32705, "人剑合一", 320, 24, 0 },
        { -6, "人剑合一·空爆·碎星辰", 320, 24, 0 },
        { 9732, "人剑合一·DOT", 0, 0, 0 },
        { 25766, "剑入", 0, 0, 0 },
        { -8, "云中剑", 0, 0, 0 },
        { 32898, "镜花影", 0, 24, 48 },
        { 18931, "万象归元", 0, 0, 0 } } }
};
inline constexpr auto Buffs = [] {
    DescriptorTable<BuffDescriptor, BUFF_COUNT> result{
        { { { 9745, Skills.entries[DIE_REN_DAMAGE].name, 384, 48, 7 },
            { 19267, Skills.entries[ZI_QI].name, 160, 16, 1 },
            { 17932, "风逝", 80, 0, 1 },
            // Negative IDs are internal labels where the pinned source has no game ID.
            { -1, Skills.entries[SUI_XING].name, 64, 0, 1 },
            { -2, Skills.entries[TUN_RI].name, 20, 0, 1 },
            { 23393, "期声", 48, 0, 1 },
            { 28626, Skills.entries[CHI_YING_DAMAGE].name, 240, 0, 3 },
            { -3, "气场·碎星辰", 384, 48, 3 },
            { -4, "气场·生太极", 384, 48, 3 },
            { -5, "气场·吞日月", 384, 20, 3 },
            { 23367, "玄门", 640, 0, 3 },
            { 25711, "裂云", 16, 0, 1 },
            { -7, "隐藏·裂云", 384, 8, 1 },
            { 9732, Skills.entries[REN_JIAN].name, 192, 48, 1 },
            { 25766, "剑入", 98, 16, 1 },
            { -9, "云中剑·碎星辰", 128, 16, 1 },
            { -10, "云中剑·生太极", 128, 16, 1 },
            { -11, "云中剑·吞日月", 128, 16, 1 },
            { 28598, "镜花影", 128, 0, 1 },
            { -12, "橙武特效·CW", 96, 0, 1 },
            { 18931, "万象归元", 480, 48, 3 },
            { -13, "套装·剑鸣", 96, 0, 1 },
            { -14, "游刃", 320, 0, 5 } } }
    };
    for (Slot i = 0; i < team::COUNT; ++i) {
        const auto &d                       = team::Definitions[i];
        result.entries[TEAM_BUFF_BEGIN + i] = { -100 - static_cast<int>(i), d.name, d.duration, 0, d.max_stacks };
    }
    return result;
}();
inline constexpr std::array<std::string_view, SKILL_COUNT> SkillAliases{
    "wuwu",        "bahuang",    "sanhuan",      "sanchai", "ziqi",    "pozhao",  "dieren",
    "suixing",     "shengtaiji", "tunriyue",     "chiying", "wanjian", "renjian", "renjian_suixing",
    "renjian_dot", "jianru",     "yunzhongjian", "jinghua", "wanxiang"
};
inline constexpr auto BuffAliases = [] {
    std::array<std::string_view, BUFF_COUNT> result{
        "dieren",
        "ziqi",
        "fengshi",
        "suixing",
        "tunriyue",
        "qisheng",
        "chiying",
        "field_suixing",
        "field_shengtaiji",
        "field_tunriyue",
        "xuanmen",
        "lieyun",
        "hidden_lieyun",
        "renjian",
        "jianru",
        "yunzhong_suixing",
        "yunzhong_shengtaiji",
        "yunzhong_tunriyue",
        "jinghua",
        "weapon_cw",
        "wanxiang",
        "set_attribute",
        "you_ren"
    };
    for (Slot i = 0; i < team::COUNT; ++i) {
        result[TEAM_BUFF_BEGIN + i] = team::Definitions[i].alias;
    }
    return result;
}();
static_assert(Skills.Valid() && Buffs.Valid());

inline constexpr SkillCatalog Catalog{ Skills.entries, Buffs.entries, CastableSkills, SkillAliases, BuffAliases };

inline CompileOptions MacroOptions(bool debug = true)
{
    return CatalogMacroOptions(Catalog, debug);
}

} // namespace JX3DPS::runtime::tai_xu
