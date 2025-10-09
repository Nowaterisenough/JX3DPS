#ifndef JX3DPS_DAMAGE_HPP
#define JX3DPS_DAMAGE_HPP

#include "jx3.h"
#include "type.h"

namespace JX3DPS {

// 负数为1，非负数为0
#define JX3DPS_OPTIMIZATION_CHECK_NEGATIVE(x) (((x) >> (sizeof(x) * 8 - 1)) & 1)

/*-----------  系数  -----------*/

cof_t PhysicsDamageCoefficient(value_t channel_interval, value_t prepare_frames)
{
    return std::max(channel_interval + prepare_frames, static_cast<value_t>(JX3_DAMAGE_CONST_PARAM)) *
           JX3_PERCENT_FLOAT_BASE / JX3_PHYSICS_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM;
}

cof_t MagicDamageCoefficient(value_t channel_interval, value_t prepare_frames)
{
    return std::max(channel_interval + prepare_frames, static_cast<value_t>(JX3_DAMAGE_CONST_PARAM)) *
           JX3_PERCENT_FLOAT_BASE / JX3_MAGIC_DAMAGE_PARAM / JX3_DAMAGE_CONST_PARAM;
}

cof_t PhysicsDotDamageCoefficient(value_t channel_interval, int effect_count_max, tick_t interval_frames)
{
    return channel_interval *
           std::max(JX3_DAMAGE_CONST_PARAM, effect_count_max * interval_frames / JX3_DOT_DAMAGE_CONST_PARAM) *
           JX3_PERCENT_FLOAT_BASE / effect_count_max / JX3_PHYSICS_DAMAGE_PARAM /
           JX3_DAMAGE_CONST_PARAM / JX3_DAMAGE_CONST_PARAM;
}

cof_t MagicDotDamageCoefficient(value_t channel_interval, int effect_count_max, tick_t interval_frames)
{
    return channel_interval *
           std::max(JX3_DAMAGE_CONST_PARAM, effect_count_max * interval_frames / JX3_DOT_DAMAGE_CONST_PARAM) *
           JX3_PERCENT_FLOAT_BASE / effect_count_max / JX3_MAGIC_DAMAGE_PARAM /
           JX3_DAMAGE_CONST_PARAM / JX3_DAMAGE_CONST_PARAM;
}

/*-----------  固定伤害  -----------*/

value_t FixedDamage(value_t fixed_damage, pctn_t fixed_damage_add)
{
    return fixed_damage * (JX3_PERCENT_INT_BASE + fixed_damage_add) / JX3_PERCENT_INT_BASE;
}

/*-----------  攻击伤害  -----------*/

// 伤害 = 攻击 * 系数
value_t AttackDamage(value_t attack, pctf_t damage_coefficient)
{
    return attack * damage_coefficient;
}

/*-----------  武器伤害  -----------*/

// 伤害 = 武器攻击 * 系数
value_t WeaponDamage(value_t weapon_attack, pctn_t weapon_damage_coefficient)
{
    return weapon_attack * weapon_damage_coefficient / JX3_PERCENT_INT_BASE;
}

/*-----------  初始伤害  -----------*/

// 伤害 = 固定伤害 + 攻击伤害 + 武器伤害
value_t InitialDamage(value_t fixedDamage, value_t attackDamage, value_t weaponDamage)
{
    return fixedDamage + attackDamage + weaponDamage;
}

/*-----------  破招伤害  -----------*/

value_t SurplusDamage(value_t surplus, pctn_t surplusCoefficientInt, int playerLevel)
{
    return static_cast<value_t>(
        surplus *
        ((surplusCoefficientInt + JX3DPS_OPTIMIZATION_CHECK_NEGATIVE(surplusCoefficientInt)) / JX3_PERCENT_INT_BASE +
         JX3_PERCENT_INT_BASE) /
        JX3_PERCENT_INT_BASE * JX3_SURPLUS_PARAM);
}

/*-----------  效果加成伤害  -----------*/

// 伤害 = 初始伤害 * 效果增益系数
value_t EffectDamage(value_t initialDamage, pctn_t damage_add)
{
    return initialDamage * (JX3_PERCENT_INT_BASE + damage_add) / JX3_PERCENT_INT_BASE;
}

/*-----------  实际防御  -----------*/

pctn_t ShieldPercentInt(value_t shieldBase, value_t shieldAdditional, pctn_t ignoreShieldBasePercentInt, pctn_t ignoreShieldAdditionalPercentInt)
{
    pctn_t shield = shieldBase - static_cast<value_t>(shieldBase * ignoreShieldBasePercentInt / JX3_PERCENT_INT_BASE);
    shield += shieldAdditional;
    shield -= static_cast<value_t>(shield * ignoreShieldAdditionalPercentInt / JX3_PERCENT_INT_BASE);
    shield = shield < 0 ? 0 : shield;
    return shield;
}

/*-----------  破防加成伤害  -----------*/

value_t PhysicsOvercomeDamage(value_t effectDamage, value_t shield, value_t overcome, int playerLevel, int targetLevel)
{
    pctn_t overcomePercentInt =
        overcome * JX3_PERCENT_INT_BASE / (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * playerLevel - JX3_LEVEL_CONST)) +
        JX3_PERCENT_INT_BASE;
    pctn_t shieldPercentInt =
        shield * JX3_PERCENT_INT_BASE /
        (shield + JX3_PHYSICS_SHIELD_PARAM * (JX3_LEVEL_PARAM * targetLevel - JX3_LEVEL_CONST));
    pctn_t overcomeDamageAdditionalPercentInt =
        overcomePercentInt - static_cast<pctn_t>(overcomePercentInt * shieldPercentInt / JX3_PERCENT_INT_BASE);
    return effectDamage * overcomeDamageAdditionalPercentInt / JX3_PERCENT_INT_BASE;
}

