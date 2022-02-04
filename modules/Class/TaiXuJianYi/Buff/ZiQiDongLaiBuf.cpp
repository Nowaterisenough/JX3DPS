#include "ZiQiDongLaiBuf.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t ZiQiDongLaiBuf::s_lastFrames = 6 * 16;
Frame_t ZiQiDongLaiBuf::s_intervalFrames = 3 * 8;
int ZiQiDongLaiBuf::s_maxEffectNum = 4;

ZiQiDongLaiBuf::ZiQiDongLaiBuf(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

ZiQiDongLaiBuf::ZiQiDongLaiBuf(const ZiQiDongLaiBuf &buff) : Buff(buff)
{

}

ZiQiDongLaiBuf::~ZiQiDongLaiBuf()
{

}

ZiQiDongLaiBuf *ZiQiDongLaiBuf::Clone()
{
    return new ZiQiDongLaiBuf(*this);
}

ZiQiDongLaiBuf &ZiQiDongLaiBuf::operator=(const ZiQiDongLaiBuf &buff)
{
    Buff::operator=(buff);
    return *this;
}

void ZiQiDongLaiBuf::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(2);
    m_intervalFrames = s_intervalFrames;
    m_effectNum--;
    if (m_effectNum == 0) {
        m_player->GetAttr().AddPhysicsAttackBaseBinPercent(-256);
        m_player->GetAttr().AddPhysicsCriticalStrikePercent(-0.25);
        m_player->GetAttr().AddPhysicsCriticalStrikePowerPercent(-0.25);
        m_lastFrames = INVALID_FRAMES_SET;
        m_intervalFrames = INVALID_FRAMES_SET;
    }
}

void ZiQiDongLaiBuf::Refresh()
{
    m_player->GetAttr().AddPhysicsAttackBaseBinPercent(256);
    m_player->GetAttr().AddPhysicsCriticalStrikePercent(0.25);
    m_player->GetAttr().AddPhysicsCriticalStrikePowerPercent(0.25);
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->GetAttr().GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->GetAttr().GetHastePercent());
    m_effectNum = s_maxEffectNum;
}

void ZiQiDongLaiBuf::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_intervalFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
}

void ZiQiDongLaiBuf::InitBaseParams()
{
    m_id = BUF_ZI_QI_DONG_LAI;
    m_name = "紫气东来";
    m_subNames.push_back("");
    m_levelNames.push_back("");
}

}

}
