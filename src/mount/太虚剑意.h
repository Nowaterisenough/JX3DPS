#ifndef JX3DPS_TAI_XU_JIAN_YI_H
#define JX3DPS_TAI_XU_JIAN_YI_H

#include <string_view>

#include "Player.hpp"
#include "Skill.h"
#include "Type.h"

namespace JX3DPS {

struct 太虚剑意
{
    inline constinit static str_t name = "太虚剑意";

    class Player : public JX3DPS::Player<Player>
    {
    };

    class 无我无剑 : public Skill<无我无剑>
    {
    public:
        CONSTANT int physics_ap         = 0;
        CONSTANT int neutral_ap         = 0;
        CONSTANT int poison_ap          = 0;
        CONSTANT int solar_ap           = 0;
        CONSTANT int lunar_ap           = 0;
        CONSTANT int solar_and_lunar_ap = 0;

        CONSTANT int physics_fixed_damage         = 0;
        CONSTANT int neutral_fixed_damage         = 0;
        CONSTANT int poison_fixed_damage          = 0;
        CONSTANT int solar_fixed_damage           = 0;
        CONSTANT int lunar_fixed_damage           = 0;
        CONSTANT int solar_and_lunar_fixed_damage = 0;

        CONSTANT pctn_t weapon_damage_percent = 0;

        PROPERTY frame_t *prepare = nullptr;
        PROPERTY frame_t *casting = nullptr;

        CONSTANT int limit      = 0;
        CONSTANT int energy_max = 0;
        PROPERTY int energy     = 0;
    };

    class 三环套月 : public Skill<三环套月>
    {
    public:
        PROPERTY frame_t *prepare = nullptr;
        PROPERTY frame_t *casting = nullptr;

        CONSTANT int    physics_ap            = 0;
        CONSTANT int    physics_fixed_damage  = 0;
        CONSTANT pctn_t weapon_damage_percent = 0;
        CONSTANT int    limit                 = 0;
        CONSTANT int    energy_max            = 0;
        PROPERTY int    energy                = 0;
    };

    class 八荒归元 : public Skill<八荒归元>
    {
    public:
        PROPERTY frame_t *prepare = nullptr;
        PROPERTY frame_t *casting = nullptr;

        CONSTANT int    physics_ap            = 0;
        CONSTANT int    physics_fixed_damage  = 0;
        CONSTANT pctn_t weapon_damage_percent = 0;
        CONSTANT int    limit                 = 0;
        CONSTANT int    energy_max            = 0;
        PROPERTY int    energy                = 0;
    };

    class 万剑归宗 : public Skill<万剑归宗>
    {
    public:
        PROPERTY frame_t *prepare = nullptr;
        PROPERTY frame_t *casting = nullptr;

        CONSTANT int    physics_ap            = 0;
        CONSTANT int    physics_fixed_damage  = 0;
        CONSTANT pctn_t weapon_damage_percent = 0;
        CONSTANT int    limit                 = 0;
        CONSTANT int    energy_max            = 0;
        PROPERTY int    energy                = 0;
    };

    class 人剑合一 : public Skill<人剑合一>
    {
    public:
        PROPERTY frame_t *prepare = nullptr;
        PROPERTY frame_t *casting = nullptr;

        CONSTANT int    physics_ap            = 0;
        CONSTANT int    physics_fixed_damage  = 0;
        CONSTANT pctn_t weapon_damage_percent = 0;
        CONSTANT int    limit                 = 0;
        CONSTANT int    energy_max            = 0;
        PROPERTY int    energy                = 0;
    };
};

} // namespace JX3DPS

#endif // JX3DPS_TAI_XU_JIAN_YI_H
