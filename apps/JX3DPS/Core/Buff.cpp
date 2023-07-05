/**
 * Project: JX3DPS
 * File: Buff.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-05 10:53:04
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
    m_damageStats                        = other.m_damageStats;
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
    m_damageStats                        = other.m_damageStats;
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

Damage Buff::CalcPhysicsDamage(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount) const
{
    PctFloat_t surplusDamagePercent =
        (JX3_PCT_INT_BASE + m_player->damageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE * // 心法加成
        (JX3_PCT_FLOAT_BASE + m_player->attr->GetPhysicsOvercomePercent()) *       // 破防加成
        (JX3_PCT_FLOAT_BASE + (*m_targets)[targetId]->GetDamageAddPercent()) *     // 易伤加成
        (JX3_PCT_FLOAT_BASE - (*m_targets)[targetId]->GetPhysicsResistPercent()) * // 忽视加成
        (JX3_PCT_FLOAT_BASE + m_player->attr->GetStrainPercent()) *                // 无双加成
        (JX3_PCT_FLOAT_BASE +
         ((*m_targets)[targetId]->GetLevel() - JX3_PLAYER_LEVEL) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成
    PctFloat_t damagePercent =
        effectCount * surplusDamagePercent * (JX3_PCT_INT_BASE + m_skillDamageAddPercentInt + m_player->skillDamageAddPercentInt) / JX3_PCT_INT_BASE;

    int fixedDamage = static_cast<int>(m_damageParams.at(sub)[level].fixedDamage * damagePercent);
    int weaponDamage =
        static_cast<int>((m_damageParams.at(sub)[level].weaponDamagePercentInt * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    int attackBaseDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetPhysicsAttackFromBase() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM;
    int attackMainDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetPhysicsAttackFromMain() * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM;
    int surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                         m_player->attr->GetSurplusDamage() * surplusDamagePercent);

    return Damage(
        GET_FINAL_DAMAGE(fixedDamage, rollResult, (m_player->attr->GetPhysicsCriticalStrikePowerPercent() + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(weaponDamage, rollResult, (m_player->attr->GetPhysicsCriticalStrikePowerPercent() + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(attackBaseDamage, rollResult, (m_player->attr->GetPhysicsCriticalStrikePowerPercent() + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(attackMainDamage, rollResult, (m_player->attr->GetPhysicsCriticalStrikePowerPercent() + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(surplusDamage, rollResult, m_player->attr->GetPhysicsCriticalStrikePowerPercent()));
}

Damage Buff::CalcMagicDamage(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount) const
{
    PctFloat_t surplusDamagePercent =
        (JX3_PCT_INT_BASE + m_player->damageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE * // 心法加成
        (JX3_PCT_FLOAT_BASE + m_player->attr->GetMagicOvercomePercent()) *       // 破防加成
        (JX3_PCT_FLOAT_BASE + (*m_targets)[targetId]->GetDamageAddPercent()) *   // 易伤加成
        (JX3_PCT_FLOAT_BASE - (*m_targets)[targetId]->GetMagicResistPercent()) * // 忽视加成
        (JX3_PCT_FLOAT_BASE + m_player->attr->GetStrainPercent()) *              // 无双加成
        (JX3_PCT_FLOAT_BASE +
         ((*m_targets)[targetId]->GetLevel() - JX3_PLAYER_LEVEL) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION); // 等级差加成

    PctFloat_t damagePercent =
        effectCount * surplusDamagePercent * ((JX3_PCT_INT_BASE + m_skillDamageAddPercentInt + m_player->skillDamageAddPercentInt) / JX3_PCT_INT_BASE);

    int fixedDamage = static_cast<int>(m_damageParams.at(sub)[level].fixedDamage * damagePercent);
    int weaponDamage =
        static_cast<int>((m_damageParams.at(sub)[level].weaponDamagePercentInt * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    int attackBaseDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetMagicAttackFromBase() * damagePercent) /
        JX3_MAGIC_DAMAGE_PARAM;
    int attackMainDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_player->attr->GetMagicAttackFromMain() * damagePercent) /
        JX3_MAGIC_DAMAGE_PARAM;
    int surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                         m_player->attr->GetSurplusDamage() * surplusDamagePercent);

    return Damage(
        GET_FINAL_DAMAGE(fixedDamage, rollResult, (m_player->attr->GetMagicCriticalStrikePowerPercent() + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(weaponDamage, rollResult, (m_player->attr->GetMagicCriticalStrikePowerPercent() + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(attackBaseDamage, rollResult, (m_player->attr->GetMagicCriticalStrikePowerPercent() + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(attackMainDamage, rollResult, (m_player->attr->GetMagicCriticalStrikePowerPercent() + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(surplusDamage, rollResult, m_player->attr->GetMagicCriticalStrikePowerPercent()));
}

RollResult Buff::GetPhysicsRollResultDot(Id_t targetId) const
{
    return RandomUniform(0.0, 1.0) < m_targetSnapshots.at(targetId).criticalStrikePercent + m_skillCriticalStrikeAddPercent

               ? RollResult::DOUBLE
               : RollResult::HIT;
}

RollResult Buff::GetMagicRollResultDot(Id_t targetId) const
{
    return RandomUniform(0.0, 1.0) < m_targetSnapshots.at(targetId).criticalStrikePercent + m_skillCriticalStrikeAddPercent
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

Damage Buff::CalcPhysicsDamageDot(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount) const
{
    PctFloat_t surplusDamagePercent =
        (JX3_PCT_INT_BASE + m_player->damageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE * // 心法加成
        (JX3_PCT_FLOAT_BASE + m_player->attr->GetPhysicsOvercomePercent()) *       // 破防加成
        (JX3_PCT_FLOAT_BASE + (*m_targets)[targetId]->GetDamageAddPercent()) *     // 易伤加成
        (JX3_PCT_FLOAT_BASE - (*m_targets)[targetId]->GetPhysicsResistPercent()) * // 忽视加成
        (JX3_PCT_FLOAT_BASE + m_targetSnapshots.at(targetId).strainPercent) *      // 无双加成
        ((*m_targets)[targetId]->GetLevel() - JX3_PLAYER_LEVEL) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION; // 等级差加成
    PctFloat_t damagePercent =
        effectCount * surplusDamagePercent * (JX3_PCT_INT_BASE + m_targetSnapshots.at(targetId).skillDamageAddPercentInt) / JX3_PCT_INT_BASE;

    int fixedDamage = static_cast<int>(m_damageParams.at(sub)[level].fixedDamage * damagePercent);
    int weaponDamage =
        static_cast<int>((m_damageParams.at(sub)[level].weaponDamagePercentInt * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    int attackBaseDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_targetSnapshots.at(targetId).attackBase * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM / JX3_DOT_SKILL_DAMAGE_CONST;
    int attackMainDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_targetSnapshots.at(targetId).attackMain * damagePercent) /
        JX3_PHYSICS_DAMAGE_PARAM / JX3_DOT_SKILL_DAMAGE_CONST;
    int surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                         m_player->attr->GetSurplusDamage() * surplusDamagePercent);

    PctFloat_t criticalStrikePowerPercent = m_targetSnapshots.at(targetId).criticalStrikePowerPercent;

    return Damage(
        GET_FINAL_DAMAGE(fixedDamage, rollResult, (criticalStrikePowerPercent + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(weaponDamage, rollResult, (criticalStrikePowerPercent + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(attackBaseDamage, rollResult, (criticalStrikePowerPercent + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(attackMainDamage, rollResult, (criticalStrikePowerPercent + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(surplusDamage, rollResult, criticalStrikePowerPercent));
}

Damage Buff::CalcMagicDamageDot(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount) const
{

    PctFloat_t surplusDamagePercent =
        (JX3_PCT_INT_BASE + m_player->damageAddPercentInt) * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE * // 心法加成
        (JX3_PCT_FLOAT_BASE + m_player->attr->GetMagicOvercomePercent()) *       // 破防加成
        (JX3_PCT_FLOAT_BASE + (*m_targets)[targetId]->GetDamageAddPercent()) *   // 易伤加成
        (JX3_PCT_FLOAT_BASE - (*m_targets)[targetId]->GetMagicResistPercent()) * // 忽视加成
        (JX3_PCT_FLOAT_BASE + m_targetSnapshots.at(targetId).strainPercent) *    // 无双加成
        ((*m_targets)[targetId]->GetLevel() - JX3_PLAYER_LEVEL) * JX3_HIGH_LEVEL_DAMAGE_REDUCTION; // 等级差加成

    PctFloat_t damagePercent =
        effectCount * surplusDamagePercent * (JX3_PCT_INT_BASE + m_targetSnapshots.at(targetId).skillDamageAddPercentInt) / JX3_PCT_INT_BASE;

    int fixedDamage = static_cast<int>(m_damageParams.at(sub)[level].fixedDamage * damagePercent);
    int weaponDamage =
        static_cast<int>((m_damageParams.at(sub)[level].weaponDamagePercentInt * JX3_PCT_FLOAT_BASE / JX3_PCT_INT_BASE) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    int attackBaseDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_targetSnapshots.at(targetId).attackBase * damagePercent) /
        JX3_MAGIC_DAMAGE_PARAM / JX3_DOT_SKILL_DAMAGE_CONST;
    int attackMainDamage =
        static_cast<int>(m_damageParams.at(sub)[level].attackDamagePercent *
                         m_targetSnapshots.at(targetId).attackMain * damagePercent) /
        JX3_MAGIC_DAMAGE_PARAM / JX3_DOT_SKILL_DAMAGE_CONST;
    int surplusDamage = static_cast<int>(m_damageParams.at(sub)[level].surplusDamagePercent *
                                         m_player->attr->GetSurplusDamage() * surplusDamagePercent);

    PctFloat_t criticalStrikePowerPercent = m_targetSnapshots.at(targetId).criticalStrikePowerPercent;

    return Damage(
        GET_FINAL_DAMAGE(fixedDamage, rollResult, (criticalStrikePowerPercent + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(weaponDamage, rollResult, (criticalStrikePowerPercent + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(attackBaseDamage, rollResult, (criticalStrikePowerPercent + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(attackMainDamage, rollResult, (criticalStrikePowerPercent + m_skillCriticalStrikePowerAddPercent)),
        GET_FINAL_DAMAGE(surplusDamage, rollResult, criticalStrikePowerPercent));
}

void Buff::Record(Id_t targetId, RollResult rollResult, const Damage &damage, int sub, int level)
{
    m_damageStats[targetId][m_id][sub][level][rollResult].first++;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.fixedDamage += damage.fixedDamage;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.weaponDamage += damage.weaponDamage;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.attackBaseDamage += damage.attackBaseDamage;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.attackMainDamage += damage.attackMainDamage;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.surplusDamage += damage.surplusDamage;
}

DamageStats &Buff::GetDamageStats()
{
    return m_damageStats;
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
    RollResult rollResult = GetPhysicsRollResult();
    Damage     damage     = CalcPhysicsDamage(m_player->targetId, rollResult, 0, 0);
    Record(m_player->targetId, rollResult, damage, 0, 0);
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
    RollResult rollResult = GetPhysicsRollResult();
    Damage     damage     = CalcPhysicsDamage(m_player->targetId, rollResult, 0, 0);
    Record(m_player->targetId, rollResult, damage, 0, 0);
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
        m_70 = true;
        m_player->skillDamageAddPercentInt += 51;
    } else {
        m_player->skillDamageAddPercentInt += 10;
    }
}

void JX3DPS::Buff3rd::EnchantBelt::SubEffectClear()
{
    if (m_70 = true) {
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
