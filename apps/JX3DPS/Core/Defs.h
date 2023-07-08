/**
 * Project: JX3DPS
 * File: Defs.h
 * Description:
 * Created Date: 2023-06-19 20:59:11
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-07 23:46:53
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef JX3DPS_DEFS_H_
#define JX3DPS_DEFS_H_

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>

namespace JX3DPS {

using PctInt_t = int;

using PctFloat_t = float;

using Value_t = int;

using Id_t = int;

using Frame_t = int;

using Error_t = int;

class Player;
class Target;
class Skill;
class Buff;

using Targets = std::unordered_map<Id_t, std::unique_ptr<Target>>;
using Skills  = std::unordered_map<Id_t, std::unique_ptr<Skill>>;
using Buffs   = std::unordered_map<Id_t, std::unique_ptr<Buff>>;

/* 奇穴 */
using Talents = std::unordered_map<Id_t, bool>;

/* 秘籍 */
using Secrets = std::unordered_map<Id_t, std::vector<bool>>;

/* 圆桌判定结果 */
enum class RollResult
{
    HIT    = 0,  // 命中
    DOUBLE = 1,  // 会心
    SIGHT  = 4,  // 识破
    MISS   = 62, // 偏离
};

/* 伤害统计 */
struct Damage
{
    long long fixedDamage;      // 固定伤害统计
    long long weaponDamage;     // 武器伤害统计
    long long attackBaseDamage; // 基础攻击伤害统计
    long long attackMainDamage; // 面板攻击伤害统计
    long long surplusDamage;    // 破招伤害统计

    Damage() :
        fixedDamage(0), weaponDamage(0), attackBaseDamage(0), attackMainDamage(0), surplusDamage(0)
    {
    }

    Damage(const Damage &damage) :
        fixedDamage(damage.fixedDamage),
        weaponDamage(damage.weaponDamage),
        attackBaseDamage(damage.attackBaseDamage),
        attackMainDamage(damage.attackMainDamage),
        surplusDamage(damage.surplusDamage)
    {
    }

    Damage(long long fixedDamage, long long weaponDamage, long long attackBaseDamage, long long attackMainDamage, long long surplusDamage) :
        fixedDamage(fixedDamage),
        weaponDamage(weaponDamage),
        attackBaseDamage(attackBaseDamage),
        attackMainDamage(attackMainDamage),
        surplusDamage(surplusDamage)
    {
    }

    long long SumDamage() const
    {
        return fixedDamage + weaponDamage + attackBaseDamage + attackMainDamage + surplusDamage;
    }

    Damage &operator+=(const Damage &damage)
    {
        this->fixedDamage      += damage.fixedDamage;
        this->weaponDamage     += damage.weaponDamage;
        this->attackBaseDamage += damage.attackBaseDamage;
        this->attackMainDamage += damage.attackMainDamage;
        this->surplusDamage    += damage.surplusDamage;
        return *this;
    }
};

/* 伤害系数 */
struct DamageParam
{
    int        fixedDamage;            // 固定伤害
    PctInt_t   weaponDamagePercentInt; // 武器伤害系数, 1024为100%
    PctFloat_t attackDamagePercent;    // 攻击系数
    PctFloat_t surplusDamagePercent;   // 破招系数

    DamageParam() :
        fixedDamage(0), weaponDamagePercentInt(0), attackDamagePercent(0.0), surplusDamagePercent(0.0)
    {
    }

    DamageParam(int fixedDamage, PctInt_t weaponDamagePercentInt, PctFloat_t attackDamagePercent, PctFloat_t surplusDamagePercent) :
        fixedDamage(fixedDamage),
        weaponDamagePercentInt(weaponDamagePercentInt),
        attackDamagePercent(attackDamagePercent),
        surplusDamagePercent(surplusDamagePercent)
    {
    }
};

/* 心法(奇数外功, 偶数内功) */
enum class Class
{
    NONE_CLASS     = 0,
    TAI_XU_JIAN_YI = 1,
};

/* [判定] - <数目, 伤害> */
using LevelStats = std::unordered_map<RollResult, std::pair<int, Damage>>;

/* [强度] - <[判定] - <数目, 伤害>> */
using SubStats = std::unordered_map<int, LevelStats>;

/* [词缀] - <[强度] - <[判定] - <数目, 伤害>>> */
using EffectStats = std::unordered_map<int, SubStats>;

/* [效果] - <[词缀] - <[强度] - <[判定] - <数目, 伤害>>>> */
using TargetStats = std::unordered_map<Id_t, EffectStats>;

/* [目标] - <[效果] - <[词缀] - <[强度] - <[判定] - <数目, 伤害>>>>> */
using DamageStats = std::unordered_map<Id_t, TargetStats>;

