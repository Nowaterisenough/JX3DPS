/**
 * Description: Player属性类, 包括主属性、攻击、会心、破防等基本类别及转换关系.
 * Create: 2020-09-08  难为水
 * Update: 2020-11-29  难为水  1. 更新破招属性;
 * Notes: 1. **FromCustom表自定义部分，包括装备，附魔等加成部分;
 *        2. **FromMajor表主属性转化而来的部分;
 *        3. **FromClass表心法额外加成的部分;
 */

#ifndef ATTR_H
#define ATTR_H

#include "Core/Global.h"

namespace JX3DPS {

class Attr
{
public:
    Attr(Class classType);
    ~Attr();

    /* 身法 */
    Value_t  GetAgilityBase() const;
    void     SetAgilityBase(Value_t value);
    void     AddAgilityBase(Value_t value);
    BinPct_t GetAgilityBaseBinPercent() const;
    void     SetAgilityBaseBinPercent(BinPct_t binPercent);
    void     AddAgilityBaseBinPercent(BinPct_t binPercent);
    Value_t  GetAgility() const;

    /* 力道 */
    Value_t  GetStrengthBase() const;
    void     SetStrengthBase(Value_t value);
    void     AddStrengthBase(Value_t value);
    BinPct_t GetStrengthBaseBinPercent() const;
    void     SetStrengthBaseBinPercent(BinPct_t binPercent);
    void     AddStrengthBaseBinPercent(BinPct_t binPercent);
    Value_t  GetStrength() const;

    /* 根骨 */
    Value_t  GetSpiritBase() const;
    void     SetSpiritBase(Value_t value);
    void     AddSpiritBase(Value_t value);
    BinPct_t GetSpiritBaseBinPercent() const;
    void     SetSpiritBaseBinPercent(BinPct_t binPercent);
    void     AddSpiritBaseBinPercent(BinPct_t binPercent);
    Value_t  GetSpirit() const;

    /* 元气 */
    Value_t  GetSpunkBase() const;
    void     SetSpunkBase(Value_t value);
    void     AddSpunkBase(Value_t value);
    BinPct_t GetSpunkBaseBinPercent() const;
    void     SetSpunkBaseBinPercent(BinPct_t binPercent);
    void     AddSpunkBaseBinPercent(BinPct_t binPercent);
    Value_t  GetSpunk() const;

    /* 武器伤害 */
    Value_t GetWeaponAttack() const;
    void    SetWeaponAttack(Value_t lower, Value_t upper);
    void    AddWeaponAttack(Value_t value);
    void    AddWeaponAttack(Value_t lower, Value_t upper);

    /* 外功攻击 */
    Value_t  GetPhysicsAttackBaseFromCustom() const;
    void     SetPhysicsAttackBaseFromCustom(Value_t value);
    void     AddPhysicsAttackBaseFromCustom(Value_t value);
    Value_t  GetPhysicsAttackBaseFromMajor() const;
    Value_t  GetPhysicsAttackBase() const;
    void     SetPhysicsAttackBase(Value_t value);
    void     AddPhysicsAttackBase(Value_t value);
    BinPct_t GetPhysicsAttackBaseBinPercent() const;
    void     SetPhysicsAttackBaseBinPercent(BinPct_t binPercent);
    void     AddPhysicsAttackBaseBinPercent(BinPct_t binPercent);
    Value_t  GetPhysicsAttackFromClass() const;
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
    BinPct_t GetMagicAttackBaseBinPercent() const;
    void     SetMagicAttackBaseBinPercent(BinPct_t binPercent);
    void     AddMagicAttackBaseBinPercent(BinPct_t binPercent);
    Value_t  GetMagicAttackFromClass() const;
    Value_t  GetMagicAttack() const;
    Value_t  GetMagicAttackBaseMinimum() const;
    Value_t  GetMagicAttackMinimum() const;

    /* 外功会心 */
    Value_t GetPhysicsCriticalStrikeFromCustom() const;
    void    SetPhysicsCriticalStrikeFromCustom(Value_t value);
    void    AddPhysicsCriticalStrikeFromCustom(Value_t value);
    Value_t GetPhysicsCriticalStrikeFromMajor() const;
    Value_t GetPhysicsCriticalStrikeFromClass() const;
    Value_t GetPhysicsCriticalStrike() const;
    void    SetPhysicsCriticalStrike(Value_t value);
    void    AddPhysicsCriticalStrike(Value_t value);
    Pct_t   GetPhysicsCriticalStrikePercentFromCustom() const;
    void    SetPhysicsCriticalStrikePercentFromCustom(const Pct_t percent);
    void    AddPhysicsCriticalStrikePercentFromCustom(const Pct_t percent);
    Pct_t   GetPhysicsCriticalStrikePercent() const;
    Value_t GetPhysicsCriticalStrikeMinimum() const;

