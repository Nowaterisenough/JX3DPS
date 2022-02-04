#include "BaHuangGuiYuan.h"

#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"
#include "Class/TaiXuJianYi/Buff/DieRen.h"

namespace JX3DPS {

namespace TaiXuJianYi {

int BaHuangGuiYuan::s_cooldown = 12 * 16;

BaHuangGuiYuan::BaHuangGuiYuan(Player &player) :
    Skill(player)
{
    InitBaseParams();
    InitDamageParams();
}

BaHuangGuiYuan::BaHuangGuiYuan(const BaHuangGuiYuan &skill) :
    Skill(skill),
    m_skillQidianAdd(skill.m_skillQidianAdd)
{

}

BaHuangGuiYuan::~BaHuangGuiYuan()
{

}

BaHuangGuiYuan *BaHuangGuiYuan::Clone()
{
    return new BaHuangGuiYuan(*this);
}

BaHuangGuiYuan &BaHuangGuiYuan::operator=(const BaHuangGuiYuan &skill)
{
    Skill::operator=(skill);
    m_skillQidianAdd = skill.m_skillQidianAdd;
    return *this;
}

void BaHuangGuiYuan::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType)
{
    TableRes tableRes = GetPhysicsRollResult(*targetsMap[NORMAL].front());
    UpdatePhysicsStats(*targetsMap[NORMAL].front(), stats, settings, tableRes, m_subNames[0], 9 - static_cast<int>(targetsMap[NORMAL].front()->GetLifePercent() * 10));
    SubEffect(targetsMap, stats, settings, tableRes);
    m_cooldown = static_cast<int>((s_cooldown - m_skillCooldownAdd) * m_player->GetAttr().GetHastePercent());
}

void BaHuangGuiYuan::UpdateCooldown(Frame_t frames)
{
    m_cooldown -= frames;
    m_cooldown = GET_MAX_INT(0, m_cooldown);
}

void BaHuangGuiYuan::UpdateSkillQidian(int num)
{
    m_skillQidianAdd += num;
}

void BaHuangGuiYuan::InitBaseParams()
{
    m_id = SKI_BA_HUANG_GUI_YUAN;
    m_name = "八荒归元";
    m_subNames.push_back("1段");
    m_subNames.push_back("2段");
    m_subNames.push_back("和光");
    m_levelNames.push_back("100%-90%");
    m_levelNames.push_back("90%-80%");
    m_levelNames.push_back("80%-70%");
    m_levelNames.push_back("70%-60%");
    m_levelNames.push_back("60%-50%");
    m_levelNames.push_back("50%-40%");
    m_levelNames.push_back("40%-30%");
    m_levelNames.push_back("30%-20%");
    m_levelNames.push_back("20%-10%");
    m_levelNames.push_back("10%-0%");
    m_publicCooldown = &m_player->publicCooldown;
    m_isPublicCooldown = PUBLIC_SKILL;
    m_skillQidianAdd = 0;
}

void BaHuangGuiYuan::InitDamageParams()
{
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));
    m_damageParams[m_subNames[0]].push_back(DamageParam((2115 + 2125) / 2, 0, 0.1));

    m_damageParams [m_subNames[1]].push_back(DamageParam((23 + 25) / 2, 2048, 1.1375));
    m_damageParams [m_subNames[1]].push_back(DamageParam((97 + 107) / 2, 2048, 1.2688));
    m_damageParams [m_subNames[1]].push_back(DamageParam((171 + 188) / 2, 2048, 1.4003));
    m_damageParams [m_subNames[1]].push_back(DamageParam((245 + 270) / 2, 2048, 1.5317));
    m_damageParams [m_subNames[1]].push_back(DamageParam((319 + 352) / 2, 2048, 1.6631));
    m_damageParams [m_subNames[1]].push_back(DamageParam((394 + 433) / 2, 2048, 1.7944));
    m_damageParams [m_subNames[1]].push_back(DamageParam((468 + 515) / 2, 2048, 1.9258));
    m_damageParams [m_subNames[1]].push_back(DamageParam((542 + 597) / 2, 2048, 2.0572));
    m_damageParams [m_subNames[1]].push_back(DamageParam((616 + 678) / 2, 2048, 2.1886));
    m_damageParams [m_subNames[1]].push_back(DamageParam((690 + 760) / 2, 2048, 2.3200));

    m_damageParams[m_subNames[2]].push_back(DamageParam((23 + 25) / 8, 512, 1.1375 / 4));
    m_damageParams[m_subNames[2]].push_back(DamageParam((97 + 107) / 8, 512, 1.2688 / 4));
    m_damageParams[m_subNames[2]].push_back(DamageParam((171 + 188) / 8, 512, 1.4003 / 4));
    m_damageParams[m_subNames[2]].push_back(DamageParam((245 + 270) / 8, 512, 1.5317 / 4));
    m_damageParams[m_subNames[2]].push_back(DamageParam((319 + 352) / 8, 512, 1.6631 / 4));
    m_damageParams[m_subNames[2]].push_back(DamageParam((394 + 433) / 8, 512, 1.7944 / 4));
    m_damageParams[m_subNames[2]].push_back(DamageParam((468 + 515) / 8, 512, 1.9258 / 4));
    m_damageParams[m_subNames[2]].push_back(DamageParam((542 + 597) / 8, 512, 2.0572 / 4));
    m_damageParams[m_subNames[2]].push_back(DamageParam((616 + 678) / 8, 512, 2.1886 / 4));
    m_damageParams[m_subNames[2]].push_back(DamageParam((690 + 760) / 8, 512, 2.3200 / 4));
}

void BaHuangGuiYuan::SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings, TableRes tableRes)
{
    if (tableRes != TableRes::MISS) {
        TableRes tableRes2 = GetPhysicsRollResult(*targetsMap[NORMAL].front(), RollType::SUB);
        UpdatePhysicsStats(*targetsMap[NORMAL].front(), stats, settings, tableRes2, m_subNames[1], 9 - static_cast<int>(targetsMap[NORMAL].front()->GetLifePercent() * 10));
        if (targetsMap[NORMAL].front()->buffs[TBUF_DIE_REN]->IsExist()) {
            if (m_player->talents[HE_GUANG]) {
                TableRes tableResHeGuang = GetPhysicsRollResult(*targetsMap[NORMAL].front());
                UpdatePhysicsStats(*targetsMap[NORMAL].front(), stats, settings, tableResHeGuang, m_subNames[2], 9 - static_cast<int>(targetsMap[NORMAL].front()->GetLifePercent() * 10));
            }
            if (targetsMap[NORMAL].front()->GetLifePercent() < 0.4 && m_player->talents[QIE_YU]) {
                targetsMap[NORMAL].front()->buffs[TBUF_DIE_REN]->Clean(targetsMap, stats, settings, tableRes);
            }
        }
        static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(m_skillQidianAdd);
        if (m_player->talents[SHEN_MAI] && tableRes == TableRes::DOUBLE) {
            static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(2);
        }
        if (m_player->GetGearSetCW() && RandBetween(0.0, 1.0) < 0.1) {
            m_player->buffs[BUF_CLASS_CW]->Refresh();
        }
    }
    if (m_player->buffs[BUF_CLASS_CW]->IsExist()) {
        m_cooldown = 0;
    }
    if (m_player->talents[FENG_SHI]) {
        m_player->buffs[BUF_FENG_SHI]->Refresh();
    }
    if (m_player->GetGearSetAttack() && RandBetween(0.0, 1.0) < 0.1) {
        m_player->buffs[BUF_CLASS_ATTACK]->Refresh();
    }
}

}

}