value_t MagicOvercomeDamage(value_t effectDamage, value_t shield, value_t overcome, int playerLevel, int targetLevel)
{
    pctn_t overcomePercentInt =
        overcome * JX3_PERCENT_INT_BASE / (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * playerLevel - JX3_LEVEL_CONST)) +
        JX3_PERCENT_INT_BASE;
    pctn_t shieldPercentInt =
        shield * JX3_PERCENT_INT_BASE /
        (shield + JX3_MAGIC_SHIELD_PARAM * (JX3_LEVEL_PARAM * targetLevel - JX3_LEVEL_CONST));
    pctn_t overcomeDamageAdditionalPercentInt =
        overcomePercentInt - static_cast<pctn_t>(overcomePercentInt * shieldPercentInt / JX3_PERCENT_INT_BASE);
    return effectDamage * overcomeDamageAdditionalPercentInt / JX3_PERCENT_INT_BASE;
}

/*-----------  判定伤害  -----------*/

// rollResult: 0 为命中，1 为会心
value_t RollDamage(value_t overcomeDamage, int rollResult, value_t criticalStrikePower, pctn_t effectCriticalStrikePowerPercentInt, int playerLevel)
{
    pctn_t criticalStrikePowerPercentInt =
        criticalStrikePower * JX3_PERCENT_INT_BASE /
        (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * playerLevel - JX3_LEVEL_CONST));

    value_t temp = overcomeDamage >> (rollResult / 2);

    return static_cast<value_t>(
        temp + temp * (rollResult & 1) * (JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE - JX3_PERCENT_FLOAT_BASE) +
        temp * (rollResult & 1) * (criticalStrikePowerPercentInt + effectCriticalStrikePowerPercentInt) /
            JX3_PERCENT_INT_BASE);
}

/*-----------  等级加成伤害  -----------*/

// 不超过JX3_LEVEL_DAMAGE_MAX_LEVEL_DIFF的差值，优化考虑，不用
int LevelDifference(int playerLevel, int targetLevel)
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

pctf_t LevelDamageAdditionalPercent(int targetLevel)
{
    int deltaLevel = JX3_PLAYER_LEVEL - targetLevel;
    return deltaLevel * (JX3DPS_OPTIMIZATION_CHECK_NEGATIVE(deltaLevel) * JX3_LEVEL_DAMAGE_REDUCTION +
                         JX3DPS_OPTIMIZATION_CHECK_NEGATIVE(-deltaLevel) * JX3_LEVEL_DAMAGE_INCREASE);
}

value_t LevelDamage(value_t rollDamage, int targetLevel)
{
    return rollDamage * (JX3_PERCENT_FLOAT_BASE + LevelDamageAdditionalPercent(targetLevel));
}

/*-----------  无双加成伤害  -----------*/

value_t StrainDamage(value_t levelDamage, value_t strain, pctn_t strainPercentInt, int playerLevel)
{
    pctn_t strainDamageAdditionalPercentInt =
        strain * JX3_PERCENT_INT_BASE / (JX3_INSIGHT_PARAM * (JX3_LEVEL_PARAM * playerLevel - JX3_LEVEL_CONST));
    strainDamageAdditionalPercentInt += (JX3_PERCENT_INT_BASE + strainPercentInt);
    return levelDamage * strainDamageAdditionalPercentInt / JX3_PERCENT_INT_BASE;
}

/*-----------  心法非侠士加成伤害  -----------*/

value_t PVEDamage(value_t strainDamage, pctn_t pveDamageAdditionalPercentInt)
{
    return strainDamage * (pveDamageAdditionalPercentInt + JX3_PERCENT_INT_BASE) / JX3_PERCENT_INT_BASE;
}

/*-----------  易伤加成伤害  -----------*/

value_t VulnerableDamage(value_t pveDamage, pctn_t vulnerablePercentInt)
{
    return pveDamage * (vulnerablePercentInt + JX3_PERCENT_INT_BASE) / JX3_PERCENT_INT_BASE;
}

/*-----------  最终伤害  -----------*/

