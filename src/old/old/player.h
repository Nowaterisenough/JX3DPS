#ifndef JX3DPS_PLAYER_H
#define JX3DPS_PLAYER_H

#include "type.h"

namespace JX3DPS {

class Player
{
public:
    frame_t GetSkillCooldown(id_t skill_id);
    bool    HasBuff(id_t buff_id);
    id_t    GetLastCastSkill();
    int     GetBuffStackNum(id_t buff_id);
    frame_t GetBuffTime(id_t buff_id);
    int     GetSkillEnergy(id_t skill_id);
};

} // namespace JX3DPS

#endif // JX3DPS_PLAYER_H