/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:17
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 13:16:12
 * @FilePath    : \JX3DPS\modules\Core\Skill.cpp
 */

#include "Skill.h"

#include "Core/Player.h"
#include "Core/Target.h"

namespace JX3DPS {

Frame_t Skill::s_publicCooldown = COMMON_PUBLIC_COOLDOWN;
int     Skill::s_maxPing        = 0;
int     Skill::s_minPing        = 0;

Skill::Skill(Player *player) : m_player(player), m_publicCooldown(&m_player->m_publicCooldown) {}

Skill::~Skill() {}

bool Skill::IsReady()
{
    return !m_publicCooldown;
}

Frame_t Skill::GetNextTime()
{
#ifndef OPTIMIZE_SET
    Frame_t nextTime = MAX_FRAMES_CONST;
    if (m_energyNum != 0) {
        nextTime = MIN_FRAMES(nextTime, m_prepareFrames);
    } else {
        nextTime = MIN_FRAMES(m_cooldown, m_prepareFrames);
    }
    nextTime = MIN_FRAMES(nextTime, m_intervalFrames);
    if (m_isPublicCooldown == PUBLIC_SKILL) {
        nextTime = MIN_FRAMES(nextTime, *m_publicCooldown);
    }
    return nextTime;
#elif OPTIMIZE_SET == 1
    Frame_t nextTime = POS_INT_TO_0_ELSE_MINUS_1(m_energyNum) & m_cooldown;
    nextTime         = MIN_FRAMES(nextTime, MAX_FRAMES(m_isPublicCooldown, *m_publicCooldown));
    nextTime         = MIN_FRAMES(nextTime, MINUS_1_TO_MAX_ELSE_NOTHING(m_prepareFrames));
    nextTime         = MIN_FRAMES(nextTime, MINUS_1_TO_MAX_ELSE_NOTHING(m_intervalFrames));
    return nextTime;
#elif OPTIMIZE_SET == 2
    Frame_t nextTime = POS_INT_TO_0_ELSE_MINUS_1(m_energyNum) & m_cooldown;
    nextTime         = MIN_FRAMES(nextTime, MAX_FRAMES(m_isPublicCooldown, *m_publicCooldown));
    nextTime         = MIN_FRAMES(nextTime, m_prepareFrames);
    nextTime         = MIN_FRAMES(nextTime, m_intervalFrames);
    return nextTime;
#endif // OPTIMIZE_SET
}

void Skill::UpdateTime(Frame_t frames)
{
#if OPTIMIZE_SET == 1
    m_cooldown -= MINUS_1_TO_0_ELSE_MINUS_1(m_cooldown) & frames;
    m_prepareFrames -= MINUS_1_TO_0_ELSE_MINUS_1(m_prepareFrames) & frames;
    m_intervalFrames -= MINUS_1_TO_0_ELSE_MINUS_1(m_intervalFrames) & frames;
#else
    m_cooldown -= frames;
    m_prepareFrames -= frames;
    m_intervalFrames -= frames;
#endif // OPTIMIZE_SET
}

Id_t Skill::GetId()
{
    return m_id;
}

const std::string &Skill::GetName()
{
    return m_name;
}

const std::string &Skill::GetSubName(int subNum)
{
    return m_subNameVec[subNum];
}

const std::string &Skill::GetLevelName(int levelNum)
{
    return m_levelNameVec[levelNum];
}

int Skill::GetSubNum()
{
    return m_subNameVec.size();
}

int Skill::GetLevelNum()
{
    return m_levelNameVec.size();
}

int Skill::GetEnergyNum()
{
    return m_energyNum;
}

Frame_t Skill::GetCooldown()
{
    return m_cooldown;
}

bool Skill::IsPublicSkill()
{
    return !m_isPublicCooldown;
}

void Skill::UpdateSkillCooldown(Frame_t frames)
{
    m_cooldown -= frames;
}

void Skill::UpdateSkillCriticalStrikePercent(Pct_t percent)
{
    m_skillCriticalStrikePercentAdd += percent;
}

void Skill::UpdateSkillCriticalStrikePowerPercent(Pct_t percent)
{
    m_skillCriticalStrikePowerPercentAdd += percent;
}

void Skill::UpdateSkillDamageBinPercent(BinPct_t binPercent)
{
    m_skillDamageBinPercentAdd += binPercent;
}

void Skill::UpdateSkillHitValuePercent(Pct_t percent)
{
    m_skillHitValuePercentAdd += percent;
}

void Skill::SetPing(int minPing, int maxPing)
{
    s_minPing = minPing;
    s_maxPing = maxPing;
}

TableRes Skill::Roll(Pct_t    playerHitValuePercent,
                     Pct_t    playerCriticalStrikePercent,
                     Pct_t    playerStrainPercent,
                     Pct_t    targetMissPercent,
                     Pct_t    targetSightPercent,
                     RollType rollType)
{
    Pct_t roll = RandBetween(0.0, 1.0);
    if (rollType == RollType::COMMON) {
        Pct_t restPercent = 1.0;
        Pct_t missPercent = targetMissPercent - playerHitValuePercent;
        missPercent       = BETWEEN(0.0, missPercent, restPercent);
        restPercent -= missPercent;
        Pct_t sightPercent = targetSightPercent - playerStrainPercent;
        sightPercent       = BETWEEN(0.0, sightPercent, restPercent);
        restPercent -= sightPercent;
        Pct_t criticalStrikePercent = playerCriticalStrikePercent;
        criticalStrikePercent       = BETWEEN(0.0, criticalStrikePercent, restPercent);
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
        Pct_t restPercent           = 1.0;
        Pct_t criticalStrikePercent = playerCriticalStrikePercent;
        criticalStrikePercent       = BETWEEN(0.0, criticalStrikePercent, restPercent);
        restPercent -= criticalStrikePercent;
        Pct_t sightPercent = targetSightPercent - playerStrainPercent;
        sightPercent       = BETWEEN(0.0, sightPercent, restPercent);
        restPercent -= sightPercent;
        if (roll < criticalStrikePercent) {
            return TableRes::DOUBLE;
        } else if (roll < criticalStrikePercent + restPercent) {
            return TableRes::NORMAL;
        } else {
            return TableRes::SIGHT;
        }
    } else /* if (rollType == RollType::SUB) */ {
        Pct_t restPercent           = 1.0;
        Pct_t criticalStrikePercent = playerCriticalStrikePercent;
        criticalStrikePercent       = BETWEEN(0.0, criticalStrikePercent, restPercent);
        restPercent -= criticalStrikePercent;
        if (roll < criticalStrikePercent) {
            return TableRes::DOUBLE;
        } else {
            return TableRes::NORMAL;
        }
    }
}

TableRes Skill::GetRollResult(Target &target)
{
    TableRes tableRes =
        Roll(m_player->Attr().GetPhysicsHitValuePercent() + m_skillHitValuePercentAdd,
             m_player->Attr().GetPhysicsCriticalStrikePercent() + m_skillCriticalStrikePercentAdd,
             m_player->Attr().GetStrainPercent(),
             target.GetMissPercent(),
             target.GetSightPercent());
    return tableRes;
}

DamageStats Skill::GetPhysicsDamage(Target &target, TableRes tableRes, std::string &subName, int level)
{
    Pct_t damagePercent =
        ((MAX_BIN_PCT_CONST + m_skillDamageBinPercentAdd) / static_cast<double>(MAX_BIN_PCT_CONST)) *
        (1 + m_player->Attr().GetPhysicsOvercomePercent()) * (1 + target.GetDamagePercentAdd()) *
        (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParam[subName][level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParam[subName][level].weaponDamageBinPercent >> 10) *
                                        m_player->Attr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParam[subName][level].attackDamagePercent *
                                        m_player->Attr().GetPhysicsAttack() * damagePercent);
    int tableResNum  = tableRes;
    return DamageStats(FINAL_DAMAGE(fixedDamage,
                                    tableResNum,
                                    (m_player->Attr().GetPhysicsCriticalStrikePowerPercent() +
                                     m_skillCriticalStrikePowerPercentAdd)),
                       FINAL_DAMAGE(weaponDamage,
                                    tableResNum,
                                    (m_player->Attr().GetPhysicsCriticalStrikePowerPercent() +
                                     m_skillCriticalStrikePowerPercentAdd)),
                       FINAL_DAMAGE(attackDamage,
                                    tableResNum,
                                    (m_player->Attr().GetPhysicsCriticalStrikePowerPercent() +
                                     m_skillCriticalStrikePowerPercentAdd)));
}

void Skill::UpdatePhysicsStats(Target      &target,
                               Stats       &stats,
                               SIM_MODE     mode,
                               TableRes     tableRes,
                               std::string &subName,
                               int          level)
{
    DamageStats damageStats;
    switch (static_cast<int>(mode)) {
    case SIM_MODE::DEFAULT:
        damageStats = GetPhysicsDamage(target, tableRes, subName, level);
        stats.effectStats[m_id][target.GetId()][subName][level][tableRes].first++;
        stats.effectStats[m_id][target.GetId()][subName][level][tableRes].second += damageStats;
        stats.attrBonusMap[AttrType::DEFAULT].damage += damageStats.SumDamage();
        if (stats.attrBonusMap[AttrType::WEAPON_ATTACK].set) {
            m_player->Attr().AddWeaponAttack(stats.attrBonusMap[AttrType::WEAPON_ATTACK].value);
            damageStats = GetPhysicsDamage(target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::WEAPON_ATTACK].damage += damageStats.SumDamage();
            m_player->Attr().AddWeaponAttack(-stats.attrBonusMap[AttrType::WEAPON_ATTACK].value);
        }
        if (stats.attrBonusMap[AttrType::ATTACK_BASE].set) {
            m_player->Attr().AddPhysicsAttackBase(stats.attrBonusMap[AttrType::ATTACK_BASE].value);
            damageStats = GetPhysicsDamage(target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::ATTACK_BASE].damage += damageStats.SumDamage();
            m_player->Attr().AddPhysicsAttackBase(-stats.attrBonusMap[AttrType::ATTACK_BASE].value);
        }
        if (stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].set) {
            m_player->Attr().AddPhysicsCriticalStrikePower(
                stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].value);
            damageStats = GetPhysicsDamage(target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].damage += damageStats.SumDamage();
            m_player->Attr().AddPhysicsCriticalStrikePower(
                -stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].value);
        }
        if (stats.attrBonusMap[AttrType::OVERCOME_BASE].set) {
            m_player->Attr().AddPhysicsOvercomeBase(stats.attrBonusMap[AttrType::OVERCOME_BASE].value);
            damageStats = GetPhysicsDamage(target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::OVERCOME_BASE].damage += damageStats.SumDamage();
            m_player->Attr().AddPhysicsOvercomeBase(
                -stats.attrBonusMap[AttrType::OVERCOME_BASE].value);
        }
        break;
    case SIM_MODE::AGILITY:
        damageStats = GetPhysicsDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::AGILITY].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::HIT_VALUE:
        damageStats = GetPhysicsDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::HIT_VALUE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::CRITICAL_STRIKE:
        damageStats = GetPhysicsDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::CRITICAL_STRIKE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::HASTE:
        damageStats = GetPhysicsDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::HASTE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::STRAIN:
        damageStats = GetPhysicsDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::STRAIN].damage += damageStats.SumDamage();
        break;
    }
}

