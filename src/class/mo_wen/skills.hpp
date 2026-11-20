#pragma once

#include "src/core/runtime/specialization.hpp"

namespace JX3DPS::runtime::mo_wen {

// Pinned to ba960f7, level 120. Negative IDs are report labels, not game IDs.
enum SkillSlot : Slot {
    PoZhao, Gong, BianGong, Shang, Jue, Zhi, BianZhi, Yu, GaoShan, YangChun,
    ShuYing, GuYing, ZhengLv1, ZhengLv2, ZhengLv3, YiXing, WuYin,
    ShangDamage, JueDamage, XianFengDamage, LiuZhaoDamage, LiuZhaoSurplus,
    ZhengMingDamage, ShadowDamage, GongCwDamage, BianGongCwDamage, YuCwDamage,
    ShenBingDamage, ZhiYinDamage, ShoesDamage, WristDamage, SkillCount
};
enum BuffSlot : Slot {
    ShangDot, JueDot, XianFeng, XianFengMark, YunHan, CanLian, MingJin,
    LiuZhao, QuFeng, GaoShanBuff, YangChunBuff, ZhiYinMiaoYi, GuYingBuff,
    ZhengLvBuff, ZhiYinHeMing, YingZi, SetAttributeBuff, WeaponCwBuff,
    ShenBingGong, YouRenBuff, JingMiaoBuff, ShoesBuff, WristBuff, BeltBuff, WaterBuff, BuffCount
};
inline constexpr std::array<Slot, 16> CastableSkills{
    Gong, BianGong, Shang, Jue, Zhi, BianZhi, Yu, GaoShan, YangChun,
    ShuYing, GuYing, ZhengLv1, ZhengLv2, ZhengLv3, YiXing, WuYin
};
inline constexpr bool IsCastable(Slot s) { return s>=Gong && s<=WuYin; }
inline constexpr bool IsZhi(Slot s) { return s==Zhi || s==BianZhi; }
inline constexpr bool IsStyleSkill(Slot s) { return s==GaoShan || s==YangChun; }
inline constexpr bool IsZhengLv(Slot s) { return s>=ZhengLv1 && s<=ZhengLv3; }
inline constexpr bool UsesGcd(Slot s) { return (s>=Gong && s<=Yu) || IsZhengLv(s); }
inline constexpr bool IsDot(Slot b) { return b==ShangDot || b==JueDot || b==ShenBingGong; }
inline constexpr DescriptorTable<SkillDescriptor, SkillCount> Skills{{{
    {-1000, "\xe7\xa0\xb4\xe6\x8b\x9b", 0, 0, 0},
    {-1001, "\xe5\xae\xab", 0, 24, 28},
    {-1002, "\xe5\x8f\x98\xe5\xae\xab", 0, 24, 28},
    {-1003, "\xe5\x95\x86", 0, 24, 0},
    {-1004, "\xe8\xa7\x92", 0, 24, 0},
    {-1005, "\xe5\xbe\xb5", 0, 24, 16},
    {-1006, "\xe5\x8f\x98\xe5\xbe\xb5", 0, 24, 16},
    {-1007, "\xe7\xbe\xbd", 96, 24, 0},
    {-1008, "\xe9\xab\x98\xe5\xb1\xb1\xe6\xb5\x81\xe6\xb0\xb4", 0, 0, 6},
    {-1009, "\xe9\x98\xb3\xe6\x98\xa5\xe7\x99\xbd\xe9\x9b\xaa", 48, 0, 6},
    {-1010, "\xe7\x96\x8f\xe5\xbd\xb1\xe6\xa8\xaa\xe6\x96\x9c", 320, 0, 0},
    {-1011, "\xe5\xad\xa4\xe5\xbd\xb1\xe5\x8c\x96\xe5\x8f\x8c", 2400, 0, 0},
    {-1012, "\xe6\xad\xa3\xe5\xbe\x8b\xe5\x92\x8c\xe9\xb8\xa3", 960, 24, 0},
    {-1013, "\xe6\xad\xa3\xe5\xbe\x8b\xe5\x92\x8c\xe9\xb8\xa3\xc2\xb7\xe4\xba\x8c", 960, 24, 0},
    {-1014, "\xe6\xad\xa3\xe5\xbe\x8b\xe5\x92\x8c\xe9\xb8\xa3\xc2\xb7\xe4\xb8\x89", 960, 24, 0},
    {-1015, "\xe7\xa7\xbb\xe5\xbd\xa2\xe6\x8d\xa2\xe5\xbd\xb1", 0, 0, 0},
    {-1016, "\xe4\xba\x94\xe9\x9f\xb3\xe5\x85\xad\xe5\xbe\x8b", 16, 0, 0},
    {-1017, "\x44\x6f\x74\xc2\xb7\xe5\x95\x86", 0, 0, 0},
    {-1018, "\x44\x6f\x74\xc2\xb7\xe8\xa7\x92", 0, 0, 0},
    {-1019, "\xe5\xbc\xa6\xe9\xa3\x8e", 0, 0, 0},
    {-1020, "\xe6\xb5\x81\xe7\x85\xa7", 0, 0, 0},
    {-1021, "\xe7\xa0\xb4\xe6\x8b\x9b\xc2\xb7\xe6\xb5\x81\xe7\x85\xa7", 0, 0, 0},
    {-1022, "\xe4\xba\x89\xe9\xb8\xa3", 0, 0, 0},
    {-1023, "\xe5\xbd\xb1\xe5\xad\x90", 0, 0, 0},
    {-1024, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe5\xae\xab", 0, 0, 0},
    {-1025, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe5\x8f\x98\xe5\xae\xab", 0, 0, 0},
    {-1026, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe7\xbe\xbd", 0, 0, 0},
    {-1027, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe5\xae\xab\xc2\xb7\x44\x4f\x54", 0, 0, 0},
    {-1028, "\xe7\x9f\xa5\xe9\x9f\xb3\xe8\xa1\x8c\xe5\xb0\xbd", 0, 0, 0},
    {-1029, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe9\x9e\x8b", 0, 0, 0},
    {-1030, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe8\x85\x95", 0, 0, 0}
}}};
inline constexpr DescriptorTable<BuffDescriptor, BuffCount> Buffs{{{
    {-2000, "\x44\x6f\x74\xc2\xb7\xe5\x95\x86", 288, 48, 1},
    {-2001, "\x44\x6f\x74\xc2\xb7\xe8\xa7\x92", 288, 48, 1},
    {-2002, "\xe5\xbc\xa6\xe9\xa3\x8e", 160, 0, 5},
    {-2003, "\xe5\xbc\xa6\xe9\xa3\x8e\xe6\xa0\x87\xe8\xae\xb0", 53, 0, 1},
    {-2004, "\xe4\xba\x91\xe6\xb1\x89", 1600, 0, 9},
    {-2005, "\xe5\x8f\x82\xe8\xbf\x9e", 0, 0, 1},
    {-2006, "\xe4\xb9\xa6\xe7\xa6\xbb", 128, 0, 1},
    {-2007, "\xe6\xb5\x81\xe7\x85\xa7", 400, 0, 32},
    {-2008, "\xe6\x9b\xb2\xe9\xa3\x8e", 0, 0, 9},
    {-2009, "\xe9\xab\x98\xe5\xb1\xb1\xe6\xb5\x81\xe6\xb0\xb4", 192, 0, 1},
    {-2010, "\xe9\x98\xb3\xe6\x98\xa5\xe7\x99\xbd\xe9\x9b\xaa", 96, 0, 3},
    {-2011, "\xe7\x9f\xa5\xe9\x9f\xb3\xe5\xa6\x99\xe6\x84\x8f", 240, 0, 3},
    {-2012, "\xe5\xad\xa4\xe5\xbd\xb1\xe5\x8c\x96\xe5\x8f\x8c", 112, 0, 1},
    {-2013, "\xe6\xad\xa3\xe5\xbe\x8b\xe5\x92\x8c\xe9\xb8\xa3", 320, 0, 1},
    {-2014, "\xe7\x9f\xa5\xe9\x9f\xb3\xe5\x92\x8c\xe9\xb8\xa3", 320, 0, 5},
    {-2015, "\xe5\xbd\xb1\xe5\xad\x90", 400, 32, 6},
    {-2016, "\xe5\xa5\x97\xe8\xa3\x85\xc2\xb7\xe6\x8c\xa5\xe6\x95\xa3", 96, 0, 1},
    {-2017, "\xe6\xad\xa6\xe5\x99\xa8\xc2\xb7\xe6\xa9\x99\xe6\xad\xa6\xe7\x89\xb9\xe6\x95\x88", 96, 0, 1},
    {-2018, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe5\xae\xab", 480, 48, 3},
    {-2019, "\xe6\xb8\xb8\xe5\x88\x83", 80, 0, 1},
    {-2020, "\xe7\xb2\xbe\xe5\xa6\x99", 80, 0, 1},
    {-2021, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe9\x9e\x8b", 0, 0, 1},
    {-2022, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe8\x85\x95", 0, 0, 1},
    {-2023, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe8\x85\xb0", 128, 0, 1},
    {-2024, "\xe6\xb0\xb4\xc2\xb7\xe6\x96\xa9\xe6\xb5\x81", 96, 0, 10}
}}};
inline constexpr std::array<std::string_view, SkillCount> SkillAliases{
    "pozhao", "gong", "biangong", "shang", "jue", "zhi", "bianzhi", "yu",
    "gaoshan", "yangchun", "shuying", "guying", "zhenglv1", "zhenglv2", "zhenglv3",
    "yixing", "wuyin", "shang_dot", "jue_dot", "xianfeng_damage", "liuzhao_damage",
    "liuzhao_surplus", "zhengming", "shadow_damage", "gong_cw", "biangong_cw",
    "yu_cw", "shenbing_dot", "zhiyin_damage", "shoes_damage", "wrist_damage"
};
inline constexpr std::array<std::string_view, BuffCount> BuffAliases{
    "shang", "jue", "xianfeng", "xianfeng_mark", "yunhan", "canlian", "mingjin",
    "liuzhao", "qufeng", "gaoshan", "yangchun", "zhiyinmiaoyi", "guying",
    "zhenglv", "zhiyinheming", "yingzi", "set_attribute", "weapon_cw",
    "shenbing_gong", "you_ren", "jingmiao", "shoes", "wrist", "belt", "weapon_water"
};
static_assert(Skills.Valid() && Buffs.Valid());
inline constexpr std::array<NameBinding, 2> ExtraBuffNames{{
    {Skills.entries[Shang].name, ShangDot}, {Skills.entries[Jue].name, JueDot}
}};
inline constexpr SkillCatalog Catalog{Skills.entries, Buffs.entries, CastableSkills,
    SkillAliases, BuffAliases, ExtraBuffNames};
inline CompileOptions MacroOptions(bool debug=true) {
    return CatalogMacroOptions(Catalog, debug);
}

} // namespace JX3DPS::runtime::mo_wen
