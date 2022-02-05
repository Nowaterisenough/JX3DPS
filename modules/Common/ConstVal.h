/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-03 16:31:46
 * @Update      : NoWats
 * @LastTime    : 2022-02-05 01:51:33
 * @FilePath    : \JX3DPS\modules\Common\ConstVal.h
 */

#ifndef CONST_VAL_H
#define CONST_VAL_H

#pragma execution_character_set("utf-8")

#include <thread>
#include <unordered_map>

#include "Version.h"

namespace JX3DPS {

#define JX3_MAJOR_VERSION 110

/*------------------ 优化 - 条件跳转 ------------------*/

/* 帧数变量无效值 */
#define INVALID_FRAMES_SET (JX3DPS::CONST_MAX_FRAMES)

/* 判定伤害 */
#if JX3_MAJOR_VERSION >= 110
#    define GET_FINAL_DAMAGE(NormalDamage, TableRes, CriticalStrikePowerPercent) \
        (static_cast<long long>((NormalDamage) * (1 + (TableRes)) *              \
                                ((CriticalStrikePowerPercent)-1.0)))
#else
#    define GET_FINAL_DAMAGE(NormalDamage, TableRes, CriticalStrikePowerPercent) \
        (static_cast<long long>(((NormalDamage) >> ((TableRes) / 2)) *           \
                                (1 + ((TableRes)&1) * ((CriticalStrikePowerPercent)-1.0))))
#endif // JX3_MAJOR_VERSION >= 110

/* 修正判定百分比 - */
#define BETWEEN(min, num, max) ((min) > (num) ? (min) : ((max) < (num) ? (max) : (num)))

/*--------------------- 类型定义 ---------------------*/

/* 技能、Buff编号 */
using Id_t = unsigned int;

/* 逻辑帧数 */
using Frame_t = int;

/* 1024百分数 */
using BinPct_t = int;

/* 百分数 */
using Pct_t = float;

/* 数值 */
using Value_t = int;

/*----------------------- 常量 -----------------------*/

/* 最大线程数 */
const int MAX_THREAD = std::thread::hardware_concurrency();

/* 帧数最大值 */
constexpr int CONST_MAX_FRAMES = INT_MAX;

/* 技能公共冷却 */
constexpr int PUBLIC_SKILL     = 0;
constexpr int NOT_PUBLIC_SKILL = INT_MAX;

/*----------------------- 游戏常数 -----------------------*/

/* 百分数基数100% */
constexpr BinPct_t CONST_BIN_PCT = 1024;
constexpr Pct_t    CONST_PCT     = 1.0;

/* 角色等级 */
constexpr int CONST_PLAYER_LEVEL = JX3_MAJOR_VERSION;

#if JX3_MAJOR_VERSION == 110
/* 等级系数 */
constexpr int CONST_LEVEL_PARAM = 205;

/* 等级常数 */
constexpr int CONST_LEVEL = 18800;

/* 全局系数 */
constexpr double CONST_CRITICAL_STRIKE_PARAM       = 9.530;
constexpr double CONST_CRITICAL_STRIKE_POWER_PARAM = 3.335;
constexpr double CONST_HIT_VALUE_PARAM             = 6.931;
constexpr double CONST_INSIGHT_PARAM               = 9.189;
constexpr double CONST_OVERCOME_PARAM              = 9.530;
constexpr double CONST_HASTE_RATE                  = 11.695;
constexpr double CONST_PHYSICS_SHIELD_PARAM        = 5.091;
constexpr double CONST_MAGIC_SHIELD_PARAM          = 5.091;
constexpr double CONST_SURPLUS_PARAM               = 2.557;
#elif JX3_MAJOR_VERSION == 100
/* 等级系数 */
constexpr int CONST_LEVEL_PARAM = 185;

/* 等级常数 */
constexpr int CONST_LEVEL = 16800;

/* 全局系数 */
constexpr double CONST_CRITICAL_STRIKE_PARAM       = 9.026;
constexpr double CONST_CRITICAL_STRIKE_POWER_PARAM = 3.159;
constexpr double CONST_HIT_VALUE_PARAM             = 8.205;
constexpr double CONST_INSIGHT_PARAM               = 5.128;
constexpr double CONST_OVERCOME_PARAM              = 9.026;
constexpr double CONST_HASTE_RATE                  = 11.077;
constexpr double CONST_PHYSICS_SHIELD_PARAM        = 4.084;
constexpr double CONST_MAGIC_SHIELD_PARAM          = 4.084;
#endif // JX3_MAJOR_VERSION

/* 主属性转换系数 */
constexpr double CONST_AGILITY_TO_CRITICAL_STRIKE = 0.64;
constexpr double CONST_STRENGTH_TO_ATTACK_BASE    = 0.15;
constexpr double CONST_STRENGTH_TO_OVERCOME_BASE  = 0.3;
constexpr double CONST_SPIRIT_TO_CRITICAL_STRIKE  = 0.64;
constexpr double CONST_SPUNK_TO_ATTACK_BASE       = 0.18;
constexpr double CONST_SPUNK_TO_OVERCOME_BASE     = 0.3;

constexpr double CONST_AGILITY_TO_ATTACK_TAI_XU_JIAN_YI          = 1.45;
constexpr double CONST_AGILITY_TO_CRITICAL_STRIKE_TAI_XU_JIAN_YI = 0.58;

/* 每秒帧数 */
constexpr Frame_t FRAMES_PER_SECOND = 16;

/* 公共冷却 */
constexpr Frame_t CONST_COMMON_PUBLIC_COOLDOWN = 24;

/* 延迟常数: (PING / DELAY_CONST) 即 延迟帧数 */
constexpr int CONST_DELAY = 35;

/* 角色基础命中 */
constexpr Pct_t CONST_PLAYER_HIT_VALUE_PERCENT_BASE = 0.9;

/* 角色基础会心效果 */
constexpr Pct_t CONST_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE = 1.75;

/* 熔嵌 - 主属性 */
const Value_t constexpr CONST_ENCHANTS_VALUE_PRIMARY[9] = {0, 1, 3, 5, 7, 9, 10, 14, 18};

/* 熔嵌 - 攻击 */
const Value_t constexpr CONST_ENCHANTS_VALUE_ATTACK[9] = {0, 2, 5, 7, 10, 12, 15, 20, 26};

/* 熔嵌 - 副属性 */
const Value_t constexpr CONST_ENCHANTS_VALUE_SUB[9] = {0, 5, 11, 17, 22, 28, 34, 45, 58};

/* 强化 - 系数 */
const double constexpr CONST_STRENGTHEN_LEVEL[9] =
    {0.0, 0.005, 0.013, 0.024, 0.038, 0.055, 0.075, 0.098, 0.124};

/* 五彩石 - 主属性 */
constexpr double CONST_STONE_PRIMARY = 5.175;

/* 五彩石 - 攻击 */
constexpr double CONST_STONE_ATTACK = 10.35;

/* 五彩石 - 副属性 */
constexpr double CONST_STONE_SUB = 23.125;

/* 五彩石 - 武器伤害 */
constexpr double CONST_STONE_WEAPON_ATTACK = 15.525;

/*----------------------- ID -----------------------*/

/* 通用 - Buff */
constexpr Id_t BUF_CLASS_EFFECT     = 101;
constexpr Id_t BUF_CLASS_TEAM_POINT = 102;
constexpr Id_t BUF_CLASS_CW         = 103;
constexpr Id_t BUF_CLASS_ATTACK     = 104;

/* 通用 - 3rd buff */
constexpr Id_t BUF_3RD_SUI_XING_CHEN    = 201;
constexpr Id_t BUF_3RD_MEI_HUA_SAN_NONG = 202;

/* 通用 - 3rd debuff */
constexpr Id_t TBUF_MIE_SHI = 501;

/*----------------------- 定义 -----------------------*/

/* 圆桌判定结果 */
enum TableRes
{
    ALL    = 3, // 全部
    HIT    = 0, // 命中
    DOUBLE = 1, // 会心
#if JX3_MAJOR_VERSION >= 110
#else
    SIGHT = 4,  // 识破
    MISS  = 62, // 偏离
#endif // JX3_MAJOR_VERSION >= 110
};

/* 圆桌判定类型 */
enum RollType
{
    COMMON, // 通用判定
    DOT,    // DOT判定
    SUB,    // 附加多段判定
};

/* 心法(奇数外功, 偶数内功) */
enum Class
{
    NONE_CLASS         = 0,
    TAI_XU_JIAN_YI     = 2,
    ZI_XIA_GONG        = 3,
    WEN_SHUI_JUE       = 4,
    AO_XUE_ZHAN_YI     = 6,
    FEN_SHAN_JING      = 8,
    XIAO_CHEN_JUE      = 10,
    LING_HAI_JUE       = 12,
    JING_YU_JUE        = 14,
    BEI_AO_JUE         = 16,
    YIN_LONG_JUE       = 18,
    BING_XIN_JUE       = 1,
    MO_WEN             = 5,
    YI_JIN_JING        = 7,
    FEN_YING_SHENG_JUE = 9,

