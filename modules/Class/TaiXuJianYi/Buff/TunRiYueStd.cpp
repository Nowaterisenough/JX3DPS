#include "TunRiYueStd.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int TunRiYueStd::s_lastFrames = 36 * 16;

TunRiYueStd::TunRiYueStd()
{
    InitBaseParams();
}

TunRiYueStd::~TunRiYueStd()
{

}

void TunRiYueStd::Cast(Player &player,
                          TargetList &targetList,
                          Stats::ThreadStats &threadStats,
                          Stats::SIM_MODE &simMode)
{
    m_lastFrames = -1;
    m_effectNum = 0;
}

void TunRiYueStd::Refresh(Player &player)
{
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_effectNum = 1;
}

void TunRiYueStd::Clean(Player &player, Target &target, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode)
{
    m_lastFrames = -1;
    m_effectNum = 0;
}

void TunRiYueStd::InitBaseParams()
{
    m_id = BUF_TUN_RI_YUE_STD;
    m_name = "气场吞日月";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}
