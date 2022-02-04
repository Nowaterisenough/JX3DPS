#include "ZiQiDongLai.h"

#include "Core/Stats.h"
#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int ZiQiDongLai::s_cooldown = 100 * 16;

ZiQiDongLai::ZiQiDongLai()
{
    InitBaseParams();
}

ZiQiDongLai::~ZiQiDongLai()
{

}

void ZiQiDongLai::Cast(Player &player, TargetList &targetList, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode)
{
    RecordStats(player, *targetList.front(), threadStats, simMode, Stats::TableResult::ALL);
    SubEffect(player, targetList, threadStats, simMode, Stats::TableResult::ALL);
    m_cooldown = s_cooldown;
}

void ZiQiDongLai::InitBaseParams()
{
    m_id = SKI_ZI_QI_DONG_LAI;
    m_name = "紫气东来";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_cooldown = 0;
    m_prepareFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_energyNum = 0;
    m_publicCooldown = false;
    m_skillCooldownAdd = 0;
    m_skillCriticalStrikePercentAdd = 0;
    m_skillCriticalStrikePowerPercentAdd = 0;
    m_skillDamageBinPercentAdd = 0;
    m_skillHitValuePercentAdd = 0;
}

void ZiQiDongLai::RecordStats(Player &player, Target &target, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode, Stats::TableResult tableResult)
{
    if (simMode == Stats::SIM_MODE::DEFAULT) {
        threadStats.threadDamageStats[m_id].second[target.GetId()][m_subNameVec[0]][0].
                second[tableResult].first++;
    }
}

void ZiQiDongLai::SubEffect(Player &player, TargetList &targetList, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode, Stats::TableResult tableResult)
{
    static_cast<TaiXuJianYi *>(&player)->UpdateQidian(10);
    player.m_buffMap[BUF_ZI_QI_DONG_LAI]->Refresh(player);
}
