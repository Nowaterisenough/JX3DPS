#include "BaHuangGuiYuan.h"

#include "Core/Stats.h"
#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"
#include "Class/TaiXuJianYi/Buff/DieRen.h"

namespace JX3DPS {

namespace TaiXuJianYi {


int BaHuangGuiYuan::s_cooldown = 12 * 16;

BaHuangGuiYuan::BaHuangGuiYuan()
{

}

BaHuangGuiYuan::BaHuangGuiYuan(Player *player) :
    Skill(player)
{
    InitBaseParams();
    InitDamageParams();
}

BaHuangGuiYuan::~BaHuangGuiYuan()
{

}

void BaHuangGuiYuan::Cast(Player &player,
                          TargetList &targetList,
                          Stats::ThreadStats &threadStats,
                          Stats::SIM_MODE &simMode)
{
    Stats::TableResult tableResult = GetRollResult(player, *targetList.front());
    RecordStats(player, *targetList.front(), threadStats, simMode, tableResult);
    SubEffect(player, targetList, threadStats, simMode, tableResult);
    m_cooldown = static_cast<int>((s_cooldown - m_skillCooldownAdd) * player.GetHastePercent());
}

void BaHuangGuiYuan::UpdateCooldown(int num)
{
    m_cooldown -= num;
    m_cooldown = MAX(0, m_cooldown);
}

void BaHuangGuiYuan::UpdateSkillQidian(int num)
{
    m_skillQidianAdd += num;
}

void BaHuangGuiYuan::InitBaseParams()
{
    m_id = SKI_BA_HUANG_GUI_YUAN;
    m_name = "八荒归元";
    m_subNameVec.push_back("1段");
    m_subNameVec.push_back("2段");
    m_subNameVec.push_back("和光");
    m_levelNameVec.push_back("100%-90%");
    m_levelNameVec.push_back("90%-80%");
    m_levelNameVec.push_back("80%-70%");
    m_levelNameVec.push_back("70%-60%");
    m_levelNameVec.push_back("60%-50%");
    m_levelNameVec.push_back("50%-40%");
    m_levelNameVec.push_back("40%-30%");
    m_levelNameVec.push_back("30%-20%");
    m_levelNameVec.push_back("20%-10%");
    m_levelNameVec.push_back("10%-0%");
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

void BaHuangGuiYuan::InitDamageParams()
{
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParamVecMap[m_subNameVec[0]].push_back(Stats::DamageParam((2115 + 2125) / 2, 0, 0.1));

    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((23 + 25) / 2, 2048, 1.1375));
    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((97 + 107) / 2, 2048, 1.2688));
    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((171 + 188) / 2, 2048, 1.4003));
    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((245 + 270) / 2, 2048, 1.5317));
    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((319 + 352) / 2, 2048, 1.6631));
    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((394 + 433) / 2, 2048, 1.7944));
    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((468 + 515) / 2, 2048, 1.9258));
    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((542 + 597) / 2, 2048, 2.0572));
    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((616 + 678) / 2, 2048, 2.1886));
    m_damageParamVecMap[m_subNameVec[1]].push_back(Stats::DamageParam((690 + 760) / 2, 2048, 2.3200));

    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((23 + 25) / 8, 512, 1.1375 / 4));
    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((97 + 107) / 8, 512, 1.2688 / 4));
    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((171 + 188) / 8, 512, 1.4003 / 4));
    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((245 + 270) / 8, 512, 1.5317 / 4));
    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((319 + 352) / 8, 512, 1.6631 / 4));
    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((394 + 433) / 8, 512, 1.7944 / 4));
    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((468 + 515) / 8, 512, 1.9258 / 4));
    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((542 + 597) / 8, 512, 2.0572 / 4));
    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((616 + 678) / 8, 512, 2.1886 / 4));
    m_damageParamVecMap[m_subNameVec[2]].push_back(Stats::DamageParam((690 + 760) / 8, 512, 2.3200 / 4));
}

