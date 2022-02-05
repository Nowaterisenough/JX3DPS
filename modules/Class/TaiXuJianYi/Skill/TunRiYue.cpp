#include "TunRiYue.h"

#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

int TunRiYue::s_cooldown = 24;
int TunRiYue::s_prepareFrames = 16;

TunRiYue::TunRiYue(Player &player) :
    Skill(player)
{
    InitBaseParams();
    InitDamageParams();
}

TunRiYue::TunRiYue(const TunRiYue &skill) :
    Skill(skill),
    m_skillQidianAdd(skill.m_skillQidianAdd)
{

}

TunRiYue::~TunRiYue()
{

}

TunRiYue *TunRiYue::Clone()
{
    return new TunRiYue(*this);
}

TunRiYue &TunRiYue::operator=(const TunRiYue &skill)
{
    Skill::operator=(skill);
    m_skillQidianAdd = skill.m_skillQidianAdd;
    return *this;
}

void TunRiYue::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType)
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

void TunRiYue::UpdateSkillQidian(int num)
{
    m_skillQidianAdd += num;
}

void TunRiYue::InitBaseParams()
{
    m_id = SKI_TUN_RI_YUE;
    m_name = "吞日月";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_publicCooldown = &m_player->publicCooldown;
    m_isPublicCooldown = PUBLIC_SKILL;
}

void TunRiYue::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0] = DamageParam(0, 0, 0.0);
}

void TunRiYue::SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings, TableRes tableRes)
{
    static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(m_skillQidianAdd);
    m_player->buffs[BUF_TUN_RI_YUE_STD]->Refresh();
    if (m_player->talents[CHANG_SHENG]) {
        m_player->buffs[BUF_CHI_YING]->Refresh();
    }
}

}

}
