/**
 * Project: 
 * File: Skill.cpp
 * Description:
 * Created Date: 2023-07-24 13:57:40
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-28 02:43:35
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "TaiXuJianYiSkill.h"

#include "Target.hpp"

namespace JX3DPS {

namespace TaiXuJianYi {

JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::WuWoWuJian(JX3DPS::Player *player, Targets *targets) :
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

    if (m_player->talents[TALENT_TONG_GEN]) {
        m_effectDamageAdditionalPercentInt += 102;
    }

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->secrets[SECRET_WU_WO_WU_JIAN_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
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

    if (m_player->equipEffects[EQUIP_EFFECT_SET_SKILL]) {
        m_effectDamageAdditionalPercentInt += 102;
    }

    if (m_player->equipEffects[EQUIP_EFFECT_WEAPON_CW]) {
        m_effectDamageAdditionalPercentInt += 51;
    }
    m_cooldown = 16;
}

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent = m_cooldown;
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::Stop() { }

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::SubEffect()
{
    int qidian = m_player->GetQidian();
    int level  = qidian - 1;

    m_player->SetQidian(0);

    // 续气秘籍
    m_player->AddQidian(m_effectQidianAdditional);

    // 无意
    m_player->triggerEffects[TALENT_WU_YI](Params());

    // 判定
    RollResult rollResult = GetPhysicsRollResult();

    // 伤害结算
    GainsDamage damage = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, level);
    Record(m_player->GetTargetId(), rollResult, damage, 0, level);

    // if (qidian >= 6) {
    //     static_cast<TaiXuJianYi::Skill::PoZhao *>(m_player->skills[SKILL_PO_ZHAO])
    //         ->TriggerDamage(m_player->GetTargetId(), 0, (qidian - 6) / 2);
    // }

    // 深埋
    m_player->triggerEffects[TALENT_SHEN_MAI](Params());

    // 无意 结束去掉状态
    m_player->triggerEffects[TALENT_WU_YI_CLEAR](Params());

    // 风逝 结束去掉状态
    m_player->triggerEffects[TALENT_FENG_SHI_CLEAR](Params());

    // 无欲
    m_player->triggerEffects[TALENT_WU_YU](Params());

    // 叠刃 需求属性 需要在无欲叠刃之后
    m_player->triggerEffects[TALENT_DIE_REN](Params());

    // 大附魔 腕 需求属性
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_WRIST](Params());

    // 大附魔 鞋 需求属性
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_SHOES](Params());

    // 长生
    m_player->triggerEffects[TALENT_CHANG_SHENG](Params());

    // 游刃 影响属性
    m_player->triggerEffects[BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN](Params());

    // 大附魔 腰 影响属性
    m_player->triggerEffects[EQUIP_EFFECT_ENCHANT_BELT](Params());

    // 橙武特效 八荒归元无调息 Dot伤害 需求属性
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_CW](Params());

    // 武器水特效 影响属性
    m_player->triggerEffects[EQUIP_EFFECT_WEAPON_WATER](Params());

    // 门派套装效果 剑鸣 影响属性
    m_player->triggerEffects[EQUIP_EFFECT_SET_ATTRIBUTE](Params());
}

void Skill::WuWoWuJian::TriggerWuYi(const Params &params)
{
    m_effectCriticalStrikeAdditionalBasisPointInt   += 1000;
    m_effectCriticalStrikePowerAdditionalPercentInt += 307;
}

void Skill::WuWoWuJian::TriggerWuYiClear(const Params &params)
{
    m_effectCriticalStrikeAdditionalBasisPointInt   -= 1000;
    m_effectCriticalStrikePowerAdditionalPercentInt -= 307;
}

void Skill::WuWoWuJian::TriggerWuYu(const Params &params) { }

JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::BaHuangGuiYuan(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_BA_HUANG_GUI_YUAN;
    m_name  = "八荒归元";
    m_range = 12;

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

    m_cooldown = 16 * 12;

    if (m_player->talents[TALENT_FENG_SHI]) {
        m_range += 2.0;
    }

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
        m_cooldown -= 16 * 1;
    }

    if (m_player->equipEffects[EQUIP_EFFECT_WEAPON_CW]) {
        m_effectDamageAdditionalPercentInt += 51;
    }
}

void JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::Cast()
{
    m_player->SetLastCastSkill(m_id);
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::Stop() { }

void JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::SubEffect()
{
    m_player->AddQidian(2 + m_effectQidianAdditional);
    int level = 10 - ((*m_targets)[m_player->GetTargetId()]->GetLifePercent() - 0.01) * 10;

    // 判定
    RollResult rollResult = GetPhysicsRollResult();

    // 伤害结算
    GainsDamage damage = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, level);
    Record(m_player->GetTargetId(), rollResult, damage, 0, level);
}

} // namespace TaiXuJianYi

} // namespace JX3DPS