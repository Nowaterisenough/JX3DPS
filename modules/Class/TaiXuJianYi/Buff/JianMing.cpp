#include "JianMing.h"

#include "Core/Player.h"

int JianMing::s_lastFrames = 6 * 16;

JianMing::JianMing()
{
    InitBaseParams();
}

JianMing::~JianMing()
{

}

void JianMing::Cast(Player &player,
                    TargetList &targetList,
                    Stats::ThreadStats &threadStats,
                    Stats::SIM_MODE &simMode)
{
    player.AddPhysicsAttackBaseBinPercent(-102);
    m_lastFrames = -1;
    m_effectNum = 0;
}

void JianMing::Refresh(Player &player)
{
    player.AddPhysicsAttackBaseBinPercent(102);
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_effectNum = 1;
}

void JianMing::InitBaseParams()
{
    m_id = BUF_CLASS_ATTACK;
    m_name = "剑鸣";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}
