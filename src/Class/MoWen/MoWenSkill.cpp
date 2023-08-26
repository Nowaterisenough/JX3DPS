/**
 * Project: JX3DPS
 * File: MoWenSkill.cpp
 * Description:
 * Created Date: 2023-07-31 16:30:22
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-26 09:55:01
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "MoWenSkill.h"

#include "MoWenBuff.h"
#include "Target.hpp"

namespace JX3DPS {

namespace MoWen {

namespace Skill {

PoZhao::PoZhao(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id    = SKILL_PO_ZHAO;
    m_name  = "破招";
    m_range = 20;

    m_damageParams[0].emplace_back(0,
                                   0,
                                   JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE *
                                       (0.25 * 0.5 * 1.3 * 1.2 * 0.5 * 1.11 - 1));
    m_damageParams[0].emplace_back(0,
                                   0,
                                   JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE *
                                       (0.3 * 0.5 * 1.3 * 1.2 * 0.5 * 1.11 - 1));
    m_damageParams[0].emplace_back(0,
                                   0,
                                   JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE *
                                       (0.36 * 0.5 * 1.3 * 1.2 * 0.5 * 1.11 - 1));
}

void PoZhao::Cast() { }

void PoZhao::Trigger() { }

void PoZhao::TriggerDamage(Id_t targetId)
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_LIU_ZHAO_SURPLUS_DAMAGE](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicSurplusDamage(targetId, rollResult, 0, 0);
    Record(targetId, rollResult, damage, 0, 0);

    rollResult = GetMagicRollResult();
    damage     = CalcMagicSurplusDamage(targetId, rollResult, 0, 1);
    Record(targetId, rollResult, damage, 0, 1);

    rollResult = GetMagicRollResult();
    damage     = CalcMagicSurplusDamage(targetId, rollResult, 0, 2);
    Record(targetId, rollResult, damage, 0, 2);
}

Gong::Gong(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id            = SKILL_GONG;
    m_name          = "宫";
    m_range         = 20;
    m_prepareFrames = 28;

    m_damageParams[0].emplace_back((160 + 160 + 40) / 2, 0, 277);

    if (m_player->recipes[RECIPE_GONG_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_GONG_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_GONG_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_GONG_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_GONG_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_GONG_PREPARE_1]) {
        m_prepareFrames -= 2;
    }

    if (m_player->recipes[RECIPE_GONG_PREPARE_2]) {
        m_prepareFrames -= 2;
    }

    if (m_player->recipes[RECIPE_GONG_RANGE]) {
        m_range += 2;
    }

    if (m_player->talents[TALENT_HAO_ZHONG]) {
        m_effectCriticalStrikeAdditionalBasisPointInt   += 1000;
        m_effectCriticalStrikePowerAdditionalPercentInt += 102;
    }
}

void Gong::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_player->SetCast(true);
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<BianGong *>(m_player->skills[SKILL_BIAN_GONG])->Sync();
}

void Gong::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
        static_cast<MoWen::Buff::QuFeng *>(m_player->buffs[BUFF_QU_FENG])->TriggerAdd(3);
        static_cast<BianGong *>(m_player->skills[SKILL_BIAN_GONG])->Sync();
        m_player->SetCast(false);
        SubEffect();
    }
}

bool Gong::IsReady(bool fcast)
{
    if (m_player->IsCast()) {
        return false;
    }
    if (!m_player->IsReCast() || fcast) {
        if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::YANG_CHUN_BAI_XUE)
        {
            return true;
        }
        return false;
    }
    return false;
}

void Gong::Sync()
{
    Frame_t cooldownCurrent =
        static_cast<BianGong *>(m_player->skills[SKILL_BIAN_GONG])->GetEnergyCooldownCurrent();
    m_cooldownCurrent = cooldownCurrent;
}

void Gong::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);
    m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    static_cast<Buff::YangChunBaiXue *>(m_player->buffs[BUFF_YANG_CHUN_BAI_XUE])->TriggerClear();
}

BianGong::BianGong(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id            = SKILL_BIAN_GONG;
    m_name          = "变宫";
    m_range         = 20;
    m_prepareFrames = 28;

    m_damageParams[0].emplace_back((160 + 160 + 40) / 2, 0, 277);

    if (m_player->recipes[RECIPE_GONG_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_GONG_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_GONG_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_GONG_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_GONG_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_GONG_PREPARE_1]) {
        m_prepareFrames -= 2;
    }

    if (m_player->recipes[RECIPE_GONG_PREPARE_2]) {
        m_prepareFrames -= 2;
    }

    if (m_player->recipes[RECIPE_GONG_RANGE]) {
        m_range += 2;
    }

    if (m_player->talents[TALENT_HAO_ZHONG]) {
        m_effectCriticalStrikeAdditionalBasisPointInt   += 1000;
        m_effectCriticalStrikePowerAdditionalPercentInt += 102;
    }
}

void BianGong::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<Gong *>(m_player->skills[SKILL_GONG])->Sync();
    m_player->SetCast(true);
}

void BianGong::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
        m_player->SetCast(false);

        static_cast<MoWen::Buff::QuFeng *>(m_player->buffs[BUFF_QU_FENG])->TriggerAdd(4);
        static_cast<Gong *>(m_player->skills[SKILL_GONG])->Sync();

        SubEffect();
    }
}

bool BianGong::IsReady(bool fcast)
{
    if (m_player->IsCast()) {
        return false;
    }
    if (!m_player->IsReCast() || fcast) {
        if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::GAO_SHAN_LIU_SHUI)
        {
            return true;
        }
        return false;
    }
    return false;
}

void BianGong::Sync()
{
    Frame_t cooldownCurrent =
        static_cast<Gong *>(m_player->skills[SKILL_GONG])->GetEnergyCooldownCurrent();
    m_cooldownCurrent = cooldownCurrent;
}

void BianGong::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);

    params.targetId = m_player->GetTargetId();
    m_triggerEffects[TRIGGER_ZHI_ZHI](params);
    m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    static_cast<Buff::YangChunBaiXue *>(m_player->buffs[BUFF_YANG_CHUN_BAI_XUE])->TriggerClear();
}

Shang::Shang(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id    = SKILL_SHANG;
    m_name  = "商";
    m_range = 20;

    m_damageParams[0].emplace_back((14 + 14 + 5) / 2, 0, 80);

    if (m_player->recipes[RECIPE_SHANG_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_SHANG_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_SHANG_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_SHANG_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_SHANG_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_SHANG_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }
}

void Shang::Cast()
{
    m_player->SetLastCastSkill(m_id);
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void Shang::Trigger() { }

void Shang::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);
    m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    static_cast<MoWen::Buff::QuFeng *>(m_player->buffs[BUFF_QU_FENG])
        ->TriggerAdd(2 + static_cast<int>(static_cast<MoWen::Player *>(m_player)->style));

    static_cast<Buff::Shang *>(m_player->buffs[BUFF_SHANG])->TriggerAdd(m_player->GetTargetId());
}

Jue::Jue(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id    = SKILL_JUE;
    m_name  = "角";
    m_range = 20;

    m_damageParams[0].emplace_back((14 + 14 + 5) / 2, 0, 80);
}

void Jue::Cast()
{
    m_player->SetLastCastSkill(m_id);
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void Jue::Trigger() { }

void Jue::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);
    m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    static_cast<MoWen::Buff::QuFeng *>(m_player->buffs[BUFF_QU_FENG])
        ->TriggerAdd(1 + static_cast<int>(static_cast<MoWen::Player *>(m_player)->style));

    static_cast<Buff::Jue *>(m_player->buffs[BUFF_JUE])->TriggerAdd(m_player->GetTargetId());
}

Zhi::Zhi(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id            = SKILL_ZHI;
    m_name          = "徵";
    m_range         = 20;
    m_cooldown      = 0;
    m_prepareFrames = 16;

    m_damageParams[0].emplace_back((190 + 190 + 20) / 2, 0, 124);

    if (m_player->recipes[RECIPE_ZHI_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_ZHI_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_ZHI_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_ZHI_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_ZHI_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_ZHI_RANGE]) {
        m_range += 2;
    }

    if (m_player->talents[TALENT_FEI_FAN]) {
        m_effectDamageAdditionalPercentInt += 152;
    }

    if (m_player->talents[TALENT_LIU_ZHAO]) {
        m_energyCountCurrent = m_energyCount = 3;
        m_cooldown                           = 8 * 16;
        m_cooldownCurrent                    = JX3DPS_INVALID_FRAMES_SET;
    }
}

void Zhi::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_player->SetReCast(true);
    m_player->SetReCastSkill(m_id);
    if (m_energyCountCurrent == m_energyCount) {
        m_cooldownCurrent = m_cooldown;
    }
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    if (m_energyCount == 3) {
        m_energyCountCurrent--;
    }
    static_cast<BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->Sync();

    static_cast<MoWen::Buff::QuFeng *>(m_player->buffs[BUFF_QU_FENG])->TriggerAdd(5);

    static_cast<MoWen::Buff::YangChunBaiXue *>(m_player->buffs[BUFF_YANG_CHUN_BAI_XUE])->TriggerClear();

    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);
    xianFengStackNum = m_player->buffs[BUFF_XIAN_FENG]->GetStackNumCurrent();
    m_triggerEffects[TRIGGER_HAO_QING_ZHI](params);
    m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

    m_triggerEffects[TRIGGER_XIAN_FENG_CLEAR](params);
}

void Zhi::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
        if (index < 2) {
            index++;
            m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
        } else {
            m_player->SetReCast(false);

            index                  = 0;
            m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
            static_cast<Buff::YangChunBaiXue *>(m_player->buffs[BUFF_YANG_CHUN_BAI_XUE])->TriggerClear();
        }
    }
    if (m_cooldownCurrent == 0 && m_energyCount == 3) {
        m_energyCountCurrent++;
        m_energyCountCurrent = std::min(m_energyCountCurrent, m_energyCount);
        m_cooldownCurrent    = m_cooldown;
        if (m_energyCountCurrent == m_energyCount) {
            m_cooldownCurrent = JX3DPS_INVALID_FRAMES_SET;
        }
    }
    static_cast<BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->Sync();
}

void Zhi::Stop()
{
    m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
}

bool Zhi::IsReady(bool fcast)
{
    if (m_player->IsCast()) {
        return false;
    }
    if (!m_player->IsReCast() || fcast) {
        if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::YANG_CHUN_BAI_XUE)
        {
            return true;
        }
        return false;
    }
    return false;
}

void Zhi::Sync()
{
    Frame_t cooldownCurrent =
        static_cast<BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->GetEnergyCooldownCurrent();
    m_cooldownCurrent = cooldownCurrent;

    int energyCountCurrent =
        static_cast<BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->GetEnergyCountCurrent();
    m_energyCountCurrent = energyCountCurrent;
}

void Zhi::TriggerShiXiang()
{
    m_cooldownCurrent -= 5 * 16;
    if (m_cooldownCurrent <= 0) {
        m_energyCountCurrent++;
        m_cooldownCurrent += m_cooldown;
        if (m_energyCountCurrent >= m_energyCount) {
            m_energyCountCurrent = m_energyCount;
            m_cooldownCurrent    = JX3DPS_INVALID_FRAMES_SET;
        }
    }
}

void Zhi::SubEffect()
{
    Params params;
    params.player   = m_player;
    params.stackNum = xianFengStackNum;
    params.targetId = m_player->GetTargetId();
    m_triggerEffects[TRIGGER_XIAN_FENG_DAMAGE](params);

    m_triggerEffects[TRIGGER_LIU_ZHAO_DAMAGE](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);
}

BianZhi::BianZhi(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id            = SKILL_BIAN_ZHI;
    m_name          = "变徵";
    m_range         = 20;
    m_cooldown      = 0;
    m_prepareFrames = 16;

    m_damageParams[0].emplace_back((190 + 190 + 20) / 2, 0, 124);

    if (m_player->recipes[RECIPE_ZHI_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_ZHI_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_ZHI_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_ZHI_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_ZHI_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->recipes[RECIPE_ZHI_RANGE]) {
        m_range += 2;
    }

    if (m_player->talents[TALENT_FEI_FAN]) {
        m_effectDamageAdditionalPercentInt += 152;
    }

    if (m_player->talents[TALENT_LIU_ZHAO]) {
        m_energyCountCurrent = m_energyCount = 3;
        m_cooldown                           = 8 * 16;
        m_cooldownCurrent                    = JX3DPS_INVALID_FRAMES_SET;
    }
}

void BianZhi::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_player->SetReCast(true);
    m_player->SetReCastSkill(m_id);
    if (m_energyCountCurrent == m_energyCount) {
        m_cooldownCurrent = m_cooldown;
    }
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    if (m_energyCount == 3) {
        m_energyCountCurrent--;
    }
    static_cast<Zhi *>(m_player->skills[SKILL_ZHI])->Sync();

    static_cast<MoWen::Buff::QuFeng *>(m_player->buffs[BUFF_QU_FENG])->TriggerAdd(6);

    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);
    xianFengStackNum = m_player->buffs[BUFF_XIAN_FENG]->GetStackNumCurrent();
    m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

    m_triggerEffects[TRIGGER_HAO_QING_BIAN_ZHI](params);

    m_triggerEffects[TRIGGER_XIAN_FENG_CLEAR](params);
}

void BianZhi::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
        if (index < 2) {
            index++;
            m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
        } else {
            m_player->SetReCast(false);

            index                  = 0;
            m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;

            Params params;
            params.player = m_player;
            m_triggerEffects[TRIGGER_XIAN_FENG_CLEAR](params);
        }
    }
    if (m_cooldownCurrent == 0 && m_energyCount == 3) {
        m_energyCountCurrent++;
        m_energyCountCurrent = std::min(m_energyCountCurrent, m_energyCount);
        m_cooldownCurrent    = m_cooldown;
        if (m_energyCountCurrent == m_energyCount) {
            m_cooldownCurrent = JX3DPS_INVALID_FRAMES_SET;
        }
    }
    static_cast<Zhi *>(m_player->skills[SKILL_ZHI])->Sync();
}

void BianZhi::Stop()
{
    m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
}

bool BianZhi::IsReady(bool fcast)
{
    if (m_player->IsCast()) {
        return false;
    }
    if (!m_player->IsReCast() || fcast) {
        if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::GAO_SHAN_LIU_SHUI)
        {
            return true;
        }
        return false;
    }
    return false;
}

void BianZhi::Sync()
{
    Frame_t cooldownCurrent =
        static_cast<Zhi *>(m_player->skills[SKILL_ZHI])->GetEnergyCooldownCurrent();
    m_cooldownCurrent = cooldownCurrent;

    int energyCountCurrent =
        static_cast<Zhi *>(m_player->skills[SKILL_ZHI])->GetEnergyCountCurrent();
    m_energyCountCurrent = energyCountCurrent;
}

void BianZhi::TriggerShiXiang()
{
    m_cooldownCurrent -= 5 * 16;
    if (m_cooldownCurrent <= 0) {
        m_energyCountCurrent++;
        m_cooldownCurrent += m_cooldown;
        if (m_energyCountCurrent >= m_energyCount) {
            m_energyCountCurrent = m_energyCount;
            m_cooldownCurrent    = JX3DPS_INVALID_FRAMES_SET;
        }
    }
}

void BianZhi::SubEffect()
{
    Params params;
    params.player   = m_player;
    params.stackNum = xianFengStackNum;
    params.targetId = m_player->GetTargetId();
    m_triggerEffects[TRIGGER_XIAN_FENG_DAMAGE](params);

    m_triggerEffects[TRIGGER_LIU_ZHAO_DAMAGE](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);
}

Yu::Yu(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id                 = SKILL_YU;
    m_name               = "羽";
    m_range              = 20;
    m_cooldown           = 6 * 16;
    m_cooldownCurrent    = JX3DPS_INVALID_FRAMES_SET;
    m_energyCountCurrent = m_energyCount = 3;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);

    if (m_player->recipes[RECIPE_YU_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_YU_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }

    if (m_player->recipes[RECIPE_YU_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->recipes[RECIPE_YU_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->talents[TALENT_SHI_XIANG]) {
        m_effectShieldIgnoreAdditionalPercentInt += 614;
    }

    if (m_player->talents[TALENT_SHU_LI]) {
        m_cooldown += 5 * 16;
    }
}

void Yu::Cast()
{
    m_player->SetLastCastSkill(m_id);
    if (m_energyCountCurrent == m_energyCount) {
        m_cooldownCurrent = m_cooldown;
    }
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    m_energyCountCurrent--;

    SubEffect();
}

void Yu::Trigger()
{
    if (m_cooldownCurrent == 0) {
        m_energyCountCurrent++;
        m_energyCountCurrent = std::min(m_energyCountCurrent, m_energyCount);
        m_cooldownCurrent    = m_cooldown;
        if (m_energyCountCurrent == m_energyCount) {
            m_cooldownCurrent = JX3DPS_INVALID_FRAMES_SET;
        }
    }
}

void Yu::TriggerShiXiang()
{
    m_cooldownCurrent -= 5 * 16;
    if (m_cooldownCurrent <= 0) {
        m_energyCountCurrent++;
        m_cooldownCurrent += m_cooldown;
        if (m_energyCountCurrent >= m_energyCount) {
            m_energyCountCurrent = m_energyCount;
            m_cooldownCurrent    = JX3DPS_INVALID_FRAMES_SET;
        }
    }
}

void Yu::SubEffect()
{
    Params params;
    params.player = m_player;

    static_cast<MoWen::Buff::QuFeng *>(m_player->buffs[BUFF_QU_FENG])
        ->TriggerAdd(4 + static_cast<int>(static_cast<MoWen::Player *>(m_player)->style));

    m_triggerEffects[TRIGGER_XIAN_FENG](params);
    m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

    m_triggerEffects[TRIGGER_SHI_XIANG](params);

    m_triggerEffects[TRIGGER_SHU_LI](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);
}

GaoShanLiuShui::GaoShanLiuShui(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                    = SKILL_GAO_SHAN_LIU_SHUI;
    m_name                  = "高山流水";
    m_range                 = JX3DPS_UNLIMITED_RANGE;
    m_globalCooldownCurrent = &m_noneGlobalCooldown;
    m_prepareFrames         = 6;
}

void GaoShanLiuShui::Cast()
{
    m_player->SetLastCastSkill(m_id);
    if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::GAO_SHAN_LIU_SHUI)
    { // 高山主动
        *m_globalCooldownCurrent = 50 * 16;
        SubEffect();
    } else { // 切曲风
        m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownGaoShanLiuShuiCurrent;
        static_cast<YangChunBaiXue *>(m_player->skills[SKILL_YANG_CHUN_BAI_XUE])->SetCooldown();
        m_prepareFramesCurrent = m_prepareFrames;

        if (m_player->buffs[BUFF_ZHENG_LV_HE_MING]->GetDurationCurrent() == 0) {
            static_cast<MoWen::Buff::QuFeng *>(m_player->buffs[BUFF_QU_FENG])->TriggerSet(0);
        }

        m_player->SetCast(true);

        static_cast<BianGong *>(m_player->skills[SKILL_BIAN_GONG])->Sync();
        static_cast<BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->Sync();
    }
}

void GaoShanLiuShui::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        m_player->SetCast(false);
        m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
        static_cast<MoWen::Player *>(m_player)->style = MoWen::Player::Style::GAO_SHAN_LIU_SHUI;

        Params params;
        params.player = m_player;
        m_triggerEffects[TRIGGER_CAN_LIAN_CLEAR](params);

        m_triggerEffects[TRIGGER_XIAN_FENG_BIAO_JI](params);
    }
}

void GaoShanLiuShui::SetCooldown()
{
    m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownStyleCurrent;
    *m_globalCooldownCurrent = 3 * 16;
}

void GaoShanLiuShui::SubEffect()
{
    static_cast<Buff::GaoShanLiuShui *>(m_player->buffs[BUFF_GAO_SHAN_LIU_SHUI])->TriggerAdd();
}

YangChunBaiXue::YangChunBaiXue(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                    = SKILL_YANG_CHUN_BAI_XUE;
    m_name                  = "阳春白雪";
    m_range                 = JX3DPS_UNLIMITED_RANGE;
    m_cooldown              = 3 * 16;
    m_prepareFrames         = 6;
    m_globalCooldownCurrent = &m_noneGlobalCooldown;

    m_damageParams[0].emplace_back((77 + 77 + 7) / 2, 0, 89);

    if (m_player->talents[TALENT_CAN_LIAN]) {
        m_cooldown -= 2 * 16;
    }
}

void YangChunBaiXue::Cast()
{
    m_player->SetLastCastSkill(m_id);
    if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::YANG_CHUN_BAI_XUE)
    { // 阳春主动
        *m_globalCooldownCurrent = m_cooldown;
        SubEffect();
    } else { // 切曲风
        static_cast<YangChunBaiXue *>(m_player->skills[SKILL_GAO_SHAN_LIU_SHUI])->SetCooldown();
        m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownYangChunBaiXueCurrent;
        m_prepareFramesCurrent = m_prepareFrames;
        static_cast<Buff::GaoShanLiuShui *>(m_player->buffs[BUFF_GAO_SHAN_LIU_SHUI])->TriggerClear();

        m_player->SetCast(true);

        static_cast<Gong *>(m_player->skills[SKILL_GONG])->Sync();
        static_cast<Zhi *>(m_player->skills[SKILL_ZHI])->Sync();
    }
}

void YangChunBaiXue::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        m_player->SetCast(false);
        m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
        static_cast<MoWen::Player *>(m_player)->style = MoWen::Player::Style::YANG_CHUN_BAI_XUE;

        Params params;
        params.player = m_player;
        m_triggerEffects[TRIGGER_CAN_LIAN_ADD](params);

        m_triggerEffects[TRIGGER_XIAN_FENG_BIAO_JI](params);
    }
}

bool YangChunBaiXue::IsReady(bool fcast)
{
    if (m_player->IsCast()) {
        return false;
    }
    if (m_player->IsReCast()) {
        if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::YANG_CHUN_BAI_XUE || fcast)
        {
            return true;
        }
    } else if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::GAO_SHAN_LIU_SHUI)
    {
        return true;
    }
    return false;
}

bool YangChunBaiXue::StopReCastSkill()
{
    if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::YANG_CHUN_BAI_XUE)
    {
        return false;
    }
    m_player->skills[m_player->GetReCastSkill()]->Stop();
    return true;
}

void YangChunBaiXue::SetCooldown()
{
    m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownStyleCurrent;
    *m_globalCooldownCurrent = 3 * 16;
}

void YangChunBaiXue::SubEffect()
{
    int level = 0;
    if ((*m_targets)[m_player->GetTargetId()]->GetLifePercent() - 0.01 < 0.5) {
        m_effectDamageAdditionalPercentInt += 410;
        level                               = 1;
    }
    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, level);
    Record(m_player->GetTargetId(), rollResult, damage, 0, level);
    if ((*m_targets)[m_player->GetTargetId()]->GetLifePercent() - 0.01 < 0.5) {
        m_effectDamageAdditionalPercentInt -= 410;
    }
    static_cast<MoWen::Buff::YangChunBaiXue *>(m_player->buffs[BUFF_YANG_CHUN_BAI_XUE])->TriggerAdd();
}

ShuYingHengXie::ShuYingHengXie(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                    = SKILL_SHU_YING_HENG_XIE;
    m_name                  = "疏影横斜";
    m_range                 = 20;
    m_cooldown              = 20 * 16;
    m_cooldownCurrent       = JX3DPS_INVALID_FRAMES_SET;
    m_globalCooldownCurrent = &m_noneGlobalCooldown;
    m_energyCountCurrent = m_energyCount = 3;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);

    m_effectDamageAdditionalPercentInt = 20;
    if (m_player->talents[TALENT_KE_MENG]) {
        m_effectDamageAdditionalPercentInt = 100;
    }
}

void ShuYingHengXie::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownShuYingHengXieCurrent;
    *m_globalCooldownCurrent = m_player->globalCooldown * m_player->attribute.GetHastePercent();
    if (m_energyCountCurrent == m_energyCount) {
        m_cooldownCurrent = m_cooldown;
    }
    m_energyCountCurrent--;
    SubEffect();
}

void ShuYingHengXie::Trigger()
{
    if (m_cooldownCurrent == 0) {
        m_energyCountCurrent++;
        m_energyCountCurrent = std::min(m_energyCountCurrent, m_energyCount);
        m_cooldownCurrent    = m_cooldown;
        if (m_energyCountCurrent == m_energyCount) {
            m_cooldownCurrent = JX3DPS_INVALID_FRAMES_SET;
        }
    }
}

bool ShuYingHengXie::IsReady(bool fcast)
{
    if (m_player->IsCast()) {
        return false;
    }
    if (!m_player->IsReCast() || fcast) {
        if (static_cast<Buff::YingZi *>(m_player->buffs[BUFF_YING_ZI])->ids.size() < 6) {
            return true;
        }
    }
    return false;
}

void ShuYingHengXie::SubEffect()
{
    static_cast<Buff::YingZi *>(m_player->buffs[BUFF_YING_ZI])->TriggerAdd();
    if (RandomUniform(1, 100) <= m_effectDamageAdditionalPercentInt) {
        static_cast<Buff::YingZi *>(m_player->buffs[BUFF_YING_ZI])->TriggerAdd();
    }
}

GuYingHuaShuang::GuYingHuaShuang(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                    = SKILL_GU_YING_HUA_SHUANG;
    m_name                  = "孤影化双";
    m_range                 = JX3DPS_UNLIMITED_RANGE;
    m_cooldown              = 150 * 16;
    m_globalCooldownCurrent = &m_noneGlobalCooldown;
}

void GuYingHuaShuang::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownGuYingHuaShuangCurrent;
    if (index == 0) {
        Snap();
        *m_globalCooldownCurrent = 16;
        index++;
    } else {
        RestoreFromSnap();
    }
}

void GuYingHuaShuang::Trigger() { }

void GuYingHuaShuang::Snap()
{
    static_cast<Buff::GuYingHuaShuang *>(m_player->buffs[BUFF_GU_YING_HUA_SHUANG])->TriggerAdd();
    for (int i = SKILL_GONG; i <= SKILL_SHU_YING_HENG_XIE; ++i) {
        snaps[static_cast<Id_t>(i)].cooldownCurrent =
            m_player->skills[static_cast<Id_t>(i)]->GetEnergyCooldownCurrent();
        snaps[static_cast<Id_t>(i)].energyCountCurrent =
            m_player->skills[static_cast<Id_t>(i)]->GetEnergyCountCurrent();
    }

    if (m_player->skills.find(SKILL_ZHENG_LV_HE_MING_1) != m_player->skills.end()) {
        snaps[SKILL_ZHENG_LV_HE_MING_1].cooldownCurrent =
            m_player->skills[SKILL_ZHENG_LV_HE_MING_1]->GetEnergyCooldownCurrent();
        snaps[SKILL_ZHENG_LV_HE_MING_1].energyCountCurrent =
            m_player->skills[SKILL_ZHENG_LV_HE_MING_1]->GetEnergyCountCurrent();

        snaps[SKILL_ZHENG_LV_HE_MING_2].cooldownCurrent =
            m_player->skills[SKILL_ZHENG_LV_HE_MING_2]->GetEnergyCooldownCurrent();
        snaps[SKILL_ZHENG_LV_HE_MING_2].energyCountCurrent =
            m_player->skills[SKILL_ZHENG_LV_HE_MING_2]->GetEnergyCountCurrent();

        snaps[SKILL_ZHENG_LV_HE_MING_3].cooldownCurrent =
            m_player->skills[SKILL_ZHENG_LV_HE_MING_3]->GetEnergyCooldownCurrent();
        snaps[SKILL_ZHENG_LV_HE_MING_3].energyCountCurrent =
            m_player->skills[SKILL_ZHENG_LV_HE_MING_3]->GetEnergyCountCurrent();
    }

    cooldownStyleCurrent = static_cast<MoWen::Player *>(m_player)->cooldownStyleCurrent;
    cooldownGaoShanLiuShuiCurrent = static_cast<MoWen::Player *>(m_player)->cooldownGaoShanLiuShuiCurrent;
    cooldownYangChunBaiXueCurrent = static_cast<MoWen::Player *>(m_player)->cooldownYangChunBaiXueCurrent;
    cooldownShuYingHengXieCurrent = static_cast<MoWen::Player *>(m_player)->cooldownShuYingHengXieCurrent;
}

void GuYingHuaShuang::RestoreFromSnap()
{
    static_cast<Buff::GuYingHuaShuang *>(m_player->buffs[BUFF_GU_YING_HUA_SHUANG])->TriggerClear();
    for (auto &[id, snap] : snaps) {
        m_player->skills[id]->SetEnergyCooldownCurrent(snap.cooldownCurrent);
        m_player->skills[id]->SetEnergyCountCurrent(snap.energyCountCurrent);
    }

    static_cast<MoWen::Player *>(m_player)->cooldownStyleCurrent = cooldownStyleCurrent;
    static_cast<MoWen::Player *>(m_player)->cooldownGaoShanLiuShuiCurrent = cooldownGaoShanLiuShuiCurrent;
    static_cast<MoWen::Player *>(m_player)->cooldownYangChunBaiXueCurrent = cooldownYangChunBaiXueCurrent;
    static_cast<MoWen::Player *>(m_player)->cooldownShuYingHengXieCurrent = cooldownShuYingHengXieCurrent;
}

void GuYingHuaShuang::Reset()
{
    *m_globalCooldownCurrent = m_cooldown;
    index                    = 0;
}

ZhengLvHeMing1::ZhengLvHeMing1(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id       = SKILL_ZHENG_LV_HE_MING_1;
    m_name     = "正律和鸣";
    m_range    = 20;
    m_cooldown = 60 * 16;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void ZhengLvHeMing1::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void ZhengLvHeMing1::Trigger() { }

void ZhengLvHeMing1::SubEffect()
{
    static_cast<MoWen::Buff::ZhengLvHeMing *>(m_player->buffs[BUFF_ZHENG_LV_HE_MING])->TriggerAdd();
    static_cast<MoWen::Buff::ZhiYinHeMing *>(m_player->buffs[BUFF_ZHI_YIN_HE_MING])->level = 1;
}

ZhengLvHeMing2::ZhengLvHeMing2(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id       = SKILL_ZHENG_LV_HE_MING_2;
    m_name     = "正律和鸣";
    m_range    = 20;
    m_cooldown = 60 * 16;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void ZhengLvHeMing2::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void ZhengLvHeMing2::Trigger() { }

void ZhengLvHeMing2::SubEffect()
{
    static_cast<MoWen::Buff::ZhengLvHeMing *>(m_player->buffs[BUFF_ZHENG_LV_HE_MING])->TriggerAdd();
    static_cast<MoWen::Buff::ZhiYinHeMing *>(m_player->buffs[BUFF_ZHI_YIN_HE_MING])->level = 2;
}

ZhengLvHeMing3::ZhengLvHeMing3(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id       = SKILL_ZHENG_LV_HE_MING_3;
    m_name     = "正律和鸣";
    m_range    = 20;
    m_cooldown = 60 * 16;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void ZhengLvHeMing3::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void ZhengLvHeMing3::Trigger() { }

void ZhengLvHeMing3::SubEffect()
{
    static_cast<MoWen::Buff::ZhengLvHeMing *>(m_player->buffs[BUFF_ZHENG_LV_HE_MING])->TriggerAdd();
    static_cast<MoWen::Buff::ZhiYinHeMing *>(m_player->buffs[BUFF_ZHI_YIN_HE_MING])->level = 3;
}

YiXingHuanYing::YiXingHuanYing(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_YI_XING_HUAN_YING;
    m_name  = "移形换影";
    m_range = JX3DPS_UNLIMITED_RANGE;
}

void YiXingHuanYing::Cast()
{
    m_player->SetLastCastSkill(m_id);
    SubEffect();
}

void YiXingHuanYing::Trigger() { }

bool YiXingHuanYing::IsReady(bool fcast)
{
    if (m_player->IsCast()) {
        return false;
    }
    if (!m_player->IsReCast() || fcast) {
        if (static_cast<Buff::YingZi *>(m_player->buffs[BUFF_YING_ZI])->ids.size() > 0) {
            return true;
        }
        return false;
    }
    return false;
}

void YiXingHuanYing::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG_BIAO_JI](params);

    static_cast<Buff::YingZi *>(m_player->buffs[BUFF_YING_ZI])->TriggerClear();
}

} // namespace Skill

} // namespace MoWen

} // namespace JX3DPS