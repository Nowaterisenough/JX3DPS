/**
 * @Description : 属性类, 包括主属性、攻击、会心、破防等基本类别及转换关系
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 10:33:49
 * @FilePath    : \JX3DPS\modules\Core\Attr.h
 */

#ifndef ATTR_H
#define ATTR_H

#include "Common/ConstVal.h"

namespace JX3DPS {

class Attr
{
public:
    Attr(JX3Class classType);
    ~Attr();

    /* 身法 */
    Value_t  GetAgilityBase() const;
    void     SetAgilityBase(Value_t value);
    void     AddAgilityBase(Value_t value);
    BinPCT_t GetAgilityBaseBinPercent() const;
    void     SetAgilityBaseBinPercent(BinPCT_t binPercent);
    void     AddAgilityBaseBinPercent(BinPCT_t binPercent);
    Value_t  GetAgility() const;

    /* 力道 */
    Value_t  GetStrengthBase() const;
    void     SetStrengthBase(Value_t value);
    void     AddStrengthBase(Value_t value);
    BinPCT_t GetStrengthBaseBinPercent() const;
    void     SetStrengthBaseBinPercent(BinPCT_t binPercent);
    void     AddStrengthBaseBinPercent(BinPCT_t binPercent);
    Value_t  GetStrength() const;

    /* 根骨 */
    Value_t  GetSpiritBase() const;
    void     SetSpiritBase(Value_t value);
    void     AddSpiritBase(Value_t value);
    BinPCT_t GetSpiritBaseBinPercent() const;
    void     SetSpiritBaseBinPercent(BinPCT_t binPercent);
    void     AddSpiritBaseBinPercent(BinPCT_t binPercent);
    Value_t  GetSpirit() const;

    /* 元气 */
    Value_t  GetSpunkBase() const;
    void     SetSpunkBase(Value_t value);
    void     AddSpunkBase(Value_t value);
    BinPCT_t GetSpunkBaseBinPercent() const;
    void     SetSpunkBaseBinPercent(BinPCT_t binPercent);
    void     AddSpunkBaseBinPercent(BinPCT_t binPercent);
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
    BinPCT_t GetPhysicsAttackBaseBinPercent() const;
    void     SetPhysicsAttackBaseBinPercent(BinPCT_t binPercent);
    void     AddPhysicsAttackBaseBinPercent(BinPCT_t binPercent);
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
    BinPCT_t GetMagicAttackBaseBinPercent() const;
    void     SetMagicAttackBaseBinPercent(BinPCT_t binPercent);
    void     AddMagicAttackBaseBinPercent(BinPCT_t binPercent);
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
    PCT_t   GetPhysicsCriticalStrikePercentFromCustom() const;
    void    SetPhysicsCriticalStrikePercentFromCustom(PCT_t percent);
    void    AddPhysicsCriticalStrikePercentFromCustom(PCT_t percent);
    PCT_t   GetPhysicsCriticalStrikePercent() const;
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
    PCT_t   GetMagicCriticalStrikePercentFromCustom() const;
    void    SetMagicCriticalStrikePercentFromCustom(PCT_t percent);
    void    AddMagicCriticalStrikePercentFromCustom(PCT_t percent);
    PCT_t   GetMagicCriticalStrikePercent() const;
    Value_t GetMagicCriticalStrikeMinimum() const;

    /* 外功会心效果 */
    Value_t GetPhysicsCriticalStrikePower() const;
    void    SetPhysicsCriticalStrikePower(Value_t value);
    void    AddPhysicsCriticalStrikePower(Value_t value);
    PCT_t   GetPhysicsCriticalStrikePowerPercentFromCustom() const;
    void    SetPhysicsCriticalStrikePowerPercentFromCustom(PCT_t percent);
    void    AddPhysicsCriticalStrikePowerPercentFromCustom(PCT_t percent);
    PCT_t   GetPhysicsCriticalStrikePowerPercent() const;

