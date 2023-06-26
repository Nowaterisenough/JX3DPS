/**
 * Project: JX3DPS
 * File: Attr.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-23 14:57:15
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Attr.h"

namespace JX3DPS {

Attr::Attr(Class classType)
{
    switch (classType) {
        case Class::TAI_XU_JIAN_YI:
            m_physicsAttackFromClass         = &Attr::GetPhysicsAttackFromClassTaiXuJianYi;
            m_physicsCriticalStrikeFromClass = &Attr::GetPhysicsCriticalStrikeFromClassTaiXuJianYi;
            break;
        default: break;
    }
}

Attr::~Attr() { }

Attr::Attr(const Attr &other)
{
    this->m_agilityBase           = other.m_agilityBase;
    this->m_agilityBaseBinPercent = other.m_agilityBaseBinPercent;
    this->m_agility               = other.m_agility;

    this->m_strengthBase           = other.m_strengthBase;
    this->m_strengthBaseBinPercent = other.m_strengthBaseBinPercent;
    this->m_strength               = other.m_strength;

    this->m_spiritBase           = other.m_spiritBase;
    this->m_spiritBaseBinPercent = other.m_spiritBaseBinPercent;
    this->m_spirit               = other.m_spirit;

    this->m_spunkBase           = other.m_spunkBase;
    this->m_spunkBaseBinPercent = other.m_spunkBaseBinPercent;
    this->m_spunk               = other.m_spunk;

    this->m_weaponAttack = other.m_weaponAttack;

    this->m_physicsAttackBaseFromCustom = other.m_physicsAttackBaseFromCustom;
    this->m_physicsAttackBaseBinPercent = other.m_physicsAttackBaseBinPercent;
    this->m_physicsAttackFromMain       = other.m_physicsAttackFromMain;
    this->m_physicsAttackFromBase       = other.m_physicsAttackFromBase;
    this->m_physicsAttack               = other.m_physicsAttack;

    this->m_magicAttackBaseFromCustom = other.m_magicAttackBaseFromCustom;
    this->m_magicAttackBaseBinPercent = other.m_magicAttackBaseBinPercent;
    this->m_magicAttackFromMain       = other.m_magicAttackFromMain;
    this->m_magicAttackFromBase       = other.m_magicAttackFromBase;
    this->m_magicAttack               = other.m_magicAttack;

    this->m_physicsCriticalStrike                  = other.m_physicsCriticalStrike;
    this->m_physicsCriticalStrikeFromCustom        = other.m_physicsCriticalStrikeFromCustom;
    this->m_physicsCriticalStrikePercentFromCustom = other.m_physicsCriticalStrikePercentFromCustom;
    this->m_physicsCriticalStrikePercent           = other.m_physicsCriticalStrikePercent;

    this->m_magicCriticalStrike                  = other.m_magicCriticalStrike;
    this->m_magicCriticalStrikeFromCustom        = other.m_magicCriticalStrikeFromCustom;
    this->m_magicCriticalStrikePercentFromCustom = other.m_magicCriticalStrikePercentFromCustom;
    this->m_magicCriticalStrikePercent           = other.m_magicCriticalStrikePercent;

    this->m_physicsCriticalStrikePower = other.m_physicsCriticalStrikePower;
    this->m_physicsCriticalStrikePowerPercentFromCustom = other.m_physicsCriticalStrikePowerPercentFromCustom;
    this->m_physicsCriticalStrikePowerPercent = other.m_physicsCriticalStrikePowerPercent;

    this->m_magicCriticalStrikePower = other.m_magicCriticalStrikePower;
    this->m_magicCriticalStrikePowerPercentFromCustom = other.m_magicCriticalStrikePowerPercentFromCustom;
    this->m_magicCriticalStrikePowerPercent = other.m_magicCriticalStrikePowerPercent;

    this->m_physicsOvercomeBaseFromCustom = other.m_physicsOvercomeBaseFromCustom;
    this->m_physicsOvercomeBaseBinPercent = other.m_physicsOvercomeBaseBinPercent;
    this->m_physicsOvercome               = other.m_physicsOvercome;
    this->m_physicsOvercomePercent        = other.m_physicsOvercomePercent;

    this->m_magicOvercomeBaseFromCustom = other.m_magicOvercomeBaseFromCustom;
    this->m_magicOvercomeBaseBinPercent = other.m_magicOvercomeBaseBinPercent;
    this->m_magicOvercome               = other.m_magicOvercome;
    this->m_magicOvercomePercent        = other.m_magicOvercomePercent;

    this->m_haste               = other.m_haste;
    this->m_hasteBinPercent     = other.m_hasteBinPercent;
    this->m_hastePercent        = other.m_hastePercent;
    this->m_hastePercentVisible = other.m_hastePercentVisible;

    this->m_strain                  = other.m_strain;
    this->m_strainPercentFromCustom = other.m_strainPercentFromCustom;
    this->m_strainPercent           = other.m_strainPercent;

    this->m_surplusBaseFromCustom = other.m_surplusBaseFromCustom;
    this->m_surplusBaseBinPercent = other.m_surplusBaseBinPercent;
    this->m_surplus               = other.m_surplus;
    this->m_surplusDamage         = other.m_surplusDamage;

    this->m_shieldIgnoreBinPercent = other.m_shieldIgnoreBinPercent;
}

Attr &Attr::operator=(const Attr &other)
{
    this->m_agilityBase           = other.m_agilityBase;
    this->m_agilityBaseBinPercent = other.m_agilityBaseBinPercent;
    this->m_agility               = other.m_agility;

    this->m_strengthBase           = other.m_strengthBase;
    this->m_strengthBaseBinPercent = other.m_strengthBaseBinPercent;
    this->m_strength               = other.m_strength;

    this->m_spiritBase           = other.m_spiritBase;
    this->m_spiritBaseBinPercent = other.m_spiritBaseBinPercent;
    this->m_spirit               = other.m_spirit;

    this->m_spunkBase           = other.m_spunkBase;
    this->m_spunkBaseBinPercent = other.m_spunkBaseBinPercent;
    this->m_spunk               = other.m_spunk;

    this->m_weaponAttack = other.m_weaponAttack;

    this->m_physicsAttackBaseFromCustom = other.m_physicsAttackBaseFromCustom;
    this->m_physicsAttackBaseBinPercent = other.m_physicsAttackBaseBinPercent;
    this->m_physicsAttackFromMain       = other.m_physicsAttackFromMain;
    this->m_physicsAttackFromBase       = other.m_physicsAttackFromBase;
    this->m_physicsAttack               = other.m_physicsAttack;

    this->m_magicAttackBaseFromCustom = other.m_magicAttackBaseFromCustom;
    this->m_magicAttackBaseBinPercent = other.m_magicAttackBaseBinPercent;
    this->m_magicAttackFromMain       = other.m_magicAttackFromMain;
    this->m_magicAttackFromBase       = other.m_magicAttackFromBase;
    this->m_magicAttack               = other.m_magicAttack;

    this->m_physicsCriticalStrike                  = other.m_physicsCriticalStrike;
    this->m_physicsCriticalStrikeFromCustom        = other.m_physicsCriticalStrikeFromCustom;
    this->m_physicsCriticalStrikePercentFromCustom = other.m_physicsCriticalStrikePercentFromCustom;
    this->m_physicsCriticalStrikePercent           = other.m_physicsCriticalStrikePercent;

    this->m_magicCriticalStrike                  = other.m_magicCriticalStrike;
    this->m_magicCriticalStrikeFromCustom        = other.m_magicCriticalStrikeFromCustom;
    this->m_magicCriticalStrikePercentFromCustom = other.m_magicCriticalStrikePercentFromCustom;
    this->m_magicCriticalStrikePercent           = other.m_magicCriticalStrikePercent;

    this->m_physicsCriticalStrikePower = other.m_physicsCriticalStrikePower;
    this->m_physicsCriticalStrikePowerPercentFromCustom = other.m_physicsCriticalStrikePowerPercentFromCustom;
    this->m_physicsCriticalStrikePowerPercent = other.m_physicsCriticalStrikePowerPercent;

    this->m_magicCriticalStrikePower = other.m_magicCriticalStrikePower;
    this->m_magicCriticalStrikePowerPercentFromCustom = other.m_magicCriticalStrikePowerPercentFromCustom;
    this->m_magicCriticalStrikePowerPercent = other.m_magicCriticalStrikePowerPercent;

    this->m_physicsOvercomeBaseFromCustom = other.m_physicsOvercomeBaseFromCustom;
    this->m_physicsOvercomeBaseBinPercent = other.m_physicsOvercomeBaseBinPercent;
    this->m_physicsOvercome               = other.m_physicsOvercome;
    this->m_physicsOvercomePercent        = other.m_physicsOvercomePercent;

    this->m_magicOvercomeBaseFromCustom = other.m_magicOvercomeBaseFromCustom;
    this->m_magicOvercomeBaseBinPercent = other.m_magicOvercomeBaseBinPercent;
    this->m_magicOvercome               = other.m_magicOvercome;
    this->m_magicOvercomePercent        = other.m_magicOvercomePercent;

    this->m_haste               = other.m_haste;
    this->m_hasteBinPercent     = other.m_hasteBinPercent;
    this->m_hastePercent        = other.m_hastePercent;
    this->m_hastePercentVisible = other.m_hastePercentVisible;

    this->m_strain                  = other.m_strain;
    this->m_strainPercentFromCustom = other.m_strainPercentFromCustom;
    this->m_strainPercent           = other.m_strainPercent;

    this->m_surplusBaseFromCustom = other.m_surplusBaseFromCustom;
    this->m_surplusBaseBinPercent = other.m_surplusBaseBinPercent;
    this->m_surplus               = other.m_surplus;
    this->m_surplusDamage         = other.m_surplusDamage;

    this->m_shieldIgnoreBinPercent = other.m_shieldIgnoreBinPercent;

    return *this;
}

Value_t Attr::GetAgilityBase() const
{
    return m_agilityBase;
}

void Attr::SetAgilityBase(const Value_t value)
{
    m_agilityBase = value;
    UpdateAgility();
}

void Attr::AddAgilityBase(const Value_t value)
{
    m_agilityBase += value;
    UpdateAgility();
}

PctInt_t Attr::GetAgilityBaseBinPercent() const
{
    return m_agilityBaseBinPercent;
}

void Attr::SetAgilityBaseBinPercent(const PctInt_t binPercent)
{
    m_agilityBaseBinPercent = binPercent;
    UpdateAgility();
}

void Attr::AddAgilityBaseBinPercent(const PctInt_t binPercent)
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

void Attr::SetStrengthBase(const Value_t value)
{
    m_strengthBase = value;
    UpdateStrength();
}

void Attr::AddStrengthBase(const Value_t value)
{
    m_strengthBase += value;
    UpdateStrength();
}

PctInt_t Attr::GetStrengthBaseBinPercent() const
{
    return m_strengthBaseBinPercent;
}

void Attr::SetStrengthBaseBinPercent(const PctInt_t binPercent)
{
    m_strengthBaseBinPercent = binPercent;
    UpdateStrength();
}

void Attr::AddStrengthBaseBinPercent(const PctInt_t binPercent)
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

void Attr::SetSpiritBase(const Value_t value)
{
    m_spiritBase = value;
    UpdateSpirit();
}

void Attr::AddSpiritBase(const Value_t value)
{
    m_spiritBase += value;
    UpdateSpirit();
}

PctInt_t Attr::GetSpiritBaseBinPercent() const
{
    return m_spiritBaseBinPercent;
}

void Attr::SetSpiritBaseBinPercent(const PctInt_t binPercent)
{
    m_spiritBaseBinPercent = binPercent;
    UpdateSpirit();
}

void Attr::AddSpiritBaseBinPercent(const PctInt_t binPercent)
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

void Attr::SetSpunkBase(const Value_t value)
{
    m_spunkBase = value;
    UpdateSpunk();
}

void Attr::AddSpunkBase(const Value_t value)
{
    m_spunkBase += value;
    UpdateSpunk();
}

PctInt_t Attr::GetSpunkBaseBinPercent() const
{
    return m_spunkBaseBinPercent;
}

void Attr::SetSpunkBaseBinPercent(const PctInt_t binPercent)
{
    m_spunkBaseBinPercent = binPercent;
    UpdateSpunk();
}

void Attr::AddSpunkBaseBinPercent(const PctInt_t binPercent)
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

void Attr::SetWeaponAttack(const Value_t lower, const Value_t upper)
{
    m_weaponAttack = (lower + upper) / 2;
}

void Attr::AddWeaponAttack(const Value_t value)
{
    m_weaponAttack += value;
}

void Attr::AddWeaponAttack(const Value_t lower, const Value_t upper)
{
    m_weaponAttack += (lower + upper) / 2;
}

Value_t Attr::GetPhysicsAttackBaseFromCustom() const
{
    return m_physicsAttackBaseFromCustom;
}

void Attr::SetPhysicsAttackBaseFromCustom(const Value_t value)
{
    m_physicsAttackBaseFromCustom = value;
    UpdatePhysicsAttack();
}

void Attr::AddPhysicsAttackBaseFromCustom(const Value_t value)
{
    m_physicsAttackBaseFromCustom += value;
    UpdatePhysicsAttack();
}

Value_t Attr::GetPhysicsAttackBaseFromMajor() const
{
    return m_strength * JX3_STRENGTH_TO_ATTACK_BASE;
}

Value_t Attr::GetPhysicsAttackBase() const
{
    return GetPhysicsAttackBaseFromCustom() + GetPhysicsAttackBaseFromMajor();
}

void Attr::SetPhysicsAttackBase(const Value_t value)
{
    SetPhysicsAttackBaseFromCustom(value - GetPhysicsAttackBaseFromMajor());
}

void Attr::AddPhysicsAttackBase(const Value_t value)
{
    AddPhysicsAttackBaseFromCustom(value);
}

PctInt_t Attr::GetPhysicsAttackBaseBinPercent() const
{
    return m_physicsAttackBaseBinPercent;
}

void Attr::SetPhysicsAttackBaseBinPercent(const PctInt_t binPercent)
{
    m_physicsAttackBaseBinPercent = binPercent;
    UpdatePhysicsAttack();
}

void Attr::AddPhysicsAttackBaseBinPercent(const PctInt_t binPercent)
{
    m_physicsAttackBaseBinPercent += binPercent;
    UpdatePhysicsAttack();
}

Value_t Attr::GetPhysicsAttackFromMain() const
{
    return m_physicsAttackFromMain;
}

Value_t Attr::GetPhysicsAttackFromBase() const
{
    return m_physicsAttackFromBase;
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
    return GetPhysicsAttackBaseFromMajor() + GetPhysicsAttackFromMain();
}

Value_t Attr::GetMagicAttackBaseFromCustom() const
{
    return m_magicAttackBaseFromCustom;
}

void Attr::SetMagicAttackBaseFromCustom(const Value_t value)
{
    m_magicAttackBaseFromCustom = value;
    UpdateMagicAttack();
}

void Attr::AddMagicAttackBaseFromCustom(const Value_t value)
{
    m_magicAttackBaseFromCustom += value;
    UpdateMagicAttack();
}

Value_t Attr::GetMagicAttackBaseFromMajor() const
{
    return m_spunk * JX3_SPUNK_TO_ATTACK_BASE;
}

Value_t Attr::GetMagicAttackBase() const
{
    return GetMagicAttackBaseFromCustom() + GetMagicAttackBaseFromMajor();
}

void Attr::SetMagicAttackBase(const Value_t value)
{
    SetMagicAttackBaseFromCustom(value - GetMagicAttackBaseFromMajor());
}

void Attr::AddMagicAttackBase(const Value_t value)
{
    AddMagicAttackBaseFromCustom(value);
}

PctInt_t Attr::GetMagicAttackBaseBinPercent() const
{
    return m_magicAttackBaseBinPercent;
}

void Attr::SetMagicAttackBaseBinPercent(const PctInt_t binPercent)
{
    m_magicAttackBaseBinPercent = binPercent;
    UpdateMagicAttack();
}

void Attr::AddMagicAttackBaseBinPercent(const PctInt_t binPercent)
{
    m_magicAttackBaseBinPercent += binPercent;
    UpdateMagicAttack();
}

Value_t Attr::GetMagicAttackFromMain() const
{
    return m_magicAttackFromMain;
}

Value_t Attr::GetMagicAttackFromBase() const
{
    return m_magicAttackFromBase;
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
    return GetMagicAttackBaseFromMajor() + GetMagicAttackFromMain();
}

Value_t Attr::GetPhysicsCriticalStrikeFromCustom() const
{
    return m_physicsCriticalStrikeFromCustom;
}

void Attr::SetPhysicsCriticalStrikeFromCustom(const Value_t value)
{
    m_physicsCriticalStrikeFromCustom = value;
    UpdatePhysicsCriticalStrikePercent();
}

void Attr::AddPhysicsCriticalStrikeFromCustom(const Value_t value)
{
    m_physicsCriticalStrikeFromCustom += value;
    UpdatePhysicsCriticalStrikePercent();
}

Value_t Attr::GetPhysicsCriticalStrikeFromMajor() const
{
    return m_agility * JX3_AGILITY_TO_CRITICAL_STRIKE;
}

Value_t Attr::GetPhysicsCriticalStrikeFromClass() const
{
    return (this->*m_physicsCriticalStrikeFromClass)();
}

Value_t Attr::GetPhysicsCriticalStrike() const
{
    return m_physicsCriticalStrike;
}

void Attr::SetPhysicsCriticalStrike(const Value_t value)
{
    SetPhysicsCriticalStrikeFromCustom(value - GetPhysicsCriticalStrikeMinimum());
}

void Attr::AddPhysicsCriticalStrike(const Value_t value)
{
    AddPhysicsCriticalStrikeFromCustom(value);
}

PctFloat_t Attr::GetPhysicsCriticalStrikePercentFromCustom() const
{
    return m_physicsCriticalStrikePercentFromCustom;
}

void Attr::SetPhysicsCriticalStrikePercentFromCustom(const PctFloat_t percent)
{
    m_physicsCriticalStrikePercentFromCustom = percent;
    UpdatePhysicsCriticalStrikePercent();
}

void Attr::AddPhysicsCriticalStrikePercentFromCustom(const PctFloat_t percent)
{
    m_physicsCriticalStrikePercentFromCustom += percent;
    UpdatePhysicsCriticalStrikePercent();
}

PctFloat_t Attr::GetPhysicsCriticalStrikePercent() const
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

void Attr::SetMagicCriticalStrikeFromCustom(const Value_t value)
{
    m_magicCriticalStrikeFromCustom = value;
    UpdateMagicCriticalStrikePercent();
}

void Attr::AddMagicCriticalStrikeFromCustom(const Value_t value)
{
    m_magicCriticalStrikeFromCustom += value;
    UpdateMagicCriticalStrikePercent();
}

Value_t Attr::GetMagicCriticalStrikeFromMajor() const
{
    return m_spirit * JX3_SPIRIT_TO_CRITICAL_STRIKE;
}

Value_t Attr::GetMagicCriticalStrikeFromClass() const
{
    return (this->*m_magicCriticalStrikeFromClass)();
}

Value_t Attr::GetMagicCriticalStrike() const
{
    return m_magicCriticalStrike;
}

void Attr::SetMagicCriticalStrike(const Value_t value)
{
    SetMagicCriticalStrikeFromCustom(value - GetMagicCriticalStrikeMinimum());
}

void Attr::AddMagicCriticalStrike(const Value_t value)
{
    AddMagicCriticalStrikeFromCustom(value);
}

PctFloat_t Attr::GetMagicCriticalStrikePercentFromCustom() const
{
    return m_magicCriticalStrikePercentFromCustom;
}

void Attr::SetMagicCriticalStrikePercentFromCustom(const PctFloat_t percent)
{
    m_magicCriticalStrikePercentFromCustom = percent;
    UpdateMagicCriticalStrikePercent();
}

void Attr::AddMagicCriticalStrikePercentFromCustom(const PctFloat_t percent)
{
    m_magicCriticalStrikePercentFromCustom += percent;
    UpdateMagicCriticalStrikePercent();
}

PctFloat_t Attr::GetMagicCriticalStrikePercent() const
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

void Attr::SetPhysicsCriticalStrikePower(const Value_t value)
{
    m_physicsCriticalStrikePower = value;
    UpdatePhysicsCriticalStrikePowerPercent();
}

void Attr::AddPhysicsCriticalStrikePower(const Value_t value)
{
    m_physicsCriticalStrikePower += value;
    UpdatePhysicsCriticalStrikePowerPercent();
}

PctFloat_t Attr::GetPhysicsCriticalStrikePowerPercentFromCustom() const
{
    return m_physicsCriticalStrikePowerPercentFromCustom;
}

void Attr::SetPhysicsCriticalStrikePowerPercentFromCustom(const PctFloat_t percent)
{
    m_physicsCriticalStrikePowerPercentFromCustom = percent;
    UpdatePhysicsCriticalStrikePowerPercent();
}

void Attr::AddPhysicsCriticalStrikePowerPercentFromCustom(const PctFloat_t percent)
{
    m_physicsCriticalStrikePowerPercentFromCustom += percent;
    UpdatePhysicsCriticalStrikePowerPercent();
}

PctFloat_t Attr::GetPhysicsCriticalStrikePowerPercent() const
{
    return m_physicsCriticalStrikePowerPercent;
}

Value_t Attr::GetMagicCriticalStrikePower() const
{
    return m_magicCriticalStrikePower;
}

void Attr::SetMagicCriticalStrikePower(const Value_t value)
{
    m_magicCriticalStrikePower = value;
    UpdateMagicCriticalStrikePowerPercent();
}

void Attr::AddMagicCriticalStrikePower(const Value_t value)
{
    m_magicCriticalStrikePower += value;
    UpdateMagicCriticalStrikePowerPercent();
}

PctFloat_t Attr::GetMagicCriticalStrikePowerPercentFromCustom() const
{
    return m_magicCriticalStrikePowerPercentFromCustom;
}

void Attr::SetMagicCriticalStrikePowerPercentFromCustom(const PctFloat_t percent)
{
    m_magicCriticalStrikePowerPercentFromCustom = percent;
    UpdateMagicCriticalStrikePowerPercent();
}

void Attr::AddMagicCriticalStrikePowerPercentFromCustom(const PctFloat_t percent)
{
    m_magicCriticalStrikePowerPercentFromCustom += percent;
    UpdateMagicCriticalStrikePowerPercent();
}

PctFloat_t Attr::GetMagicCriticalStrikePowerPercent() const
{
    return m_magicCriticalStrikePowerPercent;
}

Value_t Attr::GetPhysicsOvercomeBaseFromCustom() const
{
    return m_physicsOvercomeBaseFromCustom;
}

void Attr::SetPhysicsOvercomeBaseFromCustom(const Value_t value)
{
    m_physicsOvercomeBaseFromCustom = value;
    UpdatePhysicsOvercomePercent();
}

void Attr::AddPhysicsOvercomeBaseFromCustom(const Value_t value)
{
    m_physicsOvercomeBaseFromCustom += value;
    UpdatePhysicsOvercomePercent();
}

Value_t Attr::GetPhysicsOvercomeBaseFromMajor() const
{
    return m_strength * JX3_STRENGTH_TO_OVERCOME_BASE;
}

Value_t Attr::GetPhysicsOvercomeBase() const
{
    return GetPhysicsOvercomeBaseFromCustom() + GetPhysicsOvercomeBaseFromMajor();
}

void Attr::SetPhysicsOvercomeBase(const Value_t value)
{
    SetPhysicsOvercomeBaseFromCustom(value - GetPhysicsOvercomeBaseFromMajor());
}

void Attr::AddPhysicsOvercomeBase(const Value_t value)
{
    AddPhysicsOvercomeBaseFromCustom(value);
}

PctInt_t Attr::GetPhysicsOvercomeBaseBinPercent() const
{
    return m_physicsOvercomeBaseBinPercent;
}

void Attr::SetPhysicsOvercomeBaseBinPercent(const PctInt_t binPercent)
{
    m_physicsOvercomeBaseBinPercent = binPercent;
    UpdatePhysicsOvercomePercent();
}

void Attr::AddPhysicsOvercomeBaseBinPercent(const PctInt_t binPercent)
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

PctFloat_t Attr::GetPhysicsOvercomePercent() const
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

void Attr::SetMagicOvercomeBaseFromCustom(const Value_t value)
{
    m_magicOvercomeBaseFromCustom = value;
    UpdateMagicOvercomePercent();
}

void Attr::AddMagicOvercomeBaseFromCustom(const Value_t value)
{
    m_magicOvercomeBaseFromCustom += value;
    UpdateMagicOvercomePercent();
}

Value_t Attr::GetMagicOvercomeBaseFromMajor() const
{
    return m_spunk * JX3_SPUNK_TO_OVERCOME_BASE;
}

Value_t Attr::GetMagicOvercomeBase() const
{
    return GetMagicOvercomeBaseFromCustom() + GetMagicOvercomeBaseFromMajor();
}

void Attr::SetMagicOvercomeBase(const Value_t value)
{
    SetMagicOvercomeBaseFromCustom(value - GetMagicOvercomeBaseFromMajor());
}

void Attr::AddMagicOvercomeBase(const Value_t value)
{
    AddMagicOvercomeBaseFromCustom(value);
}

PctInt_t Attr::GetMagicOvercomeBaseBinPercent() const
{
    return m_magicOvercomeBaseBinPercent;
}

void Attr::SetMagicOvercomeBaseBinPercent(const PctInt_t binPercent)
{
    m_magicOvercomeBaseBinPercent = binPercent;
    UpdateMagicOvercomePercent();
}

void Attr::AddMagicOvercomeBaseBinPercent(const PctInt_t binPercent)
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

PctFloat_t Attr::GetMagicOvercomePercent() const
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

void Attr::SetHaste(const Value_t value)
{
    m_haste = value;
    UpdateHastePercent();
    return;
}

void Attr::AddHaste(const Value_t value)
{
    m_haste += value;
    UpdateHastePercent();
    return;
}

PctInt_t Attr::GetHasteBinPercent() const
{
    return m_hasteBinPercent;
}

void Attr::SetHasteBinPercent(const PctInt_t binPercent)
{
    m_hasteBinPercent = binPercent;
    UpdateHastePercent();
    return;
}

void Attr::AddHasteBinPercent(const PctInt_t binPercent)
{
    m_hasteBinPercent += binPercent;
    UpdateHastePercent();
    return;
}

PctFloat_t Attr::GetHastePercent() const
{
    return m_hastePercent;
}

PctFloat_t Attr::GetHastePercentVisible() const
{
    return m_hastePercentVisible;
}

Value_t Attr::GetStrain() const
{
    return m_strain;
}

void Attr::SetStrain(const Value_t value)
{
    m_strain = value;
    UpdateStrainPercent();
    return;
}

void Attr::AddStrain(const Value_t value)
{
    m_strain += value;
    UpdateStrainPercent();
    return;
}

PctFloat_t Attr::GetStrainPercent() const
{
    return m_strainPercent;
}

void Attr::AddStrainPercent(const PctFloat_t percent)
{
    m_strainPercentFromCustom += percent;
    UpdateStrainPercent();
    return;
}

Value_t Attr::GetSurplusBaseFromCustom() const
{
    return m_surplusBaseFromCustom;
}

void Attr::SetSurplusBaseFromCustom(const Value_t value)
{
    m_surplusBaseFromCustom = value;
    UpdateSurplusDamage();
}

void Attr::AddSurplusBaseFromCustom(const Value_t value)
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

void Attr::SetSurplusBase(const Value_t value)
{
    SetSurplusBaseFromCustom(value - GetSurplusBaseFromMajor());
}

void Attr::AddSurplusBase(const Value_t value)
{
    AddSurplusBaseFromCustom(value);
}

PctInt_t Attr::GetSurplusBaseBinPercent() const
{
    return m_surplusBaseBinPercent;
}

void Attr::SetSurplusBaseBinPercent(const PctInt_t binPercent)
{
    m_surplusBaseBinPercent = binPercent;
    UpdateSurplusDamage();
}

void Attr::AddSurplusBaseBinPercent(const PctInt_t binPercent)
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

PctInt_t Attr::GetShieldIgnoreBinPercent() const
{
    return m_shieldIgnoreBinPercent;
}

void Attr::SetShieldIgnoreBinPercent(Targets *targets, const PctInt_t binPercent)
{
    m_shieldIgnoreBinPercent = binPercent;
    UpdateShieldIgnorePercent(targets, m_shieldIgnoreBinPercent);
    return;
}

void Attr::AddShieldIgnoreBinPercent(Targets *targets, const PctInt_t binPercent)
{
    m_shieldIgnoreBinPercent += binPercent;
    UpdateShieldIgnorePercent(targets, m_shieldIgnoreBinPercent);
    return;
}

Value_t Attr::GetNoneFromClass() const
{
    return 0;
}

Value_t Attr::GetPhysicsAttackFromClassTaiXuJianYi() const
{
    return m_agility * JX3_AGILITY_TO_ATTACK_TAI_XU_JIAN_YI;
}

Value_t Attr::GetPhysicsCriticalStrikeFromClassTaiXuJianYi() const
{
    return m_agility * JX3_AGILITY_TO_CRITICAL_STRIKE_TAI_XU_JIAN_YI;
}

void Attr::UpdateAgility()
{
    m_agility = m_agilityBase * (JX3_BIN_PCT + m_agilityBaseBinPercent) / JX3_BIN_PCT;
    UpdatePhysicsAttack();
    UpdatePhysicsCriticalStrikePercent();
    UpdatePhysicsOvercomePercent();
}

void Attr::UpdateStrength()
{
    m_strength = m_strengthBase * (JX3_BIN_PCT + m_strengthBaseBinPercent) / JX3_BIN_PCT;
    UpdatePhysicsAttack();
    UpdatePhysicsCriticalStrikePercent();
    UpdatePhysicsOvercomePercent();
}

void Attr::UpdateSpirit()
{
    m_spirit = m_spiritBase * (JX3_BIN_PCT + m_spiritBaseBinPercent) / JX3_BIN_PCT;
    UpdateMagicAttack();
    UpdateMagicCriticalStrikePercent();
    UpdateMagicOvercomePercent();
}

void Attr::UpdateSpunk()
{
    m_spunk = m_spunkBase * (JX3_BIN_PCT + m_spunkBaseBinPercent) / JX3_BIN_PCT;
    UpdateMagicAttack();
    UpdateMagicCriticalStrikePercent();
    UpdateMagicOvercomePercent();
}

void Attr::UpdatePhysicsAttack()
{
    m_physicsAttackFromBase =
        (GetPhysicsAttackBaseFromMajor() + GetPhysicsAttackBaseFromCustom()) * // 基础攻击
        (JX3_BIN_PCT + GetPhysicsAttackBaseBinPercent()) / JX3_BIN_PCT;        // 基础攻击加成
    m_physicsAttackFromMain = (this->*m_physicsAttackFromClass)();             // 面板攻击加成
    m_physicsAttack         = m_physicsAttackFromBase + m_physicsAttackFromMain;
}

void Attr::UpdateMagicAttack()
{
    m_magicAttackFromBase = (GetMagicAttackBaseFromMajor() + GetMagicAttackBaseFromCustom()) * // 基础攻击
                            (JX3_BIN_PCT + GetMagicAttackBaseBinPercent()) / JX3_BIN_PCT; // 基础攻击加成
    m_magicAttackFromMain = (this->*m_magicAttackFromClass)(); // 面板攻击加成
    m_magicAttack         = m_magicAttackFromBase + m_magicAttackFromMain;
}

void Attr::UpdatePhysicsCriticalStrikePercent()
{
    m_physicsCriticalStrike = GetPhysicsCriticalStrikeFromCustom() +
                              GetPhysicsCriticalStrikeFromMajor() + GetPhysicsCriticalStrikeFromClass();
    m_physicsCriticalStrikePercent =
        GetPhysicsCriticalStrike() / // 会心值
            (JX3_CRITICAL_STRIKE_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) + // 会心系数
        GetPhysicsCriticalStrikePercentFromCustom(); // 会心比例加成
}

void Attr::UpdateMagicCriticalStrikePercent()
{
    m_magicCriticalStrike = GetMagicCriticalStrikeFromCustom() + GetMagicCriticalStrikeFromMajor() +
                            GetMagicCriticalStrikeFromClass();
    m_magicCriticalStrikePercent =
        GetMagicCriticalStrike() / // 会心值
            (JX3_CRITICAL_STRIKE_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) + // 会心系数
        GetMagicCriticalStrikePercentFromCustom(); // 会心比例加成
}

void Attr::UpdatePhysicsCriticalStrikePowerPercent()
{
    m_physicsCriticalStrikePowerPercent =
        GetPhysicsCriticalStrikePower() /
            (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) +
        GetPhysicsCriticalStrikePowerPercentFromCustom() + JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;
}

void Attr::UpdateMagicCriticalStrikePowerPercent()
{
    m_magicCriticalStrikePowerPercent =
        GetMagicCriticalStrikePower() /
            (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) +
        GetMagicCriticalStrikePowerPercentFromCustom() + JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;
}

void Attr::UpdatePhysicsOvercomePercent()
{
    m_physicsOvercome =
        (GetPhysicsOvercomeBaseFromMajor() + GetPhysicsOvercomeBaseFromCustom()) * // 基础破防
            (JX3_BIN_PCT + GetPhysicsOvercomeBaseBinPercent()) / JX3_BIN_PCT + // 基础破防加成
        GetPhysicsOvercomeFromClass();                                         // 面板破防加成
    m_physicsOvercomePercent =
        GetPhysicsOvercome() /                                                 // 面板破防
        (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)); // 破防系数
}

void Attr::UpdateMagicOvercomePercent()
{
    m_magicOvercome = (GetMagicOvercomeBaseFromMajor() + GetMagicOvercomeBaseFromCustom()) * // 基础破防
                          (JX3_BIN_PCT + GetMagicOvercomeBaseBinPercent()) / JX3_BIN_PCT + // 基础破防加成
                      GetMagicOvercomeFromClass(); // 面板破防加成
    m_magicOvercomePercent =
        GetMagicOvercome() /                       // 面板破防
        (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)); // 破防系数
}

void Attr::UpdateHastePercent()
{
    m_hastePercentVisible =
        m_haste / (JX3_HASTE_RATE * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
    m_hastePercent =
        static_cast<double>(JX3_BIN_PCT) /
        (static_cast<int>(m_hastePercentVisible * JX3_BIN_PCT) + m_hasteBinPercent + JX3_BIN_PCT);
}

void Attr::UpdateStrainPercent()
{
    m_strainPercent =
        m_strain / (JX3_INSIGHT_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) + m_strainPercentFromCustom;
}

void Attr::UpdateSurplusDamage()
{
    m_surplus = (GetSurplusBaseFromMajor() + GetSurplusBaseFromCustom()) *
                    (JX3_BIN_PCT + GetSurplusBaseBinPercent()) / JX3_BIN_PCT +
                GetSurplusFromClass();
    m_surplusDamage = m_surplus * JX3_SURPLUS_PARAM;
}

void Attr::UpdateShieldIgnorePercent(Targets *targets, const PctInt_t binPercent)
{
    for (auto &target : *targets) {

        target.second->SetShieldIgnoreBinPercent(binPercent);
    }
}

} // namespace JX3DPS
