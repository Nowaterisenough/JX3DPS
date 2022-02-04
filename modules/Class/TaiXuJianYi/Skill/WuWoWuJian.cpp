/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:08:10
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 15:53:38
 * @FilePath    : \JX3DPS\modules\Class\TaiXuJianYi\Skill\WuWoWuJian.cpp
 */

#include "WuWoWuJian.h"

#include "Core/Stats.h"
#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"
#include "Class/TaiXuJianYi/Buff/DieRen.h"
#include "Class/TaiXuJianYi/Buff/FengShi.h"
#include "Class/TaiXuJianYi/Skill/BaHuangGuiYuan.h"

WuWoWuJian::WuWoWuJian()
{
    InitBaseParams();
    InitDamageParams();
}

WuWoWuJian::~WuWoWuJian() {}

void WuWoWuJian::Cast(Player             &player,
                      TargetList         &targetList,
                      Stats::ThreadStats &threadStats,
                      Stats::SIM_MODE    &simMode)
{
    m_qidian                       = static_cast<TaiXuJianYi *>(&player)->GetQidian();
    Stats::TableResult tableResult = GetRollResult(player, *targetList.front());
    RecordStats(player, *targetList.front(), threadStats, simMode, tableResult);
    SubEffect(player, targetList, threadStats, simMode, tableResult);
}

void WuWoWuJian::UpdateSkillQidian(int num)
{
    m_skillQidianAdd += num;
}

void WuWoWuJian::InitBaseParams()
{
    m_id   = SKI_WU_WO_WU_JIAN;
    m_name = "无我无剑";

    m_subNameVec.resize(2);
    m_subNameVec[0] = "";
    m_subNameVec[1] = "白虹";

    m_levelNameVec.resize(10);
    m_levelNameVec[0] = "1气点";
    m_levelNameVec[1] = "2气点";
    m_levelNameVec[2] = "3气点";
    m_levelNameVec[3] = "4气点";
    m_levelNameVec[4] = "5气点";
    m_levelNameVec[5] = "6气点";
    m_levelNameVec[6] = "7气点";
    m_levelNameVec[7] = "8气点";
    m_levelNameVec[8] = "9气点";
    m_levelNameVec[9] = "10气点";

    for (const auto )
    TargetStats tagetStats;
    for (const auto &target : targets) {
        SubStats subStats;
        for (const auto &subName : m_subNameVec) {
            LevelStats levelStats;
            for (size_t i = 0; i < m_levelNameVec.size(); ++i) {
                TableStats tableResult{
                    {HIT, std::pair<int, DamageStats>(0, DamageStats(0, 0, 0))},
                    {DOUBLE, std::pair<int, DamageStats>(0, DamageStats(0, 0, 0))},
                    {MISS, std::pair<int, DamageStats>(0, DamageStats(0, 0, 0))},
                };
                levelStats.emplace_back(i, tableResult);
            }
            subStats.emplace_back(subName, levelStats);
        }
        tagetStats.emplace_back(targets.GetId(), subStats);
    }

    m_cooldown                           = 0;
    m_prepareFrames                      = -1;
    m_intervalFrames                     = -1;
    m_effectNum                          = 0;
    m_energyNum                          = 0;
    m_publicCooldown                     = true;
    m_skillCooldownAdd                   = 0;
    m_skillCriticalStrikePercentAdd      = 0;
    m_skillCriticalStrikePowerPercentAdd = 0;
    m_skillDamageBinPercentAdd           = 0;
    m_skillHitValuePercentAdd            = 0;
    m_skillQidianAdd                     = 0;
}

