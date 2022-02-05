#include "SanHuanTaoYue.h"

#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

int SanHuanTaoYue::s_cooldown = 2 * 16;

SanHuanTaoYue::SanHuanTaoYue(Player &player) :
    Skill(player)
{
    InitBaseParams();
    InitDamageParams();
}

SanHuanTaoYue::SanHuanTaoYue(const SanHuanTaoYue &skill) :
    Skill(skill),
    m_skillQidianAdd(skill.m_skillQidianAdd)
{

}

SanHuanTaoYue::~SanHuanTaoYue()
{

}

SanHuanTaoYue *SanHuanTaoYue::Clone()
{
    return new SanHuanTaoYue(*this);
}

SanHuanTaoYue &SanHuanTaoYue::operator=(const SanHuanTaoYue &skill)
{
    Skill::operator=(skill);
    m_skillQidianAdd = skill.m_skillQidianAdd;
    return *this;
}

void SanHuanTaoYue::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType)
{
    TableRes tableRes = GetPhysicsRollResult(*targetsMap[NORMAL].front());
    UpdatePhysicsStats(*targetsMap[NORMAL].front(), stats, settings, tableRes, m_subNames[0], 0);
    SubEffect(targetsMap, stats, settings, tableRes);
    m_cooldown = static_cast<int>((s_cooldown - m_skillCooldownAdd) * m_player->Attr().GetHastePercent());
}

void SanHuanTaoYue::UpdateSkillQidian(int num)
{
    m_skillQidianAdd += num;
}

void SanHuanTaoYue::InitBaseParams()
{
    m_id = SKI_SAN_HUAN_TAO_YUE;
    m_name = "三环套月";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_publicCooldown = &m_player->m_publicCooldown;
    m_isPublicCooldown = PUBLIC_SKILL;
    m_skillQidianAdd = 0;
}

void SanHuanTaoYue::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0] = DamageParam((123 + 136) / 2, 1024, 0.95);
}

void SanHuanTaoYue::SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings, TableRes tableRes)
{
    if (tableRes != TableRes::MISS) {
        static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(m_skillQidianAdd);
        if (m_player->talents[SHEN_MAI] && tableRes == TableRes::DOUBLE) {
            static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(2);
        }
        if (m_player->GetGearSetCW() && Random(0.0, 1.0) < 0.1) {
            m_player->buffs[BUF_CLASS_CW]->Refresh();
        }
    }
    if (m_player->talents[FENG_SHI]) {
        m_player->buffs[BUF_FENG_SHI]->Refresh();
    }
    if (m_player->GetGearSetAttack() && Random(0.0, 1.0) < 0.1) {
        m_player->buffs[BUF_CLASS_ATTACK]->Refresh();
    }
}

}

}
