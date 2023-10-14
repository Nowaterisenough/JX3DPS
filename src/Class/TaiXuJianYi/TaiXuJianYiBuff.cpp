/**
 * Project: JX3DPS
 * File: TaiXuJianYiBuff.cpp
 * Description:
 * Created Date: 2023-07-28 20:57:54
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-14 20:52:27
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "TaiXuJianYiBuff.h"

#include "TaiXuJianYiSkill.h"
#include "Target.hpp"

namespace JX3DPS {

namespace TaiXuJianYi {

namespace Buff {

DieRen::DieRen(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id          = BUFF_DIE_REN;
    m_name        = "叠刃";
    m_interval    = 16 * 3;
    m_stackNum    = 5;
    m_effectCount = 8;

    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 1);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 2);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 3);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 4);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 5);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 6);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 7);

    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 1);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 2);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 3);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 4);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 5);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 6);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 7);

    if (m_player->talents[TALENT_LIE_YUN]) {
        m_stackNum = 7;
    }

    if (m_player->talents[TALENT_XU_JI]) {
        m_damageParams[0][0].attackDamagePercentInt *= 1.2;
        m_damageParams[0][1].attackDamagePercentInt *= 1.2;
        m_damageParams[0][2].attackDamagePercentInt *= 1.2;
        m_damageParams[0][3].attackDamagePercentInt *= 1.2;
        m_damageParams[0][4].attackDamagePercentInt *= 1.2;
        m_damageParams[0][5].attackDamagePercentInt *= 1.2;
        m_damageParams[0][6].attackDamagePercentInt *= 1.2;

        m_damageParams[1][0].attackDamagePercentInt *= 1.2;
        m_damageParams[1][1].attackDamagePercentInt *= 1.2;
        m_damageParams[1][2].attackDamagePercentInt *= 1.2;
        m_damageParams[1][3].attackDamagePercentInt *= 1.2;
        m_damageParams[1][4].attackDamagePercentInt *= 1.2;
        m_damageParams[1][5].attackDamagePercentInt *= 1.2;
        m_damageParams[1][6].attackDamagePercentInt *= 1.2;
    }
}

void DieRen::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) {                      // 叠刃生效一次
            int level             = iter->second.stackNum - 1; // 层数
            iter->second.interval = m_interval * iter->second.hastePercent;
            SubEffect(iter->first, level);
        }
        if (iter->second.duration == 0) { // 叠刃消失
            SubEffectClear(iter->first);
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void DieRen::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在叠刃
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].stackNum += stackNum;
    m_snapshots[targetId].stackNum = std::min(m_snapshots[targetId].stackNum, m_stackNum);

    SubEffectAdd(targetId);

    // 快照属性
    m_snapshots[targetId].SnapPhysics(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetPhysicsCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration =
            m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
            m_snapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void DieRen::Clear(Id_t targetId, int stackNum)
{
    m_snapshots[targetId].stackNum -= stackNum;
    int stack                       = m_snapshots[targetId].stackNum;
    if (stack <= 0) [[likely]] { // 叠刃消失
        m_snapshots.erase(targetId);
    }
    SubEffectClear(targetId);
}

void DieRen::TriggerAdd(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在叠刃
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].stackNum += stackNum;
    m_snapshots[targetId].stackNum = std::min(m_snapshots[targetId].stackNum, m_stackNum);

    SubEffectAdd(targetId);

    // 快照属性
    m_snapshots[targetId].SnapPhysics(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetPhysicsCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    m_snapshots[targetId].duration =
        m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
        m_snapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
}

void DieRen::TriggerQieYu(Id_t targetId)
{
    // 切玉
    if (m_snapshots.empty()) {
        return;
    }
    int effectCount = m_snapshots[targetId].duration / m_interval * m_snapshots[targetId].hastePercent; // 剩余效果次数
    int level = m_snapshots[targetId].stackNum - 1; // 层数

    SubEffectQieYun(targetId, level, effectCount);
    SubEffectClear(targetId);
    m_snapshots.erase(targetId); // 结算完删除，避免快照清空
}

void DieRen::TriggerHuanYue(Id_t targetId)
{
    // 环月
    if (m_snapshots.empty()) {
        return;
    }
    int level = m_snapshots[targetId].stackNum - 1; // 层数
    m_snapshots[targetId].duration -=
        static_cast<Frame_t>(m_interval * m_snapshots[targetId].hastePercent);
    if (m_snapshots[targetId].duration < 0) {
        m_snapshots.erase(targetId);
        SubEffectClear(targetId);
    } else if (m_snapshots[targetId].duration == 0) {
        SubEffect(targetId, level);
        m_snapshots.erase(targetId);
        SubEffectClear(targetId);
    } else {
        SubEffect(targetId, level);
    }
}

void DieRen::SubEffect(Id_t targetId, int stackNum)
{
    RollResult  rollResult = GetDotRollResult(targetId);
    GainsDamage damage     = CalcPhysicsDotDamage(targetId, rollResult, 0, stackNum, 1);
    Record(m_id, targetId, rollResult, damage, 0, stackNum);
}

void DieRen::SubEffectQieYun(Id_t targetId, int stackNum, int effectCount)
{
    RollResult rollResult = GetDotRollResult(targetId);
    GainsDamage damage = CalcPhysicsDotDamage(targetId, rollResult, 1, stackNum, effectCount);
    Record(m_id, targetId, rollResult, damage, 1, stackNum);
}

void DieRen::SubEffectAdd(Id_t targetId)
{
    // 裂云
    Params params;
    params.player   = m_player;
    params.targetId = m_player->GetTargetId();
    m_triggerEffects[TRIGGER_LIE_YUN](params);
}

void DieRen::SubEffectClear(Id_t targetId)
{
    // 裂云
    Params params;
    params.player   = m_player;
    params.targetId = m_player->GetTargetId();
    m_triggerEffects[TRIGGER_LIE_YUN](params);
}

WanXiangGuiYuan::WanXiangGuiYuan(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id          = BUFF_WAN_XIANG_GUI_YUAN;
    m_name        = "万象归元";
    m_interval    = 16 * 3;
    m_stackNum    = 3;
    m_effectCount = 10;

    m_damageParams[0].emplace_back(20, 0, 205 * 1);
    m_damageParams[0].emplace_back(20, 0, 205 * 2);
    m_damageParams[0].emplace_back(20, 0, 205 * 3);
}

void WanXiangGuiYuan::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) {                      // 叠刃生效一次
            int level             = iter->second.stackNum - 1; // 层数
            iter->second.interval = m_interval * iter->second.hastePercent;
            SubEffect(iter->first, level);
        }
        if (iter->second.duration == 0) { // 叠刃消失
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void WanXiangGuiYuan::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在叠刃
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].stackNum += stackNum;
    m_snapshots[targetId].stackNum = std::min(m_snapshots[targetId].stackNum, m_stackNum);

    // 快照属性
    m_snapshots[targetId].SnapPhysics(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetPhysicsCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration =
            m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
            m_snapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void WanXiangGuiYuan::Clear(Id_t targetId, int stackNum)
{
    m_snapshots[targetId].stackNum -= stackNum;
    int stack                       = m_snapshots[targetId].stackNum;
    if (stack <= 0) [[likely]] { // 叠刃消失
        m_snapshots.erase(targetId);
    }
}

void WanXiangGuiYuan::TriggerAdd(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在叠刃
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].stackNum += stackNum;
    m_snapshots[targetId].stackNum = std::min(m_snapshots[targetId].stackNum, m_stackNum);

    // 快照属性
    m_snapshots[targetId].SnapPhysics(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetPhysicsCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    m_snapshots[targetId].duration =
        m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
        m_snapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
}

void WanXiangGuiYuan::SubEffect(Id_t targetId, int stackNum)
{
    RollResult  rollResult = GetDotRollResult(targetId);
    GainsDamage damage     = CalcPhysicsDotDamage(targetId, rollResult, 0, stackNum, 1);
    Record(m_id, targetId, rollResult, damage, 0, stackNum);
}

RenJianHeYi::RenJianHeYi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id          = BUFF_REN_JIAN_HE_YI;
    m_name        = "人剑合一";
    m_interval    = 16 * 3;
    m_effectCount = 4;

    m_damageParams[0].emplace_back(0, 0, 40);
}

void RenJianHeYi::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) { // 生效一次
            iter->second.interval = m_interval * iter->second.hastePercent;
            SubEffect(iter->first);
        }
        if (iter->second.duration == 0) { // 消失
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void RenJianHeYi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{

    m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();

    // 快照属性
    m_snapshots[targetId].SnapPhysics(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetPhysicsCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration =
            m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
            m_snapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void RenJianHeYi::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(targetId);
}

void RenJianHeYi::TriggerAdd(Id_t targetId)
{

    m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();

    // 快照属性
    m_snapshots[targetId].SnapPhysics(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetPhysicsCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    m_snapshots[targetId].duration =
        m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
        m_snapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
}

void RenJianHeYi::SubEffect(Id_t targetId)
{
    RollResult  rollResult = GetDotRollResult(targetId);
    GainsDamage damage     = CalcPhysicsDotDamage(targetId, rollResult, 0, 0, 1);
    Record(m_id, targetId, rollResult, damage, 0, 0);
}

ZiQiDongLai::ZiQiDongLai(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_ZI_QI_DONG_LAI;
    m_name     = "紫气东来";
    m_duration = 16 * 10;
    m_interval = 16 * 1;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void ZiQiDongLai::Trigger()
{
    if (m_snapshots[PLAYER_ID].interval == 0) [[likely]] {
        m_player->AddQidian(2);
        m_snapshots[PLAYER_ID].interval = m_interval;
    }
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void ZiQiDongLai::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) { // 不存在紫气东来
        m_snapshots[PLAYER_ID].interval = m_interval;
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration += m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration += RandomUniform(durationMin, durationMax);
    }
}

void ZiQiDongLai::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void ZiQiDongLai::TriggerAdd()
{
    if (m_snapshots.empty()) { // 不存在紫气东来
        m_snapshots[PLAYER_ID].interval = m_interval;
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration += m_duration;
}

void ZiQiDongLai::SubEffectAdd()
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(256);
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(2500);
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(256);
}

void ZiQiDongLai::SubEffectClear()
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(-256);
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(-2500);
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(-256);
}

XuanMen::XuanMen(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_XUAN_MEN;
    m_name     = "玄门";
    m_duration = 16 * 40;
    m_stackNum = 3;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void XuanMen::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(stackNum);
}

void XuanMen::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    int stack = std::min(stackNum, m_stackNum - m_snapshots[PLAYER_ID].stackNum);
    if (stack > 0) [[unlikely]] { // 玄门不满层 or 没有玄门
        m_snapshots[PLAYER_ID].stackNum += stack;
        SubEffectAdd(stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void XuanMen::Clear(Id_t targetId, int stackNum)
{
    m_snapshots[PLAYER_ID].stackNum -= stackNum;
    int stack                        = std::max(m_snapshots[PLAYER_ID].stackNum, 0);
    if (stack == 0) { // 清空玄门
        m_snapshots.erase(PLAYER_ID);
    }
    SubEffectClear(stack);
}

void XuanMen::TriggerAdd(int stackNum)
{
    int stack = std::min(stackNum, m_stackNum - m_snapshots[PLAYER_ID].stackNum);
    if (stack > 0) [[unlikely]] { // 玄门不满层 or 没有玄门
        m_snapshots[PLAYER_ID].stackNum += stack;
        SubEffectAdd(stack);
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void XuanMen::SubEffectAdd(int stackNum)
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditionalPercentInt(204 * stackNum);
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(300 * stackNum);
}

void XuanMen::SubEffectClear(int stackNum)
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditionalPercentInt(-204 * stackNum);
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(-300 * stackNum);
}

FieldSuiXingChen::FieldSuiXingChen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_FIELD_SUI_XING_CHEN;
    m_name     = "气场·碎星辰";
    m_duration = 16 * 24;
    m_interval = 16 * 3;
    m_stackNum = 3;

    m_damageParams[0].emplace_back(0, 0, 0);

    if (m_player->talents[TALENT_FU_YIN]) {
        m_duration += 16 * 12;
    }
}

void FieldSuiXingChen::Trigger()
{
    // 遍历每一个碎星辰气场
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) [[likely]] { // 碎星辰生效一次
            iter->second.interval = m_interval;
            SubEffect();
        }
        if (iter->second.duration == 0) [[unlikely]] { // 气场消失
            iter = m_snapshots.erase(iter);
            SubEffectClear(); // 删除同步的碎星辰 期声气场

            // 删除player记录的气场信息
            static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id);
        } else [[likely]] {
            ++iter;
        }
    }
}

void FieldSuiXingChen::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    // 最多三个气场
    stackNum = std::min(stackNum, m_stackNum);
    // 计算要清除的气场数量
    int stack = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size() + stackNum - m_stackNum;

    // 清除排序靠前的气场
    for (int i = 0; i < stack; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();
    }

    for (int i = 0; i < stackNum; ++i) {
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].interval = m_interval;
        if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration = m_duration;
        } else [[unlikely]] {
            m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration =
                RandomUniform(durationMin, durationMax);
        }
        static_cast<TaiXuJianYi::Player *>(m_player)->fieldId =
            static_cast<Id_t>(static_cast<TaiXuJianYi::Player *>(m_player)->fieldId + 1);
        // player记录气场信息
        static_cast<TaiXuJianYi::Player *>(m_player)->fields.emplace_back(m_id);
    }
    SubEffectAdd(stackNum);
}

void FieldSuiXingChen::Clear(Id_t targetId, int stackNum)
{
    // 计算要清除的气场数量
    stackNum = std::min(stackNum, std::min(m_stackNum, static_cast<int>(m_snapshots.size())));
    for (int i = 0; i < stackNum; ++i) {
        auto iter = m_snapshots.begin();
        iter      = m_snapshots.erase(iter);
    }
    // 清除player记录的气场信息
    static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id, stackNum);
    SubEffectClear(stackNum); // 删除同步的碎星辰 期声气场
}

void FieldSuiXingChen::TriggerAdd(int stackNum)
{
    // 最多三个气场
    stackNum = std::min(stackNum, m_stackNum);
    // 计算要清除的气场数量
    int stack = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size() + stackNum - m_stackNum;

    // 清除排序靠前的气场
    for (int i = 0; i < stack; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();
    }

    for (int i = 0; i < stackNum; ++i) {
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].interval = m_interval;
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration = m_duration;
        static_cast<TaiXuJianYi::Player *>(m_player)->fieldId =
            static_cast<Id_t>(static_cast<TaiXuJianYi::Player *>(m_player)->fieldId + 1);

        // player记录气场信息
        static_cast<TaiXuJianYi::Player *>(m_player)->fields.emplace_back(m_id);
    }
    SubEffectAdd(stackNum);
}

void FieldSuiXingChen::SubEffect()
{
    static_cast<TaiXuJianYi::Buff::SuiXingChen *>(m_player->buffs[BUFF_SUI_XING_CHEN])->TriggerAdd();
}

void FieldSuiXingChen::SubEffectAdd(int stackNum)
{
    Params params;
    params.player   = m_player;
    params.stackNum = stackNum;
    params.type     = Params::Type::ADD;
    m_triggerEffects[TRIGGER_FIELD_QI_SHENG](params);

    static_cast<TaiXuJianYi::Buff::SuiXingChen *>(m_player->buffs[BUFF_SUI_XING_CHEN])->TriggerAdd();
}

void FieldSuiXingChen::SubEffectClear(int stackNum)
{
    Params params;
    params.player   = m_player;
    params.stackNum = stackNum;
    params.type     = Params::Type::CLEAR;
    m_triggerEffects[TRIGGER_FIELD_QI_SHENG](params);
}

FieldShengTaiJi::FieldShengTaiJi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_FIELD_SHENG_TAI_JI;
    m_name     = "气场·生太极";
    m_duration = 16 * 24;
    m_interval = 16 * 3;
    m_stackNum = 3;

    m_damageParams[0].emplace_back(0, 0, 0);

    if (m_player->recipes[RECIPE_SHENG_TAI_JI_RANG_1]) {
        m_range += 1;
    }

    if (m_player->recipes[RECIPE_SHENG_TAI_JI_RANG_2]) {
        m_range += 1;
    }

    if (m_player->recipes[RECIPE_SHENG_TAI_JI_RANG_3]) {
        m_range += 1;
    }
}

void FieldShengTaiJi::Trigger()
{
    // 遍历每一个气场
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) [[likely]] { // 期声生效一次
            iter->second.interval = m_interval;
            SubEffect();
        }
        if (iter->second.duration == 0) [[unlikely]] { // 气场消失
            iter = m_snapshots.erase(iter);

            // 删除player记录的气场信息
            static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id);
        } else [[likely]] {
            ++iter;
        }
    }
}

void FieldShengTaiJi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    // 最多三个气场
    stackNum = std::min(stackNum, m_stackNum);
    // 计算要清除的气场数量
    int stack = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size() + stackNum - m_stackNum;

    // 清除排序靠前的气场
    for (int i = 0; i < stack; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();
    }

    for (int i = 0; i < stackNum; ++i) {
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].interval = m_interval;
        if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration = m_duration;
        } else [[unlikely]] {
            m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration =
                RandomUniform(durationMin, durationMax);
        }
        static_cast<TaiXuJianYi::Player *>(m_player)->fieldId =
            static_cast<Id_t>(static_cast<TaiXuJianYi::Player *>(m_player)->fieldId + 1);
        // player记录气场信息
        static_cast<TaiXuJianYi::Player *>(m_player)->fields.emplace_back(m_id);
    }

    SubEffectAdd(stackNum);
}

void FieldShengTaiJi::Clear(Id_t targetId, int stackNum)
{
    // 计算要清除的气场数量
    stackNum = std::min(stackNum, std::min(m_stackNum, static_cast<int>(m_snapshots.size())));

    for (int i = 0; i < stackNum; ++i) {
        auto iter = m_snapshots.begin();
        iter      = m_snapshots.erase(iter);
    }
    // 清除player记录的气场信息
    static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id, stackNum);
}

void FieldShengTaiJi::TriggerAdd(int stackNum)
{
    // 最多三个气场
    stackNum = std::min(stackNum, m_stackNum);
    // 计算要清除的气场数量
    int stack = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size() + stackNum - m_stackNum;

    // 清除排序靠前的气场
    for (int i = 0; i < stack; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();
    }

    for (int i = 0; i < stackNum; ++i) {
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].interval = m_interval;
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration = m_duration;
        static_cast<TaiXuJianYi::Player *>(m_player)->fieldId =
            static_cast<Id_t>(static_cast<TaiXuJianYi::Player *>(m_player)->fieldId + 1);

        // player记录气场信息
        static_cast<TaiXuJianYi::Player *>(m_player)->fields.emplace_back(m_id);
    }
    SubEffectAdd(stackNum);
}

void FieldShengTaiJi::SubEffect()
{
    Params params;
    params.player   = m_player;
    params.stackNum = 1;
    params.type     = Params::Type::ADD;
    m_triggerEffects[TRIGGER_QI_SHENG](params);
}

void FieldShengTaiJi::SubEffectAdd(int stackNum)
{
    Params params;
    params.player   = m_player;
    params.stackNum = stackNum;
    params.type     = Params::Type::ADD;
    m_triggerEffects[TRIGGER_QI_SHENG](params);
}

FieldTunRiYue::FieldTunRiYue(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_FIELD_TUN_RI_YUE;
    m_name     = "气场·吞日月";
    m_duration = 16 * 24;
    m_interval = 20;
    m_stackNum = 3;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void FieldTunRiYue::Trigger()
{
    // 遍历每一个气场
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) [[likely]] { // debuff生效一次
            iter->second.interval = m_interval;
            SubEffect();
        }
        if (iter->second.duration == 0) [[unlikely]] { // 气场消失
            iter = m_snapshots.erase(iter);

            // 删除player记录的气场信息
            static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id);
        } else [[likely]] {
            ++iter;
        }
    }
}

void FieldTunRiYue::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    // 最多三个气场
    stackNum = std::min(stackNum, m_stackNum);
    // 计算要清除的气场数量
    int stack = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size() + stackNum - m_stackNum;
    // 清除排序靠前的气场
    for (int i = 0; i < stack; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();
    }

    for (int i = 0; i < stackNum; ++i) {
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].interval = m_interval;
        if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration = m_duration;
        } else [[unlikely]] {
            m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration =
                RandomUniform(durationMin, durationMax);
        }
        static_cast<TaiXuJianYi::Player *>(m_player)->fieldId =
            static_cast<Id_t>(static_cast<TaiXuJianYi::Player *>(m_player)->fieldId + 1);
        // player记录气场信息
        static_cast<TaiXuJianYi::Player *>(m_player)->fields.emplace_back(m_id);
    }

    SubEffectAdd(stackNum);
}

void FieldTunRiYue::Clear(Id_t targetId, int stackNum)
{
    // 计算要清除的气场数量
    stackNum = std::min(stackNum, std::min(m_stackNum, static_cast<int>(m_snapshots.size())));
    for (int i = 0; i < stackNum; ++i) {
        auto iter = m_snapshots.begin();
        iter      = m_snapshots.erase(iter);
    }
    // 清除player记录的气场信息
    static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id, stackNum);
}

void FieldTunRiYue::TriggerAdd(int stackNum)
{
    // 最多三个气场
    stackNum = std::min(stackNum, m_stackNum);
    // 计算要清除的气场数量
    int stack = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size() + stackNum - m_stackNum;

    // 清除排序靠前的气场
    for (int i = 0; i < stack; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();
    }

    for (int i = 0; i < stackNum; ++i) {
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].interval = m_interval;
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration = m_duration;
        static_cast<TaiXuJianYi::Player *>(m_player)->fieldId =
            static_cast<Id_t>(static_cast<TaiXuJianYi::Player *>(m_player)->fieldId + 1);

        // player记录气场信息
        static_cast<TaiXuJianYi::Player *>(m_player)->fields.emplace_back(m_id);
    }
    SubEffectAdd(stackNum);
}

void FieldTunRiYue::SubEffect()
{
    // TODO : 没考虑距离
    for (auto &target : *m_targets) {
        static_cast<TunRiYue *>(m_player->buffs[BUFF_TUN_RI_YUE])->TriggerAdd(target.first);
    }
}

void FieldTunRiYue::SubEffectAdd(int stackNum)
{
    // TODO : 没考虑距离
    for (auto &target : *m_targets) {
        static_cast<TunRiYue *>(m_player->buffs[BUFF_TUN_RI_YUE])->TriggerAdd(target.first);
    }
}

FieldSuiXingChenQiSheng::FieldSuiXingChenQiSheng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_FIELD_SUI_XING_CHEN_QI_SHENG;
    m_name     = "气场·碎星辰·期声";
    m_duration = 16 * 24;
    m_interval = 16 * 3;
    m_stackNum = 3;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void FieldSuiXingChenQiSheng::Trigger()
{
    // 遍历每一个气场
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end(); ++iter) {
        if (iter->second.interval == 0) [[likely]] { // 期声生效一次
            iter->second.interval = m_interval;
        }
    }
    SubEffect();
}

void FieldSuiXingChenQiSheng::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{

    for (int i = 0; i < stackNum; ++i) {
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].interval = m_interval;
        m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration =
            JX3DPS_INVALID_FRAMES_SET;
        static_cast<TaiXuJianYi::Player *>(m_player)->fieldId =
            static_cast<Id_t>(static_cast<TaiXuJianYi::Player *>(m_player)->fieldId + 1);
    }
    SubEffect();
}

void FieldSuiXingChenQiSheng::Clear(Id_t targetId, int stackNum)
{
    for (int i = 0; i < stackNum; ++i) {
        auto iter = m_snapshots.begin();
        iter      = m_snapshots.erase(iter);
    }
}

void FieldSuiXingChenQiSheng::TriggerAdd(int stackNum)
{
    if (stackNum > 0) {

        for (int i = 0; i < stackNum; ++i) {
            m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].interval = m_interval;
            m_snapshots[static_cast<TaiXuJianYi::Player *>(m_player)->fieldId].duration =
                JX3DPS_INVALID_FRAMES_SET;
            static_cast<TaiXuJianYi::Player *>(m_player)->fieldId =
                static_cast<Id_t>(static_cast<TaiXuJianYi::Player *>(m_player)->fieldId + 1);
        }
        SubEffect();
    } else {
        for (int i = 0; i < -stackNum; ++i) {
            m_snapshots.erase(m_snapshots.begin());
        }
    }
}

void FieldSuiXingChenQiSheng::SubEffect()
{
    static_cast<QiSheng *>(m_player->buffs[BUFF_QI_SHENG])->TriggerAdd();
}

SuiXingChen::SuiXingChen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_SUI_XING_CHEN;
    m_name     = "碎星辰";
    m_duration = 16 * 4;

    m_damageParams[0].emplace_back(0, 0, 0);

    m_effectCriticalStrikeAdditionalBasisPointInt   = 500;
    m_effectCriticalStrikePowerAdditionalPercentInt = 102;

    if (m_player->talents[TALENT_FU_YIN]) {
        m_effectCriticalStrikeAdditionalBasisPointInt   = 1000;
        m_effectCriticalStrikePowerAdditionalPercentInt = 205;
    }
}

void SuiXingChen::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void SuiXingChen::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) [[unlikely]] { // buff不存在时，添加buff
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void SuiXingChen::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void SuiXingChen::TriggerAdd()
{
    if (m_snapshots.empty()) [[unlikely]] { // buff不存在时，添加buff
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void SuiXingChen::SubEffectAdd()
{
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(m_effectCriticalStrikeAdditionalBasisPointInt);
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(m_effectCriticalStrikePowerAdditionalPercentInt);

    // 故长
    Params params;
    params.player = m_player;
    params.type   = Params::Type::ADD;
    m_triggerEffects[TRIGGER_GU_CHANG](params);
}

void SuiXingChen::SubEffectClear()
{
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(-m_effectCriticalStrikeAdditionalBasisPointInt);
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(-m_effectCriticalStrikePowerAdditionalPercentInt);

    // 故长
    Params params;
    params.player = m_player;
    params.type   = Params::Type::CLEAR;
    m_triggerEffects[TRIGGER_GU_CHANG](params);
}

QiSheng::QiSheng(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_QI_SHENG;
    m_name     = "期声";
    m_duration = 16 * 3;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void QiSheng::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void QiSheng::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) { // buff不存在时，添加buff
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void QiSheng::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void QiSheng::TriggerAdd()
{
    if (m_snapshots.empty()) { // buff不存在时，添加buff
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void QiSheng::SubEffectAdd()
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(102);
}

void QiSheng::SubEffectClear()
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(-102);
}

JianRu::JianRu(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_JIAN_RU;
    m_name     = "剑入";
    m_duration = 16 * 6;
    m_interval = 16;

    m_damageParams[0].emplace_back(0, 0, 84 * 1.2 * 1.1 * 2 * 1.6774);
    m_damageParams[1].emplace_back(0, 0, 280 * 1.1 * 1.25558);
}

void JianRu::Trigger()
{
    if (m_snapshots[PLAYER_ID].interval == 0) {
        TriggerDamage(0);
        m_snapshots[PLAYER_ID].interval = m_interval;
        return;
    }
    m_snapshots.erase(PLAYER_ID);
}

void JianRu::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JianRu::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
}

void JianRu::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void JianRu::TriggerActive()
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots[PLAYER_ID].interval = m_interval;
    }
}

void JianRu::TriggerDamage(int index)
{
    RollResult rollResult = GetPhysicsRollResult();

    Params params;
    params.player     = m_player;
    params.rollResult = rollResult;

    // 大附魔 腕
    m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

    // 大附魔 腰
    m_triggerEffects[TRIGGER_ENCHANT_BELT](params);

    // 大附魔 鞋
    m_triggerEffects[TRIGGER_ENCHANT_SHOES](params);

    GainsDamage damage = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, index, 0);
    Record(m_id, m_player->GetTargetId(), rollResult, damage, index, 0);

    int count = 0;
    for (auto &[targetId, target] : *m_targets) {
        if (targetId == m_player->GetTargetId()) {
            continue;
        }
        RollResult rollResult = GetPhysicsRollResult();
        params.rollResult     = rollResult;
        // 大附魔 腕
        m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

        // 大附魔 腰
        m_triggerEffects[TRIGGER_ENCHANT_BELT](params);

        // 大附魔 鞋
        m_triggerEffects[TRIGGER_ENCHANT_SHOES](params);

        GainsDamage damage = CalcPhysicsDamage(targetId, GetPhysicsRollResult(), index, 0);
        Record(m_id, targetId, GetPhysicsRollResult(), damage, index, 0);
        count++;
        if (count == 2) {
            break;
        }
    }
}

FengShi::FengShi(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_FENG_SHI;
    m_name     = "风逝";
    m_duration = 16 * 5;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void FengShi::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void FengShi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) [[likely]] { // buff不存在时，添加buff
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void FengShi::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void FengShi::TriggerAdd()
{
    if (m_snapshots.empty()) [[likely]] { // buff不存在时，添加buff
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void FengShi::TriggerClear()
{
    if (!m_snapshots.empty()) [[likely]] { // buff不存在时，添加buff
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void FengShi::SubEffectAdd()
{
    static_cast<Skill::WuWoWuJian *>(m_player->skills[SKILL_WU_WO_WU_JIAN])->TriggerFengShiAdd();
}

void FengShi::SubEffectClear()
{
    static_cast<Skill::WuWoWuJian *>(m_player->skills[SKILL_WU_WO_WU_JIAN])->TriggerFengShiClear();
}

TunRiYue::TunRiYue(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_TUN_RI_YUE;
    m_name     = "吞日月";
    m_duration = 20;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void TunRiYue::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.duration == 0) {
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void TunRiYue::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void TunRiYue::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(targetId);
}

void TunRiYue::TriggerAdd(Id_t targetId)
{
    m_snapshots[targetId].duration = m_duration;
}

JingHuaYing::JingHuaYing(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_JING_HUA_YING;
    m_name     = "镜花影";
    m_duration = 16 * 8;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void JingHuaYing::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void JingHuaYing::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
    SubEffectAdd();
}

void JingHuaYing::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void JingHuaYing::TriggerAdd()
{
    SubEffectAdd();
}

void JingHuaYing::SubEffectAdd()
{
    static_cast<Skill::JingHuaYing *>(m_player->skills[SKILL_JING_HUA_YING])->TriggerAddJingHuaYing();
}

void JingHuaYing::SubEffectClear()
{
    static_cast<Skill::JingHuaYing *>(m_player->skills[SKILL_JING_HUA_YING])->TriggerClearJingHuaYing();
}

FieldLieYun::FieldLieYun(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_HIDDEN_LIE_YUN;
    m_name     = "隐藏·裂云";
    m_duration = 16 * 24;
    m_interval = 8;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void FieldLieYun::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) [[likely]] {
            iter->second.interval = m_interval;
            SubEffectAdd(iter->first);
        }
        if (iter->second.duration == 0) [[unlikely]] {
            iter = m_snapshots.erase(iter);
        } else [[likely]] {
            ++iter;
        }
    }
}

void FieldLieYun::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) [[unlikely]] {
        m_snapshots[targetId].interval = m_interval;
        SubEffectAdd(targetId);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void FieldLieYun::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(targetId);
}

void FieldLieYun::TriggerAdd(Id_t targetId)
{
    if (m_player->buffs[BUFF_DIE_REN]->GetStackNumCurrent(targetId) < 4) {
        if (m_snapshots.find(targetId) != m_snapshots.end()) {
            m_snapshots.erase(targetId);
        }
        return;
    }
    if (m_snapshots.find(targetId) == m_snapshots.end()) [[unlikely]] {
        m_snapshots[targetId].interval = m_interval;
        SubEffectAdd(targetId);
    }
    m_snapshots[targetId].duration = m_duration;
}

void FieldLieYun::SubEffectAdd(Id_t targetId)
{
    if ((*m_targets)[targetId]->GetDistance() <= 15) {
        static_cast<LieYun *>(m_player->buffs[BUFF_LIE_YUN])->TriggerAdd();
    }
}

LieYun::LieYun(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_LIE_YUN;
    m_name     = "裂云";
    m_duration = 16 * 1;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void LieYun::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void LieYun::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void LieYun::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void LieYun::TriggerAdd()
{
    if (m_snapshots.empty()) {
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void LieYun::SubEffectAdd()
{
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(154);
}

void LieYun::SubEffectClear()
{
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(-154);
}

ChiYing::ChiYing(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_CHI_YING;
    m_name     = "持盈";
    m_duration = 16 * 15;
    m_stackNum = 3;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 127);
}

void ChiYing::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
}

void ChiYing::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void ChiYing::Clear(Id_t targetId, int stackNum)
{
    m_snapshots[PLAYER_ID].stackNum -= stackNum;
    if (m_snapshots[PLAYER_ID].stackNum <= 0) [[likely]] {
        m_snapshots.erase(PLAYER_ID);
    }
}

void ChiYing::TriggerAdd(int stackNum)
{
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void ChiYing::TriggerDamage()
{
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_id, m_player->GetTargetId(), rollResult, damage, 0, 0);
    m_snapshots[PLAYER_ID].stackNum--;
    if (m_snapshots[PLAYER_ID].stackNum == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
}

YunZhongJianShengTaiJi::YunZhongJianShengTaiJi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI;
    m_name     = "云中剑·生太极";
    m_range    = 6.0;
    m_duration = 16 * 8;
    m_interval = 16 * 1;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 70 * 1.1);
}

void YunZhongJianShengTaiJi::Trigger()
{
    if (m_snapshots[PLAYER_ID].interval == 0) [[likely]] {
        m_snapshots[PLAYER_ID].interval = m_interval;
        SubEffect();
    }
    if (m_snapshots[PLAYER_ID].duration == 0) [[unlikely]] {
        m_snapshots.erase(PLAYER_ID);
    }
}

void YunZhongJianShengTaiJi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) [[likely]] {
        m_snapshots[PLAYER_ID].interval = m_interval;
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void YunZhongJianShengTaiJi::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
}

void YunZhongJianShengTaiJi::TriggerAdd()
{
    if (m_snapshots.empty()) [[likely]] {
        m_snapshots[PLAYER_ID].interval = m_interval;
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void YunZhongJianShengTaiJi::SubEffect()
{
    RollResult rollResult;
    Params     params;

    int count = 0;
    for (auto &[id, target] : *m_targets) {
        if (target->GetDistance() <= m_range) {
            count++;
            if (count == 4) {
                break;
            }

            rollResult = GetPhysicsRollResult();

            params.player     = m_player;
            params.rollResult = rollResult;

            // 大附魔 腕
            m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

            // 大附魔 腰
            m_triggerEffects[TRIGGER_ENCHANT_BELT](params);

            // 大附魔 鞋
            m_triggerEffects[TRIGGER_ENCHANT_SHOES](params);

            GainsDamage damage = CalcPhysicsDamage(id, rollResult, 0, 0);
            Record(m_id, id, rollResult, damage, 0, 0);
        }
    }

    // 剑入
    m_triggerEffects[TRIGGER_JIAN_RU](params);
}

YunZhongJianSuiXingChen::YunZhongJianSuiXingChen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN;
    m_name     = "云中剑·碎星辰";
    m_range    = 6.0;
    m_duration = 16 * 8;
    m_interval = 16 * 1;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 70 * 1.1);
}

void YunZhongJianSuiXingChen::Trigger()
{
    if (m_snapshots[PLAYER_ID].interval == 0) [[likely]] {
        m_snapshots[PLAYER_ID].interval = m_interval;
        SubEffect();
    }
    if (m_snapshots[PLAYER_ID].duration == 0) [[unlikely]] {
        m_snapshots.erase(PLAYER_ID);
    }
}

void YunZhongJianSuiXingChen::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) [[likely]] {
        m_snapshots[PLAYER_ID].interval = m_interval;
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void YunZhongJianSuiXingChen::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
}

void YunZhongJianSuiXingChen::TriggerAdd()
{
    if (m_snapshots.empty()) [[likely]] {
        m_snapshots[PLAYER_ID].interval = m_interval;
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void YunZhongJianSuiXingChen::SubEffect()
{
    RollResult rollResult;
    Params     params;

    int count = 0;
    for (auto &[id, target] : *m_targets) {
        if (target->GetDistance() <= m_range) {
            count++;
            if (count == 4) {
                break;
            }

            rollResult = GetPhysicsRollResult();

            params.player     = m_player;
            params.rollResult = rollResult;

            // 大附魔 腕
            m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

            // 大附魔 腰
            m_triggerEffects[TRIGGER_ENCHANT_BELT](params);

            // 大附魔 鞋
            m_triggerEffects[TRIGGER_ENCHANT_SHOES](params);

            GainsDamage damage = CalcPhysicsDamage(id, rollResult, 0, 0);
            Record(m_id, id, rollResult, damage, 0, 0);
        }
    }

    // 剑入
    m_triggerEffects[TRIGGER_JIAN_RU](params);
}

YunZhongJianTunRiYue::YunZhongJianTunRiYue(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE;
    m_name     = "云中剑·吞日月";
    m_range    = 6.0;
    m_duration = 16 * 8;
    m_interval = 16 * 1;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 70 * 1.1);
}

void YunZhongJianTunRiYue::Trigger()
{
    if (m_snapshots[PLAYER_ID].interval == 0) [[likely]] {
        m_snapshots[PLAYER_ID].interval = m_interval;
        SubEffect();
    }
    if (m_snapshots[PLAYER_ID].duration == 0) [[unlikely]] {
        m_snapshots.erase(PLAYER_ID);
    }
}

void YunZhongJianTunRiYue::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) [[likely]] {
        m_snapshots[PLAYER_ID].interval = m_interval;
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void YunZhongJianTunRiYue::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
}

void YunZhongJianTunRiYue::TriggerAdd()
{
    if (m_snapshots.empty()) [[likely]] {
        m_snapshots[PLAYER_ID].interval = m_interval;
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void YunZhongJianTunRiYue::SubEffect()
{
    RollResult rollResult;
    Params     params;

    int count = 0;
    for (auto &[id, target] : *m_targets) {
        if (target->GetDistance() <= m_range) {
            count++;
            if (count == 4) {
                break;
            }

            rollResult = GetPhysicsRollResult();

            params.player     = m_player;
            params.rollResult = rollResult;

            // 大附魔 腕
            m_triggerEffects[TRIGGER_ENCHANT_WRIST](params);

            // 大附魔 腰
            m_triggerEffects[TRIGGER_ENCHANT_BELT](params);

            // 大附魔 鞋
            m_triggerEffects[TRIGGER_ENCHANT_SHOES](params);

            GainsDamage damage = CalcPhysicsDamage(id, rollResult, 0, 0);
            Record(m_id, id, rollResult, damage, 0, 0);
        }
    }

    // 剑入
    m_triggerEffects[TRIGGER_JIAN_RU](params);
}

ClassFeature::ClassFeature(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_CLASS_FEATURE;
    m_name     = "太虚剑意·融金";
    m_interval = 16 * 1;

    m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;

    m_damageParams[0].emplace_back(0, 0, 0);
}

void ClassFeature::Trigger()
{
    m_snapshots[PLAYER_ID].interval = m_interval;
    SubEffectAdd();
}

void ClassFeature::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_snapshots[PLAYER_ID].interval = RandomUniform(0, m_interval - 1);
    m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
    SubEffectAdd();
}

void ClassFeature::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
}

void ClassFeature::SubEffectAdd()
{
    m_player->AddQidian(1);
}

WeaponEffectCW::WeaponEffectCW(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_WEAPON_EFFECT_CW;
    m_name            = "武器·橙武特效";
    m_duration        = 16 * 6;
    m_cooldownCurrent = m_cooldown = 16 * 30;
}

void WeaponEffectCW::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void WeaponEffectCW::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void WeaponEffectCW::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void WeaponEffectCW::TriggerAdd()
{
    if (m_cooldownCurrent <= 0) {
        m_snapshots[PLAYER_ID].duration = m_duration;
        m_cooldownCurrent               = m_cooldown;
        SubEffectAdd();
    }
}

void WeaponEffectCW::SubEffectAdd()
{
    static_cast<TaiXuJianYi::Skill::BaHuangGuiYuan *>(m_player->skills[SKILL_BA_HUANG_GUI_YUAN])
        ->ClearCooldown();
}

void WeaponEffectCW::SubEffectClear()
{
    static_cast<TaiXuJianYi::Skill::BaHuangGuiYuan *>(m_player->skills[SKILL_BA_HUANG_GUI_YUAN])
        ->ResetCooldown();
}

SetAttribute::SetAttribute(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_SET_ATTRIBUTE;
    m_name     = "套装·剑鸣";
    m_duration = 16 * 6;
}

void SetAttribute::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void SetAttribute::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void SetAttribute::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void SetAttribute::TriggerAdd()
{
    if (RandomUniform(1, 100) <= 10) {
        if (m_snapshots.empty()) {
            SubEffectAdd();
        }
        m_snapshots[PLAYER_ID].duration = m_duration;
    }
}

void SetAttribute::SubEffectAdd()
{
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(400);
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(41);
}

void SetAttribute::SubEffectClear()
{
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(-400);
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(-41);
}

TeamCoreTaiXuJianYiYouRen::TeamCoreTaiXuJianYiYouRen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN;
    m_name     = "游刃";
    m_duration = 20 * 16;
    m_stackNum = 5;
}

void TeamCoreTaiXuJianYiYouRen::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    // buff结束，不存在自判定，设置为无效帧避免频繁判定
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(stackNum);
}

void TeamCoreTaiXuJianYiYouRen::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) {
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        SubEffectAdd(m_snapshots[PLAYER_ID].stackNum);
    } else {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum += stackNum;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        stack = m_snapshots[PLAYER_ID].stackNum - stack;
        SubEffectAdd(stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void TeamCoreTaiXuJianYiYouRen::Clear(Id_t targetId, int stackNum)
{
    int stack = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(stack);
}

void TeamCoreTaiXuJianYiYouRen::TriggerAdd()
{
    if (m_snapshots.empty()) {
        m_snapshots[PLAYER_ID].stackNum = 1;
        SubEffectAdd(1);
    } else {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum += 1;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        stack = m_snapshots[PLAYER_ID].stackNum - stack;
        SubEffectAdd(stack);
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void TeamCoreTaiXuJianYiYouRen::SubEffectAdd(int stackNum)
{
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(100 * stackNum);
}

void TeamCoreTaiXuJianYiYouRen::SubEffectClear(int stackNum)
{
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(-100 * stackNum);
}

TeamCoreTaiXuJianYiJingMiao::TeamCoreTaiXuJianYiJingMiao(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_TEAM_CORE_TAI_XU_JIAN_YI_JING_MIAO;
    m_name     = "精妙";
    m_cooldown = 6 * 16;

    m_snapshots[PLAYER_ID].duration = RandomUniform(0, 10);
}

void TeamCoreTaiXuJianYiJingMiao::Trigger()
{
    m_snapshots[PLAYER_ID].duration = m_cooldown + RandomUniform(0, 10);
    SubEffect();
}

void TeamCoreTaiXuJianYiJingMiao::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
}

void TeamCoreTaiXuJianYiJingMiao::Clear(Id_t targetId, int stackNum) { }

void TeamCoreTaiXuJianYiJingMiao::SubEffect()
{
    m_player->AddQidian(2);
}

} // namespace Buff

} // namespace TaiXuJianYi

} // namespace JX3DPS
