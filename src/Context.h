#ifndef JX3DPS_CONTEXT_H
#define JX3DPS_CONTEXT_H

#include <array>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "Skill.h"
#include "Type.h"

namespace JX3DPS {
struct Context;
} // namespace JX3DPS

extern thread_local JX3DPS::Context context;

namespace JX3DPS {

constexpr static size_t UNIT_SIZE = 8;
constexpr static size_t TYPE_SIZE = 64;

struct alignas(32) FrameCache
{

    tick_t skill_cooldown[TYPE_SIZE] = {};
    tick_t skill_prepare[TYPE_SIZE]  = {};
    tick_t skill_casting[TYPE_SIZE]  = {};

    tick_t buff_cooldown[UNIT_SIZE][TYPE_SIZE] = {};
    tick_t buff_duration[UNIT_SIZE][TYPE_SIZE] = {};
    tick_t buff_interval[UNIT_SIZE][TYPE_SIZE] = {};
};

struct Context
{
    int bind_cooldown_index = 0;

    FrameCache frame_cache;

    tick_t skill_cooldown[64] = {};

    void Update(tick_t tick) noexcept;
    void Reset() noexcept;
};

template <typename Skill>
void BindCooldownImpl() noexcept
{
    Skill::cooldown[Skill::cd_index] = &context.frame_cache.skill_cooldown[context.bind_cooldown_index];
    ++Skill::cd_index;
}

template <typename... Skills>
void BindCooldown(tick_t cd) noexcept
{
    (BindCooldownImpl<Skills>(), ...);
    context.skill_cooldown[context.bind_cooldown_index] = cd;
    ++context.bind_cooldown_index;
}

} // namespace JX3DPS

#endif // JX3DPS_CONTEXT_H