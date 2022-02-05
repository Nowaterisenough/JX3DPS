#include "ShengTaiJiStd.h"

#include "Core/Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t ShengTaiJiStd::s_lastFrames = 3 * 16;
Frame_t ShengTaiJiStd::s_intervalFrames = 3 * 16;

ShengTaiJiStd::ShengTaiJiStd(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

ShengTaiJiStd::ShengTaiJiStd(const ShengTaiJiStd &buff) : Buff(buff)
{

}

ShengTaiJiStd::~ShengTaiJiStd()
{

}

ShengTaiJiStd *ShengTaiJiStd::Clone()
{
    return new ShengTaiJiStd(*this);
}

ShengTaiJiStd &ShengTaiJiStd::operator=(const ShengTaiJiStd &buff)
{
    Buff::operator=(buff);
    return *this;
}

void ShengTaiJiStd::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
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

void ShengTaiJiStd::Refresh()
{
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->Attr().GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->Attr().GetHastePercent());
    m_effectCount = 1;
}

void ShengTaiJiStd::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_intervalFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
}

void ShengTaiJiStd::InitBaseParams()
{
    m_id = BUF_SHENG_TAI_JI_STD;
    m_name = "生太极气场";
    m_subNames.push_back("");
    m_levelNames.push_back("");
}

}

}
