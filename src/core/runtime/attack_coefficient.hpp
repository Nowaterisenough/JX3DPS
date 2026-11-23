#pragma once

#include <algorithm>

#include "src/global/types.h"

namespace JX3DPS::runtime {
// old-version-4 Damage/Damage.hpp. The AP divisor is a season parameter.
// All direct-damage callers in Skill.cpp and Buff.cpp pass zero prepare frames.
inline constexpr double DirectAttackCoefficient(int channel, double divisor)
{
    return std::max(channel, FRAMES_PER_SECOND) * 1.0 / divisor / FRAMES_PER_SECOND;
}

inline constexpr double DotAttackCoefficient(int channel, int count, int interval, double divisor)
{
    const auto duration = std::max<value_t>(FRAMES_PER_SECOND, value_t{count} * interval / 12);
    return channel * duration * 1.0 / count / divisor / FRAMES_PER_SECOND / FRAMES_PER_SECOND;
}
} // namespace JX3DPS::runtime
