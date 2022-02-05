#include "SuiXingChenBuf.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t SuiXingChenBuf::s_lastFrames = 4 * 16;

SuiXingChenBuf::SuiXingChenBuf(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

SuiXingChenBuf::SuiXingChenBuf(const SuiXingChenBuf &buff) : Buff(buff)
{

}

SuiXingChenBuf::~SuiXingChenBuf()
{

}

SuiXingChenBuf *SuiXingChenBuf::Clone()
{
    return new SuiXingChenBuf(*this);
}

SuiXingChenBuf &SuiXingChenBuf::operator=(const SuiXingChenBuf &buff)
{
    Buff::operator=(buff);
    return *this;
}

void SuiXingChenBuf::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
    if (m_player->talents[WU_YU]) {
        m_player->Attr().AddPhysicsCriticalStrikePercent(-0.1);
        m_player->Attr().AddPhysicsCriticalStrikePowerPercent(-0.2);
    } else {
        m_player->Attr().AddPhysicsCriticalStrikePercent(-0.05);
        m_player->Attr().AddPhysicsCriticalStrikePowerPercent(-0.1);
    }
}

void SuiXingChenBuf::Refresh()
{
    if (m_player->talents[WU_YU]) {
        m_player->Attr().AddPhysicsCriticalStrikePercent(0.1 * (1 ^ m_effectCount));
        m_player->Attr().AddPhysicsCriticalStrikePowerPercent(0.2 * (1 ^ m_effectCount));
    } else {
        m_player->Attr().AddPhysicsCriticalStrikePercent(0.05 * (1 ^ m_effectCount));
        m_player->Attr().AddPhysicsCriticalStrikePowerPercent(0.1 * (1 ^ m_effectCount));
    }
    m_lastFrames = s_lastFrames;
    m_effectCount = 1;
}

void SuiXingChenBuf::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
}

void SuiXingChenBuf::InitBaseParams()
{
    m_id = BUF_SUI_XING_CHEN;
    m_name = "碎星辰buff";
    m_subNames.push_back("");
    m_levelNames.push_back("");
}

}

}
