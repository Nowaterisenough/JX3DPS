/**
 * Project: JX3DPS
 * File: Skill.cpp
 * Description:
 * Created Date: 2023-07-24 13:57:40
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-28 20:51:41
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "TaiXuJianYiSkill.h"

#include "Buff.h"
#include "Target.hpp"

namespace JX3DPS {

namespace TaiXuJianYi {

JX3DPS::TaiXuJianYi::PoZhao::PoZhao(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
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

void JX3DPS::TaiXuJianYi::PoZhao::Cast() { }

void JX3DPS::TaiXuJianYi::PoZhao::Trigger() { }

void JX3DPS::TaiXuJianYi::PoZhao::Stop() { }

void JX3DPS::TaiXuJianYi::PoZhao::TriggerDamage(Id_t targetId, int sub, int level)
{
    RollResult rollResult = GetPhysicsRollResult();
    // GainsDamage damage     = CalcPhysicsSurplusDamage(targetId, rollResult, sub, level);
    // Record(m_player->GetTargetId(), rollResult, damage, 0, level);
}

JX3DPS::TaiXuJianYi::WuWoWuJian::WuWoWuJian(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_WU_WO_WU_JIAN;
    m_name  = "无我无剑";
    m_range = 4;

    m_damageParams[0].emplace_back((2696 * 1 / 10 / 12 + (2696 + 286) * 1 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 1);
    m_damageParams[0].emplace_back((2696 * 2 / 10 / 12 + (2696 + 286) * 2 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 2);
    m_damageParams[0].emplace_back((2696 * 3 / 10 / 12 + (2696 + 286) * 3 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 3);
    m_damageParams[0].emplace_back((2696 * 4 / 10 / 12 + (2696 + 286) * 4 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 4);
    m_damageParams[0].emplace_back((2696 * 5 / 10 / 12 + (2696 + 286) * 5 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 5);
    m_damageParams[0].emplace_back((2696 * 6 / 10 / 12 + (2696 + 286) * 6 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 6);
    m_damageParams[0].emplace_back((2696 * 7 / 10 / 12 + (2696 + 286) * 7 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 7);
    m_damageParams[0].emplace_back((2696 * 8 / 10 / 12 + (2696 + 286) * 8 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 8);
    m_damageParams[0].emplace_back((2696 * 9 / 10 / 12 + (2696 + 286) * 9 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 9);
    m_damageParams[0].emplace_back((2696 * 10 / 10 / 12 + (2696 + 286) * 10 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 10);

    m_damageParams[1].emplace_back((2696 * 1 / 10 / 12 + (2696 + 286) * 1 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 1);
    m_damageParams[1].emplace_back((2696 * 2 / 10 / 12 + (2696 + 286) * 2 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 2);
    m_damageParams[1].emplace_back((2696 * 3 / 10 / 12 + (2696 + 286) * 3 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 3);
    m_damageParams[1].emplace_back((2696 * 4 / 10 / 12 + (2696 + 286) * 4 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 4);
    m_damageParams[1].emplace_back((2696 * 5 / 10 / 12 + (2696 + 286) * 5 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 5);
    m_damageParams[1].emplace_back((2696 * 6 / 10 / 12 + (2696 + 286) * 6 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 6);
    m_damageParams[1].emplace_back((2696 * 7 / 10 / 12 + (2696 + 286) * 7 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 7);
    m_damageParams[1].emplace_back((2696 * 8 / 10 / 12 + (2696 + 286) * 8 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 8);
    m_damageParams[1].emplace_back((2696 * 9 / 10 / 12 + (2696 + 286) * 9 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 9);
    m_damageParams[1].emplace_back((2696 * 10 / 10 / 12 + (2696 + 286) * 10 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 10);

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikePowerAdditionalPercentInt += 200;
    }

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikePowerAdditionalPercentInt += 300;
    }

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikePowerAdditionalPercentInt += 400;
    }

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_QIDIAN]) {
        m_effectQidianAdditional += 1;
    }

    if (m_player->talents[TALENT_TONG_GEN]) {
        m_effectDamageAdditionalPercentInt += 102;
    }

    if (m_player->equipEffects[EQUIP_EFFECT_SET_SKILL]) {
        m_effectDamageAdditionalPercentInt += 102;
    }

    if (m_player->equipEffects[EQUIP_EFFECT_WEAPON_CW]) {
        m_effectDamageAdditionalPercentInt += 51;
    }
}

void JX3DPS::TaiXuJianYi::WuWoWuJian::Cast()
{
    m_player->SetLastCastSkill(m_id);
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JX3DPS::TaiXuJianYi::WuWoWuJian::Trigger() { }

void JX3DPS::TaiXuJianYi::WuWoWuJian::Stop() { }

void JX3DPS::TaiXuJianYi::WuWoWuJian::TriggerFengShiAdd()
{
    m_effectDamageAdditionalPercentInt += 307;
}

void JX3DPS::TaiXuJianYi::WuWoWuJian::TriggerFengShiClear()
{
    m_effectDamageAdditionalPercentInt -= 307;
}

void JX3DPS::TaiXuJianYi::WuWoWuJian::SubEffect()
{
    int qidian = m_player->GetQidian();
    int level  = qidian - 1;

    m_player->SetQidian(0);

    // 续气秘籍
    m_player->AddQidian(m_effectQidianAdditional);

    Params params;

    // 无意 3格气以上
    params.skill = this;
    m_player->triggerEffects[TALENT_WU_YI](params);

    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, level);
    Record(m_player->GetTargetId(), rollResult, damage, 0, level);

    params.rollResult = rollResult;

    if (qidian >= 6) {
        static_cast<TaiXuJianYi::PoZhao *>(m_player->skills[SKILL_PO_ZHAO])
            ->TriggerDamage(m_player->GetTargetId(), 0, (qidian - 6) / 2);
    }

    // 北斗七星阵 游刃
    m_player->triggerEffects[BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN](params);

    // 无欲 目标有叠刃
    m_player->triggerEffects[TALENT_WU_YU](params);

    // 长生 持盈
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    // 大附魔 腕
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

    // 大附魔 腰
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_BELT](params);

    // 大附魔 鞋
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_SHOES](params);

    // 叠刃
    m_player->triggerEffects[TALENT_DIE_REN](params);

    // 深埋 会心
    m_player->triggerEffects[TALENT_SHEN_MAI](params);

    // 无意 结束去掉状态
    m_player->triggerEffects[TALENT_WU_YI_CLEAR](params);

    // 风逝 结束去掉状态
    m_player->triggerEffects[TALENT_FENG_SHI_CLEAR](params);

    // 橙武特效 八荒归元无调息 持续伤害
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](params);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](params);

    // 水特效
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](params);
}

JX3DPS::TaiXuJianYi::BaHuangGuiYuan::BaHuangGuiYuan(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id       = SKILL_BA_HUANG_GUI_YUAN;
    m_name     = "八荒归元";
    m_range    = 12;
    m_cooldown = 16 * 12;

    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0);

    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 1) * 1.1 * 1.1 * 1.05);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 2) * 1.1 * 1.1 * 1.05);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 3) * 1.1 * 1.1 * 1.05);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 4) * 1.1 * 1.1 * 1.05);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 5) * 1.1 * 1.1 * 1.05);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 6) * 1.1 * 1.1 * 1.05);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 7) * 1.1 * 1.1 * 1.05);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 8) * 1.1 * 1.1 * 1.05);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 9) * 1.1 * 1.1 * 1.05);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 10) * 1.1 * 1.1 * 1.05);

    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205);

    if (m_player->secrets[SECRET_BA_HUANG_GUI_YUAN_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->secrets[SECRET_BA_HUANG_GUI_YUAN_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->secrets[SECRET_BA_HUANG_GUI_YUAN_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }

    if (m_player->secrets[SECRET_BA_HUANG_GUI_YUAN_QIDIAN]) {
        m_effectQidianAdditional += 1;
    }

    if (m_player->secrets[SECRET_BA_HUANG_GUI_YUAN_COOLDOWN]) {
        m_effectCooldownAdditional  = -16 * 1;
        m_cooldown                 += m_effectCooldownAdditional;
    }

    if (m_player->talents[TALENT_FENG_SHI]) {
        m_range += 2.0;
    }

    if (m_player->equipEffects[EQUIP_EFFECT_WEAPON_CW]) {
        m_effectDamageAdditionalPercentInt += 51;
    }
}

void JX3DPS::TaiXuJianYi::BaHuangGuiYuan::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JX3DPS::TaiXuJianYi::BaHuangGuiYuan::Trigger() { }

void JX3DPS::TaiXuJianYi::BaHuangGuiYuan::Stop() { }

void JX3DPS::TaiXuJianYi::BaHuangGuiYuan::SubEffect()
{
    m_player->AddQidian(2);

    // 续气秘籍
    m_player->AddQidian(m_effectQidianAdditional);

    int level = 10 - ((*m_targets)[m_player->GetTargetId()]->GetLifePercent() - 0.01) * 10;

    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, level);
    Record(m_player->GetTargetId(), rollResult, damage, 0, level);

    Params params;
    params.rollResult = rollResult;

    rollResult = GetPhysicsRollResult();
    damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 1, level);
    Record(m_player->GetTargetId(), rollResult, damage, 1, level);

    // 深埋 会心
    m_player->triggerEffects[TALENT_SHEN_MAI](params);

    // 大附魔 腕
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

    // 大附魔 腰
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_BELT](params);

    // 大附魔 鞋
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_SHOES](params);

    // 风逝
    m_player->triggerEffects[TALENT_FENG_SHI](params);

    // 切玉
    m_player->triggerEffects[TALENT_QIE_YU](params);

    // 长生 持盈
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    // 橙武特效 八荒归元无调息 持续伤害
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](params);

    // 橙武特效 持续伤害
    m_player->triggerEffects[BUFF_WAN_XIANG_GUI_YUAN](params);

    // 橙武特效 八荒归元 额外一次伤害
    m_player->triggerEffects[BUFF_WEAPON_EFFECT_CW_2](params);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](params);

    // 北斗七星阵 游刃
    m_player->triggerEffects[BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN](params);

    // 水特效
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](params);
}

void JX3DPS::TaiXuJianYi::BaHuangGuiYuan::ResetCooldown()
{
    m_cooldown = 16 * 12 + m_effectCooldownAdditional;
}

void JX3DPS::TaiXuJianYi::BaHuangGuiYuan::ClearCooldown()
{
    m_cooldown = 0;
}

JX3DPS::TaiXuJianYi::SanHuanTaoYue::SanHuanTaoYue(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id       = SKILL_SAN_HUAN_TAO_YUE;
    m_name     = "三环套月";
    m_range    = 4;
    m_cooldown = 16 * 2;

    m_damageParams[0].emplace_back((1233 / 10 + (1233 + 137) / 10) / 2, 1024, 120 * 1.1 * 1.05 * 1.1);

    if (m_player->secrets[SECRET_SAN_HUAN_TAO_YUE_COOLDOWN]) {
        m_effectCooldownAdditional  = -16 / 2;
        m_cooldown                 += m_effectCooldownAdditional;
    }

    if (m_player->secrets[SECRET_SAN_HUAN_TAO_YUE_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikePowerAdditionalPercentInt += 200;
    }

    if (m_player->secrets[SECRET_SAN_HUAN_TAO_YUE_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikePowerAdditionalPercentInt += 300;
    }

    if (m_player->secrets[SECRET_SAN_HUAN_TAO_YUE_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikePowerAdditionalPercentInt += 400;
    }

    if (m_player->secrets[SECRET_SAN_HUAN_TAO_YUE_DAMAGE_3]) {
        m_effectDamageAdditionalPercentInt += 31;
    }

    if (m_player->secrets[SECRET_SAN_HUAN_TAO_YUE_DAMAGE_4]) {
        m_effectDamageAdditionalPercentInt += 41;
    }

    if (m_player->secrets[SECRET_SAN_HUAN_TAO_YUE_DAMAGE_5]) {
        m_effectDamageAdditionalPercentInt += 51;
    }

    if (m_player->talents[TALENT_XIN_GU]) {
        m_effectCriticalStrikePowerAdditionalPercentInt += 1000;
        m_effectCriticalStrikePowerAdditionalPercentInt += 102;
    }

    if (m_player->talents[TALENT_HUAN_YUE]) {
        m_effectCooldownAdditional  = 16 * 4;
        m_cooldown                 += m_effectCooldownAdditional;
    }

    if (m_player->talents[TALENT_FENG_SHI]) {
        m_range += 2.0;
    }
}

void JX3DPS::TaiXuJianYi::SanHuanTaoYue::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JX3DPS::TaiXuJianYi::SanHuanTaoYue::Trigger() { }

void JX3DPS::TaiXuJianYi::SanHuanTaoYue::Stop() { }

void JX3DPS::TaiXuJianYi::SanHuanTaoYue::SubEffect()
{
    m_player->AddQidian(2);

    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    Params params;
    params.rollResult = rollResult;

    rollResult = GetPhysicsRollResult();
    damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    rollResult = GetPhysicsRollResult();
    damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    // 环月
    m_player->triggerEffects[TALENT_HUAN_YUE](params);

    // 北斗七星阵 游刃
    m_player->triggerEffects[BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN](params);

    // 深埋 会心
    m_player->triggerEffects[TALENT_SHEN_MAI](params);

    // 大附魔 腕
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

    // 大附魔 腰
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_BELT](params);

    // 大附魔 鞋
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_SHOES](params);

    // 风逝
    m_player->triggerEffects[TALENT_FENG_SHI](params);

    // 长生 持盈
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    // 橙武特效1 八荒归元无调息 持续伤害
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](params);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](params);

    // 水特效
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](params);
}

JX3DPS::TaiXuJianYi::WanJianGuiZong::WanJianGuiZong(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id          = SKILL_WAN_JIAN_GUI_ZONG;
    m_name        = "万剑归宗";
    m_range       = 8;
    m_cooldown    = 16 * 12;
    m_targetLimit = 6;

    m_damageParams[0].emplace_back((225 + (225 + 37)) / 2, 0, 100 * 1.1 * 1.25 * 1.05 * 1.5 * 1.1);

    if (m_player->talents[TALENT_TONG_GEN]) {
        m_effectDamageAdditionalPercentInt += 102;
        m_targetLimit                      += 2;
    }

    if (m_player->talents[TALENT_XU_JI]) {
        m_effectCriticalStrikePowerAdditionalPercentInt += 1000;
        m_effectCriticalStrikePowerAdditionalPercentInt += 102;
        m_effectCooldownAdditional                       = -16 * 2;
        m_cooldown                                      += m_effectCooldownAdditional;
    }
}

void JX3DPS::TaiXuJianYi::WanJianGuiZong::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JX3DPS::TaiXuJianYi::WanJianGuiZong::Trigger() { }

void JX3DPS::TaiXuJianYi::WanJianGuiZong::Stop() { }

void JX3DPS::TaiXuJianYi::WanJianGuiZong::SubEffect()
{
    Params params;
    int    count = 0;
    for (auto &[id, target] : *m_targets) {
        if (target->GetDistance() > m_range) { // 超出范围
            continue;
        }
        if (count == m_targetLimit) {
            break;
        }
        count++;

        // 命中一个目标 续气半格
        m_player->AddQidian(1);

        RollResult  rollResult = GetPhysicsRollResult();
        GainsDamage damage     = CalcPhysicsDamage(id, rollResult, 0, 0);
        Record(id, rollResult, damage, 0, 0);

        params.rollResult = rollResult;
        params.targetId   = id;

        // 北斗七星阵 游刃
        m_player->triggerEffects[BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN](params);

        // 深埋 会心
        m_player->triggerEffects[TALENT_SHEN_MAI](params);

        // 大附魔 腕
        m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

        // 大附魔 腰
        m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_BELT](params);

        // 大附魔 鞋
        m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_SHOES](params);

        // 虚极
        m_player->triggerEffects[TALENT_XU_JI](params);

        // 镜花影
        m_player->triggerEffects[TALENT_JING_HUA_YING](params);

        // 橙武特效 八荒归元无调息 持续伤害
        m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](params);
    }

    // 长生 持盈
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](params);

    // 水特效
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](params);
}

JX3DPS::TaiXuJianYi::RenJianHeYi::RenJianHeYi(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id       = SKILL_REN_JIAN_HE_YI;
    m_name     = "人剑合一";
    m_range    = 6;
    m_cooldown = 16 * 20;

    m_damageParams[0].emplace_back(1273 / 20, 0, 40);

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_COOLDOWN]) {
        m_effectCooldownAdditional  = -16 * 5;
        m_cooldown                 += m_effectCooldownAdditional;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_RANG_1]) {
        m_range += 1;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_RANG_2]) {
        m_range += 1;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_RANG_3]) {
        m_range += 1;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_DAMAGE_40]) {
        m_effectDamageAdditionalPercentInt += 409;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_DAMAGE_60]) {
        m_effectDamageAdditionalPercentInt += 614;
    }
}

void JX3DPS::TaiXuJianYi::RenJianHeYi::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JX3DPS::TaiXuJianYi::RenJianHeYi::Trigger() { }

void JX3DPS::TaiXuJianYi::RenJianHeYi::Stop() { }

void JX3DPS::TaiXuJianYi::RenJianHeYi::SubEffect()
{
    m_player->skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->SetEnergyCooldownCurrent(m_cooldownCurrent);
    m_player->skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->SetEnergyCooldownCurrent(m_cooldownCurrent);

    int count = 0;

    Params params;

    for (auto &fid : static_cast<TaiXuJianYi::Player *>(m_player)->fields) {

        m_player->buffs[fid]->Clear(PLAYER_ID, 1);

        m_player->triggerEffects[static_cast<Id_t>(fid + BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN - BUFF_FIELD_SUI_XING_CHEN)](params);

        count++;

        for (auto &[id, target] : *m_targets) {
            if (target->GetDistance() > m_range) [[unlikely]] { // 超出范围
                continue;
            }

            RollResult rollResult = GetPhysicsRollResult();

            params.rollResult = rollResult;
            params.targetId   = id;

            // 深埋 会心
            m_player->triggerEffects[TALENT_SHEN_MAI](params);

            // 大附魔 腕
            m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

            // 大附魔 腰
            m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_BELT](params);

            // 大附魔 鞋
            m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_SHOES](params);

            // 橙武特效 八荒归元无调息 持续伤害
            m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](params);

            // 人剑秘籍 dot
            m_player->triggerEffects[SECRET_REN_JIAN_HE_YI_EFFECT_DOT](params);

            GainsDamage damage = CalcPhysicsDamage(id, rollResult, 0, 0);
            Record(id, rollResult, damage, 0, 0);
        }
    }

    // 长生 持盈
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](params);

    // 水特效
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](params);

    // 玄门
    params.stackNum = count;
    m_player->triggerEffects[TALENT_XUAN_MEN](params);
}

JX3DPS::TaiXuJianYi::RenJianHeYiSuiXingChen::RenJianHeYiSuiXingChen(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id       = SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN;
    m_name     = "人剑合一·空爆·碎星辰";
    m_range    = 6;
    m_cooldown = 16 * 20;

    m_damageParams[0].emplace_back(1273 / 20, 0, 40);

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_COOLDOWN]) {
        m_effectCooldownAdditional  = -16 * 5;
        m_cooldown                 += m_effectCooldownAdditional;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_RANG_1]) {
        m_range += 1;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_RANG_2]) {
        m_range += 1;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_RANG_3]) {
        m_range += 1;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_DAMAGE_40]) {
        m_effectDamageAdditionalPercentInt += 409;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_DAMAGE_60]) {
        m_effectDamageAdditionalPercentInt += 614;
    }
}

void JX3DPS::TaiXuJianYi::RenJianHeYiSuiXingChen::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JX3DPS::TaiXuJianYi::RenJianHeYiSuiXingChen::Trigger() { }

void JX3DPS::TaiXuJianYi::RenJianHeYiSuiXingChen::Stop() { }

void JX3DPS::TaiXuJianYi::RenJianHeYiSuiXingChen::SubEffect()
{
    m_player->skills[SKILL_REN_JIAN_HE_YI]->SetEnergyCooldownCurrent(m_cooldownCurrent);
    m_player->skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->SetEnergyCooldownCurrent(m_cooldownCurrent);

    int count = 0;

    Params params;

    for (auto &fid : static_cast<TaiXuJianYi::Player *>(m_player)->fields) {
        if (fid == JX3DPS::BUFF_FIELD_SUI_XING_CHEN) {
            continue;
        }

        m_player->buffs[fid]->Clear(PLAYER_ID, 1);

        m_player->triggerEffects[static_cast<Id_t>(fid + BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN - BUFF_FIELD_SUI_XING_CHEN)](params);

        count++;

        for (auto &[id, target] : *m_targets) {
            if (target->GetDistance() > m_range) [[unlikely]] { // 超出范围
                continue;
            }

            RollResult rollResult = GetPhysicsRollResult();

            params.rollResult = rollResult;
            params.targetId   = id;

            // 深埋 会心
            m_player->triggerEffects[TALENT_SHEN_MAI](params);

            // 大附魔 腕
            m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

            // 大附魔 腰
            m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_BELT](params);

            // 大附魔 鞋
            m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_SHOES](params);

            // 橙武特效 八荒归元无调息 持续伤害
            m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](params);

            // 人剑秘籍 dot
            m_player->triggerEffects[SECRET_REN_JIAN_HE_YI_EFFECT_DOT](params);

            GainsDamage damage = CalcPhysicsDamage(id, rollResult, 0, 0);
            Record(id, rollResult, damage, 0, 0);
        }
    }

    // 长生 持盈
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](params);

    // 水特效
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](params);

    // 玄门
    params.stackNum = count;
    m_player->triggerEffects[TALENT_XUAN_MEN](params);
}

JX3DPS::TaiXuJianYi::RenJianHeYiTunRiYue::RenJianHeYiTunRiYue(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id       = SKILL_REN_JIAN_HE_YI_TUN_RI_YUE;
    m_name     = "人剑合一·空爆·吞日月";
    m_range    = 6;
    m_cooldown = 16 * 20;

    m_damageParams[0].emplace_back(1273 / 20, 0, 40);

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_COOLDOWN]) {
        m_effectCooldownAdditional  = -16 * 5;
        m_cooldown                 += m_effectCooldownAdditional;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_RANG_1]) {
        m_range += 1;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_RANG_2]) {
        m_range += 1;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_RANG_3]) {
        m_range += 1;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_DAMAGE_40]) {
        m_effectDamageAdditionalPercentInt += 409;
    }

    if (m_player->secrets[SECRET_REN_JIAN_HE_YI_DAMAGE_60]) {
        m_effectDamageAdditionalPercentInt += 614;
    }
}

void JX3DPS::TaiXuJianYi::RenJianHeYiTunRiYue::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    SubEffect();
}

void JX3DPS::TaiXuJianYi::RenJianHeYiTunRiYue::Trigger() { }

void JX3DPS::TaiXuJianYi::RenJianHeYiTunRiYue::Stop() { }

void JX3DPS::TaiXuJianYi::RenJianHeYiTunRiYue::SubEffect()
{
    m_player->skills[SKILL_REN_JIAN_HE_YI]->SetEnergyCooldownCurrent(m_cooldownCurrent);
    m_player->skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->SetEnergyCooldownCurrent(m_cooldownCurrent);

    int count = 0;

    Params params;

    for (auto &fid : static_cast<TaiXuJianYi::Player *>(m_player)->fields) {
        if (fid == JX3DPS::BUFF_FIELD_TUN_RI_YUE) {
            continue;
        }

        m_player->buffs[fid]->Clear(PLAYER_ID, 1);

        m_player->triggerEffects[static_cast<Id_t>(fid + BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN - BUFF_FIELD_SUI_XING_CHEN)](params);

        count++;

        for (auto &[id, target] : *m_targets) {
            if (target->GetDistance() > m_range) [[unlikely]] { // 超出范围
                continue;
            }

            RollResult rollResult = GetPhysicsRollResult();

            params.rollResult = rollResult;
            params.targetId   = id;

            // 深埋 会心
            m_player->triggerEffects[TALENT_SHEN_MAI](params);

            // 大附魔 腕
            m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

            // 大附魔 腰
            m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_BELT](params);

            // 大附魔 鞋
            m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_SHOES](params);

            // 橙武特效 八荒归元无调息 持续伤害
            m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](params);

            // 人剑秘籍 dot
            m_player->triggerEffects[SECRET_REN_JIAN_HE_YI_EFFECT_DOT](params);

            GainsDamage damage = CalcPhysicsDamage(id, rollResult, 0, 0);
            Record(id, rollResult, damage, 0, 0);
        }
    }

    // 长生 持盈
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](params);

    // 水特效
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](params);

    // 玄门
    params.stackNum = count;
    m_player->triggerEffects[TALENT_XUAN_MEN](params);
}

JX3DPS::TaiXuJianYi::SanChaiJianFa::SanChaiJianFa(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id             = SKILL_SAN_CHAI_JIAN_FA;
    m_name           = "三柴剑法";
    m_range          = 4;
    m_cooldown       = 22;
    m_globalCooldown = &m_cooldown;

    m_damageParams[0].emplace_back(0, 1024, 16);

    m_effectDamageAdditionalPercentInt = 205;
}

void JX3DPS::TaiXuJianYi::SanChaiJianFa::Cast()
{
    m_cooldownCurrent = m_cooldown * m_player->attribute.GetHastePercent();
    SubEffect();
}

void JX3DPS::TaiXuJianYi::SanChaiJianFa::Trigger() { }

void JX3DPS::TaiXuJianYi::SanChaiJianFa::Stop() { }

void JX3DPS::TaiXuJianYi::SanChaiJianFa::SubEffect()
{
    RollResult rollResult = GetPhysicsRollResult();

    Params params;
    params.rollResult = rollResult;

    // 深埋 会心
    m_player->triggerEffects[TALENT_SHEN_MAI](params);

    // 橙武特效 八荒归元无调息 持续伤害
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](params);

    GainsDamage damage = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](params);

    // 水特效
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](params);
}

JX3DPS::TaiXuJianYi::SuiXingChen::SuiXingChen(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id            = SKILL_SUI_XING_CHEN;
    m_name          = "碎星辰";
    m_range         = JX3DPS_UNLIMITED_RANGE;
    m_cooldown      = 16 * 10;
    m_prepareFrames = 16 * 1;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void JX3DPS::TaiXuJianYi::SuiXingChen::Cast()
{
    m_prepareFrames   = m_prepareFrames * m_player->attribute.GetHastePercent();
    m_cooldownCurrent = JX3DPS_INVALID_FRAMES_SET;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    m_player->SetCast(true);
}

void JX3DPS::TaiXuJianYi::SuiXingChen::Trigger()
{
    // 读条结束
    m_prepareFrames   = JX3DPS_INVALID_FRAMES_SET;
    m_cooldownCurrent = m_cooldown;
    m_player->SetLastCastSkill(m_id);
    m_player->SetCast(false);
    SubEffect();
}

void JX3DPS::TaiXuJianYi::SuiXingChen::Stop() { }

void JX3DPS::TaiXuJianYi::SuiXingChen::SubEffect()
{
    m_player->AddQidian(2);

    m_player->buffs[BUFF_FIELD_SUI_XING_CHEN]->Add();

    Params params;

    // 大附魔 腕
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

    // 长生
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    Record(PLAYER_ID, RollResult::HIT, GainsDamage(), 0, 0);
}

JX3DPS::TaiXuJianYi::ShengTaiJi::ShengTaiJi(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id            = SKILL_SHENG_TAI_JI;
    m_name          = "生太极";
    m_range         = JX3DPS_UNLIMITED_RANGE;
    m_cooldown      = 16 * 10;
    m_prepareFrames = 16 * 1;

    m_damageParams[0].emplace_back(0, 0, 0);

    if (m_player->talents[TALENT_XUAN_MEN]) {
        m_effectCooldownAdditional  = -16 * 15;
        m_cooldown                 += m_effectCooldownAdditional;
    }

    if (m_player->secrets[SECRET_SHENG_TAI_JI_PREPARE_1]) {
        m_prepareFrames -= 2;
    }

    if (m_player->secrets[SECRET_SHENG_TAI_JI_PREPARE_2]) {
        m_prepareFrames -= 2;
    }

    if (m_player->secrets[SECRET_SHENG_TAI_JI_PREPARE_3]) {
        m_prepareFrames -= 2;
    }
}

void JX3DPS::TaiXuJianYi::ShengTaiJi::Cast()
{
    m_prepareFrames   = m_prepareFrames * m_player->attribute.GetHastePercent();
    m_cooldownCurrent = JX3DPS_INVALID_FRAMES_SET;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    m_player->SetCast(true);
}

void JX3DPS::TaiXuJianYi::ShengTaiJi::Trigger()
{
    // 读条结束
    m_prepareFrames   = JX3DPS_INVALID_FRAMES_SET;
    m_cooldownCurrent = m_cooldown;

    Params params;

    // 生太极冷却秘籍
    m_player->triggerEffects[SECRET_SHENG_TAI_JI_EFFECT_COOLDOWN](params);

    m_player->SetLastCastSkill(m_id);
    m_player->SetCast(false);
    SubEffect();
}

void JX3DPS::TaiXuJianYi::ShengTaiJi::Stop() { }

void JX3DPS::TaiXuJianYi::ShengTaiJi::SubEffect()
{
    m_player->AddQidian(2);

    m_player->buffs[BUFF_FIELD_SHENG_TAI_JI]->Add();

    Params params;

    // 大附魔 腕
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

    // 长生
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    Record(PLAYER_ID, RollResult::HIT, GainsDamage(), 0, 0);
}

JX3DPS::TaiXuJianYi::TunRiYue::TunRiYue(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id            = SKILL_TUN_RI_YUE;
    m_name          = "吞日月";
    m_range         = JX3DPS_UNLIMITED_RANGE;
    m_cooldown      = 16 * 10;
    m_prepareFrames = 16 * 1;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void JX3DPS::TaiXuJianYi::TunRiYue::Cast()
{
    m_prepareFrames   = m_prepareFrames * m_player->attribute.GetHastePercent();
    m_cooldownCurrent = JX3DPS_INVALID_FRAMES_SET;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    m_player->SetCast(true);
}

void JX3DPS::TaiXuJianYi::TunRiYue::Trigger()
{
    // 读条结束
    m_prepareFrames   = JX3DPS_INVALID_FRAMES_SET;
    m_cooldownCurrent = m_cooldown;
    m_player->SetLastCastSkill(m_id);
    m_player->SetCast(false);
    SubEffect();
}

void JX3DPS::TaiXuJianYi::TunRiYue::Stop() { }

void JX3DPS::TaiXuJianYi::TunRiYue::SubEffect()
{
    m_player->AddQidian(2);

    m_player->buffs[BUFF_FIELD_TUN_RI_YUE]->Add();

    Params params;

    // 大附魔 腕
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

    // 长生
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    Record(PLAYER_ID, RollResult::HIT, GainsDamage(), 0, 0);
}

JX3DPS::TaiXuJianYi::ZiQiDongLai::ZiQiDongLai(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                 = SKILL_ZI_QI_DONG_LAI;
    m_name               = "紫气东来";
    m_range              = JX3DPS_UNLIMITED_RANGE;
    m_cooldown           = 16 * 75;
    m_prepareFrames      = 16 * 3;
    m_globalCooldown     = &m_cooldown;
    m_energyCountCurrent = m_energyCount = 2;

    m_damageParams[0].emplace_back(0, 0, 0);

    if (m_player->talents[TALENT_RUO_SHUI]) {
        m_cooldown           = 16 * 60;
        m_energyCountCurrent = m_energyCount = 3;
    }
}

void JX3DPS::TaiXuJianYi::ZiQiDongLai::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    m_energyCount--;
    m_player->SetQidian(10);

    SubEffect();
}

void JX3DPS::TaiXuJianYi::ZiQiDongLai::Trigger()
{
    // 冷却结束，充能
    m_energyCount++;
    if (m_energyCountCurrent >= m_energyCount) {
        m_energyCountCurrent = m_energyCount;
        m_cooldown           = JX3DPS_INVALID_FRAMES_SET;
    } else {
        m_cooldownCurrent = m_cooldown;
    }
}

void JX3DPS::TaiXuJianYi::ZiQiDongLai::Stop() { }

void JX3DPS::TaiXuJianYi::ZiQiDongLai::SubEffect()
{
    m_player->buffs[BUFF_ZI_QI_DONG_LAI]->Add();
    Record(PLAYER_ID, RollResult::HIT, GainsDamage(), 0, 0);
}

JX3DPS::TaiXuJianYi::JingHuaYing::JingHuaYing(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                   = SKILL_JING_HUA_YING;
    m_name                 = "镜花影";
    m_range                = 8;
    m_prepareFrames        = 16 * 3;
    m_cooldownCurrent      = JX3DPS_INVALID_FRAMES_SET;
    m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;

    m_damageParams[0].emplace_back((113 + 113 + 67) / 2, 0, 620 * 1.1 * 1.1);

    m_damageParams[1].emplace_back(200, 0, 200 * 1.1 * 1.1);
}

void JX3DPS::TaiXuJianYi::JingHuaYing::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldown = JX3DPS_INVALID_FRAMES_SET;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    m_prepareFramesCurrent = m_prepareFrames;
    SubEffect();
}

void JX3DPS::TaiXuJianYi::JingHuaYing::Trigger()
{
    // 触发 镜花影·碎
    m_prepareFrames = JX3DPS_INVALID_FRAMES_SET;
    SubEffectSui();
}

void JX3DPS::TaiXuJianYi::JingHuaYing::Stop() { }

void JX3DPS::TaiXuJianYi::JingHuaYing::TriggerAddJingHuaYing()
{
    m_cooldown = 0;
}

void JX3DPS::TaiXuJianYi::JingHuaYing::TriggerClearJingHuaYing()
{
    m_cooldown = JX3DPS_INVALID_FRAMES_SET;
}

void JX3DPS::TaiXuJianYi::JingHuaYing::SubEffect()
{

    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 0, 0);

    Params params;
    params.rollResult = rollResult;

    // 长生 持盈
    m_player->triggerEffects[TALENT_CHANG_SHENG](params);

    // 深埋 会心
    m_player->triggerEffects[TALENT_SHEN_MAI](params);

    // 大附魔 腕
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](params);

    // 大附魔 腰
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_BELT](params);

    // 大附魔 鞋
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_SHOES](params);

    // 橙武特效 八荒归元无调息 持续伤害
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](params);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](params);

    // 水特效
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](params);
}

void JX3DPS::TaiXuJianYi::JingHuaYing::SubEffectSui()
{
    if (m_targets->find(m_player->GetTargetId()) == m_targets->end()) {
        return;
    }

    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 1, 0);
    Record(m_player->GetTargetId(), rollResult, damage, 1, 0);
}

} // namespace TaiXuJianYi

} // namespace JX3DPS