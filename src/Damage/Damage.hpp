/**
 * Project: JX3DPS
 * File: Damge.h
 * Description:
 * Created Date: 2023-07-12 00:26:38
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-15 01:23:55
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_DAMAGE_H__
#define __JX3DPS_DAMAGE_H__

#include "Global/JX3.h"
#include "Global/Types.h"

namespace JX3DPS {

#define JX3DPS_OPTIMIZATION_MAX(x, y) \
    ((1 & (~(((x) - (y)) >> 31))) * (x) - (~(((y) - (x)) >> 31)) * (y))
#define JX3DPS_OPTIMIZATION_MIN(x, y) \
    ((1 & (~(((y) - (x)) >> 31))) * (x) - (~(((x) - (y)) >> 31)) * (y))

// 负数为1，非负数为0
#define JX3DPS_OPTIMIZATION_CHECK_NEGATIVE(x) (((x) >> (sizeof(x) * 8 - 1)) & 1)

/*-----------  系数  -----------*/

inline PctFloat_t PhysicsDamageCoefficient(Value_t channelInterval, Value_t prepareFrames)
{
    // return JX3DPS_OPTIMIZATION_MAX(channelInterval + prepareFrames, JX3_DAMAGE_CONST_PARAM) / JX3_PHYSICS_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM;

    // 优化考虑替换成下面的，当 channelInterval + prepareFrames < JX3_DAMAGE_CONST_PARAM 时，系数请直接给 JX3_DAMAGE_CONST_PARAM
    return (channelInterval + prepareFrames) / JX3_PHYSICS_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM;
}

inline PctFloat_t MagicDamageCoefficient(Value_t channelInterval, Value_t prepareFrames)
{
    // return JX3DPS_OPTIMIZATION_MAX(channelInterval + prepareFrames, JX3_DAMAGE_CONST_PARAM) / JX3_MAGIC_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM;

    // 优化考虑替换成下面的，当 channelInterval + prepareFrames < JX3_DAMAGE_CONST_PARAM 时，系数请直接给 JX3_DAMAGE_CONST_PARAM
    return (channelInterval + prepareFrames) / JX3_MAGIC_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM;
}

inline PctFloat_t PhysicsDotDamageCoefficient(Value_t channelInterval, int effectCountMax, Frame_t intervalFrames)
{
    // return channelInterval / JX3_PHYSICS_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM *
    //        JX3DPS_OPTIMIZATION_MAX(JX3_DAMAGE_CONST_PARAM, (int)(effectCountMax * intervalFrames
    //        / JX3_DOT_DAMAGE_CONST_PARAM)) / (effectCountMax * JX3_DAMAGE_CONST_PARAM);

    // 优化考虑替换成下面的，当 dot持续时间小于12秒 时，请直接给 effectCountMax = 12, intervalFrames = 1
    return channelInterval * JX3_PERCENT_FLOAT_BASE / JX3_PHYSICS_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM *
           static_cast<Value_t>(effectCountMax * intervalFrames / JX3_DOT_DAMAGE_CONST_PARAM) /
           (effectCountMax * JX3_DAMAGE_CONST_PARAM);
}

inline PctFloat_t MagicDotDamageCoefficient(Value_t channelInterval, int effectCountMax, Frame_t intervalFrames)
{
    // return channelInterval / JX3_MAGIC_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM *
    //        JX3DPS_OPTIMIZATION_MAX(JX3_DAMAGE_CONST_PARAM, (int)(effectCountMax * intervalFrames
    //        / JX3_DOT_DAMAGE_CONST_PARAM)) / (effectCountMax * JX3_DAMAGE_CONST_PARAM);

    // 优化考虑替换成下面的，当 dot持续时间小于12秒 时，请直接给 effectCountMax = 12, intervalFrames = 1
    return channelInterval / JX3_PHYSICS_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM *
           static_cast<Value_t>(effectCountMax * intervalFrames / JX3_DOT_DAMAGE_CONST_PARAM) /
           (effectCountMax * JX3_DAMAGE_CONST_PARAM);
}

