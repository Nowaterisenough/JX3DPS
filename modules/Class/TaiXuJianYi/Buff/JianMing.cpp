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

void JianMing::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    m_player->GetAttr().AddPhysicsAttackBaseBinPercent(-102);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
}

void JianMing::Refresh()
{
    m_player->GetAttr().AddPhysicsAttackBaseBinPercent(102);
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->GetAttr().GetHastePercent());
    m_effectNum = 1;
}

void JianMing::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_player->GetAttr().AddPhysicsAttackBaseBinPercent(-102);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
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
