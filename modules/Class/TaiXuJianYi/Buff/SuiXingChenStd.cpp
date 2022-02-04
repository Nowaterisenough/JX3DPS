#include "SuiXingChenStd.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int SuiXingChenStd::s_lastFrames = 36 * 16;
int SuiXingChenStd::s_intervalFrames = 3 * 16;

SuiXingChenStd::SuiXingChenStd()
{
    InitBaseParams();
}

SuiXingChenStd::~SuiXingChenStd()
{

}

void SuiXingChenStd::Cast(Player &player,
                          TargetList &targetList,
                          Stats::ThreadStats &threadStats,
                          Stats::SIM_MODE &simMode)
{
    if (m_lastFrames == 0) {
        m_lastFrames = -1;
        m_intervalFrames = -1;
        m_effectNum = 0;
    } else if (!player.m_buffMap[BUF_QI_SHENG]->IsExist() &&
               static_cast<TaiXuJianYi *>(&player)->m_talentQiSheng) {
        player.m_buffMap[BUF_QI_SHENG]->Refresh(player);
        m_intervalFrames = static_cast<int>(s_intervalFrames * player.GetHastePercent());
    }
}

void SuiXingChenStd::Refresh(Player &player)
{
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * player.GetHastePercent());
    m_effectNum = 1;
}

void SuiXingChenStd::Clean(Player &player, Target &target, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode)
{
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
}

void SuiXingChenStd::InitBaseParams()
{
    m_id = BUF_SUI_XING_CHEN_STD;
    m_name = "气场碎星辰";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}

