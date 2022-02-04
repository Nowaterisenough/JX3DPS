#include "Buff.h"

#include "Core/Player.h"
#include "Core/Target.h"

namespace JX3DPS {

Buff::Buff(Player *player, Target *target) :
    m_player(player), m_target(target)
{

}

Buff::~Buff()
{

}

bool Buff::IsExist()
{
    return m_effectNum;
}

Frame_t Buff::GetNextTime()
{
#if OPTIMIZE_SET == 1
    Frame_t nextTime = MIN_FRAMES(MINUS_1_TO_MAX_ELSE_NOTHING(m_3rdCooldown), MINUS_1_TO_MAX_ELSE_NOTHING(m_cooldown));
    nextTime = MIN_FRAMES(nextTime, MINUS_1_TO_MAX_ELSE_NOTHING(m_lastFrames));
    nextTime = MIN_FRAMES(nextTime, MINUS_1_TO_MAX_ELSE_NOTHING(m_intervalFrames));
    return nextTime;
#else
    Frame_t nextTime = MIN_FRAMES(m_3rdCooldown, m_cooldown);
    nextTime = MIN_FRAMES(nextTime, m_lastFrames);
    nextTime = MIN_FRAMES(nextTime, m_intervalFrames);
    return nextTime;
#endif // OPTIMIZE_SET
}

void JX3DPS::Buff::UpdateTime(Frame_t frames)
{
#if OPTIMIZE_SET == 1
    m_3rdCooldown -= MINUS_1_TO_0_ELSE_MINUS_1(m_3rdCooldown) & frames;
    m_cooldown -= MINUS_1_TO_0_ELSE_MINUS_1(m_cooldown) & frames;
    m_lastFrames -= MINUS_1_TO_0_ELSE_MINUS_1(m_lastFrames) & frames;
    m_intervalFrames -= MINUS_1_TO_0_ELSE_MINUS_1(m_intervalFrames) & frames;
#else
    m_3rdCooldown -= frames;
    m_cooldown -= frames;
    m_lastFrames -= frames;
    m_intervalFrames -= frames;
#endif // OPTIMIZE_SET
}

Id_t Buff::GetId()
{
    return m_id;
}

const std::string &Buff::GetName()
{
    return m_name;
}

const std::string &Buff::GetSubName(int subNum)
{
    return m_subNameVec[subNum];
}

const std::string &Buff::GetLevelName(int levelNum)
{
    return m_levelNameVec[levelNum];
}

int Buff::GetSubNum()
{
    return m_subNameVec.size();
}

int Buff::GetLevelNum()
{
    return m_levelNameVec.size();
}

int Buff::GetStackNum()
{
    return m_stackNum;
}

int Buff::GetEffectNum()
{
    return m_effectNum;
}

Frame_t Buff::GetCooldown()
{
    return m_cooldown;
}

Frame_t Buff::GetRestTime()
{
    return m_lastFrames;
}

TableRes Buff::Roll(Pct_t playerHitValuePercent,
                    Pct_t playerCriticalStrikePercent,
                    Pct_t playerStrainPercent,
                    Pct_t targetMissPercent,
                    Pct_t targetSightPercent,
                    RollType rollType)
{
    Pct_t roll = RandBetween(0.0, 1.0);
    if (rollType == RollType::COMMON) {
        Pct_t restPercent = 1.0;
        Pct_t missPercent = targetMissPercent - playerHitValuePercent;
        missPercent = BETWEEN(0.0, missPercent, restPercent);
        restPercent -= missPercent;
        Pct_t sightPercent = targetSightPercent - playerStrainPercent;
        sightPercent = BETWEEN(0.0, sightPercent, restPercent);
        restPercent -= sightPercent;
        Pct_t criticalStrikePercent = playerCriticalStrikePercent;
        criticalStrikePercent = BETWEEN(0.0, criticalStrikePercent, restPercent);
        restPercent -= criticalStrikePercent;
        if (roll < criticalStrikePercent) {
            return TableRes::DOUBLE;
        } else if (roll < criticalStrikePercent + restPercent) {
            return TableRes::NORMAL;
        } else if (roll < criticalStrikePercent + restPercent + sightPercent) {
            return TableRes::SIGHT;
        } else {
            return TableRes::MISS;
        }
    } else if (rollType == RollType::DOT) {
        Pct_t restPercent = 1.0;
        Pct_t criticalStrikePercent = playerCriticalStrikePercent;
        criticalStrikePercent = BETWEEN(0.0, criticalStrikePercent, restPercent);
        restPercent -= criticalStrikePercent;
        Pct_t sightPercent = targetSightPercent - playerStrainPercent;
        sightPercent = BETWEEN(0.0, sightPercent, restPercent);
        restPercent -= sightPercent;
        if (roll < criticalStrikePercent) {
            return TableRes::DOUBLE;
        } else if (roll < criticalStrikePercent + restPercent) {
            return TableRes::NORMAL;
        } else {
            return TableRes::SIGHT;
        }
    } else /* if (rollType == RollType::SUB) */ {
        Pct_t restPercent = 1.0;
        Pct_t criticalStrikePercent = playerCriticalStrikePercent;
        criticalStrikePercent = BETWEEN(0.0, criticalStrikePercent, restPercent);
        restPercent -= criticalStrikePercent;
        if (roll < criticalStrikePercent) {
            return TableRes::DOUBLE;
        } else {
            return TableRes::NORMAL;
        }
    }
}

TableRes Buff::GetRollResult(Attr &attr, Target &target)
{
    TableRes tableRes = Roll(attr.GetPhysicsHitValuePercent(),
                             attr.GetPhysicsCriticalStrikePercent(),
                             attr.GetStrainPercent(),
                             target.GetMissPercent(),
                             target.GetSightPercent());
    return tableRes;
}

DamageStats Buff::GetPhysicsDamage(Attr &attr, Target &target, TableRes tableRes, std::string &subName, int level)
{
    Pct_t damagePercent = (1 + m_player->Attr().GetPhysicsOvercomePercent()) *
                          (1 + target.GetDamagePercentAdd()) *
                          (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParam[subName][level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParam[subName][level].weaponDamageBinPercent >> 10) *
                                        m_player->Attr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParam[subName][level].attackDamagePercent *
                                        m_player->Attr().GetPhysicsAttack() * damagePercent);
    int tableResNum = tableRes;
    return DamageStats(FINAL_DAMAGE(fixedDamage,
                                    tableResNum,
                                    attr.GetPhysicsCriticalStrikePowerPercent()),
                       FINAL_DAMAGE(weaponDamage,
                                    tableResNum,
                                    attr.GetPhysicsCriticalStrikePowerPercent()),
                       FINAL_DAMAGE(attackDamage,
                                    tableResNum,
                                    attr.GetPhysicsCriticalStrikePowerPercent()));
}

void Buff::UpdatePhysicsStats(Attr &attr, Target &target, Stats &stats, SIM_MODE mode, TableRes tableRes, std::string &subName, int level)
{
    DamageStats damageStats;
    switch (static_cast<int>(mode)) {
    case SIM_MODE::DEFAULT :
        damageStats = GetPhysicsDamage(attr, target, tableRes, subName, level);
        stats.effectStats[m_id][target.GetId()][subName][level][tableRes].first++;
        stats.effectStats[m_id][target.GetId()][subName][level][tableRes].second += damageStats;
        stats.attrBonusMap[AttrType::DEFAULT].damage += damageStats.SumDamage();
        if (stats.attrBonusMap[AttrType::WEAPON_ATTACK].set) {
            m_player->Attr().AddWeaponAttack(stats.attrBonusMap[AttrType::WEAPON_ATTACK].value);
            damageStats = GetPhysicsDamage(attr,target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::WEAPON_ATTACK].damage += damageStats.SumDamage();
            m_player->Attr().AddWeaponAttack(-stats.attrBonusMap[AttrType::WEAPON_ATTACK].value);
        }
        if (stats.attrBonusMap[AttrType::ATTACK_BASE].set) {
            m_player->Attr().AddPhysicsAttackBase(stats.attrBonusMap[AttrType::ATTACK_BASE].value);
            damageStats = GetPhysicsDamage(attr,target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::ATTACK_BASE].damage += damageStats.SumDamage();
            m_player->Attr().AddPhysicsAttackBase(-stats.attrBonusMap[AttrType::ATTACK_BASE].value);
        }
        if (stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].set) {
            m_player->Attr().AddPhysicsCriticalStrikePower(stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].value);
            damageStats = GetPhysicsDamage(attr,target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].damage += damageStats.SumDamage();
            m_player->Attr().AddPhysicsCriticalStrikePower(-stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].value);
        }
        if (stats.attrBonusMap[AttrType::OVERCOME_BASE].set) {
            m_player->Attr().AddPhysicsOvercomeBase(stats.attrBonusMap[AttrType::OVERCOME_BASE].value);
            damageStats = GetPhysicsDamage(attr,target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::OVERCOME_BASE].damage += damageStats.SumDamage();
            m_player->Attr().AddPhysicsOvercomeBase(-stats.attrBonusMap[AttrType::OVERCOME_BASE].value);
        }
        break;
    case SIM_MODE::AGILITY :
        damageStats = GetPhysicsDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::AGILITY].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::HIT_VALUE :
        damageStats = GetPhysicsDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::HIT_VALUE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::CRITICAL_STRIKE :
        damageStats = GetPhysicsDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::CRITICAL_STRIKE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::HASTE :
        damageStats = GetPhysicsDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::HASTE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::STRAIN :
        damageStats = GetPhysicsDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::STRAIN].damage += damageStats.SumDamage();
        break;
    }
}

