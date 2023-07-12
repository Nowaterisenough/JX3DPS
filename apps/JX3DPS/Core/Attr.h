/**
 * Project: JX3DPS
 * File: Attr.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-12 06:56:59
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef JX3DPS_ATTR_H_
#define JX3DPS_ATTR_H_

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_JX3DPS // CMake add_definitions
#        ifdef __GNUC__
#            define JX3DPS_API __attribute__((dllexport))
#        else
#            define JX3DPS_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define JX3DPS_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_JX3DPS_LIB
#                define JX3DPS_API __declspec(dllimport)
#            else
#                define JX3DPS_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_JX3DPS
#    define JX3DPS_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define JX3DPS_API     __attribute__((visibility("default")))
#        define JX3DPS_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define JX3DPS_API
#        define JX3DPS_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define JX3DPS_API     __attribute__((visibility("default")))
#    define JX3DPS_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <memory>

#include "Defs.h"
#include "Globals.h"
#include "JX3Params.h"
#include "Target.h"

namespace JX3DPS {

class JX3DPS_API Attr
{
public:
    Attr(Class classType = Class::TAI_XU_JIAN_YI);
    ~Attr();

    Attr(const Attr &other);
    Attr &operator=(const Attr &other);

    /* 身法 */
    Value_t  GetAgilityBase() const;
    void     SetAgilityBase(Value_t value);
    void     AddAgilityBase(Value_t value);
    PctInt_t GetAgilityBasePercentInt() const;
    void     SetAgilityBasePercentInt(PctInt_t PercentInt);
    void     AddAgilityBasePercentInt(PctInt_t PercentInt);
    Value_t  GetAgility() const;

    /* 力道 */
    Value_t  GetStrengthBase() const;
    void     SetStrengthBase(Value_t value);
    void     AddStrengthBase(Value_t value);
    PctInt_t GetStrengthBasePercentInt() const;
    void     SetStrengthBasePercentInt(PctInt_t PercentInt);
    void     AddStrengthBasePercentInt(PctInt_t PercentInt);
    Value_t  GetStrength() const;

    /* 根骨 */
    Value_t  GetSpiritBase() const;
    void     SetSpiritBase(Value_t value);
    void     AddSpiritBase(Value_t value);
    PctInt_t GetSpiritBasePercentInt() const;
    void     SetSpiritBasePercentInt(PctInt_t PercentInt);
    void     AddSpiritBasePercentInt(PctInt_t PercentInt);
    Value_t  GetSpirit() const;

    /* 元气 */
    Value_t  GetSpunkBase() const;
    void     SetSpunkBase(Value_t value);
    void     AddSpunkBase(Value_t value);
    PctInt_t GetSpunkBasePercentInt() const;
    void     SetSpunkBasePercentInt(PctInt_t PercentInt);
    void     AddSpunkBasePercentInt(PctInt_t PercentInt);
    Value_t  GetSpunk() const;

    /* 武器伤害 */
    Value_t GetWeaponAttack() const;
    void    SetWeaponAttack(Value_t value);
    void    SetWeaponAttack(Value_t lower, Value_t upper);
    void    AddWeaponAttack(Value_t value);
    void    AddWeaponAttack(Value_t lower, Value_t upper);
    Value_t GetWeaponAttackLower() const;
    Value_t GetWeaponAttackUpper() const;

    /* 外功攻击 */
    Value_t  GetPhysicsAttackBaseFromCustom() const;
    void     SetPhysicsAttackBaseFromCustom(Value_t value);
    void     AddPhysicsAttackBaseFromCustom(Value_t value);
    Value_t  GetPhysicsAttackBaseFromMajor() const;
    Value_t  GetPhysicsAttackBase() const;
    void     SetPhysicsAttackBase(Value_t value);
    void     AddPhysicsAttackBase(Value_t value);
    PctInt_t GetPhysicsAttackBasePercentInt() const;
    void     SetPhysicsAttackBasePercentInt(PctInt_t PercentInt);
    void     AddPhysicsAttackBasePercentInt(PctInt_t PercentInt);
    Value_t  GetPhysicsAttackFromMain() const;
    Value_t  GetPhysicsAttackFromBase() const;
    Value_t  GetPhysicsAttack() const;
    Value_t  GetPhysicsAttackBaseMinimum() const;
    Value_t  GetPhysicsAttackMinimum() const;

    /* 内功攻击 */
    Value_t  GetMagicAttackBaseFromCustom() const;
    void     SetMagicAttackBaseFromCustom(Value_t value);
    void     AddMagicAttackBaseFromCustom(Value_t value);
    Value_t  GetMagicAttackBaseFromMajor() const;
    Value_t  GetMagicAttackBase() const;
    void     SetMagicAttackBase(Value_t value);
    void     AddMagicAttackBase(Value_t value);
    PctInt_t GetMagicAttackBasePercentInt() const;
    void     SetMagicAttackBasePercentInt(PctInt_t PercentInt);
    void     AddMagicAttackBasePercentInt(PctInt_t PercentInt);
    Value_t  GetMagicAttackFromMain() const;
    Value_t  GetMagicAttackFromBase() const;
    Value_t  GetMagicAttack() const;
    Value_t  GetMagicAttackBaseMinimum() const;
    Value_t  GetMagicAttackMinimum() const;

    /* 外功会心 */
    Value_t    GetPhysicsCriticalStrikeFromCustom() const;
    void       SetPhysicsCriticalStrikeFromCustom(Value_t value);
    void       AddPhysicsCriticalStrikeFromCustom(Value_t value);
    Value_t    GetPhysicsCriticalStrikeFromMajor() const;
    Value_t    GetPhysicsCriticalStrikeFromClass() const;
    Value_t    GetPhysicsCriticalStrike() const;
    void       SetPhysicsCriticalStrike(Value_t value);
    void       AddPhysicsCriticalStrike(Value_t value);
    PctInt_t   GetPhysicsCriticalStrikePercentIntFromCustom() const;
    void       SetPhysicsCriticalStrikePercentIntFromCustom(PctInt_t percentInt);
    void       AddPhysicsCriticalStrikePercentIntFromCustom(PctInt_t percentInt);
    PctFloat_t GetPhysicsCriticalStrikePercent() const;
    Value_t    GetPhysicsCriticalStrikeMinimum() const;

    void SetBoxPhysicsCriticalStrikePercent(PctFloat_t percent);

    /* 内功会心 */
    Value_t    GetMagicCriticalStrikeFromCustom() const;
    void       SetMagicCriticalStrikeFromCustom(Value_t value);
    void       AddMagicCriticalStrikeFromCustom(Value_t value);
    Value_t    GetMagicCriticalStrikeFromMajor() const;
    Value_t    GetMagicCriticalStrikeFromClass() const;
    Value_t    GetMagicCriticalStrike() const;
    void       SetMagicCriticalStrike(Value_t value);
    void       AddMagicCriticalStrike(Value_t value);
    PctInt_t   GetMagicCriticalStrikePercentIntFromCustom() const;
    void       SetMagicCriticalStrikePercentIntFromCustom(PctInt_t percentInt);
    void       AddMagicCriticalStrikePercentIntFromCustom(PctInt_t percentInt);
    PctFloat_t GetMagicCriticalStrikePercent() const;
    Value_t    GetMagicCriticalStrikeMinimum() const;

    void SetBoxMagicCriticalStrikePercent(PctFloat_t percent);

    /* 外功会心效果 */
    Value_t    GetPhysicsCriticalStrikePower() const;
    void       SetPhysicsCriticalStrikePower(Value_t value);
    void       AddPhysicsCriticalStrikePower(Value_t value);
    PctInt_t   GetPhysicsCriticalStrikePowerPercentIntFromCustom() const;
    void       SetPhysicsCriticalStrikePowerPercentIntFromCustom(PctInt_t percentInt);
    void       AddPhysicsCriticalStrikePowerPercentIntFromCustom(PctInt_t percentInt);
    PctFloat_t GetPhysicsCriticalStrikePowerPercent() const;

    void SetBoxPhysicsCriticalStrikePowerPercent(PctFloat_t percent);

    /* 内功会心效果 */
    Value_t    GetMagicCriticalStrikePower() const;
    void       SetMagicCriticalStrikePower(Value_t value);
    void       AddMagicCriticalStrikePower(Value_t value);
    PctInt_t   GetMagicCriticalStrikePowerPercentIntFromCustom() const;
    void       SetMagicCriticalStrikePowerPercentIntFromCustom(PctInt_t percentInt);
    void       AddMagicCriticalStrikePowerPercentIntFromCustom(PctInt_t percentInt);
    PctFloat_t GetMagicCriticalStrikePowerPercent() const;

    void SetBoxMagicCriticalStrikePowerPercent(PctFloat_t percent);

    /* 外功破防 */
    Value_t    GetPhysicsOvercomeBaseFromCustom() const;
    void       SetPhysicsOvercomeBaseFromCustom(Value_t value);
    void       AddPhysicsOvercomeBaseFromCustom(Value_t value);
    Value_t    GetPhysicsOvercomeBaseFromMajor() const;
    Value_t    GetPhysicsOvercomeBase() const;
    void       SetPhysicsOvercomeBase(Value_t value);
    void       AddPhysicsOvercomeBase(Value_t value);
    PctInt_t   GetPhysicsOvercomeBasePercentInt() const;
    void       SetPhysicsOvercomeBasePercentInt(PctInt_t PercentInt);
    void       AddPhysicsOvercomeBasePercentInt(PctInt_t PercentInt);
    Value_t    GetPhysicsOvercomeFromClass() const;
    Value_t    GetPhysicsOvercome() const;
    PctFloat_t GetPhysicsOvercomePercent() const;
    Value_t    GetPhysicsOvercomeBaseMinimum() const;
    Value_t    GetPhysicsOvercomeMinimum() const;

    /* 内功破防 */
    Value_t    GetMagicOvercomeBaseFromCustom() const;
    void       SetMagicOvercomeBaseFromCustom(Value_t value);
    void       AddMagicOvercomeBaseFromCustom(Value_t value);
    Value_t    GetMagicOvercomeBaseFromMajor() const;
    Value_t    GetMagicOvercomeBase() const;
    void       SetMagicOvercomeBase(Value_t value);
    void       AddMagicOvercomeBase(Value_t value);
    PctInt_t   GetMagicOvercomeBasePercentInt() const;
    void       SetMagicOvercomeBasePercentInt(PctInt_t PercentInt);
    void       AddMagicOvercomeBasePercentInt(PctInt_t PercentInt);
    Value_t    GetMagicOvercomeFromClass() const;
    Value_t    GetMagicOvercome() const;
    PctFloat_t GetMagicOvercomePercent() const;
    Value_t    GetMagicOvercomeBaseMinimum() const;
    Value_t    GetMagicOvercomeMinimum() const;

    /* 加速 */
    Value_t    GetHaste() const;
    void       SetHaste(Value_t value);
    void       AddHaste(Value_t value);
    PctInt_t   GetHastePercentInt() const;
    void       SetHastePercentInt(PctInt_t PercentInt);
    void       AddHastePercentInt(PctInt_t PercentInt);
    PctFloat_t GetHastePercent() const;
    PctFloat_t GetHastePercentVisible() const;

    /* 无双 */
    Value_t    GetStrain() const;
    void       SetStrain(Value_t value);
    void       AddStrain(Value_t value);
    PctFloat_t GetStrainPercent() const;
    void       AddStrainPercent(PctFloat_t percent);

    /* 破招 */
    Value_t  GetSurplusBaseFromCustom() const;
    void     SetSurplusBaseFromCustom(Value_t value);
    void     AddSurplusBaseFromCustom(Value_t value);
    Value_t  GetSurplusBaseFromMajor() const;
    Value_t  GetSurplusBase() const;
    void     SetSurplusBase(Value_t value);
    void     AddSurplusBase(Value_t value);
    PctInt_t GetSurplusBasePercentInt() const;
    void     SetSurplusBasePercentInt(PctInt_t PercentInt);
    void     AddSurplusBasePercentInt(PctInt_t PercentInt);
    Value_t  GetSurplusFromClass() const;
    Value_t  GetSurplus() const;
    Value_t  GetSurplusBaseMinimum() const;
    Value_t  GetSurplusMinimum() const;
    Value_t  GetSurplusDamage() const;

    /* 忽视防御 */
    PctInt_t GetShieldIgnorePercentInt() const;
    void     SetShieldIgnorePercentInt(Targets *targets, PctInt_t PercentInt);
    void     AddShieldIgnorePercentInt(Targets *targets, PctInt_t PercentInt);

