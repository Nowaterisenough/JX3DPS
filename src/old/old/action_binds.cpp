#include "action_binds.h"

#include "worker.h"

using namespace JX3DPS;

using namespace Action;

bool SkillNotinCd(jx3id_t skill_id)
{
    return Worker::player.GetSkillCooldown(skill_id) == 0;
}

bool BuffExist(jx3id_t buff_id)
{
    return Worker::player.HasBuff(buff_id);
}

bool NoBuffExist(jx3id_t buff_id)
{
    return !Worker::player.HasBuff(buff_id);
}

bool TBuffExist(jx3id_t buff_id)
{
    return Worker::target->HasBuff(buff_id);
}

bool TNoBuffExist(jx3id_t buff_id)
{
    return !Worker::target->HasBuff(buff_id);
}

bool LastCastSkill(jx3id_t skill_id)
{
    return Worker::player.GetLastCastSkill() == skill_id;
}

bool NotLastCastSkill(jx3id_t skill_id)
{
    return Worker::player.GetLastCastSkill() != skill_id;
}

bool BuffStackNumLt(jx3id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) < stack_num;
}

bool BuffStackNumLe(jx3id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) <= stack_num;
}

bool BuffStackNumEq(jx3id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) == stack_num;
}

bool BuffStackNumNe(jx3id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) != stack_num;
}

bool BuffStackNumGe(jx3id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) >= stack_num;
}

bool BuffStackNumGt(jx3id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) > stack_num;
}

bool TBuffStackNumLt(jx3id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) < stack_num;
}

bool TBuffStackNumLe(jx3id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) <= stack_num;
}

bool TBuffStackNumEq(jx3id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) == stack_num;
}

bool TBuffStackNumNe(jx3id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) != stack_num;
}

bool TBuffStackNumGe(jx3id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) >= stack_num;
}

bool TBuffStackNumGt(jx3id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) > stack_num;
}

bool BuffTimeLt(jx3id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) < frame;
}

bool BuffTimeLe(jx3id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) <= frame;
}

bool BuffTimeEq(jx3id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) == frame;
}

bool BuffTimeNe(jx3id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) != frame;
}

bool BuffTimeGe(jx3id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) >= frame;
}

bool BuffTimeGt(jx3id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) > frame;
}

bool TBuffTimeLt(jx3id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) < frame;
}

bool TBuffTimeLe(jx3id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) <= frame;
}

bool TBuffTimeEq(jx3id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) == frame;
}

bool TBuffTimeNe(jx3id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) != frame;
}

bool TBuffTimeGe(jx3id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) >= frame;
}

bool TBuffTimeGt(jx3id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) > frame;
}

bool SkillCooldownLt(jx3id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) < frame;
}

bool SkillCooldownLe(jx3id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) <= frame;
}

bool SkillCooldownEq(jx3id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) == frame;
}

bool SkillCooldownNe(jx3id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) != frame;
}

bool SkillCooldownGe(jx3id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) >= frame;
}

bool SkillCooldownGt(jx3id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) > frame;
}

bool SkillEnergyLt(jx3id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) < energy;
}

bool SkillEnergyLe(jx3id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) <= energy;
}

bool SkillEnergyEq(jx3id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) == energy;
}

bool SkillEnergyNe(jx3id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) != energy;
}

bool SkillEnergyGe(jx3id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) >= energy;
}

bool SkillEnergyGt(jx3id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) > energy;
}

bool TLifeLt(jx3id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() < ratio;
}

bool TLifeLe(jx3id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() <= ratio;
}

bool TLifeEq(jx3id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() == ratio;
}

bool TLifeNe(jx3id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() != ratio;
}

bool TLifeGe(jx3id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() >= ratio;
}

bool TLifeGt(jx3id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() > ratio;
}

bool TLifeLt(double ratio)
{
    return Worker::target->GetLifePercent() < ratio;
}

bool TLifeLe(double ratio)
{
    return Worker::target->GetLifePercent() <= ratio;
}

bool TLifeEq(double ratio)
{
    return Worker::target->GetLifePercent() == ratio;
}

bool TLifeNe(double ratio)
{
    return Worker::target->GetLifePercent() != ratio;
}

bool TLifeGe(double ratio)
{
    return Worker::target->GetLifePercent() >= ratio;
}

bool TLifeGt(double ratio)
{
    return Worker::target->GetLifePercent() > ratio;
}

bool DistanceLt(jx3id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() < distance;
}

bool DistanceLe(jx3id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() <= distance;
}

bool DistanceEq(jx3id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() == distance;
}

bool DistanceNe(jx3id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() != distance;
}

bool DistanceGe(jx3id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() >= distance;
}

bool DistanceGt(jx3id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() > distance;
}

bool DistanceLt(double distance)
{
    return Worker::target->GetDistance() < distance;
}

bool DistanceLe(double distance)
{
    return Worker::target->GetDistance() <= distance;
}

bool DistanceEq(double distance)
{
    return Worker::target->GetDistance() == distance;
}

bool DistanceNe(double distance)
{
    return Worker::target->GetDistance() != distance;
}

bool DistanceGe(double distance)
{
    return Worker::target->GetDistance() >= distance;
}

bool DistanceGt(double distance)
{
    return Worker::target->GetDistance() > distance;
}

bool ManaLt(double ratio)
{
    return Worker::player.GetManaPercent() < ratio;
}

bool ManaLe(double ratio)
{
    return Worker::player.GetManaPercent() <= ratio;
}

bool ManaEq(double ratio)
{
    return Worker::player.GetManaPercent() == ratio;
}

bool ManaNe(double ratio)
{
    return Worker::player.GetManaPercent() != ratio;
}

bool ManaGe(double ratio)
{
    return Worker::player.GetManaPercent() >= ratio;
}

bool ManaGt(double ratio)
{
    return Worker::player.GetManaPercent() > ratio;
}

bool NearbyEnemyLt(double distance, int count)
{
    return Worker::player.GetNearbyEnemyCount(distance) < count;
}

bool NearbyEnemyLe(double distance, int count)
{
    return Worker::player.GetNearbyEnemyCount(distance) <= count;
}

bool NearbyEnemyEq(double distance, int count)
{
    return Worker::player.GetNearbyEnemyCount(distance) == count;
}

bool NearbyEnemyNe(double distance, int count)
{
    return Worker::player.GetNearbyEnemyCount(distance) != count;
}

bool NearbyEnemyGe(double distance, int count)
{
    return Worker::player.GetNearbyEnemyCount(distance) >= count;
}

bool NearbyEnemyGt(double distance, int count)
{
    return Worker::player.GetNearbyEnemyCount(distance) > count;
}

bool QidianLt(int count)
{
    return Worker::player.GetQidianCount() < count;
}

bool QidianLe(int count)
{
    return Worker::player.GetQidianCount() <= count;
}

bool QidianEq(int count)
{
    return Worker::player.GetQidianCount() == count;
}

bool QidianNe(int count)
{
    return Worker::player.GetQidianCount() != count;
}

bool QidianGe(int count)
{
    return Worker::player.GetQidianCount() >= count;
}

bool QidianGt(int count)
{
    return Worker::player.GetQidianCount() > count;
}

