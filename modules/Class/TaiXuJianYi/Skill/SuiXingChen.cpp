#include "SuiXingChen.h"

#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

int SuiXingChen::s_cooldown = 24;
int SuiXingChen::s_prepareFrames = 16;

SuiXingChen::SuiXingChen(Player &player) :
    Skill(player)
{
    InitBaseParams();
}

SuiXingChen::SuiXingChen(const SuiXingChen &skill) :
    Skill(skill),
    m_skillQidianAdd(skill.m_skillQidianAdd)
{

}

SuiXingChen::~SuiXingChen()
{

}

SuiXingChen *SuiXingChen::Clone()
{
    return new SuiXingChen(*this);
}

SuiXingChen &SuiXingChen::operator=(const SuiXingChen &skill)
{
    Skill::operator=(skill);
    m_skillQidianAdd = skill.m_skillQidianAdd;
    return *this;
}

void SuiXingChen::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType)
{
    if (castType == CastType::SKILL) {
        m_prepareFrames = static_cast<int>(s_prepareFrames * m_player->GetAttr().GetHastePercent());
    } else {
        m_cooldown = static_cast<int>((s_cooldown - m_skillCooldownAdd) * m_player->GetAttr().GetHastePercent());
        m_prepareFrames = INVALID_FRAMES_SET;
        UpdatePhysicsStats(*targetsMap[NORMAL].front(), stats, settings, TableRes::ALL, m_subNames[0], 0);
        SubEffect(targetsMap, stats, settings, TableRes::ALL);
    }
}

void SuiXingChen::UpdateSkillQidian(int num)
{
    m_skillQidianAdd += num;
}

void SuiXingChen::InitBaseParams()
{
    m_id = SKI_SUI_XING_CHEN;
    m_name = "碎星辰";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_publicCooldown = &m_player->publicCooldown;
    m_isPublicCooldown = PUBLIC_SKILL;
}

void SuiXingChen::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0] = DamageParam(0, 0, 0.0);
}

void SuiXingChen::SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings, TableRes tableRes)
{
    static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(m_skillQidianAdd);
    m_player->buffs[BUF_SUI_XING_CHEN_STD]->Refresh();
    m_player->buffs[BUF_SUI_XING_CHEN_STD2]->Refresh();
    if (m_player->talents[CHANG_SHENG]) {
        m_player->buffs[BUF_CHI_YING]->Refresh();
    }
}

}

}