    /* 内功会心效果 */
    Value_t GetMagicCriticalStrikePower() const;
    void    SetMagicCriticalStrikePower(Value_t value);
    void    AddMagicCriticalStrikePower(Value_t value);
    PCT_t   GetMagicCriticalStrikePowerPercentFromCustom() const;
    void    SetMagicCriticalStrikePowerPercentFromCustom(PCT_t percent);
    void    AddMagicCriticalStrikePowerPercentFromCustom(PCT_t percent);
    PCT_t   GetMagicCriticalStrikePowerPercent() const;

    /* 外功破防 */
    Value_t  GetPhysicsOvercomeBaseFromCustom() const;
    void     SetPhysicsOvercomeBaseFromCustom(Value_t value);
    void     AddPhysicsOvercomeBaseFromCustom(Value_t value);
    Value_t  GetPhysicsOvercomeBaseFromMajor() const;
    Value_t  GetPhysicsOvercomeBase() const;
    void     SetPhysicsOvercomeBase(Value_t value);
    void     AddPhysicsOvercomeBase(Value_t value);
    BinPCT_t GetPhysicsOvercomeBaseBinPercent() const;
    void     SetPhysicsOvercomeBaseBinPercent(BinPCT_t binPercent);
    void     AddPhysicsOvercomeBaseBinPercent(BinPCT_t binPercent);
    Value_t  GetPhysicsOvercomeFromClass() const;
    Value_t  GetPhysicsOvercome() const;
    PCT_t    GetPhysicsOvercomePercent() const;
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
    BinPCT_t GetMagicOvercomeBaseBinPercent() const;
    void     SetMagicOvercomeBaseBinPercent(BinPCT_t binPercent);
    void     AddMagicOvercomeBaseBinPercent(BinPCT_t binPercent);
    Value_t  GetMagicOvercomeFromClass() const;
    Value_t  GetMagicOvercome() const;
    PCT_t    GetMagicOvercomePercent() const;
    Value_t  GetMagicOvercomeBaseMinimum() const;
    Value_t  GetMagicOvercomeMinimum() const;

    /* 加速 */
    Value_t  GetHaste() const;
    void     SetHaste(Value_t value);
    void     AddHaste(Value_t value);
    BinPCT_t GetHasteBinPercent() const;
    void     SetHasteBinPercent(BinPCT_t binPercent);
    void     AddHasteBinPercent(BinPCT_t binPercent);
    PCT_t    GetHastePercent() const;
    PCT_t    GetHastePercentVisible() const;

    /* 无双 */
    Value_t GetStrain() const;
    void    SetStrain(Value_t value);
    void    AddStrain(Value_t value);
    PCT_t   GetStrainPercent() const;
    void    AddStrainPercent(PCT_t percent);

    /* 破招 */
    Value_t  GetSurplusBaseFromCustom() const;
    void     SetSurplusBaseFromCustom(Value_t value);
    void     AddSurplusBaseFromCustom(Value_t value);
    Value_t  GetSurplusBaseFromMajor() const;
    Value_t  GetSurplusBase() const;
    void     SetSurplusBase(Value_t value);
    void     AddSurplusBase(Value_t value);
    BinPCT_t GetSurplusBaseBinPercent() const;
    void     SetSurplusBaseBinPercent(BinPCT_t binPercent);
    void     AddSurplusBaseBinPercent(BinPCT_t binPercent);
    Value_t  GetSurplusFromClass() const;
    Value_t  GetSurplus() const;
    Value_t  GetSurplusBaseMinimum() const;
    Value_t  GetSurplusMinimum() const;
    Value_t  GetSurplusDamage() const;

