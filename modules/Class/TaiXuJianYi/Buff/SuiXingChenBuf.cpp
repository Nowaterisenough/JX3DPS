#include "SuiXingChenBuf.h"

#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int SuiXingChenBuf::s_lastFrames = 4 * 16;

SuiXingChenBuf::SuiXingChenBuf()
{
    InitBaseParams();
}

SuiXingChenBuf::~SuiXingChenBuf()
{

}

void SuiXingChenBuf::Cast(Player &player, TargetList &targetList, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode)
{
    m_lastFrames = -1;
    m_effectNum = 0;
    if (static_cast<TaiXuJianYi *>(&player)->m_talentWuYu) {
        player.AddPhysicsCriticalStrikePercent(-0.1);
        player.AddPhysicsCriticalStrikePowerPercent(-0.2);
    } else {
        player.AddPhysicsCriticalStrikePercent(-0.05);
        player.AddPhysicsCriticalStrikePowerPercent(-0.1);
    }
}

void SuiXingChenBuf::Refresh(Player &player)
{
    if (static_cast<TaiXuJianYi *>(&player)->m_talentWuYu) {
        player.AddPhysicsCriticalStrikePercent(0.1 * (1 ^ m_effectNum));
        player.AddPhysicsCriticalStrikePowerPercent(0.2 * (1 ^ m_effectNum));
    } else {
        player.AddPhysicsCriticalStrikePercent(0.05 * (1 ^ m_effectNum));
        player.AddPhysicsCriticalStrikePowerPercent(0.1 * (1 ^ m_effectNum));
    }
    m_lastFrames = s_lastFrames;
    m_effectNum = 1;
}

void SuiXingChenBuf::InitBaseParams()
{
    m_id = BUF_SUI_XING_CHEN;
    m_name = "碎星辰buff";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
}
