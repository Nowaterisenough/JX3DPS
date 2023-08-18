/**
 * Project: JX3DPS
 * File: Attribute.h
 * Description:
 * Created Date: 2023-07-18 15:51:36
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-19 06:10:47
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_ATTRIBUTE_H__
#define __JX3DPS_ATTRIBUTE_H__

#include <array>
#include <string>
#include <unordered_map>
#include <variant>

#include "Global/JX3.h"
#include "Global/Types.h"

#define PHYSICS_1_MAGIC_0(x) ((x)&1)
#define PHYSICS_0_MAGIC_1(x) (((x)&1) ^ 1)

namespace JX3DPS {

class Attribute
{

public:

    enum class Type
    {
        DEFAULT = 0,

        AGILITY_BASE,
        AGILITY_BASE_ADDITIONAL_PERCENT_INT,

        STRENGTH_BASE,
        STRENGTH_BASE_ADDITIONAL_PERCENT_INT,

        SPIRIT_BASE,
        SPIRIT_BASE_ADDITIONAL_PERCENT_INT,

        SPUNK_BASE,
        SPUNK_BASE_ADDITIONAL_PERCENT_INT,

        WEAPON_DAMAGE_BASE,
        WEAPON_DAMAGE_RAND,
        WEAPON_DAMAGE_BASE_ADDITIONAL_PERCENT_INT,

        ATTACK_POWER_BASE,
        PHYSICS_ATTACK_POWER_BASE,
        MAGIC_ATTACK_POWER_BASE,

        ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT,
        PHYSICS_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT,
        MAGIC_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT,

        CRITICAL_STRIKE,
        PHYSICS_CRITICAL_STRIKE,
        MAGIC_CRITICAL_STRIKE,

        CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT,
        PHYSICS_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT,
        MAGIC_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT,

        CRITICAL_STRIKE_POWER,
        PHYSICS_CRITICAL_STRIKE_POWER,
        MAGIC_CRITICAL_STRIKE_POWER,

        CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,
        PHYSICS_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,
        MAGIC_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,

        OVERCOME_BASE,
        PHYSICS_OVERCOME_BASE,
        MAGIC_OVERCOME_BASE,

        OVERCOME_BASE_ADDITIONAL_PERCENT_INT,
        PHYSICS_OVERCOME_BASE_ADDITIONAL_PERCENT_INT,
        MAGIC_OVERCOME_BASE_ADDITIONAL_PERCENT_INT,

        STRAIN_BASE,
        STRAIN_BASE_ADDITIONAL_PERCENT_INT,

        SURPLUS_VALUE_BASE,

        HASTE_BASE,
        HASTE_BASE_ADDITIONAL_PERCENT_INT,

        SHIELD_IGNORE_PERCENT_INT,

        PVE_DAMAGE_ADDITIONAL_PERCENT_INT,

        COUNT
    };

    constexpr static std::array<const char *const, static_cast<size_t>(Attribute::Type::COUNT)> ATTRIBUTE_NAME{
        {{ "默认" },
         { "身法" },
         { "身法加成" },
         { "力道" },
         { "力道加成" },
         { "根骨" },
         { "根骨加成" },
         { "元气" },
         { "元气加成" },
         { "基础武器伤害" },
         { "浮动武器伤害" },
         { "基础武器伤害加成" },
         { "基础攻击" },
         { "外功基础攻击" },
         { "内功基础攻击" },
         { "基础攻击加成" },
         { "外功基础攻击加成" },
         { "内功基础攻击加成" },
         { "会心等级" },
         { "外功会心等级" },
         { "内功会心等级" },
         { "会心加成" },
         { "外功会心加成" },
         { "内功会心加成" },
         { "会效等级" },
         { "外功会效等级" },
         { "内功会效等级" },
         { "会效加成" },
         { "外功会效加成" },
         { "内功会效加成" },
         { "基础破防等级" },
         { "外功基础破防等级" },
         { "内功基础破防等级" },
         { "基础破防等级加成" },
         { "外功基础破防等级加成" },
         { "内功基础破防等级加成" },
         { "无双" },
         { "无双加成" },
         { "破招值" },
         { "加速等级" },
         { "加速加成" },
         { "忽视防御加成" },
         { "非侠士伤害加成" }}
    };

    inline static Type AttributeType(const std::string &name)
    {
        for (int type = static_cast<int>(Type::DEFAULT); type < static_cast<int>(Type::COUNT); ++type)
        {
            if (name == ATTRIBUTE_NAME[type]) {
                return static_cast<Type>(type);
            }
        }
        return Type::COUNT;
    }

    enum class TypeByClass
    {
        AGILITY,
        STRENGTH,
        SPIRIT,
        SPUNK,
        PHYSICS_ATTACK_POWER_BASE,
        PHYSICS_ATTACK_POWER,
        MAGIC_ATTACK_POWER_BASE,
        MAGIC_ATTACK_POWER,
        PHYSICS_CRITICAL_STRIKE,
        MAGIC_CRITICAL_STRIKE,
        PHYSICS_OVERCOME_BASE,
        PHYSICS_OVERCOME,
        MAGIC_OVERCOME_BASE,
        MAGIC_OVERCOME,
        PVE_DAMAGE_ADDITIONAL_PERCENT_INT,
        COUNT
    };

    enum class MajorType
    {
        AGILITY,
        STRENGTH,
        SPIRIT,
        SPUNK,
        COUNT
    };

    constexpr static std::array<std::array<Value_t, static_cast<size_t>(TypeByClass::COUNT)>, static_cast<size_t>(ClassType::COUNT)> ATTRIBUTE_INITIAL{
        {{ 41, 41, 41, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
         { 41, 41, 41, 41, 3277, 0, 0, 0, 2929, 0, 0, 0, 0, 0, 184 }, // 太虚剑意
          { 41, 41, 41, 41, 0, 0, 3725, 0, 0, 1788, 0, 0, 0, 0, 205 }, // 紫霞功
          { 41, 41, 41, 41, 3449, 0, 0, 0, 0, 0, 1526, 0, 0, 0, 92 }, // 分山劲
          { 41, 41, 41, 41, 0, 0, 3725, 0, 0, 1279, 0, 0, 0, 0, 143 }}  // 莫问
    };

    constexpr static std::array<std::array<PctInt_t, static_cast<size_t>(TypeByClass::COUNT)>, static_cast<size_t>(ClassType::COUNT)> MAJOR_TO_OTHER_COEFFICIENT{
        {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
         { 0, 0, 0, 0, 0, 1485, 0, 0, 594, 0, 0, 0, 0, 0, 0 }, // 太虚剑意
          { 0, 0, 0, 0, 0, 0, 0, 1792, 0, 573, 0, 0, 0, 0, 0 }, // 紫霞功
          { 0, 0, 0, 0, 0, 1751, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // 分山劲
          { 0, 0, 0, 0, 0, 0, 0, 1895, 0, 389, 0, 0, 0, 0, 0 }}  // 莫问
    };

    constexpr static std::array<std::array<int, static_cast<size_t>(MajorType::COUNT)>, static_cast<size_t>(ClassType::COUNT)> MAJOR{
        {{ 0, 0, 0, 0 },
         { 1, 0, 0, 0 }, // 太虚剑意
          { 0, 0, 1, 0 }, // 紫霞功
          { 1, 0, 0, 0 }, // 分山劲
          { 0, 0, 1, 0 }}  // 莫问
    };

    Attribute() { }

    ~Attribute() { }

    inline static const std::unordered_map<Type, Value_t> &ATTRIBUTE_GAIN_BY_BASE = {
        {{ Type::DEFAULT, 0 },
         { Type::AGILITY_BASE, 179 },
         { Type::STRENGTH_BASE, 179 },
         { Type::SPIRIT_BASE, 179 },
         { Type::SPUNK_BASE, 179 },
         { Type::WEAPON_DAMAGE_BASE, 540 },
         { Type::ATTACK_POWER_BASE, 360 },
         { Type::CRITICAL_STRIKE, 799 },
         { Type::CRITICAL_STRIKE_POWER, 799 },
         { Type::OVERCOME_BASE, 799 },
         { Type::STRAIN_BASE, 799 },
         { Type::SURPLUS_VALUE_BASE, 799 },
         { Type::HASTE_BASE, 799 }}
    };

    inline void SetGainSwitch(Type type, bool sc)
    {
        int sign = (sc << 1) - 1; // true为1，false为-1
        this->AddAttribute(type, sign * ATTRIBUTE_GAIN_BY_BASE.at(type));
    }

    inline void AddAttribute(Type type, Value_t value)
    {
        switch (type) {
            case Type::AGILITY_BASE: this->AddAgilityBaseAdditional(value); break;
            case Type::AGILITY_BASE_ADDITIONAL_PERCENT_INT:
                this->AddAgilityBaseAdditionalPercentInt(value);
                break;

            case Type::STRENGTH_BASE: this->AddStrengthBaseAdditional(value); break;
            case Type::STRENGTH_BASE_ADDITIONAL_PERCENT_INT:
                this->AddStrengthBaseAdditionalPercentInt(value);
                break;

            case Type::SPIRIT_BASE: this->AddSpiritBaseAdditional(value); break;
            case Type::SPIRIT_BASE_ADDITIONAL_PERCENT_INT:
                this->AddSpiritBaseAdditionalPercentInt(value);
                break;

            case Type::SPUNK_BASE: this->AddSpunkBaseAdditional(value); break;
            case Type::SPUNK_BASE_ADDITIONAL_PERCENT_INT:
                this->AddSpunkBaseAdditionalPercentInt(value);
                break;

            case Type::WEAPON_DAMAGE_BASE: this->AddWeaponDamageBase(value); break;
            case Type::WEAPON_DAMAGE_RAND: this->AddWeaponDamageRand(value); break;
            case Type::WEAPON_DAMAGE_BASE_ADDITIONAL_PERCENT_INT:
                this->AddWeaponDamageBaseAdditionalPercentInt(value);
                break;

            case Type::ATTACK_POWER_BASE:
                this->AddPhysicsAttackPowerBaseAdditional(value);
                this->AddMagicAttackPowerBaseAdditional(value);
                break;
            case Type::PHYSICS_ATTACK_POWER_BASE:
                this->AddPhysicsAttackPowerBaseAdditional(value);
                break;
            case Type::MAGIC_ATTACK_POWER_BASE:
                this->AddMagicAttackPowerBaseAdditional(value);
                break;

            case Type::ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT:
                this->AddPhysicsAttackPowerBaseAdditionalPercentInt(value);
                this->AddMagicAttackPowerBaseAdditionalPercentInt(value);
                break;
            case Type::PHYSICS_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT:
                this->AddPhysicsAttackPowerBaseAdditionalPercentInt(value);
                break;
            case Type::MAGIC_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT:
                this->AddMagicAttackPowerBaseAdditionalPercentInt(value);
                break;

            case Type::CRITICAL_STRIKE:
                this->AddPhysicsCriticalStrikeAdditional(value);
                this->AddMagicCriticalStrikeAdditional(value);
                break;
            case Type::PHYSICS_CRITICAL_STRIKE:
                this->AddPhysicsCriticalStrikeAdditional(value);
                break;
            case Type::MAGIC_CRITICAL_STRIKE:
                this->AddMagicCriticalStrikeAdditional(value);
                break;

            case Type::CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT:
                this->AddPhysicsCriticalStrikeAdditionalBasisPointInt(value);
                this->AddMagicCriticalStrikeAdditionalBasisPointInt(value);
                break;
            case Type::PHYSICS_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT:
                this->AddPhysicsCriticalStrikeAdditionalBasisPointInt(value);
                break;
            case Type::MAGIC_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT:
                this->AddMagicCriticalStrikeAdditionalBasisPointInt(value);
                break;

            case Type::CRITICAL_STRIKE_POWER:
                this->AddPhysicsCriticalStrikePower(value);
                this->AddMagicCriticalStrikePower(value);
                break;
            case Type::PHYSICS_CRITICAL_STRIKE_POWER:
                this->AddPhysicsCriticalStrikePower(value);
                break;
            case Type::MAGIC_CRITICAL_STRIKE_POWER:
                this->AddMagicCriticalStrikePower(value);
                break;

            case Type::CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT:
                this->AddPhysicsCriticalStrikePowerAdditionalPercentInt(value);
                this->AddMagicCriticalStrikePowerAdditionalPercentInt(value);
                break;
            case Type::PHYSICS_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT:
                this->AddPhysicsCriticalStrikePowerAdditionalPercentInt(value);
                break;
            case Type::MAGIC_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT:
                this->AddMagicCriticalStrikePowerAdditionalPercentInt(value);
                break;

            case Type::OVERCOME_BASE:
                this->AddPhysicsOvercomeBaseAdditional(value);
                this->AddMagicOvercomeBaseAdditional(value);
                break;
            case Type::PHYSICS_OVERCOME_BASE:
                this->AddPhysicsOvercomeBaseAdditional(value);
                break;
            case Type::MAGIC_OVERCOME_BASE:
                this->AddMagicOvercomeBaseAdditional(value);
                break;

            case Type::OVERCOME_BASE_ADDITIONAL_PERCENT_INT:
                this->AddPhysicsOvercomeBaseAdditionalPercentInt(value);
                this->AddMagicOvercomeBaseAdditionalPercentInt(value);
                break;
            case Type::PHYSICS_OVERCOME_BASE_ADDITIONAL_PERCENT_INT:
                this->AddPhysicsOvercomeBaseAdditionalPercentInt(value);
                break;
            case Type::MAGIC_OVERCOME_BASE_ADDITIONAL_PERCENT_INT:
                this->AddMagicOvercomeBaseAdditionalPercentInt(value);
                break;

            case Type::STRAIN_BASE: this->AddStrainBase(value); break;
            case Type::STRAIN_BASE_ADDITIONAL_PERCENT_INT:
                this->AddStrainBaseAdditionalPercentInt(value);
                break;

            case Type::SURPLUS_VALUE_BASE: this->AddSurplusValueBase(value); break;
            case Type::HASTE_BASE: this->AddHasteBase(value); break;
            case Type::HASTE_BASE_ADDITIONAL_PERCENT_INT:
                this->AddHasteBaseAdditionalPercentInt(value);
                break;

            case Type::SHIELD_IGNORE_PERCENT_INT:
                this->AddShieldIgnorePercentInt(value);
                break;

            case Type::PVE_DAMAGE_ADDITIONAL_PERCENT_INT:
                this->AddPVEDamageAdditionalPercentInt(value);
                break;
            default: break;
        }
    }

    template <typename T>
    inline void AddAttribute(const std::string &typeName, T value)
    {
        this->AddAttribute(GetType(typeName), value);
    }

    inline Type GetType(const std::string &type) const
    {
        for (const auto &name : ATTRIBUTE_NAME) {
            if (type == name) {
                return static_cast<Type>(&name - ATTRIBUTE_NAME.data());
            }
        }
        return Type::COUNT;
    }

    inline void SetAttributeInitial(Type type, Value_t value)
    {
        switch (type) {
            case Type::AGILITY_BASE:
                this->SetAgilityBaseAdditional(value - this->GetAgilityBaseByClass());
                break;

            case Type::STRENGTH_BASE:
                this->SetStrengthBaseAdditional(value - this->GetStrengthBaseByClass());
                break;

            case Type::SPIRIT_BASE:
                this->SetSpiritBaseAdditional(value - this->GetSpiritBaseByClass());
                break;

            case Type::SPUNK_BASE:
                this->SetSpunkBaseAdditional(value - this->GetSpunkBaseByClass());
                break;

            case Type::WEAPON_DAMAGE_BASE: this->SetWeaponDamageBase(value); break;
            case Type::WEAPON_DAMAGE_RAND: this->SetWeaponDamageRand(value); break;

            case Type::ATTACK_POWER_BASE:
                this->SetPhysicsAttackPowerBaseAdditional(value - this->GetPhysicsAttackPowerBaseByClass());
                this->SetMagicAttackPowerBaseAdditional(value - this->GetMagicAttackPowerBaseByClass());
                break;
            case Type::PHYSICS_ATTACK_POWER_BASE:
                this->SetPhysicsAttackPowerBaseAdditional(value - this->GetPhysicsAttackPowerBaseByClass());
                break;
            case Type::MAGIC_ATTACK_POWER_BASE:
                this->SetMagicAttackPowerBaseAdditional(value - this->GetMagicAttackPowerBaseByClass());
                break;

            case Type::CRITICAL_STRIKE:
                this->SetPhysicsCriticalStrikeAdditional(value - this->GetPhysicsCriticalStrikeByClass());
                this->SetMagicCriticalStrikeAdditional(value - this->GetMagicCriticalStrikeByClass());
                break;
            case Type::PHYSICS_CRITICAL_STRIKE:
                this->SetPhysicsCriticalStrikeAdditional(value - this->GetPhysicsCriticalStrikeByClass());
                break;
            case Type::MAGIC_CRITICAL_STRIKE:
                this->SetMagicCriticalStrikeAdditional(value - this->GetMagicCriticalStrikeByClass());
                break;

            case Type::CRITICAL_STRIKE_POWER:
                this->SetPhysicsCriticalStrikePower(value);
                this->SetMagicCriticalStrikePower(value);
                break;
            case Type::PHYSICS_CRITICAL_STRIKE_POWER:
                this->SetPhysicsCriticalStrikePower(value);
                break;
            case Type::MAGIC_CRITICAL_STRIKE_POWER:
                this->SetMagicCriticalStrikePower(value);
                break;

            case Type::OVERCOME_BASE:
                this->SetPhysicsOvercomeBaseAdditional(value - this->GetPhysicsOvercomeBaseByClass());
                this->SetMagicOvercomeBaseAdditional(value - this->GetMagicOvercomeBaseByClass());
                break;
            case Type::PHYSICS_OVERCOME_BASE:
                this->SetPhysicsOvercomeBaseAdditional(value - this->GetPhysicsOvercomeBaseByClass());
                break;
            case Type::MAGIC_OVERCOME_BASE:
                this->SetMagicOvercomeBaseAdditional(value - this->GetMagicOvercomeBaseByClass());
                break;

            case Type::STRAIN_BASE: this->SetStrainBase(value); break;

            case Type::SURPLUS_VALUE_BASE: this->SetSurplusValueBase(value); break;

            case Type::HASTE_BASE: this->SetHasteBase(value); break;

            default: break;
        }
    }

    inline void SetClassType(ClassType classType)
    {
        m_classType = classType;

        this->SetAgilityBaseByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::AGILITY)]);
        this->SetStrengthBaseByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::STRENGTH)]);
        this->SetSpiritBaseByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::SPIRIT)]);
        this->SetSpunkBaseByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::SPUNK)]);
    }

    inline ClassType GetClassType() const { return m_classType; }

    /* 身法 */
    inline Value_t GetAgilityBaseByClass() const { return m_agilityBaseByClass; }

    inline void SetAgilityBaseByClass(Value_t value)
    {
        m_agilityBaseByClass = value;
        UpdateAgility();
    }

    inline Value_t GetAgilityBaseAdditional() const { return m_agilityBaseAdditional; }

    inline void SetAgilityBaseAdditional(Value_t value)
    {
        m_agilityBaseAdditional = value;
        UpdateAgility();
    }

    inline void AddAgilityBaseAdditional(Value_t value)
    {
        m_agilityBaseAdditional += value;
        UpdateAgility();
    }

    inline Value_t GetAgilityBase() const { return m_agilityBase; }

    inline PctInt_t GetAgilityBaseAdditionalPercentInt() const
    {
        return m_agilityBaseAdditionalPercentInt;
    }

    inline void SetAgilityBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_agilityBaseAdditionalPercentInt = percentInt;
        UpdateAgility();
    }

    inline void AddAgilityBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_agilityBaseAdditionalPercentInt += percentInt;
        UpdateAgility();
    }

    inline Value_t GetAgility() const { return m_agility; }

    /* 力道 */
    inline Value_t GetStrengthBaseByClass() const { return m_strengthBaseByClass; }

    inline void SetStrengthBaseByClass(Value_t value)
    {
        m_strengthBaseByClass = value;
        UpdateStrength();
    }

    inline Value_t GetStrengthBaseAdditional() const { return m_strengthBaseAdditional; }

    inline void SetStrengthBaseAdditional(Value_t value)
    {
        m_strengthBaseAdditional = value;
        UpdateStrength();
    }

    inline void AddStrengthBaseAdditional(Value_t value)
    {
        m_strengthBaseAdditional += value;
        UpdateStrength();
    }

    inline Value_t GetStrengthBase() const { return m_strengthBase; }

    inline PctInt_t GetStrengthBaseAdditionalPercentInt() const
    {
        return m_strengthBaseAdditionalPercentInt;
    }

    inline void SetStrengthBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_strengthBaseAdditionalPercentInt = percentInt;
        UpdateStrength();
    }

    inline void AddStrengthBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_strengthBaseAdditionalPercentInt += percentInt;
        UpdateStrength();
    }

    inline Value_t GetStrength() const { return m_strength; }

    /* 根骨 */
    inline Value_t GetSpiritBaseByClass() const { return m_spiritBaseByClass; }

    inline void SetSpiritBaseByClass(Value_t value)
    {
        m_spiritBaseByClass = value;
        UpdateSpirit();
    }

    inline Value_t GetSpiritBaseAdditional() const { return m_spiritBaseAdditional; }

    inline void SetSpiritBaseAdditional(Value_t value)
    {
        m_spiritBaseAdditional = value;
        UpdateSpirit();
    }

    inline void AddSpiritBaseAdditional(Value_t value)
    {
        m_spiritBaseAdditional += value;
        UpdateSpirit();
    }

    inline Value_t GetSpiritBase() const { return m_spiritBase; }

    inline PctInt_t GetSpiritBaseAdditionalPercentInt() const
    {
        return m_spiritBaseAdditionalPercentInt;
    }

    inline void SetSpiritBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_spiritBaseAdditionalPercentInt = percentInt;
        UpdateSpirit();
    }

    inline void AddSpiritBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_spiritBaseAdditionalPercentInt += percentInt;
        UpdateSpirit();
    }

    inline Value_t GetSpirit() const { return m_spirit; }

    /* 元气 */
    inline Value_t GetSpunkBaseByClass() { return m_spunkBaseByClass; }

    inline void SetSpunkBaseByClass(Value_t value)
    {
        m_spunkBaseByClass = value;
        UpdateSpunk();
    }

    inline Value_t GetSpunkBaseAdditional() const { return m_spunkBaseAdditional; }

    inline void SetSpunkBaseAdditional(Value_t value)
    {
        m_spunkBaseAdditional = value;
        UpdateSpunk();
    }

    inline void AddSpunkBaseAdditional(Value_t value)
    {
        m_spunkBaseAdditional += value;
        UpdateSpunk();
    }

    inline Value_t GetSpunkBase() const { return m_spunkBase; }

    inline PctInt_t GetSpunkBaseAdditionalPercentInt() const
    {
        return m_spunkBaseAdditionalPercentInt;
    }

    inline void SetSpunkBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_spunkBaseAdditionalPercentInt = percentInt;
        UpdateSpunk();
    }

    inline void AddSpunkBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_spunkBaseAdditionalPercentInt += percentInt;
        UpdateSpunk();
    }

    inline Value_t GetSpunk() const { return m_spunk; }

    /* 武器伤害 */
    inline Value_t GetWeaponDamageBase() const { return m_weaponDamageBase; }

    inline Value_t GetWeaponDamageRand() const { return m_weaponDamageRand; }

    inline Value_t GetWeaponDamageLower() const { return m_weaponDamageBase; }

    inline Value_t GetWeaponDamageUpper() const
    {
        return m_weaponDamageBase + m_weaponDamageRand;
    }

    inline void SetWeaponDamage(Value_t base, Value_t rand)
    {
        m_weaponDamageBase = base;
        m_weaponDamageRand = rand;
        UpdateWeaponDamage();
    }

    inline void SetWeaponDamageBase(Value_t base)
    {
        m_weaponDamageBase = base;
        UpdateWeaponDamage();
    }

    inline void SetWeaponDamageRand(Value_t rand)
    {
        m_weaponDamageRand = rand;
        UpdateWeaponDamage();
    }

    inline void AddWeaponDamageBase(Value_t base)
    {
        m_weaponDamageBase += base;
        UpdateWeaponDamage();
    }

    inline void AddWeaponDamageRand(Value_t rand)
    {
        m_weaponDamageRand += rand;
        UpdateWeaponDamage();
    }

    inline PctInt_t GetWeaponDamageBaseAdditionalPercentInt() const
    {
        return m_weaponDamageBaseAdditionalPercentInt;
    }

    inline void SetWeaponDamageBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_weaponDamageBaseAdditionalPercentInt = percentInt;
        UpdateWeaponDamage();
    }

    inline void AddWeaponDamageBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_weaponDamageBaseAdditionalPercentInt += percentInt;
        UpdateWeaponDamage();
    }

    inline Value_t GetWeaponDamage() const { return m_weaponDamage; }

    /* 外功攻击 */
    inline Value_t GetPhysicsAttackPowerBaseByClass() const
    {
        return m_physicsAttackPowerBaseByClass;
    }

    inline void SetPhysicsAttackPowerBaseByClass(Value_t value)
    {
        m_physicsAttackPowerBaseByClass = value;
        UpdatePhysicsAttackPower();
    }

    inline void AddPhysicsAttackPowerBaseByClass(Value_t value)
    {
        m_physicsAttackPowerBaseByClass += value;
        UpdatePhysicsAttackPower();
    }

    inline Value_t GetPhysicsAttackPowerBaseAdditional() const
    {
        return m_physicsAttackPowerBaseAdditional;
    }

    inline void SetPhysicsAttackPowerBaseAdditional(Value_t value)
    {
        m_physicsAttackPowerBaseAdditional = value;
        UpdatePhysicsAttackPower();
    }

    inline void AddPhysicsAttackPowerBaseAdditional(Value_t value)
    {
        m_physicsAttackPowerBaseAdditional += value;
        UpdatePhysicsAttackPower();
    }

    inline Value_t GetPhysicsAttackPowerBaseMinimum() const
    {
        return m_physicsAttackPowerBaseByClass;
    }

    inline Value_t GetPhysicsAttackPowerBase() const { return m_physicsAttackPowerBase; }

    inline PctInt_t GetPhysicsAttackPowerBaseAdditionalPercentInt() const
    {
        return m_physicsAttackPowerBaseAdditionalPercentInt;
    }

    inline void SetPhysicsAttackPowerBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_physicsAttackPowerBaseAdditionalPercentInt = percentInt;
        UpdatePhysicsAttackPower();
    }

    inline void AddPhysicsAttackPowerBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_physicsAttackPowerBaseAdditionalPercentInt += percentInt;
        UpdatePhysicsAttackPower();
    }

    inline Value_t GetPhysicsAttackPowerByClass() const
    {
        return m_physicsAttackPowerByClass;
    }

    inline void SetPhysicsAttackPowerByClass(Value_t value)
    {
        m_physicsAttackPowerByClass = value;
        UpdatePhysicsAttackPower();
    }

    inline void AddPhysicsAttackPowerByClass(Value_t value)
    {
        m_physicsAttackPowerByClass += value;
        UpdatePhysicsAttackPower();
    }

    inline Value_t GetPhysicsAttackPower() const { return m_physicsAttackPower; }

    /* 内功攻击 */
    inline Value_t GetMagicAttackPowerBaseByClass() const
    {
        return m_magicAttackPowerBaseByClass;
    }

    inline void SetMagicAttackPowerBaseByClass(Value_t value)
    {
        m_magicAttackPowerBaseByClass = value;
        UpdateMagicAttackPower();
    }

    inline void AddMagicAttackPowerBaseByClass(Value_t value)
    {
        m_magicAttackPowerBaseByClass += value;
        UpdateMagicAttackPower();
    }

    inline Value_t GetMagicAttackPowerBaseAdditional() const
    {
        return m_magicAttackPowerBaseAdditional;
    }

    inline void SetMagicAttackPowerBaseAdditional(Value_t value)
    {
        m_magicAttackPowerBaseAdditional = value;
        UpdateMagicAttackPower();
    }

    inline void AddMagicAttackPowerBaseAdditional(Value_t value)
    {
        m_magicAttackPowerBaseAdditional += value;
        UpdateMagicAttackPower();
    }

    inline Value_t GetMagicAttackPowerBaseMinimum() const
    {
        return m_magicAttackPowerBaseByClass;
    }

    inline Value_t GetMagicAttackPowerBase() const { return m_magicAttackPowerBase; }

    inline PctInt_t GetMagicAttackPowerBaseAdditionalPercentInt() const
    {
        return m_magicAttackPowerBaseAdditionalPercentInt;
    }

    inline void SetMagicAttackPowerBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_magicAttackPowerBaseAdditionalPercentInt = percentInt;
        UpdateMagicAttackPower();
    }

    inline void AddMagicAttackPowerBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_magicAttackPowerBaseAdditionalPercentInt += percentInt;
        UpdateMagicAttackPower();
    }

    inline Value_t GetMagicAttackPowerByClass() const
    {
        return m_magicAttackPowerByClass;
    }

    inline void SetMagicAttackPowerByClass(Value_t value)
    {
        m_magicAttackPowerByClass = value;
        UpdateMagicAttackPower();
    }

    inline void AddMagicAttackPowerByClass(Value_t value)
    {
        m_magicAttackPowerByClass += value;
        UpdateMagicAttackPower();
    }

    inline Value_t GetMagicAttackPower() const { return m_magicAttackPower; }

    /* 外功会心 */
    inline Value_t GetPhysicsCriticalStrikeByClass() const
    {
        return m_physicsCriticalStrikeByClass;
    }

    inline void SetPhysicsCriticalStrikeByClass(Value_t value)
    {
        m_physicsCriticalStrikeByClass = value;
        UpdatePhysicsCriticalStrike();
    }

    inline void AddPhysicsCriticalStrikeByClass(Value_t value)
    {
        m_physicsCriticalStrikeByClass += value;
        UpdatePhysicsCriticalStrike();
    }

    inline Value_t GetPhysicsCriticalStrikeAdditional() const
    {
        return m_physicsCriticalStrikeAdditional;
    }

    inline void SetPhysicsCriticalStrikeAdditional(Value_t value)
    {
        m_physicsCriticalStrikeAdditional = value;
        UpdatePhysicsCriticalStrike();
    }

    inline void AddPhysicsCriticalStrikeAdditional(Value_t value)
    {
        m_physicsCriticalStrikeAdditional += value;
        UpdatePhysicsCriticalStrike();
    }

    inline Value_t GetPhysicsCriticalStrikeMinimum() const
    {
        return m_physicsCriticalStrikeByClass;
    }

    inline Value_t GetPhysicsCriticalStrike() const { return m_physicsCriticalStrike; }

    inline BPInt_t GetPhysicsCriticalStrikeAdditionalBasisPointInt() const
    {
        return m_physicsCriticalStrikeAdditionalBasisPointInt;
    }

    inline void SetPhysicsCriticalStrikeAdditionalBasisPointInt(BPInt_t basisPointInt)
    {
        m_physicsCriticalStrikeAdditionalBasisPointInt = basisPointInt;
        UpdatePhysicsCriticalStrike();
    }

    inline void AddPhysicsCriticalStrikeAdditionalBasisPointInt(BPInt_t basisPointInt)
    {
        m_physicsCriticalStrikeAdditionalBasisPointInt += basisPointInt;
        UpdatePhysicsCriticalStrike();
    }

    inline PctFloat_t GetPhysicsCriticalStrikePercent() const
    {
        return m_physicsCriticalStrikePercent;
    }

    /* 内功会心 */
    inline Value_t GetMagicCriticalStrikeByClass() const
    {
        return m_magicCriticalStrikeByClass;
    }

    inline void SetMagicCriticalStrikeByClass(Value_t value)
    {
        m_magicCriticalStrikeByClass = value;
        UpdateMagicCriticalStrike();
    }

    inline void AddMagicCriticalStrikeByClass(Value_t value)
    {
        m_magicCriticalStrikeByClass += value;
        UpdateMagicCriticalStrike();
    }

    inline Value_t GetMagicCriticalStrikeAdditional() const
    {
        return m_magicCriticalStrikeAdditional;
    }

    inline void SetMagicCriticalStrikeAdditional(Value_t value)
    {
        m_magicCriticalStrikeAdditional = value;
        UpdateMagicCriticalStrike();
    }

    inline void AddMagicCriticalStrikeAdditional(Value_t value)
    {
        m_magicCriticalStrikeAdditional += value;
        UpdateMagicCriticalStrike();
    }

    inline Value_t GetMagicCriticalStrikeMinimum() const
    {
        return m_magicCriticalStrikeByClass;
    }

    inline Value_t GetMagicCriticalStrike() const { return m_magicCriticalStrike; }

    inline BPInt_t GetMagicCriticalStrikeAdditionalBasisPointInt() const
    {
        return m_magicCriticalStrikeAdditionalBasisPointInt;
    }

    inline void SetMagicCriticalStrikeAdditionalBasisPointInt(BPInt_t basisPointInt)
    {
        m_magicCriticalStrikeAdditionalBasisPointInt = basisPointInt;
        UpdateMagicCriticalStrike();
    }

    inline void AddMagicCriticalStrikeAdditionalBasisPointInt(BPInt_t basisPointInt)
    {
        m_magicCriticalStrikeAdditionalBasisPointInt += basisPointInt;
        UpdateMagicCriticalStrike();
    }

    inline PctFloat_t GetMagicCriticalStrikePercent() const
    {
        return m_magicCriticalStrikePercent;
    }

    /* 外功会效 */
    inline Value_t GetPhysicsCriticalStrikePower() const
    {
        return m_physicsCriticalStrikePowerAdditional;
    }

    inline void SetPhysicsCriticalStrikePower(Value_t value)
    {
        m_physicsCriticalStrikePowerAdditional = value;
        UpdatePhysicsCriticalStrikePower();
    }

    inline void AddPhysicsCriticalStrikePower(Value_t value)
    {
        m_physicsCriticalStrikePowerAdditional += value;
        UpdatePhysicsCriticalStrikePower();
    }

    inline Value_t GetPhysicsCriticalStrikePowerMinimum() const
    {
        return m_physicsCriticalStrikePowerPercentBySelf;
    }

    inline PctInt_t GetPhysicsCriticalStrikePowerAdditionalPercentInt() const
    {
        return m_physicsCriticalStrikePowerAdditionalPercentInt;
    }

    inline void SetPhysicsCriticalStrikePowerAdditionalPercentInt(PctInt_t percentInt)
    {
        m_physicsCriticalStrikePowerAdditionalPercentInt = percentInt;
        UpdatePhysicsCriticalStrikePower();
    }

    inline void AddPhysicsCriticalStrikePowerAdditionalPercentInt(PctInt_t percentInt)
    {
        m_physicsCriticalStrikePowerAdditionalPercentInt += percentInt;
        UpdatePhysicsCriticalStrikePower();
    }

    inline PctFloat_t GetPhysicsCriticalStrikePowerPercent() const
    {
        return m_physicsCriticalStrikePowerPercent;
    }

    /* 内功会效 */
    inline Value_t GetMagicCriticalStrikePower() const
    {
        return m_magicCriticalStrikePowerAdditional;
    }

    inline void SetMagicCriticalStrikePower(Value_t value)
    {
        m_magicCriticalStrikePowerAdditional = value;
        UpdateMagicCriticalStrikePower();
    }

    inline void AddMagicCriticalStrikePower(Value_t value)
    {
        m_magicCriticalStrikePowerAdditional += value;
        UpdateMagicCriticalStrikePower();
    }

    inline Value_t GetMagicCriticalStrikePowerMinimum() const
    {
        return m_magicCriticalStrikePowerPercentBySelf;
    }

    inline PctInt_t GetMagicCriticalStrikePowerAdditionalPercentInt() const
    {
        return m_magicCriticalStrikePowerAdditionalPercentInt;
    }

    inline void SetMagicCriticalStrikePowerAdditionalPercentInt(PctInt_t percentInt)
    {
        m_magicCriticalStrikePowerAdditionalPercentInt = percentInt;
        UpdateMagicCriticalStrikePower();
    }

    inline void AddMagicCriticalStrikePowerAdditionalPercentInt(PctInt_t percentInt)
    {
        m_magicCriticalStrikePowerAdditionalPercentInt += percentInt;
        UpdateMagicCriticalStrikePower();
    }

    inline PctFloat_t GetMagicCriticalStrikePowerPercent() const
    {
        return m_magicCriticalStrikePowerPercent;
    }

    /* 外功破防 */
    inline Value_t GetPhysicsOvercomeBaseByClass() const
    {
        return m_physicsOvercomeBaseByClass;
    }

    inline void SetPhysicsOvercomeBaseByClass(Value_t value)
    {
        m_physicsOvercomeBaseByClass = value;
        UpdatePhysicsOvercome();
    }

    inline void AddPhysicsOvercomeBaseByClass(Value_t value)
    {
        m_physicsOvercomeBaseByClass += value;
        UpdatePhysicsOvercome();
    }

    inline Value_t GetPhysicsOvercomeBaseAdditional() const
    {
        return m_physicsOvercomeBaseAdditional;
    }

    inline void SetPhysicsOvercomeBaseAdditional(Value_t value)
    {
        m_physicsOvercomeBaseAdditional = value;
        UpdatePhysicsOvercome();
    }

    inline void AddPhysicsOvercomeBaseAdditional(Value_t value)
    {
        m_physicsOvercomeBaseAdditional += value;
        UpdatePhysicsOvercome();
    }

    inline Value_t GetPhysicsOvercomeBaseMinimum() const
    {
        return m_physicsOvercomeBaseByClass;
    }

    inline Value_t GetPhysicsOvercomeBase() const { return m_physicsOvercomeBase; }

    inline PctInt_t GetPhysicsOvercomeBaseAdditionalPercentInt() const
    {
        return m_physicsOvercomeBaseAdditionalPercentInt;
    }

    inline void SetPhysicsOvercomeBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_physicsOvercomeBaseAdditionalPercentInt = percentInt;
        UpdatePhysicsOvercome();
    }

    inline void AddPhysicsOvercomeBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_physicsOvercomeBaseAdditionalPercentInt += percentInt;
        UpdatePhysicsOvercome();
    }

    inline Value_t GetPhysicsOvercomeByClass() const { return m_physicsOvercomeByClass; }

    inline void SetPhysicsOvercomeByClass(Value_t value)
    {
        m_physicsOvercomeByClass = value;
        UpdatePhysicsOvercome();
    }

    inline void AddPhysicsOvercomeByClass(Value_t value)
    {
        m_physicsOvercomeByClass += value;
        UpdatePhysicsOvercome();
    }

    inline Value_t GetPhysicsOvercome() const { return m_physicsOvercome; }

    inline PctFloat_t GetPhysicsOvercomePercent() const
    {
        return m_physicsOvercomePercent;
    }

    /* 内功破防 */
    inline Value_t GetMagicOvercomeBaseByClass() const
    {
        return m_magicOvercomeBaseByClass;
    }

    inline void SetMagicOvercomeBaseByClass(Value_t value)
    {
        m_magicOvercomeBaseByClass = value;
        UpdateMagicOvercome();
    }

    inline void AddMagicOvercomeBaseByClass(Value_t value)
    {
        m_magicOvercomeBaseByClass += value;
        UpdateMagicOvercome();
    }

    inline Value_t GetMagicOvercomeBaseAdditional() const
    {
        return m_magicOvercomeBaseAdditional;
    }

    inline void SetMagicOvercomeBaseAdditional(Value_t value)
    {
        m_magicOvercomeBaseAdditional = value;
        UpdateMagicOvercome();
    }

    inline void AddMagicOvercomeBaseAdditional(Value_t value)
    {
        m_magicOvercomeBaseAdditional += value;
        UpdateMagicOvercome();
    }

    inline Value_t GetMagicOvercomeBaseMinimum() const
    {
        return m_magicOvercomeBaseByClass;
    }

    inline Value_t GetMagicOvercomeBase() const { return m_magicOvercomeBase; }

    inline PctInt_t GetMagicOvercomeBaseAdditionalPercentInt() const
    {
        return m_magicOvercomeBaseAdditionalPercentInt;
    }

    inline void SetMagicOvercomeBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_magicOvercomeBaseAdditionalPercentInt = percentInt;
        UpdateMagicOvercome();
    }

    inline void AddMagicOvercomeBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_magicOvercomeBaseAdditionalPercentInt += percentInt;
        UpdateMagicOvercome();
    }

    inline Value_t GetMagicOvercomeByClass() const { return m_magicOvercomeByClass; }

    inline void SetMagicOvercomeByClass(Value_t value)
    {
        m_magicOvercomeByClass = value;
        UpdateMagicOvercome();
    }

    inline void AddMagicOvercomeByClass(Value_t value)
    {
        m_magicOvercomeByClass += value;
        UpdateMagicOvercome();
    }

    inline Value_t GetMagicOvercome() const { return m_magicOvercome; }

    inline PctFloat_t GetMagicOvercomePercent() const { return m_magicOvercomePercent; }

    /* 无双 */
    inline Value_t GetStrainBase() const { return m_strainBaseAdditional; }

    inline void SetStrainBase(Value_t value)
    {
        m_strainBaseAdditional = value;
        UpdateStrain();
    }

    inline void AddStrainBase(Value_t value)
    {
        m_strainBaseAdditional += value;
        UpdateStrain();
    }

    inline PctInt_t GetStrainBaseAdditionalPercentInt() const
    {
        return m_strainBaseAdditionalPercentInt;
    }

    inline void SetStrainBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_strainBaseAdditionalPercentInt = percentInt;
        UpdateStrain();
    }

    inline void AddStrainBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_strainBaseAdditionalPercentInt += percentInt;
        UpdateStrain();
    }

    inline PctFloat_t GetStrainPercent() const { return m_strainPercent; }

    /* 破招 */
    inline Value_t GetSurplusValueBase() const { return m_surplusValueBaseAdditional; }

    inline void SetSurplusValueBase(Value_t value)
    {
        m_surplusValueBaseAdditional = value;
        UpdateSurplusValue();
    }

    inline void AddSurplusValueBase(Value_t value)
    {
        m_surplusValueBaseAdditional += value;
        UpdateSurplusValue();
    }

    /* 加速 */
    inline Value_t GetHasteBase() const { return m_hasteBaseAdditional; }

    inline void SetHasteBase(Value_t value)
    {
        m_hasteBaseAdditional = value;
        UpdateHaste();
    }

    inline void AddHasteBase(Value_t value)
    {
        m_hasteBaseAdditional += value;
        UpdateHaste();
    }

    inline PctInt_t GetHasteBaseAdditionalPercentInt() const
    {
        return m_hasteBaseAdditionalPercentInt;
    }

    inline void SetHasteBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_hasteBaseAdditionalPercentInt = percentInt;
        UpdateHaste();
    }

    inline void AddHasteBaseAdditionalPercentInt(PctInt_t percentInt)
    {
        m_hasteBaseAdditionalPercentInt += percentInt;
        UpdateHaste();
    }

    inline PctFloat_t GetHastePercent() const { return m_hastePercent; }

    inline PctFloat_t GetHasteVisiblePercent() const { return m_hasteVisiblePercent; }

    /* 防御忽视 */
    inline PctInt_t GetShieldIgnorePercentInt() const { return m_shieldIgnorePercentInt; }

    inline void SetShieldIgnorePercentInt(PctInt_t percentInt)
    {
        m_shieldIgnorePercentInt = percentInt;
    }

    inline void AddShieldIgnorePercentInt(PctInt_t percentInt)
    {
        m_shieldIgnorePercentInt += percentInt;
    }

    /* 非侠士伤害加成 */
    inline PctInt_t GetPVEDamageAdditionalPercentInt() const
    {
        return m_pveDamageAdditionalPercentIntByClass;
    }

    inline void SetPVEDamageAdditionalPercentInt(PctInt_t percentInt)
    {
        m_pveDamageAdditionalPercentIntByClass = percentInt;
    }

    inline void AddPVEDamageAdditionalPercentInt(PctInt_t percentInt)
    {
        m_pveDamageAdditionalPercentIntByClass += percentInt;
    }