DamageStats Buff::GetMagicDamage(Attr &attr, Target &target, TableRes tableRes, std::string &subName, int level)
{
    Pct_t damagePercent = (1 + m_player->Attr().GetMagicOvercomePercent()) *
                          (1 + target.GetDamagePercentAdd()) *
                          (1 - target.GetMagicResistPercent());
    int fixedDamage = static_cast<int>(m_damageParam[subName][level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParam[subName][level].weaponDamageBinPercent >> 10) *
                                        m_player->Attr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParam[subName][level].attackDamagePercent *
                                        m_player->Attr().GetMagicAttack() * damagePercent);
    int tableResNum = tableRes;
    return DamageStats(FINAL_DAMAGE(fixedDamage,
                                    tableResNum,
                                    attr.GetMagicCriticalStrikePowerPercent()),
                       FINAL_DAMAGE(weaponDamage,
                                    tableResNum,
                                    attr.GetMagicCriticalStrikePowerPercent()),
                       FINAL_DAMAGE(attackDamage,
                                    tableResNum,
                                    attr.GetMagicCriticalStrikePowerPercent()));
}

void Buff::UpdateMagicStats(Attr &attr, Target &target, Stats &stats, SIM_MODE mode, TableRes tableRes, std::string &subName, int level)
{
    DamageStats damageStats;
    switch (static_cast<int>(mode)) {
    case SIM_MODE::DEFAULT :
        damageStats = GetMagicDamage(attr, target, tableRes, subName, level);
        stats.effectStats[m_id][target.GetId()][subName][level][tableRes].first++;
        stats.effectStats[m_id][target.GetId()][subName][level][tableRes].second += damageStats;
        stats.attrBonusMap[AttrType::DEFAULT].damage += damageStats.SumDamage();
        if (stats.attrBonusMap[AttrType::WEAPON_ATTACK].set) {
            m_player->Attr().AddWeaponAttack(stats.attrBonusMap[AttrType::WEAPON_ATTACK].value);
            damageStats = GetMagicDamage(attr,target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::WEAPON_ATTACK].damage += damageStats.SumDamage();
            m_player->Attr().AddWeaponAttack(-stats.attrBonusMap[AttrType::WEAPON_ATTACK].value);
        }
        if (stats.attrBonusMap[AttrType::ATTACK_BASE].set) {
            m_player->Attr().AddMagicAttackBase(stats.attrBonusMap[AttrType::ATTACK_BASE].value);
            damageStats = GetMagicDamage(attr,target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::ATTACK_BASE].damage += damageStats.SumDamage();
            m_player->Attr().AddMagicAttackBase(-stats.attrBonusMap[AttrType::ATTACK_BASE].value);
        }
        if (stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].set) {
            m_player->Attr().AddMagicCriticalStrikePower(stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].value);
            damageStats = GetMagicDamage(attr,target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].damage += damageStats.SumDamage();
            m_player->Attr().AddMagicCriticalStrikePower(-stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].value);
        }
        if (stats.attrBonusMap[AttrType::OVERCOME_BASE].set) {
            m_player->Attr().AddMagicOvercomeBase(stats.attrBonusMap[AttrType::OVERCOME_BASE].value);
            damageStats = GetMagicDamage(attr,target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::OVERCOME_BASE].damage += damageStats.SumDamage();
            m_player->Attr().AddMagicOvercomeBase(-stats.attrBonusMap[AttrType::OVERCOME_BASE].value);
        }
        break;
    case SIM_MODE::AGILITY :
        damageStats = GetMagicDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::AGILITY].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::HIT_VALUE :
        damageStats = GetMagicDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::HIT_VALUE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::CRITICAL_STRIKE :
        damageStats = GetMagicDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::CRITICAL_STRIKE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::HASTE :
        damageStats = GetMagicDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::HASTE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::STRAIN :
        damageStats = GetMagicDamage(attr,target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::STRAIN].damage += damageStats.SumDamage();
        break;
    }
}

}
