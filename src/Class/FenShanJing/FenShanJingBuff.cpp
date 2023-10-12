/**
 * Project: JX3DPS
 * File: FenShanJingBuff.cpp
 * Description:
 * Created Date: 2023-08-25 20:44:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-05 16:55:22
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "FenShanJingBuff.h"

#include "FenShanJingSkill.h"
#include "Target.hpp"

namespace JX3DPS {

namespace FenShanJing {

namespace Buff {

DunFei::DunFei(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_DUN_FEI;
    m_name     = "盾飞";
    m_interval = 16 * 3;
    m_duration = 16 * 15;
}

void DunFei::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    // buff结束，不存在自判定，设置为无效帧避免频繁判定
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    //SubEffectClear(stackNum);
}

void DunFei::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) {
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        //SubEffectAdd(m_snapshots[PLAYER_ID].stackNum);
    } else {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum += stackNum;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        stack = m_snapshots[PLAYER_ID].stackNum - stack;
        //SubEffectAdd(stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void DunFei::Clear(Id_t targetId, int stackNum)
{
    int stack = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    //SubEffectClear(stack);
}

void DunFei::TriggerAdd() { }

XuRuo::XuRuo(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_DUN_FEI;
    m_name     = "盾飞";
    m_interval = 16 * 3;
    m_duration = 16 * 15;
}

void XuRuo::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    // buff结束，不存在自判定，设置为无效帧避免频繁判定
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    //SubEffectClear(stackNum);
}

void XuRuo::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) {
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        //SubEffectAdd(m_snapshots[PLAYER_ID].stackNum);
    } else {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum += stackNum;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        stack = m_snapshots[PLAYER_ID].stackNum - stack;
        //SubEffectAdd(stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void XuRuo::Clear(Id_t targetId, int stackNum)
{
    int stack = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    //SubEffectClear(stack);
}

void XuRuo::TriggerAdd() { }

} // namespace Buff

} // namespace FenShanJing

} // namespace JX3DPS
