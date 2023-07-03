/**
 * Project: JX3DPS
 * File: Attr.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-04 06:30:54
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
    this->m_agilityBasePercentInt = other.m_agilityBasePercentInt;
    this->m_agility               = other.m_agility;

    this->m_strengthBase           = other.m_strengthBase;
    this->m_strengthBasePercentInt = other.m_strengthBasePercentInt;
    this->m_strength               = other.m_strength;

    this->m_spiritBase           = other.m_spiritBase;
    this->m_spiritBasePercentInt = other.m_spiritBasePercentInt;
    this->m_spirit               = other.m_spirit;

    this->m_spunkBase           = other.m_spunkBase;
    this->m_spunkBasePercentInt = other.m_spunkBasePercentInt;
    this->m_spunk               = other.m_spunk;

    this->m_weaponAttack      = other.m_weaponAttack;
    this->m_weaponAttackLower = other.m_weaponAttackLower;
    this->m_weaponAttackUpper = other.m_weaponAttackUpper;

    this->m_physicsAttackBaseFromCustom = other.m_physicsAttackBaseFromCustom;
    this->m_physicsAttackBasePercentInt = other.m_physicsAttackBasePercentInt;
    this->m_physicsAttackFromMain       = other.m_physicsAttackFromMain;
    this->m_physicsAttackFromBase       = other.m_physicsAttackFromBase;
    this->m_physicsAttack               = other.m_physicsAttack;

    this->m_magicAttackBaseFromCustom = other.m_magicAttackBaseFromCustom;
    this->m_magicAttackBasePercentInt = other.m_magicAttackBasePercentInt;
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
    this->m_physicsOvercomeBasePercentInt = other.m_physicsOvercomeBasePercentInt;
    this->m_physicsOvercome               = other.m_physicsOvercome;
    this->m_physicsOvercomePercent        = other.m_physicsOvercomePercent;

    this->m_magicOvercomeBaseFromCustom = other.m_magicOvercomeBaseFromCustom;
    this->m_magicOvercomeBasePercentInt = other.m_magicOvercomeBasePercentInt;
    this->m_magicOvercome               = other.m_magicOvercome;
    this->m_magicOvercomePercent        = other.m_magicOvercomePercent;

    this->m_haste               = other.m_haste;
    this->m_hastePercentInt     = other.m_hastePercentInt;
    this->m_hastePercent        = other.m_hastePercent;
    this->m_hastePercentVisible = other.m_hastePercentVisible;

    this->m_strain                  = other.m_strain;
    this->m_strainPercentFromCustom = other.m_strainPercentFromCustom;
    this->m_strainPercent           = other.m_strainPercent;

    this->m_surplusBaseFromCustom = other.m_surplusBaseFromCustom;
    this->m_surplusBasePercentInt = other.m_surplusBasePercentInt;
    this->m_surplus               = other.m_surplus;
    this->m_surplusDamage         = other.m_surplusDamage;

    this->m_shieldIgnorePercentInt = other.m_shieldIgnorePercentInt;
}

Attr &Attr::operator=(const Attr &other)
{
    this->m_agilityBase           = other.m_agilityBase;
    this->m_agilityBasePercentInt = other.m_agilityBasePercentInt;
    this->m_agility               = other.m_agility;

    this->m_strengthBase           = other.m_strengthBase;
    this->m_strengthBasePercentInt = other.m_strengthBasePercentInt;
    this->m_strength               = other.m_strength;

    this->m_spiritBase           = other.m_spiritBase;
    this->m_spiritBasePercentInt = other.m_spiritBasePercentInt;
    this->m_spirit               = other.m_spirit;

    this->m_spunkBase           = other.m_spunkBase;
    this->m_spunkBasePercentInt = other.m_spunkBasePercentInt;
    this->m_spunk               = other.m_spunk;

    this->m_weaponAttack      = other.m_weaponAttack;
    this->m_weaponAttackLower = other.m_weaponAttackLower;
    this->m_weaponAttackUpper = other.m_weaponAttackUpper;

    this->m_physicsAttackBaseFromCustom = other.m_physicsAttackBaseFromCustom;
    this->m_physicsAttackBasePercentInt = other.m_physicsAttackBasePercentInt;
    this->m_physicsAttackFromMain       = other.m_physicsAttackFromMain;
    this->m_physicsAttackFromBase       = other.m_physicsAttackFromBase;
    this->m_physicsAttack               = other.m_physicsAttack;

    this->m_magicAttackBaseFromCustom = other.m_magicAttackBaseFromCustom;
    this->m_magicAttackBasePercentInt = other.m_magicAttackBasePercentInt;
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
    this->m_physicsOvercomeBasePercentInt = other.m_physicsOvercomeBasePercentInt;
    this->m_physicsOvercome               = other.m_physicsOvercome;
    this->m_physicsOvercomePercent        = other.m_physicsOvercomePercent;

    this->m_magicOvercomeBaseFromCustom = other.m_magicOvercomeBaseFromCustom;
    this->m_magicOvercomeBasePercentInt = other.m_magicOvercomeBasePercentInt;
    this->m_magicOvercome               = other.m_magicOvercome;
    this->m_magicOvercomePercent        = other.m_magicOvercomePercent;

    this->m_haste               = other.m_haste;
    this->m_hastePercentInt     = other.m_hastePercentInt;
    this->m_hastePercent        = other.m_hastePercent;
    this->m_hastePercentVisible = other.m_hastePercentVisible;

    this->m_strain                  = other.m_strain;
    this->m_strainPercentFromCustom = other.m_strainPercentFromCustom;
    this->m_strainPercent           = other.m_strainPercent;

    this->m_surplusBaseFromCustom = other.m_surplusBaseFromCustom;
    this->m_surplusBasePercentInt = other.m_surplusBasePercentInt;
    this->m_surplus               = other.m_surplus;
    this->m_surplusDamage         = other.m_surplusDamage;

    this->m_shieldIgnorePercentInt = other.m_shieldIgnorePercentInt;

    return *this;
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

PctInt_t Attr::GetAgilityBasePercentInt() const
{
    return m_agilityBasePercentInt;
}

void Attr::SetAgilityBasePercentInt(PctInt_t PercentInt)
{
    m_agilityBasePercentInt = PercentInt;
    UpdateAgility();
}

void Attr::AddAgilityBasePercentInt(PctInt_t PercentInt)
{
    m_agilityBasePercentInt += PercentInt;
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

PctInt_t Attr::GetStrengthBasePercentInt() const
{
    return m_strengthBasePercentInt;
}

void Attr::SetStrengthBasePercentInt(PctInt_t PercentInt)
{
    m_strengthBasePercentInt = PercentInt;
    UpdateStrength();
}

void Attr::AddStrengthBasePercentInt(PctInt_t PercentInt)
{
    m_strengthBasePercentInt += PercentInt;
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

PctInt_t Attr::GetSpiritBasePercentInt() const
{
    return m_spiritBasePercentInt;
}

void Attr::SetSpiritBasePercentInt(PctInt_t PercentInt)
{
    m_spiritBasePercentInt = PercentInt;
    UpdateSpirit();
}

void Attr::AddSpiritBasePercentInt(PctInt_t PercentInt)
{
    m_spiritBasePercentInt += PercentInt;
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

PctInt_t Attr::GetSpunkBasePercentInt() const
{
    return m_spunkBasePercentInt;
}

void Attr::SetSpunkBasePercentInt(PctInt_t PercentInt)
{
    m_spunkBasePercentInt = PercentInt;
    UpdateSpunk();
}

void Attr::AddSpunkBasePercentInt(PctInt_t PercentInt)
{
    m_spunkBasePercentInt += PercentInt;
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
    m_weaponAttackLower = lower;
    m_weaponAttackUpper = upper;
    m_weaponAttack      = (lower + upper) / 2;
}

void Attr::AddWeaponAttack(Value_t value)
{
    m_weaponAttack += value;
}

void Attr::AddWeaponAttack(Value_t lower, Value_t upper)
{
    m_weaponAttack += (lower + upper) / 2;
}

Value_t Attr::GetWeaponAttackLower() const
{
    return m_weaponAttackLower;
}

Value_t Attr::GetWeaponAttackUpper() const
{
    return m_weaponAttackUpper;
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
    return m_strength * JX3_STRENGTH_TO_ATTACK_BASE;
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

PctInt_t Attr::GetPhysicsAttackBasePercentInt() const
{
    return m_physicsAttackBasePercentInt;
}

void Attr::SetPhysicsAttackBasePercentInt(PctInt_t PercentInt)
{
    m_physicsAttackBasePercentInt = PercentInt;
    UpdatePhysicsAttack();
}

void Attr::AddPhysicsAttackBasePercentInt(PctInt_t PercentInt)
{
    m_physicsAttackBasePercentInt += PercentInt;
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
    return m_spunk * JX3_SPUNK_TO_ATTACK_BASE;
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

PctInt_t Attr::GetMagicAttackBasePercentInt() const
{
    return m_magicAttackBasePercentInt;
}

void Attr::SetMagicAttackBasePercentInt(PctInt_t PercentInt)
{
    m_magicAttackBasePercentInt = PercentInt;
    UpdateMagicAttack();
}

void Attr::AddMagicAttackBasePercentInt(PctInt_t PercentInt)
{
    m_magicAttackBasePercentInt += PercentInt;
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

void Attr::SetPhysicsCriticalStrike(Value_t value)
{
    SetPhysicsCriticalStrikeFromCustom(value - GetPhysicsCriticalStrikeMinimum());
}

void Attr::AddPhysicsCriticalStrike(Value_t value)
{
    AddPhysicsCriticalStrikeFromCustom(value);
}

PctFloat_t Attr::GetPhysicsCriticalStrikePercentFromCustom() const
{
    return m_physicsCriticalStrikePercentFromCustom;
}

void Attr::SetPhysicsCriticalStrikePercentFromCustom(PctFloat_t percent)
{
    m_physicsCriticalStrikePercentFromCustom = percent;
    UpdatePhysicsCriticalStrikePercent();
}

void Attr::AddPhysicsCriticalStrikePercentFromCustom(PctFloat_t percent)
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
    return GetPhysicsCriticalStrikeFromMajor() + GetPhysicsCriticalStrikeFromClass();
}

void Attr::SetBoxPhysicsCriticalStrikePercent(PctFloat_t percent)
{
    m_physicsCriticalStrikeFromCustom =
        1 + percent * (JX3_CRITICAL_STRIKE_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) -
        GetPhysicsCriticalStrikeFromClass() - GetPhysicsCriticalStrikeFromMajor();
    UpdatePhysicsCriticalStrikePercent();
    if (GetPhysicsCriticalStrikePercent() != percent) {
        m_physicsCriticalStrikeFromCustom =
            percent * (JX3_CRITICAL_STRIKE_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) -
            GetPhysicsCriticalStrikeFromClass() - GetPhysicsCriticalStrikeFromMajor();
        UpdatePhysicsCriticalStrikePercent();
    }
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

void Attr::SetMagicCriticalStrike(Value_t value)
{
    SetMagicCriticalStrikeFromCustom(value - GetMagicCriticalStrikeMinimum());
}

void Attr::AddMagicCriticalStrike(Value_t value)
{
    AddMagicCriticalStrikeFromCustom(value);
}

PctFloat_t Attr::GetMagicCriticalStrikePercentFromCustom() const
{
    return m_magicCriticalStrikePercentFromCustom;
}

void Attr::SetMagicCriticalStrikePercentFromCustom(PctFloat_t percent)
{
    m_magicCriticalStrikePercentFromCustom = percent;
    UpdateMagicCriticalStrikePercent();
}

void Attr::AddMagicCriticalStrikePercentFromCustom(PctFloat_t percent)
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
    return GetMagicCriticalStrikeFromMajor() + GetMagicCriticalStrikeFromClass();
}

void Attr::SetBoxMagicCriticalStrikePercent(PctFloat_t percent)
{
    m_magicCriticalStrikeFromCustom =
        1 + percent * (JX3_CRITICAL_STRIKE_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) -
        GetMagicCriticalStrikeFromClass() - GetMagicCriticalStrikeFromMajor();
    UpdateMagicCriticalStrikePercent();
    if (GetMagicCriticalStrikePercent() != percent) {
        m_magicCriticalStrikeFromCustom =
            percent * (JX3_CRITICAL_STRIKE_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) -
            GetMagicCriticalStrikeFromClass() - GetMagicCriticalStrikeFromMajor();
        UpdateMagicCriticalStrikePercent();
    }
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

PctFloat_t Attr::GetPhysicsCriticalStrikePowerPercentFromCustom() const
{
    return m_physicsCriticalStrikePowerPercentFromCustom;
}

void Attr::SetPhysicsCriticalStrikePowerPercentFromCustom(PctFloat_t percent)
{
    m_physicsCriticalStrikePowerPercentFromCustom = percent;
    UpdatePhysicsCriticalStrikePowerPercent();
}

void Attr::AddPhysicsCriticalStrikePowerPercentFromCustom(PctFloat_t percent)
{
    m_physicsCriticalStrikePowerPercentFromCustom += percent;
    UpdatePhysicsCriticalStrikePowerPercent();
}

PctFloat_t Attr::GetPhysicsCriticalStrikePowerPercent() const
{
    return m_physicsCriticalStrikePowerPercent;
}

void Attr::SetBoxPhysicsCriticalStrikePowerPercent(PctFloat_t percent)
{
    m_physicsCriticalStrikePower =
        1 + (percent - JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE) *
                (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
    UpdatePhysicsCriticalStrikePowerPercent();
    if (GetPhysicsCriticalStrikePowerPercent() != percent) {
        m_physicsCriticalStrikePower =
            (percent - JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE) *
            (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
        UpdatePhysicsCriticalStrikePowerPercent();
    }
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

PctFloat_t Attr::GetMagicCriticalStrikePowerPercentFromCustom() const
{
    return m_magicCriticalStrikePowerPercentFromCustom;
}

void Attr::SetMagicCriticalStrikePowerPercentFromCustom(PctFloat_t percent)
{
    m_magicCriticalStrikePowerPercentFromCustom = percent;
    UpdateMagicCriticalStrikePowerPercent();
}

void Attr::AddMagicCriticalStrikePowerPercentFromCustom(PctFloat_t percent)
{
    m_magicCriticalStrikePowerPercentFromCustom += percent;
    UpdateMagicCriticalStrikePowerPercent();
}

PctFloat_t Attr::GetMagicCriticalStrikePowerPercent() const
{
    return m_magicCriticalStrikePowerPercent;
}

void Attr::SetBoxMagicCriticalStrikePowerPercent(PctFloat_t percent)
{
    m_magicCriticalStrikePower =
        1 + (percent - JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE) *
                (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
    UpdateMagicCriticalStrikePowerPercent();
    if (GetMagicCriticalStrikePowerPercent() != percent) {
        m_magicCriticalStrikePower =
            (percent - JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE) *
            (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
        UpdateMagicCriticalStrikePowerPercent();
    }
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
    return m_strength * JX3_STRENGTH_TO_OVERCOME_BASE;
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

PctInt_t Attr::GetPhysicsOvercomeBasePercentInt() const
{
    return m_physicsOvercomeBasePercentInt;
}

void Attr::SetPhysicsOvercomeBasePercentInt(PctInt_t PercentInt)
{
    m_physicsOvercomeBasePercentInt = PercentInt;
    UpdatePhysicsOvercomePercent();
}

void Attr::AddPhysicsOvercomeBasePercentInt(PctInt_t PercentInt)
{
    m_physicsOvercomeBasePercentInt += PercentInt;
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
    return m_spunk * JX3_SPUNK_TO_OVERCOME_BASE;
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

PctInt_t Attr::GetMagicOvercomeBasePercentInt() const
{
    return m_magicOvercomeBasePercentInt;
}

void Attr::SetMagicOvercomeBasePercentInt(PctInt_t PercentInt)
{
    m_magicOvercomeBasePercentInt = PercentInt;
    UpdateMagicOvercomePercent();
}

void Attr::AddMagicOvercomeBasePercentInt(PctInt_t PercentInt)
{
    m_magicOvercomeBasePercentInt += PercentInt;
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

PctInt_t Attr::GetHastePercentInt() const
{
    return m_hastePercentInt;
}

void Attr::SetHastePercentInt(PctInt_t PercentInt)
{
    m_hastePercentInt = PercentInt;
    UpdateHastePercent();
    return;
}

void Attr::AddHastePercentInt(PctInt_t PercentInt)
{
    m_hastePercentInt += PercentInt;
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

PctFloat_t Attr::GetStrainPercent() const
{
    return m_strainPercent;
}

void Attr::AddStrainPercent(PctFloat_t percent)
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

PctInt_t Attr::GetSurplusBasePercentInt() const
{
    return m_surplusBasePercentInt;
}

void Attr::SetSurplusBasePercentInt(PctInt_t PercentInt)
{
    m_surplusBasePercentInt = PercentInt;
    UpdateSurplusDamage();
}

void Attr::AddSurplusBasePercentInt(PctInt_t PercentInt)
{
    m_surplusBasePercentInt += PercentInt;
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

PctInt_t Attr::GetShieldIgnorePercentInt() const
{
    return m_shieldIgnorePercentInt;
}

void Attr::SetShieldIgnorePercentInt(Targets *targets, PctInt_t PercentInt)
{
    m_shieldIgnorePercentInt = PercentInt;
    UpdateShieldIgnorePercent(targets, m_shieldIgnorePercentInt);
    return;
}

void Attr::AddShieldIgnorePercentInt(Targets *targets, PctInt_t PercentInt)
{
    m_shieldIgnorePercentInt += PercentInt;
    UpdateShieldIgnorePercent(targets, m_shieldIgnorePercentInt);
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
    m_agility = m_agilityBase * (JX3_PCT_INT_BASE + m_agilityBasePercentInt) / JX3_PCT_INT_BASE;
    UpdatePhysicsAttack();
    UpdatePhysicsCriticalStrikePercent();
    UpdatePhysicsOvercomePercent();
}

void Attr::UpdateStrength()
{
    m_strength = m_strengthBase * (JX3_PCT_INT_BASE + m_strengthBasePercentInt) / JX3_PCT_INT_BASE;
    UpdatePhysicsAttack();
    UpdatePhysicsCriticalStrikePercent();
    UpdatePhysicsOvercomePercent();
}

void Attr::UpdateSpirit()
{
    m_spirit = m_spiritBase * (JX3_PCT_INT_BASE + m_spiritBasePercentInt) / JX3_PCT_INT_BASE;
    UpdateMagicAttack();
    UpdateMagicCriticalStrikePercent();
    UpdateMagicOvercomePercent();
}

void Attr::UpdateSpunk()
{
    m_spunk = m_spunkBase * (JX3_PCT_INT_BASE + m_spunkBasePercentInt) / JX3_PCT_INT_BASE;
    UpdateMagicAttack();
    UpdateMagicCriticalStrikePercent();
    UpdateMagicOvercomePercent();
}

void Attr::UpdatePhysicsAttack()
{
    m_physicsAttackFromBase =
        (GetPhysicsAttackBaseFromMajor() + GetPhysicsAttackBaseFromCustom()) *    // 基础攻击
        (JX3_PCT_INT_BASE + GetPhysicsAttackBasePercentInt()) / JX3_PCT_INT_BASE; // 基础攻击加成
    m_physicsAttackFromMain = (this->*m_physicsAttackFromClass)(); // 面板攻击加成
    m_physicsAttack         = m_physicsAttackFromBase + m_physicsAttackFromMain;
}

void Attr::UpdateMagicAttack()
{
    m_magicAttackFromBase = (GetMagicAttackBaseFromMajor() + GetMagicAttackBaseFromCustom()) * // 基础攻击
                            (JX3_PCT_INT_BASE + GetMagicAttackBasePercentInt()) / JX3_PCT_INT_BASE; // 基础攻击加成
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
            (JX3_PCT_INT_BASE + GetPhysicsOvercomeBasePercentInt()) / JX3_PCT_INT_BASE + // 基础破防加成
        GetPhysicsOvercomeFromClass(); // 面板破防加成
    m_physicsOvercomePercent =
        GetPhysicsOvercome() /         // 面板破防
        (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)); // 破防系数
}

void Attr::UpdateMagicOvercomePercent()
{
    m_magicOvercome =
        (GetMagicOvercomeBaseFromMajor() + GetMagicOvercomeBaseFromCustom()) * // 基础破防
            (JX3_PCT_INT_BASE + GetMagicOvercomeBasePercentInt()) / JX3_PCT_INT_BASE + // 基础破防加成
        GetMagicOvercomeFromClass(); // 面板破防加成
    m_magicOvercomePercent =
        GetMagicOvercome() /         // 面板破防
        (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)); // 破防系数
}

void Attr::UpdateHastePercent()
{
    m_hastePercentVisible =
        m_haste / (JX3_HASTE_RATE * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
    m_hastePercent =
        static_cast<double>(JX3_PCT_INT_BASE) /
        (static_cast<int>(m_hastePercentVisible * JX3_PCT_INT_BASE) + m_hastePercentInt + JX3_PCT_INT_BASE);
}

void Attr::UpdateStrainPercent()
{
    m_strainPercent =
        m_strain / (JX3_INSIGHT_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) + m_strainPercentFromCustom;
}

void Attr::UpdateSurplusDamage()
{
    m_surplus = (GetSurplusBaseFromMajor() + GetSurplusBaseFromCustom()) *
                    (JX3_PCT_INT_BASE + GetSurplusBasePercentInt()) / JX3_PCT_INT_BASE +
                GetSurplusFromClass();
    m_surplusDamage = m_surplus * JX3_SURPLUS_PARAM;
}

void Attr::UpdateShieldIgnorePercent(Targets *targets, PctInt_t PercentInt)
{
    for (auto &target : *targets) {

        target.second->SetShieldIgnorePercentInt(PercentInt);
    }
}

} // namespace JX3DPS
