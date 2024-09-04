#ifndef JX3DPS_PLAYER_H
#define JX3DPS_PLAYER_H

#include "type.h"

namespace JX3DPS {

class Player
{
public:
    frame_t GetSkillCooldown(jx3id_t skill_id);
    bool    HasBuff(jx3id_t buff_id);
    jx3id_t    GetLastCastSkill();
    int     GetBuffStackNum(jx3id_t buff_id);
    frame_t GetBuffTime(jx3id_t buff_id);
    int     GetSkillEnergy(jx3id_t skill_id);
};

} // namespace JX3DPS

#endif // JX3DPS_PLAYER_H