    XIANG_ZHI = 11,
};

/* 奇穴表 */
using Talents = std::unordered_map<Id_t, bool>;

/* 秘籍表 */
using Secrets = std::unordered_map<Id_t, std::list<bool>>;

/* 属性类型 */
enum AttrType
{
    DEFALUT_TYPE,
    PRIMARY,               // 主属性
    ATTACK_BASE,           // 基础攻击
    CRITICAL_STRIKE,       // 会心
    CRITICAL_STRIKE_POWER, // 会心效果
    OVERCOME_BASE,         // 基础破防
    HASTE,                 // 加速
    STRAIN,                // 无双
    SURPLUS,               // 破招
    WEAPON_ATTACK,         // 武器伤害
};

/* 模拟模式 */
using SIM_MODE = AttrType;

/* 阵眼 */
using TEAM_CORE = AttrType;

/* 收益信息 */
struct BonusSet
{
    bool    set;
    Value_t value;

    BonusSet() : set(false), value(0) {}
};

/* 属性收益表 */
using BonusSets = std::unordered_map<AttrType, BonusSet *>;

/* 武器类型 */
enum WeaponType
{
    CW,
    LITTLE_CW,
    THUNDER_WEAPON,
    WATER_WEAPON,
    EXCL_WEAPON,
    NORMAL_WEAPON,
};

/* 套装效果类型 */
enum GearSetType
{
    GEAR_SET_ATTACK,
    GEAR_SET_CLASS,
    WAIST_EFFECT,
};

/* 套装效果表 */
using GearSets = std::unordered_map<GearSetType, bool>;

/* 装备类型 */
enum EquipType
{
    DEFAULT_EQUIP_TYPE, // 默认
    HELMS,              // 帽子
    ARMOR,              // 上衣
    WRISTS,             // 护腕
    BELTS,              // 腰带
    BOOTS,              // 鞋子
    LEGS,               // 下装
    AMULETS,            // 项链
    WAIST,              // 腰坠
    RINGS,              // 戒指
    SUB_WEAPON,         // 副武器
    PRIMARY_WEAPON,     // 武器
};

/* 附魔表 */
using Enchants = std::unordered_map<EquipType, bool>;

/* 技能列表 */
class Skill;
class Player;
using Skills = std::unordered_map<Id_t, Skill *>;

enum CastType
{
    SKILL,
    EFFECT,
    FORCE,
};

/* 目标状态 */
enum TargetStatus
{
    NORMAL,
    DETACHED,
    DEAD,
};

/* Buff列表 */
class Buff;
class Target;
using Targets    = std::list<Target *>;
using TargetsMap = std::unordered_map<TargetStatus, Targets>;
using Buffs      = std::unordered_map<Id_t, Buff *>;

/* 设置 */
class Attr;
struct Settings
{
    Class                  classType; // 心法
    SIM_MODE               simMode;   // 模拟模式
    int                    simTimes;  // 模拟次数
    int                    minPing;   // 延迟最小
    int                    maxPing;   // 延迟最大
    Frame_t                time;      // 时间
    Attr                  *attr;      // 属性
    Attr                  *attrFinal;
    BonusSets              bonusSets;
    WeaponType             weaponType;
    GearSets               gearSets;
    Enchants               enchants;
    Talents                talents;
    Secrets                secrets;
    TEAM_CORE              teamCore;
    Buffs                  buffSets;
    std::list<std::string> macroStrList;
    std::list<std::string> eventStrList;

