#include "RongJin.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t RongJin::s_intervalFrames = 16;

RongJin::RongJin(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

RongJin::RongJin(const RongJin &buff) : Buff(buff)
{

}

RongJin::~RongJin()
{

}

RongJin *RongJin::Clone()
{
    return new RongJin(*this);
}

RongJin &RongJin::operator=(const RongJin &buff)
{
    Buff::operator=(buff);
    return *this;
}

void RongJin::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(1);
    m_intervalFrames = s_intervalFrames;
}

void RongJin::Refresh()
{

}

void RongJin::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_intervalFrames = INVALID_FRAMES_SET;
}

void RongJin::InitBaseParams()
{
    m_id = BUF_CLASS_EFFECT;
    m_name = "融金";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_intervalFrames = static_cast<int>(RandBetween(0.0, 1.0) * s_intervalFrames);
}

}

}