value_t EffectDamageAll(value_t attack,
                        pctf_t  damageCoefficient,
                        value_t weaponAttack,
                        pctn_t  weaponDamageCoefficientInt,
                        value_t fixedDamage,
                        pctn_t  effectDamageAdditionalPercentInt)
{
    value_t attackDamage  = AttackDamage(attack, damageCoefficient);
    value_t weaponDamage  = WeaponDamage(weaponAttack, weaponDamageCoefficientInt);
    value_t initialDamage = InitialDamage(fixedDamage, attackDamage, weaponDamage);
    return EffectDamage(initialDamage, effectDamageAdditionalPercentInt);
}

/**
 * 计算最终外功伤害值
 * @param playerLevel 玩家等级
 * @param targetLevel 目标等级
 * @param damage 基础伤害值
 * @param shieldBase 防御基础值
 * @param shieldAdditional 防御附加值
 * @param ignoreShieldBasePercentInt 忽略基础防御百分比
 * @param ignoreShieldAdditionalPercentInt 忽略最终防御百分比
 * @param overcome 破防值
 * @param rollResult 骰子结果
 * @param criticalStrikePower 会效
 * @param effectCriticalStrikePowerPercentInt 会效加成百分比
 * @param strain 无双
 * @param strainPercentInt 无双加成百分比
 * @param pveDamageAdditionalPercentInt PVE伤害附加百分比
 * @param vulnerablePercentInt 易伤百分比
 * @return 最终外功伤害值
 */
value_t FinalPhysicsDamage(
    int     playerLevel,
    int     targetLevel,
    value_t damage,
    value_t shieldBase,
    value_t shieldAdditional,
    pctn_t  ignoreShieldBasePercentInt,
    pctn_t  ignoreShieldAdditionalPercentInt,
    value_t overcome,
    int     rollResult,
    value_t criticalStrikePower,
    pctn_t  effectCriticalStrikePowerPercentInt,
    value_t strain,
    pctn_t  strainPercentInt,
    pctn_t  pveDamageAdditionalPercentInt,
    pctn_t  vulnerablePercentInt)
{

    pctn_t shieldPercentInt =
        ShieldPercentInt(shieldBase, shieldAdditional, ignoreShieldBasePercentInt, ignoreShieldAdditionalPercentInt);
    value_t physicsOvercomeDamage =
        PhysicsOvercomeDamage(damage, shieldPercentInt, overcome, playerLevel, targetLevel);
    value_t rollDamage =
        RollDamage(physicsOvercomeDamage, rollResult, criticalStrikePower, effectCriticalStrikePowerPercentInt, playerLevel);
    value_t levelDamage  = LevelDamage(rollDamage, targetLevel);
    value_t strainDamage = StrainDamage(levelDamage, strain, strainPercentInt, playerLevel);
    value_t classDamage  = PVEDamage(strainDamage, pveDamageAdditionalPercentInt);
    return VulnerableDamage(classDamage, vulnerablePercentInt);
}

/**
 * 计算最终内功伤害值
 * @param playerLevel 玩家等级
 * @param targetLevel 目标等级
 * @param damage 基础伤害值
 * @param shieldBase 防御基础值
 * @param shieldAdditional 防御附加值
 * @param ignoreShieldBasePercentInt 忽略基础防御百分比
 * @param ignoreShieldAdditionalPercentInt 忽略最终防御百分比
 * @param overcome 破防值
 * @param rollResult 骰子结果
 * @param criticalStrikePower 会效
 * @param effectCriticalStrikePowerPercentInt 会效加成百分比
 * @param strain 无双
 * @param strainPercentInt 无双加成百分比
 * @param pveDamageAdditionalPercentInt PVE伤害附加百分比
 * @param vulnerablePercentInt 易伤百分比
 * @return 最终内功伤害值
 */
value_t FinalMagicDamage(
    int     playerLevel,
    int     targetLevel,
    value_t damage,
    value_t shieldBase,
    value_t shieldAdditional,
    pctn_t  ignoreShieldBasePercentInt,
    pctn_t  ignoreShieldAdditionalPercentInt,
    value_t overcome,
    int     rollResult,
    value_t criticalStrikePower,
    pctn_t  effectCriticalStrikePowerPercentInt,
    value_t strain,
    pctn_t  strainPercentInt,
    pctn_t  pveDamageAdditionalPercentInt,
    pctn_t  vulnerablePercentInt)
{

    pctn_t shieldPercentInt =
        ShieldPercentInt(shieldBase, shieldAdditional, ignoreShieldBasePercentInt, ignoreShieldAdditionalPercentInt);
    value_t magicOvercomeDamage =
        MagicOvercomeDamage(damage, shieldPercentInt, overcome, playerLevel, targetLevel);
    value_t rollDamage =
        RollDamage(magicOvercomeDamage, rollResult, criticalStrikePower, effectCriticalStrikePowerPercentInt, playerLevel);
    value_t levelDamage  = LevelDamage(rollDamage, targetLevel);
    value_t strainDamage = StrainDamage(levelDamage, strain, strainPercentInt, playerLevel);
    value_t classDamage  = PVEDamage(strainDamage, pveDamageAdditionalPercentInt);
    return VulnerableDamage(classDamage, vulnerablePercentInt);
}

} // namespace JX3DPS

#endif // JX3DPS_DAMAGE_HPP