void WuWoWuJian::InitDamageParams()
{
    m_damageParamVec.resize(10);
    m_damageParamVec[0] = Stats::DamageParam((22 + 24) / 2, 2048, 691);
    m_damageParamVec[1] = Stats::DamageParam((44 + 49) / 2, 2048, 1405);
    m_damageParamVec[2] = Stats::DamageParam((67 + 74) / 2, 2048, 2119);
    m_damageParamVec[3] = Stats::DamageParam((89 + 99) / 2, 2048, 2833);
    m_damageParamVec[4] = Stats::DamageParam((112 + 124) / 2, 2048, 3547);
    m_damageParamVec[5] = Stats::DamageParam((134 + 149) / 2, 2048, 4261);
    m_damageParamVec[6] = Stats::DamageParam((157 + 174) / 2, 2048, 4975);
    m_damageParamVec[7] = Stats::DamageParam((179 + 198) / 2, 2048, 5689);
    m_damageParamVec[8] = Stats::DamageParam((202 + 223) / 2, 2048, 6403);
    m_damageParamVec[9] = Stats::DamageParam((224 + 248) / 2, 2048, 7117);
}

Stats::TableResult WuWoWuJian::GetRollResult(Player &player, Target &target)
{
    Stats::TableResult tableResult;
    int                temp = m_qidian > 7 && static_cast<TaiXuJianYi *>(&player)->m_talentWuYi;
    tableResult =
        Roll(player.GetPhysicsHitValuePercent() + m_skillHitValuePercentAdd,
             player.GetPhysicsCriticalStrikePercent() + m_skillCriticalStrikePercentAdd + 0.1 * temp,
             player.GetStrainPercent(),
             target.GetMissPercent(),
             target.GetSightPercent());
    return tableResult;
}

Stats::DamageStats WuWoWuJian::GetDamage(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1024 + m_skillDamageBinPercentAdd) / 1024.0 *
                        (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) * (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(
        m_damageParamVec[static_cast<unsigned>(m_qidian - 1)].fixedDamage * percentAdd);
    int weaponDamage = static_cast<int>(
        (m_damageParamVec[static_cast<unsigned>(m_qidian - 1)].weaponDamageBinPercent >> 10) *
        player.GetWeaponAttack() * percentAdd);
    int attackDamage =
        static_cast<int>(m_damageParamVec[static_cast<unsigned>(m_qidian - 1)].attackDamagePercent *
                         player.GetPhysicsAttack() * percentAdd);
    int num  = tableResult;
    int temp = m_qidian > 7 && static_cast<TaiXuJianYi *>(&player)->m_talentWuYi;
    return Stats::DamageStats(
        static_cast<int>(
            (fixedDamage >> (num / 2)) *
            (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() + 0.3 * temp - 1))),
        static_cast<int>(
            (weaponDamage >> (num / 2)) *
            (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() + 0.3 * temp - 1))),
        static_cast<int>(
            (attackDamage >> (num / 2)) *
            (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() + 0.3 * temp - 1))));
}

void WuWoWuJian::RecordStats(Player             &player,
                             Target             &target,
                             Stats::ThreadStats &threadStats,
                             Stats::SIM_MODE    &simMode,
                             Stats::TableResult  tableResult)
{
    Stats::DamageStats damageStats;
    switch (static_cast<int>(simMode)) {
    case Stats::SIM_MODE::DEFAULT:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.threadDamageStats[m_id]
            .second[target.GetId()][m_subNameVec[0]][m_qidian - 1]
            .second[tableResult]
            .first++;
        threadStats.threadDamageStats[m_id]
            .second[target.GetId()][m_subNameVec[0]][m_qidian - 1]
            .second[tableResult]
            .second += damageStats;
        threadStats.attrBonusMap[Stats::AttrType::DEFAULT].damage += damageStats.SumDamage();
        if (threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].set) {
            player.AddWeaponAttack(threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].damage +=
                damageStats.SumDamage();
            player.AddWeaponAttack(-threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].set) {
            player.AddPhysicsAttackBase(threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].damage += damageStats.SumDamage();
            player.AddPhysicsAttackBase(-threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].set) {
            player.AddPhysicsCriticalStrikePower(
                threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].damage +=
                damageStats.SumDamage();
            player.AddPhysicsCriticalStrikePower(
                -threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].set) {
            player.AddPhysicsOvercomeBase(
                threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].damage +=
                damageStats.SumDamage();
            player.AddPhysicsOvercomeBase(
                -threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].value);
        }
        break;
    case Stats::SIM_MODE::AGILITY:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::AGILITY].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::HIT_VALUE:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::HIT_VALUE].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::CRITICAL_STRIKE:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::STRAIN:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::STRAIN].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::HASTE:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::HASTE].damage += damageStats.SumDamage();
        break;
    }
}

