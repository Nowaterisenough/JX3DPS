/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-03 16:31:46
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 15:14:15
 * @FilePath    : \JX3DPS\modules\Common\ConstVal.h
 */

#ifndef CONST_VAL_H
#define CONST_VAL_H

#pragma execution_character_set("utf-8")

#include <unordered_map>

#include "Version.h"

namespace JX3DPS {

#define JX3DPS_MAJOR_VERSION 110

/*------------------ 优化 - 条件跳转 ------------------*/

/* 帧数变量无效值 */
#define INVALID_FRAMES_SET (JX3DPS::JX3DPS_MAX_FRAMES)

/* 判定伤害 */
#if JX3DPS_MAJOR_VERSION >= 110
#    define GET_FINAL_DAMAGE(NormalDamage, Table, CriticalStrikePowerPercent) \
        (static_cast<long long>((NormalDamage) * (1 + (Table)) * ((CriticalStrikePowerPercent)-1.0)))
#else
#    define GET_FINAL_DAMAGE(NormalDamage, Table, CriticalStrikePowerPercent) \
        (static_cast<long long>(((NormalDamage) >> ((Table) / 2)) *           \
                                (1 + ((Table)&1) * ((CriticalStrikePowerPercent)-1.0))))
#endif // JX3DPS_MAJOR_VERSION >= 110

/* 修正判定百分比 - */
#define BETWEEN(min, num, max) ((min) > (num) ? (min) : ((max) < (num) ? (max) : (num)))

/*--------------------- 类型定义 ---------------------*/

/* 技能、Buff编号 */
using Id_t = unsigned int;

/* 逻辑帧数 */
using Frame_t = int;

/* 1024百分数 */
using BinPCT_t = int;

/* 百分数 */
using PCT_t = float;

/* 数值 */
using Value_t = int;

/*----------------------- JX3DPS常量 -----------------------*/

#if JX3DPS_MAJOR_VERSION == 110
constexpr int    JX3DPS_LEVEL_PARAM                 = 205;   // 等级系数
constexpr int    JX3DPS_LEVEL_CONST                 = 18800; // 等级常数
constexpr double JX3DPS_CRITICAL_STRIKE_PARAM       = 9.530;
constexpr double JX3DPS_CRITICAL_STRIKE_POWER_PARAM = 3.335;
constexpr double JX3DPS_HIT_VALUE_PARAM             = 6.931;
constexpr double JX3DPS_INSIGHT_PARAM               = 9.189;
constexpr double JX3DPS_OVERCOME_PARAM              = 9.530;
constexpr double JX3DPS_HASTE_RATE                  = 11.695;
constexpr double JX3DPS_PHYSICS_SHIELD_PARAM        = 5.091;
constexpr double JX3DPS_MAGIC_SHIELD_PARAM          = 5.091;
constexpr double JX3DPS_SURPLUS_PARAM               = 2.557;
#elif JX3DPS_MAJOR_VERSION == 100
constexpr int    JX3DPS_LEVEL_PARAM                 = 185;
constexpr int    JX3DPS_LEVEL_CONST                 = 16800;
constexpr double JX3DPS_CRITICAL_STRIKE_PARAM       = 9.026;
constexpr double JX3DPS_CRITICAL_STRIKE_POWER_PARAM = 3.159;
constexpr double JX3DPS_HIT_VALUE_PARAM             = 8.205;
constexpr double JX3DPS_INSIGHT_PARAM               = 5.128;
constexpr double JX3DPS_OVERCOME_PARAM              = 9.026;
constexpr double JX3DPS_HASTE_RATE                  = 11.077;
constexpr double JX3DPS_PHYSICS_SHIELD_PARAM        = 4.084;
constexpr double JX3DPS_MAGIC_SHIELD_PARAM          = 4.084;
#endif // JX3DPS_MAJOR_VERSION

constexpr double JX3DPS_CRITICAL_STRIKE_PER_AGILITY            = 0.64; // 主属性转换系数
constexpr double JX3DPS_ATTACK_BASE_PER_STRENGTH               = 0.15;
constexpr double JX3DPS_OVERCOME_BASE_PER_STRENGTH             = 0.3;
constexpr double JX3DPS_CRITICAL_STRIKE_PER_SPIRIT             = 0.64;
constexpr double JX3DPS_ATTACK_BASE_PER_SPUNK                  = 0.18;
constexpr double JX3DPS_OVERCOME_BASE_PER_SPUNK                = 0.3;
constexpr double JX3DPS_ATTACK_PER_AGILITY_FROM_TAI_XU_JIAN_YI = 1.45;
constexpr double JX3DPS_CRITICAL_STRIKE_PER_AGILITY_FROM_TAI_XU_JIAN_YI = 0.58;

constexpr PCT_t JX3DPS_PLAYER_HIT_VALUE_PERCENT_BASE             = 0.9;  // 角色基础命中
constexpr PCT_t JX3DPS_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE = 1.75; // 角色基础会心效果

constexpr BinPCT_t JX3DPS_BIN_PCT           = 1024; // 百分数基数100%
constexpr PCT_t    JX3DPS_PCT               = 1.0;  // 百分数基数100%
constexpr Frame_t  JX3DPS_FRAMES_PER_SECOND = 16;   // 每秒帧数
constexpr Frame_t  JX3DPS_PUBLIC_COOLDOWN   = 24;   // 公共冷却

Value_t constexpr JX3DPS_ENCHANTS_VALUE_PRIMARY[9] = {0, 1, 3, 5, 7, 9, 10, 14, 18}; // 熔嵌 - 主属性
Value_t constexpr JX3DPS_ENCHANTS_VALUE_ATTACK[9] = {0, 2, 5, 7, 10, 12, 15, 20, 26}; // 熔嵌 - 攻击
Value_t constexpr JX3DPS_ENCHANTS_VALUE_SUB[9] = {0, 5, 11, 17, 22, 28, 34, 45, 58}; // 熔嵌 - 副属性
double constexpr JX3DPS_STRENGTHEN_LEVEL_PARAM[9] =
    {0.0, 0.005, 0.013, 0.024, 0.038, 0.055, 0.075, 0.098, 0.124}; // 强化 - 系数
constexpr double JX3DPS_STONE_PRIMARY       = 5.175;               // 五彩石 - 主属性
constexpr double JX3DPS_STONE_ATTACK        = 10.35;               // 五彩石 - 攻击
constexpr double JX3DPS_STONE_SUB           = 23.125;              // 五彩石 - 副属性
constexpr double JX3DPS_STONE_WEAPON_ATTACK = 15.525;              // 五彩石 - 武器伤害

constexpr Frame_t JX3DPS_MAX_FRAMES       = INT_MAX; // 帧数最大值
constexpr int     JX3DPS_PUBLIC_SKILL     = 0;       // 公共冷却技能: true
constexpr int     JX3DPS_NOT_PUBLIC_SKILL = INT_MAX; // 公共冷却技能: false
constexpr int     JX3DPS_PLAYER_LEVEL     = JX3DPS_MAJOR_VERSION;
constexpr int     JX3DPS_DELAY_CONST      = 35; // 延迟常数: PING / JX3DPS_DELAY_CONST

/*----------------------- 定义 -----------------------*/

#if _MSVC_LANG >= 201703L || __cplusplus >= 201703L
union Param
{
    int   paramInt = 0;
    float paramFloat;

