#include "QiSheng.h"

#include "Core/Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t QiSheng::s_lastFrames = 3 * 16;

QiSheng::QiSheng(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

QiSheng::QiSheng(const QiSheng &buff) : Buff(buff)
{

}

QiSheng::~QiSheng()
{

}

QiSheng *QiSheng::Clone()
{
    return new QiSheng(*this);
}

QiSheng &QiSheng::operator=(const QiSheng &buff)
{
    Buff::operator=(buff);
    return *this;
}

void QiSheng::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    m_player->GetAttr().AddPhysicsAttackBaseBinPercent(-102);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
}

void QiSheng::Refresh()
{
    m_player->GetAttr().AddPhysicsAttackBaseBinPercent(102);
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->GetAttr().GetHastePercent());
    m_effectNum = 1;
}

void QiSheng::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_player->GetAttr().AddPhysicsAttackBaseBinPercent(-102);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
}

void QiSheng::InitBaseParams()
{
    m_id = BUF_QI_SHENG;
    m_name = "期声";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_isDamage = false;
}


}

}
