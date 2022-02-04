#include "SuiXingChenStd2.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int SuiXingChenStd2::s_lastFrames = 36 * 16;
int SuiXingChenStd2::s_intervalFrames = 3 * 16;

SuiXingChenStd2::SuiXingChenStd2()
{
    InitBaseParams();
}

SuiXingChenStd2::~SuiXingChenStd2()
{

}

void SuiXingChenStd2::Cast(Player &player,
                          TargetList &targetList,
                          Stats::ThreadStats &threadStats,
                          Stats::SIM_MODE &simMode)
{
    if (m_lastFrames == 0) {
        m_lastFrames = -1;
        m_intervalFrames = -1;
        m_effectNum = 0;
    } else if (!player.m_buffMap[BUF_SUI_XING_CHEN]->IsExist()) {
        player.m_buffMap[BUF_SUI_XING_CHEN]->Refresh(player);
        m_intervalFrames = static_cast<int>(s_intervalFrames * player.GetHastePercent());
    }
}

void SuiXingChenStd2::Refresh(Player &player)
{
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * player.GetHastePercent());
    m_effectNum = 1;
}

void SuiXingChenStd2::Clean(Player &player, Target &target, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode)
{
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
}

void SuiXingChenStd2::InitBaseParams()
{
    m_id = BUF_SUI_XING_CHEN_STD2;
    m_name = "气场碎星辰2";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}
