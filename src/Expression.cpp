/**
 * Project: JX3DPS
 * File: Expression.cpp
 * Description:
 * Created Date: 2023-07-23 13:16:27
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-18 11:22:09
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Expression.h"

#include "Global/Id.h"

#include "Buff.h"
#include "Player.h"
#include "Skill.h"
#include "Target.hpp"

JX3DPS::Id_t JX3DPS::Expression::SetStart(Player *player, Targets *targets)
{
    return EXPRESSION_EVENT_START;
}

JX3DPS::Id_t JX3DPS::Expression::SetEnd(Player *player, Targets *targets)
{
    return EXPRESSION_EVENT_END;
}

JX3DPS::Id_t JX3DPS::Expression::AddTarget(Player *player, Targets *targets, Id_t targetId, int level, Value_t shield, double distance, Frame_t lifeTime)
{
    Target *target = new Target();
    target->SetId(targetId);
    target->SetLevel(level);
    target->SetPhysicsShield(shield);
    target->SetMagicShield(shield);
    target->SetDistance(distance);
    target->SetLifeTime(lifeTime);
    targets->insert(std::pair<Id_t, Target *>(targetId, target));
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::ChangeTarget(Player *player, Targets *targets, Id_t targetId)
{
    player->SetTargetId(targetId);
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::SetTargetLife(Player *player, Targets *targets, Id_t targetId, PctFloat_t life)
{
    targets->at(targetId)->SetLifePercent(life);
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::SetTargetMana(Player *player, Targets *targets, Id_t targetId, PctFloat_t mana)
{
    targets->at(targetId)->SetManaPercent(mana);
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::SetTargetDistance(Player *player, Targets *targets, Id_t targetId, double distance)
{
    targets->at(targetId)->SetDistance(distance);
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::SetTargetDead(Player *player, Targets *targets, Id_t targetId)
{
    Target *p = (*targets)[targetId];
    delete p;
    targets->erase(targetId);
    for (auto &buff : player->buffs) {
        buff.second->Remove(targetId);
    }
    if (player->GetTargetId() == targetId) {
        player->SetTargetId(TARGET_PLACE_HOLDERS_1);
    }
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::SetPlayerLife(Player *player, Targets *targets, Id_t targetId, PctFloat_t lifePercent)
{
    player->SetLifePercent(lifePercent);
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::SetPlayerMana(Player *player, Targets *targets, Id_t targetId, PctFloat_t manaPercent)
{
    player->SetManaPercent(manaPercent);
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::SetPlayerStop(Player *player, Targets *targets, bool stop)
{
    player->SetStop(stop);
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::AddBuff(Player *player, Targets *targets, Id_t id, Id_t buffId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    player->buffs[buffId]->Add(id, stackNum, durationMin, durationMax);
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

JX3DPS::Id_t JX3DPS::Expression::ClearBuff(Player *player, Targets *targets, Id_t id, Id_t buffId, int stackNum)
{
    player->buffs[buffId]->Clear(id, stackNum);
    return EXPRESSION_EVENT_PLACE_HOLDERS_DEFAULT;
}

bool JX3DPS::Expression::IsTargetSwitchable(Player *player, Targets *targets, Id_t id)
{
    if (targets->find(id) == targets->end() || player->GetTargetId() == id) {
        return false;
    }
    return true;
}

bool JX3DPS::Expression::IsReady(Player *player, Targets *targets, Id_t id, bool fcast)
{
    return player->skills[id]->IsReady(fcast);
}

bool JX3DPS::Expression::StopReCastSkill(Player *player, Targets *targets)
{
    return player->StopReCastSkill();
}

bool JX3DPS::Expression::IsScast(Player *player, Targets *targets, bool scast)
{
    return scast;
}

bool JX3DPS::Expression::IsTargetWithinRange(Player *player, Targets *targets, Id_t skillId)
{
    float range = player->skills[skillId]->GetRange();
    return targets->at(player->GetTargetId())->GetDistance() <= range;
}

bool JX3DPS::Expression::IsNotReCast(Player *player, Targets *targets)
{
    return !player->IsReCast() && !player->IsCast();
}

bool JX3DPS::Expression::IsNotCast(Player *player, Targets *targets)
{
    return !player->IsCast();
}

bool JX3DPS::Expression::StopReCast(Player *player, Targets *targets)
{
    if (player->IsReCast()) {
        player->SetReCast(false);
        player->skills[player->GetReCastSkill()]->Stop();
    }
    return true;
}

bool JX3DPS::Expression::IsCooldown(Player *player, Targets *targets, Id_t skillId)
{
    return player->skills[skillId]->GetCooldownCurrent() == 0;
}

bool JX3DPS::Expression::BuffExist(Player *player, Targets *targets, Id_t id)
{
    return player->buffs.at(id)->GetDurationCurrent() != 0;
}

bool JX3DPS::Expression::NoBuffExist(Player *player, Targets *targets, Id_t id)
{
    return player->buffs.at(id)->GetDurationCurrent() == 0;
}

bool JX3DPS::Expression::TBuffExist(Player *player, Targets *targets, Id_t id)
{
    return player->buffs.at(id)->GetDurationCurrent(player->GetTargetId()) != 0;
}

bool JX3DPS::Expression::TNoBuffExist(Player *player, Targets *targets, Id_t id)
{
    return player->buffs.at(id)->GetDurationCurrent(player->GetTargetId()) == 0;
}

bool JX3DPS::Expression::LastCastSkill(Player *player, Targets *targets, Id_t id)
{
    return player->GetLastCastSkill() == id;
}

bool JX3DPS::Expression::NotLastCastSkill(Player *player, Targets *targets, Id_t id)
{
    return player->GetLastCastSkill() != id;
}

bool JX3DPS::Expression::BuffStackNumLt(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent() < stackNum;
}

bool JX3DPS::Expression::BuffStackNumLe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent() <= stackNum;
}

bool JX3DPS::Expression::BuffStackNumEq(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent() == stackNum;
}

bool JX3DPS::Expression::BuffStackNumNe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent() != stackNum;
}

bool JX3DPS::Expression::BuffStackNumGe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent() >= stackNum;
}

bool JX3DPS::Expression::BuffStackNumGt(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent() > stackNum;
}

bool JX3DPS::Expression::TBuffStackNumLt(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent(player->GetTargetId()) < stackNum;
}

bool JX3DPS::Expression::TBuffStackNumLe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent(player->GetTargetId()) <= stackNum;
}

bool JX3DPS::Expression::TBuffStackNumEq(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent(player->GetTargetId()) == stackNum;
}

bool JX3DPS::Expression::TBuffStackNumNe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent(player->GetTargetId()) != stackNum;
}

bool JX3DPS::Expression::TBuffStackNumGe(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent(player->GetTargetId()) >= stackNum;
}

bool JX3DPS::Expression::TBuffStackNumGt(Player *player, Targets *targets, Id_t id, int stackNum)
{
    return player->buffs.at(id)->GetStackNumCurrent(player->GetTargetId()) > stackNum;
}

bool JX3DPS::Expression::BuffTimeLt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent() < time;
}

bool JX3DPS::Expression::BuffTimeLe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent() <= time;
}

bool JX3DPS::Expression::BuffTimeEq(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent() == time;
}

bool JX3DPS::Expression::BuffTimeNe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent() != time;
}

bool JX3DPS::Expression::BuffTimeGe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent() >= time;
}

bool JX3DPS::Expression::BuffTimeGt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent() > time;
}

bool JX3DPS::Expression::TBuffTimeLt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent(player->GetTargetId()) < time;
}

bool JX3DPS::Expression::TBuffTimeLe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent(player->GetTargetId()) <= time;
}

bool JX3DPS::Expression::TBuffTimeEq(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent(player->GetTargetId()) == time;
}

bool JX3DPS::Expression::TBuffTimeNe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent(player->GetTargetId()) != time;
}

bool JX3DPS::Expression::TBuffTimeGe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent(player->GetTargetId()) >= time;
}

bool JX3DPS::Expression::TBuffTimeGt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->buffs.at(id)->GetDurationCurrent(player->GetTargetId()) > time;
}

bool JX3DPS::Expression::SkillCooldownLt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldownCurrentWithoutGlobal() < time;
}

bool JX3DPS::Expression::SkillCooldownLe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldownCurrentWithoutGlobal() <= time;
}

bool JX3DPS::Expression::SkillCooldownEq(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldownCurrentWithoutGlobal() == time;
}

bool JX3DPS::Expression::SkillCooldownNe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldownCurrentWithoutGlobal() != time;
}

bool JX3DPS::Expression::SkillCooldownGe(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldownCurrentWithoutGlobal() >= time;
}

bool JX3DPS::Expression::SkillCooldownGt(Player *player, Targets *targets, Id_t id, Frame_t time)
{
    return player->skills.at(id)->GetCooldownCurrentWithoutGlobal() > time;
}

bool JX3DPS::Expression::SkillNotinCd(Player *player, Targets *targets, Id_t id)
{
    if (player->skills.at(id)->GetEnergyCountCurrent() > 0 && player->globalCooldownCurrent == 0) {
        return true;
    }
    return player->skills.at(id)->GetCooldownCurrent() == 0;
}

bool JX3DPS::Expression::SkillEnergyLt(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCountCurrent() < energy;
}

bool JX3DPS::Expression::SkillEnergyLe(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCountCurrent() <= energy;
}

bool JX3DPS::Expression::SkillEnergyEq(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCountCurrent() == energy;
}

bool JX3DPS::Expression::SkillEnergyNe(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCountCurrent() != energy;
}

bool JX3DPS::Expression::SkillEnergyGe(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCountCurrent() >= energy;
}

bool JX3DPS::Expression::SkillEnergyGt(Player *player, Targets *targets, Id_t id, int energy)
{
    return player->skills.at(id)->GetEnergyCountCurrent() > energy;
}

bool JX3DPS::Expression::TLifeLt(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->GetTargetId()]->GetLifePercent() < ratio;
}

bool JX3DPS::Expression::TLifeLe(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->GetTargetId()]->GetLifePercent() <= ratio;
}

bool JX3DPS::Expression::TLifeEq(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->GetTargetId()]->GetLifePercent() == ratio;
}

bool JX3DPS::Expression::TLifeNe(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->GetTargetId()]->GetLifePercent() != ratio;
}

bool JX3DPS::Expression::TLifeGe(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->GetTargetId()]->GetLifePercent() >= ratio;
}

bool JX3DPS::Expression::TLifeGt(Player *player, Targets *targets, Id_t id, double ratio)
{
    return (*targets)[player->GetTargetId()]->GetLifePercent() > ratio;
}

bool JX3DPS::Expression::DistanceLt(Player *player, Targets *targets, Id_t id, double distance)
{
    return (*targets)[player->GetTargetId()]->GetDistance() < distance;
}

bool JX3DPS::Expression::DistanceLe(Player *player, Targets *targets, Id_t id, double distance)
{
    return (*targets)[player->GetTargetId()]->GetDistance() <= distance;
}

bool JX3DPS::Expression::DistanceEq(Player *player, Targets *targets, Id_t id, double distance)
{
    return (*targets)[player->GetTargetId()]->GetDistance() == distance;
}

bool JX3DPS::Expression::DistanceNe(Player *player, Targets *targets, Id_t id, double distance)
{
    return (*targets)[player->GetTargetId()]->GetDistance() != distance;
}

bool JX3DPS::Expression::DistanceGe(Player *player, Targets *targets, Id_t id, double distance)
{
    return (*targets)[player->GetTargetId()]->GetDistance() > distance;
}

bool JX3DPS::Expression::DistanceGt(Player *player, Targets *targets, Id_t id, double distance)
{
    return (*targets)[player->GetTargetId()]->GetDistance() >= distance;
}

bool JX3DPS::Expression::ManaLt(Player *player, Targets *targets, double ratio)
{
    return player->GetManaPercent() < ratio;
}

bool JX3DPS::Expression::ManaLe(Player *player, Targets *targets, double ratio)
{
    return player->GetManaPercent() <= ratio;
}

bool JX3DPS::Expression::ManaEq(Player *player, Targets *targets, double ratio)
{
    return player->GetManaPercent() == ratio;
}

bool JX3DPS::Expression::ManaNe(Player *player, Targets *targets, double ratio)
{
    return player->GetManaPercent() != ratio;
}

bool JX3DPS::Expression::ManaGe(Player *player, Targets *targets, double ratio)
{
    return player->GetManaPercent() >= ratio;
}

bool JX3DPS::Expression::ManaGt(Player *player, Targets *targets, double ratio)
{
    return player->GetManaPercent() > ratio;
}

bool JX3DPS::Expression::NearbyEnemyLt(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp < count;
}

bool JX3DPS::Expression::NearbyEnemyLe(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp <= count;
}

bool JX3DPS::Expression::NearbyEnemyEq(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp == count;
}

bool JX3DPS::Expression::NearbyEnemyNe(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp != count;
}

bool JX3DPS::Expression::NearbyEnemyGe(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp >= count;
}

bool JX3DPS::Expression::NearbyEnemyGt(Player *player, Targets *targets, double distance, int count)
{
    int temp = 0;
    for (auto &target : *targets) {
        if (target.second->GetDistance() < distance) {
            temp++;
        }
    }
    return temp > count;
}

bool JX3DPS::Expression::QidianLt(Player *player, Targets *targets, int count)
{
    return player->GetQidian() < count;
}

bool JX3DPS::Expression::QidianLe(Player *player, Targets *targets, int count)
{
    return player->GetQidian() <= count;
}

bool JX3DPS::Expression::QidianEq(Player *player, Targets *targets, int count)
{
    return player->GetQidian() == count;
}

bool JX3DPS::Expression::QidianNe(Player *player, Targets *targets, int count)
{
    return player->GetQidian() != count;
}

bool JX3DPS::Expression::QidianGe(Player *player, Targets *targets, int count)
{
    return player->GetQidian() >= count;
}

bool JX3DPS::Expression::QidianGt(Player *player, Targets *targets, int count)
{
    return player->GetQidian() > count;
}
