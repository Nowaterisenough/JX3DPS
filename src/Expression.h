/**
 * Project: JX3DPS
 * File: Expression.h
 * Description:
 * Created Date: 2023-07-22 13:41:19
 * Author: 难为水
 * -----
 * Last Modified: 2023-11-06 02:10:13
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_EXPRESSION_H__
#define __JX3DPS_EXPRESSION_H__

#include "Global/Defs.h"
#include "Global/Id.h"
#include "Global/Types.h"

namespace JX3DPS {

class Player;

using ExprIf         = std::function<bool(Player *player, Targets *targets)>;
using ExprIfs        = std::list<std::list<ExprIf>>;
using ExprSkill      = std::pair<ExprIfs, Id_t>;
using ExprSkills     = std::list<ExprSkill>;
using ExprSkillsHash = std::unordered_map<Id_t, ExprSkills>;

using Expr       = std::function<Id_t(Player *player, Targets *targets)>;
using ExprEvent  = std::pair<Frame_t, Expr>;
using ExprEvents = std::list<ExprEvent>;

namespace Expression {

Id_t SetStart(Player *player, Targets *targets);

Id_t SetEnd(Player *player, Targets *targets);

Id_t AddTarget(Player *player, Targets *targets, Id_t targetId, int level, Value_t shield, double distance, Frame_t lifeTime = JX3DPS_UNLIMITED_LIFE_TIME);

Id_t ChangeTarget(Player *player, Targets *targets, Id_t targetId);

Id_t SetTargetLife(Player *player, Targets *targets, Id_t targetId, PctFloat_t lifePercent);

Id_t SetTargetMana(Player *player, Targets *targets, Id_t targetId, PctFloat_t manaPercent);

Id_t SetTargetDistance(Player *player, Targets *targets, Id_t targetId, double distance);

Id_t SetTargetDead(Player *player, Targets *targets, Id_t targetId);

Id_t SetPlayerLife(Player *player, Targets *targets, Id_t targetId, PctFloat_t lifePercent);

Id_t SetPlayerMana(Player *player, Targets *targets, Id_t targetId, PctFloat_t manaPercent);

Id_t SetPlayerStop(Player *player, Targets *targets, bool stop);

Id_t AddBuff(Player  *player,
             Targets *targets,
             Id_t     id,
             Id_t     buffId,
             int      stackNum    = 1,
             Frame_t  durationMin = JX3DPS_DEFAULT_DURATION_FRAMES,
             Frame_t  durationMax = JX3DPS_DEFAULT_DURATION_FRAMES);

Id_t ClearBuff(Player *player, Targets *targets, Id_t id, Id_t buffId, int stackNum = 1);

bool IsTargetSwitchable(Player *player, Targets *targets, Id_t id);

bool IsReady(Player *player, Targets *targets, Id_t id, bool fcast);

bool StopReCastSkill(Player *player, Targets *targets);

bool IsScast(Player *player, Targets *targets, bool scast);

bool IsTargetWithinRange(Player *player, Targets *targets, Id_t skillId);

bool IsNotReCast(Player *player, Targets *targets);

bool IsNotCast(Player *player, Targets *targets);

bool StopReCast(Player *player, Targets *targets);

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

bool SkillNotinCd(Player *player, Targets *targets, Id_t id);

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

} // namespace Expression

} // namespace JX3DPS

#endif // __JX3DPS_EXPRESSION_H__