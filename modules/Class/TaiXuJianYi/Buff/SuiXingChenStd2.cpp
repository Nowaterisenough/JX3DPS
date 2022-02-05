#include "SuiXingChenStd2.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t SuiXingChenStd2::s_lastFrames = 36 * 16;
Frame_t SuiXingChenStd2::s_intervalFrames = 3 * 16;

SuiXingChenStd2::SuiXingChenStd2(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

SuiXingChenStd2::SuiXingChenStd2(const SuiXingChenStd2 &buff) : Buff(buff)
{

}

SuiXingChenStd2::~SuiXingChenStd2()
{

}

SuiXingChenStd2 *SuiXingChenStd2::Clone()
{
    return new SuiXingChenStd2(*this);
}

SuiXingChenStd2 &SuiXingChenStd2::operator=(const SuiXingChenStd2 &buff)
{
    Buff::operator=(buff);
    return *this;
}

void SuiXingChenStd2::Cast(TargetsMap &targetsMap, Stats &stats)
{
    if (m_lastFrames == 0) {
        m_lastFrames = INVALID_FRAMES_SET;
        m_intervalFrames = INVALID_FRAMES_SET;
        m_effectCount = 0;
    } else if (!m_player->buffs[BUF_SUI_XING_CHEN]->IsExist()) {
        m_player->buffs[BUF_SUI_XING_CHEN]->Refresh();
        m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->Attr().GetHastePercent());
    }
}

void SuiXingChenStd2::Refresh()
{
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->Attr().GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->Attr().GetHastePercent());
    m_effectCount = 1;
}

void SuiXingChenStd2::Clean(TargetsMap &targetsMap, Stats &stats, int param)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_intervalFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
}

void SuiXingChenStd2::InitBaseParams()
{
    m_id = BUF_SUI_XING_CHEN_STD2;
    m_name = "气场碎星辰2";
    m_subNames.push_back("");
    m_levelNames.push_back("");
}

}

}