    Settings(Class classType);
};

extern Settings g_settings;

/* 伤害系数 */
struct DamageParam
{
    int      fixedDamage;            // 固定伤害
    BinPct_t weaponDamageBinPercent; // 武器伤害系数, 1024为100%
    Pct_t    attackDamagePercent;    // 攻击系数

    DamageParam() : fixedDamage(0), weaponDamageBinPercent(0), attackDamagePercent(0.0) {}

    DamageParam(int fixedDamage, BinPct_t weaponDamageBinPercent, Pct_t attackDamagePercent) :
        fixedDamage(fixedDamage), weaponDamageBinPercent(weaponDamageBinPercent),
        attackDamagePercent(attackDamagePercent)
    {
    }
};

using DamageParams = std::unordered_map<std::string, std::vector<DamageParam>>;

/*--------------------- 函数指针 ---------------------*/

/* 函数指针参数 */
struct Param
{
    int    int1st;
    int    int2nd;
    int    int3rd;
    double double4th;
    double double5th;

    Param() {}

    Param(int int1st, int int2nd, int int3rd, double double4th, double double5th) :
        int1st(int1st), int2nd(int2nd), int3rd(int3rd), double4th(double4th), double5th(double5th)
    {
    }
};

class Event;

using EventFuncPtr = void (Event::*)(const Param &param);

struct EventFunc
{
    EventFuncPtr eventFuncPtr;
    Param        param;

