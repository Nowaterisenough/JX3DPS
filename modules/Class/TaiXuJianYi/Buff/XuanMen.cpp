#include "XuanMen.h"

#include "Core/Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t XuanMen::s_lastFrames = 36 * 16;
int XuanMen::s_maxStackNum = 3;

XuanMen::XuanMen(Player &player) :
    Buff(player)
{

}

XuanMen::XuanMen(const XuanMen &buff) : Buff(buff)
{

}

XuanMen::~XuanMen()
{

}

XuanMen *XuanMen::Clone()
{
    return new XuanMen(*this);
}

XuanMen &XuanMen::operator=(const XuanMen &buff)
{
    Buff::operator=(buff);
    return *this;
}

void XuanMen::Cast(TargetsMap &targetsMap, Stats &stats)
{
    m_player->Attr().AddPhysicsCriticalStrikePercent(-m_stackCount * 0.03);
    m_player->Attr().AddPhysicsOvercomeBaseBinPercent(-m_stackCount * 204);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
    m_stackCount = 0;
}

void XuanMen::Refresh()
{
    m_player->Attr().AddPhysicsCriticalStrikePercent(-m_stackCount * 0.03);
    m_player->Attr().AddPhysicsOvercomeBaseBinPercent(-m_stackCount * 204);
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->Attr().GetHastePercent());
    m_effectCount = 1;
    m_stackCount++;
    m_stackCount = GET_MIN_INT(m_stackCount, s_maxStackNum);
    m_player->Attr().AddPhysicsCriticalStrikePercent(m_stackCount * 0.03);
    m_player->Attr().AddPhysicsOvercomeBaseBinPercent(m_stackCount * 204);
}

void XuanMen::Clean(TargetsMap &targetsMap, Stats &stats, int param)
{
    m_player->Attr().AddPhysicsCriticalStrikePercent(-m_stackCount * 0.03);
    m_player->Attr().AddPhysicsOvercomeBaseBinPercent(-m_stackCount * 204);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
    m_stackCount = 0;
}

void XuanMen::InitBaseParams()
{
    m_id = BUF_XUAN_MEN;
    m_name = "玄门";
    m_subNames.push_back("");
    m_levelNames.push_back("");
}

}

}
