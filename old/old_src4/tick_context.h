/**
 * Project: JX3DPS
 * File: tick_context.h
 * Description: 基于 tick 的执行上下文，用数组存储技能状态
 * Created Date: 2025-10-06
 * Author: Claude
 */

#ifndef __JX3DPS_TICK_CONTEXT_H__
#define __JX3DPS_TICK_CONTEXT_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "global/Types.h"

namespace JX3DPS {

/**
 * @brief 基于 tick 的技能状态
 */
struct TickSkillState
{
    Frame_t     cooldownRemaining = 0;    // 剩余冷却 tick
    Frame_t     intervalRemaining = 0;    // 剩余间隔 tick (对于 /interval 指令)
    bool        isReady           = true; // 技能是否就绪
    std::string skillName;                // 技能名称
    int         skillId           = 0;    // 技能ID (如果需要)
};

/**
 * @brief Tick-based 执行上下文
 *
 * 所有状态用数组按索引存储，避免复杂的 KeyFrame 查找
 */
class TickContext
{
public:
    TickContext();
    ~TickContext();

    /**
     * @brief 初始化上下文，设置技能列表
     * @param skillNames 技能名称列表
     * @return 是否成功
     */
    bool Initialize(const std::vector<std::string> &skillNames);

    /**
     * @brief 获取技能索引
     * @param skillName 技能名称
     * @return 技能索引，-1 表示未找到
     */
    int GetSkillIndex(const std::string &skillName) const;

    /**
     * @brief 检查技能是否就绪
     * @param skillIndex 技能索引
     * @return 是否就绪
     */
    bool IsSkillReady(int skillIndex) const;

    /**
     * @brief 施放技能（设置冷却）
     * @param skillIndex 技能索引
     * @param cooldownTicks 冷却 tick 数
     */
    void CastSkill(int skillIndex, Frame_t cooldownTicks);

    /**
     * @brief 推进一个 tick
     *
     * 所有技能的冷却和间隔减 1
     */
    void Tick();

    /**
     * @brief 获取当前 tick 计数
     */
    Frame_t GetCurrentTick() const { return m_currentTick; }

    /**
     * @brief 重置上下文
     */
    void Reset();

    /**
     * @brief 获取技能状态（用于调试显示）
     */
    const TickSkillState &GetSkillState(int skillIndex) const;

    /**
     * @brief 获取所有技能状态
     */
    const std::vector<TickSkillState> &GetAllSkillStates() const { return m_skillStates; }

    /**
     * @brief 设置全局冷却 (GCD)
     * @param ticks GCD tick 数
     */
    void SetGlobalCooldown(Frame_t ticks);

    /**
     * @brief 检查 GCD 是否就绪
     */
    bool IsGlobalCooldownReady() const { return m_globalCooldownRemaining <= 0; }

private:
    Frame_t                              m_currentTick            = 0; // 当前 tick 计数
    Frame_t                              m_globalCooldownRemaining = 0; // 全局冷却剩余 tick
    std::vector<TickSkillState>          m_skillStates;                // 技能状态数组
    std::unordered_map<std::string, int> m_skillNameToIndex;           // 技能名称到索引的映射
};

} // namespace JX3DPS

#endif // __JX3DPS_TICK_CONTEXT_H__
