#ifndef JX3DPS_ACTION_BINDS_H
#define JX3DPS_ACTION_BINDS_H

#include "type.h"

namespace JX3DPS {

namespace Action {

bool SkillNotinCd(id_t skill_id);

bool BuffExist(id_t buff_id);

bool NoBuffExist(id_t buff_id);

bool TBuffExist(id_t buff_id);

bool TNoBuffExist(id_t buff_id);

bool LastCastSkill(id_t buff_id);

bool NotLastCastSkill(id_t buff_id);

bool BuffStackNumLt(id_t buff_id, int stack_num);

bool BuffStackNumLe(id_t buff_id, int stack_num);

bool BuffStackNumEq(id_t buff_id, int stack_num);

bool BuffStackNumNe(id_t buff_id, int stack_num);

bool BuffStackNumGe(id_t buff_id, int stack_num);

bool BuffStackNumGt(id_t buff_id, int stack_num);

bool TBuffStackNumLt(id_t buff_id, int stack_num);

bool TBuffStackNumLe(id_t buff_id, int stack_num);

bool TBuffStackNumEq(id_t buff_id, int stack_num);

bool TBuffStackNumNe(id_t buff_id, int stack_num);

bool TBuffStackNumGe(id_t buff_id, int stack_num);

bool TBuffStackNumGt(id_t buff_id, int stack_num);

bool BuffTimeLt(id_t buff_id, frame_t frame);

bool BuffTimeLe(id_t buff_id, frame_t frame);

bool BuffTimeEq(id_t buff_id, frame_t frame);

bool BuffTimeNe(id_t buff_id, frame_t frame);

bool BuffTimeGe(id_t buff_id, frame_t frame);

bool BuffTimeGt(id_t buff_id, frame_t frame);

bool TBuffTimeLt(id_t buff_id, frame_t frame);

bool TBuffTimeLe(id_t buff_id, frame_t frame);

bool TBuffTimeEq(id_t buff_id, frame_t frame);

bool TBuffTimeNe(id_t buff_id, frame_t frame);

bool TBuffTimeGe(id_t buff_id, frame_t frame);

bool TBuffTimeGt(id_t buff_id, frame_t frame);

bool SkillCooldownLt(id_t skill_id, frame_t frame);

bool SkillCooldownLe(id_t skill_id, frame_t frame);

bool SkillCooldownEq(id_t skill_id, frame_t frame);

bool SkillCooldownNe(id_t skill_id, frame_t frame);

bool SkillCooldownGe(id_t skill_id, frame_t frame);

bool SkillCooldownGt(id_t skill_id, frame_t frame);

bool SkillEnergyLt(id_t skill_id, int energy);

bool SkillEnergyLe(id_t skill_id, int energy);

bool SkillEnergyEq(id_t skill_id, int energy);

bool SkillEnergyNe(id_t skill_id, int energy);

bool SkillEnergyGe(id_t skill_id, int energy);

bool SkillEnergyGt(id_t skill_id, int energy);

bool TLifeLt(id_t target_id, double ratio);

bool TLifeLe(id_t target_id, double ratio);

bool TLifeEq(id_t target_id, double ratio);

bool TLifeNe(id_t target_id, double ratio);

bool TLifeGe(id_t target_id, double ratio);

bool TLifeGt(id_t target_id, double ratio);

bool DistanceLt(id_t target_id, double distance);

bool DistanceLe(id_t target_id, double distance);

bool DistanceEq(id_t target_id, double distance);

bool DistanceNe(id_t target_id, double distance);

bool DistanceGe(id_t target_id, double distance);

bool DistanceGt(id_t target_id, double distance);

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