/*-----------  攻击伤害  -----------*/

// 伤害 = 攻击 * 系数
inline Value_t AttackDamage(Value_t attack, PctFloat_t damageCoefficient)
{
    return attack * damageCoefficient;
}

/*-----------  武器伤害  -----------*/

// 伤害 = 武器攻击 * 系数
inline Value_t WeaponDamage(Value_t weaponAttack, PctInt_t weaponDamageCoefficientInt)
{
    return weaponAttack * weaponDamageCoefficientInt * JX3_PERCENT_FLOAT_BASE / JX3_PERCENT_INT_BASE;
}

/*-----------  初始伤害  -----------*/

// 伤害 = 固定伤害 + 攻击伤害 + 武器伤害
inline Value_t InitialDamage(Value_t fixedDamage, Value_t attackDamage, Value_t weaponDamage)
{
    return fixedDamage + attackDamage + weaponDamage;
}

/*-----------  破招伤害  -----------*/

inline Value_t SurplusDamage(Value_t surplus, PctInt_t surplusCoefficientInt, int playerLevel)
{
    return static_cast<Value_t>(
        surplus *
        ((surplusCoefficientInt + JX3DPS_OPTIMIZATION_CHECK_NEGATIVE(surplusCoefficientInt)) / JX3_PERCENT_INT_BASE +
         JX3_PERCENT_INT_BASE) /
        JX3_PERCENT_INT_BASE * JX3_SURPLUS_PARAM);
}

/*-----------  效果加成伤害  -----------*/

// 伤害 = 初始伤害 * 效果增益系数
inline Value_t EffectDamage(Value_t initialDamage, PctInt_t effectDamageAdditionalPercentInt)
{
    return initialDamage * (JX3_PERCENT_INT_BASE + effectDamageAdditionalPercentInt) / JX3_PERCENT_INT_BASE;
}

/*-----------  实际防御  -----------*/

inline PctInt_t ShieldPercentInt(Value_t shieldBase, Value_t shieldAdditional, PctInt_t ignoreShieldBasePercentInt, PctInt_t ignoreShieldAdditionalPercentInt)
{
    PctInt_t shield =
        shieldBase - static_cast<Value_t>(shieldBase * ignoreShieldBasePercentInt / JX3_PERCENT_INT_BASE);
    shield += shieldAdditional;
    shield -= static_cast<Value_t>(shield * ignoreShieldAdditionalPercentInt / JX3_PERCENT_INT_BASE);
    shield = shield < 0 ? 0 : shield;
    return shield;
}

/*-----------  破防加成伤害  -----------*/

inline Value_t PhysicsOvercomeDamage(Value_t effectDamage, Value_t shield, Value_t overcome, int playerLevel, int targetLevel)
{
    PctInt_t overcomePercentInt =
        overcome * JX3_PERCENT_INT_BASE /
            (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * playerLevel - JX3_LEVEL_CONST)) +
        JX3_PERCENT_INT_BASE;
    PctInt_t shieldPercentInt =
        shield * JX3_PERCENT_INT_BASE /
        (shield + JX3_PHYSICS_SHIELD_PARAM * (JX3_LEVEL_PARAM * targetLevel - JX3_LEVEL_CONST));
    PctInt_t overcomeDamageAdditionalPercentInt =
        overcomePercentInt -
        static_cast<PctInt_t>(overcomePercentInt * shieldPercentInt / JX3_PERCENT_INT_BASE);
    return effectDamage * overcomeDamageAdditionalPercentInt / JX3_PERCENT_INT_BASE;
}