void WuWoWuJian::SubEffect(Player             &player,
                           TargetList         &targetList,
                           Stats::ThreadStats &threadStats,
                           Stats::SIM_MODE    &simMode,
                           Stats::TableResult  tableResult)
{
    static_cast<TaiXuJianYi *>(&player)->UpdateQidian(-10);
    if (Stats::GetRand(0, 1) < 0.25 && static_cast<TaiXuJianYi *>(&player)->m_talentKunWu &&
        m_qidian == 10)
    {
        static_cast<TaiXuJianYi *>(&player)->UpdateQidian(6);
    }
    if (tableResult != Stats::TableResult::MISS) {
        if (static_cast<TaiXuJianYi *>(&player)->m_talentFengShi) {
            static_cast<FengShi *>(player.m_buffMap[BUF_FENG_SHI])
                ->Clean(player, *targetList.front(), threadStats, simMode);
        }
        static_cast<TaiXuJianYi *>(&player)->UpdateQidian(m_skillQidianAdd);
        if (player.m_buffMap[BUF_CLASS_CW]->IsExist()) {
            static_cast<TaiXuJianYi *>(&player)->UpdateQidian(10);
        }
        if (tableResult == Stats::TableResult::DOUBLE) {
            if (static_cast<TaiXuJianYi *>(&player)->m_talentShenMai) {
                static_cast<TaiXuJianYi *>(&player)->UpdateQidian(2);
            }
            if (static_cast<TaiXuJianYi *>(&player)->m_talentDieRen) {
                static_cast<DieRen *>(targetList.front()->m_buffMap[TBUF_DIE_REN])->Refresh(player);
            }
        }
        if (targetList.front()->m_buffMap[TBUF_DIE_REN]->IsExist() &&
            static_cast<TaiXuJianYi *>(&player)->m_talentWuYu)
        {
            static_cast<BaHuangGuiYuan *>(player.m_skillMap[SKI_BA_HUANG_GUI_YUAN])
                ->UpdateCooldown(8 * m_qidian);
        }
        if (static_cast<TaiXuJianYi *>(&player)->m_talentBaiHong) {
            for (auto it : targetList) {
                if (it != targetList.front()) {
                    Stats::TableResult tableResultOther = GetRollResultBaiHong(player, *it);
                    RecordStatsBaiHong(player, *it, threadStats, simMode, tableResultOther);
                    SubEffectBaiHong(player, *it, threadStats, simMode, tableResultOther);
                }
            }
        }
    } else {
        static_cast<TaiXuJianYi *>(&player)->UpdateQidian(6);
    }
    if (player.GetGearSetAttack() && Stats::GetRand(0, 1) < 0.1) {
        static_cast<Buff *>(player.m_buffMap[BUF_CLASS_ATTACK])->Refresh(player);
    }
}

Stats::TableResult WuWoWuJian::GetRollResultBaiHong(Player &player, Target &target)
{
    Stats::TableResult tableResult = Roll(player.GetPhysicsHitValuePercent(),
                                          player.GetPhysicsCriticalStrikePercent(),
                                          player.GetStrainPercent(),
                                          target.GetMissPercent(),
                                          target.GetSightPercent());
    return tableResult;
}

