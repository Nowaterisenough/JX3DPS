#ifndef JX3DPS_SKILL_IMPL_HPP
#define JX3DPS_SKILL_IMPL_HPP

#include "Context.h"
#include "Skill.h"

namespace JX3DPS {

template <typename T>
void Skill<T>::Cast() noexcept
{
    std::cout << "Casting skill " << typeid(T).name() << std::endl;
    ApplyCooldown();
}

template <typename T>
void Skill<T>::ApplyCooldown() noexcept
{
    for (int i = 0; i < cd_index; ++i) {
        *cooldown[i] = context.skill_cooldown[cooldown[i] - &context.frame_cache.skill_cooldown[0]];
    }
}

} // namespace JX3DPS

#endif // JX3DPS_SKILL_IMPL_HPP