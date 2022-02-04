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

void XuanMen::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    m_player->GetAttr().AddPhysicsCriticalStrikePercent(-m_stackNum * 0.03);
    m_player->GetAttr().AddPhysicsOvercomeBaseBinPercent(-m_stackNum * 204);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
    m_stackNum = 0;
}

void XuanMen::Refresh()
{
    m_player->GetAttr().AddPhysicsCriticalStrikePercent(-m_stackNum * 0.03);
    m_player->GetAttr().AddPhysicsOvercomeBaseBinPercent(-m_stackNum * 204);
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->GetAttr().GetHastePercent());
    m_effectNum = 1;
    m_stackNum++;
    m_stackNum = GET_MIN_INT(m_stackNum, s_maxStackNum);
    m_player->GetAttr().AddPhysicsCriticalStrikePercent(m_stackNum * 0.03);
    m_player->GetAttr().AddPhysicsOvercomeBaseBinPercent(m_stackNum * 204);
}

void XuanMen::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_player->GetAttr().AddPhysicsCriticalStrikePercent(-m_stackNum * 0.03);
    m_player->GetAttr().AddPhysicsOvercomeBaseBinPercent(-m_stackNum * 204);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
    m_stackNum = 0;
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
