#include "ShengTaiJi.h"

#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

int ShengTaiJi::s_cooldown = 24;
int ShengTaiJi::s_prepareFrames = 16;

ShengTaiJi::ShengTaiJi(Player &player) :
    Skill(player)
{
    InitBaseParams();
}

ShengTaiJi::ShengTaiJi(const ShengTaiJi &skill) :
    Skill(skill),
    m_skillQidianAdd(skill.m_skillQidianAdd)
{

}

ShengTaiJi::~ShengTaiJi()
{

}

ShengTaiJi *ShengTaiJi::Clone()
{
    return new ShengTaiJi(*this);
}

ShengTaiJi &ShengTaiJi::operator=(const ShengTaiJi &skill)
{
    Skill::operator=(skill);
    m_skillQidianAdd = skill.m_skillQidianAdd;
    return *this;
}

void ShengTaiJi::Cast(TargetsMap &targetsMap, Stats &stats, CastType castType)
{
    if (castType == CastType::SKILL) {
        m_prepareFrames = static_cast<int>(s_prepareFrames * m_player->Attr().GetHastePercent());
    } else {
        m_cooldown = static_cast<int>((s_cooldown - m_skillCooldownAdd) * m_player->Attr().GetHastePercent());
        m_prepareFrames = INVALID_FRAMES_SET;
        UpdatePhysicsStats(*targetsMap[NORMAL].front(), stats, settings, TableRes::ALL, m_subNames[0], 0);
        SubEffect(targetsMap, stats, settings, TableRes::ALL);
    }
}

void ShengTaiJi::UpdateSkillQidian(int num)
{
    m_skillQidianAdd += num;
}

void ShengTaiJi::InitBaseParams()
{
    m_id = SKI_SHENG_TAI_JI;
    m_name = "生太极";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_publicCooldown = &m_player->m_publicCooldown;
    m_isPublicCooldown = PUBLIC_SKILL;
}

void ShengTaiJi::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0] = DamageParam(0, 0, 0.0);
}

void ShengTaiJi::SubEffect(TargetsMap &targetsMap, Stats &stats, TableRes tableRes)
{
    static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(m_skillQidianAdd);
    m_player->buffs[BUF_SHENG_TAI_JI_STD]->Refresh();
    if (m_player->talents[CHANG_SHENG]) {
        m_player->buffs[BUF_CHI_YING]->Refresh();
    }
}

}

}
