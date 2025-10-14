#ifndef JX3DPS_CORE_CONTEXT_H
#define JX3DPS_CORE_CONTEXT_H

#include <cstring>
#include <limits>
#include <string>

#include "src/global/jx3.h"
#include "src/global/types.h"

namespace JX3DPS {

// 前向声明
template <typename T>
class Player;
struct Target;

// ============= 帧缓存 =============

/**
 * @brief 帧状态缓存 - 用于高性能技能/BUFF状态管理
 *
 * 使用固定大小数组存储所有状态，避免HashMap查找开销
 * 64字节对齐，优化缓存行访问
 */
struct alignas(64) TickCache
{
    static constexpr size_t MAX_SKILLS = 64; // 最大技能数
    static constexpr size_t MAX_BUFFS  = 64; // 最大BUFF数
    static constexpr size_t MAX_UNITS  = 8;  // 最大单位数(玩家+目标)

    // 技能状态
    tick_t skill_cooldown[MAX_SKILLS] = {}; // 技能冷却
    tick_t skill_prepare[MAX_SKILLS]  = {}; // 技能前摇
    tick_t skill_casting[MAX_SKILLS]  = {}; // 技能读条

    // BUFF状态 [单位ID][BUFF ID]
    tick_t buff_cooldown[MAX_UNITS][MAX_BUFFS] = {}; // BUFF冷却
    tick_t buff_duration[MAX_UNITS][MAX_BUFFS] = {}; // BUFF持续时间
    tick_t buff_interval[MAX_UNITS][MAX_BUFFS] = {}; // BUFF间隔

    void Reset() { std::memset(this, 0, sizeof(TickCache)); }

    /**
     * @brief 获取下一个最小的tick值（用于跳帧）
     *
     * @return 最小的非零tick值，如果所有都为0则返回0
     *
     * 这个方法会在以下时刻生成关键帧：
     * 1. 技能CD/前摇/读条结束时（值=0）
     * 2. BUFF持续时间/间隔结束时（值=0）
     */
    tick_t GetMinTick() const
    {
        tick_t min_tick = std::numeric_limits<tick_t>::max();

        // 检查技能状态
        for (size_t i = 0; i < MAX_SKILLS; ++i) {
            if (skill_cooldown[i] > 0 && skill_cooldown[i] < min_tick) {
                min_tick = skill_cooldown[i];
            }
            if (skill_prepare[i] > 0 && skill_prepare[i] < min_tick) {
                min_tick = skill_prepare[i];
            }
            if (skill_casting[i] > 0 && skill_casting[i] < min_tick) {
                min_tick = skill_casting[i];
            }
        }

        // 检查BUFF状态
        for (size_t u = 0; u < MAX_UNITS; ++u) {
            for (size_t b = 0; b < MAX_BUFFS; ++b) {
                if (buff_cooldown[u][b] > 0 && buff_cooldown[u][b] < min_tick) {
                    min_tick = buff_cooldown[u][b];
                }
                if (buff_duration[u][b] > 0 && buff_duration[u][b] < min_tick) {
                    min_tick = buff_duration[u][b];
                }
                if (buff_interval[u][b] > 0 && buff_interval[u][b] < min_tick) {
                    min_tick = buff_interval[u][b];
                }
            }
        }

        return min_tick == std::numeric_limits<tick_t>::max() ? 0 : min_tick;
    }
};

// 编译期检查对齐
static_assert(sizeof(TickCache) % 64 == 0, "TickCache must be cache-line aligned");

// ============= 模拟上下文 =============

/**
 * @brief 宏时间阈值（用于生成精确关键帧）
 */
struct MacroTrigger
{
    enum class Type
    {
        SKILL_COOLDOWN, // skill_cd
        BUFF_DURATION,  // bufftime
        TBUFF_DURATION  // tbufftime
    };

    Type   type;
    size_t cache_index; // 在 TickCache 数组中的索引
    tick_t threshold;   // 阈值（当值等于此阈值时触发关键帧）

    MacroTrigger(Type t, size_t idx, tick_t thr) : type(t), cache_index(idx), threshold(thr) { }
};

/**
 * @brief 全局模拟上下文
 *
 * 线程本地存储，每个线程独立
 */
struct Context
{
    // 帧缓存
    TickCache cache;

    // 当前Tick
    tick_t current_tick = 0;

    // 玩家引用 (使用void*实现类型擦除，使用时需转换)
    void *player = nullptr;

    // 辅助函数：获取类型化的player指针
    template <typename PlayerType>
    PlayerType *GetPlayer()
    {
        return static_cast<PlayerType *>(player);
    }

    // 目标列表
    hash_t<jx3id_t, Target *> targets;

    // 技能冷却列表 (用于冷却绑定)
    tick_t skill_cooldown_list[TickCache::MAX_SKILLS] = {};

    // GCD 冷却槽位索引（指向 skill_cooldown 数组中的槽位）
    size_t gcd_slot_index = 0;

    // 宏时间阈值列表（用于生成精确关键帧）
    vector_t<MacroTrigger> macro_triggers;

    // 辅助函数：获取当前 GCD 剩余时间
    tick_t GetGlobalCooldown() const {
        return cache.skill_cooldown[gcd_slot_index];
    }

    // 辅助函数：设置 GCD
    void SetGlobalCooldown(tick_t cooldown) {
        cache.skill_cooldown[gcd_slot_index] = cooldown;
    }

