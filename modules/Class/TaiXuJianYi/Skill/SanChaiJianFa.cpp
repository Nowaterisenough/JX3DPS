#include "SanChaiJianFa.h"

#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

int SanChaiJianFa::s_cooldown = 21;

SanChaiJianFa::SanChaiJianFa(Player &player) :
    Skill(player)
{
    InitBaseParams();
    InitDamageParams();
}

SanChaiJianFa::SanChaiJianFa(const SanChaiJianFa &skill) : Skill(skill)
{

}

SanChaiJianFa::~SanChaiJianFa()
{

}

SanChaiJianFa *SanChaiJianFa::Clone()
{
    return new SanChaiJianFa(*this);
}

SanChaiJianFa &SanChaiJianFa::operator=(const SanChaiJianFa &skill)
{
    Skill::operator=(skill);
    return *this;
}

void SanChaiJianFa::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType)
{
    if (!m_player->IsCast()) {
        TableRes tableRes = GetPhysicsRollResult(*targetsMap[NORMAL].front());
        UpdatePhysicsStats(*targetsMap[NORMAL].front(), stats, settings, tableRes, m_subNames[0], 0);
        SubEffect(targetsMap, stats, settings, tableRes);
        m_cooldown = static_cast<int>((s_cooldown - m_skillCooldownAdd) * m_player->Attr().GetHastePercent());
    }
}

void SanChaiJianFa::InitBaseParams()
{
    m_id = SKI_SAN_CHAI_JIAN_FA;
    m_name = "三柴剑法";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_publicCooldown = &m_player->m_publicCooldown;
}

void SanChaiJianFa::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0] = DamageParam(0, 1229, 0.12);
}

void SanChaiJianFa::SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings, TableRes tableRes)
{
    if (m_player->talents[SHEN_MAI] && tableRes == TableRes::DOUBLE) {
        static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(2);
    }
    if (m_player->GetGearSetAttack() && Random(0.0, 1.0) < 0.1) {
        m_player->buffs[BUF_CLASS_ATTACK]->Refresh();
    }
}

}

}