    EventFunc(EventFuncPtr eventFuncPtr, const Param &param) :
        eventFuncPtr(eventFuncPtr), param(param)
    {
    }
};

using Events = std::list<std::pair<Frame_t, EventFunc>>;

class Macro;

using MacroFuncPtr = bool (Macro::*)(const Param &param);

struct MacroFunc
{
    MacroFuncPtr macroFuncPtr;
    Param        param;

    MacroFunc() {}

    MacroFunc(MacroFuncPtr macroFuncPtr) : macroFuncPtr(macroFuncPtr) {}

    MacroFunc(MacroFuncPtr macroFuncPtr, const Param &param) :
        macroFuncPtr(macroFuncPtr), param(param)
    {
    }
};

using Macros = std::list<std::pair<std::list<std::list<MacroFunc>>, Id_t>>;

using ForceMacros = std::list<std::pair<Frame_t, std::pair<std::list<std::list<MacroFunc>>, Id_t>>>;

/*----------------------- 统计 -----------------------*/

/* 伤害统计 */
struct Damage
{
    long long fixedDamage;  // 固定伤害统计
    long long weaponDamage; // 武器伤害统计
    long long attackDamage; // 攻击伤害统计

    Damage() : fixedDamage(0), weaponDamage(0), attackDamage(0) {}

    Damage(const Damage &damage) :
        fixedDamage(damage.fixedDamage), weaponDamage(damage.weaponDamage),
        attackDamage(damage.attackDamage)
    {
    }

    Damage(const long long fixedDamage, const long long weaponDamage, const long long attackDamage) :
        fixedDamage(fixedDamage), weaponDamage(weaponDamage), attackDamage(attackDamage)
    {
    }

    long long SumDamage() const
    {
        // Damage(1, 2, 3);
        return fixedDamage + weaponDamage + attackDamage;
    }

    Damage &operator+=(const Damage &damage)
    {
        this->fixedDamage += damage.fixedDamage;
        this->weaponDamage += damage.weaponDamage;
        this->attackDamage += damage.attackDamage;
        return *this;
    }
};

/* [判定] - <数目, 伤害> */
using TableStats = std::unordered_map<TableRes, std::pair<int, Damage>>;

/* [强度] - <[判定] - <数目, 伤害>> */
using LevelStats = std::unordered_map<int, TableStats>;

/* [词缀] - <[强度] - <[判定] - <数目, 伤害>>> */
using SubStats = std::unordered_map<std::string, LevelStats>;

/* [效果] - <[词缀] - <[强度] - <[判定] - <数目, 伤害>>>> */
using EffectStats = std::unordered_map<Id_t, SubStats>;

/* [目标] - <[效果] - <[词缀] - <[强度] - <[判定] - <数目, 伤害>>>>> */
using DamageStats = std::unordered_map<Id_t, EffectStats>;

/* 收益统计 */
using BonusStats = std::unordered_map<AttrType, long long>;

/* 统计 */
struct Stats
{
    DamageStats damageStats;
    BonusStats  bonusStats;

