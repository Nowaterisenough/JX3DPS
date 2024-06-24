#ifndef JX3DPS_LING_HAI_JUE_H
#define JX3DPS_LING_HAI_JUE_H

#include "type.h"

namespace JX3DPS {

class 凌海诀
{
public:

    class 无我无剑
    {
    public:
        CONSTANT int physics_ap         = 0;
        CONSTANT int neutral_ap         = 0;
        CONSTANT int poison_ap          = 0;
        CONSTANT int solar_ap           = 0;
        CONSTANT int lunar_ap           = 0;
        CONSTANT int solar_and_lunar_ap = 0;

        CONSTANT value_t physics_fixed_damage         = 0;
        CONSTANT value_t neutral_fixed_damage         = 0;
        CONSTANT value_t poison_fixed_damage          = 0;
        CONSTANT value_t solar_fixed_damage           = 0;
        CONSTANT value_t lunar_fixed_damage           = 0;
        CONSTANT value_t solar_and_lunar_fixed_damage = 0;

        CONSTANT pctn_t weapon_damage_percent = 0;

        PROPERTY frame_t *cooldown[4] = nullptr;
        PROPERTY frame_t *prepare     = nullptr;
        PROPERTY frame_t *casting     = nullptr;

        PROPERTY int range  = 0;
        PROPERTY int limit  = 0;
        PROPERTY int energy = 0;

        CONSTANT int energy_max = 0;
    };
};

} // namespace JX3DPS

#endif // JX3DPS_LING_HAI_JUE_H