DamageStats Skill::GetMagicDamage(Target &target, TableRes tableRes, std::string &subName, int level)
{
    Pct_t damagePercent =
        ((MAX_BIN_PCT_CONST + m_skillDamageBinPercentAdd) / static_cast<double>(MAX_BIN_PCT_CONST)) *
        (1 + m_player->Attr().GetMagicOvercomePercent()) * (1 + target.GetDamagePercentAdd()) *
        (1 - target.GetMagicResistPercent());
    int fixedDamage = static_cast<int>(m_damageParam[subName][level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParam[subName][level].weaponDamageBinPercent >> 10) *
                                        m_player->Attr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParam[subName][level].attackDamagePercent *
                                        m_player->Attr().GetMagicAttack() * damagePercent);
    int tableResNum  = tableRes;
    return DamageStats(FINAL_DAMAGE(fixedDamage,
                                    tableResNum,
                                    (m_player->Attr().GetMagicCriticalStrikePowerPercent() +
                                     m_skillCriticalStrikePowerPercentAdd)),
                       FINAL_DAMAGE(weaponDamage,
                                    tableResNum,
                                    (m_player->Attr().GetMagicCriticalStrikePowerPercent() +
                                     m_skillCriticalStrikePowerPercentAdd)),
                       FINAL_DAMAGE(attackDamage,
                                    tableResNum,
                                    (m_player->Attr().GetMagicCriticalStrikePowerPercent() +
                                     m_skillCriticalStrikePowerPercentAdd)));
}

