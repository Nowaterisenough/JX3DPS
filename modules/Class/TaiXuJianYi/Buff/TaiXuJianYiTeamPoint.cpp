#include "TaiXuJianYiTeamPoint.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t TaiXuJianYiTeamPoint::s_cooldown = 6 * 16;

TaiXuJianYiTeamPoint::TaiXuJianYiTeamPoint(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

TaiXuJianYiTeamPoint::TaiXuJianYiTeamPoint(const TaiXuJianYiTeamPoint &buff) : Buff(buff)
{

}

TaiXuJianYiTeamPoint::~TaiXuJianYiTeamPoint()
{

}

TaiXuJianYiTeamPoint *TaiXuJianYiTeamPoint::Clone()
{
    return new TaiXuJianYiTeamPoint(*this);
}

TaiXuJianYiTeamPoint &TaiXuJianYiTeamPoint::operator=(const TaiXuJianYiTeamPoint &buff)
{
    Buff::operator=(buff);
    return *this;
}

void TaiXuJianYiTeamPoint::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    if (m_cooldown == 0) {
        m_cooldown = static_cast<int>(RandBetween(1.0, 1.2) * s_cooldown);
        static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(2);
    }
}

void TaiXuJianYiTeamPoint::Refresh()
{

}

void TaiXuJianYiTeamPoint::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_cooldown = INVALID_FRAMES_SET;
}

void TaiXuJianYiTeamPoint::InitBaseParams()
{
    m_id = BUF_CLASS_TEAM_POINT;
    m_name = "阵眼";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_cooldown = static_cast<int>(RandBetween(1.0, 1.2) * s_cooldown);
}

}

}
