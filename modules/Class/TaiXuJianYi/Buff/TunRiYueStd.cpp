#include "TunRiYueStd.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t TunRiYueStd::s_lastFrames = 36 * 16;

TunRiYueStd::TunRiYueStd(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

TunRiYueStd::TunRiYueStd(const TunRiYueStd &buff) : Buff(buff)
{

}

TunRiYueStd::~TunRiYueStd()
{

}

TunRiYueStd *TunRiYueStd::Clone()
{
    return new TunRiYueStd(*this);
}

TunRiYueStd &TunRiYueStd::operator=(const TunRiYueStd &buff)
{
    Buff::operator=(buff);
    return *this;
}

void TunRiYueStd::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
}

void TunRiYueStd::Refresh()
{
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->Attr().GetHastePercent());
    m_effectCount = 1;
}

void TunRiYueStd::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
}

void TunRiYueStd::InitBaseParams()
{
    m_id = BUF_TUN_RI_YUE_STD;
    m_name = "气场吞日月";
    m_subNames.push_back("");
    m_levelNames.push_back("");

}

}

}
