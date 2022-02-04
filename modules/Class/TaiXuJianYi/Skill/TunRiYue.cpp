/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:08:10
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 13:26:30
 * @FilePath    : \JX3DPS\modules\Class\TaiXuJianYi\Skill\TunRiYue.cpp
 */

#include "TunRiYue.h"

#include "Core/Stats.h"
#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int TunRiYue::s_cooldown      = 24;
int TunRiYue::s_prepareFrames = 16;

TunRiYue::TunRiYue()
{
    InitBaseParams();
}

TunRiYue::~TunRiYue() {}

void TunRiYue::Cast(Player             &player,
                    TargetList         &targetList,
                    Stats::ThreadStats &threadStats,
                    Stats::SIM_MODE    &simMode)
{
    if (m_prepareFrames == 0) {
        m_prepareFrames = -1;
    } else {
        Stats::TableResult tableResult;
        RecordStats(player, *targetList.front(), threadStats, simMode, tableResult);
        SubEffect(player, targetList, threadStats, simMode, tableResult);
        m_cooldown = static_cast<int>((s_cooldown - m_skillCooldownAdd) * player.GetHastePercent());
        m_prepareFrames = static_cast<int>(s_prepareFrames * player.GetHastePercent());
    }
}

void TunRiYue::UpdateSkillQidian(int num)
{
    m_skillQidianAdd += num;
}

void TunRiYue::InitBaseParams()
{
    m_id   = SKI_TUN_RI_YUE;
    m_name = "吞日月";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
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
}

void TunRiYue::RecordStats(Player             &player,
                           Target             &target,
                           Stats::ThreadStats &threadStats,
                           Stats::SIM_MODE    &simMode,
                           Stats::TableResult  tableResult)
{
    if (simMode == Stats::SIM_MODE::DEFAULT) {
        threadStats.threadDamageStats[m_id]
            .second[target.GetId()][m_subNameVec[0]][0]
            .second[tableResult]
            .first++;
    }
}

void TunRiYue::SubEffect(Player             &player,
                         TargetList         &targetList,
                         Stats::ThreadStats &threadStats,
                         Stats::SIM_MODE    &simMode,
                         Stats::TableResult  tableResult)
{
    static_cast<TaiXuJianYi *>(&player)->UpdateQidian(m_skillQidianAdd);
    if (static_cast<TaiXuJianYi *>(&player)->m_talentChangSheng) {
        player.m_buffMap[BUF_CHI_YING]->Refresh(player);
    }
}