inline Value_t MagicOvercomeDamage(Value_t effectDamage, Value_t shield, Value_t overcome, int playerLevel, int targetLevel)
{
    PctInt_t overcomePercentInt =
        overcome * JX3_PERCENT_INT_BASE /
            (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * playerLevel - JX3_LEVEL_CONST)) +
        JX3_PERCENT_INT_BASE;
    PctInt_t shieldPercentInt =
        shield * JX3_PERCENT_INT_BASE /
        (shield + JX3_MAGIC_SHIELD_PARAM * (JX3_LEVEL_PARAM * targetLevel - JX3_LEVEL_CONST));
    PctInt_t overcomeDamageAdditionalPercentInt =
        overcomePercentInt -
        static_cast<PctInt_t>(overcomePercentInt * shieldPercentInt / JX3_PERCENT_INT_BASE);
    return effectDamage * overcomeDamageAdditionalPercentInt / JX3_PERCENT_INT_BASE;
}

/*-----------  判定伤害  -----------*/

// rollResult: 0 为命中，1 为会心
inline Value_t RollDamage(Value_t  overcomeDamage,
                          int      rollResult,
                          Value_t  criticalStrikePower,
                          PctInt_t effectCriticalStrikePowerPercentInt,
                          int      playerLevel)
{
    PctInt_t criticalStrikePowerPercentInt =
        criticalStrikePower * JX3_PERCENT_INT_BASE /
        (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * playerLevel - JX3_LEVEL_CONST));

    Value_t criticalStrikePowerDamage =
        rollResult *
        ((overcomeDamage * (JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE - JX3_PERCENT_FLOAT_BASE)) +
         static_cast<Value_t>(overcomeDamage * (criticalStrikePowerPercentInt + effectCriticalStrikePowerPercentInt) /
                              JX3_PERCENT_INT_BASE));

    return overcomeDamage + criticalStrikePowerDamage;
}

/*-----------  等级加成伤害  -----------*/

// 不超过JX3_LEVEL_DAMAGE_MAX_LEVEL_DIFF的差值，优化考虑，不用
inline int LevelDifference(int playerLevel, int targetLevel)
{
    int diff = playerLevel - targetLevel;
    if (diff < -JX3_LEVEL_DAMAGE_MAX_LEVEL_DIFF) {
        return -JX3_LEVEL_DAMAGE_MAX_LEVEL_DIFF;
    }
    if (diff > JX3_LEVEL_DAMAGE_MAX_LEVEL_DIFF) {
        return JX3_LEVEL_DAMAGE_MAX_LEVEL_DIFF;
    }
    return diff;
}

inline PctFloat_t LevelDamageAdditionalPercent(int targetLevel)
{
    int deltaLevel = JX3_PLAYER_LEVEL - targetLevel;
    return deltaLevel *
           (JX3DPS_OPTIMIZATION_CHECK_NEGATIVE(deltaLevel) * JX3_LEVEL_DAMAGE_REDUCTION +
            JX3DPS_OPTIMIZATION_CHECK_NEGATIVE(-deltaLevel) * JX3_LEVEL_DAMAGE_INCREASE);
}

inline Value_t LevelDamage(Value_t rollDamage, int targetLevel)
{
    return rollDamage * (JX3_PERCENT_FLOAT_BASE + LevelDamageAdditionalPercent(targetLevel));
}

/*-----------  无双加成伤害  -----------*/

inline Value_t StrainDamage(Value_t levelDamage, Value_t strain, PctInt_t strainPercentInt, int playerLevel)
{
    PctInt_t strainDamageAdditionalPercentInt =
        strain * JX3_PERCENT_INT_BASE /
        (JX3_INSIGHT_PARAM * (JX3_LEVEL_PARAM * playerLevel - JX3_LEVEL_CONST));
    strainDamageAdditionalPercentInt += (JX3_PERCENT_INT_BASE + strainPercentInt);
    return levelDamage * strainDamageAdditionalPercentInt / JX3_PERCENT_INT_BASE;
}

/*-----------  心法非侠士加成伤害  -----------*/

inline Value_t PVEDamage(Value_t strainDamage, PctInt_t pveDamageAdditionalPercentInt)
{
    return strainDamage * (pveDamageAdditionalPercentInt + JX3_PERCENT_INT_BASE) / JX3_PERCENT_INT_BASE;
}

/*-----------  易伤加成伤害  -----------*/