Stats::DamageStats
WuWoWuJian::GetDamageBaiHong(Player &player, Target &target, Stats::TableResult tableResult)
{
    double percentAdd = (1 + player.GetPhysicsOvercomePercent()) *
                        (1 + target.GetDamagePercentAdd()) * (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(
        m_damageParamVec[static_cast<unsigned>(m_qidian - 1)].fixedDamage * percentAdd);
    int weaponDamage = static_cast<int>(
        (m_damageParamVec[static_cast<unsigned>(m_qidian - 1)].weaponDamageBinPercent >> 10) *
        player.GetWeaponAttack() * percentAdd);
    int attackDamage =
        static_cast<int>(m_damageParamVec[static_cast<unsigned>(m_qidian - 1)].attackDamagePercent *
                         player.GetPhysicsAttack() * percentAdd);
    int num = tableResult;
    return Stats::DamageStats(
        static_cast<int>((fixedDamage >> (num / 2)) *
                         (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
        static_cast<int>((weaponDamage >> (num / 2)) *
                         (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))),
        static_cast<int>((attackDamage >> (num / 2)) *
                         (1 + (num & 1) * (player.GetPhysicsCriticalStrikePowerPercent() - 1))));
}

void WuWoWuJian::RecordStatsBaiHong(Player             &player,
                                    Target             &target,
                                    Stats::ThreadStats &threadStats,
                                    Stats::SIM_MODE    &simMode,
                                    Stats::TableResult  tableResult)
{
    Stats::DamageStats damageStats;
    switch (static_cast<int>(simMode)) {
    case Stats::SIM_MODE::DEFAULT:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.threadDamageStats[m_id]
            .second[target.GetId()][m_subNameVec[1]][m_qidian - 1]
            .second[tableResult]
            .first++;
        threadStats.threadDamageStats[m_id]
            .second[target.GetId()][m_subNameVec[1]][m_qidian - 1]
            .second[tableResult]
            .second += damageStats;
        threadStats.attrBonusMap[Stats::AttrType::DEFAULT].damage += damageStats.SumDamage();
        if (threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].set) {
            player.AddWeaponAttack(threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].damage +=
                damageStats.SumDamage();
            player.AddWeaponAttack(-threadStats.attrBonusMap[Stats::AttrType::WEAPON_ATTACK].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].set) {
            player.AddPhysicsAttackBase(threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].damage += damageStats.SumDamage();
            player.AddPhysicsAttackBase(-threadStats.attrBonusMap[Stats::AttrType::ATTACK_BASE].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].set) {
            player.AddPhysicsCriticalStrikePower(
                threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].damage +=
                damageStats.SumDamage();
            player.AddPhysicsCriticalStrikePower(
                -threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE_POWER].value);
        }
        if (threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].set) {
            player.AddPhysicsOvercomeBase(
                threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].value);
            damageStats = GetDamage(player, target, tableResult);
            threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].damage +=
                damageStats.SumDamage();
            player.AddPhysicsOvercomeBase(
                -threadStats.attrBonusMap[Stats::AttrType::OVERCOME_BASE].value);
        }
        break;
    case Stats::SIM_MODE::AGILITY:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::AGILITY].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::HIT_VALUE:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::HIT_VALUE].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::CRITICAL_STRIKE:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::CRITICAL_STRIKE].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::STRAIN:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::STRAIN].damage += damageStats.SumDamage();
        break;
    case Stats::SIM_MODE::HASTE:
        damageStats = GetDamage(player, target, tableResult);
        threadStats.attrBonusMap[Stats::AttrType::HASTE].damage += damageStats.SumDamage();
        break;
    }
}

void WuWoWuJian::SubEffectBaiHong(Player             &player,
                                  Target             &target,
                                  Stats::ThreadStats &threadStats,
                                  Stats::SIM_MODE    &simMode,
                                  Stats::TableResult  tableResult)
{
    if (static_cast<TaiXuJianYi *>(&player)->m_talentShenMai &&
        tableResult == Stats::TableResult::DOUBLE)
    {
        static_cast<TaiXuJianYi *>(&player)->UpdateQidian(2);
    }
}
