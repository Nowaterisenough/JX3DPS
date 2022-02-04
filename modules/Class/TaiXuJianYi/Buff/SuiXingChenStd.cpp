#include "SuiXingChenStd.h"

#include "Core/Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t SuiXingChenStd::s_lastFrames = 36 * 16;
Frame_t SuiXingChenStd::s_intervalFrames = 3 * 16;

SuiXingChenStd::SuiXingChenStd(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

SuiXingChenStd::SuiXingChenStd(const SuiXingChenStd &buff) : Buff(buff)
{

}

SuiXingChenStd::~SuiXingChenStd()
{

}

SuiXingChenStd *SuiXingChenStd::Clone()
{
    return new SuiXingChenStd(*this);
}

SuiXingChenStd &SuiXingChenStd::operator=(const SuiXingChenStd &buff)
{
    Buff::operator=(buff);
    return *this;
}

void SuiXingChenStd::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    if (m_lastFrames == 0) {
        m_lastFrames = INVALID_FRAMES_SET;
        m_intervalFrames = INVALID_FRAMES_SET;
        m_effectNum = 0;
    } else if (!m_player->buffs[BUF_QI_SHENG]->IsExist() && m_player->talents[QI_SHENG]) {
        m_player->buffs[BUF_QI_SHENG]->Refresh();
        m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->GetAttr().GetHastePercent());
    }
}

void SuiXingChenStd::Refresh()
{
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->GetAttr().GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->GetAttr().GetHastePercent());
    m_effectNum = 1;
}

void SuiXingChenStd::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_intervalFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
}

void SuiXingChenStd::InitBaseParams()
{
    m_id = BUF_SUI_XING_CHEN_STD;
    m_name = "气场碎星辰";
    m_subNames.push_back("");
    m_levelNames.push_back("");
}

}

}
