/**
 * Project: JX3DPS
 * File: Macro.h
 * Description:
 * Created Date: 2023-06-01 18:06:02
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-24 01:37:08
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef JX3DPS_MACRO_H_
#define JX3DPS_MACRO_H_

#include "Defs.h"
#include "Globals.h"

namespace JX3DPS {

Error_t ParseMacro(const std::string &line, ExprEvent &exprEvent, ExprSkill &exprSkill);

void CastSkill(Player *player, Targets *targets, ExprSkills &exprSkills);

namespace Macro {

bool CastSkill(Player *player, Targets *targets, ExprSkill &exprSkill);

bool NeverCast(Player *player, Targets *targets);

bool AddTarget(Player *player, Targets *targets, Id_t targetId, int level, Value_t shield, float distance);

bool SetTarget(Player *player, Targets *targets, Id_t targetId);

bool SetTargetLife(Player *player, Targets *targets, Id_t targetId, PctFloat_t life);

bool SetTargetMana(Player *player, Targets *targets, Id_t targetId, PctFloat_t mana);

bool SetTargetDistance(Player *player, Targets *targets, Id_t targetId, float distance);

bool SetTargetDead(Player *player, Targets *targets, Id_t targetId);

bool SetPlayerLife(Player *player, Targets *targets, Id_t targetId, PctFloat_t life);

bool SetPlayerMana(Player *player, Targets *targets, Id_t targetId, PctFloat_t mana);

bool SetPlayerStop(Player *player, Targets *targets, bool stop);

bool SetEnd(Player *player, Targets *targets);

bool AddBuff(Player *player, Targets *targets, Id_t id, Id_t buffId, int stackNum = 1, Frame_t duration = JX3DPS_DEFAULT_DURATION_FRAMES);

bool ClearBuff(Player *player, Targets *targets, Id_t id, Id_t buffId, int stackNum = 1);

bool IsTargetWithinRange(Player *player, Targets *targets, Id_t skillId);

bool IsNotReCast(Player *player, Targets *targets);

bool IsNotCast(Player *player, Targets *targets);

bool IsCooldown(Player *player, Targets *targets, Id_t skillId);

bool BuffExist(Player *player, Targets *targets, Id_t id);

bool NoBuffExist(Player *player, Targets *targets, Id_t id);

bool TBuffExist(Player *player, Targets *targets, Id_t id);

bool TNoBuffExist(Player *player, Targets *targets, Id_t id);

bool LastCastSkill(Player *player, Targets *targets, Id_t id);

bool NotLastCastSkill(Player *player, Targets *targets, Id_t id);

bool BuffStackNumLt(Player *player, Targets *targets, Id_t id, int stackNum);

bool BuffStackNumLe(Player *player, Targets *targets, Id_t id, int stackNum);

bool BuffStackNumEq(Player *player, Targets *targets, Id_t id, int stackNum);

bool BuffStackNumNe(Player *player, Targets *targets, Id_t id, int stackNum);

bool BuffStackNumGe(Player *player, Targets *targets, Id_t id, int stackNum);

bool BuffStackNumGt(Player *player, Targets *targets, Id_t id, int stackNum);

bool TBuffStackNumLt(Player *player, Targets *targets, Id_t id, int stackNum);

bool TBuffStackNumLe(Player *player, Targets *targets, Id_t id, int stackNum);

bool TBuffStackNumEq(Player *player, Targets *targets, Id_t id, int stackNum);

bool TBuffStackNumNe(Player *player, Targets *targets, Id_t id, int stackNum);

bool TBuffStackNumGe(Player *player, Targets *targets, Id_t id, int stackNum);

bool TBuffStackNumGt(Player *player, Targets *targets, Id_t id, int stackNum);

bool BuffTimeLt(Player *player, Targets *targets, Id_t id, Frame_t time);

bool BuffTimeLe(Player *player, Targets *targets, Id_t id, Frame_t time);

bool BuffTimeEq(Player *player, Targets *targets, Id_t id, Frame_t time);

bool BuffTimeNe(Player *player, Targets *targets, Id_t id, Frame_t time);

bool BuffTimeGe(Player *player, Targets *targets, Id_t id, Frame_t time);

bool BuffTimeGt(Player *player, Targets *targets, Id_t id, Frame_t time);

bool TBuffTimeLt(Player *player, Targets *targets, Id_t id, Frame_t time);

bool TBuffTimeLe(Player *player, Targets *targets, Id_t id, Frame_t time);

bool TBuffTimeEq(Player *player, Targets *targets, Id_t id, Frame_t time);

bool TBuffTimeNe(Player *player, Targets *targets, Id_t id, Frame_t time);

bool TBuffTimeGe(Player *player, Targets *targets, Id_t id, Frame_t time);

bool TBuffTimeGt(Player *player, Targets *targets, Id_t id, Frame_t time);

bool SkillCooldownLt(Player *player, Targets *targets, Id_t id, Frame_t time);

bool SkillCooldownLe(Player *player, Targets *targets, Id_t id, Frame_t time);

bool SkillCooldownEq(Player *player, Targets *targets, Id_t id, Frame_t time);

bool SkillCooldownNe(Player *player, Targets *targets, Id_t id, Frame_t time);

bool SkillCooldownGe(Player *player, Targets *targets, Id_t id, Frame_t time);

bool SkillCooldownGt(Player *player, Targets *targets, Id_t id, Frame_t time);

bool SkillEnergyLt(Player *player, Targets *targets, Id_t id, int energy);

bool SkillEnergyLe(Player *player, Targets *targets, Id_t id, int energy);

bool SkillEnergyEq(Player *player, Targets *targets, Id_t id, int energy);

bool SkillEnergyNe(Player *player, Targets *targets, Id_t id, int energy);

bool SkillEnergyGe(Player *player, Targets *targets, Id_t id, int energy);

bool SkillEnergyGt(Player *player, Targets *targets, Id_t id, int energy);

bool TLifeLt(Player *player, Targets *targets, Id_t id, double ratio);

bool TLifeLe(Player *player, Targets *targets, Id_t id, double ratio);

bool TLifeEq(Player *player, Targets *targets, Id_t id, double ratio);

bool TLifeNe(Player *player, Targets *targets, Id_t id, double ratio);

bool TLifeGe(Player *player, Targets *targets, Id_t id, double ratio);

bool TLifeGt(Player *player, Targets *targets, Id_t id, double ratio);

bool ManaLt(Player *player, Targets *targets, double ratio);

bool ManaLe(Player *player, Targets *targets, double ratio);

bool ManaEq(Player *player, Targets *targets, double ratio);

bool ManaNe(Player *player, Targets *targets, double ratio);

bool ManaGe(Player *player, Targets *targets, double ratio);

bool ManaGt(Player *player, Targets *targets, double ratio);

bool NearbyEnemyLt(Player *player, Targets *targets, double distance, int count);

bool NearbyEnemyLe(Player *player, Targets *targets, double distance, int count);

bool NearbyEnemyEq(Player *player, Targets *targets, double distance, int count);

bool NearbyEnemyNe(Player *player, Targets *targets, double distance, int count);

bool NearbyEnemyGe(Player *player, Targets *targets, double distance, int count);

bool NearbyEnemyGt(Player *player, Targets *targets, double distance, int count);

bool QidianLt(Player *player, Targets *targets, int count);

bool QidianLe(Player *player, Targets *targets, int count);

bool QidianEq(Player *player, Targets *targets, int count);

bool QidianNe(Player *player, Targets *targets, int count);

bool QidianGe(Player *player, Targets *targets, int count);

bool QidianGt(Player *player, Targets *targets, int count);

} // namespace Macro

} // namespace JX3DPS

#endif // JX3DPS_MACRO_H_
