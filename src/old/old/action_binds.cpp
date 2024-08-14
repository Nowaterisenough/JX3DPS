#include "action_binds.h"

#include "worker.h"

using namespace JX3DPS;

using namespace Action;

bool SkillNotinCd(id_t skill_id)
{
    return Worker::player.GetSkillCooldown(skill_id) == 0;
}

bool BuffExist(id_t buff_id)
{
    return Worker::player.HasBuff(buff_id);
}

bool NoBuffExist(id_t buff_id)
{
    return !Worker::player.HasBuff(buff_id);
}

bool TBuffExist(id_t buff_id)
{
    return Worker::target->HasBuff(buff_id);
}

bool TNoBuffExist(id_t buff_id)
{
    return !Worker::target->HasBuff(buff_id);
}

bool LastCastSkill(id_t skill_id)
{
    return Worker::player.GetLastCastSkill() == skill_id;
}

bool NotLastCastSkill(id_t skill_id)
{
    return Worker::player.GetLastCastSkill() != skill_id;
}

bool BuffStackNumLt(id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) < stack_num;
}

bool BuffStackNumLe(id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) <= stack_num;
}

bool BuffStackNumEq(id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) == stack_num;
}

bool BuffStackNumNe(id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) != stack_num;
}

bool BuffStackNumGe(id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) >= stack_num;
}

bool BuffStackNumGt(id_t buff_id, int stack_num)
{
    return Worker::player.GetBuffStackNum(buff_id) > stack_num;
}

bool TBuffStackNumLt(id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) < stack_num;
}

bool TBuffStackNumLe(id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) <= stack_num;
}

bool TBuffStackNumEq(id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) == stack_num;
}

bool TBuffStackNumNe(id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) != stack_num;
}

bool TBuffStackNumGe(id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) >= stack_num;
}

bool TBuffStackNumGt(id_t buff_id, int stack_num)
{
    return Worker::target->GetBuffStackNum(buff_id) > stack_num;
}

bool BuffTimeLt(id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) < frame;
}

bool BuffTimeLe(id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) <= frame;
}

bool BuffTimeEq(id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) == frame;
}

bool BuffTimeNe(id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) != frame;
}

bool BuffTimeGe(id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) >= frame;
}

bool BuffTimeGt(id_t buff_id, frame_t frame)
{
    return Worker::player.GetBuffTime(buff_id) > frame;
}

bool TBuffTimeLt(id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) < frame;
}

bool TBuffTimeLe(id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) <= frame;
}

bool TBuffTimeEq(id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) == frame;
}

bool TBuffTimeNe(id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) != frame;
}

bool TBuffTimeGe(id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) >= frame;
}

bool TBuffTimeGt(id_t buff_id, frame_t frame)
{
    return Worker::target->GetBuffTime(buff_id) > frame;
}

bool SkillCooldownLt(id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) < frame;
}

bool SkillCooldownLe(id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) <= frame;
}

bool SkillCooldownEq(id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) == frame;
}

bool SkillCooldownNe(id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) != frame;
}

bool SkillCooldownGe(id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) >= frame;
}

bool SkillCooldownGt(id_t skill_id, frame_t frame)
{
    return Worker::player.GetSkillCooldown(skill_id) > frame;
}

bool SkillEnergyLt(id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) < energy;
}

bool SkillEnergyLe(id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) <= energy;
}

bool SkillEnergyEq(id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) == energy;
}

bool SkillEnergyNe(id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) != energy;
}

bool SkillEnergyGe(id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) >= energy;
}

bool SkillEnergyGt(id_t skill_id, int energy)
{
    return Worker::player.GetSkillEnergy(skill_id) > energy;
}

bool TLifeLt(id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() < ratio;
}

bool TLifeLe(id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() <= ratio;
}

bool TLifeEq(id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() == ratio;
}

bool TLifeNe(id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() != ratio;
}

bool TLifeGe(id_t target_id, double ratio)
{
    return Worker::targets[target_id].GetLifePercent() >= ratio;
}

bool TLifeGt(id_t target_id, double ratio)
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

bool DistanceLt(id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() < distance;
}

bool DistanceLe(id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() <= distance;
}

bool DistanceEq(id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() == distance;
}

bool DistanceNe(id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() != distance;
}

bool DistanceGe(id_t target_id, double distance)
{
    return Worker::targets[target_id].GetDistance() >= distance;
}

bool DistanceGt(id_t target_id, double distance)
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

