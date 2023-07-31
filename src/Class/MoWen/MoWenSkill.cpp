/**
 * Project: JX3DPS
 * File: MoWenSkill.cpp
 * Description:
 * Created Date: 2023-07-31 16:30:22
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-01 01:47:01
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "MoWenSkill.h"

#include "MoWenBuff.h"

JX3DPS::MoWen::Skill::Gong::Gong(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id            = SKILL_GONG;
    m_name          = "宫";
    m_range         = 20;
    m_prepareFrames = 28;

    m_damageParams[0].emplace_back((160 + 160 + 40) / 2, 0, 277);
}

void JX3DPS::MoWen::Skill::Gong::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
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
}

JX3DPS::MoWen::Skill::BianGong::BianGong(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id            = SKILL_BIAN_GONG;
    m_name          = "变宫";
    m_range         = 20;
    m_prepareFrames = 28;

    m_damageParams[0].emplace_back((160 + 160 + 40) / 2, 0, 277);
}

void JX3DPS::MoWen::Skill::BianGong::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
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
}

void JX3DPS::MoWen::Skill::Shang::Cast()
{
    m_player->SetLastCastSkill(m_id);
    *m_globalCooldown =
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

    static_cast<Buff::Shang *>(m_player->buffs[BUFF_SHANG])->TriggerAdd();
}

JX3DPS::MoWen::Skill::Jue::Jue(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id    = SKILL_JUE;
    m_name  = "角";
    m_range = 20;

    m_damageParams[0].emplace_back((14 + 14 + 5) / 2, 0, 80);
}

void JX3DPS::MoWen::Skill::Jue::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(1);
}

void JX3DPS::MoWen::Skill::Jue::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::Jue::SubEffect() { }

JX3DPS::MoWen::Skill::Zhi::Zhi(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id    = SKILL_ZHI;
    m_name  = "徵";
    m_range = 20;

    m_damageParams[0].emplace_back((190 + 190 + 20) / 2, 0, 124);
}

void JX3DPS::MoWen::Skill::Zhi::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(5);
}

void JX3DPS::MoWen::Skill::Zhi::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::Zhi::SubEffect() { }

JX3DPS::MoWen::Skill::BianZhi::BianZhi(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_BIAN_ZHI;
    m_name  = "变徵";
    m_range = 20;

    m_damageParams[0].emplace_back((190 + 190 + 20) / 2, 0, 124);
}

void JX3DPS::MoWen::Skill::BianZhi::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(5);
}

void JX3DPS::MoWen::Skill::BianZhi::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::BianZhi::SubEffect() { }

JX3DPS::MoWen::Skill::Yu::Yu(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id    = SKILL_YU;
    m_name  = "羽";
    m_range = 20;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void JX3DPS::MoWen::Skill::Yu::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::Yu::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::Yu::SubEffect() { }

JX3DPS::MoWen::Skill::GaoShanLiuShui::GaoShanLiuShui(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_GAO_SHAN_LIU_SHUI;
    m_name  = "高山流水";
    m_range = 20;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void JX3DPS::MoWen::Skill::GaoShanLiuShui::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::GaoShanLiuShui::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::GaoShanLiuShui::SubEffect() { }

JX3DPS::MoWen::Skill::YangChunBaiXue::YangChunBaiXue(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_YANG_CHUN_BAI_XUE;
    m_name  = "阳春白雪";
    m_range = 20;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void JX3DPS::MoWen::Skill::YangChunBaiXue::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::YangChunBaiXue::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::YangChunBaiXue::SubEffect() { }

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
    *m_globalCooldown =
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
    *m_globalCooldown =
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

JX3DPS::MoWen::Skill::ZhengLvHeMing::ZhengLvHeMing(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = SKILL_ZHENG_LV_HE_MING;
    m_name  = "正律和鸣";
    m_range = 20;

    m_damageParams[0].emplace_back((143 + 143 + 13) / 2, 0, 208);
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing::Cast()
{
    m_player->SetLastCastSkill(m_id);
    m_cooldownCurrent      = m_cooldown * m_player->attribute.GetHastePercent();
    m_prepareFramesCurrent = m_prepareFrames * m_player->attribute.GetHastePercent();
    *m_globalCooldown =
        m_player->globalCooldown * m_player->attribute.GetHastePercent() + m_player->DelayFrames();
    static_cast<MoWen::Player *>(m_player)->AddStyleCount(4);
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing::Trigger()
{
    if (m_prepareFramesCurrent == 0) {
        SubEffect();
    }
}

void JX3DPS::MoWen::Skill::ZhengLvHeMing::SubEffect() { }