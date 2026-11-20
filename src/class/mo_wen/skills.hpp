#pragma once

#include "src/core/runtime/specialization.hpp"

namespace JX3DPS::runtime::mo_wen {

// Pinned to ba960f7, level 120. Negative IDs are report labels, not game IDs.
enum SkillSlot : Slot
{
    PO_ZHAO,
    GONG,
    BIAN_GONG,
    SHANG,
    JUE,
    ZHI,
    BIAN_ZHI,
    YU,
    GAO_SHAN,
    YANG_CHUN,
    SHU_YING,
    GU_YING,
    ZHENG_LV1,
    ZHENG_LV2,
    ZHENG_LV3,
    YI_XING,
    WU_YIN,
    SHANG_DAMAGE,
    JUE_DAMAGE,
    XIAN_FENG_DAMAGE,
    LIU_ZHAO_DAMAGE,
    LIU_ZHAO_SURPLUS,
    ZHENG_MING_DAMAGE,
    SHADOW_DAMAGE,
    GONG_CW_DAMAGE,
    BIAN_GONG_CW_DAMAGE,
    YU_CW_DAMAGE,
    SHEN_BING_DAMAGE,
    ZHI_YIN_DAMAGE,
    SHOES_DAMAGE,
    WRIST_DAMAGE,
    SKILL_COUNT
};

enum BuffSlot : Slot
{
    SHANG_DOT,
    JUE_DOT,
    XIAN_FENG,
    XIAN_FENG_MARK,
    YUN_HAN,
    CAN_LIAN,
    MING_JIN,
    LIU_ZHAO,
    QU_FENG,
    GAO_SHAN_BUFF,
    YANG_CHUN_BUFF,
    ZHI_YIN_MIAO_YI,
    GU_YING_BUFF,
    ZHENG_LV_BUFF,
    ZHI_YIN_HE_MING,
    YING_ZI,
    SET_ATTRIBUTE_BUFF,
    WEAPON_CW_BUFF,
    SHEN_BING_GONG,
    YOU_REN_BUFF,
    JING_MIAO_BUFF,
    SHOES_BUFF,
    WRIST_BUFF,
    BELT_BUFF,
    WATER_BUFF,
    BUFF_COUNT
};

inline constexpr std::array<Slot, 16> CastableSkills{
    GONG,      BIAN_GONG, SHANG,   JUE,       ZHI,       BIAN_ZHI,  YU,      GAO_SHAN,
    YANG_CHUN, SHU_YING,  GU_YING, ZHENG_LV1, ZHENG_LV2, ZHENG_LV3, YI_XING, WU_YIN
};

constexpr bool IsCastable(Slot s)
{
    return s >= GONG && s <= WU_YIN;
}

constexpr bool IsZhi(Slot s)
{
    return s == ZHI || s == BIAN_ZHI;
}

constexpr bool IsStyleSkill(Slot s)
{
    return s == GAO_SHAN || s == YANG_CHUN;
}

constexpr bool IsZhengLv(Slot s)
{
    return s >= ZHENG_LV1 && s <= ZHENG_LV3;
}

constexpr bool UsesGcd(Slot s)
{
    return (s >= GONG && s <= YU) || IsZhengLv(s);
}

constexpr bool IsDot(Slot b)
{
    return b == SHANG_DOT || b == JUE_DOT || b == SHEN_BING_GONG;
}

inline constexpr DescriptorTable<SkillDescriptor, SKILL_COUNT> Skills{
    { { { -1000, "破招", 0, 0, 0 },
        { -1001, "宫", 0, 24, 28 },
        { -1002, "变宫", 0, 24, 28 },
        { -1003, "商", 0, 24, 0 },
        { -1004, "角", 0, 24, 0 },
        { -1005, "徵", 0, 24, 16 },
        { -1006, "变徵", 0, 24, 16 },
        { -1007, "羽", 96, 24, 0 },
        { -1008, "高山流水", 0, 0, 6 },
        { -1009, "阳春白雪", 48, 0, 6 },
        { -1010, "疏影横斜", 320, 0, 0 },
        { -1011, "孤影化双", 2400, 0, 0 },
        { -1012, "正律和鸣", 960, 24, 0 },
        { -1013, "正律和鸣·二", 960, 24, 0 },
        { -1014, "正律和鸣·三", 960, 24, 0 },
        { -1015, "移形换影", 0, 0, 0 },
        { -1016, "五音六律", 16, 0, 0 },
        { -1017, "Dot·商", 0, 0, 0 },
        { -1018, "Dot·角", 0, 0, 0 },
        { -1019, "弦风", 0, 0, 0 },
        { -1020, "流照", 0, 0, 0 },
        { -1021, "破招·流照", 0, 0, 0 },
        { -1022, "争鸣", 0, 0, 0 },
        { -1023, "影子", 0, 0, 0 },
        { -1024, "神兵·宫", 0, 0, 0 },
        { -1025, "神兵·变宫", 0, 0, 0 },
        { -1026, "神兵·羽", 0, 0, 0 },
        { -1027, "神兵·宫·DOT", 0, 0, 0 },
        { -1028, "知音行尽", 0, 0, 0 },
        { -1029, "大附魔·鞋", 0, 0, 0 },
        { -1030, "大附魔·腕", 0, 0, 0 } } }
};
inline constexpr DescriptorTable<BuffDescriptor, BUFF_COUNT> Buffs{
    { { { -2000, "Dot·商", 288, 48, 1 },  { -2001, "Dot·角", 288, 48, 1 },  { -2002, "弦风", 160, 0, 5 },
        { -2003, "弦风标记", 53, 0, 1 },  { -2004, "云汉", 1600, 0, 9 },    { -2005, "参连", 0, 0, 1 },
        { -2006, "书离", 128, 0, 1 },     { -2007, "流照", 400, 0, 32 },    { -2008, "曲风", 0, 0, 9 },
        { -2009, "高山流水", 192, 0, 1 }, { -2010, "阳春白雪", 96, 0, 3 },  { -2011, "知音妙意", 240, 0, 3 },
        { -2012, "孤影化双", 112, 0, 1 }, { -2013, "正律和鸣", 320, 0, 1 }, { -2014, "知音和鸣", 320, 0, 5 },
        { -2015, "影子", 400, 32, 6 },    { -2016, "套装·挥散", 96, 0, 1 }, { -2017, "武器·橙武特效", 96, 0, 1 },
        { -2018, "神兵·宫", 480, 48, 3 }, { -2019, "游刃", 80, 0, 1 },      { -2020, "精妙", 80, 0, 1 },
        { -2021, "大附魔·鞋", 0, 0, 1 },  { -2022, "大附魔·腕", 0, 0, 1 },  { -2023, "大附魔·腰", 128, 0, 1 },
        { -2024, "水·斩流", 96, 0, 10 } } }
};
inline constexpr std::array<std::string_view, SKILL_COUNT> SkillAliases{
    "pozhao",
    "gong",
    "biangong",
    "shang",
    "jue",
    "zhi",
    "bianzhi",
    "yu",
    "gaoshan",
    "yangchun",
    "shuying",
    "guying",
    "zhenglv1",
    "zhenglv2",
    "zhenglv3",
    "yixing",
    "wuyin",
    "shang_dot",
    "jue_dot",
    "xianfeng_damage",
    "liuzhao_damage",
    "liuzhao_surplus",
    "zhengming",
    "shadow_damage",
    "gong_cw",
    "biangong_cw",
    "yu_cw",
    "shenbing_dot",
    "zhiyin_damage",
    "shoes_damage",
    "wrist_damage"
};
inline constexpr std::array<std::string_view, BUFF_COUNT> BuffAliases{
    "shang",        "jue",    "xianfeng",      "xianfeng_mark", "yunhan",        "canlian", "mingjin",
    "liuzhao",      "qufeng", "gaoshan",       "yangchun",      "zhiyinmiaoyi",  "guying",  "zhenglv",
    "zhiyinheming", "yingzi", "set_attribute", "weapon_cw",     "shenbing_gong", "you_ren", "jingmiao",
    "shoes",        "wrist",  "belt",          "weapon_water"
};
static_assert(Skills.Valid() && Buffs.Valid());
inline constexpr std::array<NameBinding, 2> ExtraBuffNames{
    { { Skills.entries[SHANG].name, SHANG_DOT }, { Skills.entries[JUE].name, JUE_DOT } }
};
inline constexpr SkillCatalog Catalog{ Skills.entries, Buffs.entries, CastableSkills,
                                       SkillAliases,   BuffAliases,   ExtraBuffNames };

inline CompileOptions MacroOptions(bool debug = true)
{
    return CatalogMacroOptions(Catalog, debug);
}

} // namespace JX3DPS::runtime::mo_wen
