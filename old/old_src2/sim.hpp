#ifndef __JX3DPS_SIM_HPP__
#define __JX3DPS_SIM_HPP__

#include "player.hpp"
#include "skill.hpp"

#define property inline static thread_local
#define status   inline static thread_local

namespace JX3DPS {

enum CacheType
{
    SINGLE_TARGET,
    MULTI_TARGET
};

// 基础模板定义
template <CacheType TYPE>
struct Cache;

// 单目标版本的特化
template <>
struct Cache<CacheType::SINGLE_TARGET>
{
    constexpr static int CACHE_SIZE = 32;

    int skill_cooldown[CACHE_SIZE]               = { 0 };
    int skill_global_cooldown[CACHE_SIZE]        = { 0 };
    int skill_global_cooldown_remain[CACHE_SIZE] = { 0 };
    int skill_prepare[CACHE_SIZE]                = { 0 };
    int skill_casting[CACHE_SIZE]                = { 0 };

    int buff_duration[CACHE_SIZE] = { 0 };
    int buff_interval[CACHE_SIZE] = { 0 };

    // 单目标版本特有的成员
    int debuff_duration[CACHE_SIZE] = { 0 };
    int debuff_interval[CACHE_SIZE] = { 0 };
};

// 多目标版本的特化
template <>
struct Cache<CacheType::MULTI_TARGET> : Cache<CacheType::SINGLE_TARGET>
{
    constexpr static int TARGET_SIZE = 16;

    // 多目标版本特有的成员
    int debuff_duration[TARGET_SIZE][CACHE_SIZE] = { 0 };
    int debuff_interval[TARGET_SIZE][CACHE_SIZE] = { 0 };
};

class Target{};

using Targets = vector_t<Target>;

struct Sim
{
    Cache<CacheType::SINGLE_TARGET> cache;
    Cache<CacheType::MULTI_TARGET>  multi_cache;

    Player player;
    Target *target = nullptr;

    Targets targets;
};

inline thread_local int tls_index = 0;
inline thread_local Sim tls_sim;

// Bind 函数不再需要 global_cooldown 和 index
template <typename... Skills>
void BindGlobalCoolDown(int value, Skills &...skills)
{
    tls_sim.cache.skill_global_cooldown[tls_index] = value;

    auto assign = [&](auto &skill) {
        skill.global_cooldown[tls_index] = &tls_sim.cache.skill_global_cooldown_remain[tls_index];
    };
    (assign(skills), ...);
    tls_index += 1;
}

inline std::atomic<int> iteration = 0;

} // namespace JX3DPS

#endif // __JX3DPS_SIM_HPP__