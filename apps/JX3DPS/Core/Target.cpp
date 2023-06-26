/**
 * Project: JX3DPS
 * File: Target.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-22 14:36:23
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Target.h"

#include "Buff.h"
#include "JX3Params.h"

JX3DPS::Target::Target() { }

JX3DPS::Target::~Target() { }

void JX3DPS::Target::SetId(Id_t id)
{
    m_id = id;
}

JX3DPS::Id_t JX3DPS::Target::GetId() const
{
    return m_id;
}

void JX3DPS::Target::SetLevel(int level)
{
    m_level = level;
}

int JX3DPS::Target::GetLevel() const
{
    return m_level;
}

void JX3DPS::Target::SetDistance(float distance)
{
    m_distance = distance;
}

float JX3DPS::Target::GetDistance() const
{
    return m_distance;
}

JX3DPS::PctFloat_t JX3DPS::Target::GetLifePercent() const
{
    return m_lifePercent;
}

void JX3DPS::Target::SetLifePercent(const PctFloat_t percent)
{
    m_lifePercent = percent;
}

void JX3DPS::Target::AddLifePercent(const PctFloat_t percent)
{
    m_lifePercent += percent;
}

JX3DPS::PctFloat_t JX3DPS::Target::GetManaPercent() const
{
    return m_manaPercent;
}

void JX3DPS::Target::SetManaPercent(const PctFloat_t percent)
{
    m_manaPercent = percent;
}

void JX3DPS::Target::AddManaPercent(const PctFloat_t percent)
{
    m_manaPercent += percent;
}

JX3DPS::PctInt_t JX3DPS::Target::GetShieldIgnoreBinPercent() const
{
    return m_shieldIgnoreBinPercent;
}

void JX3DPS::Target::SetShieldIgnoreBinPercent(const PctInt_t binPercent)
{
    m_shieldIgnoreBinPercent = binPercent;
    UpdatePhysicsResistPercent();
    UpdateMagicResistPercent();
}

JX3DPS::Value_t JX3DPS::Target::GetPhysicsShield() const
{
    return m_physicsShield;
}

void JX3DPS::Target::SetPhysicsShield(const Value_t value)
{
    m_physicsShield = value;
    UpdatePhysicsResistPercent();
}

void JX3DPS::Target::AddPhysicsShield(const Value_t value)
{
    m_physicsShield += value;
    UpdatePhysicsResistPercent();
}

JX3DPS::PctInt_t JX3DPS::Target::GetPhysicsShieldBinPercent() const
{
    return m_physicsShieldBinPercent;
}

void JX3DPS::Target::SetPhysicsShieldBinPercent(const PctInt_t binPercent)
{
    m_physicsShieldBinPercent = binPercent;
    UpdatePhysicsResistPercent();
}

void JX3DPS::Target::AddPhysicsShieldBinPercent(const PctInt_t binPercent)
{
    m_physicsShieldBinPercent += binPercent;
    UpdatePhysicsResistPercent();
}

JX3DPS::PctFloat_t JX3DPS::Target::GetPhysicsResistPercent() const
{
    return m_physicsResistPercent;
}

JX3DPS::Value_t JX3DPS::Target::GetMagicShield() const
{
    return m_magicShield;
}

void JX3DPS::Target::SetMagicShield(const Value_t value)
{
    m_magicShield = value;
    UpdateMagicResistPercent();
}

void JX3DPS::Target::AddMagicShield(const Value_t value)
{
    m_magicShield += value;
    UpdateMagicResistPercent();
}

JX3DPS::PctInt_t JX3DPS::Target::GetMagicShieldBinPercent() const
{
    return m_magicShieldBinPercent;
}

void JX3DPS::Target::SetMagicShieldBinPercent(const PctInt_t binPercent)
{
    m_magicShieldBinPercent = binPercent;
    UpdateMagicResistPercent();
}

void JX3DPS::Target::AddMagicShieldBinPercent(const PctInt_t binPercent)
{
    m_magicShieldBinPercent += binPercent;
    UpdateMagicResistPercent();
}

JX3DPS::PctFloat_t JX3DPS::Target::GetMagicResistPercent() const
{
    return m_magicResistPercent;
}

JX3DPS::PctInt_t JX3DPS::Target::GetDamageAddBinPercent() const
{
    return m_damageAddBinPercent;
}

void JX3DPS::Target::SetDamageAddBinPercent(const PctInt_t binPercent)
{
    m_damageAddBinPercent = binPercent;
    UpdateDamageAddPercent();
}

void JX3DPS::Target::AddDamageAddBinPercent(const PctInt_t binPercent)
{
    m_damageAddBinPercent += binPercent;
    UpdateDamageAddPercent();
}

JX3DPS::PctFloat_t JX3DPS::Target::GetDamageAddPercent() const
{
    return m_damageAddPercent;
}

void JX3DPS::Target::UpdatePhysicsResistPercent()
{
    Value_t physicsShield =
        static_cast<int>(m_physicsShield *
                         (static_cast<PctFloat_t>(JX3_BIN_PCT + m_physicsShieldBinPercent) / JX3_BIN_PCT) *
                         (static_cast<PctFloat_t>(JX3_BIN_PCT - m_shieldIgnoreBinPercent) / JX3_BIN_PCT));
    m_physicsResistPercent =
        physicsShield /
        (physicsShield + JX3_PHYSICS_SHIELD_PARAM * (JX3_LEVEL_PARAM * m_level - JX3_LEVEL_CONST));
    m_physicsResistPercent = m_physicsResistPercent < 0.75 ? m_physicsResistPercent : 0.75;
}

void JX3DPS::Target::UpdateMagicResistPercent()
{
    Value_t magicShield =
        static_cast<int>(m_magicShield *
                         (static_cast<PctFloat_t>(JX3_BIN_PCT + m_magicShieldBinPercent) / JX3_BIN_PCT) *
                         (static_cast<PctFloat_t>(JX3_BIN_PCT - m_shieldIgnoreBinPercent) / JX3_BIN_PCT));
    m_magicResistPercent =
        magicShield / (magicShield + JX3_MAGIC_SHIELD_PARAM * (JX3_LEVEL_PARAM * m_level - JX3_LEVEL_CONST));
    m_magicResistPercent = m_magicResistPercent < 0.75 ? m_magicResistPercent : 0.75;
}

void JX3DPS::Target::UpdateDamageAddPercent()
{
    m_damageAddPercent = static_cast<PctFloat_t>(m_damageAddBinPercent) / JX3_BIN_PCT;
}
