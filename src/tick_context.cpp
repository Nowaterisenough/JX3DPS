/**
 * Project: JX3DPS
 * File: tick_context.cpp
 * Description: 基于 tick 的执行上下文实现
 * Created Date: 2025-10-06
 * Author: Claude
 */

#include "tick_context.h"

#include <spdlog/spdlog.h>

namespace JX3DPS {

TickContext::TickContext() { }

TickContext::~TickContext() { }

bool TickContext::Initialize(const std::vector<std::string> &skillNames)
{
    m_skillStates.clear();
    m_skillNameToIndex.clear();
    m_currentTick            = 0;
    m_globalCooldownRemaining = 0;

    int index = 0;
    for (const std::string &skillName : skillNames) {
        TickSkillState state;
        state.skillName = skillName;
        state.skillId   = index; // 暂时用索引作为 ID
        m_skillStates.push_back(state);
        m_skillNameToIndex[skillName] = index;
        index++;
    }

    spdlog::debug("[TickContext] 初始化完成，共 {} 个技能", m_skillStates.size());
    return true;
}

int TickContext::GetSkillIndex(const std::string &skillName) const
{
    auto it = m_skillNameToIndex.find(skillName);
    if (it != m_skillNameToIndex.end()) {
        return it->second;
    }
    return -1;
}

bool TickContext::IsSkillReady(int skillIndex) const
{
    if (skillIndex < 0 || skillIndex >= m_skillStates.size()) {
        return false;
    }

    // 检查技能自身冷却和全局冷却
    return m_skillStates[skillIndex].isReady &&
           m_skillStates[skillIndex].cooldownRemaining <= 0 &&
           m_globalCooldownRemaining <= 0;
}

void TickContext::CastSkill(int skillIndex, Frame_t cooldownTicks)
{
    if (skillIndex < 0 || skillIndex >= m_skillStates.size()) {
        spdlog::warn("[TickContext] 技能索引越界: {}", skillIndex);
        return;
    }

    m_skillStates[skillIndex].cooldownRemaining = cooldownTicks;
    m_skillStates[skillIndex].isReady           = false;

    spdlog::debug("[TickContext] 施放技能: {}，冷却: {} ticks",
                  m_skillStates[skillIndex].skillName, cooldownTicks);
}

void TickContext::Tick()
{
    m_currentTick++;

    // 更新全局冷却
    if (m_globalCooldownRemaining > 0) {
        m_globalCooldownRemaining--;
    }

    // 更新所有技能状态
    for (auto &skillState : m_skillStates) {
        if (skillState.cooldownRemaining > 0) {
            skillState.cooldownRemaining--;
            if (skillState.cooldownRemaining == 0) {
                skillState.isReady = true;
                spdlog::debug("[TickContext] 技能就绪: {}", skillState.skillName);
            }
        }

        if (skillState.intervalRemaining > 0) {
            skillState.intervalRemaining--;
        }
    }
}

void TickContext::Reset()
{
    m_currentTick            = 0;
    m_globalCooldownRemaining = 0;

    for (auto &skillState : m_skillStates) {
        skillState.cooldownRemaining = 0;
        skillState.intervalRemaining = 0;
        skillState.isReady           = true;
    }

    spdlog::debug("[TickContext] 上下文已重置");
}

const TickSkillState &TickContext::GetSkillState(int skillIndex) const
{
    static TickSkillState emptyState;
    if (skillIndex < 0 || skillIndex >= m_skillStates.size()) {
        return emptyState;
    }
    return m_skillStates[skillIndex];
}

void TickContext::SetGlobalCooldown(Frame_t ticks)
{
    m_globalCooldownRemaining = ticks;
    spdlog::debug("[TickContext] 设置全局冷却: {} ticks", ticks);
}

} // namespace JX3DPS
