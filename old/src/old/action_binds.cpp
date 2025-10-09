#include "action_binds.h"

#include "sim.hpp"

using namespace JX3DPS::Action;

bool SkillNotinCd(jx3id_t skill_id)
{
    return tls_sim.player.GetSkillCooldown(skill_id) == 0;
}

bool BuffExist(jx3id_t buff_id)
{
    return tls_sim.player.HasBuff(buff_id);
}

bool NoBuffExist(jx3id_t buff_id)
{
    return !tls_sim.player.HasBuff(buff_id);
}

bool TBuffExist(jx3id_t buff_id)
{
    return tls_sim.target->HasBuff(buff_id);
}

bool TNoBuffExist(jx3id_t buff_id)
{
    return !tls_sim.target->HasBuff(buff_id);
}

bool LastCastSkill(jx3id_t skill_id)
{
    return tls_sim.player.GetLastCastSkill() == skill_id;
}

bool NotLastCastSkill(jx3id_t skill_id)
{
    return tls_sim.player.GetLastCastSkill() != skill_id;
}

bool BuffStackNumLt(jx3id_t buff_id, int stack_num)
{
    return tls_sim.player.GetBuffStackNum(buff_id) < stack_num;
}

bool BuffStackNumLe(jx3id_t buff_id, int stack_num)
{
    return tls_sim.player.GetBuffStackNum(buff_id) <= stack_num;
}

bool BuffStackNumEq(jx3id_t buff_id, int stack_num)
{
    return tls_sim.player.GetBuffStackNum(buff_id) == stack_num;
}

bool BuffStackNumNe(jx3id_t buff_id, int stack_num)
{
    return tls_sim.player.GetBuffStackNum(buff_id) != stack_num;
}

bool BuffStackNumGe(jx3id_t buff_id, int stack_num)
{
    return tls_sim.player.GetBuffStackNum(buff_id) >= stack_num;
}

bool BuffStackNumGt(jx3id_t buff_id, int stack_num)
{
    return tls_sim.player.GetBuffStackNum(buff_id) > stack_num;
}

bool TBuffStackNumLt(jx3id_t buff_id, int stack_num)
{
    return tls_sim.target->GetBuffStackNum(buff_id) < stack_num;
}

bool TBuffStackNumLe(jx3id_t buff_id, int stack_num)
{
    return tls_sim.target->GetBuffStackNum(buff_id) <= stack_num;
}

bool TBuffStackNumEq(jx3id_t buff_id, int stack_num)
{
    return tls_sim.target->GetBuffStackNum(buff_id) == stack_num;
}

bool TBuffStackNumNe(jx3id_t buff_id, int stack_num)
{
    return tls_sim.target->GetBuffStackNum(buff_id) != stack_num;
}

bool TBuffStackNumGe(jx3id_t buff_id, int stack_num)
{
    return tls_sim.target->GetBuffStackNum(buff_id) >= stack_num;
}

bool TBuffStackNumGt(jx3id_t buff_id, int stack_num)
{
    return tls_sim.target->GetBuffStackNum(buff_id) > stack_num;
}

bool BuffTimeLt(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.player.GetBuffTime(buff_id) < tick;
}

bool BuffTimeLe(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.player.GetBuffTime(buff_id) <= tick;
}

bool BuffTimeEq(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.player.GetBuffTime(buff_id) == tick;
}

bool BuffTimeNe(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.player.GetBuffTime(buff_id) != tick;
}

bool BuffTimeGe(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.player.GetBuffTime(buff_id) >= tick;
}

bool BuffTimeGt(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.player.GetBuffTime(buff_id) > tick;
}

bool TBuffTimeLt(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.target->GetBuffTime(buff_id) < tick;
}

bool TBuffTimeLe(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.target->GetBuffTime(buff_id) <= tick;
}

bool TBuffTimeEq(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.target->GetBuffTime(buff_id) == tick;
}

bool TBuffTimeNe(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.target->GetBuffTime(buff_id) != tick;
}

bool TBuffTimeGe(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.target->GetBuffTime(buff_id) >= tick;
}

bool TBuffTimeGt(jx3id_t buff_id, tick_t tick)
{
    return tls_sim.target->GetBuffTime(buff_id) > tick;
}

bool SkillCooldownLt(jx3id_t skill_id, tick_t tick)
{
    return tls_sim.player.GetSkillCooldown(skill_id) < tick;
}

bool SkillCooldownLe(jx3id_t skill_id, tick_t tick)
{
    return tls_sim.player.GetSkillCooldown(skill_id) <= tick;
}

bool SkillCooldownEq(jx3id_t skill_id, tick_t tick)
{
    return tls_sim.player.GetSkillCooldown(skill_id) == tick;
}

bool SkillCooldownNe(jx3id_t skill_id, tick_t tick)
{
    return tls_sim.player.GetSkillCooldown(skill_id) != tick;
}

bool SkillCooldownGe(jx3id_t skill_id, tick_t tick)
{
    return tls_sim.player.GetSkillCooldown(skill_id) >= tick;
}

bool SkillCooldownGt(jx3id_t skill_id, tick_t tick)
{
    return tls_sim.player.GetSkillCooldown(skill_id) > tick;
}

bool SkillEnergyLt(jx3id_t skill_id, int energy)
{
    return tls_sim.player.GetSkillEnergy(skill_id) < energy;
}

bool SkillEnergyLe(jx3id_t skill_id, int energy)
{
    return tls_sim.player.GetSkillEnergy(skill_id) <= energy;
}