    /* 忽视防御 */
    BinPCT_t GetShieldIgnoreBinPercent() const;
    void     SetShieldIgnoreBinPercent(Targets &targets, BinPCT_t binPercent);
    void     AddShieldIgnoreBinPercent(Targets &targets, BinPCT_t binPercent);

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
    void UpdateShieldIgnorePercent(Targets &targets, BinPCT_t binPercent);

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
    BinPCT_t m_agilityBaseBinPercent = 0;
    Value_t  m_agility               = 0;

    /* 力道 */
    Value_t  m_strengthBase           = 0;
    BinPCT_t m_strengthBaseBinPercent = 0;
    Value_t  m_strength               = 0;

    /* 根骨 */
    Value_t  m_spiritBase           = 0;
    BinPCT_t m_spiritBaseBinPercent = 0;
    Value_t  m_spirit               = 0;

    /* 元气 */
    Value_t  m_spunkBase           = 0;
    BinPCT_t m_spunkBaseBinPercent = 0;
    Value_t  m_spunk               = 0;

    /* 武器伤害 */
    Value_t m_weaponAttack = 0;

    /* 外功攻击 */
    Value_t  m_physicsAttackBaseFromCustom = 0;
    BinPCT_t m_physicsAttackBaseBinPercent = 0;
    Value_t  m_physicsAttack               = 0;

    /* 内功攻击 */
    Value_t  m_magicAttackBaseFromCustom = 0;
    BinPCT_t m_magicAttackBaseBinPercent = 0;
    Value_t  m_magicAttack               = 0;

    /* 外功会心 */
    Value_t m_physicsCriticalStrike                  = 0;
    Value_t m_physicsCriticalStrikeFromCustom        = 0;
    PCT_t   m_physicsCriticalStrikePercentFromCustom = 0.0;
    PCT_t   m_physicsCriticalStrikePercent           = 0.0;

    /* 内功会心 */
    Value_t m_magicCriticalStrike                  = 0;
    Value_t m_magicCriticalStrikeFromCustom        = 0;
    PCT_t   m_magicCriticalStrikePercentFromCustom = 0.0;
    PCT_t   m_magicCriticalStrikePercent           = 0.0;

    /* 外功会心效果 */
    Value_t m_physicsCriticalStrikePower                  = 0;
    PCT_t   m_physicsCriticalStrikePowerPercentFromCustom = 0.0;
    PCT_t   m_physicsCriticalStrikePowerPercent = CONST_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    /* 内功会心效果 */
    Value_t m_magicCriticalStrikePower                  = 0;
    PCT_t   m_magicCriticalStrikePowerPercentFromCustom = 0.0;
    PCT_t   m_magicCriticalStrikePowerPercent = CONST_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE;

    /* 外功破防 */
    Value_t  m_physicsOvercomeBaseFromCustom = 0;
    BinPCT_t m_physicsOvercomeBaseBinPercent = 0;
    Value_t  m_physicsOvercome               = 0;
    PCT_t    m_physicsOvercomePercent        = 0.0;

    /* 内功破防 */
    Value_t  m_magicOvercomeBaseFromCustom = 0;
    BinPCT_t m_magicOvercomeBaseBinPercent = 0;
    Value_t  m_magicOvercome               = 0;
    PCT_t    m_magicOvercomePercent        = 0.0;

    /* 加速 */
    Value_t  m_haste               = 0;
    BinPCT_t m_hasteBinPercent     = 0;
    PCT_t    m_hastePercent        = 0.0;
    PCT_t    m_hastePercentVisible = 0.0;

    /* 无双 */
    Value_t m_strain                  = 0;
    PCT_t   m_strainPercentFromCustom = 0.0;
    PCT_t   m_strainPercent           = 0.0;

    /* 破招 */
    Value_t  m_surplusBaseFromCustom = 0;
    BinPCT_t m_surplusBaseBinPercent = 0;
    Value_t  m_surplus               = 0;
    Value_t  m_surplusDamage         = 0;

    /* 忽视防御 */
    BinPCT_t m_shieldIgnoreBinPercent = 0;
};

} // namespace JX3DPS

#endif // ATTR_H
