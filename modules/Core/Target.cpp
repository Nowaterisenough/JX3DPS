#include "Target.h"

#include "Core/Buff.h"
#include "Class/TaiXuJianYi/Buff/DieRen.h"
#include "Class/TaiXuJianYi/Buff/RenJianHeYiTBuf.h"

namespace JX3DPS {

Target::Target(Id_t id, int level, Value_t shield, Pct_t missPercent, Pct_t sightPercent, Class classType) :
    m_id(id), m_level(level), m_missPercent(missPercent), m_sightPercent(sightPercent),
    m_lifeBinPercent(MAX_BIN_PCT_CONST), m_lifePercent(MAX_PCT_CONST), m_shieldIgnoreBinPercent(0),
    m_physicsShield(0), m_physicsShieldBinPercent(0), m_physicsResistPercent(0.0),
    m_magicShield(0), m_magicShieldBinPercent(0), m_magicResistPercent(0.0),
    m_damageBinPercent(0), m_damagePercentAdd(0.0)
{
    AddPhysicsShield(shield);
    AddMagicShield(shield);
    InitClassTBuff(classType);
}

Target::~Target()
{
    for (auto it : m_buffMap) {
        delete it.second;
        it.second = nullptr;
    }
    m_buffMap.clear();
}

Id_t Target::GetId()
{
    return m_id;
}

Pct_t Target::GetMissPercent()
{
    return m_missPercent;
}

Pct_t Target::GetSightPercent()
{
    return m_sightPercent;
}

BinPct_t Target::SetLifeBinPercent(BinPct_t binPercent)
{
    m_lifeBinPercent = binPercent;
    UpdateLife();
    return m_lifeBinPercent;
}

BinPct_t Target::GetLifeBinPercent()
{
    return m_lifeBinPercent;
}

Pct_t Target::GetLifePercent()
{
    return m_lifePercent;
}

BinPct_t Target::SetShieldIgnoreBinPercent(BinPct_t binPercent)
{
    m_shieldIgnoreBinPercent = binPercent;
    UpdatePhysicsResistPercent();
    UpdateMagicResistPercent();
    return m_shieldIgnoreBinPercent;
}

Value_t Target::AddPhysicsShield(Value_t value)
{
    m_physicsShield += value;
    UpdatePhysicsResistPercent();
    return m_physicsShield;
}

BinPct_t Target::AddPhysicsShieldBinPercent(BinPct_t binPercent)
{
    m_physicsShieldBinPercent += binPercent;
    UpdatePhysicsResistPercent();
    return m_physicsShieldBinPercent;
}

Pct_t Target::GetPhysicsResistPercent()
{
    return m_physicsResistPercent;
}

Value_t Target::AddMagicShield(Value_t value)
{
    m_magicShield += value;
    UpdateMagicResistPercent();
    return m_magicShield;
}

BinPct_t Target::AddMagicShieldBinPercent(BinPct_t binPercent)
{
    m_magicShieldBinPercent += binPercent;
    UpdateMagicResistPercent();
    return m_magicShieldBinPercent;
}

Pct_t Target::GetMagicResistPercent()
{
    return m_magicResistPercent;
}

BinPct_t Target::AddDamageBinPercent(BinPct_t binPercent)
{
    m_damageBinPercent += binPercent;
    UpdateDamagePercentAdd();
    return m_damageBinPercent;
}

Pct_t Target::GetDamagePercentAdd()
{
    return m_damagePercentAdd;
}

void Target::InitClassTBuff(Class classType)
{
    switch (static_cast<int>(classType)) {
    case TAI_XU_JIAN_YI:
        m_buffMap[TBUF_DIE_REN] = new DieRen(*this);
        m_buffMap[TBUF_REN_JIAN_HE_YI] = new RenJianHeYiTBuf(*this);
        break;
    }
}

void Target::UpdateLife()
{
    m_lifePercent = static_cast<Pct_t>(m_lifeBinPercent) / MAX_BIN_PCT_CONST;
}

void Target::UpdatePhysicsResistPercent()
{
    Value_t physicsShield = static_cast<int>(m_physicsShield *
                                             (static_cast<Pct_t>(MAX_BIN_PCT_CONST + m_physicsShieldBinPercent) / MAX_BIN_PCT_CONST) *
                                             (static_cast<Pct_t>(MAX_BIN_PCT_CONST - m_shieldIgnoreBinPercent) / MAX_BIN_PCT_CONST));
    m_physicsResistPercent = physicsShield / (physicsShield + PHYSICS_SHIELD_PARAM *
                                              (LEVEL_PARAM(m_level) * m_level - LEVEL_CONST(m_level)));
    m_physicsResistPercent = m_physicsResistPercent < 0.75 ? m_physicsResistPercent : 0.75;
}

void Target::UpdateMagicResistPercent()
{
    Value_t magicShield = static_cast<int>(m_magicShield *
                                           (static_cast<Pct_t>(MAX_BIN_PCT_CONST + m_magicShieldBinPercent) / MAX_BIN_PCT_CONST) *
                                           (static_cast<Pct_t>(MAX_BIN_PCT_CONST - m_shieldIgnoreBinPercent) / MAX_BIN_PCT_CONST));
    m_magicResistPercent = magicShield / (magicShield + MAGIC_SHIELD_PARAM *
                                          (LEVEL_PARAM(m_level) * m_level - LEVEL_CONST(m_level)));
    m_magicResistPercent = m_magicResistPercent < 0.75 ? m_magicResistPercent : 0.75;
}

void Target::UpdateDamagePercentAdd()
{
    m_damagePercentAdd = static_cast<Pct_t>(m_damageBinPercent) / MAX_BIN_PCT_CONST;
}

}