bool SkillEnergyEq(jx3id_t skill_id, int energy)
{
    return tls_sim.player.GetSkillEnergy(skill_id) == energy;
}

bool SkillEnergyNe(jx3id_t skill_id, int energy)
{
    return tls_sim.player.GetSkillEnergy(skill_id) != energy;
}

bool SkillEnergyGe(jx3id_t skill_id, int energy)
{
    return tls_sim.player.GetSkillEnergy(skill_id) >= energy;
}

bool SkillEnergyGt(jx3id_t skill_id, int energy)
{
    return tls_sim.player.GetSkillEnergy(skill_id) > energy;
}

bool TLifeLt(jx3id_t target_id, double ratio)
{
    return tls_sim.targets[target_id].GetLifePercent() < ratio;
}

bool TLifeLe(jx3id_t target_id, double ratio)
{
    return tls_sim.targets[target_id].GetLifePercent() <= ratio;
}

bool TLifeEq(jx3id_t target_id, double ratio)
{
    return tls_sim.targets[target_id].GetLifePercent() == ratio;
}

bool TLifeNe(jx3id_t target_id, double ratio)
{
    return tls_sim.targets[target_id].GetLifePercent() != ratio;
}

bool TLifeGe(jx3id_t target_id, double ratio)
{
    return tls_sim.targets[target_id].GetLifePercent() >= ratio;
}

bool TLifeGt(jx3id_t target_id, double ratio)
{
    return tls_sim.targets[target_id].GetLifePercent() > ratio;
}

bool TLifeLt(double ratio)
{
    return tls_sim.target->GetLifePercent() < ratio;
}

bool TLifeLe(double ratio)
{
    return tls_sim.target->GetLifePercent() <= ratio;
}

bool TLifeEq(double ratio)
{
    return tls_sim.target->GetLifePercent() == ratio;
}

bool TLifeNe(double ratio)
{
    return tls_sim.target->GetLifePercent() != ratio;
}

bool TLifeGe(double ratio)
{
    return tls_sim.target->GetLifePercent() >= ratio;
}

bool TLifeGt(double ratio)
{
    return tls_sim.target->GetLifePercent() > ratio;
}

bool DistanceLt(jx3id_t target_id, double distance)
{
    return tls_sim.targets[target_id].GetDistance() < distance;
}

bool DistanceLe(jx3id_t target_id, double distance)
{
    return tls_sim.targets[target_id].GetDistance() <= distance;
}

bool DistanceEq(jx3id_t target_id, double distance)
{
    return tls_sim.targets[target_id].GetDistance() == distance;
}

bool DistanceNe(jx3id_t target_id, double distance)
{
    return tls_sim.targets[target_id].GetDistance() != distance;
}

bool DistanceGe(jx3id_t target_id, double distance)
{
    return tls_sim.targets[target_id].GetDistance() >= distance;
}

bool DistanceGt(jx3id_t target_id, double distance)
{
    return tls_sim.targets[target_id].GetDistance() > distance;
}

bool DistanceLt(double distance)
{
    return tls_sim.target->GetDistance() < distance;
}

bool DistanceLe(double distance)
{
    return tls_sim.target->GetDistance() <= distance;
}

bool DistanceEq(double distance)
{
    return tls_sim.target->GetDistance() == distance;
}

bool DistanceNe(double distance)
{
    return tls_sim.target->GetDistance() != distance;
}

bool DistanceGe(double distance)
{
    return tls_sim.target->GetDistance() >= distance;
}

bool DistanceGt(double distance)
{
    return tls_sim.target->GetDistance() > distance;
}

bool ManaLt(double ratio)
{
    return tls_sim.player.GetManaPercent() < ratio;
}

bool ManaLe(double ratio)
{
    return tls_sim.player.GetManaPercent() <= ratio;
}

bool ManaEq(double ratio)
{
    return tls_sim.player.GetManaPercent() == ratio;
}

bool ManaNe(double ratio)
{
    return tls_sim.player.GetManaPercent() != ratio;
}

bool ManaGe(double ratio)
{
    return tls_sim.player.GetManaPercent() >= ratio;
}

bool ManaGt(double ratio)
{
    return tls_sim.player.GetManaPercent() > ratio;
}

bool NearbyEnemyLt(double distance, int count)
{
    return tls_sim.player.GetNearbyEnemyCount(distance) < count;
}

bool NearbyEnemyLe(double distance, int count)
{
    return tls_sim.player.GetNearbyEnemyCount(distance) <= count;
}

bool NearbyEnemyEq(double distance, int count)
{
    return tls_sim.player.GetNearbyEnemyCount(distance) == count;
}

bool NearbyEnemyNe(double distance, int count)
{
    return tls_sim.player.GetNearbyEnemyCount(distance) != count;
}

bool NearbyEnemyGe(double distance, int count)
{
    return tls_sim.player.GetNearbyEnemyCount(distance) >= count;
}

bool NearbyEnemyGt(double distance, int count)
{
    return tls_sim.player.GetNearbyEnemyCount(distance) > count;
}

bool QidianLt(int count)
{
    return tls_sim.player.GetQidianCount() < count;
}

bool QidianLe(int count)
{
    return tls_sim.player.GetQidianCount() <= count;
}

bool QidianEq(int count)
{
    return tls_sim.player.GetQidianCount() == count;
}

bool QidianNe(int count)
{
    return tls_sim.player.GetQidianCount() != count;
}

bool QidianGe(int count)
{
    return tls_sim.player.GetQidianCount() >= count;
}

bool QidianGt(int count)
{
    return tls_sim.player.GetQidianCount() > count;
}