/* [序号] - <[目标] - <[效果] - <[词缀] - <[强度] - <[判定] - <数目, 伤害>>>>>>
 * 序号0表示总计
 */
// using Stats = std::list<DamageStats>;

inline DamageStats &operator+=(DamageStats &lhs, DamageStats &rhs)
{
    for (auto &target : rhs) {
        auto &targetId       = target.first;
        auto &targetStats    = target.second;
        auto &targetStatsLhs = lhs[targetId];
        for (auto &effect : targetStats) {
            auto &effectName     = effect.first;
            auto &effectStats    = effect.second;
            auto &effectStatsLhs = targetStatsLhs[effectName];
            for (auto &affix : effectStats) {
                auto &affixLevel    = affix.first;
                auto &affixStats    = affix.second;
                auto &affixStatsLhs = effectStatsLhs[affixLevel];
                for (auto &level : affixStats) {
                    auto &levelType     = level.first;
                    auto &levelStats    = level.second;
                    auto &levelStatsLhs = affixStatsLhs[levelType];
                    for (auto &stat : levelStats) {
                        auto &statType                  = stat.first;
                        levelStatsLhs[statType].first  += stat.second.first;
                        levelStatsLhs[statType].second += stat.second.second;
                    }
                }
            }
        }
    }
    return lhs;
}

struct GainsDamage
{
    Damage normalDamage;
    Damage criticalStrikePowerDamage;
    Damage overcomeDamage;
    Damage strainDamage;
    Damage surplusDamage;
};

struct Stats
{
    DamageStats damageStats;
    long long   normalDamage = 0;
    long long   criticalStrikePowerDamage = 0;
    long long   overcomeDamage = 0;
    long long   strainDamage = 0;
    long long   surplusDamage = 0;

    inline Stats &operator+=(Stats &other)
    {
        damageStats               += other.damageStats;
        normalDamage              += other.normalDamage;
        criticalStrikePowerDamage += other.criticalStrikePowerDamage;
        overcomeDamage            += other.overcomeDamage;
        strainDamage              += other.strainDamage;
        surplusDamage             += other.surplusDamage;
        return *this;
    }
};

/* 伤害参数 */
using DamageParams = std::unordered_map<int, std::vector<DamageParam>>;



enum class AttributeType
{
    NONE = 0,
    AGILITY,
    SPIRIT,
    STRENGTH,
    SPUNK,
    PHYSICS_ATTACK,
    PHYSICS_CRITICAL_STRIKE,
    PHYSICS_CRITICAL_STRIKE_POWER,
    PHYSICS_OVERCOME,
    MAGIC_ATTACK,
    MAGIC_CRITICAL_STRIKE,
    MAGIC_CRITICAL_STRIKE_POWER,
    MAGIC_OVERCOME,
    HASTE,
    STRAIN,
    SURPLUS,
    WEAPON_ATTACK,
};

inline const std::unordered_map<AttributeType, std::string> &ATTRIBUTE_NAME = {
    {AttributeType::NONE,                           "空"         },
    { AttributeType::AGILITY,                       "身法"      },
    { AttributeType::SPIRIT,                        "根骨"      },
    { AttributeType::STRENGTH,                      "力道"      },
    { AttributeType::SPUNK,                         "元气"      },
    { AttributeType::PHYSICS_ATTACK,                "外功攻击"},
    { AttributeType::PHYSICS_CRITICAL_STRIKE,       "外功会心"},
    { AttributeType::PHYSICS_CRITICAL_STRIKE_POWER, "外功会效"},
    { AttributeType::PHYSICS_OVERCOME,              "外功破防"},
    { AttributeType::MAGIC_ATTACK,                  "内功攻击"},
    { AttributeType::MAGIC_CRITICAL_STRIKE,         "内功会心"},
    { AttributeType::MAGIC_CRITICAL_STRIKE_POWER,   "内功会效"},
    { AttributeType::MAGIC_OVERCOME,                "内功破防"},
    { AttributeType::HASTE,                         "加速"      },
    { AttributeType::STRAIN,                        "无双"      },
    { AttributeType::SURPLUS,                       "破招"      },
    { AttributeType::WEAPON_ATTACK,                 "武器伤害"},
};

inline const std::vector<int> &ATTRIBUTE_GAIN_BY_BASE = {
    0, 179, 179, 179, 179, 360, 799, 799, 799, 389, 799, 799, 799, 799, 799, 799, 540,
};

using ExprIf     = std::function<bool(Player *player, Targets *targets)>;
using ExprIfs    = std::list<std::list<ExprIf>>;
using ExprSkill  = std::pair<ExprIfs, Id_t>;
using ExprSkills = std::list<ExprSkill>;
using ExprEvent  = std::pair<Frame_t, ExprIf>;
using ExprEvents = std::list<ExprEvent>;

} // namespace JX3DPS

#endif // JX3DPS_DEFS_H