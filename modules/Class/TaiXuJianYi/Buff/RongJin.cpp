#include "RongJin.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int RongJin::s_intervalFrames = 16;

RongJin::RongJin()
{
    InitBaseParams();
}

RongJin::~RongJin()
{

}

void RongJin::Cast(Player &player, TargetList &targetList, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode)
{
    static_cast<TaiXuJianYi *>(&player)->UpdateQidian(1);
    m_intervalFrames = s_intervalFrames;
}

void RongJin::Refresh(Player &player)
{

}

void RongJin::InitBaseParams()
{
    m_id = BUF_CLASS_EFFECT;
    m_name = "融金";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = static_cast<int>(Stats::GetRand(0, 1) * s_intervalFrames);
    m_effectNum = 0;
    m_stackNum = 0;
}