Stats::TableResult BaHuangGuiYuan::GetRollResult(Player &player, Target &target)
{
    Stats::TableResult tableResult;
    tableResult = Roll(player.GetPhysicsHitValuePercent() + m_skillHitValuePercentAdd,
                       player.GetPhysicsCriticalStrikePercent(),
                       player.GetStrainPercent(),
                       target.GetMissPercent(),
                       target.GetSightPercent());
    return tableResult;
}

Stats::DamageStats BaHuangGuiYuan::GetDamage(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1024 + m_skillDamageBinPercentAdd) / 1024.0 *
                        (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) *
                        (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParamVecMap[m_subNameVec[0]]
                                       [static_cast<unsigned>(10 - target.GetLifeDecPercent())].
                                       fixedDamage * percentAdd);
    int weaponDamage = static_cast<int>((m_damageParamVecMap[m_subNameVec[0]]
                                        [static_cast<unsigned>(10 - target.GetLifeDecPercent())].
                                        weaponDamageBinPercent >> 10) * player.GetWeaponAttack() * percentAdd);
    int attackDamage = static_cast<int>(m_damageParamVecMap[m_subNameVec[0]]
                                        [static_cast<unsigned>(10 - target.GetLifeDecPercent())].
                                        attackDamagePercent * player.GetPhysicsAttack() * percentAdd);
    int num = tableResult;
    return Stats::DamageStats(static_cast<int>((fixedDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              static_cast<int>((weaponDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              static_cast<int>((attackDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))));
}

void BaHuangGuiYuan::RecordStats(Player &player,
                                 Target &target,
                                 Stats::ThreadStats &threadStats,
                                 Stats::SIM_MODE &simMode,
                                 Stats::TableResult tableResult)
{
    Stats::DamageStats damageStats;
    switch (static_cast<int>(simMode)) {
    case Stats::SIM_MODE::DEFAULT :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[0]][10 - target.GetLifeDecPercent()].
                second[tableResult].first++;
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[0]][10 - target.GetLifeDecPercent()].
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

void BaHuangGuiYuan::SubEffect(Player &player,
                               TargetList &targetList,
                               Stats::ThreadStats &threadStats,
                               Stats::SIM_MODE &simMode,
                               Stats::TableResult tableResult)
{
    if (tableResult != Stats::TableResult::MISS) {
        Stats::TableResult tableResult2 = GetRollResult2(player, *targetList.front());
        RecordStats2(player, *targetList.front(), threadStats, simMode, tableResult2);
        if (static_cast<Buff *>(targetList.front()->m_buffMap[TBUF_DIE_REN])->IsExist()) {
            if (static_cast<TaiXuJianYi *>(&player)->m_talentHeGuang) {
                Stats::TableResult tableResultHeGuang = GetRollResultHeGuang(player, *targetList.front());
                RecordStatsHeGuang(player, *targetList.front(), threadStats, simMode, tableResultHeGuang);
            }
            if (targetList.front()->GetLifeDecPercent() < 5 && static_cast<TaiXuJianYi *>(&player)->m_talentQieYu) {
                static_cast<DieRen *>(targetList.front()->m_buffMap[TBUF_DIE_REN])->
                        Clean(player, *targetList.front(), threadStats, simMode);
            }
        }
        static_cast<TaiXuJianYi *>(&player)->UpdateQidian(m_skillQidianAdd);
        if (static_cast<TaiXuJianYi *>(&player)->m_talentShenMai && tableResult == Stats::TableResult::DOUBLE) {
            static_cast<TaiXuJianYi *>(&player)->UpdateQidian(2);
        }
    }
    if (static_cast<Buff *>(player.m_buffMap[BUF_CLASS_CW])->IsExist()) {
        m_cooldown = 0;
    }
    if (static_cast<TaiXuJianYi *>(&player)->m_talentFengShi) {
        static_cast<Buff *>(player.m_buffMap[BUF_FENG_SHI])->Refresh(player);
    }
    if (player.GetGearSetAttack() && Stats::GetRand(0, 1) < 0.1) {
        static_cast<Buff *>(player.m_buffMap[BUF_CLASS_ATTACK])->Refresh(player);
    }
}

Stats::TableResult BaHuangGuiYuan::GetRollResult2(Player &player, Target &target)
{
    Stats::TableResult tableResult;
    tableResult = Roll(player.GetPhysicsHitValuePercent() + m_skillHitValuePercentAdd,
                       player.GetPhysicsCriticalStrikePercent(),
                       player.GetStrainPercent(),
                       target.GetMissPercent(),
                       target.GetSightPercent(),
                       Stats::RollType::SUB);
    return tableResult;
}

Stats::DamageStats BaHuangGuiYuan::GetDamage2(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1024 + m_skillDamageBinPercentAdd) / 1024.0 *
                        (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) *
                        (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParamVecMap[m_subNameVec[1]]
                                       [static_cast<unsigned>(10 - target.GetLifeDecPercent())].
                                       fixedDamage * percentAdd);
    int weaponDamage = static_cast<int>((m_damageParamVecMap[m_subNameVec[1]]
                                        [static_cast<unsigned>(10 - target.GetLifeDecPercent())].
                                        weaponDamageBinPercent >> 10) * player.GetWeaponAttack() * percentAdd);
    int attackDamage = static_cast<int>(m_damageParamVecMap[m_subNameVec[1]]
                                        [static_cast<unsigned>(10 - target.GetLifeDecPercent())].
                                        attackDamagePercent * player.GetPhysicsAttack() * percentAdd);
    int num = tableResult;
    return Stats::DamageStats(static_cast<int>((fixedDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              static_cast<int>((weaponDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              static_cast<int>((attackDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))));
}

void BaHuangGuiYuan::RecordStats2(Player &player,
                                  Target &target,
                                  Stats::ThreadStats &threadStats,
                                  Stats::SIM_MODE &simMode,
                                  Stats::TableResult tableResult)
{
    Stats::DamageStats damageStats;
    switch (static_cast<int>(simMode)) {
    case Stats::SIM_MODE::DEFAULT :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[1]][10 - target.GetLifeDecPercent()].
                second[tableResult].first++;
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[1]][10 - target.GetLifeDecPercent()].
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

Stats::TableResult BaHuangGuiYuan::GetRollResultHeGuang(Player &player, Target &target)
{
    Stats::TableResult tableResult;
    tableResult = Roll(player.GetPhysicsHitValuePercent() + m_skillHitValuePercentAdd,
                       player.GetPhysicsCriticalStrikePercent(),
                       player.GetStrainPercent(),
                       target.GetMissPercent(),
                       target.GetSightPercent());
    return tableResult;
}

Stats::DamageStats BaHuangGuiYuan::GetDamageHeGuang(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1024 + m_skillDamageBinPercentAdd) / 1024.0 *
                        (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) *
                        (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParamVecMap[m_subNameVec[2]]
                                       [static_cast<unsigned>(10 - target.GetLifeDecPercent())].
                                       fixedDamage * percentAdd);
    int weaponDamage = static_cast<int>((m_damageParamVecMap[m_subNameVec[2]]
                                        [static_cast<unsigned>(10 - target.GetLifeDecPercent())].
                                        weaponDamageBinPercent >> 10) * player.GetWeaponAttack() * percentAdd);
    int attackDamage = static_cast<int>(m_damageParamVecMap[m_subNameVec[2]]
                                        [static_cast<unsigned>(10 - target.GetLifeDecPercent())].
                                        attackDamagePercent * player.GetPhysicsAttack() * percentAdd);
    int num = tableResult;
    return Stats::DamageStats(static_cast<int>((fixedDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              static_cast<int>((weaponDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
                              static_cast<int>((attackDamage >> (num / 2)) *
                                               (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))));
}

void BaHuangGuiYuan::RecordStatsHeGuang(Player &player,
                                        Target &target,
                                        Stats::ThreadStats &threadStats,
                                        Stats::SIM_MODE &simMode,
                                        Stats::TableResult tableResult)
{
    Stats::DamageStats damageStats;
    switch (static_cast<int>(simMode)) {
    case Stats::SIM_MODE::DEFAULT :
        damageStats = GetDamage(player, target, tableResult);
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[2]][10 - target.GetLifeDecPercent()].
                second[tableResult].first++;
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[2]][10 - target.GetLifeDecPercent()].
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

}

}
