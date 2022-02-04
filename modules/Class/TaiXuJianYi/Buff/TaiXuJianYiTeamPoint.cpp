#include "TaiXuJianYiTeamPoint.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int TaiXuJianYiTeamPoint::s_cooldown = 6 * 16;

TaiXuJianYiTeamPoint::TaiXuJianYiTeamPoint()
{
    InitBaseParams();
}

TaiXuJianYiTeamPoint::~TaiXuJianYiTeamPoint()
{

}

void TaiXuJianYiTeamPoint::Cast(Player &player, TargetList &targetList, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode)
{
    if (m_cooldown == 0) {
        m_cooldown = static_cast<int>(Stats::GetRand(1, 1.2) * s_cooldown);
        static_cast<TaiXuJianYi *>(&player)->UpdateQidian(2);
    }
}

void TaiXuJianYiTeamPoint::Refresh(Player &player)
{

}

void TaiXuJianYiTeamPoint::InitBaseParams()
{
    m_id = BUF_CLASS_TEAM_POINT;
    m_name = "阵眼";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = static_cast<int>(Stats::GetRand(1, 1.2) * s_cooldown);
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}
