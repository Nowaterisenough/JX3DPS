#include "JianMing.h"

#include "Core/Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t JianMing::s_lastFrames = 6 * 16;

JianMing::JianMing(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

JianMing::JianMing(const JianMing &buff) : Buff(buff)
{

}

JianMing::~JianMing()
{

}

JianMing *JianMing::Clone()
{
    return new JianMing(*this);
}

JianMing &JianMing::operator=(const JianMing &buff)
{
    Buff::operator=(buff);
    return *this;
}

void JianMing::Cast(TargetsMap &targetsMap, Stats &stats)
{
    m_player->Attr().AddPhysicsAttackBaseBinPercent(-102);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
}

void JianMing::Refresh()
{
    m_player->Attr().AddPhysicsAttackBaseBinPercent(102);
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->Attr().GetHastePercent());
    m_effectCount = 1;
}

void JianMing::Clean(TargetsMap &targetsMap, Stats &stats, int param)
{
    m_player->Attr().AddPhysicsAttackBaseBinPercent(-102);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
}

void JianMing::InitBaseParams()
{
    m_id = BUF_CLASS_ATTACK;
    m_name = "剑鸣";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_isDamage = false;
}

}

}
