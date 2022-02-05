#include "TaiXuJianYiCW.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t TaiXuJianYiCW::s_lastFrames = 6 * 16;
Frame_t TaiXuJianYiCW::s_cooldown = 40 * 16;

TaiXuJianYiCW::TaiXuJianYiCW(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

TaiXuJianYiCW::TaiXuJianYiCW(const TaiXuJianYiCW &buff) : Buff(buff)
{

}

TaiXuJianYiCW::~TaiXuJianYiCW()
{

}

TaiXuJianYiCW *TaiXuJianYiCW::Clone()
{
    return new TaiXuJianYiCW(*this);
}

TaiXuJianYiCW &TaiXuJianYiCW::operator=(const TaiXuJianYiCW &buff)
{
    Buff::operator=(buff);
    return *this;
}

void TaiXuJianYiCW::Cast(TargetsMap &targetsMap, Stats &stats)
{
    m_cooldown = IF_1ST_0_TO_2ND_ELSE_3RD(m_cooldown, INVALID_FRAMES_SET, m_cooldown);
    m_lastFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_lastFrames, INVALID_FRAMES_SET, m_lastFrames);
    m_effectCount = IF_1ST_0_TO_0_ELSE_2ND(m_lastFrames, m_effectCount);
}

void TaiXuJianYiCW::Refresh()
{
    if (m_cooldown == INVALID_FRAMES_SET) {
        m_cooldown = s_cooldown;
        m_lastFrames = s_lastFrames;
        m_effectCount = 1;
    }
}

void TaiXuJianYiCW::Clean(TargetsMap &targetsMap, Stats &stats, int param)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
}

void TaiXuJianYiCW::InitBaseParams()
{
    m_id = BUF_CLASS_CW;
    m_name = "橙武特效";
    m_subNames.push_back("");
    m_levelNames.push_back("");
}

}

}
