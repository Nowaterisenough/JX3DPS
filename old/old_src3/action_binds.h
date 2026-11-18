#ifndef JX3DPS_ACTION_BINDS_H
#define JX3DPS_ACTION_BINDS_H

#include "type.h"

namespace JX3DPS {

namespace Action {

bool SkillNotinCd(jx3id_t skill_id);

bool BuffExist(jx3id_t buff_id);

bool NoBuffExist(jx3id_t buff_id);

bool TBuffExist(jx3id_t buff_id);

bool TNoBuffExist(jx3id_t buff_id);

bool LastCastSkill(jx3id_t buff_id);

bool NotLastCastSkill(jx3id_t buff_id);

bool BuffStackNumLt(jx3id_t buff_id, int stack_num);

bool BuffStackNumLe(jx3id_t buff_id, int stack_num);

bool BuffStackNumEq(jx3id_t buff_id, int stack_num);

bool BuffStackNumNe(jx3id_t buff_id, int stack_num);

bool BuffStackNumGe(jx3id_t buff_id, int stack_num);

bool BuffStackNumGt(jx3id_t buff_id, int stack_num);

bool TBuffStackNumLt(jx3id_t buff_id, int stack_num);

bool TBuffStackNumLe(jx3id_t buff_id, int stack_num);

bool TBuffStackNumEq(jx3id_t buff_id, int stack_num);

bool TBuffStackNumNe(jx3id_t buff_id, int stack_num);

bool TBuffStackNumGe(jx3id_t buff_id, int stack_num);

bool TBuffStackNumGt(jx3id_t buff_id, int stack_num);

bool BuffTimeLt(jx3id_t buff_id, tick_t tick);

bool BuffTimeLe(jx3id_t buff_id, tick_t tick);

bool BuffTimeEq(jx3id_t buff_id, tick_t tick);

bool BuffTimeNe(jx3id_t buff_id, tick_t tick);

bool BuffTimeGe(jx3id_t buff_id, tick_t tick);

bool BuffTimeGt(jx3id_t buff_id, tick_t tick);

bool TBuffTimeLt(jx3id_t buff_id, tick_t tick);

bool TBuffTimeLe(jx3id_t buff_id, tick_t tick);

bool TBuffTimeEq(jx3id_t buff_id, tick_t tick);

bool TBuffTimeNe(jx3id_t buff_id, tick_t tick);

bool TBuffTimeGe(jx3id_t buff_id, tick_t tick);

bool TBuffTimeGt(jx3id_t buff_id, tick_t tick);

bool SkillCooldownLt(jx3id_t skill_id, tick_t tick);

bool SkillCooldownLe(jx3id_t skill_id, tick_t tick);

bool SkillCooldownEq(jx3id_t skill_id, tick_t tick);

bool SkillCooldownNe(jx3id_t skill_id, tick_t tick);

bool SkillCooldownGe(jx3id_t skill_id, tick_t tick);

bool SkillCooldownGt(jx3id_t skill_id, tick_t tick);

bool SkillEnergyLt(jx3id_t skill_id, int energy);

bool SkillEnergyLe(jx3id_t skill_id, int energy);

bool SkillEnergyEq(jx3id_t skill_id, int energy);

bool SkillEnergyNe(jx3id_t skill_id, int energy);

bool SkillEnergyGe(jx3id_t skill_id, int energy);

bool SkillEnergyGt(jx3id_t skill_id, int energy);

bool TLifeLt(jx3id_t target_id, double ratio);

bool TLifeLe(jx3id_t target_id, double ratio);

bool TLifeEq(jx3id_t target_id, double ratio);

bool TLifeNe(jx3id_t target_id, double ratio);

bool TLifeGe(jx3id_t target_id, double ratio);

bool TLifeGt(jx3id_t target_id, double ratio);

bool DistanceLt(jx3id_t target_id, double distance);

bool DistanceLe(jx3id_t target_id, double distance);

bool DistanceEq(jx3id_t target_id, double distance);

bool DistanceNe(jx3id_t target_id, double distance);

bool DistanceGe(jx3id_t target_id, double distance);

bool DistanceGt(jx3id_t target_id, double distance);

bool ManaLt(double ratio);

bool ManaLe(double ratio);

bool ManaEq(double ratio);

bool ManaNe(double ratio);

bool ManaGe(double ratio);

bool ManaGt(double ratio);

bool NearbyEnemyLt(double distance, int count);

bool NearbyEnemyLe(double distance, int count);

bool NearbyEnemyEq(double distance, int count);

bool NearbyEnemyNe(double distance, int count);

bool NearbyEnemyGe(double distance, int count);

bool NearbyEnemyGt(double distance, int count);

bool QidianLt(int count);

bool QidianLe(int count);

bool QidianEq(int count);

bool QidianNe(int count);

bool QidianGe(int count);

bool QidianGt(int count);

} // namespace Action

} // namespace JX3DPS

#endif // __JX3DPS_EXPRESSION_H__