private:
    /* 缺省函数 */
    Value_t GetNoneFromClass() const;

    /* 太虚剑意主属性转换 */
    Value_t GetPhysicsAttackFromClassTaiXuJianYi() const;
    Value_t GetPhysicsCriticalStrikeFromClassTaiXuJianYi() const;

    /* 身法 */
    void UpdateAgility();

    /* 力道 */
    void UpdateStrength();

    /* 根骨 */
    void UpdateSpirit();

    /* 元气 */
    void UpdateSpunk();

    /* 外功攻击 */
    void UpdatePhysicsAttack();

    /* 内功攻击 */
    void UpdateMagicAttack();

    /* 外功会心 */
    void UpdatePhysicsCriticalStrikePercent();

    /* 内功会心 */
    void UpdateMagicCriticalStrikePercent();

    /* 外功会心效果 */
    void UpdatePhysicsCriticalStrikePowerPercent();

    /* 内功会心效果 */
    void UpdateMagicCriticalStrikePowerPercent();

    /* 外功破防 */
    void UpdatePhysicsOvercomePercent();

    /* 内功破防 */
    void UpdateMagicOvercomePercent();

    /* 加速 */
    void UpdateHastePercent();

    /* 无双 */
    void UpdateStrainPercent();

    /* 破招 */
    void UpdateSurplusDamage();

    /* 忽视防御 */
    void UpdateShieldIgnorePercent(Targets *targets, PctInt_t PercentInt);

