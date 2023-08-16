/**
 * Project: JX3DPS
 * File: Defs.h
 * Description:
 * Created Date: 2023-07-22 08:40:04
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-16 18:05:42
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_DEFS_H__
#define __JX3DPS_DEFS_H__

#include <functional>
#include <list>
#include <unordered_map>

#include "Attribute/Attribute.hpp"
#include "Utils/Utils.hpp"

#include "Id.h"
#include "JX3.h"
#include "Types.h"

namespace JX3DPS {

/*----------------------- 常量定义 -----------------------*/

/* 延迟常数: (PING / JX3DPS_DELAY) 即 延迟帧数 */
const int JX3DPS_DELAY = 35;

/* 无限制释放距离 */
const double JX3DPS_UNLIMITED_RANGE = INT_MAX;

/* 无限制生命周期 */
const Frame_t JX3DPS_UNLIMITED_LIFE_TIME = INT_MAX;

/* 帧数变量无效值 */
const Frame_t JX3DPS_INVALID_FRAMES_SET = INT_MAX;

/* 默认持续时间 */
const Frame_t JX3DPS_DEFAULT_DURATION_FRAMES = -1;

/* 充能数无效值 */
const int JX3DPS_INVALID_ENERGY_SET = -1;

/*----------------------- 类型定义 -----------------------*/

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
    long long damage;        // 伤害统计
    long long surplusDamage; // 破招伤害统计

    Damage() : damage(0), surplusDamage(0) { }

    Damage(const Damage &damage) :
        damage(damage.damage), surplusDamage(damage.surplusDamage)
    {
    }

    Damage(long long damage, long long surplusDamage) :
        damage(damage), surplusDamage(surplusDamage)
    {
    }

    long long SumDamage() const { return damage + surplusDamage; }

    Damage &operator+=(const Damage &damage)
    {
        this->damage        += damage.damage;
        this->surplusDamage += damage.surplusDamage;
        return *this;
    }
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

using GainsDamage = std::unordered_map<Attribute::Type, Damage>;

struct Stats
{
    std::list<long long> damageList;

    std::unordered_map<Attribute::Type, DamageStats> gainStats;

    inline Stats &operator+=(Stats &rhs)
    {
        for (auto &[type, damageStats] : rhs.gainStats) {
            gainStats[type] += damageStats;
        }
        return *this;
    }
};

inline void StatsInit(ClassType classType, Stats &stats)
{
    // if (Attribute::MAJOR[static_cast<int>(classType)][static_cast<int>(Attribute::MajorType::STRENGTH)])
    // {
    //     stats.gainStats[Attribute::Type::STRENGTH_BASE] = DamageStats();
    // } else if (Attribute::MAJOR[static_cast<int>(classType)][static_cast<int>(Attribute::MajorType::SPUNK)])
    // {
    //     stats.gainStats[Attribute::Type::SPUNK_BASE] = DamageStats();
    // }
    stats.gainStats[Attribute::Type::DEFAULT]               = DamageStats();
    stats.gainStats[Attribute::Type::WEAPON_DAMAGE_BASE]    = DamageStats();
    stats.gainStats[Attribute::Type::ATTACK_POWER_BASE]     = DamageStats();
    stats.gainStats[Attribute::Type::CRITICAL_STRIKE_POWER] = DamageStats();
    stats.gainStats[Attribute::Type::OVERCOME_BASE]         = DamageStats();
    stats.gainStats[Attribute::Type::STRAIN_BASE]           = DamageStats();
    stats.gainStats[Attribute::Type::SURPLUS_VALUE_BASE]    = DamageStats();
}

/* 伤害系数 */
struct DamageParam
{
    int      fixedDamage;            // 固定伤害
    PctInt_t weaponDamagePercentInt; // 武器伤害系数, 1024为100%
    PctInt_t attackDamagePercentInt; // 攻击系数

    DamageParam() : fixedDamage(0), weaponDamagePercentInt(0), attackDamagePercentInt(0)
    {
    }

    DamageParam(int fixedDamage, PctInt_t weaponDamagePercentInt, PctInt_t attackDamagePercentInt) :
        fixedDamage(fixedDamage),
        weaponDamagePercentInt(weaponDamagePercentInt),
        attackDamagePercentInt(attackDamagePercentInt)
    {
    }
};

/* 伤害参数 */
using DamageParams = std::unordered_map<int, std::vector<DamageParam>>;

class Target;
using Targets = std::unordered_map<Id_t, Target *>;

class Skill;
using Skills = std::unordered_map<Id_t, Skill *>;

class Buff;
using Buffs = std::unordered_map<Id_t, Buff *>;

/* 奇穴 */
using Talents = std::unordered_map<Id_t, bool>;

/* 秘籍 */
using Recipes = std::unordered_map<Id_t, bool>;

/* 装备效果 */
using EquipEffects = std::unordered_map<Id_t, bool>;

struct Options
{
    enum Mode
    {
        DEFAULT,
        DEBUG
    };

    Mode    mode          = DEFAULT;
    int     simIterations = 1;
    int     delayMin      = 0;
    int     delayMax      = 0;
    Frame_t totalFrames   = 0;
    Frame_t framePrecision = 16;

    std::unordered_map<Attribute::Type, bool> gainSwitch;

    Frame_t DelayFrames() const
    {
        return RandomNormal(delayMin, delayMax) / JX3DPS_DELAY;
    }
};

class Player;

struct Params
{
    enum class Type
    {
        ADD   = 1,
        CLEAR = -1,
    };

    Player    *player = nullptr;
    Type       type;
    Id_t       targetId;
    RollResult rollResult;
    int        sub   = 0;
    int        level = 0;
    int        stackNum;
    int        effectCount;
};

/* 附加效果 */
using TriggerEffect = std::function<void(const Params &)>;

using TriggerEffects = std::unordered_map<Id_t, TriggerEffect>;

} // namespace JX3DPS

#endif // __JX3DPS_DEFS_H__