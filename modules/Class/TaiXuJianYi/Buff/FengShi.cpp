#include "FengShi.h"

#include "Core/Player.h"
#include "Class/TaiXuJianYi/Skill/WuWoWuJian.h"

int FengShi::s_lastFrames = 60 * 16;

FengShi::FengShi()
{
    InitBaseParams();
}

FengShi::~FengShi()
{

}

void FengShi::Cast(Player &player,
                   TargetList &targetList,
                   Stats::ThreadStats &threadStats,
                   Stats::SIM_MODE &simMode)
{
    player.m_skillMap[SKI_WU_WO_WU_JIAN]->UpdateSkillDamageBinPercent(-306);
    m_lastFrames = -1;
    m_effectNum = 0;
}

void FengShi::Refresh(Player &player)
{
    player.m_skillMap[SKI_WU_WO_WU_JIAN]->UpdateSkillDamageBinPercent(306 * (1 ^ m_effectNum));
    m_lastFrames = static_cast<int>(m_lastFrames * player.GetHastePercent());
    m_effectNum = 1;
}

void FengShi::Clean(Player &player, Target &target,
                    Stats::ThreadStats &threadStats,
                    Stats::SIM_MODE &simMode)
{
    player.m_skillMap[SKI_WU_WO_WU_JIAN]->UpdateSkillDamageBinPercent(-306);
    m_lastFrames = -1;
    m_effectNum = 0;
}

void FengShi::InitBaseParams()
{
    m_id = BUF_FENG_SHI;
    m_name = "风逝";
    m_subNameVec.push_back("");
    m_levelNameVec.push_back("");
    m_3rdCooldown = -1;
    m_cooldown = -1;
    m_lastFrames = -1;
    m_intervalFrames = -1;
    m_effectNum = 0;
    m_stackNum = 0;
}
