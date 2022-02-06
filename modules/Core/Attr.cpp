/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 15:19:09
 * @FilePath    : \JX3DPS\modules\Core\Attr.cpp
 */

#include "Attr.h"

#include "Target.h"

namespace JX3DPS {

Attr::Attr(JX3Class classType)
{
    switch (classType) {
    case TAI_XU_JIAN_YI:
        m_physicsAttackFromClass         = &Attr::GetPhysicsAttackFromClassTaiXuJianYi;
        m_physicsCriticalStrikeFromClass = &Attr::GetPhysicsCriticalStrikeFromClassTaiXuJianYi;
        break;
    default: break;
    }
}

Value_t Attr::GetAgilityBase() const
{
    return m_agilityBase;
}

void Attr::SetAgilityBase(Value_t value)
{
    m_agilityBase = value;
    UpdateAgility();
}

void Attr::AddAgilityBase(Value_t value)
{
    m_agilityBase += value;
    UpdateAgility();
}

BinPCT_t Attr::GetAgilityBaseBinPercent() const
{
    return m_agilityBaseBinPercent;
}

void Attr::SetAgilityBaseBinPercent(BinPCT_t binPercent)
{
    m_agilityBaseBinPercent = binPercent;
    UpdateAgility();
}

void Attr::AddAgilityBaseBinPercent(BinPCT_t binPercent)
{
    m_agilityBaseBinPercent += binPercent;
    UpdateAgility();
}

Value_t Attr::GetAgility() const
{
    return m_agility;
}

Value_t Attr::GetStrengthBase() const
{
    return m_strengthBase;
}

void Attr::SetStrengthBase(Value_t value)
{
    m_strengthBase = value;
    UpdateStrength();
}

void Attr::AddStrengthBase(Value_t value)
{
    m_strengthBase += value;
    UpdateStrength();
}

BinPCT_t Attr::GetStrengthBaseBinPercent() const
{
    return m_strengthBaseBinPercent;
}

void Attr::SetStrengthBaseBinPercent(BinPCT_t binPercent)
{
    m_strengthBaseBinPercent = binPercent;
    UpdateStrength();
}

void Attr::AddStrengthBaseBinPercent(BinPCT_t binPercent)
{
    m_strengthBaseBinPercent += binPercent;
    UpdateStrength();
}

Value_t Attr::GetStrength() const
{
    return m_strength;
}

Value_t Attr::GetSpiritBase() const
{
    return m_spiritBase;
}

void Attr::SetSpiritBase(Value_t value)
{
    m_spiritBase = value;
    UpdateSpirit();
}

void Attr::AddSpiritBase(Value_t value)
{
    m_spiritBase += value;
    UpdateSpirit();
}

BinPCT_t Attr::GetSpiritBaseBinPercent() const
{
    return m_spiritBaseBinPercent;
}

void Attr::SetSpiritBaseBinPercent(BinPCT_t binPercent)
{
    m_spiritBaseBinPercent = binPercent;
    UpdateSpirit();
}

void Attr::AddSpiritBaseBinPercent(BinPCT_t binPercent)
{
    m_spiritBaseBinPercent += binPercent;
    UpdateSpirit();
}

Value_t Attr::GetSpirit() const
{
    return m_spirit;
}

Value_t Attr::GetSpunkBase() const
{
    return m_spunkBase;
}

void Attr::SetSpunkBase(Value_t value)
{
    m_spunkBase = value;
    UpdateSpunk();
}

void Attr::AddSpunkBase(Value_t value)
{
    m_spunkBase += value;
    UpdateSpunk();
}

BinPCT_t Attr::GetSpunkBaseBinPercent() const
{
    return m_spunkBaseBinPercent;
}

void Attr::SetSpunkBaseBinPercent(BinPCT_t binPercent)
{
    m_spunkBaseBinPercent = binPercent;
    UpdateSpunk();
}

void Attr::AddSpunkBaseBinPercent(BinPCT_t binPercent)
{
    m_spunkBaseBinPercent += binPercent;
    UpdateSpunk();
}

Value_t Attr::GetSpunk() const
{
    return m_spunk;
}

Value_t Attr::GetWeaponAttack() const
{
    return m_weaponAttack;
}

void Attr::SetWeaponAttack(Value_t lower, Value_t upper)
{
    m_weaponAttack = (lower + upper) / 2;
}

void Attr::AddWeaponAttack(Value_t value)
{
    m_weaponAttack += value;
}

void Attr::AddWeaponAttack(Value_t lower, Value_t upper)
{
    m_weaponAttack += (lower + upper) / 2;
}

Value_t Attr::GetPhysicsAttackBaseFromCustom() const
{
    return m_physicsAttackBaseFromCustom;
}

void Attr::SetPhysicsAttackBaseFromCustom(Value_t value)
{
    m_physicsAttackBaseFromCustom = value;
    UpdatePhysicsAttack();
}
void Attr::AddPhysicsAttackBaseFromCustom(Value_t value)
{
    m_physicsAttackBaseFromCustom += value;
    UpdatePhysicsAttack();
}

Value_t Attr::GetPhysicsAttackBaseFromMajor() const
{
    return m_strength * JX3DPS_ATTACK_BASE_PER_STRENGTH;
}

Value_t Attr::GetPhysicsAttackBase() const
{
    return GetPhysicsAttackBaseFromCustom() + GetPhysicsAttackBaseFromMajor();
}

void Attr::SetPhysicsAttackBase(Value_t value)
{
    SetPhysicsAttackBaseFromCustom(value - GetPhysicsAttackBaseFromMajor());
}

void Attr::AddPhysicsAttackBase(Value_t value)
{
    AddPhysicsAttackBaseFromCustom(value);
}

BinPCT_t Attr::GetPhysicsAttackBaseBinPercent() const
{
    return m_physicsAttackBaseBinPercent;
}

void Attr::SetPhysicsAttackBaseBinPercent(BinPCT_t binPercent)
{
    m_physicsAttackBaseBinPercent = binPercent;
    UpdatePhysicsAttack();
}

void Attr::AddPhysicsAttackBaseBinPercent(BinPCT_t binPercent)
{
    m_physicsAttackBaseBinPercent += binPercent;
    UpdatePhysicsAttack();
}

Value_t Attr::GetPhysicsAttackFromClass() const
{
    return (this->*m_physicsAttackFromClass)();
}

Value_t Attr::GetPhysicsAttack() const
{
    return m_physicsAttack;
}

Value_t Attr::GetPhysicsAttackBaseMinimum() const
{
    return GetPhysicsAttackBaseFromMajor();
}

Value_t Attr::GetPhysicsAttackMinimum() const
{
    return GetPhysicsAttackBaseFromMajor() + GetPhysicsAttackFromClass();
}

Value_t Attr::GetMagicAttackBaseFromCustom() const
{
    return m_magicAttackBaseFromCustom;
}

void Attr::SetMagicAttackBaseFromCustom(Value_t value)
{
    m_magicAttackBaseFromCustom = value;
    UpdateMagicAttack();
}
void Attr::AddMagicAttackBaseFromCustom(Value_t value)
{
    m_magicAttackBaseFromCustom += value;
    UpdateMagicAttack();
}

Value_t Attr::GetMagicAttackBaseFromMajor() const
{
    return m_spunk * JX3DPS_ATTACK_BASE_PER_SPUNK;
}

Value_t Attr::GetMagicAttackBase() const
{
    return GetMagicAttackBaseFromCustom() + GetMagicAttackBaseFromMajor();
}

void Attr::SetMagicAttackBase(Value_t value)
{
    SetMagicAttackBaseFromCustom(value - GetMagicAttackBaseFromMajor());
}

void Attr::AddMagicAttackBase(Value_t value)
{
    AddMagicAttackBaseFromCustom(value);
}

BinPCT_t Attr::GetMagicAttackBaseBinPercent() const
{
    return m_magicAttackBaseBinPercent;
}

void Attr::SetMagicAttackBaseBinPercent(BinPCT_t binPercent)
{
    m_magicAttackBaseBinPercent = binPercent;
    UpdateMagicAttack();
}

void Attr::AddMagicAttackBaseBinPercent(BinPCT_t binPercent)
{
    m_magicAttackBaseBinPercent += binPercent;
    UpdateMagicAttack();
}

Value_t Attr::GetMagicAttackFromClass() const
{
    return (this->*m_magicAttackFromClass)();
}

Value_t Attr::GetMagicAttack() const
{
    return m_magicAttack;
}

Value_t Attr::GetMagicAttackBaseMinimum() const
{
    return GetMagicAttackBaseFromMajor();
}

Value_t Attr::GetMagicAttackMinimum() const
{
    return GetMagicAttackBaseFromMajor() + GetMagicAttackFromClass();
}

Value_t Attr::GetPhysicsCriticalStrikeFromCustom() const
{
    return m_physicsCriticalStrikeFromCustom;
}

void Attr::SetPhysicsCriticalStrikeFromCustom(Value_t value)
{
    m_physicsCriticalStrikeFromCustom = value;
    UpdatePhysicsCriticalStrikePercent();
}

void Attr::AddPhysicsCriticalStrikeFromCustom(Value_t value)
{
    m_physicsCriticalStrikeFromCustom += value;
    UpdatePhysicsCriticalStrikePercent();
}

Value_t Attr::GetPhysicsCriticalStrikeFromMajor() const
{
    return m_agility * JX3DPS_CRITICAL_STRIKE_PER_AGILITY;
}

Value_t Attr::GetPhysicsCriticalStrikeFromClass() const
{
    return (this->*m_physicsCriticalStrikeFromClass)();
}

Value_t Attr::GetPhysicsCriticalStrike() const
{
    return m_physicsCriticalStrike;
}

void Attr::SetPhysicsCriticalStrike(Value_t value)
{
    SetPhysicsCriticalStrikeFromCustom(value - GetPhysicsCriticalStrikeMinimum());
}

void Attr::AddPhysicsCriticalStrike(Value_t value)
{
    AddPhysicsCriticalStrikeFromCustom(value);
}

PCT_t Attr::GetPhysicsCriticalStrikePercentFromCustom() const
{
    return m_physicsCriticalStrikePercentFromCustom;
}

void Attr::SetPhysicsCriticalStrikePercentFromCustom(PCT_t percent)
{
    m_physicsCriticalStrikePercentFromCustom = percent;
    UpdatePhysicsCriticalStrikePercent();
}

void Attr::AddPhysicsCriticalStrikePercentFromCustom(PCT_t percent)
{
    m_physicsCriticalStrikePercentFromCustom += percent;
    UpdatePhysicsCriticalStrikePercent();
}

PCT_t Attr::GetPhysicsCriticalStrikePercent() const
{
    return m_physicsCriticalStrikePercent;
}

Value_t Attr::GetPhysicsCriticalStrikeMinimum() const
{
    return GetPhysicsCriticalStrikeFromCustom() + GetPhysicsCriticalStrikeFromClass();
}

Value_t Attr::GetMagicCriticalStrikeFromCustom() const
{
    return m_magicCriticalStrikeFromCustom;
}

void Attr::SetMagicCriticalStrikeFromCustom(Value_t value)
{
    m_magicCriticalStrikeFromCustom = value;
    UpdateMagicCriticalStrikePercent();
}

void Attr::AddMagicCriticalStrikeFromCustom(Value_t value)
{
    m_magicCriticalStrikeFromCustom += value;
    UpdateMagicCriticalStrikePercent();
}

Value_t Attr::GetMagicCriticalStrikeFromMajor() const
{
    return m_spirit * JX3DPS_CRITICAL_STRIKE_PER_SPIRIT;
}

Value_t Attr::GetMagicCriticalStrikeFromClass() const
{
    return (this->*m_magicCriticalStrikeFromClass)();
}

Value_t Attr::GetMagicCriticalStrike() const
{
    return m_magicCriticalStrike;
}

void Attr::SetMagicCriticalStrike(Value_t value)
{
    SetMagicCriticalStrikeFromCustom(value - GetMagicCriticalStrikeMinimum());
}

void Attr::AddMagicCriticalStrike(Value_t value)
{
    AddMagicCriticalStrikeFromCustom(value);
}

PCT_t Attr::GetMagicCriticalStrikePercentFromCustom() const
{
    return m_magicCriticalStrikePercentFromCustom;
}

void Attr::SetMagicCriticalStrikePercentFromCustom(PCT_t percent)
{
    m_magicCriticalStrikePercentFromCustom = percent;
    UpdateMagicCriticalStrikePercent();
}

void Attr::AddMagicCriticalStrikePercentFromCustom(PCT_t percent)
{
    m_magicCriticalStrikePercentFromCustom += percent;
    UpdateMagicCriticalStrikePercent();
}

PCT_t Attr::GetMagicCriticalStrikePercent() const
{
    return m_magicCriticalStrikePercent;
}

Value_t Attr::GetMagicCriticalStrikeMinimum() const
{
    return GetMagicCriticalStrikeFromCustom() + GetMagicCriticalStrikeFromClass();
}

Value_t Attr::GetPhysicsCriticalStrikePower() const
{
    return m_physicsCriticalStrikePower;
}

void Attr::SetPhysicsCriticalStrikePower(Value_t value)
{
    m_physicsCriticalStrikePower = value;
    UpdatePhysicsCriticalStrikePowerPercent();
}

void Attr::AddPhysicsCriticalStrikePower(Value_t value)
{
    m_physicsCriticalStrikePower += value;
    UpdatePhysicsCriticalStrikePowerPercent();
}

PCT_t Attr::GetPhysicsCriticalStrikePowerPercentFromCustom() const
{
    return m_physicsCriticalStrikePowerPercentFromCustom;
}

void Attr::SetPhysicsCriticalStrikePowerPercentFromCustom(PCT_t percent)
{
    m_physicsCriticalStrikePowerPercentFromCustom = percent;
    UpdatePhysicsCriticalStrikePowerPercent();
}

void Attr::AddPhysicsCriticalStrikePowerPercentFromCustom(PCT_t percent)
{
    m_physicsCriticalStrikePowerPercentFromCustom += percent;
    UpdatePhysicsCriticalStrikePowerPercent();
}

PCT_t Attr::GetPhysicsCriticalStrikePowerPercent() const
{
    return m_physicsCriticalStrikePowerPercent;
}

Value_t Attr::GetMagicCriticalStrikePower() const
{
    return m_magicCriticalStrikePower;
}

void Attr::SetMagicCriticalStrikePower(Value_t value)
{
    m_magicCriticalStrikePower = value;
    UpdateMagicCriticalStrikePowerPercent();
}

void Attr::AddMagicCriticalStrikePower(Value_t value)
{
    m_magicCriticalStrikePower += value;
    UpdateMagicCriticalStrikePowerPercent();
}

PCT_t Attr::GetMagicCriticalStrikePowerPercentFromCustom() const
{
    return m_magicCriticalStrikePowerPercentFromCustom;
}

void Attr::SetMagicCriticalStrikePowerPercentFromCustom(PCT_t percent)
{
    m_magicCriticalStrikePowerPercentFromCustom = percent;
    UpdateMagicCriticalStrikePowerPercent();
}

void Attr::AddMagicCriticalStrikePowerPercentFromCustom(PCT_t percent)
{
    m_magicCriticalStrikePowerPercentFromCustom += percent;
    UpdateMagicCriticalStrikePowerPercent();
}

PCT_t Attr::GetMagicCriticalStrikePowerPercent() const
{
    return m_magicCriticalStrikePowerPercent;
}

Value_t Attr::GetPhysicsOvercomeBaseFromCustom() const
{
    return m_physicsOvercomeBaseFromCustom;
}

void Attr::SetPhysicsOvercomeBaseFromCustom(Value_t value)
{
    m_physicsOvercomeBaseFromCustom = value;
    UpdatePhysicsOvercomePercent();
}

void Attr::AddPhysicsOvercomeBaseFromCustom(Value_t value)
{
    m_physicsOvercomeBaseFromCustom += value;
    UpdatePhysicsOvercomePercent();
}

Value_t Attr::GetPhysicsOvercomeBaseFromMajor() const
{
    return m_strength * JX3DPS_OVERCOME_BASE_PER_STRENGTH;
}

Value_t Attr::GetPhysicsOvercomeBase() const
{
    return GetPhysicsOvercomeBaseFromCustom() + GetPhysicsOvercomeBaseFromMajor();
}

void Attr::SetPhysicsOvercomeBase(Value_t value)
{
    SetPhysicsOvercomeBaseFromCustom(value - GetPhysicsOvercomeBaseFromMajor());
}

void Attr::AddPhysicsOvercomeBase(Value_t value)
{
    AddPhysicsOvercomeBaseFromCustom(value);
}

BinPCT_t Attr::GetPhysicsOvercomeBaseBinPercent() const
{
    return m_physicsOvercomeBaseBinPercent;
}

void Attr::SetPhysicsOvercomeBaseBinPercent(BinPCT_t binPercent)
{
    m_physicsOvercomeBaseBinPercent = binPercent;
    UpdatePhysicsOvercomePercent();
}

void Attr::AddPhysicsOvercomeBaseBinPercent(BinPCT_t binPercent)
{
    m_physicsOvercomeBaseBinPercent += binPercent;
    UpdatePhysicsOvercomePercent();
}

Value_t Attr::GetPhysicsOvercomeFromClass() const
{
    return (this->*m_physicsOvercomeFromClass)();
}

Value_t Attr::GetPhysicsOvercome() const
{
    return m_physicsOvercome;
}

PCT_t Attr::GetPhysicsOvercomePercent() const
{
    return m_physicsOvercomePercent;
}

Value_t Attr::GetPhysicsOvercomeBaseMinimum() const
{
    return GetPhysicsOvercomeBaseFromMajor();
}

Value_t Attr::GetPhysicsOvercomeMinimum() const
{
    return GetPhysicsOvercomeBaseFromMajor() + GetPhysicsOvercomeFromClass();
}

Value_t Attr::GetMagicOvercomeBaseFromCustom() const
{
    return m_magicOvercomeBaseFromCustom;
}

void Attr::SetMagicOvercomeBaseFromCustom(Value_t value)
{
    m_magicOvercomeBaseFromCustom = value;
    UpdateMagicOvercomePercent();
}

void Attr::AddMagicOvercomeBaseFromCustom(Value_t value)
{
    m_magicOvercomeBaseFromCustom += value;
    UpdateMagicOvercomePercent();
}

Value_t Attr::GetMagicOvercomeBaseFromMajor() const
{
    return m_spunk * JX3DPS_OVERCOME_BASE_PER_SPUNK;
}

Value_t Attr::GetMagicOvercomeBase() const
{
    return GetMagicOvercomeBaseFromCustom() + GetMagicOvercomeBaseFromMajor();
}

void Attr::SetMagicOvercomeBase(Value_t value)
{
    SetMagicOvercomeBaseFromCustom(value - GetMagicOvercomeBaseFromMajor());
}

void Attr::AddMagicOvercomeBase(Value_t value)
{
    AddMagicOvercomeBaseFromCustom(value);
}

BinPCT_t Attr::GetMagicOvercomeBaseBinPercent() const
{
    return m_magicOvercomeBaseBinPercent;
}

void Attr::SetMagicOvercomeBaseBinPercent(BinPCT_t binPercent)
{
    m_magicOvercomeBaseBinPercent = binPercent;
    UpdateMagicOvercomePercent();
}

void Attr::AddMagicOvercomeBaseBinPercent(BinPCT_t binPercent)
{
    m_magicOvercomeBaseBinPercent += binPercent;
    UpdateMagicOvercomePercent();
}

Value_t Attr::GetMagicOvercomeFromClass() const
{
    return (this->*m_magicOvercomeFromClass)();
}

Value_t Attr::GetMagicOvercome() const
{
    return m_magicOvercome;
}

PCT_t Attr::GetMagicOvercomePercent() const
{
    return m_magicOvercomePercent;
}

Value_t Attr::GetMagicOvercomeBaseMinimum() const
{
    return GetMagicOvercomeBaseFromMajor();
}

Value_t Attr::GetMagicOvercomeMinimum() const
{
    return GetMagicOvercomeBaseFromMajor() + GetMagicOvercomeFromClass();
}

Value_t Attr::GetHaste() const
{
    return m_haste;
}

void Attr::SetHaste(Value_t value)
{
    m_haste = value;
    UpdateHastePercent();
    return;
}

void Attr::AddHaste(Value_t value)
{
    m_haste += value;
    UpdateHastePercent();
    return;
}

BinPCT_t Attr::GetHasteBinPercent() const
{
    return m_hasteBinPercent;
}

void Attr::SetHasteBinPercent(BinPCT_t binPercent)
{
    m_hasteBinPercent = binPercent;
    UpdateHastePercent();
    return;
}

void Attr::AddHasteBinPercent(BinPCT_t binPercent)
{
    m_hasteBinPercent += binPercent;
    UpdateHastePercent();
    return;
}

PCT_t Attr::GetHastePercent() const
{
    return m_hastePercent;
}

PCT_t Attr::GetHastePercentVisible() const
{
    return m_hastePercentVisible;
}

Value_t Attr::GetStrain() const
{
    return m_strain;
}

void Attr::SetStrain(Value_t value)
{
    m_strain = value;
    UpdateStrainPercent();
    return;
}

void Attr::AddStrain(Value_t value)
{
    m_strain += value;
    UpdateStrainPercent();
    return;
}

PCT_t Attr::GetStrainPercent() const
{
    return m_strainPercent;
}

void Attr::AddStrainPercent(PCT_t percent)
{
    m_strainPercentFromCustom += percent;
    UpdateStrainPercent();
    return;
}

Value_t Attr::GetSurplusBaseFromCustom() const
{
    return m_surplusBaseFromCustom;
}

void Attr::SetSurplusBaseFromCustom(Value_t value)
{
    m_surplusBaseFromCustom = value;
    UpdateSurplusDamage();
}

void Attr::AddSurplusBaseFromCustom(Value_t value)
{
    m_surplusBaseFromCustom += value;
    UpdateSurplusDamage();
}

Value_t Attr::GetSurplusBaseFromMajor() const
{
    return 0;
}

Value_t Attr::GetSurplusBase() const
{
    return GetSurplusBaseFromCustom() + GetSurplusBaseFromMajor();
}

void Attr::SetSurplusBase(Value_t value)
{
    SetSurplusBaseFromCustom(value - GetSurplusBaseFromMajor());
}

void Attr::AddSurplusBase(Value_t value)
{
    AddSurplusBaseFromCustom(value);
}

BinPCT_t Attr::GetSurplusBaseBinPercent() const
{
    return m_surplusBaseBinPercent;
}

void Attr::SetSurplusBaseBinPercent(BinPCT_t binPercent)
{
    m_surplusBaseBinPercent = binPercent;
    UpdateSurplusDamage();
}

void Attr::AddSurplusBaseBinPercent(BinPCT_t binPercent)
{
    m_surplusBaseBinPercent += binPercent;
    UpdateSurplusDamage();
}

Value_t Attr::GetSurplusFromClass() const
{
    return (this->*m_surplusFromClass)();
}

Value_t Attr::GetSurplus() const
{
    return m_surplus;
}

Value_t Attr::GetSurplusBaseMinimum() const
{
    return GetSurplusBaseFromMajor();
}

Value_t Attr::GetSurplusMinimum() const
{
    return GetSurplusBaseFromMajor() + GetSurplusFromClass();
}

Value_t Attr::GetSurplusDamage() const
{
    return m_surplusDamage;
}

BinPCT_t Attr::GetShieldIgnoreBinPercent() const
{
    return m_shieldIgnoreBinPercent;
}

void Attr::SetShieldIgnoreBinPercent(Targets &targets, BinPCT_t binPercent)
{
    m_shieldIgnoreBinPercent = binPercent;
    UpdateShieldIgnorePercent(targets, binPercent);
    return;
}

void Attr::AddShieldIgnoreBinPercent(Targets &targets, BinPCT_t binPercent)
{
    m_shieldIgnoreBinPercent += binPercent;
    UpdateShieldIgnorePercent(targets, binPercent);
    return;
}

Value_t Attr::GetNoneFromClass() const
{
    return 0;
}

Value_t Attr::GetPhysicsAttackFromClassTaiXuJianYi() const
{
    return m_agility * JX3DPS_ATTACK_PER_AGILITY_FROM_TAI_XU_JIAN_YI;
}

Value_t Attr::GetPhysicsCriticalStrikeFromClassTaiXuJianYi() const
{
    return m_agility * JX3DPS_CRITICAL_STRIKE_PER_AGILITY_FROM_TAI_XU_JIAN_YI;
}

void Attr::UpdateAgility()
{
    m_agility = m_agilityBase * (JX3DPS_BIN_PCT + m_agilityBaseBinPercent) / JX3DPS_BIN_PCT;
    UpdatePhysicsAttack();
    UpdatePhysicsCriticalStrikePercent();
    UpdatePhysicsOvercomePercent();
}

void Attr::UpdateStrength()
{
    m_strength = m_strengthBase * (JX3DPS_BIN_PCT + m_strengthBaseBinPercent) / JX3DPS_BIN_PCT;
    UpdatePhysicsAttack();
    UpdatePhysicsCriticalStrikePercent();
    UpdatePhysicsOvercomePercent();
}

void Attr::UpdateSpirit()
{
    m_spirit = m_spiritBase * (JX3DPS_BIN_PCT + m_spiritBaseBinPercent) / JX3DPS_BIN_PCT;
    UpdateMagicAttack();
    UpdateMagicCriticalStrikePercent();
    UpdateMagicOvercomePercent();
}

void Attr::UpdateSpunk()
{
    m_spunk = m_spunkBase * (JX3DPS_BIN_PCT + m_spunkBaseBinPercent) / JX3DPS_BIN_PCT;
    UpdateMagicAttack();
    UpdateMagicCriticalStrikePercent();
    UpdateMagicOvercomePercent();
}

void Attr::UpdatePhysicsAttack()
{
    m_physicsAttack =
        (GetPhysicsAttackBaseFromMajor() + GetPhysicsAttackBaseFromCustom()) *     // 基础攻击
            (JX3DPS_BIN_PCT + GetPhysicsAttackBaseBinPercent()) / JX3DPS_BIN_PCT + // 基础攻击加成
        GetPhysicsAttackFromClass(); // 面板攻击加成
}

void Attr::UpdateMagicAttack()
{
    m_magicAttack = (GetMagicAttackBaseFromMajor() + GetMagicAttackBaseFromCustom()) * // 基础攻击
                        (JX3DPS_BIN_PCT + GetMagicAttackBaseBinPercent()) /
                        JX3DPS_BIN_PCT +       // 基础攻击加成
                    GetMagicAttackFromClass(); // 面板攻击加成
}

void Attr::UpdatePhysicsCriticalStrikePercent()
{
    m_physicsCriticalStrike = GetPhysicsCriticalStrikeFromCustom() +
                              GetPhysicsCriticalStrikeFromMajor() +
                              GetPhysicsCriticalStrikeFromClass();
    m_physicsCriticalStrikePercent =
        GetPhysicsCriticalStrike() / // 会心值
            (JX3DPS_CRITICAL_STRIKE_PARAM *
             (JX3DPS_LEVEL_PARAM * JX3DPS_PLAYER_LEVEL - JX3DPS_LEVEL_CONST)) + // 会心系数
        GetPhysicsCriticalStrikePercentFromCustom(); // 会心比例加成
}

void Attr::UpdateMagicCriticalStrikePercent()
{
    m_magicCriticalStrike = GetMagicCriticalStrikeFromCustom() + GetMagicCriticalStrikeFromMajor() +
                            GetMagicCriticalStrikeFromClass();
    m_magicCriticalStrikePercent =
        GetMagicCriticalStrike() / // 会心值
            (JX3DPS_CRITICAL_STRIKE_PARAM *
             (JX3DPS_LEVEL_PARAM * JX3DPS_PLAYER_LEVEL - JX3DPS_LEVEL_CONST)) + // 会心系数
        GetMagicCriticalStrikePercentFromCustom(); // 会心比例加成
}

void Attr::UpdatePhysicsCriticalStrikePowerPercent()
{
    m_physicsCriticalStrikePowerPercent =
        GetPhysicsCriticalStrikePower() /
            (JX3DPS_CRITICAL_STRIKE_POWER_PARAM *
             (JX3DPS_LEVEL_PARAM * JX3DPS_PLAYER_LEVEL - JX3DPS_LEVEL_CONST)) +
        GetPhysicsCriticalStrikePowerPercentFromCustom() +
        JX3DPS_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;
}

void Attr::UpdateMagicCriticalStrikePowerPercent()
{
    m_magicCriticalStrikePowerPercent =
        GetMagicCriticalStrikePower() /
            (JX3DPS_CRITICAL_STRIKE_POWER_PARAM *
             (JX3DPS_LEVEL_PARAM * JX3DPS_PLAYER_LEVEL - JX3DPS_LEVEL_CONST)) +
        GetMagicCriticalStrikePowerPercentFromCustom() +
        JX3DPS_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;
}

void Attr::UpdatePhysicsOvercomePercent()
{
    m_physicsOvercome =
        (GetPhysicsOvercomeBaseFromMajor() + GetPhysicsOvercomeBaseFromCustom()) * // 基础破防
            (JX3DPS_BIN_PCT + GetPhysicsOvercomeBaseBinPercent()) / JX3DPS_BIN_PCT + // 基础破防加成
        GetPhysicsOvercomeFromClass();                // 面板破防加成
    m_physicsOvercomePercent = GetPhysicsOvercome() / // 面板破防
                               (JX3DPS_OVERCOME_PARAM * (JX3DPS_LEVEL_PARAM * JX3DPS_PLAYER_LEVEL -
                                                         JX3DPS_LEVEL_CONST)); // 破防系数
}

void Attr::UpdateMagicOvercomePercent()
{
    m_magicOvercome =
        (GetMagicOvercomeBaseFromMajor() + GetMagicOvercomeBaseFromCustom()) *     // 基础破防
            (JX3DPS_BIN_PCT + GetMagicOvercomeBaseBinPercent()) / JX3DPS_BIN_PCT + // 基础破防加成
        GetMagicOvercomeFromClass();              // 面板破防加成
    m_magicOvercomePercent = GetMagicOvercome() / // 面板破防
                             (JX3DPS_OVERCOME_PARAM * (JX3DPS_LEVEL_PARAM * JX3DPS_PLAYER_LEVEL -
                                                       JX3DPS_LEVEL_CONST)); // 破防系数
}

void Attr::UpdateHastePercent()
{
    m_hastePercentVisible =
        m_haste /
        (JX3DPS_HASTE_RATE * (JX3DPS_LEVEL_PARAM * JX3DPS_PLAYER_LEVEL - JX3DPS_LEVEL_CONST));
    m_hastePercent = static_cast<double>(JX3DPS_BIN_PCT) /
                     (static_cast<int>(m_hastePercentVisible * JX3DPS_BIN_PCT) + m_hasteBinPercent +
                      JX3DPS_BIN_PCT);
}

void Attr::UpdateStrainPercent()
{
    m_strainPercent = m_strain / (JX3DPS_INSIGHT_PARAM *
                                  (JX3DPS_LEVEL_PARAM * JX3DPS_PLAYER_LEVEL - JX3DPS_LEVEL_CONST)) +
                      m_strainPercentFromCustom;
}

void Attr::UpdateSurplusDamage()
{
    m_surplus = (GetSurplusBaseFromMajor() + GetSurplusBaseFromCustom()) *
                    (JX3DPS_BIN_PCT + GetSurplusBaseBinPercent()) / JX3DPS_BIN_PCT +
                GetSurplusFromClass();
    m_surplusDamage = m_surplus * JX3DPS_SURPLUS_PARAM;
}

void Attr::UpdateShieldIgnorePercent(Targets &targets, BinPCT_t binPercent)
{
    for (auto &tars : targets) {
        for (auto &target : tars.second) {
            target->SetShieldIgnoreBinPercent(binPercent);
        }
    }
}

} // namespace JX3DPS