    /**
     * @brief 获取下一个关键帧时刻（包含宏阈值）
     *
     * @return 最小的非零tick值
     *
     * 这个方法会在以下时刻生成关键帧：
     * 1. TickCache 中的状态变化（技能CD、BUFF持续时间等）
     * 2. 宏时间条件的阈值时刻（如 bufftime:玄门=35, skill_cd:生太极=6）
     */
    tick_t GetNextTick() const
    {
        // 先从 TickCache 获取基础的最小 tick
        tick_t min_tick = cache.GetMinTick();

        // 检查宏时间阈值
        for (const auto &threshold : macro_triggers) {
            tick_t current_value = 0;

            switch (threshold.type) {
                case MacroTrigger::Type::SKILL_COOLDOWN:
                    current_value = cache.skill_cooldown[threshold.cache_index];
                    break;
                case MacroTrigger::Type::BUFF_DURATION:
                    current_value = cache.buff_duration[0][threshold.cache_index];
                    break;
                case MacroTrigger::Type::TBUFF_DURATION:
                    // 目标BUFF，假设第一个目标
                    if (!targets.empty()) {
                        current_value = cache.buff_duration[targets.begin()->first][threshold.cache_index];
                    }
                    break;
            }

            // 如果当前值大于阈值，计算到达阈值需要的时间
            if (current_value > threshold.threshold) {
                tick_t time_to_threshold = current_value - threshold.threshold;
                if (time_to_threshold > 0 && time_to_threshold < min_tick) {
                    min_tick = time_to_threshold;
                }
            }
        }

        return min_tick;
    }

    /**
     * @brief 更新上下文 - 每帧调用
     */
    void Update(tick_t tick)
    {
        current_tick += tick;

        // 更新技能冷却
        for (size_t i = 0; i < TickCache::MAX_SKILLS; ++i) {
            if (cache.skill_cooldown[i] > 0) {
                cache.skill_cooldown[i] -= tick;
            }
            if (cache.skill_prepare[i] > 0) {
                cache.skill_prepare[i] -= tick;
            }
            if (cache.skill_casting[i] > 0) {
                cache.skill_casting[i] -= tick;
            }
        }

        // 更新BUFF状态
        for (size_t u = 0; u < TickCache::MAX_UNITS; ++u) {
            for (size_t b = 0; b < TickCache::MAX_BUFFS; ++b) {
                if (cache.buff_duration[u][b] > 0) {
                    cache.buff_duration[u][b] -= tick;
                }
                if (cache.buff_interval[u][b] > 0) {
                    cache.buff_interval[u][b] -= tick;
                }
            }
        }
    }

    /**
     * @brief 重置上下文
     */
    void Reset()
    {
        cache.Reset();
        current_tick = 0;
        macro_triggers.clear();
    }

    /**
     * @brief 从 JSON 配置初始化上下文
     *
     * @param config_json JSON 配置字符串
     * @return true 初始化成功
     * @return false 初始化失败
     *
     * JSON 格式示例（兼容 jx3dps_simulate 输入格式）:
     * {
     *   "player": {
     *     "name": "玩家名称",     // 可选
     *     "class": "太虚剑意",    // 心法名称（优先使用）
     *     "kungfu": "太虚剑意",   // 心法名称（兼容字段）
     *     "level": 120           // 等级
     *   },
     *   "target": {
     *     "name": "木桩",
     *     "level": 130
     *   },
     *   "macros": [              // 可选
     *     "/cast [buff:破|cd:无我无剑<2] 无我无剑",
     *     "/cast [bufftime:玄门>35] 生太极"
     *   ],
     *   "simulation": {          // 可选
     *     "duration": 300,
     *     "iterations": 1
     *   }
     * }
     */
    bool Init(const std::string &config_json);
};

// 线程本地上下文
extern thread_local Context context;

// ============= 冷却绑定工具 =============

/**
 * @brief 为单个技能绑定冷却
 */
template <typename Skill>
void BindCooldownImpl(int slot_index) noexcept
{
    if constexpr (requires {
                      Skill::cooldown;
                      Skill::current_cooldown;
                  })
    {
        Skill::current_cooldown = &context.cache.skill_cooldown[slot_index];
    }
}

/**
 * @brief 为多个技能绑定共享冷却
 *
 * @tparam Skills 技能类型列表
 * @param cd 冷却帧数
 *
 * 示例:
 * BindCooldown<八荒归元, 三环套月, 万剑归宗>(JX3_FRAMES_PER_SECOND * 2);
 */
template <typename... Skills>
void BindCooldown(tick_t cd) noexcept
{
    static int cooldown_slot_index = 0;

    (BindCooldownImpl<Skills>(cooldown_slot_index), ...);
    context.skill_cooldown_list[cooldown_slot_index] = cd;
    ++cooldown_slot_index;
}

// ============= 目标结构 =============

struct Target
{
    jx3id_t id     = 0;
    int     level  = JX3_PLAYER_LEVEL;
    tick_t  action = 0;

    // BUFF管理
    hash_t<jx3id_t, int> buffs; // BUFF ID -> 层数

    bool HasBuff(jx3id_t buff_id) const { return buffs.find(buff_id) != buffs.end(); }

    int GetBuffStack(jx3id_t buff_id) const
    {
        auto it = buffs.find(buff_id);
        return it != buffs.end() ? it->second : 0;
    }

    void AddBuff(jx3id_t buff_id, int stack = 1) { buffs[buff_id] += stack; }

    void RemoveBuff(jx3id_t buff_id, int stack = 1)
    {
        auto it = buffs.find(buff_id);
        if (it != buffs.end()) {
            it->second -= stack;
            if (it->second <= 0) {
                buffs.erase(it);
            }
        }
    }

    void ClearBuff(jx3id_t buff_id) { buffs.erase(buff_id); }

    void TakeDamage(long long damage)
    {
        // TODO: 记录伤害统计
    }
};

} // namespace JX3DPS

#endif // JX3DPS_CORE_CONTEXT_H