private:
    /* 身法 */
    inline void UpdateAgility()
    {
        m_agilityBase = m_agilityBaseByClass + m_agilityBaseAdditional;
        m_agility = m_agilityBase * (JX3_PERCENT_INT_BASE + m_agilityBaseAdditionalPercentInt) / JX3_PERCENT_INT_BASE;
        this->UpdateByClass();
    }

    /* 力道 */
    inline void UpdateStrength()
    {
        m_strengthBase = m_strengthBaseByClass + m_strengthBaseAdditional;
        m_strength = m_strengthBase * (JX3_PERCENT_INT_BASE + m_strengthBaseAdditionalPercentInt) / JX3_PERCENT_INT_BASE;
        this->UpdateByClass();
    }

    /* 根骨 */
    inline void UpdateSpirit()
    {
        m_spiritBase = m_spiritBaseByClass + m_spiritBaseAdditional;
        m_spirit = m_spiritBase * (JX3_PERCENT_INT_BASE + m_spiritBaseAdditionalPercentInt) / JX3_PERCENT_INT_BASE;
        this->UpdateByClass();
    }

    /* 元气 */
    inline void UpdateSpunk()
    {
        m_spunkBase = m_spunkBaseByClass + m_spunkBaseAdditional;
        m_spunk = m_spunkBase * (JX3_PERCENT_INT_BASE + m_spunkBaseAdditionalPercentInt) / JX3_PERCENT_INT_BASE;
        this->UpdateByClass();
    }

    /* 武器伤害 */
    inline void UpdateWeaponDamage()
    {
        m_weaponDamage =
            (m_weaponDamageBase * (JX3_PERCENT_INT_BASE + m_weaponDamageBaseAdditionalPercentInt) *
                 2 / JX3_PERCENT_INT_BASE +
             m_weaponDamageRand) /
            2;
    }

    /* 外功攻击 */
    inline void UpdatePhysicsAttackPower()
    {
        m_physicsAttackPowerBase = m_physicsAttackPowerBaseByClass + m_physicsAttackPowerBaseAdditional;
        m_physicsAttackPower =
            m_physicsAttackPowerBase *
                (JX3_PERCENT_INT_BASE + m_physicsAttackPowerBaseAdditionalPercentInt) / JX3_PERCENT_INT_BASE +
            m_physicsAttackPowerByClass;
    }

    /* 内功攻击 */
    inline void UpdateMagicAttackPower()
    {
        m_magicAttackPowerBase = m_magicAttackPowerBaseByClass + m_magicAttackPowerBaseAdditional;
        m_magicAttackPower =
            m_magicAttackPowerBase *
                (JX3_PERCENT_INT_BASE + m_magicAttackPowerBaseAdditionalPercentInt) / JX3_PERCENT_INT_BASE +
            m_magicAttackPowerByClass;
    }

    /* 外功会心 */
    inline void UpdatePhysicsCriticalStrike()
    {
        m_physicsCriticalStrike = m_physicsCriticalStrikeByClass + m_physicsCriticalStrikeAdditional;
        m_physicsCriticalStrikePercent =
            m_physicsCriticalStrike /
                (JX3_CRITICAL_STRIKE_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) + // 会心系数
            m_physicsCriticalStrikeAdditionalBasisPointInt * JX3_PERCENT_FLOAT_BASE /
                JX3_BASIS_POINT_INT_BASE; // 会心比例加成
    }

    /* 内功会心 */
    inline void UpdateMagicCriticalStrike()
    {
        m_magicCriticalStrike = m_magicCriticalStrikeByClass + m_magicCriticalStrikeAdditional;
        m_magicCriticalStrikePercent =
            m_magicCriticalStrike /
                (JX3_CRITICAL_STRIKE_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST)) + // 会心系数
            m_magicCriticalStrikeAdditionalBasisPointInt * JX3_PERCENT_FLOAT_BASE /
                JX3_BASIS_POINT_INT_BASE; // 会心比例加成
    }

    /* 外功会心效果 */
    inline void UpdatePhysicsCriticalStrikePower()
    {
        m_physicsCriticalStrikePower =
            m_physicsCriticalStrikePowerAdditional *
            (JX3_PERCENT_INT_BASE + m_physicsCriticalStrikePowerAdditionalPercentInt) / JX3_PERCENT_INT_BASE;
        m_physicsCriticalStrikePowerPercent =
            m_physicsCriticalStrikePowerPercentBySelf +
            m_physicsCriticalStrikePower /
                (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
    }

    /* 内功会心效果 */
    inline void UpdateMagicCriticalStrikePower()
    {
        m_magicCriticalStrikePower =
            m_magicCriticalStrikePowerAdditional *
            (JX3_PERCENT_INT_BASE + m_magicCriticalStrikePowerAdditionalPercentInt) / JX3_PERCENT_INT_BASE;
        m_magicCriticalStrikePowerPercent =
            m_magicCriticalStrikePowerPercentBySelf +
            m_magicCriticalStrikePower /
                (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
    }

    /* 外功破防 */
    inline void UpdatePhysicsOvercome()
    {
        m_physicsOvercomeBase = m_physicsOvercomeBaseByClass + m_physicsOvercomeBaseAdditional;
        m_physicsOvercome =
            m_physicsOvercomeBase *
                (JX3_PERCENT_INT_BASE + m_physicsOvercomeBaseAdditionalPercentInt) / JX3_PERCENT_INT_BASE +
            m_physicsOvercomeByClass;
        m_physicsOvercomePercent =
            m_physicsOvercome /
            (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
    }

    /* 内功破防 */
    inline void UpdateMagicOvercome()
    {
        m_magicOvercomeBase = m_magicOvercomeBaseByClass + m_magicOvercomeBaseAdditional;
        m_magicOvercome =
            m_magicOvercomeBase * (JX3_PERCENT_INT_BASE + m_magicOvercomeBaseAdditionalPercentInt) /
                JX3_PERCENT_INT_BASE +
            m_magicOvercomeByClass;
        m_magicOvercomePercent =
            m_magicOvercome /
            (JX3_OVERCOME_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
    }

    /* 无双 */
    inline void UpdateStrain()
    {
        m_strainBase = m_strainBaseAdditional *
                       (JX3_PERCENT_INT_BASE + m_strainBaseAdditionalPercentInt) / JX3_PERCENT_INT_BASE;
        m_strainPercent =
            m_strainBase / (JX3_INSIGHT_PARAM * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
    }

    /* 破招 */
    inline void UpdateSurplusValue()
    {
        m_surplusValueBase = m_surplusValueBaseAdditional;
    }

    /* 加速 */
    inline void UpdateHaste()
    {
        m_hasteBase = m_hasteBaseAdditional;
        m_hasteVisiblePercent =
            m_hasteBase / (JX3_HASTE_RATE * (JX3_LEVEL_PARAM * JX3_PLAYER_LEVEL - JX3_LEVEL_CONST));
        m_hastePercent = JX3_PERCENT_FLOAT_BASE * JX3_PERCENT_INT_BASE /
                         (static_cast<int>(m_hasteVisiblePercent * JX3_PERCENT_INT_BASE) +
                          m_hasteBaseAdditionalPercentInt + JX3_PERCENT_INT_BASE);
    }

    inline void UpdateByClass()
    {

        Value_t major =
            m_agility * MAJOR[static_cast<int>(m_classType)][static_cast<int>(MajorType::AGILITY)] +
            m_strength * MAJOR[static_cast<int>(m_classType)][static_cast<int>(MajorType::STRENGTH)] +
            m_spirit * MAJOR[static_cast<int>(m_classType)][static_cast<int>(MajorType::SPIRIT)] +
            m_spunk * MAJOR[static_cast<int>(m_classType)][static_cast<int>(MajorType::SPUNK)];

        this->SetPhysicsAttackPowerBaseByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_ATTACK_POWER_BASE)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_ATTACK_POWER_BASE)] *
                major  / JX3_PERCENT_INT_BASE +
            m_strength * JX3_STRENGTH_TO_ATTACK_POWER_BASE / JX3_PERCENT_INT_BASE);

        this->SetMagicAttackPowerBaseByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_ATTACK_POWER_BASE)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_ATTACK_POWER_BASE)] *
                major / JX3_PERCENT_INT_BASE +
            m_spunk * JX3_SPUNK_TO_ATTACK_POWER_BASE / JX3_PERCENT_INT_BASE);

        this->SetPhysicsAttackPowerByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_ATTACK_POWER)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_ATTACK_POWER)] *
                major / JX3_PERCENT_INT_BASE);

        this->SetMagicAttackPowerByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_ATTACK_POWER)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_ATTACK_POWER)] *
                major / JX3_PERCENT_INT_BASE);

        this->SetPhysicsCriticalStrikeByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_CRITICAL_STRIKE)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_CRITICAL_STRIKE)] *
                major / JX3_PERCENT_INT_BASE +
            m_agility * JX3_AGILITY_TO_CRITICAL_STRIKE / JX3_PERCENT_INT_BASE);

        this->SetMagicCriticalStrikeByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_CRITICAL_STRIKE)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_CRITICAL_STRIKE)] *
                major / JX3_PERCENT_INT_BASE +
            m_spirit * JX3_SPIRIT_TO_CRITICAL_STRIKE / JX3_PERCENT_INT_BASE);

        this->SetPhysicsOvercomeBaseByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_OVERCOME_BASE)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_OVERCOME_BASE)] *
                major / JX3_PERCENT_INT_BASE +
            m_strength * JX3_STRENGTH_TO_OVERCOME_BASE / JX3_PERCENT_INT_BASE);

        this->SetMagicOvercomeBaseByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_OVERCOME_BASE)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_OVERCOME_BASE)] *
                major / JX3_PERCENT_INT_BASE +
            m_spunk * JX3_SPUNK_TO_OVERCOME_BASE / JX3_PERCENT_INT_BASE);

        this->SetPhysicsOvercomeByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_OVERCOME)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PHYSICS_OVERCOME)] *
                major / JX3_PERCENT_INT_BASE);

        this->SetMagicOvercomeByClass(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_OVERCOME)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::MAGIC_OVERCOME)] *
                major / JX3_PERCENT_INT_BASE);

        this->SetPVEDamageAdditionalPercentInt(
            ATTRIBUTE_INITIAL[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PVE_DAMAGE_ADDITIONAL_PERCENT_INT)] +
            MAJOR_TO_OTHER_COEFFICIENT[static_cast<int>(m_classType)][static_cast<int>(TypeByClass::PVE_DAMAGE_ADDITIONAL_PERCENT_INT)] *
                major / JX3_PERCENT_INT_BASE);
    }

    ClassType m_classType = ClassType::DEFAULT;

    Value_t  m_agilityBaseByClass              = 0;
    Value_t  m_agilityBaseAdditional           = 0;
    Value_t  m_agilityBase                     = 0;
    PctInt_t m_agilityBaseAdditionalPercentInt = 0;
    Value_t  m_agility                         = 0;

    Value_t  m_strengthBaseByClass              = 0;
    Value_t  m_strengthBaseAdditional           = 0;
    Value_t  m_strengthBase                     = 0;
    PctInt_t m_strengthBaseAdditionalPercentInt = 0;
    Value_t  m_strength                         = 0;

    Value_t  m_spiritBaseByClass              = 0;
    Value_t  m_spiritBaseAdditional           = 0;
    Value_t  m_spiritBase                     = 0;
    PctInt_t m_spiritBaseAdditionalPercentInt = 0;
    Value_t  m_spirit                         = 0;

    Value_t  m_spunkBaseByClass              = 0;
    Value_t  m_spunkBaseAdditional           = 0;
    Value_t  m_spunkBase                     = 0;
    PctInt_t m_spunkBaseAdditionalPercentInt = 0;
    Value_t  m_spunk                         = 0;

    Value_t  m_weaponDamageBase                     = 0;
    Value_t  m_weaponDamageRand                     = 0;
    PctInt_t m_weaponDamageBaseAdditionalPercentInt = 0;
    Value_t  m_weaponDamage                         = 0;

    Value_t  m_physicsAttackPowerBaseByClass              = 0;
    Value_t  m_physicsAttackPowerBaseAdditional           = 0;
    Value_t  m_physicsAttackPowerBase                     = 0;
    PctInt_t m_physicsAttackPowerBaseAdditionalPercentInt = 0;
    Value_t  m_physicsAttackPowerByClass                  = 0;
    Value_t  m_physicsAttackPower                         = 0;

    Value_t  m_magicAttackPowerBaseByClass              = 0;
    Value_t  m_magicAttackPowerBaseAdditional           = 0;
    Value_t  m_magicAttackPowerBase                     = 0;
    PctInt_t m_magicAttackPowerBaseAdditionalPercentInt = 0;
    Value_t  m_magicAttackPowerByClass                  = 0;
    Value_t  m_magicAttackPower                         = 0;

    Value_t    m_physicsCriticalStrikeByClass                 = 0;
    Value_t    m_physicsCriticalStrikeAdditional              = 0;
    Value_t    m_physicsCriticalStrike                        = 0;
    BPInt_t    m_physicsCriticalStrikeAdditionalBasisPointInt = 0;
    PctFloat_t m_physicsCriticalStrikePercent                 = 0.0;

    Value_t    m_magicCriticalStrikeByClass                 = 0;
    Value_t    m_magicCriticalStrikeAdditional              = 0;
    Value_t    m_magicCriticalStrike                        = 0;
    BPInt_t    m_magicCriticalStrikeAdditionalBasisPointInt = 0;
    PctFloat_t m_magicCriticalStrikePercent                 = 0.0;

    Value_t  m_physicsCriticalStrikePowerAdditional           = 0;
    Value_t  m_physicsCriticalStrikePower                     = 0;
    PctInt_t m_physicsCriticalStrikePowerAdditionalPercentInt = 0;
    PctFloat_t m_physicsCriticalStrikePowerPercentBySelf = JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;
    PctFloat_t m_physicsCriticalStrikePowerPercent = JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    Value_t  m_magicCriticalStrikePowerAdditional           = 0;
    Value_t  m_magicCriticalStrikePower                     = 0;
    PctInt_t m_magicCriticalStrikePowerAdditionalPercentInt = 0;
    PctFloat_t m_magicCriticalStrikePowerPercentBySelf = JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;
    PctFloat_t m_magicCriticalStrikePowerPercent = JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    Value_t    m_physicsOvercomeBaseByClass              = 0;
    Value_t    m_physicsOvercomeBaseAdditional           = 0;
    Value_t    m_physicsOvercomeBase                     = 0;
    PctInt_t   m_physicsOvercomeBaseAdditionalPercentInt = 0;
    Value_t    m_physicsOvercomeByClass                  = 0;
    Value_t    m_physicsOvercome                         = 0;
    PctFloat_t m_physicsOvercomePercent                  = 0.0;

    Value_t    m_magicOvercomeBaseByClass              = 0;
    Value_t    m_magicOvercomeBaseAdditional           = 0;
    Value_t    m_magicOvercomeBase                     = 0;
    PctInt_t   m_magicOvercomeBaseAdditionalPercentInt = 0;
    Value_t    m_magicOvercomeByClass                  = 0;
    Value_t    m_magicOvercome                         = 0;
    PctFloat_t m_magicOvercomePercent                  = 0.0;

    Value_t    m_strainBaseAdditional           = 0;
    Value_t    m_strainBase                     = 0;
    PctInt_t   m_strainBaseAdditionalPercentInt = 0;
    PctFloat_t m_strainPercent                  = 0.0;

    Value_t m_surplusValueBaseAdditional = 0;
    Value_t m_surplusValueBase           = 0;

    Value_t    m_hasteBaseAdditional           = 0;
    Value_t    m_hasteBase                     = 0;
    PctInt_t   m_hasteBaseAdditionalPercentInt = 0;
    PctFloat_t m_hastePercent                  = 0.0;
    PctFloat_t m_hasteVisiblePercent           = 0.0;

    PctInt_t m_shieldIgnorePercentInt = 0;

    PctInt_t m_pveDamageAdditionalPercentIntByClass = 0;
};

} // namespace JX3DPS

#endif // __JX3DPS_ATTRIBUTE_H__
