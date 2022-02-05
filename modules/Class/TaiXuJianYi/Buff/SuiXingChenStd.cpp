/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-05 16:32:22
 * @Update      : NoWats
 * @LastTime    : 2022-02-05 16:32:52
 * @FilePath    : \JX3DPS\modules\Class\TaiXuJianYi\Buff\SuiXingChenStd.cpp
 */
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

void SuiXingChenStd::Cast(TargetsMap &targetsMap, Stats &stats)
{
    if (m_lastFrames == 0) {
        m_lastFrames = INVALID_FRAMES_SET;
        m_intervalFrames = INVALID_FRAMES_SET;
        m_effectCount = 0;
    } else if (!m_player->buffs[BUF_QI_SHENG]->IsExist() && m_player->talents[QI_SHENG]) {
        m_player->buffs[BUF_QI_SHENG]->Refresh();
        m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->Attr().GetHastePercent());
    }
}

void SuiXingChenStd::Refresh()
{
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->Attr().GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->Attr().GetHastePercent());
    m_effectCount = 1;
}

void SuiXingChenStd::Clean(TargetsMap &targetsMap, Stats &stats, int param)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_intervalFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
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