    Param(int i)
    {
        paramInt = i;
    }

    Param(Id_t i)
    {
        paramInt = i;
    }

    Param(float i)
    {
        paramFloat = i;
    }

    Param(double i)
    {
        paramFloat = i;
    }
};
using Params = std::vector<Param>;

#else
#    include <variant>

using Params = std::vector<std::variant>;
#endif // _MSVC_LANG >= 201703L || __cplusplus >= 201703L

class Macro;
using ConditionFunc = bool (Macro::*)(const Params &);

struct Condition
{
    ConditionFunc func;
    Params        params;

    Condition() {}
    Condition(ConditionFunc func, const Params &params) : func(func), params(params) {}
};

using Macros = std::list<std::pair<std::list<std::list<Condition>>, Id_t>>;

enum CastType {
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
class Player;
class Target;
using Targets = std::unordered_map<TargetStatus, std::list<Target *>>;

class Skill;
class Buff;
using Skills  = std::unordered_map<Id_t, Skill *>;
using Buffs   = std::unordered_map<Id_t, Buff *>;
using Talents = std::unordered_map<Id_t, bool>;
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

/* 收益信息 */
struct BonusSet
{
    bool    set;
    Value_t value;

    BonusSet() : set(false), value(0) {}
};

/* 属性收益表 */
using BonusSets = std::unordered_map<AttrType, BonusSet *>;

/* 模拟模式 */
using SimMode = AttrType;

struct Settings
{
    JX3Class               classType; // 心法
    SimMode                simMode;   // 模拟模式
    int                    simTimes;  // 模拟次数
    int                    minPing;   // 延迟最小
    int                    maxPing;   // 延迟最大
    Frame_t                time;      // 时间
    Attr                  *attr;      // 属性
    Attr                  *attrFinal;
    BonusSets              bonusSets;
    Talents                talents;
    Secrets                secrets;
    Buffs                  buffSets;
    std::list<std::string> macroStrList;
    std::list<std::string> eventStrList;

