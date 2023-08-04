/**
 * Project: JX3DPS
 * File: MoWenSkill.cpp
 * Description:
 * Created Date: 2023-07-31 16:30:22
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-05 04:07:26
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "MoWenSkill.h"

#include "MoWenBuff.h"
#include "Target.h"

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
    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicSurplusDamage(targetId, rollResult, 0, 0);
    Record(targetId, rollResult, damage, 0, 0);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicSurplusDamage(targetId, rollResult, 0, 1);
    Record(targetId, rollResult, damage, 0, 1);

    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicSurplusDamage(targetId, rollResult, 0, 2);
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
}

void JX3DPS::MoWen::Skill::Gong::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        static_cast<MoWen::Player *>(m_player)->AddStyleCount(3);
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::Gong::SubEffect()
{
    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    static_cast<YangChunBaiXue *>(m_player->buffs[BUFF_YANG_CHUN_BAI_XUE])->TriggerClear();
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
}

void JX3DPS::MoWen::Skill::BianGong::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        static_cast<MoWen::Player *>(m_player)->AddStyleCount(3);
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::BianGong::SubEffect()
{
    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);
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
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(2);
    SubEffect();
}

void JX3DPS::MoWen::Skill::Shang::Trigger() { }

void JX3DPS::MoWen::Skill::Shang::SubEffect()
{
    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

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
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(1);
    SubEffect();
}

void JX3DPS::MoWen::Skill::Jue::Trigger() { }

void JX3DPS::MoWen::Skill::Jue::SubEffect()
{
    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

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
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(5);
}

void JX3DPS::MoWen::Skill::Zhi::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
        if (index < 2) {
            index++;
            m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
        } else {
            index                  = 0;
            m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
            static_cast<YangChunBaiXue *>(m_player->buffs[BUFF_YANG_CHUN_BAI_XUE])->TriggerClear();
        }
    }
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

void JX3DPS::MoWen::Skill::Zhi::TriggerShiXiang()
{
    m_cooldownCurrent -= 5 * 16;
    m_cooldownCurrent  = std::max(m_cooldownCurrent, 0);
}

void JX3DPS::MoWen::Skill::Zhi::SubEffect()
{
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
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(5);
}

void JX3DPS::MoWen::Skill::BianZhi::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        if (index < 2) {
            index++;
            m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
        } else {
            index                  = 0;
            m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
        }
        SubEffect();
    }
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

void JX3DPS::MoWen::Skill::BianZhi::TriggerShiXiang()
{
    m_cooldownCurrent -= 5 * 16;
    m_cooldownCurrent  = std::max(m_cooldownCurrent, 0);
}

void JX3DPS::MoWen::Skill::BianZhi::SubEffect() { }

JX3DPS::MoWen::Skill::Yu::Yu(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_YU;
    m_name  = "羽";
    m_range = 20;

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
}

void JX3DPS::MoWen::Skill::Yu::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::Yu::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::Yu::TriggerShiXiang()
{
    m_cooldownCurrent -= 5 * 16;
    m_cooldownCurrent  = std::max(m_cooldownCurrent, 0);
}

void JX3DPS::MoWen::Skill::Yu::SubEffect() { }

JX3DPS::MoWen::Skill::GaoShanLiuShui::GaoShanLiuShui(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                    = SKILL_GAO_SHAN_LIU_SHUI;
    m_name                  = "高山流水";
    m_range                 = JX3DPS_UNLIMITED_RANGE;
    m_globalCooldownCurrent = &m_noneGlobalCooldown;
}

void JX3DPS::MoWen::Skill::GaoShanLiuShui::Cast()
{
    m_player->SetLastCastSkill(m_id);
    if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::GAO_SHAN_LIU_SHUI)
    { // 高山主动
        m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownGaoShanLiuShuiCurrent;
        *m_globalCooldownCurrent = 50 * 16;
        SubEffect();
    } else { // 切曲风
        m_globalCooldownCurrent = &static_cast<MoWen::Player *>(m_player)->cooldownGaoShanLiuShuiCurrent;
        static_cast<YangChunBaiXue *>(m_player->skills[SKILL_YANG_CHUN_BAI_XUE])->SetCooldown();
        m_prepareFramesCurrent = 6;
    }
}

void JX3DPS::MoWen::Skill::GaoShanLiuShui::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;
        static_cast<MoWen::Player *>(m_player)->style = MoWen::Player::Style::GAO_SHAN_LIU_SHUI;
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
    m_id       = SKILL_YANG_CHUN_BAI_XUE;
    m_name     = "阳春白雪";
    m_range    = JX3DPS_UNLIMITED_RANGE;
    m_cooldown = 3 * 16;

    m_damageParams[0].emplace_back((77 + 77 + 7) / 2, 0, 89);

    if (m_player->talents[TALENT_CAN_LIAN]) {
        m_cooldown -= 2 * 16;
    }
}

void JX3DPS::MoWen::Skill::YangChunBaiXue::Cast()
{
    m_player->SetLastCastSkill(m_id);
    if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::YANG_CHUN_BAI_XUE)
    {        // 阳春主动
        m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    } else { // 切曲风
        static_cast<YangChunBaiXue *>(m_player->skills[SKILL_GAO_SHAN_LIU_SHUI])->SetCooldown();
        m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    }
}

void JX3DPS::MoWen::Skill::YangChunBaiXue::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        static_cast<MoWen::Player *>(m_player)->style = MoWen::Player::Style::YANG_CHUN_BAI_XUE;
    }
}

bool JX3DPS::MoWen::Skill::YangChunBaiXue::IsReady(bool fcast)
{
    if (m_player->IsCast()) {
        return false;
    }
    if (!m_player->IsReCast() || fcast) {
        if (static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::YANG_CHUN_BAI_XUE &&
                m_player->IsReCast() ||
            static_cast<MoWen::Player *>(m_player)->style == MoWen::Player::Style::GAO_SHAN_LIU_SHUI)
        {
            return true;
        }
        return false;
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
}

JX3DPS::MoWen::Skill::ShuYingHengXie::ShuYingHengXie(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_SHU_YING_HENG_XIE;
    m_name  = "疏影横斜";
    m_range = 20;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void JX3DPS::MoWen::Skill::ShuYingHengXie::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::ShuYingHengXie::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::ShuYingHengXie::SubEffect() { }

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
    m_range = 20;
}

void JX3DPS::MoWen::Skill::YiXingHuanYing::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldownCurrent =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::YiXingHuanYing::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::YiXingHuanYing::SubEffect() { }