#include "XuanMen.h"

#include "Core/Player.h"

int XuanMen::s_lastFrames = 36 * 16;
int XuanMen::s_maxStackNum = 3;

XuanMen::XuanMen()
{

}

XuanMen::~XuanMen()
{

}

void XuanMen::Cast(Player &player,
                          TargetList &targetList,
                          Stats::ThreadStats &threadStats,
                          Stats::SIM_MODE &simMode)
{
    player.AddPhysicsCriticalStrikePercent(-m_stackNum * 0.03);
    player.AddPhysicsOvercomeBaseBinPercent(-m_stackNum * 204);
    m_lastFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}

void XuanMen::Refresh(Player &player)
{
    player.AddPhysicsCriticalStrikePercent(-m_stackNum * 0.03);
    player.AddPhysicsOvercomeBaseBinPercent(-m_stackNum * 204);
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_effectNum = 1;
    m_stackNum++;
    m_stackNum = MIN(m_stackNum, s_maxStackNum);
    player.AddPhysicsCriticalStrikePercent(m_stackNum * 0.03);
    player.AddPhysicsOvercomeBaseBinPercent(-m_stackNum * 204);
}

void XuanMen::InitBaseParams()
{
    m_id = BUF_XUAN_MEN;
    m_name = "玄门";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}