    Settings(JX3Class classType);
};

extern Settings g_settings;

/* 圆桌判定结果 */
enum Table
{
    ALL    = 3, // 全部
    HIT    = 0, // 命中
    DOUBLE = 1, // 会心
#if JX3DPS_MAJOR_VERSION < 110
    SIGHT = 4,  // 识破
    MISS  = 62, // 偏离
#endif          // JX3DPS_MAJOR_VERSION < 110
};

/* 圆桌判定类型 */
enum RollType
{
    COMMON, // 通用判定
    DOT,    // DOT判定
    SUB,    // 附加多段判定
};

/* 心法(偶数外功, 奇数内功) */
enum JX3Class
{
    TAI_XU_JIAN_YI = 2,
    ZI_XIA_GONG    = 3,
};

/* 伤害系数 */
struct DamageParam
{
    int      fixedDamage;            // 固定伤害
    BinPCT_t weaponDamageBinPercent; // 武器伤害系数, 1024为100%
    PCT_t    attackDamagePercent;    // 攻击系数

    DamageParam() : fixedDamage(0), weaponDamageBinPercent(0), attackDamagePercent(0.0) {}

    DamageParam(int fixedDamage, BinPCT_t weaponDamageBinPercent, PCT_t attackDamagePercent) :
        fixedDamage(fixedDamage), weaponDamageBinPercent(weaponDamageBinPercent),
        attackDamagePercent(attackDamagePercent)
    {
    }
};

using DamageParams = std::unordered_map<std::string, std::vector<DamageParam>>;

/*----------------------- 统计 -----------------------*/

/* 伤害统计 */
struct Damage
{
    long long fixedDamage;  // 固定伤害统计
    long long weaponDamage; // 武器伤害统计
    long long attackDamage; // 攻击伤害统计

    Damage() : fixedDamage(0), weaponDamage(0), attackDamage(0) {}

    Damage(long long fixedDamage, long long weaponDamage, long long attackDamage) :
        fixedDamage(fixedDamage), weaponDamage(weaponDamage), attackDamage(attackDamage)
    {
    }

    Damage(const Damage &damage) :
        fixedDamage(damage.fixedDamage), weaponDamage(damage.weaponDamage),
        attackDamage(damage.attackDamage)
    {
    }

    Damage &operator=(const Damage &damage)
    {
        this->fixedDamage  = damage.fixedDamage;
        this->weaponDamage = damage.weaponDamage;
        this->attackDamage = damage.attackDamage;
        return *this;
    }

    Damage &operator+=(const Damage &damage)
    {
        this->fixedDamage += damage.fixedDamage;
        this->weaponDamage += damage.weaponDamage;
        this->attackDamage += damage.attackDamage;
        return *this;
    }

    long long Sum() const
    {
        return fixedDamage + weaponDamage + attackDamage;
    }
};

/* [判定] - <数目, 伤害> */
using TableStats = std::unordered_map<Table, std::pair<int, Damage>>;

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
    BonusStats bonusStats;

    Stats(Player &player, Targets targets);

    TableStats InitTableStats();
    EffectStats InitEffectStats(Player &player, Targets targets);
    void AddDamageStats(Id_t id, Player &player, Targets targets);
};

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

/* Buff - 太虚剑意 */
constexpr Id_t BUF_ZI_QI_DONG_LAI      = 2101;
constexpr Id_t BUF_XUAN_MEN            = 2102;
constexpr Id_t BUF_SUI_XING_CHEN_STD   = 2103;
constexpr Id_t BUF_SHENG_TAI_JI_STD    = 2104;
constexpr Id_t BUF_TUN_RI_YUE_STD      = 2105;
constexpr Id_t BUF_YUN_ZHONG_JIAN_C    = 2106;
constexpr Id_t BUF_YUN_ZHONG_JIAN_J    = 2107;
constexpr Id_t BUF_YUN_ZHONG_JIAN_Y    = 2108;
constexpr Id_t BUF_CHI_YING            = 2109;
constexpr Id_t BUF_QI_SHENG            = 2110;
constexpr Id_t BUF_FENG_SHI            = 2111;
constexpr Id_t BUF_SUI_XING_CHEN_BUFF  = 2112;
constexpr Id_t BUF_DIE_REN             = 2113;
constexpr Id_t BUF_REN_JIAN_HE_YI_BUFF = 2114;

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
