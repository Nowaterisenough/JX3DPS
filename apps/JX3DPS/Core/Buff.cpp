/**
 * Project: JX3DPS
 * File: Buff.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-18 10:40:01
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Buff.h"

#include <spdlog/spdlog.h>

#include <Utils/Utils.h>

#include "Damage.h"
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
    m_id                                     = other.m_id;
    m_name                                   = other.m_name;
    m_range                                  = other.m_range;
    m_tbuff                                  = other.m_tbuff;
    m_subNames                               = other.m_subNames;
    m_levelNames                             = other.m_levelNames;
    m_targetSnapshots                        = other.m_targetSnapshots;
    m_durationFixed                          = other.m_durationFixed;
    m_intervalFixed                          = other.m_intervalFixed;
    m_stackNumFixed                          = other.m_stackNumFixed;
    m_effectCountFixed                       = other.m_effectCountFixed;
    m_damageParams                           = other.m_damageParams;
    m_stats                                  = other.m_stats;
    m_effectCriticalStrikeAddPercentInt      = other.m_effectCriticalStrikeAddPercentInt;
    m_effectCriticalStrikePowerAddPercentInt = other.m_effectCriticalStrikePowerAddPercentInt;
    m_effectDamageAddPercentInt              = other.m_effectDamageAddPercentInt;
}

JX3DPS::Buff &JX3DPS::Buff::operator=(const Buff &other)
{
    if (this == &other) {
        return *this;
    }

    m_id                                     = other.m_id;
    m_name                                   = other.m_name;
    m_range                                  = other.m_range;
    m_tbuff                                  = other.m_tbuff;
    m_subNames                               = other.m_subNames;
    m_levelNames                             = other.m_levelNames;
    m_targetSnapshots                        = other.m_targetSnapshots;
    m_durationFixed                          = other.m_durationFixed;
    m_intervalFixed                          = other.m_intervalFixed;
    m_stackNumFixed                          = other.m_stackNumFixed;
    m_effectCountFixed                       = other.m_effectCountFixed;
    m_damageParams                           = other.m_damageParams;
    m_stats                                  = other.m_stats;
    m_effectCriticalStrikeAddPercentInt      = other.m_effectCriticalStrikeAddPercentInt;
    m_effectCriticalStrikePowerAddPercentInt = other.m_effectCriticalStrikePowerAddPercentInt;
    m_effectDamageAddPercentInt              = other.m_effectDamageAddPercentInt;

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
    return RandomUniform(0.0, 1.0) < m_player->attr->GetPhysicsCriticalStrikePercent() +
                                         m_effectCriticalStrikeAddPercentInt / JX3_CRITICAL_STRIKE_PERCENT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

RollResult Buff::GetMagicRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attr->GetMagicCriticalStrikePercent() +
                                         m_effectCriticalStrikeAddPercentInt / JX3_CRITICAL_STRIKE_PERCENT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

Damage Buff::GetPhysicsDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    int        effectCount,
    Value_t    attack,
    Value_t    weaponDamage,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain)
{
    Damage damage;

    PctFloat_t physicsDamageCoefficient =
        PhysicsDamageCoefficient(m_damageParams.at(sub)[level].attackDamagePercent, 0);
    PctInt_t weaponDamageCoefficientInt = m_damageParams.at(sub)[level].weaponDamagePercentInt;
    Value_t  fixedDamage                = m_damageParams.at(sub)[level].fixedDamage;
    PctInt_t effectDamageAddPercentInt = m_effectDamageAddPercentInt + m_player->skillDamageAddPercentInt;
    Value_t effectDamage =
        effectCount *
        EffectDamageAll(attack, physicsDamageCoefficient, weaponDamage, weaponDamageCoefficientInt, fixedDamage, effectDamageAddPercentInt);

    int      playerLevel                = JX3_PLAYER_LEVEL;
    int      targetLevel                = (*m_targets)[targetId]->GetLevel();
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetPhysicsShield();
    Value_t  shieldAdd                  = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attr->GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldPercentInt     = 0;
    int      rollResultInt              = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePower =
        m_effectCriticalStrikePowerAddPercentInt +
        m_player->attr->GetPhysicsCriticalStrikePowerPercentIntFromCustom();
    PctInt_t strainPercentInt         = m_player->attr->GetStrainPercentInt();
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

    return damage;
}

GainsDamage Buff::CalcPhysicsDamage(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount)
{
    GainsDamage gainsDamage;
    gainsDamage.normalDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain());

    // 攻击收益
    m_player->attr->AddPhysicsAttackBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_ATTACK)]);

    gainsDamage.attackDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain());

    m_player->attr->AddPhysicsAttackBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_ATTACK)]);

    // 会效收益
    m_player->attr->AddPhysicsCriticalStrikePower(
        ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_CRITICAL_STRIKE_POWER)]);

    gainsDamage.criticalStrikePowerDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain());

    m_player->attr->AddPhysicsCriticalStrikePower(
        -ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_CRITICAL_STRIKE_POWER)]);

    // 破防收益
    m_player->attr->AddPhysicsOvercomeBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);

    gainsDamage.overcomeDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain());

    m_player->attr->AddPhysicsOvercomeBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);

    // 无双收益
    m_player->attr->AddStrain(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);

    gainsDamage.strainDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain());

    m_player->attr->AddStrain(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);

    // 破招收益
    gainsDamage.surplusDamage = gainsDamage.normalDamage;

    // 武器伤害收益
    m_player->attr->AddWeaponAttack(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::WEAPON_ATTACK)]);

    gainsDamage.weaponDamage = GetPhysicsDamage(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_player->attr->GetPhysicsAttack(),
        m_player->attr->GetWeaponAttack(),
        m_player->attr->GetPhysicsCriticalStrikePower(),
        m_player->attr->GetPhysicsOvercome(),
        m_player->attr->GetStrain());

    m_player->attr->AddWeaponAttack(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::WEAPON_ATTACK)]);

    return gainsDamage;
}

GainsDamage Buff::CalcMagicDamage(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount)
{

    GainsDamage gainsDamage;

    return gainsDamage;
}

RollResult Buff::GetRollResultDot(Id_t targetId) const
{
    return RandomUniform(0.0, 1.0) < m_targetSnapshots.at(targetId).criticalStrikePercent +
                                         m_targetSnapshots.at(targetId).effectCriticalStrikeAddPercentInt / JX3_CRITICAL_STRIKE_PERCENT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

Damage Buff::GetPhysicsDamageDot(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    int        effectCount,
    Value_t    attack,
    Value_t    weaponDamage,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain)
{
    Damage damage;

    PctFloat_t physicsDamageCoefficient =
        PhysicsDotDamageCoefficient(m_damageParams.at(sub)[level].attackDamagePercent, m_effectCountFixed, m_intervalFixed);
    PctInt_t weaponDamageCoefficientInt = m_damageParams.at(sub)[level].weaponDamagePercentInt;
    Value_t  fixedDamage                = m_damageParams.at(sub)[level].fixedDamage;
    PctInt_t effectDamageAddPercentInt  = m_targetSnapshots.at(targetId).effectDamageAddPercentInt;
    Value_t  effectDamage =
        effectCount *
        EffectDamageAll(attack, physicsDamageCoefficient, weaponDamage, weaponDamageCoefficientInt, fixedDamage, effectDamageAddPercentInt);

    int      playerLevel                = JX3_PLAYER_LEVEL;
    int      targetLevel                = (*m_targets)[targetId]->GetLevel();
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetPhysicsShield();
    Value_t  shieldAdd                  = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attr->GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldPercentInt     = 0;
    int      rollResultInt              = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePower = m_targetSnapshots.at(targetId).effectCriticalStrikePowerAddPercentInt;
    PctInt_t strainPercentInt         = m_targetSnapshots.at(targetId).strainAddPercentInt;
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

    return damage;
}

GainsDamage Buff::CalcPhysicsDamageDot(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount)
{
    GainsDamage gainsDamage;

    gainsDamage.normalDamage = GetPhysicsDamageDot(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_targetSnapshots.at(targetId).attack,
        m_player->attr->GetWeaponAttack(),
        m_targetSnapshots.at(targetId).criticalStrikePower,
        m_player->attr->GetPhysicsOvercome(),
        m_targetSnapshots.at(targetId).strain);

    // 攻击收益
    m_player->attr->AddPhysicsAttackBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_ATTACK)]);

    gainsDamage.attackDamage = GetPhysicsDamageDot(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_targetSnapshots.at(targetId).attackGain,
        m_player->attr->GetWeaponAttack(),
        m_targetSnapshots.at(targetId).criticalStrikePower,
        m_player->attr->GetPhysicsOvercome(),
        m_targetSnapshots.at(targetId).strain);

    m_player->attr->AddPhysicsAttackBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_ATTACK)]);

    // 会效收益
    m_player->attr->AddPhysicsCriticalStrikePower(
        ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_CRITICAL_STRIKE_POWER)]);

    gainsDamage.criticalStrikePowerDamage = GetPhysicsDamageDot(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_targetSnapshots.at(targetId).attack,
        m_player->attr->GetWeaponAttack(),
        m_targetSnapshots.at(targetId).criticalStrikePowerGain,
        m_player->attr->GetPhysicsOvercome(),
        m_targetSnapshots.at(targetId).strain);

    m_player->attr->AddPhysicsCriticalStrikePower(
        -ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_CRITICAL_STRIKE_POWER)]);

    // 破防收益
    m_player->attr->AddPhysicsOvercomeBase(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);

    gainsDamage.overcomeDamage = GetPhysicsDamageDot(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_targetSnapshots.at(targetId).attack,
        m_player->attr->GetWeaponAttack(),
        m_targetSnapshots.at(targetId).criticalStrikePower,
        m_player->attr->GetPhysicsOvercome(),
        m_targetSnapshots.at(targetId).strain);

    m_player->attr->AddPhysicsOvercomeBase(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::PHYSICS_OVERCOME)]);

    // 无双收益
    m_player->attr->AddStrain(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);

    gainsDamage.strainDamage = GetPhysicsDamageDot(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_targetSnapshots.at(targetId).attack,
        m_player->attr->GetWeaponAttack(),
        m_targetSnapshots.at(targetId).criticalStrikePower,
        m_player->attr->GetPhysicsOvercome(),
        m_targetSnapshots.at(targetId).strainGain);

    m_player->attr->AddStrain(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::STRAIN)]);

    // 破招收益
    gainsDamage.surplusDamage = gainsDamage.normalDamage;

    // 武器伤害收益
    m_player->attr->AddWeaponAttack(ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::WEAPON_ATTACK)]);

    gainsDamage.weaponDamage = GetPhysicsDamageDot(
        targetId,
        rollResult,
        sub,
        level,
        effectCount,
        m_targetSnapshots.at(targetId).attack,
        m_player->attr->GetWeaponAttack(),
        m_targetSnapshots.at(targetId).criticalStrikePower,
        m_player->attr->GetPhysicsOvercome(),
        m_targetSnapshots.at(targetId).strain);

    m_player->attr->AddWeaponAttack(-ATTRIBUTE_GAIN_BY_BASE[static_cast<int>(AttributeType::WEAPON_ATTACK)]);

    return gainsDamage;
}

GainsDamage Buff::CalcMagicDamageDot(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount)
{
    GainsDamage gainsDamage;

    return gainsDamage;
}

void Buff::Record(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub, int level)
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

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 60);

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

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 75);

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

    m_damageParams[0].emplace_back(0, 0, 0);

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
    spdlog::debug("添加buff:{}", m_name);
    if (RandomUniform(1, 100) <= 70) {
        m_70                                = true;
        m_player->skillDamageAddPercentInt += 51;
    } else {
        m_player->skillDamageAddPercentInt += 10;
    }
}

void JX3DPS::Buff3rd::EnchantBelt::SubEffectClear()
{
    spdlog::debug("移除buff:{}", m_name);
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

    m_damageParams[0].emplace_back(0, 0, 0);

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
    m_player->attr->AddPhysicsCriticalStrikePowerPercentIntFromCustom(51);
}

void JX3DPS::Buff3rd::JiaoSu::SubEffectClear()
{
    m_player->attr->AddPhysicsCriticalStrikePowerPercentIntFromCustom(51);
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
            
            SubEffectClear(it->first);
            it = m_targetSnapshots.erase(it);
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
            SubEffectClear(it->first);
            it = m_targetSnapshots.erase(it);
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
            SubEffectClear(it->first);
            it = m_targetSnapshots.erase(it);
            
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
    m_player->attr->AddPhysicsCriticalStrikePercentIntFromCustom(1000);
}

void JX3DPS::Buff3rd::HanChangLinLi::SubEffectClear(int targetId)
{
    m_player->attr->AddPhysicsCriticalStrikePercentIntFromCustom(-1000);
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

JX3DPS::Buff3rd::WeaponEffectWater::WeaponEffectWater(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::WEAPON_EFFECT_WATER;
    m_name = "水·斩流";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 6;
    m_stackNumFixed = 10;
}

void JX3DPS::Buff3rd::WeaponEffectWater::Trigger()
{
    int stackNum = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(stackNum);
}

void JX3DPS::Buff3rd::WeaponEffectWater::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = stackNum;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum =
            std::min(m_targetSnapshots[JX3DPS_PLAYER].stackNum, m_stackNumFixed);
        SubEffectAdd(m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    } else {
        int stack                                  = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum += stackNum;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum =
            std::min(m_targetSnapshots[JX3DPS_PLAYER].stackNum, m_stackNumFixed);
        stack = m_targetSnapshots[JX3DPS_PLAYER].stackNum - stack;
        SubEffectAdd(stack);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::Buff3rd::WeaponEffectWater::Clear(Id_t targetId, int stackNum)
{
    int stack = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(stack);
}

void JX3DPS::Buff3rd::WeaponEffectWater::TriggerAdd(int stackNum)
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        m_targetSnapshots[JX3DPS_PLAYER].stackNum = stackNum;
        SubEffectAdd(m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    } else {
        int stack                                  = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum += stackNum;
        m_targetSnapshots[JX3DPS_PLAYER].stackNum =
            std::min(m_targetSnapshots[JX3DPS_PLAYER].stackNum, m_stackNumFixed);
        stack = m_targetSnapshots[JX3DPS_PLAYER].stackNum - stack;
        SubEffectAdd(stack);
    }
    m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
}

void JX3DPS::Buff3rd::WeaponEffectWater::SubEffectAdd(int stackNum)
{
    m_player->attr->AddPhysicsAttackBaseFromCustom(88 * stackNum);
}

void JX3DPS::Buff3rd::WeaponEffectWater::SubEffectClear(int stackNum)
{
    m_player->attr->AddPhysicsAttackBaseFromCustom(-88 * stackNum);
}

JX3DPS::Buff3rd::PendantOvercome::PendantOvercome(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::WEAPON_EFFECT_WATER;
    m_name = "腰坠·破防";
    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_durationFixed = 16 * 15;
}

void JX3DPS::Buff3rd::PendantOvercome::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::Buff3rd::PendantOvercome::Add(Id_t targetId, int stackNum, Frame_t duration)
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

void JX3DPS::Buff3rd::PendantOvercome::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::Buff3rd::PendantOvercome::TriggerAdd()
{
    if (m_targetSnapshots.find(JX3DPS_PLAYER) == m_targetSnapshots.end()) {
        SubEffectAdd();
    }
    m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
}

void JX3DPS::Buff3rd::PendantOvercome::SubEffectAdd()
{
    m_player->attr->AddPhysicsOvercomeBaseFromCustom(6408);
}

void JX3DPS::Buff3rd::PendantOvercome::SubEffectClear()
{
    m_player->attr->AddPhysicsOvercomeBaseFromCustom(-6408);
}