#pragma once

#include "src/core/runtime/specialization.hpp"
#include "team_buffs.hpp"

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

inline constexpr SkillCatalog Catalog{Skills.entries, Buffs.entries, CastableSkills, SkillAliases, BuffAliases};

inline CompileOptions MacroOptions(bool debug = true) {
    return CatalogMacroOptions(Catalog, debug);
}

} // namespace JX3DPS::runtime::tai_xu
