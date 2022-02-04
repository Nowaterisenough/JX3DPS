/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:08:10
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 12:54:30
 * @FilePath    : \JX3DPS\modules\Class\TaiXuJianYi\Buff\DieRen.cpp
 */
#include "DieRen.h"

#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int DieRen::s_lastFrames = 8 * 3 * 16;
int DieRen::s_intervalFrames = 3 * 16;
int DieRen::s_maxEffectNum = 8;
int DieRen::s_maxStackNum = 5;

DieRen::DieRen(Target &target)
{
    InitBaseParams();
    InitDamageParams();
    m_target = &target;
}

DieRen::~DieRen()
{

}

void DieRen::Cast(Player &player,
                  TargetList &targetList,
                  Stats::ThreadStats &threadStats,
                  Stats::SIM_MODE &simMode)
{
    Stats::TableResult tableResult = GetRollResult(player, *m_target);
    RecordStats(player, *m_target, threadStats, simMode, tableResult);
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_snapshotAttribute.hastePercent);
    m_effectNum--;
    if (m_effectNum == 0) {
        m_lastFrames = -1;
        m_intervalFrames = -1;
        m_stackNum = 0;
    }
}

void DieRen::Refresh(Player &player)
{
    m_snapshotAttribute = player;
    m_lastFrames = static_cast<int>(s_lastFrames * m_snapshotAttribute.hastePercent);
    if (m_stackNum == 0) {
        m_intervalFrames = static_cast<int>(s_intervalFrames * m_snapshotAttribute.hastePercent);
    } else {
        m_lastFrames = m_lastFrames - static_cast<int>(s_intervalFrames * m_snapshotAttribute.hastePercent) +
                       m_intervalFrames;
    }
    m_effectNum = 8;
    m_stackNum++;
    m_stackNum = MIN(m_stackNum, s_maxStackNum);
}

void DieRen::Clean(Player &player,
                   Target &target,
                   Stats::ThreadStats &threadStats,
                   Stats::SIM_MODE &simMode)
{
    Stats::TableResult tableResult = GetRollResult(player, target);
    RecordStatsQieYu(player, target, threadStats, simMode, tableResult);
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}

void DieRen::InitBaseParams()
{
    m_id = TBUF_DIE_REN;
    m_name = "叠刃";
    m_subNameVec.push_back("");
    m_subNameVec.push_back("切玉");
    m_levelNameVec.push_back("1层");
    m_levelNameVec.push_back("2层");
    m_levelNameVec.push_back("3层");
    m_levelNameVec.push_back("4层");
    m_levelNameVec.push_back("5层");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}

void DieRen::InitDamageParams()
{
    m_damageParam.fixedDamage = 0;
    m_damageParam.weaponDamageBinPercent = 0;
    m_damageParam.attackDamagePercent = 0.1155;
}

Stats::TableResult DieRen::GetRollResult(Player &player, Target &target)
{
    return Roll(m_snapshotAttribute.physicsHitValuePercent,
                m_snapshotAttribute.physicsCriticalStrikePercent,
                m_snapshotAttribute.strainPercent,
                target.GetMissPercent(),
                target.GetSightPercent(),
                Stats::RollType::DOT);
}

Stats::DamageStats DieRen::GetDamage(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) *
                        (1 - target.GetPhysicsResistPercent());
    int attackDamage = static_cast<int>(m_damageParam.attackDamagePercent *
                                        m_snapshotAttribute.physicsAttack * percentAdd * m_stackNum);
    int num = tableResult;
    return Stats::DamageStats(0,
                              0,
                              static_cast<int>((attackDamage >> (num / 2)) *
                                               (1 + (num & 1) *
                                                (m_snapshotAttribute.physicsCriticalStrikePowerPercent - 1))));
}

void DieRen::RecordStats(Player &player,
                         Target &target,
                         Stats::ThreadStats &threadStats,
                         Stats::SIM_MODE &simMode,
                         Stats::TableResult tableResult)
{
    Stats::DamageStats damageStats;
    switch (static_cast<int>(simMode)) {
    case Stats::SIM_MODE::DEFAULT :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[0]][m_stackNum - 1].
                second[tableResult].first++;
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[0]][m_stackNum - 1].
                second[tableResult].second += damageStats;
        threadStats.attrBonusMap[Stats::AttrType::DEFAULT].damage += damageStats.SumDamage();
        if (threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].set) {
            player.AddWeaponAttack(threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].damage += damageStats.SumDamage();
            player.AddWeaponAttack(-threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].set) {
            player.AddPhysicsAttackBase(threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].damage += damageStats.SumDamage();
            player.AddPhysicsAttackBase(-threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].set) {
            player.AddPhysicsCriticalStrikePower(threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].damage += damageStats.SumDamage();
            player.AddPhysicsCriticalStrikePower(-threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].set) {
            player.AddPhysicsOvercomeBase(threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].damage += damageStats.SumDamage();
            player.AddPhysicsOvercomeBase(-threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].value);
        }
        break;
    case Stats::SIM_MODE::AGILITY :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::AGILITY].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::HIT_VALUE :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::HIT_VALUE].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::CRITICAL_STRIKE :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::STRAIN :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::STRAIN].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::HASTE :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::HASTE].damage += damageStats.SumDamage();
        break;
    }
}

Stats::DamageStats DieRen::GetDamageQieYu(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) *
                        (1 - target.GetPhysicsResistPercent());
    int attackDamage = static_cast<int>(m_damageParam.attackDamagePercent *
                                        m_snapshotAttribute.physicsAttack * percentAdd * m_stackNum * m_effectNum);
    if (tableResult == Stats::TableResult::DOUBLE) {
        return Stats::DamageStats(0,
                                  0,
                                  static_cast<int>(attackDamage *
                                                   m_snapshotAttribute.physicsCriticalStrikePowerPercent));
    } else {
        return Stats::DamageStats(0,
                                  0,
                                  attackDamage >> tableResult);
    }
}

void DieRen::RecordStatsQieYu(Player &player,
                              Target &target,
                              Stats::ThreadStats &threadStats,
                              Stats::SIM_MODE &simMode,
                              Stats::TableResult tableResult)
{
    Stats::DamageStats damageStats;
    switch (static_cast<int>(simMode)) {
    case Stats::SIM_MODE::DEFAULT :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[1]][m_stackNum - 1].
                second[tableResult].first++;
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[1]][m_stackNum - 1].
                second[tableResult].second += damageStats;
        threadStats.attrBonusMap[Stats::AttrType::DEFAULT].damage += damageStats.SumDamage();
        if (threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].set) {
            player.AddWeaponAttack(threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].damage += damageStats.SumDamage();
            player.AddWeaponAttack(-threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].set) {
            player.AddPhysicsAttackBase(threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].damage += damageStats.SumDamage();
            player.AddPhysicsAttackBase(-threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].set) {
            player.AddPhysicsCriticalStrikePower(threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].damage += damageStats.SumDamage();
            player.AddPhysicsCriticalStrikePower(-threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].set) {
            player.AddPhysicsOvercomeBase(threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].damage += damageStats.SumDamage();
            player.AddPhysicsOvercomeBase(-threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].value);
        }
        break;
    case Stats::SIM_MODE::AGILITY :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::AGILITY].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::HIT_VALUE :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::HIT_VALUE].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::CRITICAL_STRIKE :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::STRAIN :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::STRAIN].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::HASTE :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::HASTE].damage += damageStats.SumDamage();
        break;
    }
}
