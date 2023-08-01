/**
 * Project: JX3DPS
 * File: MoWenBuff.cpp
 * Description:
 * Created Date: 2023-08-01 23:06:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-02 00:57:11
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "MoWenBuff.h"

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
    m_snapshots[targetId].Snap(m_player->attribute,
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
    m_snapshots[targetId].Snap(m_player->attribute,
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
    m_snapshots[targetId].Snap(m_player->attribute,
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
    m_snapshots[targetId].Snap(m_player->attribute,
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