void Skill::UpdateMagicStats(Target      &target,
                             Stats       &stats,
                             SIM_MODE     mode,
                             TableRes     tableRes,
                             std::string &subName,
                             int          level)
{
    DamageStats damageStats;
    switch (static_cast<int>(mode)) {
    case SIM_MODE::DEFAULT:
        damageStats = GetMagicDamage(target, tableRes, subName, level);
        stats.effectStats[m_id][target.GetId()][subName][level][tableRes].first++;
        stats.effectStats[m_id][target.GetId()][subName][level][tableRes].second += damageStats;
        stats.attrBonusMap[AttrType::DEFAULT].damage += damageStats.SumDamage();
        if (stats.attrBonusMap[AttrType::WEAPON_ATTACK].set) {
            m_player->Attr().AddWeaponAttack(stats.attrBonusMap[AttrType::WEAPON_ATTACK].value);
            damageStats = GetMagicDamage(target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::WEAPON_ATTACK].damage += damageStats.SumDamage();
            m_player->Attr().AddWeaponAttack(-stats.attrBonusMap[AttrType::WEAPON_ATTACK].value);
        }
        if (stats.attrBonusMap[AttrType::ATTACK_BASE].set) {
            m_player->Attr().AddMagicAttackBase(stats.attrBonusMap[AttrType::ATTACK_BASE].value);
            damageStats = GetMagicDamage(target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::ATTACK_BASE].damage += damageStats.SumDamage();
            m_player->Attr().AddMagicAttackBase(-stats.attrBonusMap[AttrType::ATTACK_BASE].value);
        }
        if (stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].set) {
            m_player->Attr().AddMagicCriticalStrikePower(
                stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].value);
            damageStats = GetMagicDamage(target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].damage += damageStats.SumDamage();
            m_player->Attr().AddMagicCriticalStrikePower(
                -stats.attrBonusMap[AttrType::CRITICAL_STRIKE_POWER].value);
        }
        if (stats.attrBonusMap[AttrType::OVERCOME_BASE].set) {
            m_player->Attr().AddMagicOvercomeBase(stats.attrBonusMap[AttrType::OVERCOME_BASE].value);
            damageStats = GetMagicDamage(target, tableRes, subName, level);
            stats.attrBonusMap[AttrType::OVERCOME_BASE].damage += damageStats.SumDamage();
            m_player->Attr().AddMagicOvercomeBase(-stats.attrBonusMap[AttrType::OVERCOME_BASE].value);
        }
        break;
    case SIM_MODE::AGILITY:
        damageStats = GetMagicDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::AGILITY].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::HIT_VALUE:
        damageStats = GetMagicDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::HIT_VALUE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::CRITICAL_STRIKE:
        damageStats = GetMagicDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::CRITICAL_STRIKE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::HASTE:
        damageStats = GetMagicDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::HASTE].damage += damageStats.SumDamage();
        break;
    case SIM_MODE::STRAIN:
        damageStats = GetPhysicsDamage(target, tableRes, subName, level);
        stats.attrBonusMap[AttrType::STRAIN].damage += damageStats.SumDamage();
        break;
    }
}

void Skill::ResetPublicCooldown()
{
    int     rand      = RandBetween(m_minPing, m_maxPing);
    Frame_t delay     = rand / DELAY_CONST;
    *m_publicCooldown = s_publicCooldown * m_player->Attr().GetHastePercent() + delay;
}

} // namespace JX3DPS
