#include "YunZhongJianY.h"

#include "Core/Target.h"
#include "Core/Player.h"

int YunZhongJianY::s_lastFrames = 8 * 16;
int YunZhongJianY::s_intervalFrames = 16;
int YunZhongJianY::s_maxEffectNum = 8;

YunZhongJianY::YunZhongJianY()
{
    InitBaseParams();
    InitDamageParams();
}

YunZhongJianY::~YunZhongJianY()
{

}

void YunZhongJianY::Cast(Player &player,
                         TargetList &targetList,
                         Stats::ThreadStats &threadStats,
                         Stats::SIM_MODE &simMode)
{
    for (auto it : targetList) {
        Stats::TableResult tableResult = GetRollResult(player, *it);
        RecordStats(player, *it, threadStats, simMode, tableResult);
    }
    m_intervalFrames = s_intervalFrames;
    m_effectNum--;
    if (m_effectNum == 0) {
        m_lastFrames = -1;
        m_intervalFrames = -1;
    }
}

void YunZhongJianY::Refresh(Player &player)
{
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * player.GetHastePercent());
    m_effectNum = s_maxEffectNum;
}

void YunZhongJianY::InitBaseParams()
{
    m_id = BUF_YUN_ZHONG_JIAN_Y;
    m_name = "云中剑_吞日月";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}

void YunZhongJianY::InitDamageParams()
{
    m_damageParam.fixedDamage = (35 + 55) / 2;
    m_damageParam.weaponDamageBinPercent = 0;
    m_damageParam.attackDamagePercent = 0.48125;
}

Stats::TableResult YunZhongJianY::GetRollResult(Player &player, Target &target)
{
    return Roll(player.GetPhysicsHitValuePercent(),
                player.GetPhysicsCriticalStrikePercent(),
                player.GetStrainPercent(),
                target.GetMissPercent(),
                target.GetSightPercent());
}

Stats::DamageStats YunZhongJianY::GetDamage(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) *
                        (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParam.fixedDamage * percentAdd);
    int attackDamage = static_cast<int>(m_damageParam.attackDamagePercent *
                                        player.GetPhysicsAttack() * percentAdd);
    int num = tableResult;
    return Stats::DamageStats(static_cast<int>((fixedDamage >> (num / 2)) *
                                               (1 + (num & 1) *
                                                (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              0,
                              static_cast<int>((attackDamage >> (num / 2)) *
                                               (1 + (num & 1) *
                                                (player.GetPhysicsCriticalStrikePowerPercent() - 1))));
}

void YunZhongJianY::RecordStats(Player &player,
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

void YunZhongJianY::SubEffect(Player &player, TargetList &targetList, Stats::ThreadStats threadStats, Stats::SIM_MODE simMode, Stats::TableResult tableResult)
{
    if (player.GetGearSetAttack()  && Stats::GetRand(0, 1) < 0.1) {
        player.m_buffMap[BUF_CLASS_ATTACK]->Refresh(player);
    }
}