private:
    /* 心法属性加成 */
    Value_t (Attr::*m_physicsAttackFromClass)() const         = &Attr::GetNoneFromClass;
    Value_t (Attr::*m_physicsCriticalStrikeFromClass)() const = &Attr::GetNoneFromClass;
    Value_t (Attr::*m_physicsOvercomeFromClass)() const       = &Attr::GetNoneFromClass;

    Value_t (Attr::*m_magicAttackFromClass)() const         = &Attr::GetNoneFromClass;
    Value_t (Attr::*m_magicCriticalStrikeFromClass)() const = &Attr::GetNoneFromClass;
    Value_t (Attr::*m_magicOvercomeFromClass)() const       = &Attr::GetNoneFromClass;

    Value_t (Attr::*m_surplusFromClass)() const = &Attr::GetNoneFromClass;

    /* 身法 */
    Value_t  m_agilityBase           = 0;
    PctInt_t m_agilityBasePercentInt = 0;
    Value_t  m_agility               = 0;

    /* 力道 */
    Value_t  m_strengthBase           = 0;
    PctInt_t m_strengthBasePercentInt = 0;
    Value_t  m_strength               = 0;

    /* 根骨 */
    Value_t  m_spiritBase           = 0;
    PctInt_t m_spiritBasePercentInt = 0;
    Value_t  m_spirit               = 0;

    /* 元气 */
    Value_t  m_spunkBase           = 0;
    PctInt_t m_spunkBasePercentInt = 0;
    Value_t  m_spunk               = 0;

    /* 武器伤害 */
    Value_t m_weaponAttackLower = 0;
    Value_t m_weaponAttackUpper = 0;
    Value_t m_weaponAttack      = 0;

    /* 外功攻击 */
    Value_t  m_physicsAttackBaseFromCustom = 0;
    PctInt_t m_physicsAttackBasePercentInt = 0;
    Value_t  m_physicsAttackFromMain       = 0;
    Value_t  m_physicsAttackFromBase       = 0;
    Value_t  m_physicsAttack               = 0;

    /* 内功攻击 */
    Value_t  m_magicAttackBaseFromCustom = 0;
    PctInt_t m_magicAttackBasePercentInt = 0;
    Value_t  m_magicAttackFromMain       = 0;
    Value_t  m_magicAttackFromBase       = 0;
    Value_t  m_magicAttack               = 0;

    /* 外功会心 */
    Value_t    m_physicsCriticalStrike                  = 0;
    Value_t    m_physicsCriticalStrikeFromCustom        = 0;
    PctInt_t m_physicsCriticalStrikePercentIntFromCustom = 0.0;
    PctFloat_t m_physicsCriticalStrikePercent           = 0.0;

    /* 内功会心 */
    Value_t    m_magicCriticalStrike                  = 0;
    Value_t    m_magicCriticalStrikeFromCustom        = 0;
    PctInt_t m_magicCriticalStrikePercentIntFromCustom = 0.0;
    PctFloat_t m_magicCriticalStrikePercent           = 0.0;

    /* 外功会心效果 */
    Value_t    m_physicsCriticalStrikePower                  = 0;
    PctInt_t m_physicsCriticalStrikePowerPercentIntFromCustom = 0.0;
    PctFloat_t m_physicsCriticalStrikePowerPercent = JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    /* 内功会心效果 */
    Value_t    m_magicCriticalStrikePower                  = 0;
    PctInt_t m_magicCriticalStrikePowerPercentIntFromCustom = 0.0;
    PctFloat_t m_magicCriticalStrikePowerPercent = JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    /* 外功破防 */
    Value_t    m_physicsOvercomeBaseFromCustom = 0;
    PctInt_t   m_physicsOvercomeBasePercentInt = 0;
    Value_t    m_physicsOvercome               = 0;
    PctFloat_t m_physicsOvercomePercent        = 0.0;

    /* 内功破防 */
    Value_t    m_magicOvercomeBaseFromCustom = 0;
    PctInt_t   m_magicOvercomeBasePercentInt = 0;
    Value_t    m_magicOvercome               = 0;
    PctFloat_t m_magicOvercomePercent        = 0.0;

    /* 加速 */
    Value_t    m_haste               = 0;
    PctInt_t   m_hastePercentInt     = 0;
    PctFloat_t m_hastePercent        = 0.0;
    PctFloat_t m_hastePercentVisible = 0.0;

    /* 无双 */
    Value_t    m_strain                  = 0;
    PctFloat_t m_strainPercentFromCustom = 0.0;
    PctFloat_t m_strainPercent           = 0.0;

    /* 破招 */
    Value_t  m_surplusBaseFromCustom = 0;
    PctInt_t m_surplusBasePercentInt = 0;
    Value_t  m_surplus               = 0;
    Value_t  m_surplusDamage         = 0;

    /* 忽视防御 */
    PctInt_t m_shieldIgnorePercentInt = 0;
};

} // namespace JX3DPS

#endif // JX3DPS_ATTR_H