    /* 内功会心 */
    Value_t GetMagicCriticalStrikeFromCustom() const;
    void    SetMagicCriticalStrikeFromCustom(Value_t value);
    void    AddMagicCriticalStrikeFromCustom(Value_t value);
    Value_t GetMagicCriticalStrikeFromMajor() const;
    Value_t GetMagicCriticalStrikeFromClass() const;
    Value_t GetMagicCriticalStrike() const;
    void    SetMagicCriticalStrike(Value_t value);
    void    AddMagicCriticalStrike(Value_t value);
    Pct_t   GetMagicCriticalStrikePercentFromCustom() const;
    void    SetMagicCriticalStrikePercentFromCustom(const Pct_t percent);
    void    AddMagicCriticalStrikePercentFromCustom(const Pct_t percent);
    Pct_t   GetMagicCriticalStrikePercent() const;
    Value_t GetMagicCriticalStrikeMinimum() const;

    /* 外功会心效果 */
    Value_t GetPhysicsCriticalStrikePower() const;
    void    SetPhysicsCriticalStrikePower(Value_t value);
    void    AddPhysicsCriticalStrikePower(Value_t value);
    Pct_t   GetPhysicsCriticalStrikePowerPercentFromCustom() const;
    void    SetPhysicsCriticalStrikePowerPercentFromCustom(const Pct_t percent);
    void    AddPhysicsCriticalStrikePowerPercentFromCustom(const Pct_t percent);
    Pct_t   GetPhysicsCriticalStrikePowerPercent() const;

    /* 内功会心效果 */
    Value_t GetMagicCriticalStrikePower() const;
    void    SetMagicCriticalStrikePower(Value_t value);
    void    AddMagicCriticalStrikePower(Value_t value);
    Pct_t   GetMagicCriticalStrikePowerPercentFromCustom() const;
    void    SetMagicCriticalStrikePowerPercentFromCustom(const Pct_t percent);
    void    AddMagicCriticalStrikePowerPercentFromCustom(const Pct_t percent);
    Pct_t   GetMagicCriticalStrikePowerPercent() const;

    /* 外功破防 */
    Value_t  GetPhysicsOvercomeBaseFromCustom() const;
    void     SetPhysicsOvercomeBaseFromCustom(Value_t value);
    void     AddPhysicsOvercomeBaseFromCustom(Value_t value);
    Value_t  GetPhysicsOvercomeBaseFromMajor() const;
    Value_t  GetPhysicsOvercomeBase() const;
    void     SetPhysicsOvercomeBase(Value_t value);
    void     AddPhysicsOvercomeBase(Value_t value);
    BinPct_t GetPhysicsOvercomeBaseBinPercent() const;
    void     SetPhysicsOvercomeBaseBinPercent(BinPct_t binPercent);
    void     AddPhysicsOvercomeBaseBinPercent(BinPct_t binPercent);
    Value_t  GetPhysicsOvercomeFromClass() const;
    Value_t  GetPhysicsOvercome() const;
    Pct_t    GetPhysicsOvercomePercent() const;
    Value_t  GetPhysicsOvercomeBaseMinimum() const;
    Value_t  GetPhysicsOvercomeMinimum() const;

    /* 内功破防 */
    Value_t  GetMagicOvercomeBaseFromCustom() const;
    void     SetMagicOvercomeBaseFromCustom(Value_t value);
    void     AddMagicOvercomeBaseFromCustom(Value_t value);
    Value_t  GetMagicOvercomeBaseFromMajor() const;
    Value_t  GetMagicOvercomeBase() const;
    void     SetMagicOvercomeBase(Value_t value);
    void     AddMagicOvercomeBase(Value_t value);
    BinPct_t GetMagicOvercomeBaseBinPercent() const;
    void     SetMagicOvercomeBaseBinPercent(BinPct_t binPercent);
    void     AddMagicOvercomeBaseBinPercent(BinPct_t binPercent);
    Value_t  GetMagicOvercomeFromClass() const;
    Value_t  GetMagicOvercome() const;
    Pct_t    GetMagicOvercomePercent() const;
    Value_t  GetMagicOvercomeBaseMinimum() const;
    Value_t  GetMagicOvercomeMinimum() const;

    /* 加速 */
    Value_t  GetHaste() const;
    void     SetHaste(Value_t value);
    void     AddHaste(Value_t value);
    BinPct_t GetHasteBinPercent() const;
    void     SetHasteBinPercent(BinPct_t binPercent);
    void     AddHasteBinPercent(BinPct_t binPercent);
    Pct_t    GetHastePercent() const;
    Pct_t    GetHastePercentVisible() const;

    /* 无双 */
    Value_t GetStrain() const;
    void    SetStrain(Value_t value);
    void    AddStrain(Value_t value);
    Pct_t   GetStrainPercent() const;
    void    AddStrainPercent(const Pct_t percent);

