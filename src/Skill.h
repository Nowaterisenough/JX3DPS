#ifndef JX3DPS_SKILL_H
#define JX3DPS_SKILL_H

#include <iostream>

#include "Type.h"

namespace JX3DPS {

template <typename T>
class Skill
{
public:
    PROPERTY int      cd_index    = 0;
    PROPERTY frame_t *cooldown[4] = { nullptr };

    void Cast() noexcept;

protected:
    void ApplyCooldown() noexcept;
};

} // namespace JX3DPS

#endif // JX3DPS_SKILL_H