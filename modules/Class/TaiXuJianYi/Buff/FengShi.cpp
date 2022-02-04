#include "FengShi.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/Skill/WuWoWuJian.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t FengShi::s_lastFrames = 60 * 16;

FengShi::FengShi(Player &player) :
    Buff(player)
{
    InitBaseParams();
}

FengShi::FengShi(const FengShi &buff) : Buff(buff)
{

}

FengShi::~FengShi()
{

}

FengShi *FengShi::Clone()
{
    return new FengShi(*this);
}

FengShi &FengShi::operator=(const FengShi &buff)
{
    Buff::operator=(buff);
    return *this;
}

void FengShi::Cast(JX3DPS::TargetsMap &targetsMap, JX3DPS::Stats &stats, JX3DPS::Settings &settings)
{
    m_player->skills[SKI_WU_WO_WU_JIAN]->UpdateSkillDamageBinPercent(-306);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
}

void FengShi::Refresh()
{
    m_player->skills[SKI_WU_WO_WU_JIAN]->UpdateSkillDamageBinPercent(306 * (1 ^ m_effectNum));
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->GetAttr().GetHastePercent());
    m_effectNum = 1;
}

void FengShi::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_player->skills[SKI_WU_WO_WU_JIAN]->UpdateSkillDamageBinPercent(-306);
    m_lastFrames = INVALID_FRAMES_SET;
    m_effectNum = 0;
}

void FengShi::InitBaseParams()
{
    m_id = BUF_FENG_SHI;
    m_name = "风逝";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_isDamage = false;
}

}

}
