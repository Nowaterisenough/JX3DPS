#include "RenJianHeYi.h"

#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"
#include "Class/TaiXuJianYi/Buff/SuiXingChenStd.h"
#include "Class/TaiXuJianYi/Buff/SuiXingChenStd2.h"
#include "Class/TaiXuJianYi/Buff/ShengTaiJiStd.h"
#include "Class/TaiXuJianYi/Buff/TunRiYueStd.h"

namespace JX3DPS {

namespace TaiXuJianYi {

int RenJianHeYi::s_cooldown = 12 * 16;

RenJianHeYi::RenJianHeYi(Player &player) :
    Skill(player)
{
    InitBaseParams();
    InitDamageParams();
}

RenJianHeYi::RenJianHeYi(const RenJianHeYi &skill) :
    Skill(skill),
    m_skillQidianAdd(skill.m_skillQidianAdd)
{

}

RenJianHeYi::~RenJianHeYi()
{

}

RenJianHeYi *RenJianHeYi::Clone()
{
    return new RenJianHeYi(*this);
}

RenJianHeYi &RenJianHeYi::operator=(const RenJianHeYi &skill)
{
    Skill::operator=(skill);
    m_skillQidianAdd = skill.m_skillQidianAdd;
    return *this;
}

void RenJianHeYi::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType)
{
    for (auto it : targetsMap[NORMAL]) {
        TableRes tableRes = GetPhysicsRollResult(*it);
        UpdatePhysicsStats(*it, stats, settings, tableRes, m_subNames[0], 0);
        SubEffect(*it, stats, settings, tableRes);
    }
    if (m_player->buffs[BUF_SHENG_TAI_JI_STD]->IsExist()) {
        if (m_player->GetGearSetAttack() && Random(0.0, 1.0) < 0.1) {
            m_player->buffs[BUF_CLASS_ATTACK]->Refresh();
        }
        if (m_player->GetGearSetCW() && Random(0.0, 1.0) < 0.1) {
            m_player->buffs[BUF_CLASS_CW]->Refresh();
        }
    }
    if (m_player->buffs[BUF_TUN_RI_YUE_STD]->IsExist()) {
        if (m_player->GetGearSetAttack() && Random(0.0, 1.0) < 0.1) {
            m_player->buffs[BUF_CLASS_ATTACK]->Refresh();
        }
        if (m_player->GetGearSetCW() && Random(0.0, 1.0) < 0.1) {
            m_player->buffs[BUF_CLASS_CW]->Refresh();
        }
    }
    if (m_player->buffs[BUF_SUI_XING_CHEN_STD]->IsExist()) {
        if (m_player->GetGearSetAttack() && Random(0.0, 1.0) < 0.1) {
            m_player->buffs[BUF_CLASS_ATTACK]->Refresh();
        }
        if (m_player->GetGearSetCW() && Random(0.0, 1.0) < 0.1) {
            m_player->buffs[BUF_CLASS_CW]->Refresh();
        }
    }
    m_cooldown = static_cast<int>((s_cooldown - m_skillCooldownAdd) * m_player->Attr().GetHastePercent());
}

void RenJianHeYi::InitBaseParams()
{
    m_id = SKI_REN_JIAN_HE_YI;
    m_name = "人剑合一";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_publicCooldown = &m_player->publicCooldown;
    m_isPublicCooldown = PUBLIC_SKILL;
    m_skillQidianAdd = 0;
}

void RenJianHeYi::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0] = DamageParam(63, 0, 0.1);
}

void RenJianHeYi::SubEffect(Target &target, Stats &stats, Settings &settings, TableRes tableRes)
{
    if (m_player->talents[SHEN_MAI] && tableRes == TableRes::DOUBLE) {
        static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(2);
    }

}

}

}
