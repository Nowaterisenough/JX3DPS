#include "QiSheng.h"

#include "Core/Player.h"

int QiSheng::s_lastFrames = 3 * 16;

QiSheng::QiSheng()
{
    InitBaseParams();
}

QiSheng::~QiSheng()
{

}

void QiSheng::Cast(Player &player,
                   TargetList &targetList,
                   Stats::ThreadStats &threadStats,
                   Stats::SIM_MODE &simMode)
{
    player.AddPhysicsAttackBaseBinPercent(-102);
    m_lastFrames = -1;
    m_effectNum = 0;
}

void QiSheng::Refresh(Player &player)
{
    player.AddPhysicsAttackBaseBinPercent(102);
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_effectNum = 1;
}

void QiSheng::InitBaseParams()
{
    m_id = BUF_QI_SHENG;
    m_name = "期声";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}
