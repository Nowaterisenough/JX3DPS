#include "ZiQiDongLaiBuf.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

int ZiQiDongLaiBuf::s_lastFrames = 6 * 16;
int ZiQiDongLaiBuf::s_intervalFrames = 3 * 8;
int ZiQiDongLaiBuf::s_maxEffectNum = 4;

ZiQiDongLaiBuf::ZiQiDongLaiBuf()
{
    InitBaseParams();
}

ZiQiDongLaiBuf::~ZiQiDongLaiBuf()
{

}

void ZiQiDongLaiBuf::Cast(Player &player, TargetList &targetList, Stats::ThreadStats &threadStats, Stats::SIM_MODE &simMode)
{
    static_cast<TaiXuJianYi *>(&player)->UpdateQidian(2);
    m_intervalFrames = s_intervalFrames;
    m_effectNum--;
    if (m_effectNum == 0) {
        player.AddPhysicsAttackBaseBinPercent(-256);
        player.AddPhysicsCriticalStrikePercent(-0.25);
        player.AddPhysicsCriticalStrikePowerPercent(-0.25);
        m_lastFrames = -1;
        m_intervalFrames = -1;
    }
}

void ZiQiDongLaiBuf::Refresh(Player &player)
{
    player.AddPhysicsAttackBaseBinPercent(256);
    player.AddPhysicsCriticalStrikePercent(0.25);
    player.AddPhysicsCriticalStrikePowerPercent(0.25);
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * player.GetHastePercent());
    m_effectNum = s_maxEffectNum;
}

void ZiQiDongLaiBuf::InitBaseParams()
{
    m_id = BUF_ZI_QI_DONG_LAI;
    m_name = "紫气东来";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}
