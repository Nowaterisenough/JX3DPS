/**
 * Project: JX3DPS
 * File: MoWenSkill.cpp
 * Description:
 * Created Date: 2023-07-31 16:30:22
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-15 14:49:19
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "MoWenSkill.h"

#include "MoWenBuff.h"
#include "Target.hpp"

JX3DPS::MoWen::Skill::PoZhao::PoZhao(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
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

void JX3DPS::MoWen::Skill::PoZhao::Cast() { }

void JX3DPS::MoWen::Skill::PoZhao::Trigger() { }

void JX3DPS::MoWen::Skill::PoZhao::TriggerDamage(Id_t targetId)
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

JX3DPS::MoWen::Skill::Gong::Gong(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
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

void JX3DPS::MoWen::Skill::Gong::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<BianGong *>(m_player->skills[SKILL_BIAN_GONG])->Sync();
}

void JX3DPS::MoWen::Skill::Gong::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
        m_player->buffs[BUFF_QU_FENG]->Add(PLAYER_ID, 3);
        static_cast<BianGong *>(m_player->skills[SKILL_BIAN_GONG])->Sync();
        SubEffect();
    }
}

bool JX3DPS::MoWen::Skill::Gong::IsReady(bool fcast)
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

void JX3DPS::MoWen::Skill::Gong::Sync()
{
    Frame_t cooldownCurrent =
        static_cast<BianGong *>(m_player->skills[SKILL_BIAN_GONG])->GetEnergyCooldownCurrent();
    m_cooldownCurrent = cooldownCurrent;
}

void JX3DPS::MoWen::Skill::Gong::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    static_cast<Buff::YangChunBaiXue *>(m_player->buffs[BUFF_YANG_CHUN_BAI_XUE])->TriggerClear();
}

JX3DPS::MoWen::Skill::BianGong::BianGong(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
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

void JX3DPS::MoWen::Skill::BianGong::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<Gong *>(m_player->skills[SKILL_GONG])->Sync();
}

void JX3DPS::MoWen::Skill::BianGong::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
        m_player->buffs[BUFF_QU_FENG]->Add(PLAYER_ID, 4);
        static_cast<Gong *>(m_player->skills[SKILL_GONG])->Sync();
        SubEffect();
    }
}

bool JX3DPS::MoWen::Skill::BianGong::IsReady(bool fcast)
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

void JX3DPS::MoWen::Skill::BianGong::Sync()
{
    Frame_t cooldownCurrent =
        static_cast<Gong *>(m_player->skills[SKILL_GONG])->GetEnergyCooldownCurrent();
    m_cooldownCurrent = cooldownCurrent;
}

void JX3DPS::MoWen::Skill::BianGong::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    static_cast<Buff::YangChunBaiXue *>(m_player->buffs[BUFF_YANG_CHUN_BAI_XUE])->TriggerClear();
}

JX3DPS::MoWen::Skill::Shang::Shang(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
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

void JX3DPS::MoWen::Skill::Shang::Cast()
{
    m_player->SetLastCastSkill(m_id);
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JX3DPS::MoWen::Skill::Shang::Trigger() { }

void JX3DPS::MoWen::Skill::Shang::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    m_player->buffs[BUFF_QU_FENG]
        ->Add(PLAYER_ID, 2 + static_cast<int>(static_cast<MoWen::Player *>(m_player)->style));

    static_cast<Buff::Shang *>(m_player->buffs[BUFF_SHANG])->TriggerAdd(m_player->GetTargetId());
}

JX3DPS::MoWen::Skill::Jue::Jue(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_JUE;
    m_name  = "角";
    m_range = 20;

    m_damageParams[0].emplace_back((14 + 14 + 5) / 2, 0, 80);
}

void JX3DPS::MoWen::Skill::Jue::Cast()
{
    m_player->SetLastCastSkill(m_id);
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JX3DPS::MoWen::Skill::Jue::Trigger() { }

void JX3DPS::MoWen::Skill::Jue::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    m_player->buffs[BUFF_QU_FENG]
        ->Add(PLAYER_ID, 1 + static_cast<int>(static_cast<MoWen::Player *>(m_player)->style));

    static_cast<Buff::Jue *>(m_player->buffs[BUFF_JUE])->TriggerAdd(m_player->GetTargetId());
}

JX3DPS::MoWen::Skill::Zhi::Zhi(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
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

void JX3DPS::MoWen::Skill::Zhi::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_player->SetReCast(true);
    m_player->SetReCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown;
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    if (m_energyCount == 3) {
        m_energyCountCurrent--;
    }
    static_cast<BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->Sync();

    m_player->buffs[BUFF_QU_FENG]->Add(PLAYER_ID, 5);

    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);
    xianFengStackNum = m_player->buffs[BUFF_XIAN_FENG]->GetStackNumCurrent();
    m_triggerEffects[TRIGGER_HAO_QING_ZHI](params);

    m_triggerEffects[TRIGGER_XIAN_FENG_CLEAR](params);
}

void JX3DPS::MoWen::Skill::Zhi::Trigger()
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

void JX3DPS::MoWen::Skill::Zhi::Stop()
{
    m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
}

bool JX3DPS::MoWen::Skill::Zhi::IsReady(bool fcast)
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

void JX3DPS::MoWen::Skill::Zhi::Sync()
{
    Frame_t cooldownCurrent =
        static_cast<BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->GetEnergyCooldownCurrent();
    m_cooldownCurrent = cooldownCurrent;

    int energyCountCurrent =
        static_cast<BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->GetEnergyCountCurrent();
    m_energyCountCurrent = energyCountCurrent;
}

void JX3DPS::MoWen::Skill::Zhi::TriggerShiXiang()
{
    m_cooldownCurrent -= 5 * 16;
    m_cooldownCurrent  = std::max(m_cooldownCurrent, 0);
}

void JX3DPS::MoWen::Skill::Zhi::SubEffect()
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

JX3DPS::MoWen::Skill::BianZhi::BianZhi(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
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

void JX3DPS::MoWen::Skill::BianZhi::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_player->SetReCast(true);
    m_player->SetReCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown;
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<Zhi *>(m_player->skills[SKILL_ZHI])->Sync();

    m_player->buffs[BUFF_QU_FENG]->Add(PLAYER_ID, 6);

    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG](params);
    xianFengStackNum = m_player->buffs[BUFF_XIAN_FENG]->GetStackNumCurrent();

    m_triggerEffects[TRIGGER_HAO_QING_BIAN_ZHI](params);

    m_triggerEffects[TRIGGER_XIAN_FENG_CLEAR](params);
}

void JX3DPS::MoWen::Skill::BianZhi::Trigger()
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

void JX3DPS::MoWen::Skill::BianZhi::Stop()
{
    m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
}

bool JX3DPS::MoWen::Skill::BianZhi::IsReady(bool fcast)
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

void JX3DPS::MoWen::Skill::BianZhi::Sync()
{
    Frame_t cooldownCurrent =
        static_cast<Zhi *>(m_player->skills[SKILL_ZHI])->GetEnergyCooldownCurrent();
    m_cooldownCurrent = cooldownCurrent;

    int energyCountCurrent =
        static_cast<Zhi *>(m_player->skills[SKILL_ZHI])->GetEnergyCountCurrent();
    m_energyCountCurrent = energyCountCurrent;
}

void JX3DPS::MoWen::Skill::BianZhi::TriggerShiXiang()
{
    m_cooldownCurrent -= 5 * 16;
    m_cooldownCurrent  = std::max(m_cooldownCurrent, 0);
}

void JX3DPS::MoWen::Skill::BianZhi::SubEffect()
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

JX3DPS::MoWen::Skill::Yu::Yu(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
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

void JX3DPS::MoWen::Skill::Yu::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    m_energyCountCurrent--;

    SubEffect();
}

void JX3DPS::MoWen::Skill::Yu::Trigger()
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

void JX3DPS::MoWen::Skill::Yu::TriggerShiXiang()
{
    m_cooldownCurrent -= 5 * 16;
    m_cooldownCurrent  = std::max(m_cooldownCurrent, 0);
}

void JX3DPS::MoWen::Skill::Yu::SubEffect()
{
    Params params;
    params.player = m_player;

    m_player->buffs[BUFF_QU_FENG]->Add(PLAYER_ID, 4 + static_cast<int>(static_cast<MoWen::Player *>(m_player)->style));

    m_triggerEffects[TRIGGER_XIAN_FENG](params);

    m_triggerEffects[TRIGGER_SHI_XIANG](params);

    m_triggerEffects[TRIGGER_SHU_LI](params);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);
}

JX3DPS::MoWen::Skill::GaoShanLiuShui::GaoShanLiuShui(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                    = SKILL_GAO_SHAN_LIU_SHUI;
    m_name                  = "高山流水";
    m_range                 = JX3DPS_UNLIMITED_RANGE;
    m_globalCooldownCurrent = &m_noneGlobalCooldown;
    m_prepareFrames         = 6;
}

void JX3DPS::MoWen::Skill::GaoShanLiuShui::Cast()
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
        m_player->SetCast(true);

        static_cast<BianGong *>(m_player->skills[SKILL_BIAN_GONG])->Sync();
        static_cast<BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->Sync();
    }
}

void JX3DPS::MoWen::Skill::GaoShanLiuShui::Trigger()
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

void JX3DPS::MoWen::Skill::GaoShanLiuShui::SetCooldown()
{
    m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownStyleCurrent;
    *m_globalCooldownCurrent = 3 * 16;
}

void JX3DPS::MoWen::Skill::GaoShanLiuShui::SubEffect()
{
    static_cast<Buff::GaoShanLiuShui *>(m_player->buffs[BUFF_GAO_SHAN_LIU_SHUI])->TriggerAdd();
}

JX3DPS::MoWen::Skill::YangChunBaiXue::YangChunBaiXue(JX3DPS::Player *player, Targets *targets) :
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

void JX3DPS::MoWen::Skill::YangChunBaiXue::Cast()
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

        m_player->SetCast(true);

        static_cast<Gong *>(m_player->skills[SKILL_GONG])->Sync();
        static_cast<Zhi *>(m_player->skills[SKILL_ZHI])->Sync();
    }
}

void JX3DPS::MoWen::Skill::YangChunBaiXue::Trigger()
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

bool JX3DPS::MoWen::Skill::YangChunBaiXue::IsReady(bool fcast)
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

bool JX3DPS::MoWen::Skill::YangChunBaiXue::StopReCastSkill()
{
    if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::YANG_CHUN_BAI_XUE)
    {
        return false;
    }
    m_player->skills[m_player->GetReCastSkill()]->Stop();
    return true;
}

void JX3DPS::MoWen::Skill::YangChunBaiXue::SetCooldown()
{
    m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownStyleCurrent;
    *m_globalCooldownCurrent = 3 * 16;
}

void JX3DPS::MoWen::Skill::YangChunBaiXue::SubEffect()
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

JX3DPS::MoWen::Skill::ShuYingHengXie::ShuYingHengXie(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                 = SKILL_SHU_YING_HENG_XIE;
    m_name               = "疏影横斜";
    m_range              = 20;
    m_cooldown           = 20 * 16;
    m_cooldownCurrent    = JX3DPS_INVALID_FRAMES_SET;
    m_energyCountCurrent = m_energyCount = 3;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);

    if (m_player->talents[TALENT_KE_MENG]) {
        percent = 100;
    }
}

void JX3DPS::MoWen::Skill::ShuYingHengXie::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    m_energyCountCurrent--;
    SubEffect();
}

void JX3DPS::MoWen::Skill::ShuYingHengXie::Trigger()
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

void JX3DPS::MoWen::Skill::ShuYingHengXie::SubEffect()
{
    static_cast<Buff::YingZi *>(m_player->buffs[BUFF_YING_ZI])->TriggerAdd();
    if (RandomUniform(1, 100) <= percent) {
        static_cast<Buff::YingZi *>(m_player->buffs[BUFF_YING_ZI])->TriggerAdd();
    }
}

JX3DPS::MoWen::Skill::ShuYingHuaShuang::ShuYingHuaShuang(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_SHU_YING_HUA_SHUANG;
    m_name  = "疏影化双";
    m_range = 20;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void JX3DPS::MoWen::Skill::ShuYingHuaShuang::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::ShuYingHuaShuang::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::ShuYingHuaShuang::SubEffect() { }

JX3DPS::MoWen::Skill::ZhengLvHeMing1::ZhengLvHeMing1(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_ZHENG_LV_HE_MING_1;
    m_name  = "正律和鸣";
    m_range = 20;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing1::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing1::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing1::SubEffect() { }

JX3DPS::MoWen::Skill::ZhengLvHeMing2::ZhengLvHeMing2(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_ZHENG_LV_HE_MING_2;
    m_name  = "正律和鸣";
    m_range = 20;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing2::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing2::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing2::SubEffect() { }

JX3DPS::MoWen::Skill::ZhengLvHeMing3::ZhengLvHeMing3(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_ZHENG_LV_HE_MING_3;
    m_name  = "正律和鸣";
    m_range = 20;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing3::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing3::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing3::SubEffect() { }

JX3DPS::MoWen::Skill::YiXingHuanYing::YiXingHuanYing(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_YI_XING_HUAN_YING;
    m_name  = "移形换影";
    m_range = JX3DPS_UNLIMITED_RANGE;
}

void JX3DPS::MoWen::Skill::YiXingHuanYing::Cast()
{
    m_player->SetLastCastSkill(m_id);
    SubEffect();
}

void JX3DPS::MoWen::Skill::YiXingHuanYing::Trigger() { }

bool JX3DPS::MoWen::Skill::YiXingHuanYing::IsReady(bool fcast)
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

void JX3DPS::MoWen::Skill::YiXingHuanYing::SubEffect()
{
    Params params;
    params.player = m_player;
    m_triggerEffects[TRIGGER_XIAN_FENG_BIAO_JI](params);

    static_cast<Buff::YingZi *>(m_player->buffs[BUFF_YING_ZI])->TriggerClear();
}