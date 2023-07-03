/**
 * Project: JX3DPS
 * File: Skill.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-03 04:59:35
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Skill.h"

#include <spdlog/spdlog.h>

#include <Utils/Utils.h>

#include "Optimization.h"
#include "Player.h"

namespace JX3DPS {

Skill::Skill(Player *player, Targets *targets)
{
    this->m_player   = player;
    this->m_targets  = targets;
    m_globalCooldown = &(player->globalCooldown);
}

Skill::~Skill() { }

Skill::Skill(const Skill &other)
{
    m_id                                 = other.m_id;
    m_name                               = other.m_name;
    m_subNames                           = other.m_subNames;
    m_levelNames                         = other.m_levelNames;
    m_cooldown                           = other.m_cooldown;
    m_cooldownFixed                      = other.m_cooldownFixed;
    m_prepareFrames                      = other.m_prepareFrames;
    m_prepareFramesFixed                 = other.m_prepareFramesFixed;
    m_energyCount                        = other.m_energyCount;
    m_energyCountFixed                   = other.m_energyCountFixed;
    m_targetCountFixed                   = other.m_targetCountFixed;
    m_range                              = other.m_range;
    m_skillCriticalStrikeAddPercent      = other.m_skillCriticalStrikeAddPercent;
    m_skillCriticalStrikePowerAddPercent = other.m_skillCriticalStrikePowerAddPercent;
    m_skillDamageAddPercentInt           = other.m_skillDamageAddPercentInt;
    m_skillQidianAdd                     = other.m_skillQidianAdd;
    m_damageParams                       = other.m_damageParams;
    m_damageStats                        = other.m_damageStats;

    if (other.m_globalCooldown == &(other.m_cooldown)) {
        m_globalCooldown = &(m_cooldown);
    }
}

Skill &Skill::operator=(const Skill &other)
{
    if (this == &other) {
        return *this;
    }

    m_id                                 = other.m_id;
    m_name                               = other.m_name;
    m_subNames                           = other.m_subNames;
    m_levelNames                         = other.m_levelNames;
    m_cooldown                           = other.m_cooldown;
    m_cooldownFixed                      = other.m_cooldownFixed;
    m_prepareFrames                      = other.m_prepareFrames;
    m_prepareFramesFixed                 = other.m_prepareFramesFixed;
    m_energyCount                        = other.m_energyCount;
    m_energyCountFixed                   = other.m_energyCountFixed;
    m_targetCountFixed                   = other.m_targetCountFixed;
    m_range                              = other.m_range;
    m_skillCriticalStrikeAddPercent      = other.m_skillCriticalStrikeAddPercent;
    m_skillCriticalStrikePowerAddPercent = other.m_skillCriticalStrikePowerAddPercent;
    m_skillDamageAddPercentInt           = other.m_skillDamageAddPercentInt;
    m_skillQidianAdd                     = other.m_skillQidianAdd;
    m_damageParams                       = other.m_damageParams;
    m_damageStats                        = other.m_damageStats;

    if (other.m_globalCooldown == &(other.m_cooldown)) {
        m_globalCooldown = &(m_cooldown);
    }

    return *this;
}

void Skill::SetPlayer(Player *player)
{
    m_player = player;
    if (m_globalCooldown != &(m_cooldown)) {
        m_globalCooldown = &(m_player->globalCooldown);
    }
}

void Skill::SetTargets(Targets *targets)
{
    m_targets = targets;
}

Frame_t Skill::GetCooldown() const
{
    if (m_energyCount > 0) {
        return 0;
    }
    return m_cooldown > *m_globalCooldown ? m_cooldown : *m_globalCooldown;
}

float Skill::GetRange() const
{
    return m_range;
}

int Skill::GetEnergyCount() const
{
    return m_energyCount;
}

DamageStats &Skill::GetDamageStats()
{
    return m_damageStats;
}

const std::string &Skill::GetName() const
{
    return m_name;
}

bool Skill::IsEnergy() const
{
    if (m_energyCount == -1) {
        return false;
    } else {
        return m_cooldown == 0;
    }
}

Frame_t Skill::GetNextKeyFrame() const
{
    if (m_energyCount > 0) {
        return 0;
    }
    Frame_t frame = std::max(m_cooldown, *m_globalCooldown);
    if (m_prepareFrames != JX3DPS_INVALID_FRAMES_SET) { // 在蓄力时间才进行关键帧比较
        frame = std::min(frame, m_prepareFrames);
    }
    return frame;
}

void Skill::UpdateKeyFrame(Frame_t frame)
{
    m_cooldown -= frame;
    m_cooldown  = m_cooldown < 0 ? 0 : m_cooldown;
    if (m_prepareFrames != JX3DPS_INVALID_FRAMES_SET) { // 在蓄力时间才进行关键帧刷新
        m_prepareFrames -= frame;
    }
}

RollResult Skill::GetPhysicsRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attr->GetPhysicsCriticalStrikePercent() + m_skillCriticalStrikeAddPercent
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

RollResult Skill::GetMagicRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attr->GetMagicCriticalStrikePercent() + m_skillCriticalStrikeAddPercent
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

Damage Skill::CalcPhysicsDamage(Id_t targetId, RollResult rollResult, int sub, int level) const
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
        surplusDamagePercent * (JX3_PCT_INT_BASE + m_skillDamageAddPercentInt) / JX3_PCT_INT_BASE;

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

Damage Skill::CalcMagicDamage(Id_t targetId, RollResult rollResult, int sub, int level) const
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
        ((JX3_PCT_INT_BASE + m_skillDamageAddPercentInt) / JX3_PCT_INT_BASE) * surplusDamagePercent;

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

void Skill::Record(Id_t targetId, RollResult rollResult, const Damage &damage, int sub, int level)
{
    m_damageStats[targetId][m_id][sub][level][rollResult].first++;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.fixedDamage += damage.fixedDamage;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.weaponDamage += damage.weaponDamage;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.attackBaseDamage += damage.attackBaseDamage;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.attackMainDamage += damage.attackMainDamage;
    m_damageStats[targetId][m_id][sub][level][rollResult].second.surplusDamage += damage.surplusDamage;
}

} // namespace JX3DPS