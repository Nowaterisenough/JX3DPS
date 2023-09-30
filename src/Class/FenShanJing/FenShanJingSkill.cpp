/**
 * Project: JX3DPS
 * File: FenShanJingSkill.cpp
 * Description:
 * Created Date: 2023-08-25 20:44:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-30 12:03:49
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "FenShanJingSkill.h"

#include "Buff.h"
#include "FenShanJingBuff.h"
#include "Target.hpp"

namespace JX3DPS {

namespace FenShanJing {

namespace Skill {

PoZhao::PoZhao(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id    = SKILL_PO_ZHAO;
    m_name  = "破招";
    m_range = 4;

    m_damageParams[0].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.065 - 1));
    m_damageParams[0].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.13 - 1));
    m_damageParams[0].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.23 - 1));

    m_damageParams[1].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.065 - 1));
    m_damageParams[1].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.065 - 1));
    m_damageParams[1].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.065 - 1));
}

void PoZhao::Cast() { }

void PoZhao::Trigger() { }

JieDao::JieDao(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                   = SKILL_JIE_DAO;
    m_name                 = "劫刀";
    m_range                = 4;
    m_cooldown             = 24;
    m_cooldownCurrent      = 0;
    m_effectRageAdditional = -10;

    m_damageParams[0].emplace_back((110 + 139) / 2, 1024, 246);

    if (m_player->recipes[RECIPE_JIE_DAO_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_JIE_DAO_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_JIE_DAO_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_JIE_DAO_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_JIE_DAO_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_JIE_DAO_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }

    if (m_player->recipes[RECIPE_JIE_DAO_RAGE]) {
        m_effectRageAdditional += 5;
    }
}

void JieDao::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JieDao::Trigger() { }

void JieDao::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id, m_player->GetTargetId(), rollResult, damage, 0, 0);
}

ZhanDao::ZhanDao(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                   = SKILL_ZHAN_DAO;
    m_name                 = "斩刀";
    m_range                = 20;
    m_cooldown             = 10 * 16;
    m_cooldownCurrent      = 0;
    m_effectRageAdditional = -15;

    m_damageParams[0].emplace_back((228 + 238) / 2, 1024, 371);

    if (m_player->recipes[RECIPE_ZHAN_DAO_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_ZHAN_DAO_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_ZHAN_DAO_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_ZHAN_DAO_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_ZHAN_DAO_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_ZHAN_DAO_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }
}

void ZhanDao::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void ZhanDao::Trigger() { }

void ZhanDao::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

JueDao::JueDao(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                   = SKILL_JUE_DAO;
    m_name                 = "绝刀";
    m_range                = 6;
    m_cooldown             = 10 * 16;
    m_cooldownCurrent      = 0;
    m_effectRageAdditional = -25;

    m_damageParams[0].emplace_back((180 + 192) / 2, 1024, 317);

    if (m_player->recipes[RECIPE_JUE_DAO_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_JUE_DAO_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_JUE_DAO_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_JUE_DAO_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }
}

void JueDao::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JueDao::Trigger() { }

void JueDao::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

ShanDao::ShanDao(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                   = SKILL_SHAN_DAO;
    m_name                 = "闪刀";
    m_range                = 6;
    m_cooldown             = 8 * 16;
    m_cooldownCurrent      = 0;
    m_effectRageAdditional = -15;

    m_damageParams[0].emplace_back((180 + 192) / 2, 1024, 317);
}

void ShanDao::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void ShanDao::Trigger() { }

void ShanDao::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

DunYa::DunYa(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                   = SKILL_DUN_YA;
    m_name                 = "盾压";
    m_range                = 4;
    m_cooldown             = 12 * 16;
    m_cooldownCurrent      = 0;
    m_effectRageAdditional = 15;

    m_damageParams[0].emplace_back((202 + 220) / 2, 1024, 241);

    if (m_player->recipes[RECIPE_DUN_YA_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_DUN_YA_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_DUN_YA_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_DUN_YA_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }

    if (m_player->recipes[RECIPE_DUN_YA_COOLDOWN_1]) {
        m_cooldown -= 16;
    }

    if (m_player->recipes[RECIPE_DUN_YA_COOLDOWN_2]) {
        m_cooldown -= 16;
    }

    if (m_player->recipes[RECIPE_DUN_YA_EFFECT_1]) {
        m_cooldown -= 16;
    }

    if (m_player->recipes[RECIPE_DUN_YA_EFFECT_2]) {
        m_cooldown -= 16;
    }
}

void DunYa::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void DunYa::Trigger() { }

void DunYa::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

DunJi::DunJi(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                 = SKILL_DUN_JI;
    m_name               = "盾击";
    m_range              = 4;
    m_cooldown           = 4 * 16;
    m_cooldownCurrent    = 0;
    m_energyCountCurrent = m_energyCount = 3;
    m_effectRageAdditional               = 10;

    m_damageParams[0].emplace_back((202 + 220) / 2, 1024, 241);
}

void DunJi::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void DunJi::Trigger() { }

void DunJi::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

DunMeng::DunMeng(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                 = SKILL_DUN_MENG;
    m_name               = "盾猛";
    m_range              = 4;
    m_cooldown           = 4 * 16;
    m_cooldownCurrent    = 0;
    m_energyCountCurrent = m_energyCount = 3;
    m_effectRageAdditional               = 10;

    m_damageParams[0].emplace_back((202 + 220) / 2, 1024, 241);
}

void DunMeng::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void DunMeng::Trigger() { }

void DunMeng::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

DunDao::DunDao(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                 = SKILL_DUN_DAO;
    m_name               = "盾刀";
    m_range              = 4;
    m_cooldown           = 4 * 16;
    m_cooldownCurrent    = 0;
    m_energyCountCurrent = m_energyCount = 3;
    m_effectRageAdditional               = 10;

    m_damageParams[0].emplace_back((202 + 220) / 2, 1024, 241);

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_RAGE]) {
        m_effectRageAdditional += 5;
    }
}

void DunDao::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void DunDao::Trigger() { }

void DunDao::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

DunFei::DunFei(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                 = SKILL_DUN_DAO;
    m_name               = "盾刀";
    m_range              = 4;
    m_cooldown           = 4 * 16;
    m_cooldownCurrent    = 0;
    m_energyCountCurrent = m_energyCount = 3;
    m_effectRageAdditional               = 10;

    m_damageParams[0].emplace_back((202 + 220) / 2, 1024, 241);

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_RAGE]) {
        m_effectRageAdditional += 5;
    }
}

void DunFei::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void DunFei::Trigger() { }

void DunFei::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

DunWu::DunWu(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                 = SKILL_DUN_DAO;
    m_name               = "盾刀";
    m_range              = 4;
    m_cooldown           = 4 * 16;
    m_cooldownCurrent    = 0;
    m_energyCountCurrent = m_energyCount = 3;
    m_effectRageAdditional               = 10;

    m_damageParams[0].emplace_back((202 + 220) / 2, 1024, 241);

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_RAGE]) {
        m_effectRageAdditional += 5;
    }
}

void DunWu::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void DunWu::Trigger() { }

void DunWu::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

XueNu::XueNu(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                 = SKILL_DUN_DAO;
    m_name               = "盾刀";
    m_range              = 4;
    m_cooldown           = 4 * 16;
    m_cooldownCurrent    = 0;
    m_energyCountCurrent = m_energyCount = 3;
    m_effectRageAdditional               = 10;

    m_damageParams[0].emplace_back((202 + 220) / 2, 1024, 241);

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }

    if (m_player->recipes[RECIPE_DUN_DAO_RAGE]) {
        m_effectRageAdditional += 5;
    }
}

void XueNu::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void XueNu::Trigger() { }

void XueNu::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

YeHuoLinGuang::YeHuoLinGuang(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                 = SKILL_YE_HUO_LIN_GUANG;
    m_name               = "业火麟光";
    m_range              = 4;
    m_cooldown           = 4 * 16;
    m_cooldownCurrent    = 0;
    m_energyCountCurrent = m_energyCount = 3;
    m_effectRageAdditional               = 10;

    m_damageParams[0].emplace_back((202 + 220) / 2, 1024, 241);
}

void YeHuoLinGuang::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void YeHuoLinGuang::Trigger() { }

void YeHuoLinGuang::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

ZhenYunJieHui::ZhenYunJieHui(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                 = SKILL_ZHEN_YUN_JIE_HUI;
    m_name               = "阵云结晦";
    m_range              = 4;
    m_cooldown           = 4 * 16;
    m_cooldownCurrent    = 0;
    m_energyCountCurrent = m_energyCount = 3;
    m_effectRageAdditional               = 10;

    m_damageParams[0].emplace_back((202 + 220) / 2, 1024, 241);
}

void ZhenYunJieHui::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void ZhenYunJieHui::Trigger() { }

void ZhenYunJieHui::SubEffect()
{
    m_player->AddRage(m_effectRageAdditional);
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id,m_player->GetTargetId(), rollResult, damage, 0, 0);
}

} // namespace Skill

} // namespace FenShanJing

} // namespace JX3DPS