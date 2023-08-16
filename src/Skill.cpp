/**
 * Project: JX3DPS
 * File: Skill.cpp
 * Description:
 * Created Date: 2023-07-21 08:37:24
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-16 03:16:16
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Skill.h"

#include "Damage/Damage.hpp"

#include "Player.h"
#include "Target.hpp"

JX3DPS::Skill::Skill(Player *player, Targets *targets)
{
    this->m_player          = player;
    this->m_targets         = targets;
    m_globalCooldownCurrent = &(player->globalCooldownCurrent);
}

JX3DPS::Skill::~Skill() { }

JX3DPS::Skill::Skill(const Skill &other)
{
    m_id                     = other.m_id;
    m_name                   = other.m_name;
    m_cooldown               = other.m_cooldown;
    m_cooldownCurrent        = other.m_cooldownCurrent;
    m_prepareFrames          = other.m_prepareFrames;
    m_prepareFramesCurrent   = other.m_prepareFramesCurrent;
    m_energyCount            = other.m_energyCount;
    m_energyCountCurrent     = other.m_energyCountCurrent;
    m_targetLimit            = other.m_targetLimit;
    m_range                  = other.m_range;
    m_effectQidianAdditional = other.m_effectQidianAdditional;
    m_damageParams           = other.m_damageParams;
    m_stats                  = other.m_stats;
    m_effectCriticalStrikeAdditionalBasisPointInt = other.m_effectCriticalStrikeAdditionalBasisPointInt;
    m_effectCriticalStrikePowerAdditionalPercentInt = other.m_effectCriticalStrikePowerAdditionalPercentInt;
    m_effectDamageAdditionalPercentInt = other.m_effectDamageAdditionalPercentInt;
    m_effectShieldIgnoreAdditionalPercentInt = other.m_effectShieldIgnoreAdditionalPercentInt;
    m_triggerEffects = other.m_triggerEffects;

    if (other.m_globalCooldownCurrent == &(other.m_noneGlobalCooldown)) {
        m_globalCooldownCurrent = &(m_noneGlobalCooldown);
    }
}

JX3DPS::Skill &JX3DPS::Skill::operator=(const Skill &other)
{
    if (this == &other) {
        return *this;
    }

    m_id                     = other.m_id;
    m_name                   = other.m_name;
    m_cooldown               = other.m_cooldown;
    m_cooldownCurrent        = other.m_cooldownCurrent;
    m_prepareFrames          = other.m_prepareFrames;
    m_prepareFramesCurrent   = other.m_prepareFramesCurrent;
    m_energyCount            = other.m_energyCount;
    m_energyCountCurrent     = other.m_energyCountCurrent;
    m_targetLimit            = other.m_targetLimit;
    m_range                  = other.m_range;
    m_effectQidianAdditional = other.m_effectQidianAdditional;
    m_damageParams           = other.m_damageParams;
    m_stats                  = other.m_stats;
    m_effectCriticalStrikeAdditionalBasisPointInt = other.m_effectCriticalStrikeAdditionalBasisPointInt;
    m_effectCriticalStrikePowerAdditionalPercentInt = other.m_effectCriticalStrikePowerAdditionalPercentInt;
    m_effectDamageAdditionalPercentInt = other.m_effectDamageAdditionalPercentInt;
    m_effectShieldIgnoreAdditionalPercentInt = other.m_effectShieldIgnoreAdditionalPercentInt;
    m_triggerEffects = other.m_triggerEffects;

    if (other.m_globalCooldownCurrent == &(other.m_noneGlobalCooldown)) {
        m_globalCooldownCurrent = &(m_noneGlobalCooldown);
    }

    return *this;
}

bool JX3DPS::Skill::IsReady(bool fcast)
{
    if (m_player->IsCast()) {
        return false;
    }
    if (!m_player->IsReCast()) {
        return true;
    }
    if (fcast) {
        return true;
    }
    return false;
}

bool JX3DPS::Skill::StopReCastSkill()
{
    m_player->skills[m_player->GetReCastSkill()]->Stop();
    return true;
}

void JX3DPS::Skill::SetPlayer(Player *player)
{
    m_player = player;
    if (m_globalCooldownCurrent != &(m_noneGlobalCooldown)) {
        m_globalCooldownCurrent = &(m_player->globalCooldownCurrent);
    }
}

void JX3DPS::Skill::SetTargets(Targets *targets)
{
    m_targets = targets;
}

JX3DPS::Frame_t JX3DPS::Skill::GetNextKeyFrame() const
{
    if (m_energyCount > 0) { // 充能技能 取 预处理时间 和 冷却时间 最小值
        return std::min(m_cooldownCurrent, m_prepareFramesCurrent);
    }
    return m_prepareFramesCurrent;
}

void JX3DPS::Skill::UpdateKeyFrame(Frame_t frame)
{
    if (m_cooldownCurrent != JX3DPS_INVALID_FRAMES_SET) {
        m_cooldownCurrent -= frame;
    }
    m_cooldownCurrent = std::max(m_cooldownCurrent, 0);
    if (m_prepareFramesCurrent != JX3DPS_INVALID_FRAMES_SET) { // 在蓄力时间才进行关键帧刷新
        m_prepareFramesCurrent -= frame;
    }
}

JX3DPS::Frame_t JX3DPS::Skill::GetTriggerFrame() const
{
    return m_prepareFramesCurrent;
}

JX3DPS::Frame_t JX3DPS::Skill::GetCooldownCurrent() const
{
    if (m_energyCountCurrent > 0 && *m_globalCooldownCurrent == 0) {
        return 0;
    }
    return std::max(m_cooldownCurrent, *m_globalCooldownCurrent);
}

void JX3DPS::Skill::SetEnergyCooldownCurrent(Frame_t frame)
{
    m_cooldownCurrent = frame;
}

JX3DPS::Frame_t JX3DPS::Skill::GetEnergyCooldownCurrent() const
{
    return m_cooldownCurrent;
}

int JX3DPS::Skill::GetEnergyCountCurrent() const
{
    return m_energyCountCurrent;
}

void JX3DPS::Skill::SetEnergyCountCurrent(int count)
{
    m_energyCountCurrent = count;
}

double JX3DPS::Skill::GetRange() const
{
    return m_range;
}

JX3DPS::RollResult JX3DPS::Skill::GetPhysicsRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attribute.GetPhysicsCriticalStrikePercent() +
                                         m_effectCriticalStrikeAdditionalBasisPointInt / JX3_BASIS_POINT_INT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

JX3DPS::Damage JX3DPS::Skill::GetPhysicsDamage(
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

JX3DPS::GainsDamage JX3DPS::Skill::CalcPhysicsDamage(Id_t targetId, RollResult rollResult, int sub, int level)
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
    gainsDamage[Attribute::Type::SURPLUS_VALUE_BASE] = gainsDamage[Attribute::Type::DEFAULT];

    return gainsDamage;
}

JX3DPS::RollResult JX3DPS::Skill::GetMagicRollResult() const
{
    return RandomUniform(0.0, 1.0) < m_player->attribute.GetMagicCriticalStrikePercent() +
                                         m_effectCriticalStrikeAdditionalBasisPointInt / JX3_BASIS_POINT_INT_BASE
               ? RollResult::DOUBLE
               : RollResult::HIT;
}

JX3DPS::Damage JX3DPS::Skill::GetMagicDamage(
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
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetMagicShield();
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

JX3DPS::GainsDamage JX3DPS::Skill::CalcMagicDamage(Id_t targetId, RollResult rollResult, int sub, int level)
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
    gainsDamage[Attribute::Type::SURPLUS_VALUE_BASE] = gainsDamage[Attribute::Type::DEFAULT];

    return gainsDamage;
}

JX3DPS::Damage JX3DPS::Skill::GetPhysicsSurplusDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    Value_t    surplus,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain)
{
    Damage damage;

    PctInt_t surplusCoefficientInt = m_damageParams.at(sub)[level].attackDamagePercentInt;
    Value_t surplusDamage = SurplusDamage(surplus, surplusCoefficientInt, JX3_PLAYER_LEVEL);

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

    damage.surplusDamage = FinalPhysicsDamage(
        playerLevel,
        targetLevel,
        surplusDamage,
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

JX3DPS::GainsDamage JX3DPS::Skill::CalcPhysicsSurplusDamage(Id_t targetId, RollResult rollResult, int sub, int level)
{
    GainsDamage gainsDamage;

    std::array<Attribute::Type, 6> types = {
        {{ Attribute::Type::DEFAULT },
         { Attribute::Type::SURPLUS_VALUE_BASE },
         { Attribute::Type::CRITICAL_STRIKE_POWER },
         { Attribute::Type::OVERCOME_BASE },
         { Attribute::Type::STRAIN_BASE }}
    };

    for (const auto &type : types) {
        Value_t surplus             = m_player->attribute.GetSurplusValueBase();
        Value_t criticalStrikePower = m_player->attribute.GetPhysicsCriticalStrikePower();
        Value_t strain              = m_player->attribute.GetStrainBase();
        Value_t overcome            = m_player->attribute.GetPhysicsOvercome();
        Value_t weaponDamage        = m_player->attribute.GetWeaponDamage();

        switch (type) {
            case Attribute::Type::SURPLUS_VALUE_BASE:
                m_player->attribute.AddSurplusValueBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                surplus = m_player->attribute.GetSurplusValueBase();
                m_player->attribute.AddSurplusValueBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
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
            GetPhysicsSurplusDamage(targetId, rollResult, sub, level, surplus, criticalStrikePower, overcome, strain);
    }
    gainsDamage[Attribute::Type::WEAPON_DAMAGE_BASE] = gainsDamage[Attribute::Type::DEFAULT];
    gainsDamage[Attribute::Type::ATTACK_POWER_BASE] = gainsDamage[Attribute::Type::DEFAULT];

    return gainsDamage;
}

JX3DPS::Damage JX3DPS::Skill::GetMagicSurplusDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    Value_t    surplus,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain)
{
    Damage damage;

    PctInt_t surplusCoefficientInt = m_damageParams.at(sub)[level].attackDamagePercentInt;
    Value_t surplusDamage = SurplusDamage(surplus, surplusCoefficientInt, JX3_PLAYER_LEVEL);

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

    damage.surplusDamage = FinalMagicDamage(
        playerLevel,
        targetLevel,
        surplusDamage,
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

JX3DPS::GainsDamage JX3DPS::Skill::CalcMagicSurplusDamage(Id_t targetId, RollResult rollResult, int sub, int level)
{
    GainsDamage gainsDamage;

    std::array<Attribute::Type, 6> types = {
        {{ Attribute::Type::DEFAULT },
         { Attribute::Type::SURPLUS_VALUE_BASE },
         { Attribute::Type::CRITICAL_STRIKE_POWER },
         { Attribute::Type::OVERCOME_BASE },
         { Attribute::Type::STRAIN_BASE }}
    };

    for (const auto &type : types) {
        Value_t surplus             = m_player->attribute.GetSurplusValueBase();
        Value_t criticalStrikePower = m_player->attribute.GetMagicCriticalStrikePower();
        Value_t strain              = m_player->attribute.GetStrainBase();
        Value_t overcome            = m_player->attribute.GetMagicOvercome();
        Value_t weaponDamage        = m_player->attribute.GetWeaponDamage();

        switch (type) {
            case Attribute::Type::SURPLUS_VALUE_BASE:
                m_player->attribute.AddSurplusValueBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                surplus = m_player->attribute.GetSurplusValueBase();
                m_player->attribute.AddSurplusValueBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
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
            GetMagicSurplusDamage(targetId, rollResult, sub, level, surplus, criticalStrikePower, overcome, strain);
    }
    gainsDamage[Attribute::Type::WEAPON_DAMAGE_BASE] = gainsDamage[Attribute::Type::DEFAULT];
    gainsDamage[Attribute::Type::ATTACK_POWER_BASE] = gainsDamage[Attribute::Type::DEFAULT];

    return gainsDamage;
}

void JX3DPS::Skill::Record(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub, int level)
{
    for (const auto &[type, damage] : gainsDamage) {
        m_stats.gainStats[type][targetId][m_id][sub][level][rollResult].first++;
        m_stats.gainStats[type][targetId][m_id][sub][level][rollResult].second.damage +=
            damage.damage;
        m_stats.gainStats[type][targetId][m_id][sub][level][rollResult].second.surplusDamage +=
            damage.surplusDamage;
    }
}

void JX3DPS::Skill::AddTriggerEffect(Id_t id, const TriggerEffect &triggerEffect)
{
    m_triggerEffects[id] = triggerEffect;
}