inline Value_t VulnerableDamage(Value_t pveDamage, PctInt_t vulnerablePercentInt)
{
    return pveDamage * (vulnerablePercentInt + JX3_PERCENT_INT_BASE) / JX3_PERCENT_INT_BASE;
}

/*-----------  最终伤害  -----------*/

inline Value_t EffectDamageAll(Value_t    attack,
                               PctFloat_t damageCoefficient,
                               Value_t    weaponAttack,
                               PctInt_t   weaponDamageCoefficientInt,
                               Value_t    fixedDamage,
                               PctInt_t   effectDamageAdditionalPercentInt)
{
    Value_t attackDamage  = AttackDamage(attack, damageCoefficient);
    Value_t weaponDamage  = WeaponDamage(weaponAttack, weaponDamageCoefficientInt);
    Value_t initialDamage = InitialDamage(fixedDamage, attackDamage, weaponDamage);
    return EffectDamage(initialDamage, effectDamageAdditionalPercentInt);
}

inline Value_t FinalPhysicsDamage(
    int      playerLevel,
    int      targetLevel,
    Value_t  damage,
    Value_t  shieldBase,
    Value_t  shieldAdditional,
    PctInt_t ignoreShieldBasePercentInt,
    PctInt_t ignoreShieldAdditionalPercentInt,
    Value_t  overcome,
    int      rollResult,
    Value_t  criticalStrikePower,
    PctInt_t effectCriticalStrikePowerPercentInt,
    Value_t  strain,
    PctInt_t strainPercentInt,
    PctInt_t pveDamageAdditionalPercentInt,
    PctInt_t vulnerablePercentInt)
{

    PctInt_t shieldPercentInt =
        ShieldPercentInt(shieldBase, shieldAdditional, ignoreShieldBasePercentInt, ignoreShieldAdditionalPercentInt);
    Value_t physicsOvercomeDamage =
        PhysicsOvercomeDamage(damage, shieldPercentInt, overcome, playerLevel, targetLevel);
    Value_t rollDamage =
        RollDamage(physicsOvercomeDamage, rollResult, criticalStrikePower, effectCriticalStrikePowerPercentInt, playerLevel);
    Value_t levelDamage = LevelDamage(rollDamage, targetLevel);
    Value_t strainDamage = StrainDamage(levelDamage, strain, strainPercentInt, playerLevel);
    Value_t classDamage = PVEDamage(strainDamage, pveDamageAdditionalPercentInt);
    return VulnerableDamage(classDamage, vulnerablePercentInt);
}

inline Value_t FinalMagicDamage(
    int      playerLevel,
    int      targetLevel,
    Value_t  damage,
    Value_t  shieldBase,
    Value_t  shieldAdditional,
    PctInt_t ignoreShieldBasePercentInt,
    PctInt_t ignoreShieldAdditionalPercentInt,
    Value_t  overcome,
    int      rollResult,
    Value_t  criticalStrikePower,
    PctInt_t effectCriticalStrikePowerPercentInt,
    Value_t  strain,
    PctInt_t strainPercentInt,
    PctInt_t pveDamageAdditionalPercentInt,
    PctInt_t vulnerablePercentInt)
{

    PctInt_t shieldPercentInt =
        ShieldPercentInt(shieldBase, shieldAdditional, ignoreShieldBasePercentInt, ignoreShieldAdditionalPercentInt);
    Value_t magicOvercomeDamage =
        MagicOvercomeDamage(damage, shieldPercentInt, overcome, playerLevel, targetLevel);
    Value_t rollDamage =
        RollDamage(magicOvercomeDamage, rollResult, criticalStrikePower, effectCriticalStrikePowerPercentInt, playerLevel);
    Value_t levelDamage = LevelDamage(rollDamage, targetLevel);
    Value_t strainDamage = StrainDamage(levelDamage, strain, strainPercentInt, playerLevel);
    Value_t classDamage = PVEDamage(strainDamage, pveDamageAdditionalPercentInt);
    return VulnerableDamage(classDamage, vulnerablePercentInt);
}

} // namespace JX3DPS

#endif // __JX3DPS_DAMAGE_H__