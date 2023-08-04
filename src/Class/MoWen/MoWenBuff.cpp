/**
 * Project: JX3DPS
 * File: MoWenBuff.cpp
 * Description:
 * Created Date: 2023-08-01 23:06:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-05 03:25:20
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "MoWenBuff.h"

#include "Damage.hpp"
#include "Skill.h"

JX3DPS::MoWen::Buff::Shang::Shang(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id          = BUFF_SHANG;
    m_name        = "Dot·商";
    m_interval    = 48;
    m_effectCount = 6;

    m_damageParams[0].emplace_back((14 + 14 + 5) / 2, 0, 263);

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

void JX3DPS::MoWen::Buff::Shang::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) {
            iter->second.interval = m_interval * iter->second.hastePercent;
            SubEffect(iter->first);
        }
        if (iter->second.duration == 0) {
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void JX3DPS::MoWen::Buff::Shang::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在Dot
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt,
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration =
            m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
            m_snapshots[targetId].interval; // 对齐Dot保证每次刷新都是6跳，且消失时最后一跳
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::Shang::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(targetId) != m_snapshots.end()) {
        m_snapshots.erase(targetId);
    }
}

void JX3DPS::MoWen::Buff::Shang::TriggerAdd(Id_t targetId)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在Dot
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].duration =
        m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
        m_snapshots[targetId].interval; // 对齐Dot保证每次刷新都是6跳，且消失时最后一跳

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt,
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);
}

void JX3DPS::MoWen::Buff::Shang::SubEffect(Id_t targetId)
{
    RollResult  rollResult = GetDotRollResult(targetId);
    GainsDamage damage     = CalcMagicDotDamage(targetId, rollResult, 0, 0, 1);
    Record(targetId, rollResult, damage, 0, 0);
}

JX3DPS::MoWen::Buff::Jue::Jue(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id          = BUFF_SHANG;
    m_name        = "Dot·商";
    m_interval    = 48;
    m_effectCount = 6;

    m_damageParams[0].emplace_back((14 + 14 + 5) / 2, 0, 256);
}

void JX3DPS::MoWen::Buff::Jue::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) {
            iter->second.interval = m_interval * iter->second.hastePercent;
            SubEffect(iter->first);
        }
        if (iter->second.duration == 0) {
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void JX3DPS::MoWen::Buff::Jue::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在Dot
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt,
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration =
            m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
            m_snapshots[targetId].interval; // 对齐Dot保证每次刷新都是6跳，且消失时最后一跳
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::Jue::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(targetId) != m_snapshots.end()) {
        m_snapshots.erase(targetId);
    }
}

void JX3DPS::MoWen::Buff::Jue::TriggerAdd(Id_t targetId)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在Dot
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].duration =
        m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
        m_snapshots[targetId].interval; // 对齐Dot保证每次刷新都是6跳，且消失时最后一跳

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt,
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);
}

void JX3DPS::MoWen::Buff::Jue::SubEffect(Id_t targetId)
{
    RollResult  rollResult = GetDotRollResult(targetId);
    GainsDamage damage     = CalcMagicDotDamage(targetId, rollResult, 0, 0, 1);
    Record(targetId, rollResult, damage, 0, 0);
}

JX3DPS::MoWen::Buff::XianFeng::XianFeng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_XIAN_FENG;
    m_name     = "弦风";
    m_stackNum = 5;
    m_duration = 10 * 16;

    m_damageParams[0].emplace_back((40 + 40 + 2) / 2, 0, 40);
}

void JX3DPS::MoWen::Buff::XianFeng::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::XianFeng::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::XianFeng::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        if (m_snapshots[PLAYER_ID].stackNum <= 0) {
            m_snapshots.erase(PLAYER_ID);
        }
    }
}

void JX3DPS::MoWen::Buff::XianFeng::TriggerAdd()
{
    m_snapshots[PLAYER_ID].stackNum++;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void JX3DPS::MoWen::Buff::XianFeng::TriggerDamage(Id_t targetId, int stackNum)
{
    for (int i = 0; i < stackNum; ++i) {
        RollResult  rollResult = GetDotRollResult(targetId);
        GainsDamage damage     = CalcMagicDotDamage(targetId, rollResult, 0, 0, 1);
        Record(targetId, rollResult, damage, 0, 0);
    }
}

JX3DPS::MoWen::Buff::XianFengBiaoJi::XianFengBiaoJi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_XIAN_FENG_BIAO_JI;
    m_name     = "弦风标记";
    m_duration = 3 * 16;
}

void JX3DPS::MoWen::Buff::XianFengBiaoJi::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::XianFengBiaoJi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::XianFengBiaoJi::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::XianFengBiaoJi::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = m_duration;
}

JX3DPS::MoWen::Buff::LiuZhao::LiuZhao(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_LIU_ZHAO;
    m_name     = "流照";
    m_duration = 25 * 16;
    m_stackNum = 32;
}

void JX3DPS::MoWen::Buff::LiuZhao::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::LiuZhao::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::LiuZhao::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::LiuZhao::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = m_duration;
}

JX3DPS::MoWen::Buff::YunHan::YunHan(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_YUN_HAN;
    m_name     = "云汉";
    m_stackNum = 9;
    m_duration = 100 * 16;

    m_damageParams[0].emplace_back((40 + 40 + 2) / 2, 0, 40);
}

void JX3DPS::MoWen::Buff::YunHan::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        int stack = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots.erase(PLAYER_ID);
        SubEffectAdd(-stack);
    }
}

void JX3DPS::MoWen::Buff::YunHan::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    stack = m_snapshots[PLAYER_ID].stackNum - stack;
    SubEffectAdd(stack);
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::YunHan::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        m_snapshots[PLAYER_ID].stackNum  = std::max(m_snapshots[PLAYER_ID].stackNum, 0);
        stack                            = m_snapshots[PLAYER_ID].stackNum - stack;
        SubEffectAdd(stack);
        if (m_snapshots[PLAYER_ID].stackNum <= 0) {
            m_snapshots.erase(PLAYER_ID);
        }
    }
}

void JX3DPS::MoWen::Buff::YunHan::TriggerAdd(int stackNum)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].duration  = m_duration;
    if (m_snapshots[PLAYER_ID].stackNum <= 0) {
        m_snapshots.erase(PLAYER_ID);
    }
    SubEffectAdd(m_snapshots[PLAYER_ID].stackNum - stack);
}

void JX3DPS::MoWen::Buff::YunHan::SubEffectAdd(int stackNum)
{
    m_player->effectDamageAdditionalPercentInt += stackNum * 51;
}

JX3DPS::MoWen::Buff::CanLian::CanLian(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = BUFF_CAN_LIAN;
    m_name = "参连";
}

void JX3DPS::MoWen::Buff::CanLian::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void JX3DPS::MoWen::Buff::CanLian::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    SubEffectAdd();
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::CanLian::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        SubEffectClear();
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::CanLian::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
    SubEffectAdd();
}

void JX3DPS::MoWen::Buff::CanLian::TriggerClear()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void JX3DPS::MoWen::Buff::CanLian::SubEffectAdd()
{
    m_player->attribute.AddMagicAttackPowerBaseAdditionalPercentInt(102);
}

void JX3DPS::MoWen::Buff::CanLian::SubEffectClear()
{
    m_player->attribute.AddMagicAttackPowerBaseAdditionalPercentInt(-102);
}

JX3DPS::MoWen::Buff::ShuLi::ShuLi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_SHU_LI;
    m_name     = "书离";
    m_duration = 8 * 16;
}

void JX3DPS::MoWen::Buff::ShuLi::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void JX3DPS::MoWen::Buff::ShuLi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration += m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration += RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::ShuLi::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        SubEffectClear();
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::ShuLi::TriggerAdd()
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration += m_duration;
}

void JX3DPS::MoWen::Buff::ShuLi::TriggerClear()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void JX3DPS::MoWen::Buff::ShuLi::SubEffectAdd()
{
    m_player->skills[SKILL_GONG]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_SHANG]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_JUE]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_ZHI]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_YU]->AddDamageAdditionalPercentInt(205);

    m_player->buffs[BUFF_SHANG]->AddDamageAdditionalPercentInt(205);
    m_player->buffs[BUFF_JUE]->AddDamageAdditionalPercentInt(205);
}

void JX3DPS::MoWen::Buff::ShuLi::SubEffectClear()
{
    m_player->skills[SKILL_GONG]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_SHANG]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_JUE]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_ZHI]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_YU]->AddDamageAdditionalPercentInt(-205);

    m_player->buffs[BUFF_SHANG]->AddDamageAdditionalPercentInt(-205);
    m_player->buffs[BUFF_JUE]->AddDamageAdditionalPercentInt(-205);
}

JX3DPS::MoWen::Buff::GaoShanLiuShui::GaoShanLiuShui(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_GAO_SHAN_LIU_SHUI;
    m_name     = "高山流水";
    m_duration = 12 * 16;
}

void JX3DPS::MoWen::Buff::GaoShanLiuShui::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void JX3DPS::MoWen::Buff::GaoShanLiuShui::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::GaoShanLiuShui::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        SubEffectClear();
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::GaoShanLiuShui::TriggerAdd()
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void JX3DPS::MoWen::Buff::GaoShanLiuShui::SubEffectAdd()
{
    m_player->attribute.AddHasteBaseAdditionalPercentInt(205);
}

void JX3DPS::MoWen::Buff::GaoShanLiuShui::SubEffectClear()
{
    m_player->attribute.AddHasteBaseAdditionalPercentInt(-205);
}

JX3DPS::MoWen::Buff::YangChunBaiXue::YangChunBaiXue(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_YANG_CHUN_BAI_XUE;
    m_name     = "阳春白雪";
    m_duration = 6 * 16;
    m_stackNum = 3;
}

void JX3DPS::MoWen::Buff::YangChunBaiXue::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        int stackNum = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear(stackNum);
    }
}

void JX3DPS::MoWen::Buff::YangChunBaiXue::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_stackNum, m_snapshots[PLAYER_ID].stackNum);
    stack = m_snapshots[PLAYER_ID].stackNum - stack;
    SubEffectClear(stack);
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::YangChunBaiXue::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        m_snapshots[PLAYER_ID].stackNum  = std::max(0, m_snapshots[PLAYER_ID].stackNum);
        stack                            = stack - m_snapshots[PLAYER_ID].stackNum;
        SubEffectClear(stack);
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::YangChunBaiXue::TriggerAdd()
{
    SubEffectAdd(1);
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void JX3DPS::MoWen::Buff::YangChunBaiXue::TriggerClear()
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stackNum = m_snapshots[PLAYER_ID].stackNum;
        SubEffectClear(stackNum);
    }
}

void JX3DPS::MoWen::Buff::YangChunBaiXue::SubEffectAdd(int stackNum)
{
    m_player->skills[SKILL_GONG]->AddShieldIgnoreAdditionalPercentInt(205 * stackNum);
    m_player->skills[SKILL_ZHI]->AddShieldIgnoreAdditionalPercentInt(205 * stackNum);
}

void JX3DPS::MoWen::Buff::YangChunBaiXue::SubEffectClear(int stackNum)
{
    m_player->skills[SKILL_GONG]->AddShieldIgnoreAdditionalPercentInt(-205 * stackNum);
    m_player->skills[SKILL_ZHI]->AddShieldIgnoreAdditionalPercentInt(-205 * stackNum);
}

JX3DPS::MoWen::Buff::ZhiYinMiaoYi::ZhiYinMiaoYi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_ZHI_YIN_MIAO_YI;
    m_name     = "知音妙意";
    m_duration = 15 * 16;
    m_stackNum = 3;
}

void JX3DPS::MoWen::Buff::ZhiYinMiaoYi::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        int stackNum = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear(stackNum);
    }
}

void JX3DPS::MoWen::Buff::ZhiYinMiaoYi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_stackNum, m_snapshots[PLAYER_ID].stackNum);
    stack = m_snapshots[PLAYER_ID].stackNum - stack;
    SubEffectClear(stack);
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::MoWen::Buff::ZhiYinMiaoYi::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        m_snapshots[PLAYER_ID].stackNum  = std::max(0, m_snapshots[PLAYER_ID].stackNum);
        stack                            = stack - m_snapshots[PLAYER_ID].stackNum;
        SubEffectClear(stack);
        m_snapshots.erase(PLAYER_ID);
    }
}

void JX3DPS::MoWen::Buff::ZhiYinMiaoYi::TriggerAdd()
{
    SubEffectAdd(1);
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void JX3DPS::MoWen::Buff::ZhiYinMiaoYi::TriggerClear()
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stackNum = m_snapshots[PLAYER_ID].stackNum;
        SubEffectClear(stackNum);
    }
}

void JX3DPS::MoWen::Buff::ZhiYinMiaoYi::SubEffectAdd(int stackNum)
{
    m_player->attribute.AddMagicCriticalStrikePowerAdditionalPercentInt(102.5 * stackNum);
}

void JX3DPS::MoWen::Buff::ZhiYinMiaoYi::SubEffectClear(int stackNum)
{
    m_player->attribute.AddMagicCriticalStrikePowerAdditionalPercentInt(-102.5 * stackNum);
}

JX3DPS::MoWen::Buff::YingZi::YingZi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_YING_ZI;
    m_name     = "影子";
    m_duration = 25 * 16;
    m_stackNum = 6;

    m_damageParams[0].emplace_back((160 + 160 + 40) / 2, 0, 277);
}

void JX3DPS::MoWen::Buff::YingZi::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) {
            iter->second.interval = m_interval * iter->second.hastePercent;
            SubEffect(iter->first);
        }
        if (iter->second.duration == 0) {
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void JX3DPS::MoWen::Buff::YingZi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    for (int i = 0; i < stackNum; ++i) {
        id = static_cast<Id_t>(id + 1);
        ids.push_back(id);
        m_snapshots[id].interval = m_interval * m_player->attribute.GetHastePercent();

        // 快照属性
        m_snapshots[id].SnapMagic(m_player->attribute,
                                  m_effectCriticalStrikeAdditionalBasisPointInt,
                                  m_effectCriticalStrikePowerAdditionalPercentInt,
                                  m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

        if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_snapshots[id].duration = m_duration;
        } else [[unlikely]] {
            m_snapshots[id].duration = RandomUniform(durationMin, durationMax);
        }
    }

    int size = m_snapshots.size() - 6;
    for (int i = 0; i < size; ++i) {
        m_snapshots.erase(m_snapshots.begin());
        ids.pop_front();
    }
}

void JX3DPS::MoWen::Buff::YingZi::Clear(Id_t targetId, int stackNum)
{
    for (int i = 0; i < stackNum; ++i) {
        m_snapshots.erase(ids.front());
        ids.pop_front();
    }
}

void JX3DPS::MoWen::Buff::YingZi::TriggerAdd(int count)
{
    for (int i = 0; i < count; ++i) {
        id = static_cast<Id_t>(id + 1);
        ids.push_back(id);
        m_snapshots[static_cast<Id_t>(id)].interval =
            m_interval * m_player->attribute.GetHastePercent();

        // 快照属性
        m_snapshots[id].Snap(m_player->attribute,
                             m_effectCriticalStrikeAdditionalBasisPointInt,
                             m_effectCriticalStrikePowerAdditionalPercentInt,
                             m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

        m_snapshots[id].duration = m_duration;
    }

    int size = m_snapshots.size() - 6;
    for (int i = 0; i < size; ++i) {
        m_snapshots.erase(m_snapshots.begin());
        ids.pop_front();
    }
}

void JX3DPS::MoWen::Buff::YingZi::TriggerClear()
{
    m_snapshots.erase(ids.front());
    ids.pop_front();
}

void JX3DPS::MoWen::Buff::YingZi::SubEffect(Id_t id)
{
    Id_t        targetId   = m_player->GetTargetId();
    RollResult  rollResult = GetDotRollResult(targetId);
    GainsDamage damage     = CalcMagicYingZiDamage(targetId, rollResult, 0, 0, 1);
    RecordYingZi(targetId, rollResult, damage, 1, 0);
}

JX3DPS::Damage JX3DPS::MoWen::Buff::YingZi::GetMagicYingZiDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    int        effectCount,
    Value_t    attack,
    Value_t    weaponDamage,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain)
{
    Damage damage;

    PctFloat_t magicDamageCoefficient =
        MagicDamageCoefficient(m_damageParams.at(sub)[level].attackDamagePercentInt, 0);
    PctInt_t weaponDamageCoefficientInt = m_damageParams.at(sub)[level].weaponDamagePercentInt;
    Value_t fixedDamage = m_damageParams.at(sub)[level].fixedDamage;
    PctInt_t effectDamageAdditionalPercentInt = m_snapshots.at(targetId).effectDamageAdditionalPercentInt;
    Value_t effectDamage =
        effectCount *
        EffectDamageAll(attack, magicDamageCoefficient, weaponDamage, weaponDamageCoefficientInt, fixedDamage, effectDamageAdditionalPercentInt);

    int      playerLevel                = JX3_PLAYER_LEVEL;
    int      targetLevel                = (*m_targets)[targetId]->GetLevel();
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetMagicShield();
    Value_t  shieldAdditional           = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attribute.GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldAdditionalPercentInt = m_effectShieldIgnoreAdditionalPercentInt;
    int      rollResultInt                    = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePowerAdditionalPercentInt =
        m_snapshots.at(targetId).effectCriticalStrikePowerAdditionalPercentInt;
    PctInt_t strainPercentInt = m_snapshots.at(targetId).strainBaseAdditionalPercentInt;
    PctInt_t pveDamageAdditionalPercentInt = m_player->attribute.GetPVEDamageAdditionalPercentInt();
    PctInt_t vulnerablePercentInt = (*m_targets)[targetId]->GetDamageAdditionalPercentInt();

    damage.damage = FinalMagicDamage(
        playerLevel,
        targetLevel,
        effectDamage,
        shieldBase,
        shieldAdditional,
        ignoreShieldBasePercentInt,
        ignoreShieldAdditionalPercentInt,
        overcome,
        rollResultInt,
        criticalStrikePower,
        effectCriticalStrikePowerAdditionalPercentInt,
        strain,
        strainPercentInt,
        pveDamageAdditionalPercentInt,
        vulnerablePercentInt);

    return damage;
}

JX3DPS::GainsDamage JX3DPS::MoWen::Buff::YingZi::CalcMagicYingZiDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    int        effectCount)
{
    GainsDamage gainsDamage;

    std::array<Attribute::Type, 6> types = {
        {{ Attribute::Type::DEFAULT },
         { Attribute::Type::WEAPON_DAMAGE_BASE },
         { Attribute::Type::ATTACK_POWER_BASE },
         { Attribute::Type::CRITICAL_STRIKE_POWER },
         { Attribute::Type::OVERCOME_BASE },
         { Attribute::Type::STRAIN_BASE }}
    };

    for (const auto &type : types) {
        Value_t attack              = m_snapshots.at(targetId).attackPower;
        Value_t criticalStrikePower = m_snapshots.at(targetId).criticalStrikePower;
        Value_t strain              = m_snapshots.at(targetId).strainBase;
        Value_t overcome            = m_snapshots.at(targetId).overcome;
        Value_t weaponDamage        = m_player->attribute.GetWeaponDamage();

        switch (type) {
            case Attribute::Type::WEAPON_DAMAGE_BASE:
                m_player->attribute.AddWeaponDamageBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                weaponDamage = m_player->attribute.GetWeaponDamage();
                m_player->attribute.AddWeaponDamageBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::ATTACK_POWER_BASE:
                attack = m_snapshots.at(targetId).attackPowerGain;
                break;
            case Attribute::Type::CRITICAL_STRIKE_POWER:
                criticalStrikePower = m_snapshots.at(targetId).criticalStrikePowerGain;
                break;
            case Attribute::Type::OVERCOME_BASE:
                overcome = m_snapshots.at(targetId).overcomeGain;
                break;
            case Attribute::Type::STRAIN_BASE:
                strain = m_snapshots.at(targetId).strainBaseGain;
                break;
            default: break;
        }

        gainsDamage[type] =
            GetMagicYingZiDamage(targetId, rollResult, sub, level, effectCount, attack, weaponDamage, criticalStrikePower, overcome, strain);
    }

    return gainsDamage;
}

void JX3DPS::MoWen::Buff::YingZi::RecordYingZi(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub, int level)
{
    for (const auto &[type, damage] : gainsDamage) {
        m_stats.gainStats[type][targetId][SKILL_GONG][sub][level][rollResult].first++;
        m_stats.gainStats[type][targetId][SKILL_GONG][sub][level][rollResult].second.damage +=
            damage.damage;
        m_stats.gainStats[type][targetId][SKILL_GONG][sub][level][rollResult].second.surplusDamage +=
            damage.surplusDamage;
    }
}
