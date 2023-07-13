/**
 * Project: JX3DPS
 * File: Skill.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-12 09:18:55
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Skill.h"

#include <spdlog/spdlog.h>

#include <Utils/Utils.h>

#include "Damage.h"
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
    m_id                                     = other.m_id;
    m_name                                   = other.m_name;
    m_subNames                               = other.m_subNames;
    m_levelNames                             = other.m_levelNames;
    m_cooldown                               = other.m_cooldown;
    m_cooldownFixed                          = other.m_cooldownFixed;
    m_prepareFrames                          = other.m_prepareFrames;
    m_prepareFramesFixed                     = other.m_prepareFramesFixed;
    m_energyCount                            = other.m_energyCount;
    m_energyCountFixed                       = other.m_energyCountFixed;
    m_targetCountFixed                       = other.m_targetCountFixed;
    m_range                                  = other.m_range;
    m_effectCriticalStrikeAddPercentInt      = other.m_effectCriticalStrikeAddPercentInt;
    m_effectCriticalStrikePowerAddPercentInt = other.m_effectCriticalStrikePowerAddPercentInt;
    m_effectDamageAddPercentInt              = other.m_effectDamageAddPercentInt;
    m_skillQidianAdd                         = other.m_skillQidianAdd;
    m_damageParams                           = other.m_damageParams;
    m_stats                                  = other.m_stats;

    if (other.m_globalCooldown == &(other.m_cooldown)) {
        m_globalCooldown = &(m_cooldown);
    }
}

Skill &Skill::operator=(const Skill &other)
{
    if (this == &other) {
        return *this;
    }

    m_id                                     = other.m_id;
    m_name                                   = other.m_name;
    m_subNames                               = other.m_subNames;
    m_levelNames                             = other.m_levelNames;
    m_cooldown                               = other.m_cooldown;
    m_cooldownFixed                          = other.m_cooldownFixed;
    m_prepareFrames                          = other.m_prepareFrames;
    m_prepareFramesFixed                     = other.m_prepareFramesFixed;
    m_energyCount                            = other.m_energyCount;
    m_energyCountFixed                       = other.m_energyCountFixed;
    m_targetCountFixed                       = other.m_targetCountFixed;
    m_range                                  = other.m_range;
    m_effectCriticalStrikeAddPercentInt      = other.m_effectCriticalStrikeAddPercentInt;
    m_effectCriticalStrikePowerAddPercentInt = other.m_effectCriticalStrikePowerAddPercentInt;
    m_effectDamageAddPercentInt              = other.m_effectDamageAddPercentInt;
    m_skillQidianAdd                         = other.m_skillQidianAdd;
    m_damageParams                           = other.m_damageParams;
    m_stats                                  = other.m_stats;

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

void Skill::SetCooldown(Frame_t frame)
{
    m_cooldown = frame;
}

float Skill::GetRange() const
{
    return m_range;
}

int Skill::GetEnergyCount() const
{
    return m_energyCount;
}

Stats &Skill::GetStats()
{
    return m_stats;
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
    return RandomUniform(0.0, 1.0) < m_player->attr->GetPhysicsCriticalStrikePercent() +
                                         m_effectCriticalStrikeAddPercentInt / JX3_CRITICAL_STRIKE_PERCENT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

RollResult Skill::GetMagicRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attr->GetMagicCriticalStrikePercent() +
                                         m_effectCriticalStrikeAddPercentInt / JX3_CRITICAL_STRIKE_PERCENT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

Damage Skill::GetPhysicsDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    Value_t    attack,
    Value_t    weaponDamage,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain,
    Value_t    surplus)
{
    Damage damage;

    PctFloat_t physicsDamageCoefficient =
        PhysicsDamageCoefficient(m_damageParams.at(sub)[level].attackDamagePercent, 0);
    PctInt_t weaponDamageCoefficientInt = m_damageParams.at(sub)[level].weaponDamagePercentInt;
    Value_t  fixedDamage                = m_damageParams.at(sub)[level].fixedDamage;
    PctInt_t effectDamageAddPercentInt = m_effectDamageAddPercentInt + m_player->skillDamageAddPercentInt;
    Value_t effectDamage =
        EffectDamageAll(attack, physicsDamageCoefficient, weaponDamage, weaponDamageCoefficientInt, fixedDamage, effectDamageAddPercentInt);

    PctInt_t surplusCoefficientInt = m_damageParams.at(sub)[level].surplusDamagePercent;
    Value_t  surplusDamage = SurplusDamage(surplus, surplusCoefficientInt, JX3_PLAYER_LEVEL);

    int      playerLevel                = JX3_PLAYER_LEVEL;
    int      targetLevel                = (*m_targets)[targetId]->GetLevel();
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetPhysicsShield();
    Value_t  shieldAdd                  = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attr->GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldPercentInt     = 0;
    int      rollResultInt              = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePower =
        (m_effectCriticalStrikePowerAddPercentInt + m_player->attr->GetPhysicsCriticalStrikePercentIntFromCustom());
    PctInt_t strainPercentInt         = 0;
    PctInt_t classDamageAddPercentInt = m_player->damageAddPercentInt;
    PctInt_t vulnerablePercentInt     = (*m_targets)[targetId]->GetDamageAddPercentInt();

    damage.damage = FinalPhysicsDamage(
        playerLevel,
        targetLevel,
        effectDamage,
        shieldBase,
        shieldAdd,
        ignoreShieldBasePercentInt,
        ignoreShieldPercentInt,
        overcome,
        rollResultInt,
        criticalStrikePower,
        effectCriticalStrikePower,
        strain,
        strainPercentInt,
        classDamageAddPercentInt,
        vulnerablePercentInt);

    damage.surplusDamage = FinalPhysicsDamage(
        playerLevel,
        targetLevel,
        surplusDamage,
        shieldBase,
        shieldAdd,
        ignoreShieldBasePercentInt,
        ignoreShieldPercentInt,
        overcome,
        rollResultInt,
        criticalStrikePower,
        effectCriticalStrikePower,
        strain,
        strainPercentInt,
        classDamageAddPercentInt,
        vulnerablePercentInt);

    return damage;
}

GainsDamage Skill::CalcPhysicsDamage(Id_t targetId, RollResult rollResult, int sub, int level)
{
    GainsDamage gainsDamage;
    gainsDamage.normalDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain(),
        m_player->attr->GetSurplus());

    // 攻击收益
    m_player->attr->AddPhysicsAttackBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_ATTACK)]);

    gainsDamage.attackDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain(),
        m_player->attr->GetSurplus());

    m_player->attr->AddPhysicsAttackBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_ATTACK)]);

    // 会效收益
    m_player->attr->AddPhysicsCriticalStrikePower(
        ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_CRITICAL_STRIKE_POWER)]);

    gainsDamage.criticalStrikePowerDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain(),
        m_player->attr->GetSurplus());

    m_player->attr->AddPhysicsCriticalStrikePower(
        -ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_CRITICAL_STRIKE_POWER)]);

    // 破防收益
    m_player->attr->AddPhysicsOvercomeBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);

    gainsDamage.overcomeDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain(),
        m_player->attr->GetSurplus());

    m_player->attr->AddPhysicsOvercomeBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);

    // 无双收益
    m_player->attr->AddStrain(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);

    gainsDamage.strainDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain(),
        m_player->attr->GetSurplus());

    m_player->attr->AddStrain(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);

    // 破招收益
    m_player->attr->AddSurplusBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);

    gainsDamage.surplusDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain(),
        m_player->attr->GetSurplus());

    m_player->attr->AddSurplusBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::SURPLUS)]);

    // 武器伤害收益
    m_player->attr->AddWeaponAttack(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::WEAPON_ATTACK)]);

    gainsDamage.weaponDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain(),
        m_player->attr->GetSurplus());

    m_player->attr->AddWeaponAttack(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::WEAPON_ATTACK)]);

    return gainsDamage;
}

GainsDamage Skill::CalcMagicDamage(Id_t targetId, RollResult rollResult, int sub, int level)
{

    GainsDamage gainsDamage;

    return gainsDamage;
}

void Skill::Record(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub, int level)
{
    m_stats.damageStats[targetId][m_id][sub][level][rollResult].first++;
    m_stats.damageStats[targetId][m_id][sub][level][rollResult].second.damage +=
        gainsDamage.normalDamage.damage;
    m_stats.damageStats[targetId][m_id][sub][level][rollResult].second.surplusDamage +=
        gainsDamage.normalDamage.surplusDamage;

    m_stats.normalDamage              += gainsDamage.normalDamage.SumDamage();
    m_stats.attackDamage              += gainsDamage.attackDamage.SumDamage();
    m_stats.criticalStrikePowerDamage += gainsDamage.criticalStrikePowerDamage.SumDamage();
    m_stats.overcomeDamage            += gainsDamage.overcomeDamage.SumDamage();
    m_stats.strainDamage              += gainsDamage.strainDamage.SumDamage();
    m_stats.surplusDamage             += gainsDamage.surplusDamage.SumDamage();
    m_stats.weaponDamage              += gainsDamage.weaponDamage.SumDamage();
}

} // namespace JX3DPS