#include "RenJianHeYi.h"

#include "Core/Stats.h"
#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"
#include "Class/TaiXuJianYi/Buff/SuiXingChenStd.h"
#include "Class/TaiXuJianYi/Buff/SuiXingChenStd2.h"
#include "Class/TaiXuJianYi/Buff/ShengTaiJiStd.h"
#include "Class/TaiXuJianYi/Buff/TunRiYueStd.h"

int RenJianHeYi::s_cooldown = 12 * 16;

RenJianHeYi::RenJianHeYi()
{
    InitBaseParams();
    InitDamageParams();
}

RenJianHeYi::~RenJianHeYi()
{

}

void RenJianHeYi::Cast(Player &player,
                          TargetList &targetList,
                          Stats::ThreadStats &threadStats,
                          Stats::SIM_MODE &simMode)
{
    Stats::TableResult tableResult;
    tableResult = GetRollResult(player, *targetList.front());
    RecordStats(player, *targetList.front(), threadStats, simMode, tableResult);
    SubEffect(player, targetList, threadStats, simMode, tableResult);
    m_cooldown = static_cast<int>((s_cooldown - m_skillCooldownAdd) * player.GetHastePercent());
}

void RenJianHeYi::InitBaseParams()
{
    m_id = SKI_REN_JIAN_HE_YI;
    m_name = "人剑合一";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_cooldown = 0;
    m_prepareFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_energyNum = 0;
    m_publicCooldown = true;
    m_skillCooldownAdd = 0;
    m_skillCriticalStrikePercentAdd = 0;
    m_skillCriticalStrikePowerPercentAdd = 0;
    m_skillDamageBinPercentAdd = 0;
    m_skillHitValuePercentAdd = 0;
    m_skillQidianAdd = 0;
}

void RenJianHeYi::InitDamageParams()
{
    m_damageParam = Stats::DamageParam(63, 0, 0.1);
}

Stats::TableResult RenJianHeYi::GetRollResult(Player &player, Target &target)
{
    Stats::TableResult tableResult;
    tableResult = Roll(player.GetPhysicsHitValuePercent(),
                       player.GetPhysicsCriticalStrikePercent(),
                       player.GetStrainPercent(),
                       target.GetMissPercent(),
                       target.GetSightPercent());
    return tableResult;
}

Stats::DamageStats RenJianHeYi::GetDamage(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1024 + m_skillDamageBinPercentAdd) / 1024.0*
                        (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) *
                        (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParam.fixedDamage * percentAdd);
    int attackDamage = static_cast<int>(m_damageParam.attackDamagePercent * player.GetPhysicsAttack() * percentAdd);
    int num = tableResult;
    return Stats::DamageStats(static_cast<int>((fixedDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              0,
                              static_cast<int>((attackDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))));
}

void RenJianHeYi::RecordStats(Player &player,
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

void RenJianHeYi::SubEffect(Player &player,
                               TargetList &targetList,
                               Stats::ThreadStats &threadStats,
                               Stats::SIM_MODE &simMode,
                               Stats::TableResult tableResult)
{
    if (player.m_buffMap[BUF_SUI_XING_CHEN_STD]->IsExist()) {
        static_cast<SuiXingChenStd *>(player.m_buffMap[BUF_SUI_XING_CHEN_STD])->Clean(player, *targetList.front(), threadStats, simMode);
        static_cast<SuiXingChenStd2 *>(player.m_buffMap[BUF_SUI_XING_CHEN_STD2])->Clean(player, *targetList.front(), threadStats, simMode);
        player.m_buffMap[BUF_XUAN_MEN]->Refresh(player);
    }
    if (player.m_buffMap[BUF_SHENG_TAI_JI_STD]->IsExist()) {
        static_cast<ShengTaiJiStd *>(player.m_buffMap[BUF_SHENG_TAI_JI_STD])->Clean(player, *targetList.front(), threadStats, simMode);
        player.m_buffMap[BUF_XUAN_MEN]->Refresh(player);
    }
    if (player.m_buffMap[BUF_TUN_RI_YUE_STD]->IsExist()) {
        static_cast<TunRiYueStd *>(player.m_buffMap[BUF_TUN_RI_YUE_STD])->Clean(player, *targetList.front(), threadStats, simMode);
        player.m_buffMap[BUF_XUAN_MEN]->Refresh(player);
    }
    if (static_cast<TaiXuJianYi *>(&player)->m_talentShenMai && tableResult == Stats::TableResult::DOUBLE) {
        static_cast<TaiXuJianYi *>(&player)->UpdateQidian(2);
    }
    for (auto it : targetList) {
        if (it != targetList.front()) {
            Stats::TableResult tableResultOther = GetRollResult(player, *it);
            RecordStats(player, *it, threadStats, simMode, tableResultOther);
            if (static_cast<TaiXuJianYi *>(&player)->m_talentShenMai && tableResultOther == Stats::TableResult::DOUBLE) {
                static_cast<TaiXuJianYi *>(&player)->UpdateQidian(2);
            }
        }
    }
    if (player.GetGearSetAttack() && Stats::GetRand(0, 1) < 0.1) {
        static_cast<Buff *>(player.m_buffMap[BUF_CLASS_ATTACK])->Refresh(player);
    }
}
