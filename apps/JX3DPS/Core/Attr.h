/**
 * Project: JX3DPS
 * File: Attr.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-23 14:46:56
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
    void     SetAgilityBase(const Value_t value);
    void     AddAgilityBase(const Value_t value);
    PctInt_t GetAgilityBaseBinPercent() const;
    void     SetAgilityBaseBinPercent(const PctInt_t binPercent);
    void     AddAgilityBaseBinPercent(const PctInt_t binPercent);
    Value_t  GetAgility() const;

    /* 力道 */
    Value_t  GetStrengthBase() const;
    void     SetStrengthBase(const Value_t value);
    void     AddStrengthBase(const Value_t value);
    PctInt_t GetStrengthBaseBinPercent() const;
    void     SetStrengthBaseBinPercent(const PctInt_t binPercent);
    void     AddStrengthBaseBinPercent(const PctInt_t binPercent);
    Value_t  GetStrength() const;

    /* 根骨 */
    Value_t  GetSpiritBase() const;
    void     SetSpiritBase(const Value_t value);
    void     AddSpiritBase(const Value_t value);
    PctInt_t GetSpiritBaseBinPercent() const;
    void     SetSpiritBaseBinPercent(const PctInt_t binPercent);
    void     AddSpiritBaseBinPercent(const PctInt_t binPercent);
    Value_t  GetSpirit() const;

    /* 元气 */
    Value_t  GetSpunkBase() const;
    void     SetSpunkBase(const Value_t value);
    void     AddSpunkBase(const Value_t value);
    PctInt_t GetSpunkBaseBinPercent() const;
    void     SetSpunkBaseBinPercent(const PctInt_t binPercent);
    void     AddSpunkBaseBinPercent(const PctInt_t binPercent);
    Value_t  GetSpunk() const;

    /* 武器伤害 */
    Value_t GetWeaponAttack() const;
    void    SetWeaponAttack(const Value_t lower, const Value_t upper);
    void    AddWeaponAttack(const Value_t value);
    void    AddWeaponAttack(const Value_t lower, const Value_t upper);

    /* 外功攻击 */
    Value_t  GetPhysicsAttackBaseFromCustom() const;
    void     SetPhysicsAttackBaseFromCustom(const Value_t value);
    void     AddPhysicsAttackBaseFromCustom(const Value_t value);
    Value_t  GetPhysicsAttackBaseFromMajor() const;
    Value_t  GetPhysicsAttackBase() const;
    void     SetPhysicsAttackBase(const Value_t value);
    void     AddPhysicsAttackBase(const Value_t value);
    PctInt_t GetPhysicsAttackBaseBinPercent() const;
    void     SetPhysicsAttackBaseBinPercent(const PctInt_t binPercent);
    void     AddPhysicsAttackBaseBinPercent(const PctInt_t binPercent);
    Value_t  GetPhysicsAttackFromMain() const;
    Value_t  GetPhysicsAttackFromBase() const;
    Value_t  GetPhysicsAttack() const;
    Value_t  GetPhysicsAttackBaseMinimum() const;
    Value_t  GetPhysicsAttackMinimum() const;

    /* 内功攻击 */
    Value_t  GetMagicAttackBaseFromCustom() const;
    void     SetMagicAttackBaseFromCustom(const Value_t value);
    void     AddMagicAttackBaseFromCustom(const Value_t value);
    Value_t  GetMagicAttackBaseFromMajor() const;
    Value_t  GetMagicAttackBase() const;
    void     SetMagicAttackBase(const Value_t value);
    void     AddMagicAttackBase(const Value_t value);
    PctInt_t GetMagicAttackBaseBinPercent() const;
    void     SetMagicAttackBaseBinPercent(const PctInt_t binPercent);
    void     AddMagicAttackBaseBinPercent(const PctInt_t binPercent);
    Value_t  GetMagicAttackFromMain() const;
    Value_t  GetMagicAttackFromBase() const;
    Value_t  GetMagicAttack() const;
    Value_t  GetMagicAttackBaseMinimum() const;
    Value_t  GetMagicAttackMinimum() const;

    /* 外功会心 */
    Value_t    GetPhysicsCriticalStrikeFromCustom() const;
    void       SetPhysicsCriticalStrikeFromCustom(const Value_t value);
    void       AddPhysicsCriticalStrikeFromCustom(const Value_t value);
    Value_t    GetPhysicsCriticalStrikeFromMajor() const;
    Value_t    GetPhysicsCriticalStrikeFromClass() const;
    Value_t    GetPhysicsCriticalStrike() const;
    void       SetPhysicsCriticalStrike(const Value_t value);
    void       AddPhysicsCriticalStrike(const Value_t value);
    PctFloat_t GetPhysicsCriticalStrikePercentFromCustom() const;
    void       SetPhysicsCriticalStrikePercentFromCustom(const PctFloat_t percent);
    void       AddPhysicsCriticalStrikePercentFromCustom(const PctFloat_t percent);
    PctFloat_t GetPhysicsCriticalStrikePercent() const;
    Value_t    GetPhysicsCriticalStrikeMinimum() const;

    /* 内功会心 */
    Value_t    GetMagicCriticalStrikeFromCustom() const;
    void       SetMagicCriticalStrikeFromCustom(const Value_t value);
    void       AddMagicCriticalStrikeFromCustom(const Value_t value);
    Value_t    GetMagicCriticalStrikeFromMajor() const;
    Value_t    GetMagicCriticalStrikeFromClass() const;
    Value_t    GetMagicCriticalStrike() const;
    void       SetMagicCriticalStrike(const Value_t value);
    void       AddMagicCriticalStrike(const Value_t value);
    PctFloat_t GetMagicCriticalStrikePercentFromCustom() const;
    void       SetMagicCriticalStrikePercentFromCustom(const PctFloat_t percent);
    void       AddMagicCriticalStrikePercentFromCustom(const PctFloat_t percent);
    PctFloat_t GetMagicCriticalStrikePercent() const;
    Value_t    GetMagicCriticalStrikeMinimum() const;

    /* 外功会心效果 */
    Value_t    GetPhysicsCriticalStrikePower() const;
    void       SetPhysicsCriticalStrikePower(const Value_t value);
    void       AddPhysicsCriticalStrikePower(const Value_t value);
    PctFloat_t GetPhysicsCriticalStrikePowerPercentFromCustom() const;
    void       SetPhysicsCriticalStrikePowerPercentFromCustom(const PctFloat_t percent);
    void       AddPhysicsCriticalStrikePowerPercentFromCustom(const PctFloat_t percent);
    PctFloat_t GetPhysicsCriticalStrikePowerPercent() const;

    /* 内功会心效果 */
    Value_t    GetMagicCriticalStrikePower() const;
    void       SetMagicCriticalStrikePower(const Value_t value);
    void       AddMagicCriticalStrikePower(const Value_t value);
    PctFloat_t GetMagicCriticalStrikePowerPercentFromCustom() const;
    void       SetMagicCriticalStrikePowerPercentFromCustom(const PctFloat_t percent);
    void       AddMagicCriticalStrikePowerPercentFromCustom(const PctFloat_t percent);
    PctFloat_t GetMagicCriticalStrikePowerPercent() const;

    /* 外功破防 */
    Value_t    GetPhysicsOvercomeBaseFromCustom() const;
    void       SetPhysicsOvercomeBaseFromCustom(const Value_t value);
    void       AddPhysicsOvercomeBaseFromCustom(const Value_t value);
    Value_t    GetPhysicsOvercomeBaseFromMajor() const;
    Value_t    GetPhysicsOvercomeBase() const;
    void       SetPhysicsOvercomeBase(const Value_t value);
    void       AddPhysicsOvercomeBase(const Value_t value);
    PctInt_t   GetPhysicsOvercomeBaseBinPercent() const;
    void       SetPhysicsOvercomeBaseBinPercent(const PctInt_t binPercent);
    void       AddPhysicsOvercomeBaseBinPercent(const PctInt_t binPercent);
    Value_t    GetPhysicsOvercomeFromClass() const;
    Value_t    GetPhysicsOvercome() const;
    PctFloat_t GetPhysicsOvercomePercent() const;
    Value_t    GetPhysicsOvercomeBaseMinimum() const;
    Value_t    GetPhysicsOvercomeMinimum() const;

    /* 内功破防 */
    Value_t    GetMagicOvercomeBaseFromCustom() const;
    void       SetMagicOvercomeBaseFromCustom(const Value_t value);
    void       AddMagicOvercomeBaseFromCustom(const Value_t value);
    Value_t    GetMagicOvercomeBaseFromMajor() const;
    Value_t    GetMagicOvercomeBase() const;
    void       SetMagicOvercomeBase(const Value_t value);
    void       AddMagicOvercomeBase(const Value_t value);
    PctInt_t   GetMagicOvercomeBaseBinPercent() const;
    void       SetMagicOvercomeBaseBinPercent(const PctInt_t binPercent);
    void       AddMagicOvercomeBaseBinPercent(const PctInt_t binPercent);
    Value_t    GetMagicOvercomeFromClass() const;
    Value_t    GetMagicOvercome() const;
    PctFloat_t GetMagicOvercomePercent() const;
    Value_t    GetMagicOvercomeBaseMinimum() const;
    Value_t    GetMagicOvercomeMinimum() const;

    /* 加速 */
    Value_t    GetHaste() const;
    void       SetHaste(const Value_t value);
    void       AddHaste(const Value_t value);
    PctInt_t   GetHasteBinPercent() const;
    void       SetHasteBinPercent(const PctInt_t binPercent);
    void       AddHasteBinPercent(const PctInt_t binPercent);
    PctFloat_t GetHastePercent() const;
    PctFloat_t GetHastePercentVisible() const;

    /* 无双 */
    Value_t    GetStrain() const;
    void       SetStrain(const Value_t value);
    void       AddStrain(const Value_t value);
    PctFloat_t GetStrainPercent() const;
    void       AddStrainPercent(const PctFloat_t percent);

    /* 破招 */
    Value_t  GetSurplusBaseFromCustom() const;
    void     SetSurplusBaseFromCustom(const Value_t value);
    void     AddSurplusBaseFromCustom(const Value_t value);
    Value_t  GetSurplusBaseFromMajor() const;
    Value_t  GetSurplusBase() const;
    void     SetSurplusBase(const Value_t value);
    void     AddSurplusBase(const Value_t value);
    PctInt_t GetSurplusBaseBinPercent() const;
    void     SetSurplusBaseBinPercent(const PctInt_t binPercent);
    void     AddSurplusBaseBinPercent(const PctInt_t binPercent);
    Value_t  GetSurplusFromClass() const;
    Value_t  GetSurplus() const;
    Value_t  GetSurplusBaseMinimum() const;
    Value_t  GetSurplusMinimum() const;
    Value_t  GetSurplusDamage() const;

    /* 忽视防御 */
    PctInt_t GetShieldIgnoreBinPercent() const;
    void     SetShieldIgnoreBinPercent(Targets *targets, const PctInt_t binPercent);
    void     AddShieldIgnoreBinPercent(Targets *targets, const PctInt_t binPercent);

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
    void UpdateShieldIgnorePercent(Targets *targets, const PctInt_t binPercent);

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
    PctInt_t m_agilityBaseBinPercent = 0;
    Value_t  m_agility               = 0;

    /* 力道 */
    Value_t  m_strengthBase           = 0;
    PctInt_t m_strengthBaseBinPercent = 0;
    Value_t  m_strength               = 0;

    /* 根骨 */
    Value_t  m_spiritBase           = 0;
    PctInt_t m_spiritBaseBinPercent = 0;
    Value_t  m_spirit               = 0;

    /* 元气 */
    Value_t  m_spunkBase           = 0;
    PctInt_t m_spunkBaseBinPercent = 0;
    Value_t  m_spunk               = 0;

    /* 武器伤害 */
    Value_t m_weaponAttack = 0;

    /* 外功攻击 */
    Value_t  m_physicsAttackBaseFromCustom = 0;
    PctInt_t m_physicsAttackBaseBinPercent = 0;
    Value_t  m_physicsAttackFromMain       = 0;
    Value_t  m_physicsAttackFromBase       = 0;
    Value_t  m_physicsAttack               = 0;

    /* 内功攻击 */
    Value_t  m_magicAttackBaseFromCustom = 0;
    PctInt_t m_magicAttackBaseBinPercent = 0;
    Value_t  m_magicAttackFromMain       = 0;
    Value_t  m_magicAttackFromBase       = 0;
    Value_t  m_magicAttack               = 0;

    /* 外功会心 */
    Value_t    m_physicsCriticalStrike                  = 0;
    Value_t    m_physicsCriticalStrikeFromCustom        = 0;
    PctFloat_t m_physicsCriticalStrikePercentFromCustom = 0.0;
    PctFloat_t m_physicsCriticalStrikePercent           = 0.0;

    /* 内功会心 */
    Value_t    m_magicCriticalStrike                  = 0;
    Value_t    m_magicCriticalStrikeFromCustom        = 0;
    PctFloat_t m_magicCriticalStrikePercentFromCustom = 0.0;
    PctFloat_t m_magicCriticalStrikePercent           = 0.0;

    /* 外功会心效果 */
    Value_t    m_physicsCriticalStrikePower                  = 0;
    PctFloat_t m_physicsCriticalStrikePowerPercentFromCustom = 0.0;
    PctFloat_t m_physicsCriticalStrikePowerPercent = JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    /* 内功会心效果 */
    Value_t    m_magicCriticalStrikePower                  = 0;
    PctFloat_t m_magicCriticalStrikePowerPercentFromCustom = 0.0;
    PctFloat_t m_magicCriticalStrikePowerPercent = JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    /* 外功破防 */
    Value_t    m_physicsOvercomeBaseFromCustom = 0;
    PctInt_t   m_physicsOvercomeBaseBinPercent = 0;
    Value_t    m_physicsOvercome               = 0;
    PctFloat_t m_physicsOvercomePercent        = 0.0;

    /* 内功破防 */
    Value_t    m_magicOvercomeBaseFromCustom = 0;
    PctInt_t   m_magicOvercomeBaseBinPercent = 0;
    Value_t    m_magicOvercome               = 0;
    PctFloat_t m_magicOvercomePercent        = 0.0;

    /* 加速 */
    Value_t    m_haste               = 0;
    PctInt_t   m_hasteBinPercent     = 0;
    PctFloat_t m_hastePercent        = 0.0;
    PctFloat_t m_hastePercentVisible = 0.0;

    /* 无双 */
    Value_t    m_strain                  = 0;
    PctFloat_t m_strainPercentFromCustom = 0.0;
    PctFloat_t m_strainPercent           = 0.0;

    /* 破招 */
    Value_t  m_surplusBaseFromCustom = 0;
    PctInt_t m_surplusBaseBinPercent = 0;
    Value_t  m_surplus               = 0;
    Value_t  m_surplusDamage         = 0;

    /* 忽视防御 */
    PctInt_t m_shieldIgnoreBinPercent = 0;
};

} // namespace JX3DPS

#endif // JX3DPS_ATTR_H
