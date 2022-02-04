#include "ChiYing.h"

#include "Core/Target.h"
#include "Core/Player.h"

int ChiYing::s_lastFrames = 15 * 16;
int ChiYing::s_maxStackNum = 3;

ChiYing::ChiYing()
{
    InitBaseParams();
    InitDamageParams();
}

ChiYing::~ChiYing()
{

}

void ChiYing::Cast(Player &player,
                   TargetList &targetList,
                   Stats::ThreadStats &threadStats,
                   Stats::SIM_MODE &simMode)
{
    m_lastFrames = -1;
    m_stackNum = 0;
    m_effectNum = 0;
}

void ChiYing::Refresh(Player &player)
{
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_effectNum = 1;
    m_stackNum++;
    m_stackNum = MIN(s_maxStackNum, m_stackNum);
}

void ChiYing::Clean(Player &player,
                    Target &target,
                    Stats::ThreadStats &threadStats,
                    Stats::SIM_MODE &simMode)
{
    Stats::TableResult tableResult = GetRollResult(player, target);
    RecordStats(player, target, threadStats, simMode, tableResult);
    m_stackNum--;
    if (m_stackNum == 0) {
        m_lastFrames = -1;
        m_effectNum = 0;
    }
}

void ChiYing::Clean(Player &player,
                    TargetList &targetList,
                    Stats::ThreadStats &threadStats,
                    Stats::SIM_MODE &simMode)
{
    for (auto it : targetList) {
        if (m_stackNum > 0) {
            m_stackNum--;
            Stats::TableResult tableResult = GetRollResult(player, *it);
            RecordStats(player, *it, threadStats, simMode, tableResult);
        }
    }
    if (m_stackNum == 0) {
        m_lastFrames = -1;
        m_effectNum = 0;
    }
}

void ChiYing::InitBaseParams()
{
    m_id = BUF_CHI_YING;
    m_name = "持盈";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}

void ChiYing::InitDamageParams()
{
    m_damageParam.fixedDamage = (35 + 55) / 2;
    m_damageParam.weaponDamageBinPercent = 0;
    m_damageParam.attackDamagePercent = 0.795;
}

Stats::TableResult ChiYing::GetRollResult(Player &player, Target &target)
{
    return Roll(player.GetPhysicsHitValuePercent(),
                player.GetPhysicsCriticalStrikePercent(),
                player.GetStrainPercent(),
                target.GetMissPercent(),
                target.GetSightPercent());
}

Stats::DamageStats ChiYing::GetDamage(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) *
                        (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParam.fixedDamage * percentAdd);
    int weaponDamage = static_cast<int>((m_damageParam.weaponDamageBinPercent >> 10) *
                                        player.GetWeaponAttack() * percentAdd);
    int attackDamage = static_cast<int>(m_damageParam.attackDamagePercent *
                                        player.GetPhysicsAttack() * percentAdd);
    int num = tableResult;
    return Stats::DamageStats(static_cast<int>((fixedDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              static_cast<int>((weaponDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              static_cast<int>((attackDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))));
}

void ChiYing::RecordStats(Player &player,
                          Target &target,
                          Stats::ThreadStats &threadStats,
                          Stats::SIM_MODE &simMode,
                          Stats::TableResult tableResult)
{
    Stats::DamageStats damageStats;
    switch (static_cast<int>(simMode)) {
    case Stats::SIM_MODE::DEFAULT :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[0]][0].
                second[tableResult].first++;
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[0]][0].
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
