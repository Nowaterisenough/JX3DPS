
#include "DieRen.h"

#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t DieRen::s_lastFrames = 8 * 3 * 16;
Frame_t DieRen::s_intervalFrames = 3 * 16;
int DieRen::s_maxEffectNum = 8;
int DieRen::s_maxStackNum = 5;

DieRen::DieRen(Player &player, Target &target) :
    Buff(player)
{
    InitBaseParams();
    InitDamageParams();
    m_target = &target;
}

DieRen::DieRen(const DieRen &buff) : Buff(buff)
{

}

DieRen::~DieRen()
{

}

DieRen *DieRen::Clone()
{
    return new DieRen(*this);
}

DieRen &DieRen::operator=(const DieRen &buff)
{
    Buff::operator=(buff);
    return *this;
}

void DieRen::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    TableRes tableRes = GetPhysicsRollResult(*m_attrSnap, *m_target, RollType::DOT);
    UpdatePhysicsStats(*m_attrSnap, *m_target, tableRes, m_subNames[0], m_stackCount - 1);
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_attrSnap->GetHastePercent());
    m_effectCount--;
    m_lastFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_effectCount, INVALID_FRAMES_SET, m_lastFrames);
    m_intervalFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_effectCount, INVALID_FRAMES_SET, m_intervalFrames);
    m_stackCount = IF_1ST_0_TO_0_ELSE_2ND(m_effectCount, m_stackCount);
}

void DieRen::Refresh()
{
    *m_attrSnap = m_player->GetAttr();
    m_lastFrames = static_cast<int>(s_lastFrames * m_attrSnap->GetHastePercent());
    if (m_stackCount == 0) {
        m_intervalFrames = static_cast<int>(s_intervalFrames * m_attrSnap->GetHastePercent());
    } else {
        m_lastFrames = m_lastFrames - static_cast<int>(s_intervalFrames * m_attrSnap->GetHastePercent()) +
                       m_intervalFrames;
    }
    m_effectCount = s_maxEffectNum;
    m_stackCount++;
    m_stackCount = GET_MIN_INT(m_stackCount, s_maxStackNum);
}

void DieRen::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    if (param != FORCE) {
        TableRes tableRes = GetPhysicsRollResult(*m_attrSnap, *m_target, RollType::DOT);
        UpdatePhysicsStatsQieYu(*m_attrSnap, *m_target, tableRes, m_subNames[1], m_stackCount - 1);
    }
    m_lastFrames = INVALID_FRAMES_SET;
    m_intervalFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
    m_stackCount = 0;
}

void DieRen::InitBaseParams()
{
    m_id = TBUF_DIE_REN;
    m_name = "叠刃";
    m_subNames.push_back("");
    m_subNames.push_back("切玉");
    m_levelNames.push_back("1层");
    m_levelNames.push_back("2层");
    m_levelNames.push_back("3层");
    m_levelNames.push_back("4层");
    m_levelNames.push_back("5层");
    m_isDamage = true;
}

void DieRen::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0].fixedDamage = 0;
    m_damageParams[m_subNames[0]][0].weaponDamageBinPercent = 0;
    m_damageParams[m_subNames[0]][0].attackDamagePercent = 0.1155;
    m_damageParams[m_subNames[0]][1].fixedDamage = 0;
    m_damageParams[m_subNames[0]][1].weaponDamageBinPercent = 0;
    m_damageParams[m_subNames[0]][1].attackDamagePercent = 0.1155 * 2;
    m_damageParams[m_subNames[0]][2].fixedDamage = 0;
    m_damageParams[m_subNames[0]][2].weaponDamageBinPercent = 0;
    m_damageParams[m_subNames[0]][2].attackDamagePercent = 0.1155 * 3;
    m_damageParams[m_subNames[0]][3].fixedDamage = 0;
    m_damageParams[m_subNames[0]][3].weaponDamageBinPercent = 0;
    m_damageParams[m_subNames[0]][3].attackDamagePercent = 0.1155 * 4;
    m_damageParams[m_subNames[0]][4].fixedDamage = 0;
    m_damageParams[m_subNames[0]][4].weaponDamageBinPercent = 0;
    m_damageParams[m_subNames[0]][4].attackDamagePercent = 0.1155 * 5;
}

