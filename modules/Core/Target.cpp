/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 13:44:50
 * @FilePath    : \JX3DPS\modules\Core\Target.cpp
 */

#include "Target.h"

#include "Common/Utils.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/Buff/DieRen.h"
#include "Class/TaiXuJianYi/Buff/RenJianHeYiTBuf.h"

namespace JX3DPS {

Target::Target(Player &player, Id_t id, int level, Value_t shield) :
    m_id(id), m_level(level), m_lifePercent(1.0), m_shieldIgnoreBinPercent(0), m_physicsShield(0),
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
    Frame_t min = CONST_MAX_FRAMES;
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

PCT_t Target::GetLifePercent() const
{
    return m_lifePercent;
}

void Target::SetLifePercent(PCT_t percent)
{
    m_lifePercent = percent;
}

void Target::AddLifePercent(PCT_t percent)
{
    m_lifePercent += percent;
}

BinPCT_t Target::GetShieldIgnoreBinPercent() const
{
    return m_shieldIgnoreBinPercent;
}

void Target::SetShieldIgnoreBinPercent(BinPCT_t binPercent)
{
    m_shieldIgnoreBinPercent = binPercent;
    UpdatePhysicsResistPercent();
    UpdateMagicResistPercent();
}

Value_t Target::GetPhysicsShield() const
{
    return m_physicsShield;
}

void Target::SetPhysicsShield(Value_t value)
{
    m_physicsShield = value;
    UpdatePhysicsResistPercent();
}

void Target::AddPhysicsShield(Value_t value)
{
    m_physicsShield += value;
    UpdatePhysicsResistPercent();
}

BinPCT_t Target::GetPhysicsShieldBinPercent() const
{
    return m_physicsShieldBinPercent;
}

void Target::SetPhysicsShieldBinPercent(BinPCT_t binPercent)
{
    m_physicsShieldBinPercent = binPercent;
    UpdatePhysicsResistPercent();
}

void Target::AddPhysicsShieldBinPercent(BinPCT_t binPercent)
{
    m_physicsShieldBinPercent += binPercent;
    UpdatePhysicsResistPercent();
}

PCT_t Target::GetPhysicsResistPercent() const
{
    return m_physicsResistPercent;
}

Value_t Target::GetMagicShield() const
{
    return m_magicShield;
}

void Target::SetMagicShield(Value_t value)
{
    m_magicShield = value;
    UpdateMagicResistPercent();
}

void Target::AddMagicShield(Value_t value)
{
    m_magicShield += value;
    UpdateMagicResistPercent();
}

BinPCT_t Target::GetMagicShieldBinPercent() const
{
    return m_magicShieldBinPercent;
}

void Target::SetMagicShieldBinPercent(BinPCT_t binPercent)
{
    m_magicShieldBinPercent = binPercent;
    UpdateMagicResistPercent();
}

void Target::AddMagicShieldBinPercent(BinPCT_t binPercent)
{
    m_magicShieldBinPercent += binPercent;
    UpdateMagicResistPercent();
}

PCT_t Target::GetMagicResistPercent() const
{
    return m_magicResistPercent;
}

BinPCT_t Target::GetDamageAddBinPercent() const
{
    return m_damageAddBinPercent;
}

void Target::SetDamageAddBinPercent(BinPCT_t binPercent)
{
    m_damageAddBinPercent = binPercent;
    UpdateDamageAddPercent();
}

void Target::AddDamageAddBinPercent(BinPCT_t binPercent)
{
    m_damageAddBinPercent += binPercent;
    UpdateDamageAddPercent();
}

PCT_t Target::GetDamageAddPercent() const
{
    return m_damageAddPercent;
}

void Target::UpdatePhysicsResistPercent()
{
    Value_t physicsShield = static_cast<int>(
        m_physicsShield *
        (static_cast<PCT_t>(JX3DPS_BIN_PCT + m_physicsShieldBinPercent) / JX3DPS_BIN_PCT) *
        (static_cast<PCT_t>(JX3DPS_BIN_PCT - m_shieldIgnoreBinPercent) / JX3DPS_BIN_PCT));
    m_physicsResistPercent =
        physicsShield /
        (physicsShield + CONST_PHYSICS_SHIELD_PARAM * (CONST_LEVEL_PARAM * m_level - CONST_LEVEL));
    m_physicsResistPercent = m_physicsResistPercent < 0.75 ? m_physicsResistPercent : 0.75;
}

void Target::UpdateMagicResistPercent()
{
    Value_t magicShield = static_cast<int>(
        m_magicShield * (static_cast<PCT_t>(JX3DPS_BIN_PCT + m_magicShieldBinPercent) / JX3DPS_BIN_PCT) *
        (static_cast<PCT_t>(JX3DPS_BIN_PCT - m_shieldIgnoreBinPercent) / JX3DPS_BIN_PCT));
    m_magicResistPercent =
        magicShield /
        (magicShield + CONST_MAGIC_SHIELD_PARAM * (CONST_LEVEL_PARAM * m_level - CONST_LEVEL));
    m_magicResistPercent = m_magicResistPercent < 0.75 ? m_magicResistPercent : 0.75;
}

void Target::UpdateDamageAddPercent()
{
    m_damageAddPercent = static_cast<PCT_t>(m_damageAddBinPercent) / JX3DPS_BIN_PCT;
}

} // namespace JX3DPS
