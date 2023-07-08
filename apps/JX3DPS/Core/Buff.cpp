/**
 * Project: JX3DPS
 * File: Buff.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-08 06:58:02
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Buff.h"

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
         ((*m_targets)[targetId]->GetLevel() - JX3_PLAYER_LEVEL) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成
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
         ((*m_targets)[targetId]->GetLevel() - JX3_PLAYER_LEVEL) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成
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
         ((*m_targets)[targetId]->GetLevel() - JX3_PLAYER_LEVEL) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成
    PctFloat_t damagePercent =
        effectCount * surplusDamageAddPercent *
        (JX3_PCT_INT_BASE + m_targetSnapshots.at(targetId).skillDamageAddPercentInt) / JX3_PCT_INT_BASE; // 武学加成

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
        GetDamage(damage, rollResult, damageAddOvercome, JX3_PCT_FLOAT_BASE + m_targetSnapshots.at(targetId).strainGainPercent, physicsCriticalStrikePowerPercent, 0.0);

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
         ((*m_targets)[targetId]->GetLevel() - JX3_PLAYER_LEVEL) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成
    PctFloat_t damagePercent =
        effectCount * surplusDamageAddPercent *
        (JX3_PCT_INT_BASE + m_targetSnapshots.at(targetId).skillDamageAddPercentInt) / JX3_PCT_INT_BASE;

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
        GetDamage(damage, rollResult, damageAddOvercome, JX3_PCT_FLOAT_BASE + m_targetSnapshots.at(targetId).strainGainPercent, magicCriticalStrikePowerPercent, 0.0);

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
    if (m_targetSnapshots[JX3DPS_PLAYER].interval == JX3DPS_INVALID_FRAMES_SET && RandomUniform(1, 100) <= 10)
    {
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
    m_name = "皎素";

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
