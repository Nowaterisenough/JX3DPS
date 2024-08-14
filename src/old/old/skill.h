#ifndef JX3DPS_SKILL_H
#define JX3DPS_SKILL_H

#include "type.h"

namespace JX3DPS {

template <typename T>
class Skill
{
public:
    Skill()  = default;
    ~Skill() = default;

protected:
    frame_t *_cooldown[4]       = { nullptr };
    frame_t *_check_cooldown[4] = { nullptr };
    frame_t *_prepare           = nullptr;

    struct Addon
    {
        pctn_t  channel_interval      = 0; // 系数加成
        pctn_t  critical_strike       = 0; // 会心加成
        pctn_t  critical_strike_power = 0; // 会心效果加成
        pctn_t  damage                = 0; // 伤害加成
        pctn_t  move_damage           = 0; // 位移状态伤害加成
        pctn_t  fixed_damage          = 0; // 固定伤害加成
        pctn_t  global_damage_factor  = 0; // 全局伤害加成
        pctn_t  shield_ignore         = 0; // 无视防御加成
        frame_t cooldown              = 0; // 冷却减免

        int qidian     = 0;
        int rage       = 0;
        int energy     = 0;
        int sun        = 0;
        int moon       = 0;
        int sun_power  = 0;
        int moon_power = 0;

    } _addon;
};

} // namespace JX3DPS

#endif // JX3DPS_SKILL_H