/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-05 11:36:29
 * @FilePath    : \JX3DPS\modules\Core\Target.cpp
 */

#include "Target.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/Buff/DieRen.h"
#include "Class/TaiXuJianYi/Buff/RenJianHeYiTBuf.h"

namespace JX3DPS {

Target::Target(Player       &player,
               const Id_t    id,
               const int     level,
               const Value_t shield,
               const Class   classType) :
    m_id(id),
    m_level(level), m_lifePercent(1.0), m_shieldIgnoreBinPercent(0), m_physicsShield(0),
    m_physicsShieldBinPercent(0), m_physicsResistPercent(0.0), m_magicShield(0),
    m_magicShieldBinPercent(0), m_magicResistPercent(0.0), m_damageAddBinPercent(0),
    m_damageAddPercent(0.0)
{
    SetPhysicsShield(shield);
    SetMagicShield(shield);
    SetDamageAddBinPercent(0);
}

Target::Target(const Target &target) :
    m_id(target.m_id), m_level(target.m_level), m_lifePercent(target.m_lifePercent),
    m_shieldIgnoreBinPercent(target.m_shieldIgnoreBinPercent),
    m_physicsShield(target.m_physicsShield),
    m_physicsShieldBinPercent(target.m_physicsShieldBinPercent),
    m_physicsResistPercent(target.m_physicsResistPercent), m_magicShield(target.m_magicShield),
    m_magicShieldBinPercent(target.m_magicShieldBinPercent),
    m_magicResistPercent(target.m_magicResistPercent),
    m_damageAddBinPercent(target.m_damageAddBinPercent),
    m_damageAddPercent(target.m_damageAddPercent)
{
    for (auto it : target.buffs) {
        buffs[it.first] = it.second->Clone();
        buffs[it.first]->SetTarget(*this);
    }
}

Target::~Target()
{
    for (auto it : buffs) {
        delete it.second;
    }
}

Target &Target::operator=(const Target &target)
{
    m_id                      = target.m_id;
    m_level                   = target.m_level;
    m_lifePercent             = target.m_lifePercent;
    m_shieldIgnoreBinPercent  = target.m_shieldIgnoreBinPercent;
    m_physicsShield           = target.m_physicsShield;
    m_physicsShieldBinPercent = target.m_physicsShieldBinPercent;
    m_physicsResistPercent    = target.m_physicsResistPercent;
    m_magicShield             = target.m_magicShield;
    m_magicShieldBinPercent   = target.m_magicShieldBinPercent;
    m_magicResistPercent      = target.m_magicResistPercent;
    m_damageAddBinPercent     = target.m_damageAddBinPercent;
    m_damageAddPercent        = target.m_damageAddPercent;
    for (auto it : target.buffs) {
        buffs[it.first] = it.second->Clone();
        buffs[it.first]->SetTarget(*this);
    }
    return *this;
}

void Target::SetPlayer(Player &player)
{
    for (auto it : buffs) {
        it.second->SetPlayer(player);
    }
}

void Target::UpdateTime(const Frame_t frames)
{
    for (const auto &buff : buffs) {
        buff.second->UpdateTime(frames);
    }
}

Frame_t Target::GetNextTime() const
{
    Frame_t min = MAX_FRAMES_CONST;
    for (const auto &buff : buffs) {
        min = GET_MIN_INT(buff.second->GetNextTime(), min);
    }
    return min;
}

Id_t Target::GetId() const
{
    return m_id;
}

int Target::GetLevel() const
{
    return m_level;
}

Pct_t Target::GetLifePercent() const
{
    return m_lifePercent;
}

void Target::SetLifePercent(const Pct_t percent)
{
    m_lifePercent = percent;
}

void Target::AddLifePercent(const Pct_t percent)
{
    m_lifePercent += percent;
}

BinPct_t Target::GetShieldIgnoreBinPercent() const
{
    return m_shieldIgnoreBinPercent;
}

void Target::SetShieldIgnoreBinPercent(const BinPct_t binPercent)
{
    m_shieldIgnoreBinPercent = binPercent;
    UpdatePhysicsResistPercent();
    UpdateMagicResistPercent();
}

Value_t Target::GetPhysicsShield() const
{
    return m_physicsShield;
}

void Target::SetPhysicsShield(const Value_t value)
{
    m_physicsShield = value;
    UpdatePhysicsResistPercent();
}

void Target::AddPhysicsShield(const Value_t value)
{
    m_physicsShield += value;
    UpdatePhysicsResistPercent();
}

BinPct_t Target::GetPhysicsShieldBinPercent() const
{
    return m_physicsShieldBinPercent;
}

void Target::SetPhysicsShieldBinPercent(const BinPct_t binPercent)
{
    m_physicsShieldBinPercent = binPercent;
    UpdatePhysicsResistPercent();
}

void Target::AddPhysicsShieldBinPercent(const BinPct_t binPercent)
{
    m_physicsShieldBinPercent += binPercent;
    UpdatePhysicsResistPercent();
}

Pct_t Target::GetPhysicsResistPercent() const
{
    return m_physicsResistPercent;
}

Value_t Target::GetMagicShield() const
{
    return m_magicShield;
}

void Target::SetMagicShield(const Value_t value)
{
    m_magicShield = value;
    UpdateMagicResistPercent();
}

void Target::AddMagicShield(const Value_t value)
{
    m_magicShield += value;
    UpdateMagicResistPercent();
}

BinPct_t Target::GetMagicShieldBinPercent() const
{
    return m_magicShieldBinPercent;
}

void Target::SetMagicShieldBinPercent(const BinPct_t binPercent)
{
    m_magicShieldBinPercent = binPercent;
    UpdateMagicResistPercent();
}

void Target::AddMagicShieldBinPercent(const BinPct_t binPercent)
{
    m_magicShieldBinPercent += binPercent;
    UpdateMagicResistPercent();
}

Pct_t Target::GetMagicResistPercent() const
{
    return m_magicResistPercent;
}

BinPct_t Target::GetDamageAddBinPercent() const
{
    return m_damageAddBinPercent;
}

void Target::SetDamageAddBinPercent(const BinPct_t binPercent)
{
    m_damageAddBinPercent = binPercent;
    UpdateDamageAddPercent();
}

void Target::AddDamageAddBinPercent(const BinPct_t binPercent)
{
    m_damageAddBinPercent += binPercent;
    UpdateDamageAddPercent();
}

Pct_t Target::GetDamageAddPercent() const
{
    return m_damageAddPercent;
}

void Target::UpdatePhysicsResistPercent()
{
    Value_t physicsShield = static_cast<int>(
        m_physicsShield *
        (static_cast<Pct_t>(MAX_BIN_PCT_CONST + m_physicsShieldBinPercent) / MAX_BIN_PCT_CONST) *
        (static_cast<Pct_t>(MAX_BIN_PCT_CONST - m_shieldIgnoreBinPercent) / MAX_BIN_PCT_CONST));
    m_physicsResistPercent =
        physicsShield /
        (physicsShield + PHYSICS_SHIELD_PARAM * (LEVEL_PARAM * m_level - LEVEL_CONST));
    m_physicsResistPercent = m_physicsResistPercent < 0.75 ? m_physicsResistPercent : 0.75;
}

void Target::UpdateMagicResistPercent()
{
    Value_t magicShield = static_cast<int>(
        m_magicShield *
        (static_cast<Pct_t>(MAX_BIN_PCT_CONST + m_magicShieldBinPercent) / MAX_BIN_PCT_CONST) *
        (static_cast<Pct_t>(MAX_BIN_PCT_CONST - m_shieldIgnoreBinPercent) / MAX_BIN_PCT_CONST));
    m_magicResistPercent =
        magicShield / (magicShield + MAGIC_SHIELD_PARAM * (LEVEL_PARAM * m_level - LEVEL_CONST));
    m_magicResistPercent = m_magicResistPercent < 0.75 ? m_magicResistPercent : 0.75;
}

void Target::UpdateDamageAddPercent()
{
    m_damageAddPercent = static_cast<Pct_t>(m_damageAddBinPercent) / MAX_BIN_PCT_CONST;
}

} // namespace JX3DPS
