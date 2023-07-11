/**
 * Project: JX3DPS
 * File: Buff.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-11 10:05:33
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Buff.h"

#include <spdlog/spdlog.h>

#include <Utils/Utils.h>

#include "Optimization.h"
#include "Player.h"

namespace JX3DPS {

JX3DPS::Buff::Buff(Player *player, Targets *targets)
{
    this->m_player  = player;
    this->m_targets = targets;
}

JX3DPS::Buff::~Buff() { }

JX3DPS::Buff::Buff(const Buff &other)
{
    m_id                                 = other.m_id;
    m_name                               = other.m_name;
    m_range                              = other.m_range;
    m_tbuff                              = other.m_tbuff;
    m_subNames                           = other.m_subNames;
    m_levelNames                         = other.m_levelNames;
    m_targetSnapshots                    = other.m_targetSnapshots;
    m_durationFixed                      = other.m_durationFixed;
    m_intervalFixed                      = other.m_intervalFixed;
    m_stackNumFixed                      = other.m_stackNumFixed;
    m_effectCountFixed                   = other.m_effectCountFixed;
    m_damageParams                       = other.m_damageParams;
    m_stats                              = other.m_stats;
    m_skillCriticalStrikeAddPercent      = other.m_skillCriticalStrikeAddPercent;
    m_skillCriticalStrikePowerAddPercent = other.m_skillCriticalStrikePowerAddPercent;
    m_skillDamageAddPercentInt           = other.m_skillDamageAddPercentInt;
}

JX3DPS::Buff &JX3DPS::Buff::operator=(const Buff &other)
{
    if (this == &other) {
        return *this;
    }

    m_id                                 = other.m_id;
    m_name                               = other.m_name;
    m_range                              = other.m_range;
    m_tbuff                              = other.m_tbuff;
    m_subNames                           = other.m_subNames;
    m_levelNames                         = other.m_levelNames;
    m_targetSnapshots                    = other.m_targetSnapshots;
    m_durationFixed                      = other.m_durationFixed;
    m_intervalFixed                      = other.m_intervalFixed;
    m_stackNumFixed                      = other.m_stackNumFixed;
    m_effectCountFixed                   = other.m_effectCountFixed;
    m_damageParams                       = other.m_damageParams;
    m_stats                              = other.m_stats;
    m_skillCriticalStrikeAddPercent      = other.m_skillCriticalStrikeAddPercent;
    m_skillCriticalStrikePowerAddPercent = other.m_skillCriticalStrikePowerAddPercent;
    m_skillDamageAddPercentInt           = other.m_skillDamageAddPercentInt;

    return *this;
}

void Buff::RemoveTarget(Id_t targetId)
{
    if (m_tbuff) {
        m_targetSnapshots.erase(targetId);
    }
}

void JX3DPS::Buff::SetPlayer(Player *player)
{
    m_player = player;
}

void JX3DPS::Buff::SetTargets(Targets *targets)
{
    m_targets = targets;
}

int JX3DPS::Buff::GetStackNum(Id_t targetId) const
{
    if (m_targetSnapshots.end() == m_targetSnapshots.find(targetId)) {
        return 0;
    }
    return m_targetSnapshots.at(targetId).stackNum;
}

JX3DPS::Frame_t JX3DPS::Buff::GetTimeLeft(Id_t targetId) const
{
    if (m_targetSnapshots.end() == m_targetSnapshots.find(targetId)) {
        return 0;
    }
    return m_targetSnapshots.at(targetId).duration;
}

JX3DPS::Frame_t JX3DPS::Buff::GetNextKeyFrame() const
{
    Frame_t next = JX3DPS_INVALID_FRAMES_SET;
    for (auto &targetSnapshot : m_targetSnapshots) {
        next = std::min(next, targetSnapshot.second.interval);
        next = std::min(next, targetSnapshot.second.duration);
    }
    return next;
}

void JX3DPS::Buff::UpdateKeyFrame(Frame_t frame)
{
    for (auto &targetSnapshot : m_targetSnapshots) {
        if (targetSnapshot.second.duration != JX3DPS_INVALID_FRAMES_SET) {
            targetSnapshot.second.duration -= frame;
        }
        if (targetSnapshot.second.interval != JX3DPS_INVALID_FRAMES_SET) {
            targetSnapshot.second.interval -= frame;
        }
    }
}

RollResult Buff::GetPhysicsRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attr->GetPhysicsCriticalStrikePercent() + m_skillCriticalStrikeAddPercent
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

RollResult Buff::GetMagicRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attr->GetMagicCriticalStrikePercent() + m_skillCriticalStrikeAddPercent
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

inline JX3DPS::Damage GetDamage(const Damage &damage,
                                RollResult    rollResult,
                                float         damageAddOvercome,
                                float         damageAddStrain,
                                float         criticalStrikePowerPercent,
                                float         skillCriticalStrikePowerPercent)
{
    return Damage(
        GET_FINAL_DAMAGE(damage.fixedDamage * damageAddOvercome * damageAddStrain,
                         rollResult,
                         criticalStrikePowerPercent + skillCriticalStrikePowerPercent),
        GET_FINAL_DAMAGE(damage.weaponDamage * damageAddOvercome * damageAddStrain,
                         rollResult,
                         criticalStrikePowerPercent + skillCriticalStrikePowerPercent),
        GET_FINAL_DAMAGE(damage.attackBaseDamage * damageAddOvercome * damageAddStrain,
                         rollResult,
                         criticalStrikePowerPercent + skillCriticalStrikePowerPercent),
        GET_FINAL_DAMAGE(damage.attackMainDamage * damageAddOvercome * damageAddStrain,
                         rollResult,
                         criticalStrikePowerPercent + skillCriticalStrikePowerPercent),
        GET_FINAL_DAMAGE(damage.surplusDamage * damageAddOvercome * damageAddStrain, rollResult, criticalStrikePowerPercent));
}

GainsDamage Buff::CalcPhysicsDamage(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount) const
{
    PctFloat_t surplusDamageAddPercent =
        (JX3_PCT_INT_BASE + m_player->damageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE * // 心法加成
        (JX3_PCT_FLOAT_BASE + (*m_targets)[targetId]->GetDamageAddPercent()) *     // 易伤加成
        (JX3_PCT_FLOAT_BASE - (*m_targets)[targetId]->GetPhysicsResistPercent()) * // 忽视加成
        (JX3_PCT_FLOAT_BASE +
         (JX3_PLAYER_LEVEL - (*m_targets)[targetId]->GetLevel()) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成
    PctFloat_t damagePercent =
        effectCount * surplusDamageAddPercent *
        (JX3_PCT_INT_BASE + m_skillDamageAddPercentInt + m_player->skillDamageAddPercentInt) / JX3_PCT_INT_BASE;

    float damageAddOvercome = (JX3_PCT_FLOAT_BASE + m_player->attr->GetPhysicsOvercomePercent()); // 破防加成
    float damageAddStrain = (JX3_PCT_FLOAT_BASE + m_player->attr->GetStrainPercent()); // 无双加成

    Damage damage;
    damage.fixedDamage = static_cast<int>(m_damageParams.at(sub)[level].fixedDamage * damagePercent);
    damage.weaponDamage =
        static_cast<int>((m_damageParams.at(sub)[level].weaponDamagePercentInt * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    damage.attackBaseDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetPhysicsAttackFromBase() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM;
    damage.attackMainDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetPhysicsAttackFromMain() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM;
    damage.surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                            m_player->attr->GetSurplusDamage() * surplusDamageAddPercent);

    float physicsCriticalStrikePowerPercent = m_player->attr->GetPhysicsCriticalStrikePowerPercent();

    GainsDamage gainsDamage;
    gainsDamage.normalDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrain, physicsCriticalStrikePowerPercent, m_skillCriticalStrikePowerAddPercent);

    // 会效收益
    m_player->attr->AddPhysicsCriticalStrikePower(
        ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_CRITICAL_STRIKE_POWER)]);
    gainsDamage.criticalStrikePowerDamage =
        GetDamage(damage,
                  rollResult,
                  damageAddOvercome,
                  damageAddStrain,
                  m_player->attr->GetPhysicsCriticalStrikePowerPercent(),
                  m_skillCriticalStrikePowerAddPercent);
    m_player->attr->AddPhysicsCriticalStrikePower(
        -ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_CRITICAL_STRIKE_POWER)]);

    // 破防收益
    m_player->attr->AddPhysicsOvercomeBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);
    float damageAddOvercomeGain = (JX3_PCT_FLOAT_BASE + m_player->attr->GetPhysicsOvercomePercent());
    gainsDamage.overcomeDamage =
        GetDamage(damage, rollResult, damageAddOvercomeGain, damageAddStrain, physicsCriticalStrikePowerPercent, m_skillCriticalStrikePowerAddPercent);
    m_player->attr->AddPhysicsOvercomeBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);

    // 无双收益
    m_player->attr->AddStrain(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);
    float damageAddStrainGain = (JX3_PCT_FLOAT_BASE + m_player->attr->GetStrainPercent());
    gainsDamage.strainDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrainGain, physicsCriticalStrikePowerPercent, m_skillCriticalStrikePowerAddPercent);
    m_player->attr->AddStrain(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);

    // 破招收益
    m_player->attr->AddSurplusBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);
    damage.surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                            m_player->attr->GetSurplusDamage() * surplusDamageAddPercent);
    gainsDamage.surplusDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrain, physicsCriticalStrikePowerPercent, m_skillCriticalStrikePowerAddPercent);
    m_player->attr->AddSurplusBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);

    return gainsDamage;
}

GainsDamage Buff::CalcMagicDamage(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount) const
{
    PctFloat_t surplusDamageAddPercent =
        (JX3_PCT_INT_BASE + m_player->damageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE * // 心法加成
        (JX3_PCT_FLOAT_BASE + (*m_targets)[targetId]->GetDamageAddPercent()) *   // 易伤加成
        (JX3_PCT_FLOAT_BASE - (*m_targets)[targetId]->GetMagicResistPercent()) * // 忽视加成
        (JX3_PCT_FLOAT_BASE +
         (JX3_PLAYER_LEVEL - (*m_targets)[targetId]->GetLevel()) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成
    PctFloat_t damagePercent =
        effectCount * surplusDamageAddPercent *
        (JX3_PCT_INT_BASE + m_skillDamageAddPercentInt + m_player->skillDamageAddPercentInt) / JX3_PCT_INT_BASE;

    float damageAddOvercome = (JX3_PCT_FLOAT_BASE + m_player->attr->GetMagicOvercomePercent()); // 破防加成
    float damageAddStrain = (JX3_PCT_FLOAT_BASE + m_player->attr->GetStrainPercent()); // 无双加成

    Damage damage;
    damage.fixedDamage = static_cast<int>(m_damageParams.at(sub)[level].fixedDamage * damagePercent);
    damage.weaponDamage =
        static_cast<int>((m_damageParams.at(sub)[level].weaponDamagePercentInt * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    damage.attackBaseDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetMagicAttackFromBase() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM;
    damage.attackMainDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetMagicAttackFromMain() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM;
    damage.surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                            m_player->attr->GetSurplusDamage() * surplusDamageAddPercent);

    float magicCriticalStrikePowerPercent = m_player->attr->GetMagicCriticalStrikePowerPercent();

    GainsDamage gainsDamage;
    gainsDamage.normalDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrain, magicCriticalStrikePowerPercent, m_skillCriticalStrikePowerAddPercent);

    // 会效收益
    m_player->attr->AddMagicCriticalStrikePower(
        ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::MAGIC_CRITICAL_STRIKE_POWER)]);
    gainsDamage.criticalStrikePowerDamage =
        GetDamage(damage,
                  rollResult,
                  damageAddOvercome,
                  damageAddStrain,
                  m_player->attr->GetMagicCriticalStrikePowerPercent(),
                  m_skillCriticalStrikePowerAddPercent);
    m_player->attr->AddMagicCriticalStrikePower(
        -ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::MAGIC_CRITICAL_STRIKE_POWER)]);

    // 破防收益
    m_player->attr->AddMagicOvercomeBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::MAGIC_OVERCOME)]);
    float damageAddOvercomeGain = (JX3_PCT_FLOAT_BASE + m_player->attr->GetMagicOvercomePercent());
    gainsDamage.overcomeDamage =
        GetDamage(damage, rollResult, damageAddOvercomeGain, damageAddStrain, magicCriticalStrikePowerPercent, m_skillCriticalStrikePowerAddPercent);
    m_player->attr->AddMagicOvercomeBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::MAGIC_OVERCOME)]);

    // 无双收益
    m_player->attr->AddStrain(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);
    float damageAddStrainGain = (JX3_PCT_FLOAT_BASE + m_player->attr->GetStrainPercent());
    gainsDamage.strainDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrainGain, magicCriticalStrikePowerPercent, m_skillCriticalStrikePowerAddPercent);
    m_player->attr->AddStrain(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);

    // 破招收益
    m_player->attr->AddSurplusBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);
    damage.surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                            m_player->attr->GetSurplusDamage() * surplusDamageAddPercent);
    gainsDamage.surplusDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrain, magicCriticalStrikePowerPercent, m_skillCriticalStrikePowerAddPercent);
    m_player->attr->AddSurplusBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);

    return gainsDamage;
}

RollResult Buff::GetPhysicsRollResultDot(Id_t targetId) const
{
    return RandomUniform(0.0, 1.0) < m_targetSnapshots.at(targetId).criticalStrikePercent
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

RollResult Buff::GetMagicRollResultDot(Id_t targetId) const
{
    return RandomUniform(0.0, 1.0) < m_targetSnapshots.at(targetId).criticalStrikePercent
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

GainsDamage Buff::CalcPhysicsDamageDot(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount) const
{
    PctFloat_t surplusDamageAddPercent =
        (JX3_PCT_INT_BASE + m_player->damageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE * // 心法加成
        (JX3_PCT_FLOAT_BASE + (*m_targets)[targetId]->GetDamageAddPercent()) *     // 易伤加成
        (JX3_PCT_FLOAT_BASE - (*m_targets)[targetId]->GetPhysicsResistPercent()) * // 忽视加成
        (JX3_PCT_FLOAT_BASE +
         (JX3_PLAYER_LEVEL - (*m_targets)[targetId]->GetLevel()) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成
    PctFloat_t damagePercent =
        effectCount * surplusDamageAddPercent *
        (JX3_PCT_INT_BASE + m_targetSnapshots.at(targetId).skillDamageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE; // 武学加成

    float damageAddOvercome = (JX3_PCT_FLOAT_BASE + m_player->attr->GetPhysicsOvercomePercent()); // 破防加成
    float damageAddStrain = (JX3_PCT_FLOAT_BASE + m_targetSnapshots.at(targetId).strainPercent); // 无双加成

    Damage damage;
    damage.fixedDamage = static_cast<int>(m_damageParams.at(sub)[level].fixedDamage * damagePercent);
    damage.weaponDamage =
        static_cast<int>((m_damageParams.at(sub)[level].weaponDamagePercentInt * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    damage.attackBaseDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetPhysicsAttackFromBase() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM / JX3_DOT_SKILL_DAMAGE_CONST;
    damage.attackMainDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetPhysicsAttackFromMain() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM / JX3_DOT_SKILL_DAMAGE_CONST;
    damage.surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                            m_player->attr->GetSurplusDamage() * surplusDamageAddPercent);

    float physicsCriticalStrikePowerPercent = m_targetSnapshots.at(targetId).criticalStrikePowerPercent;

    spdlog::debug(
        "心法加成 {} 易伤加成 {} 忽视加成 {} 等级差加成 {} 破防加成 {} 无双加成 {} 武学加成 {} --- 固定伤害 {} "
        "武器伤害 {} 系数伤害 {} 层数 {} --- 最终伤害 {}",
        (JX3_PCT_INT_BASE + m_player->damageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE,
        (JX3_PCT_FLOAT_BASE + (*m_targets)[targetId]->GetDamageAddPercent()),
        (JX3_PCT_FLOAT_BASE - (*m_targets)[targetId]->GetPhysicsResistPercent()),
        (JX3_PCT_FLOAT_BASE + (JX3_PLAYER_LEVEL - (*m_targets)[targetId]->GetLevel()) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION),
        damageAddOvercome,
        damageAddStrain,
        (JX3_PCT_INT_BASE + m_targetSnapshots.at(targetId).skillDamageAddPercentInt)  * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE,
        m_damageParams.at(sub)[level].fixedDamage,
        m_damageParams.at(sub)[level].weaponDamagePercentInt * JX3_PCT_FLOAT_BASE /
            JX3_PCT_INT_BASE * m_player->attr->GetWeaponAttack(),
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent * m_player->attr->GetPhysicsAttack()) /
            JX3_PHYSICS_DAMAGE_PARAM,
        level,
        damage.SumDamage());

    GainsDamage gainsDamage;
    gainsDamage.normalDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrain, physicsCriticalStrikePowerPercent, 0.0);

    // 会效收益
    gainsDamage.criticalStrikePowerDamage =
        GetDamage(damage,
                  rollResult,
                  damageAddOvercome,
                  damageAddStrain,
                  m_targetSnapshots.at(targetId).criticalStrikePowerGainPercent,
                  0.0);

    // 破防收益
    float overcome = m_player->attr->GetPhysicsCriticalStrikePower();
    m_player->attr->AddPhysicsOvercomeBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);
    float damageAddOvercomeGain = (JX3_PCT_FLOAT_BASE + m_player->attr->GetPhysicsOvercomePercent());
    gainsDamage.overcomeDamage =
        GetDamage(damage, rollResult, damageAddOvercomeGain, damageAddStrain, physicsCriticalStrikePowerPercent, 0.0);
    m_player->attr->AddPhysicsOvercomeBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);

    // 无双收益
    gainsDamage.strainDamage =
        GetDamage(damage,
                  rollResult,
                  damageAddOvercome,
                  JX3_PCT_FLOAT_BASE + m_targetSnapshots.at(targetId).strainGainPercent,
                  physicsCriticalStrikePowerPercent,
                  0.0);

    // 破招收益
    m_player->attr->AddSurplusBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);
    damage.surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                            m_player->attr->GetSurplusDamage() * surplusDamageAddPercent);
    gainsDamage.surplusDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrain, physicsCriticalStrikePowerPercent, 0.0);
    m_player->attr->AddSurplusBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);

    return gainsDamage;
}

GainsDamage Buff::CalcMagicDamageDot(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount) const
{
    PctFloat_t surplusDamageAddPercent =
        (JX3_PCT_INT_BASE + m_player->damageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE * // 心法加成
        (JX3_PCT_FLOAT_BASE + (*m_targets)[targetId]->GetDamageAddPercent()) *   // 易伤加成
        (JX3_PCT_FLOAT_BASE - (*m_targets)[targetId]->GetMagicResistPercent()) * // 忽视加成
        (JX3_PCT_FLOAT_BASE +
         (JX3_PLAYER_LEVEL - (*m_targets)[targetId]->GetLevel()) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成
    PctFloat_t damagePercent =
        effectCount * surplusDamageAddPercent *
        (JX3_PCT_INT_BASE + m_targetSnapshots.at(targetId).skillDamageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE;

    float damageAddOvercome = (JX3_PCT_FLOAT_BASE + m_player->attr->GetMagicOvercomePercent()); // 破防加成
    float damageAddStrain = (JX3_PCT_FLOAT_BASE + m_targetSnapshots.at(targetId).strainPercent); // 无双加成

    Damage damage;
    damage.fixedDamage = static_cast<int>(m_damageParams.at(sub)[level].fixedDamage * damagePercent);
    damage.weaponDamage =
        static_cast<int>((m_damageParams.at(sub)[level].weaponDamagePercentInt * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    damage.attackBaseDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetMagicAttackFromBase() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM / JX3_DOT_SKILL_DAMAGE_CONST;
    damage.attackMainDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetMagicAttackFromMain() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM / JX3_DOT_SKILL_DAMAGE_CONST;
    damage.surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                            m_player->attr->GetSurplusDamage() * surplusDamageAddPercent);

    float magicCriticalStrikePowerPercent = m_targetSnapshots.at(targetId).criticalStrikePowerPercent;

    GainsDamage gainsDamage;
    gainsDamage.normalDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrain, magicCriticalStrikePowerPercent, 0.0);

    // 会效收益
    gainsDamage.criticalStrikePowerDamage =
        GetDamage(damage,
                  rollResult,
                  damageAddOvercome,
                  damageAddStrain,
                  m_targetSnapshots.at(targetId).criticalStrikePowerGainPercent,
                  0.0);

    // 破防收益
    float overcome = m_player->attr->GetMagicCriticalStrikePower();
    m_player->attr->AddMagicOvercomeBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::MAGIC_OVERCOME)]);
    float damageAddOvercomeGain = (JX3_PCT_FLOAT_BASE + m_player->attr->GetMagicOvercomePercent());
    gainsDamage.overcomeDamage =
        GetDamage(damage, rollResult, damageAddOvercomeGain, damageAddStrain, magicCriticalStrikePowerPercent, 0.0);
    m_player->attr->AddMagicOvercomeBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::MAGIC_OVERCOME)]);

    // 无双收益
    gainsDamage.strainDamage =
        GetDamage(damage,
                  rollResult,
                  damageAddOvercome,
                  JX3_PCT_FLOAT_BASE + m_targetSnapshots.at(targetId).strainGainPercent,
                  magicCriticalStrikePowerPercent,
                  0.0);

    // 破招收益
    m_player->attr->AddSurplusBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);
    damage.surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                            m_player->attr->GetSurplusDamage() * surplusDamageAddPercent);
    gainsDamage.surplusDamage =
        GetDamage(damage, rollResult, damageAddOvercome, damageAddStrain, magicCriticalStrikePowerPercent, 0.0);
    m_player->attr->AddSurplusBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);

    return gainsDamage;
}

void Buff::Record(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub, int level)
{
    m_stats.damageStats[targetId][m_id][sub][level][rollResult].first++;
    m_stats.damageStats[targetId][m_id][sub][level][rollResult].second.fixedDamage +=
        gainsDamage.normalDamage.fixedDamage;
    m_stats.damageStats[targetId][m_id][sub][level][rollResult].second.weaponDamage +=
        gainsDamage.normalDamage.weaponDamage;
    m_stats.damageStats[targetId][m_id][sub][level][rollResult].second.attackBaseDamage +=
        gainsDamage.normalDamage.attackBaseDamage;
    m_stats.damageStats[targetId][m_id][sub][level][rollResult].second.attackMainDamage +=
        gainsDamage.normalDamage.attackMainDamage;
    m_stats.damageStats[targetId][m_id][sub][level][rollResult].second.surplusDamage +=
        gainsDamage.normalDamage.surplusDamage;

    m_stats.normalDamage              += gainsDamage.normalDamage.SumDamage();
    m_stats.criticalStrikePowerDamage += gainsDamage.criticalStrikePowerDamage.SumDamage();
    m_stats.overcomeDamage            += gainsDamage.overcomeDamage.SumDamage();
    m_stats.strainDamage              += gainsDamage.strainDamage.SumDamage();
    m_stats.surplusDamage             += gainsDamage.surplusDamage.SumDamage();
}

Stats &Buff::GetStats()
{
    return m_stats;
}

const std::string &Buff::GetName() const
{
    return m_name;
}

} // namespace JX3DPS

JX3DPS::Buff3rd::EnchantShoesPhysics::EnchantShoesPhysics(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::ENCHANT_SHOES;
    m_name = "大附魔·鞋";

    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 100, 0.0);

    m_intervalFixed = 16 * 10;
}

void JX3DPS::Buff3rd::EnchantShoesPhysics::Trigger()
{
    // 内置CD时间结束
    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;
}

void JX3DPS::Buff3rd::EnchantShoesPhysics::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;
}

void JX3DPS::Buff3rd::EnchantShoesPhysics::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::EnchantShoesPhysics::TriggerDamage()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].interval == JX3DPS_INVALID_FRAMES_SET) {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
        SubEffect();
    }
}

void JX3DPS::Buff3rd::EnchantShoesPhysics::SubEffect()
{
    RollResult  rollResult  = GetPhysicsRollResult();
    GainsDamage gainsDamage = CalcPhysicsDamage(m_player->targetId, rollResult, 0, 0);
    Record(m_player->targetId, rollResult, gainsDamage, 0, 0);
}

JX3DPS::Buff3rd::EnchantWristPhysics::EnchantWristPhysics(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id    = Buff::ENCHANT_WRIST;
    m_name  = "大附魔·腕";
    m_tbuff = true;

    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 100, 0.0);

    m_intervalFixed = 16 * 10;
}

void JX3DPS::Buff3rd::EnchantWristPhysics::Trigger()
{
    // 内置CD时间结束
    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;
}

void JX3DPS::Buff3rd::EnchantWristPhysics::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;
}

void JX3DPS::Buff3rd::EnchantWristPhysics::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::EnchantWristPhysics::TriggerDamage()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].interval == JX3DPS_INVALID_FRAMES_SET && RandomUniform(1, 100) <= 10)
    {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
        SubEffect();
    }
}

void JX3DPS::Buff3rd::EnchantWristPhysics::SubEffect()
{
    RollResult  rollResult  = GetPhysicsRollResult();
    GainsDamage gainsDamage = CalcPhysicsDamage(m_player->targetId, rollResult, 0, 0);
    Record(m_player->targetId, rollResult, gainsDamage, 0, 0);
}

JX3DPS::Buff3rd::EnchantBelt::EnchantBelt(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::ENCHANT_BELT;
    m_name = "大附魔·腰";

    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_intervalFixed = 16 * 30;
    m_durationFixed = 16 * 8;
}

void JX3DPS::Buff3rd::EnchantBelt::Trigger()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == 0) { // buff时间结束
        m_targetSnapshots[JX3DPS_PLAYER].duration = JX3DPS_INVALID_FRAMES_SET;
        SubEffectClear();
    } else { // 内置CD时间结束
        m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;
    }
}

void JX3DPS::Buff3rd::EnchantBelt::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == JX3DPS_INVALID_FRAMES_SET) {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
    SubEffectAdd();
}

void JX3DPS::Buff3rd::EnchantBelt::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::Buff3rd::EnchantBelt::TriggerAdd()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].interval == JX3DPS_INVALID_FRAMES_SET && RandomUniform(1, 100) <= 20)
    {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
        SubEffectAdd();
    }
}

void JX3DPS::Buff3rd::EnchantBelt::SubEffectAdd()
{
    if (RandomUniform(1, 100) <= 70) {
        m_70                                = true;
        m_player->skillDamageAddPercentInt += 51;
    } else {
        m_player->skillDamageAddPercentInt += 10;
    }
}

void JX3DPS::Buff3rd::EnchantBelt::SubEffectClear()
{
    if (m_70 == true) {
        m_player->skillDamageAddPercentInt -= 51;
    } else {
        m_player->skillDamageAddPercentInt -= 10;
    }
    m_70 = false;
}

JX3DPS::Buff3rd::JiaoSu::JiaoSu(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::ENCHANT_BELT;
    m_name = "3rd·皎素";

    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_intervalFixed = 16 * 30;
    m_durationFixed = 16 * 5;
}

void JX3DPS::Buff3rd::JiaoSu::Trigger()
{
    m_targetSnapshots[JX3DPS_PLAYER].duration = JX3DPS_INVALID_FRAMES_SET;
    SubEffectClear();
}

void JX3DPS::Buff3rd::JiaoSu::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == JX3DPS_INVALID_FRAMES_SET) {
        SubEffectAdd();
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::JiaoSu::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::Buff3rd::JiaoSu::SubEffectAdd()
{
    m_player->attr->AddPhysicsCriticalStrikePowerPercentFromCustom(0.05);
}

void JX3DPS::Buff3rd::JiaoSu::SubEffectClear()
{
    m_player->attr->AddPhysicsCriticalStrikePowerPercentFromCustom(-0.05);
}

JX3DPS::Buff3rd::HanRuLei::HanRuLei(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_HAN_RU_LEI;
    m_name = "3rd·撼如雷";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 60 * 30;
}

void JX3DPS::Buff3rd::HanRuLei::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::Buff3rd::HanRuLei::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        SubEffectAdd();
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::HanRuLei::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::Buff3rd::HanRuLei::SubEffectAdd()
{
    m_player->attr->AddPhysicsAttackBasePercentInt(51);
}

void JX3DPS::Buff3rd::HanRuLei::SubEffectClear()
{
    m_player->attr->AddPhysicsAttackBasePercentInt(-51);
}

JX3DPS::Buff3rd::XiuQi::XiuQi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_XIU_QI;
    m_name = "3rd·袖气";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 60 * 30;
}

void JX3DPS::Buff3rd::XiuQi::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::Buff3rd::XiuQi::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        SubEffectAdd();
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::XiuQi::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::Buff3rd::XiuQi::SubEffectAdd()
{
    m_player->attr->AddAgilityBase(244);
    m_player->attr->AddSpiritBase(244);
    m_player->attr->AddStrengthBase(244);
    m_player->attr->AddSpunkBase(244);
}

void JX3DPS::Buff3rd::XiuQi::SubEffectClear()
{
    m_player->attr->AddAgilityBase(-244);
    m_player->attr->AddSpiritBase(-244);
    m_player->attr->AddStrengthBase(-244);
    m_player->attr->AddSpunkBase(-244);
}

JX3DPS::Buff3rd::PoFeng::PoFeng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_PO_FENG;
    m_name = "3rd·破风";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 14;
}

void JX3DPS::Buff3rd::PoFeng::Trigger()
{
    for (auto it = m_targetSnapshots.begin(); it != m_targetSnapshots.end();) {
        if (it->second.duration == 0) {
            it = m_targetSnapshots.erase(it);
            SubEffectClear(it->first);
        } else {
            ++it;
        }
    }
}

void JX3DPS::Buff3rd::PoFeng::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(targetId) == m_targetSnapshots.end()) {
        SubEffectAdd(targetId);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[unlikely]] {
        m_targetSnapshots[targetId].duration = m_durationFixed;
    } else [[likely]] {
        m_targetSnapshots[targetId].duration = duration;
    }
}

void JX3DPS::Buff3rd::PoFeng::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(targetId);
    SubEffectClear(targetId);
}

void JX3DPS::Buff3rd::PoFeng::SubEffectAdd(int targetId)
{
    (*m_targets)[targetId]->AddPhysicsShield(-1150);
}

void JX3DPS::Buff3rd::PoFeng::SubEffectClear(int targetId)
{
    (*m_targets)[targetId]->AddPhysicsShield(1150);
}

JX3DPS::Buff3rd::JingFeng::JingFeng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_JING_FENG;
    m_name = "3rd·劲风";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 14;
}

void JX3DPS::Buff3rd::JingFeng::Trigger()
{
    for (auto it = m_targetSnapshots.begin(); it != m_targetSnapshots.end();) {
        if (it->second.duration == 0) {
            it = m_targetSnapshots.erase(it);
            SubEffectClear(it->first);
        } else {
            ++it;
        }
    }
}

void JX3DPS::Buff3rd::JingFeng::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(targetId) == m_targetSnapshots.end()) {
        SubEffectAdd(targetId);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[unlikely]] {
        m_targetSnapshots[targetId].duration = m_durationFixed;
    } else [[likely]] {
        m_targetSnapshots[targetId].duration = duration;
    }
}

void JX3DPS::Buff3rd::JingFeng::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(targetId);
    SubEffectClear(targetId);
}

void JX3DPS::Buff3rd::JingFeng::SubEffectAdd(int targetId)
{
    (*m_targets)[targetId]->AddPhysicsShield(-1397);
}

void JX3DPS::Buff3rd::JingFeng::SubEffectClear(int targetId)
{
    (*m_targets)[targetId]->AddPhysicsShield(1397);
}

JX3DPS::Buff3rd::JieHuo::JieHuo(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_JIE_HUO;
    m_name = "3rd·戒火";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 15;
}

void JX3DPS::Buff3rd::JieHuo::Trigger()
{
    for (auto it = m_targetSnapshots.begin(); it != m_targetSnapshots.end();) {
        if (it->second.duration == 0) {
            it = m_targetSnapshots.erase(it);
            SubEffectClear(it->first);
        } else {
            ++it;
        }
    }
}

void JX3DPS::Buff3rd::JieHuo::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(targetId) == m_targetSnapshots.end()) {
        SubEffectAdd(targetId);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[unlikely]] {
        m_targetSnapshots[targetId].duration = m_durationFixed;
    } else [[likely]] {
        m_targetSnapshots[targetId].duration = duration;
    }
}

void JX3DPS::Buff3rd::JieHuo::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(targetId);
    SubEffectClear(targetId);
}

void JX3DPS::Buff3rd::JieHuo::SubEffectAdd(int targetId)
{
    (*m_targets)[targetId]->AddDamageAddPercentInt(20);
}

void JX3DPS::Buff3rd::JieHuo::SubEffectClear(int targetId)
{
    (*m_targets)[targetId]->AddDamageAddPercentInt(-20);
}

JX3DPS::Buff3rd::HaoLingSanJun::HaoLingSanJun(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_HAO_LING_SAN_JUN;
    m_name = "3rd·号令三军";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 60;
    m_intervalFixed = 16 * 30;
    m_stackNumFixed = 48;
}

void JX3DPS::Buff3rd::HaoLingSanJun::Trigger()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == 0) { // 消失
        int stackNum = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
        m_targetSnapshots.erase(JX3DPS_PLAYER);
        SubEffectClear(JX3DPS_PLAYER, stackNum);
    } else { // 一鼓结束，二鼓开始
        m_targetSnapshots[JX3DPS_PLAYER].interval  = m_intervalFixed;
        int stackNum                               = m_targetSnapshots[JX3DPS_PLAYER].stackNum / 2;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum -= stackNum;
        SubEffectClear(JX3DPS_PLAYER, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    }
}

void JX3DPS::Buff3rd::HaoLingSanJun::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = std::min(stackNum, m_stackNumFixed);
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
        SubEffectAdd(JX3DPS_PLAYER);
    }
}

void JX3DPS::Buff3rd::HaoLingSanJun::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    m_targetSnapshots[JX3DPS_PLAYER].stackNum -= stackNum;
    if (m_targetSnapshots[JX3DPS_PLAYER].stackNum == 0) {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
    SubEffectClear(JX3DPS_PLAYER, stackNum);
}

void JX3DPS::Buff3rd::HaoLingSanJun::SubEffectAdd(int targetId)
{
    m_player->attr->AddStrain(m_targetSnapshots[JX3DPS_PLAYER].stackNum * 440);
}

void JX3DPS::Buff3rd::HaoLingSanJun::SubEffectClear(int targetId, int stackNum)
{
    m_player->attr->AddStrain(-stackNum * 440);
}

JX3DPS::Buff3rd::SheShenHongFa::SheShenHongFa(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_SHE_SHEN_HONG_FA;
    m_name = "3rd·舍身弘法";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 20;
    m_stackNumFixed = 36;
}

void JX3DPS::Buff3rd::SheShenHongFa::Trigger()
{
    int stackNum = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER, stackNum);
}

void JX3DPS::Buff3rd::SheShenHongFa::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = std::min(stackNum, m_stackNumFixed);
        SubEffectAdd(JX3DPS_PLAYER, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    } else { // 已有buff,刷新层数
        stackNum  = std::min(stackNum, m_stackNumFixed);
        int stack = stackNum - m_targetSnapshots[JX3DPS_PLAYER].stackNum;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = stackNum;
        SubEffectAdd(JX3DPS_PLAYER, stack);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::SheShenHongFa::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    m_targetSnapshots[JX3DPS_PLAYER].stackNum -= stackNum;
    if (m_targetSnapshots[JX3DPS_PLAYER].stackNum == 0) {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
    SubEffectClear(JX3DPS_PLAYER, stackNum);
}

void JX3DPS::Buff3rd::SheShenHongFa::SubEffectAdd(int targetId, int stackNum)
{
    m_player->attr->AddStrain(stackNum * 400);
}

void JX3DPS::Buff3rd::SheShenHongFa::SubEffectClear(int targetId, int stackNum)
{
    m_player->attr->AddStrain(-stackNum * 400);
}

JX3DPS::Buff3rd::ChaoSheng::ChaoSheng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_CHAO_SHENG;
    m_name = "3rd·朝圣";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 8;
    m_stackNumFixed = 24;
}

void JX3DPS::Buff3rd::ChaoSheng::Trigger()
{
    int stackNum = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER, stackNum);
}

void JX3DPS::Buff3rd::ChaoSheng::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = std::min(stackNum, m_stackNumFixed);
        SubEffectAdd(JX3DPS_PLAYER, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    } else { // 已有buff,刷新层数
        stackNum  = std::min(stackNum, m_stackNumFixed);
        int stack = stackNum - m_targetSnapshots[JX3DPS_PLAYER].stackNum;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = stackNum;
        SubEffectAdd(JX3DPS_PLAYER, stack);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::ChaoSheng::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    m_targetSnapshots[JX3DPS_PLAYER].stackNum -= stackNum;
    if (m_targetSnapshots[JX3DPS_PLAYER].stackNum == 0) {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
    SubEffectClear(JX3DPS_PLAYER, stackNum);
}

void JX3DPS::Buff3rd::ChaoSheng::SubEffectAdd(int targetId, int stackNum)
{
    m_player->attr->AddStrain(stackNum * 400);
}

void JX3DPS::Buff3rd::ChaoSheng::SubEffectClear(int targetId, int stackNum)
{
    m_player->attr->AddStrain(-stackNum * 400);
}

JX3DPS::Buff3rd::ShengYuMingXin::ShengYuMingXin(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_SHENG_YU_MING_XIN;
    m_name = "3rd·圣浴明心";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 8;
    m_stackNumFixed = 24;
}

void JX3DPS::Buff3rd::ShengYuMingXin::Trigger()
{
    int stackNum = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER, stackNum);
}

void JX3DPS::Buff3rd::ShengYuMingXin::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = std::min(stackNum, m_stackNumFixed);
        SubEffectAdd(JX3DPS_PLAYER, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    } else { // 已有buff,刷新层数
        stackNum  = std::min(stackNum, m_stackNumFixed);
        int stack = stackNum - m_targetSnapshots[JX3DPS_PLAYER].stackNum;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = stackNum;
        SubEffectAdd(JX3DPS_PLAYER, stack);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::ShengYuMingXin::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    m_targetSnapshots[JX3DPS_PLAYER].stackNum -= stackNum;
    if (m_targetSnapshots[JX3DPS_PLAYER].stackNum == 0) {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
    SubEffectClear(JX3DPS_PLAYER, stackNum);
}

void JX3DPS::Buff3rd::ShengYuMingXin::SubEffectAdd(int targetId, int stackNum)
{
    m_player->attr->AddStrain(stackNum * 600);
}

void JX3DPS::Buff3rd::ShengYuMingXin::SubEffectClear(int targetId, int stackNum)
{
    m_player->attr->AddStrain(-stackNum * 600);
}

JX3DPS::Buff3rd::ZhenFen::ZhenFen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_ZHEN_FEN;
    m_name = "3rd·振奋";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 10;
    m_stackNumFixed = 125;
}

void JX3DPS::Buff3rd::ZhenFen::Trigger()
{
    int stackNum = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER, stackNum);
}

void JX3DPS::Buff3rd::ZhenFen::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = std::min(stackNum, m_stackNumFixed);
        SubEffectAdd(JX3DPS_PLAYER, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    } else { // 已有buff,刷新层数
        stackNum  = std::min(stackNum, m_stackNumFixed);
        int stack = stackNum - m_targetSnapshots[JX3DPS_PLAYER].stackNum;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = stackNum;
        SubEffectAdd(JX3DPS_PLAYER, stack);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::ZhenFen::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    m_targetSnapshots[JX3DPS_PLAYER].stackNum -= stackNum;
    if (m_targetSnapshots[JX3DPS_PLAYER].stackNum == 0) {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
    SubEffectClear(JX3DPS_PLAYER, stackNum);
}

void JX3DPS::Buff3rd::ZhenFen::SubEffectAdd(int targetId, int stackNum)
{
    m_player->attr->AddPhysicsOvercomeBase(stackNum * 60);
    m_player->attr->AddMagicOvercomeBase(stackNum * 60);
}

void JX3DPS::Buff3rd::ZhenFen::SubEffectClear(int targetId, int stackNum)
{
    m_player->attr->AddPhysicsOvercomeBase(-stackNum * 60);
    m_player->attr->AddMagicOvercomeBase(-stackNum * 60);
}

JX3DPS::Buff3rd::HanXiaoQianJun::HanXiaoQianJun(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_HAN_XIAO_QIAN_JUN;
    m_name = "3rd·寒啸千军";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 15;
}

void JX3DPS::Buff3rd::HanXiaoQianJun::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::HanXiaoQianJun::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        SubEffectAdd(JX3DPS_PLAYER);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::HanXiaoQianJun::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::HanXiaoQianJun::SubEffectAdd(int targetId)
{
    m_player->attr->AddPhysicsOvercomeBasePercentInt(256);
    m_player->attr->AddMagicOvercomeBasePercentInt(256);
}

void JX3DPS::Buff3rd::HanXiaoQianJun::SubEffectClear(int targetId)
{
    m_player->attr->AddPhysicsOvercomeBasePercentInt(-256);
    m_player->attr->AddMagicOvercomeBasePercentInt(-256);
}

JX3DPS::Buff3rd::ShuKuang::ShuKuang(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_SHU_KUANG;
    m_name = "3rd·疏狂";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 12;
}

void JX3DPS::Buff3rd::ShuKuang::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::ShuKuang::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        SubEffectAdd(JX3DPS_PLAYER);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::ShuKuang::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::ShuKuang::SubEffectAdd(int targetId)
{
    m_player->attr->AddPhysicsAttackBasePercentInt(307);
}

void JX3DPS::Buff3rd::ShuKuang::SubEffectClear(int targetId)
{
    m_player->attr->AddPhysicsAttackBasePercentInt(-307);
}

JX3DPS::Buff3rd::HanChangLinLi::HanChangLinLi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_HAN_CHANG_LIN_LI;
    m_name = "3rd·酣畅淋漓";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 5;
}

void JX3DPS::Buff3rd::HanChangLinLi::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::HanChangLinLi::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        SubEffectAdd(JX3DPS_PLAYER);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::HanChangLinLi::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::HanChangLinLi::SubEffectAdd(int targetId)
{
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(0.1);
}

void JX3DPS::Buff3rd::HanChangLinLi::SubEffectClear(int targetId)
{
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(-0.1);
}

JX3DPS::Buff3rd::JiLei::JiLei(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::THIRD_JI_LEI;
    m_name = "3rd·激雷";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 6;
}

void JX3DPS::Buff3rd::JiLei::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::JiLei::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        SubEffectAdd(JX3DPS_PLAYER);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::JiLei::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(JX3DPS_PLAYER);
}

void JX3DPS::Buff3rd::JiLei::SubEffectAdd(int targetId)
{
    m_player->attr->AddPhysicsOvercomeBasePercentInt(205);
    m_player->attr->AddPhysicsAttackBasePercentInt(205);
}

void JX3DPS::Buff3rd::JiLei::SubEffectClear(int targetId)
{
    m_player->attr->AddPhysicsOvercomeBasePercentInt(-205);
    m_player->attr->AddPhysicsAttackBasePercentInt(-205);
}