Damage DieRen::GetPhysicsDamageQieYu(Attr &attr,
                                          Target &target,
                                          TableRes tableRes,
                                          std::string &subName,
                                          int level)
{
    Pct_t damagePercent = (MAX_BIN_PCT_CONST / static_cast<double>(MAX_BIN_PCT_CONST)) *
                          (1 + m_player->GetAttr().GetPhysicsOvercomePercent()) *
                          (1 + target.GetDamagePercentAdd()) *
                          (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams[subName][level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParams[subName][level].weaponDamageBinPercent >> 10) *
                                        m_player->GetAttr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams[subName][level].attackDamagePercent *
                                        attr.GetPhysicsAttack() * damagePercent * m_effectCount);
    int tableResNum = tableRes;
    return Damage(GET_FINAL_DAMAGE(fixedDamage,
                                        tableResNum,
                                        (attr.GetPhysicsCriticalStrikePowerPercent())),
                       GET_FINAL_DAMAGE(weaponDamage,
                                        tableResNum,
                                        (attr.GetPhysicsCriticalStrikePowerPercent())),
                       GET_FINAL_DAMAGE(attackDamage,
                                        tableResNum,
                                        (attr.GetPhysicsCriticalStrikePowerPercent())));
}

void DieRen::UpdatePhysicsStatsQieYu(Attr &attr,
                                     Target &target,
                                     Stats &stats,
                                     Settings &settings,
                                     TableRes tableRes,
                                     std::string &subName,
                                     int level)
{
    Damage damage;
    switch (static_cast<int>(settings.mode)) {
    case SIM_MODE::DEFAULT :
        damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
        stats.damageStats[target.GetId()][m_id][subName][level][tableRes].first++;
        stats.damageStats[target.GetId()][m_id][subName][level][tableRes].second += damage;
        stats.bonusStats[AttrType::DEFAULT] += damage.SumDamage();
        if (settings.bonusSets[AttrType::WEAPON_ATTACK]->set) {
            m_player->GetAttr().AddWeaponAttack(settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
            damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
            stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.SumDamage();
            m_player->GetAttr().AddWeaponAttack(-settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
        }
        if (settings.bonusSets[AttrType::ATTACK_BASE]->set) {
            m_player->GetAttr().AddPhysicsAttackBase(settings.bonusSets[AttrType::ATTACK_BASE]->value);
            damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
            stats.bonusStats[AttrType::ATTACK_BASE] += damage.SumDamage();
            m_player->GetAttr().AddPhysicsAttackBase(-settings.bonusSets[AttrType::ATTACK_BASE]->value);
        }
        if (settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->set) {
            m_player->GetAttr().AddPhysicsCriticalStrikePower(settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
            damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
            stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.SumDamage();
            m_player->GetAttr().AddPhysicsCriticalStrikePower(-settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
        }
        if (settings.bonusSets[AttrType::OVERCOME_BASE]->set) {
            m_player->GetAttr().AddPhysicsOvercomeBase(settings.bonusSets[AttrType::OVERCOME_BASE]->value);
            damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
            stats.bonusStats[AttrType::OVERCOME_BASE] += damage.SumDamage();
            m_player->GetAttr().AddPhysicsOvercomeBase(-settings.bonusSets[AttrType::OVERCOME_BASE]->value);
        }
        break;
    case SIM_MODE::PRIMARY :
        damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
        stats.bonusStats[AttrType::PRIMARY] += damage.SumDamage();
        break;
    case SIM_MODE::HIT_VALUE :
        damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
        stats.bonusStats[AttrType::HIT_VALUE] += damage.SumDamage();
        break;
    case SIM_MODE::CRITICAL_STRIKE :
        damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
        stats.bonusStats[AttrType::CRITICAL_STRIKE] += damage.SumDamage();
        break;
    case SIM_MODE::HASTE :
        damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
        stats.bonusStats[AttrType::HASTE] += damage.SumDamage();
        break;
    case SIM_MODE::STRAIN :
        damage = GetPhysicsDamageQieYu(attr, target, tableRes, subName, level);
        stats.bonusStats[AttrType::STRAIN] += damage.SumDamage();
        break;
    }
}

}

}