    /* 破招 */
    Value_t  GetSurplusBaseFromCustom() const;
    void     SetSurplusBaseFromCustom(Value_t value);
    void     AddSurplusBaseFromCustom(Value_t value);
    Value_t  GetSurplusBaseFromMajor() const;
    Value_t  GetSurplusBase() const;
    void     SetSurplusBase(Value_t value);
    void     AddSurplusBase(Value_t value);
    BinPct_t GetSurplusBaseBinPercent() const;
    void     SetSurplusBaseBinPercent(BinPct_t binPercent);
    void     AddSurplusBaseBinPercent(BinPct_t binPercent);
    Value_t  GetSurplusFromClass() const;
    Value_t  GetSurplus() const;
    Value_t  GetSurplusBaseMinimum() const;
    Value_t  GetSurplusMinimum() const;
    Value_t  GetSurplusDamage() const;

    /* 忽视防御 */
    BinPct_t GetShieldIgnoreBinPercent() const;
    void     SetShieldIgnoreBinPercent(TargetsMap &targetsMap, BinPct_t binPercent);
    void     AddShieldIgnoreBinPercent(TargetsMap &targetsMap, BinPct_t binPercent);

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
    void UpdateShieldIgnorePercent(TargetsMap &targetsMap, BinPct_t binPercent);

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
    BinPct_t m_agilityBaseBinPercent = 0;
    Value_t  m_agility               = 0;

    /* 力道 */
    Value_t  m_strengthBase           = 0;
    BinPct_t m_strengthBaseBinPercent = 0;
    Value_t  m_strength               = 0;

    /* 根骨 */
    Value_t  m_spiritBase           = 0;
    BinPct_t m_spiritBaseBinPercent = 0;
    Value_t  m_spirit               = 0;

    /* 元气 */
    Value_t  m_spunkBase           = 0;
    BinPct_t m_spunkBaseBinPercent = 0;
    Value_t  m_spunk               = 0;

    /* 武器伤害 */
    Value_t m_weaponAttack = 0;

    /* 外功攻击 */
    Value_t  m_physicsAttackBaseFromCustom = 0;
    BinPct_t m_physicsAttackBaseBinPercent = 0;
    Value_t  m_physicsAttack               = 0;

    /* 内功攻击 */
    Value_t  m_magicAttackBaseFromCustom = 0;
    BinPct_t m_magicAttackBaseBinPercent = 0;
    Value_t  m_magicAttack               = 0;

    /* 外功会心 */
    Value_t m_physicsCriticalStrike                  = 0;
    Value_t m_physicsCriticalStrikeFromCustom        = 0;
    Pct_t   m_physicsCriticalStrikePercentFromCustom = 0.0;
    Pct_t   m_physicsCriticalStrikePercent           = 0.0;

    /* 内功会心 */
    Value_t m_magicCriticalStrike                  = 0;
    Value_t m_magicCriticalStrikeFromCustom        = 0;
    Pct_t   m_magicCriticalStrikePercentFromCustom = 0.0;
    Pct_t   m_magicCriticalStrikePercent           = 0.0;

    /* 外功会心效果 */
    Value_t m_physicsCriticalStrikePower                  = 0;
    Pct_t   m_physicsCriticalStrikePowerPercentFromCustom = 0.0;
    Pct_t   m_physicsCriticalStrikePowerPercent = CONST_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    /* 内功会心效果 */
    Value_t m_magicCriticalStrikePower                  = 0;
    Pct_t   m_magicCriticalStrikePowerPercentFromCustom = 0.0;
    Pct_t   m_magicCriticalStrikePowerPercent = CONST_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    /* 外功破防 */
    Value_t  m_physicsOvercomeBaseFromCustom = 0;
    BinPct_t m_physicsOvercomeBaseBinPercent = 0;
    Value_t  m_physicsOvercome               = 0;
    Pct_t    m_physicsOvercomePercent        = 0.0;

    /* 内功破防 */
    Value_t  m_magicOvercomeBaseFromCustom = 0;
    BinPct_t m_magicOvercomeBaseBinPercent = 0;
    Value_t  m_magicOvercome               = 0;
    Pct_t    m_magicOvercomePercent        = 0.0;

    /* 加速 */
    Value_t  m_haste               = 0;
    BinPct_t m_hasteBinPercent     = 0;
    Pct_t    m_hastePercent        = 0.0;
    Pct_t    m_hastePercentVisible = 0.0;

    /* 无双 */
    Value_t m_strain                  = 0;
    Pct_t   m_strainPercentFromCustom = 0.0;
    Pct_t   m_strainPercent           = 0.0;

    /* 破招 */
    Value_t  m_surplusBaseFromCustom = 0;
    BinPct_t m_surplusBaseBinPercent = 0;
    Value_t  m_surplus               = 0;
    Value_t  m_surplusDamage         = 0;

    /* 忽视防御 */
    BinPct_t m_shieldIgnoreBinPercent = 0;
};

} // namespace JX3DPS

#endif // ATTR_H
