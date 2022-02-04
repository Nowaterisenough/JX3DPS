#include "TaiXuJianYiCW.h"

int TaiXuJianYiCW::s_lastFrames = 6 * 16;
int TaiXuJianYiCW::s_cooldown = 40 * 16;

TaiXuJianYiCW::TaiXuJianYiCW()
{
    InitBaseParams();
}

TaiXuJianYiCW::~TaiXuJianYiCW()
{

}

void TaiXuJianYiCW::Cast(Player &player, TargetList &targetList, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode)
{
    if (m_cooldown == 0) {
        m_cooldown = -1;
    }
    if (m_lastFrames == 0) {
        m_lastFrames = -1;
        m_effectNum = 0;
    }
}

void TaiXuJianYiCW::Refresh(Player &player)
{
    if (m_cooldown == -1) {
        m_cooldown = s_cooldown;
        m_lastFrames = s_lastFrames;
        m_effectNum = 1;
    }
}

void TaiXuJianYiCW::InitBaseParams()
{
    m_id = BUF_CLASS_CW;
    m_name = "橙武特效";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}
