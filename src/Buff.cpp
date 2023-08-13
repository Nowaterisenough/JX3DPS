/**
 * Project: JX3DPS
 * File: Buff.cpp
 * Description:
 * Created Date: 2023-07-22 08:33:14
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-12 22:11:05
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Buff.h"

#include "Damage/Damage.hpp"

#include "Player.h"
#include "Target.hpp"

JX3DPS::Buff::Buff(Player *player, Targets *targets)
{
    this->m_player  = player;
    this->m_targets = targets;
}

JX3DPS::Buff::~Buff() { }

JX3DPS::Buff::Buff(const Buff &other)
{
    m_id           = other.m_id;
    m_name         = other.m_name;
    m_range        = other.m_range;
    m_snapshots    = other.m_snapshots;
    m_duration     = other.m_duration;
    m_interval     = other.m_interval;
    m_stackNum     = other.m_stackNum;
    m_effectCount  = other.m_effectCount;
    m_damageParams = other.m_damageParams;
    m_stats        = other.m_stats;
    m_effectCriticalStrikeAdditionalBasisPointInt = other.m_effectCriticalStrikeAdditionalBasisPointInt;
    m_effectCriticalStrikePowerAdditionalPercentInt = other.m_effectCriticalStrikePowerAdditionalPercentInt;
    m_effectDamageAdditionalPercentInt = other.m_effectDamageAdditionalPercentInt;
    m_effectShieldIgnoreAdditionalPercentInt = other.m_effectShieldIgnoreAdditionalPercentInt;
    m_triggerEffects = other.m_triggerEffects;
}

JX3DPS::Buff &JX3DPS::Buff::operator=(const Buff &other)
{
    if (this == &other) {
        return *this;
    }

    m_id           = other.m_id;
    m_name         = other.m_name;
    m_range        = other.m_range;
    m_snapshots    = other.m_snapshots;
    m_duration     = other.m_duration;
    m_interval     = other.m_interval;
    m_stackNum     = other.m_stackNum;
    m_effectCount  = other.m_effectCount;
    m_damageParams = other.m_damageParams;
    m_stats        = other.m_stats;
    m_effectCriticalStrikeAdditionalBasisPointInt = other.m_effectCriticalStrikeAdditionalBasisPointInt;
    m_effectCriticalStrikePowerAdditionalPercentInt = other.m_effectCriticalStrikePowerAdditionalPercentInt;
    m_effectDamageAdditionalPercentInt = other.m_effectDamageAdditionalPercentInt;
    m_effectShieldIgnoreAdditionalPercentInt = other.m_effectShieldIgnoreAdditionalPercentInt;
    m_triggerEffects = other.m_triggerEffects;
    return *this;
}

void JX3DPS::Buff::SetPlayer(Player *player)
{
    m_player = player;
}

void JX3DPS::Buff::SetTargets(Targets *targets)
{
    m_targets = targets;
}

void JX3DPS::Buff::Remove(Id_t targetId)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) {
        return;
    }
    m_snapshots.erase(targetId);
}

JX3DPS::Frame_t JX3DPS::Buff::GetNextKeyFrame() const
{
    Frame_t next = JX3DPS_INVALID_FRAMES_SET;
    for (auto &snapshot : m_snapshots) {
        next = std::min(next, snapshot.second.interval);
        next = std::min(next, snapshot.second.duration);
    }
    return next;
}

void JX3DPS::Buff::UpdateKeyFrame(Frame_t frame)
{
    for (auto &snapshot : m_snapshots) {
        if (snapshot.second.duration != JX3DPS_INVALID_FRAMES_SET) {
            snapshot.second.duration -= frame;
        }
        if (snapshot.second.interval != JX3DPS_INVALID_FRAMES_SET) {
            snapshot.second.interval -= frame;
        }
    }
    m_cooldownCurrent -= frame;
}

JX3DPS::Frame_t JX3DPS::Buff::GetDurationCurrent(Id_t targetId) const
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) {
        return 0;
    }
    return m_snapshots.at(targetId).duration;
}

int JX3DPS::Buff::GetStackNumCurrent(Id_t targetId) const
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) {
        return 0;
    }
    return m_snapshots.at(targetId).stackNum;
}

double JX3DPS::Buff::GetRange() const
{
    return m_range;
}

JX3DPS::RollResult JX3DPS::Buff::GetPhysicsRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attribute.GetPhysicsCriticalStrikePercent() +
                                         m_effectCriticalStrikeAdditionalBasisPointInt / JX3_BASIS_POINT_INT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

JX3DPS::Damage JX3DPS::Buff::GetPhysicsDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    Value_t    attack,
    Value_t    weaponDamage,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain)
{
    Damage damage;

    PctFloat_t physicsDamageCoefficient =
        PhysicsDamageCoefficient(m_damageParams.at(sub)[level].attackDamagePercentInt, 0);
    PctInt_t weaponDamageCoefficientInt = m_damageParams.at(sub)[level].weaponDamagePercentInt;
    Value_t  fixedDamage = m_damageParams.at(sub)[level].fixedDamage;
    PctInt_t effectDamageAdditionalPercentInt =
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt;
    Value_t effectDamage =
        EffectDamageAll(attack, physicsDamageCoefficient, weaponDamage, weaponDamageCoefficientInt, fixedDamage, effectDamageAdditionalPercentInt);

    int      playerLevel                = JX3_PLAYER_LEVEL;
    int      targetLevel                = (*m_targets)[targetId]->GetLevel();
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetPhysicsShield();
    Value_t  shieldAdditional           = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attribute.GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldAdditionalPercentInt = m_effectShieldIgnoreAdditionalPercentInt;
    int      rollResultInt                    = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePowerAdditionalPercentInt =
        (m_effectCriticalStrikePowerAdditionalPercentInt +
         m_player->attribute.GetPhysicsCriticalStrikePowerAdditionalPercentInt());
    PctInt_t strainPercentInt = m_player->attribute.GetStrainBaseAdditionalPercentInt();
    PctInt_t pveDamageAdditionalPercentInt = m_player->attribute.GetPVEDamageAdditionalPercentInt();
    PctInt_t vulnerablePercentInt = (*m_targets)[targetId]->GetDamageAdditionalPercentInt();

    damage.damage = FinalPhysicsDamage(
        playerLevel,
        targetLevel,
        effectDamage,
        shieldBase,
        shieldAdditional,
        ignoreShieldBasePercentInt,
        ignoreShieldAdditionalPercentInt,
        overcome,
        rollResultInt,
        criticalStrikePower,
        effectCriticalStrikePowerAdditionalPercentInt,
        strain,
        strainPercentInt,
        pveDamageAdditionalPercentInt,
        vulnerablePercentInt);

    return damage;
}

JX3DPS::GainsDamage JX3DPS::Buff::CalcPhysicsDamage(Id_t targetId, RollResult rollResult, int sub, int level)
{
    GainsDamage gainsDamage;

    std::array<Attribute::Type, 6> types = {
        {{ Attribute::Type::DEFAULT },
         { Attribute::Type::WEAPON_DAMAGE_BASE },
         { Attribute::Type::ATTACK_POWER_BASE },
         { Attribute::Type::CRITICAL_STRIKE_POWER },
         { Attribute::Type::OVERCOME_BASE },
         { Attribute::Type::STRAIN_BASE }}
    };

    for (const auto &type : types) {
        Value_t attack              = m_player->attribute.GetPhysicsAttackPower();
        Value_t criticalStrikePower = m_player->attribute.GetPhysicsCriticalStrikePower();
        Value_t strain              = m_player->attribute.GetStrainBase();
        Value_t overcome            = m_player->attribute.GetPhysicsOvercome();
        Value_t weaponDamage        = m_player->attribute.GetWeaponDamage();

        switch (type) {
            case Attribute::Type::WEAPON_DAMAGE_BASE:
                m_player->attribute.AddWeaponDamageBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                weaponDamage = m_player->attribute.GetWeaponDamage();
                m_player->attribute.AddWeaponDamageBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::ATTACK_POWER_BASE:
                m_player->attribute.AddPhysicsAttackPowerBaseAdditional(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                attack = m_player->attribute.GetPhysicsAttackPower();
                m_player->attribute.AddPhysicsAttackPowerBaseAdditional(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::CRITICAL_STRIKE_POWER:
                m_player->attribute.AddPhysicsCriticalStrikeAdditional(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                criticalStrikePower = m_player->attribute.GetPhysicsCriticalStrikePower();
                m_player->attribute.AddPhysicsCriticalStrikeAdditional(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::OVERCOME_BASE:
                m_player->attribute.AddPhysicsOvercomeBaseAdditional(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                overcome = m_player->attribute.GetPhysicsOvercome();
                m_player->attribute.AddPhysicsOvercomeBaseAdditional(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::STRAIN_BASE:
                m_player->attribute.AddStrainBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                strain = m_player->attribute.GetStrainBase();
                m_player->attribute.AddStrainBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            default: break;
        }

        gainsDamage[type] =
            GetPhysicsDamage(targetId, rollResult, sub, level, attack, weaponDamage, criticalStrikePower, overcome, strain);
    }

    return gainsDamage;
}

JX3DPS::RollResult JX3DPS::Buff::GetMagicRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attribute.GetMagicCriticalStrikePercent() +
                                         m_effectCriticalStrikeAdditionalBasisPointInt / JX3_BASIS_POINT_INT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

JX3DPS::Damage JX3DPS::Buff::GetMagicDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    Value_t    attack,
    Value_t    weaponDamage,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain)
{
    Damage damage;

    PctFloat_t magicDamageCoefficient =
        MagicDamageCoefficient(m_damageParams.at(sub)[level].attackDamagePercentInt, 0);
    PctInt_t weaponDamageCoefficientInt = m_damageParams.at(sub)[level].weaponDamagePercentInt;
    Value_t  fixedDamage = m_damageParams.at(sub)[level].fixedDamage;
    PctInt_t effectDamageAdditionalPercentInt =
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt;
    Value_t effectDamage =
        EffectDamageAll(attack, magicDamageCoefficient, weaponDamage, weaponDamageCoefficientInt, fixedDamage, effectDamageAdditionalPercentInt);

    int      playerLevel                = JX3_PLAYER_LEVEL;
    int      targetLevel                = (*m_targets)[targetId]->GetLevel();
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetPhysicsShield();
    Value_t  shieldAdditional           = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attribute.GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldAdditionalPercentInt = m_effectShieldIgnoreAdditionalPercentInt;
    int      rollResultInt                    = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePowerAdditionalPercentInt =
        (m_effectCriticalStrikePowerAdditionalPercentInt +
         m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt());
    PctInt_t strainPercentInt = m_player->attribute.GetStrainBaseAdditionalPercentInt();
    PctInt_t pveDamageAdditionalPercentInt = m_player->attribute.GetPVEDamageAdditionalPercentInt();
    PctInt_t vulnerablePercentInt = (*m_targets)[targetId]->GetDamageAdditionalPercentInt();

    damage.damage = FinalMagicDamage(
        playerLevel,
        targetLevel,
        effectDamage,
        shieldBase,
        shieldAdditional,
        ignoreShieldBasePercentInt,
        ignoreShieldAdditionalPercentInt,
        overcome,
        rollResultInt,
        criticalStrikePower,
        effectCriticalStrikePowerAdditionalPercentInt,
        strain,
        strainPercentInt,
        pveDamageAdditionalPercentInt,
        vulnerablePercentInt);

    return damage;
}

JX3DPS::GainsDamage JX3DPS::Buff::CalcMagicDamage(Id_t targetId, RollResult rollResult, int sub, int level)
{
    GainsDamage gainsDamage;

    std::array<Attribute::Type, 6> types = {
        {{ Attribute::Type::DEFAULT },
         { Attribute::Type::WEAPON_DAMAGE_BASE },
         { Attribute::Type::ATTACK_POWER_BASE },
         { Attribute::Type::CRITICAL_STRIKE_POWER },
         { Attribute::Type::OVERCOME_BASE },
         { Attribute::Type::STRAIN_BASE }}
    };

    for (const auto &type : types) {
        Value_t attack              = m_player->attribute.GetMagicAttackPower();
        Value_t criticalStrikePower = m_player->attribute.GetMagicCriticalStrikePower();
        Value_t strain              = m_player->attribute.GetStrainBase();
        Value_t overcome            = m_player->attribute.GetMagicOvercome();
        Value_t weaponDamage        = m_player->attribute.GetWeaponDamage();

        switch (type) {
            case Attribute::Type::WEAPON_DAMAGE_BASE:
                m_player->attribute.AddWeaponDamageBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                weaponDamage = m_player->attribute.GetWeaponDamage();
                m_player->attribute.AddWeaponDamageBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::ATTACK_POWER_BASE:
                m_player->attribute.AddMagicAttackPowerBaseAdditional(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                attack = m_player->attribute.GetMagicAttackPower();
                m_player->attribute.AddMagicAttackPowerBaseAdditional(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::CRITICAL_STRIKE_POWER:
                m_player->attribute.AddMagicCriticalStrikeAdditional(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                criticalStrikePower = m_player->attribute.GetMagicCriticalStrikePower();
                m_player->attribute.AddMagicCriticalStrikeAdditional(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::OVERCOME_BASE:
                m_player->attribute.AddMagicOvercomeBaseAdditional(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                overcome = m_player->attribute.GetMagicOvercome();
                m_player->attribute.AddMagicOvercomeBaseAdditional(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::STRAIN_BASE:
                m_player->attribute.AddStrainBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                strain = m_player->attribute.GetStrainBase();
                m_player->attribute.AddStrainBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            default: break;
        }

        gainsDamage[type] =
            GetMagicDamage(targetId, rollResult, sub, level, attack, weaponDamage, criticalStrikePower, overcome, strain);
    }

    return gainsDamage;
}

JX3DPS::RollResult JX3DPS::Buff::GetDotRollResult(Id_t targetId) const
{
    return RandomUniform(0.0, 1.0) <
                   m_snapshots.at(targetId).criticalStrikePercent +
                       m_snapshots.at(targetId).effectCriticalStrikeAdditionalBasisPointInt / JX3_BASIS_POINT_INT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

JX3DPS::Damage JX3DPS::Buff::GetPhysicsDotDamage(
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
        PhysicsDamageCoefficient(m_damageParams.at(sub)[level].attackDamagePercentInt, 0);
    PctInt_t weaponDamageCoefficientInt = m_damageParams.at(sub)[level].weaponDamagePercentInt;
    Value_t fixedDamage = m_damageParams.at(sub)[level].fixedDamage;
    PctInt_t effectDamageAdditionalPercentInt = m_snapshots.at(targetId).effectDamageAdditionalPercentInt;
    Value_t effectDamage =
        effectCount *
        EffectDamageAll(attack, physicsDamageCoefficient, weaponDamage, weaponDamageCoefficientInt, fixedDamage, effectDamageAdditionalPercentInt);

    int      playerLevel                = JX3_PLAYER_LEVEL;
    int      targetLevel                = (*m_targets)[targetId]->GetLevel();
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetPhysicsShield();
    Value_t  shieldAdditional           = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attribute.GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldAdditionalPercentInt = m_effectShieldIgnoreAdditionalPercentInt;
    int      rollResultInt                    = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePowerAdditionalPercentInt =
        m_snapshots.at(targetId).effectCriticalStrikePowerAdditionalPercentInt;
    PctInt_t strainPercentInt = m_snapshots.at(targetId).strainBaseAdditionalPercentInt;
    PctInt_t pveDamageAdditionalPercentInt = m_player->attribute.GetPVEDamageAdditionalPercentInt();
    PctInt_t vulnerablePercentInt = (*m_targets)[targetId]->GetDamageAdditionalPercentInt();

    damage.damage = FinalPhysicsDamage(
        playerLevel,
        targetLevel,
        effectDamage,
        shieldBase,
        shieldAdditional,
        ignoreShieldBasePercentInt,
        ignoreShieldAdditionalPercentInt,
        overcome,
        rollResultInt,
        criticalStrikePower,
        effectCriticalStrikePowerAdditionalPercentInt,
        strain,
        strainPercentInt,
        pveDamageAdditionalPercentInt,
        vulnerablePercentInt);

    return damage;
}

JX3DPS::GainsDamage JX3DPS::Buff::CalcPhysicsDotDamage(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount)
{
    GainsDamage gainsDamage;

    std::array<Attribute::Type, 6> types = {
        {{ Attribute::Type::DEFAULT },
         { Attribute::Type::WEAPON_DAMAGE_BASE },
         { Attribute::Type::ATTACK_POWER_BASE },
         { Attribute::Type::CRITICAL_STRIKE_POWER },
         { Attribute::Type::OVERCOME_BASE },
         { Attribute::Type::STRAIN_BASE }}
    };

    for (const auto &type : types) {
        Value_t attack              = m_snapshots.at(targetId).attackPower;
        Value_t criticalStrikePower = m_snapshots.at(targetId).criticalStrikePower;
        Value_t strain              = m_snapshots.at(targetId).strainBase;
        Value_t overcome            = m_player->attribute.GetPhysicsOvercome();
        Value_t weaponDamage        = m_player->attribute.GetWeaponDamage();

        switch (type) {
            case Attribute::Type::WEAPON_DAMAGE_BASE:
                m_player->attribute.AddWeaponDamageBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                weaponDamage = m_player->attribute.GetWeaponDamage();
                m_player->attribute.AddWeaponDamageBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::ATTACK_POWER_BASE:
                attack = m_snapshots.at(targetId).attackPowerGain;
                break;
            case Attribute::Type::CRITICAL_STRIKE_POWER:
                criticalStrikePower = m_snapshots.at(targetId).criticalStrikePowerGain;
                break;
            case Attribute::Type::OVERCOME_BASE:
                m_player->attribute.AddPhysicsOvercomeBaseAdditional(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                overcome = m_player->attribute.GetPhysicsOvercome();
                m_player->attribute.AddPhysicsOvercomeBaseAdditional(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::STRAIN_BASE:
                strain = m_snapshots.at(targetId).strainBaseGain;
                break;
            default: break;
        }

        gainsDamage[type] =
            GetPhysicsDotDamage(targetId, rollResult, sub, level, effectCount, attack, weaponDamage, criticalStrikePower, overcome, strain);
    }

    return gainsDamage;
}

JX3DPS::Damage JX3DPS::Buff::GetMagicDotDamage(
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

    PctFloat_t magicDamageCoefficient =
        MagicDamageCoefficient(m_damageParams.at(sub)[level].attackDamagePercentInt, 0);
    PctInt_t weaponDamageCoefficientInt = m_damageParams.at(sub)[level].weaponDamagePercentInt;
    Value_t fixedDamage = m_damageParams.at(sub)[level].fixedDamage;
    PctInt_t effectDamageAdditionalPercentInt = m_snapshots.at(targetId).effectDamageAdditionalPercentInt;
    Value_t effectDamage =
        effectCount *
        EffectDamageAll(attack, magicDamageCoefficient, weaponDamage, weaponDamageCoefficientInt, fixedDamage, effectDamageAdditionalPercentInt);

    int      playerLevel                = JX3_PLAYER_LEVEL;
    int      targetLevel                = (*m_targets)[targetId]->GetLevel();
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetMagicShield();
    Value_t  shieldAdditional           = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attribute.GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldAdditionalPercentInt = m_effectShieldIgnoreAdditionalPercentInt;
    int      rollResultInt                    = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePowerAdditionalPercentInt =
        m_snapshots.at(targetId).effectCriticalStrikePowerAdditionalPercentInt;
    PctInt_t strainPercentInt = m_snapshots.at(targetId).strainBaseAdditionalPercentInt;
    PctInt_t pveDamageAdditionalPercentInt = m_player->attribute.GetPVEDamageAdditionalPercentInt();
    PctInt_t vulnerablePercentInt = (*m_targets)[targetId]->GetDamageAdditionalPercentInt();

    damage.damage = FinalMagicDamage(
        playerLevel,
        targetLevel,
        effectDamage,
        shieldBase,
        shieldAdditional,
        ignoreShieldBasePercentInt,
        ignoreShieldAdditionalPercentInt,
        overcome,
        rollResultInt,
        criticalStrikePower,
        effectCriticalStrikePowerAdditionalPercentInt,
        strain,
        strainPercentInt,
        pveDamageAdditionalPercentInt,
        vulnerablePercentInt);

    return damage;
}

JX3DPS::GainsDamage JX3DPS::Buff::CalcMagicDotDamage(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount)
{
    GainsDamage gainsDamage;

    std::array<Attribute::Type, 6> types = {
        {{ Attribute::Type::DEFAULT },
         { Attribute::Type::WEAPON_DAMAGE_BASE },
         { Attribute::Type::ATTACK_POWER_BASE },
         { Attribute::Type::CRITICAL_STRIKE_POWER },
         { Attribute::Type::OVERCOME_BASE },
         { Attribute::Type::STRAIN_BASE }}
    };

    for (const auto &type : types) {
        Value_t attack              = m_snapshots.at(targetId).attackPower;
        Value_t criticalStrikePower = m_snapshots.at(targetId).criticalStrikePower;
        Value_t strain              = m_snapshots.at(targetId).strainBase;
        Value_t overcome            = m_player->attribute.GetMagicOvercome();
        Value_t weaponDamage        = m_player->attribute.GetWeaponDamage();

        switch (type) {
            case Attribute::Type::WEAPON_DAMAGE_BASE:
                m_player->attribute.AddWeaponDamageBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                weaponDamage = m_player->attribute.GetWeaponDamage();
                m_player->attribute.AddWeaponDamageBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::ATTACK_POWER_BASE:
                attack = m_snapshots.at(targetId).attackPowerGain;
                break;
            case Attribute::Type::CRITICAL_STRIKE_POWER:
                criticalStrikePower = m_snapshots.at(targetId).criticalStrikePowerGain;
                break;
            case Attribute::Type::OVERCOME_BASE:
                m_player->attribute.AddMagicOvercomeBaseAdditional(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                overcome = m_player->attribute.GetMagicOvercome();
                m_player->attribute.AddMagicOvercomeBaseAdditional(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::STRAIN_BASE:
                strain = m_snapshots.at(targetId).strainBaseGain;
                break;
            default: break;
        }

        gainsDamage[type] =
            GetMagicDotDamage(targetId, rollResult, sub, level, effectCount, attack, weaponDamage, criticalStrikePower, overcome, strain);
    }

    return gainsDamage;
}

void JX3DPS::Buff::Record(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub, int level)
{
    for (const auto &[type, damage] : gainsDamage) {
        m_stats.gainStats[type][targetId][m_id][sub][level][rollResult].first++;
        m_stats.gainStats[type][targetId][m_id][sub][level][rollResult].second.damage +=
            damage.damage;
        m_stats.gainStats[type][targetId][m_id][sub][level][rollResult].second.surplusDamage +=
            damage.surplusDamage;
    }
}

void JX3DPS::Buff::AddTriggerEffect(Id_t id, const TriggerEffect &triggerEffect)
{
    m_triggerEffects[id] = triggerEffect;
}