    Stats(Player &player, TargetsMap targetsMap);

    TableStats  InitTableStats();
    EffectStats InitEffectStats(Player &player, TargetsMap targetsMap);
    void        AddDamageStats(Id_t id, Player &player, TargetsMap targetsMap);
};

/* 特征值 */
struct CharStats
{
    int minDamage;
    int maxDamage;

    CharStats() : minDamage(CONST_MAX_FRAMES), maxDamage(0) {}
};

/* 统计汇总 */
struct TotalStats
{
    DamageStats damageStats;
    BonusStats  bonusStats;
    CharStats   charStats;

    TotalStats();

    TotalStats &operator+=(Stats &stats);
};

/* Buff - 太虚剑意 */
constexpr Id_t BUF_ZI_QI_DONG_LAI     = 2101;
constexpr Id_t BUF_XUAN_MEN           = 2102;
constexpr Id_t BUF_SUI_XING_CHEN      = 2103;
constexpr Id_t BUF_SHENG_TAI_JI       = 2104;
constexpr Id_t BUF_TUN_RI_YUE         = 2105;
constexpr Id_t BUF_YUN_ZHONG_JIAN_C   = 2106;
constexpr Id_t BUF_YUN_ZHONG_JIAN_J   = 2107;
constexpr Id_t BUF_YUN_ZHONG_JIAN_Y   = 2108;
constexpr Id_t BUF_CHI_YING           = 2109;
constexpr Id_t BUF_QI_SHENG           = 2110;
constexpr Id_t BUF_FENG_SHI           = 2111;
constexpr Id_t BUF_SUI_XING_CHEN_BUFF = 2112;
constexpr Id_t BUF_DIE_REN            = 2113;
constexpr Id_t BUF_REN_JIAN_HE_YI     = 2114;

/* 技能 - 太虚剑意 */
constexpr Id_t SKI_WU_WO_WU_JIAN     = 2201;
constexpr Id_t SKI_BA_HUANG_GUI_YUAN = 2202;
constexpr Id_t SKI_SAN_HUAN_TAO_YUE  = 2203;
constexpr Id_t SKI_REN_JIAN_HE_YI    = 2204;
constexpr Id_t SKI_SUI_XING_CHEN     = 2205;
constexpr Id_t SKI_SHENG_TAI_JI      = 2206;
constexpr Id_t SKI_TUN_RI_YUE        = 2207;
constexpr Id_t SKI_ZI_QI_DONG_LAI    = 2208;
constexpr Id_t SKI_SAN_CHAI_JIAN_FA  = 2209;

/* 奇穴 - 太虚剑意 */
constexpr Id_t TAL_CUO_RUI        = 2301;
constexpr Id_t TAL_XIN_GU         = 2302;
constexpr Id_t TAL_TONG_GEN       = 2303;
constexpr Id_t TAL_SHEN_MAI       = 2304;
constexpr Id_t TAL_KUN_WU         = 2305;
constexpr Id_t TAL_BAI_HONG       = 2306;
constexpr Id_t TAL_HUA_SAN_QING   = 2307;
constexpr Id_t TAL_YUN_ZHONG_JIAN = 2308;
constexpr Id_t TAL_WU_YI          = 2309;
constexpr Id_t TAL_FENG_SHI       = 2310;
constexpr Id_t TAL_KUANG_GE       = 2311;
constexpr Id_t TAL_DIE_REN        = 2312;
constexpr Id_t TAL_QIE_YU         = 2313;
constexpr Id_t TAL_CHANG_SHENG    = 2314;
constexpr Id_t TAL_FU_YIN         = 2315;
constexpr Id_t TAL_HE_GUANG       = 2316;
constexpr Id_t TAL_SUI_WU         = 2317;
constexpr Id_t TAL_QI_SHENG       = 2318;
constexpr Id_t TAL_WU_YU          = 2319;
constexpr Id_t TAL_XUAN_MEN       = 2320;

} // namespace JX3DPS

#endif // CONST_VAL_H
