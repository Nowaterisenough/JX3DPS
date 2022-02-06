/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-06 13:50:25
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 15:15:26
 * @FilePath    : \JX3DPS\modules\Core\Buff.cpp
 */
#include "Buff.h"

#include "Common/Utils.h"
#include "Core/Player.h"
#include "Core/Target.h"
#include "Core/Attr.h"

namespace JX3DPS {

Buff::Buff()
{

}

Buff::Buff(const Buff &buff) :
    m_player(nullptr),
    m_target(nullptr),
    m_attrSnap(nullptr),
    m_id(buff.m_id),
    m_name(buff.m_name),
    m_subNames(buff.m_subNames),
    m_levelNames(buff.m_levelNames),
    m_3rdCooldown(buff.m_3rdCooldown),
    m_cooldown(buff.m_cooldown),
    m_lastFrames(buff.m_lastFrames),
    m_intervalFrames(buff.m_intervalFrames),
    m_effectCount(buff.m_effectCount),
    m_stackCount(buff.m_stackCount),
    m_damageParams(buff.m_damageParams),
    m_isDamage(buff.m_isDamage)
{
    if (g_settings.simMode == SimMode::DEFALUT_TYPE && g_settings.classType % 2 == 0) {    // ClassType % 2 == 0 即外功
        m_statsDetail = &Buff::StatsDetail;
        if (g_settings.bonusSets[WEAPON_ATTACK]->set) {
            m_statsWeaponAttackBonus = &Buff::StatsPhysicsWeaponAttackBonus;
        }
        if (g_settings.bonusSets[ATTACK_BASE]->set) {
            m_statsAttackBaseBonus = &Buff::StatsPhysicsAttackBaseBonus;
        }
        if (g_settings.bonusSets[CRITICAL_STRIKE_POWER]->set) {
            m_statsCriticalStrikePowerBonus = &Buff::StatsPhysicsCriticalStrikePowerBonus;
        }
        if (g_settings.bonusSets[OVERCOME_BASE]->set) {
            m_statsOvercomeBaseBonus = &Buff::StatsPhysicsOvercomeBaseBonus;
        }
        if (g_settings.bonusSets[STRAIN]->set) {
            m_statsStrainBonus = &Buff::StatsPhysicsStrainBonus;
        }
        if (g_settings.bonusSets[SURPLUS]->set) {
            m_statsSurplusBonus = &Buff::StatsPhysicsSurplusBonus;
        }
    } else if (g_settings.simMode == SimMode::DEFALUT_TYPE && g_settings.classType % 2 == 1) {
        if (g_settings.bonusSets[WEAPON_ATTACK]->set) {
            m_statsWeaponAttackBonus = &Buff::StatsMagicWeaponAttackBonus;
        }
        if (g_settings.bonusSets[ATTACK_BASE]->set) {
            m_statsAttackBaseBonus = &Buff::StatsMagicAttackBaseBonus;
        }
        if (g_settings.bonusSets[CRITICAL_STRIKE_POWER]->set) {
            m_statsCriticalStrikePowerBonus = &Buff::StatsMagicCriticalStrikePowerBonus;
        }
        if (g_settings.bonusSets[OVERCOME_BASE]->set) {
            m_statsOvercomeBaseBonus = &Buff::StatsMagicOvercomeBaseBonus;
        }
        if (g_settings.bonusSets[STRAIN]->set) {
            m_statsStrainBonus = &Buff::StatsMagicStrainBonus;
        }
        if (g_settings.bonusSets[SURPLUS]->set) {
            m_statsSurplusBonus = &Buff::StatsMagicSurplusBonus;
        }
    }
    
}

Buff::~Buff()
{

}

Buff *Buff::Clone()
{
    return nullptr;
}

void Buff::SetPlayer(Player &player)
{
    m_player = &player;
}

void Buff::SetTarget(Target &target)
{
    m_target = &target;
}

Buff &Buff::operator=(const Buff &buff)
{
    m_player = nullptr;
    m_target = nullptr;
    m_attrSnap = nullptr;
    m_id = buff.m_id;
    m_name = buff.m_name;
    m_subNames = buff.m_subNames;
    m_levelNames = buff.m_levelNames;
    m_3rdCooldown = buff.m_3rdCooldown;
    m_cooldown = buff.m_cooldown;
    m_lastFrames = buff.m_lastFrames;
    m_intervalFrames = buff.m_intervalFrames;
    m_effectCount = buff.m_effectCount;
    m_stackCount = buff.m_stackCount;
    m_damageParams = buff.m_damageParams;
    m_isDamage = buff.m_isDamage;
    if (g_settings.simMode == SimMode::DEFALUT_TYPE && g_settings.classType % 2 == 0) {    // ClassType % 2 == 0 即外功
        m_statsDetail = &Buff::StatsDetail;
        if (g_settings.bonusSets[WEAPON_ATTACK]->set) {
            m_statsWeaponAttackBonus = &Buff::StatsPhysicsWeaponAttackBonus;
        }
        if (g_settings.bonusSets[ATTACK_BASE]->set) {
            m_statsAttackBaseBonus = &Buff::StatsPhysicsAttackBaseBonus;
        }
        if (g_settings.bonusSets[CRITICAL_STRIKE_POWER]->set) {
            m_statsCriticalStrikePowerBonus = &Buff::StatsPhysicsCriticalStrikePowerBonus;
        }
        if (g_settings.bonusSets[OVERCOME_BASE]->set) {
            m_statsOvercomeBaseBonus = &Buff::StatsPhysicsOvercomeBaseBonus;
        }
        if (g_settings.bonusSets[STRAIN]->set) {
            m_statsStrainBonus = &Buff::StatsPhysicsStrainBonus;
        }
        if (g_settings.bonusSets[SURPLUS]->set) {
            m_statsSurplusBonus = &Buff::StatsPhysicsSurplusBonus;
        }
    } else if (g_settings.simMode == SimMode::DEFALUT_TYPE && g_settings.classType % 2 == 1) {
        if (g_settings.bonusSets[WEAPON_ATTACK]->set) {
            m_statsWeaponAttackBonus = &Buff::StatsMagicWeaponAttackBonus;
        }
        if (g_settings.bonusSets[ATTACK_BASE]->set) {
            m_statsAttackBaseBonus = &Buff::StatsMagicAttackBaseBonus;
        }
        if (g_settings.bonusSets[CRITICAL_STRIKE_POWER]->set) {
            m_statsCriticalStrikePowerBonus = &Buff::StatsMagicCriticalStrikePowerBonus;
        }
        if (g_settings.bonusSets[OVERCOME_BASE]->set) {
            m_statsOvercomeBaseBonus = &Buff::StatsMagicOvercomeBaseBonus;
        }
        if (g_settings.bonusSets[STRAIN]->set) {
            m_statsStrainBonus = &Buff::StatsMagicStrainBonus;
        }
        if (g_settings.bonusSets[SURPLUS]->set) {
            m_statsSurplusBonus = &Buff::StatsMagicSurplusBonus;
        }
    }
    return *this;
}

bool Buff::IsExist() const
{
    return m_effectCount;
}

Frame_t Buff::GetNextTime() const
{
    Frame_t nextTime = GET_MIN_INT(m_3rdCooldown, m_cooldown);
    nextTime = GET_MIN_INT(nextTime, m_lastFrames);
    nextTime = GET_MIN_INT(nextTime, m_intervalFrames);
    return nextTime;
}

void Buff::UpdateTime(Frame_t frames)
{
    m_3rdCooldown -= frames;
    m_cooldown -= frames;
    m_lastFrames -= frames;
    m_intervalFrames -= frames;
}

bool Buff::IsDamage() const
{
    return m_isDamage;
}

Id_t Buff::GetId() const
{
    return m_id;
}

const std::string &Buff::GetName() const
{
    return m_name;
}

const std::string &Buff::GetSubName(int subIndex) const
{
    return m_subNames[subIndex];
}

const std::string &Buff::GetLevelName(int levelIndex) const
{
    return m_levelNames[levelIndex];
}

int Buff::GetSubCount() const
{
    return m_subNames.size();
}

int Buff::GetLevelCount() const
{
    return m_levelNames.size();
}

int Buff::GetStackCount() const
{
    return m_stackCount;
}

int Buff::GetEffectCount() const
{
    return m_effectCount;
}

Frame_t Buff::GetCooldown() const
{
    return m_cooldown;
}

Frame_t Buff::GetRestTime() const
{
    return m_lastFrames;
}

#if JX3DPS_MAJOR_VERSION >= 110
Table Buff::Roll(PCT_t playerCriticalStrikePercent) const
{
    return Random(0.0, 1.0) < playerCriticalStrikePercent ? Table::DOUBLE : Table::HIT;
}
#else
Table Buff::Roll(PCT_t playerHitPercent, PCT_t playerCriticalStrikePercent, PCT_t playerStrainPercent,
                    PCT_t targetMissPercent, PCT_t targetSightPercent, const RollType rollType = RollType::COMMON);
{
    PCT_t roll = Random(0.0, 1.0);
    if (rollType == RollType::COMMON) {
        PCT_t restPercent = 1.0;
        PCT_t missPercent = targetMissPercent - playerHitValuePercent;
        missPercent = BETWEEN(0.0, missPercent, restPercent);
        restPercent -= missPercent;
        PCT_t sightPercent = targetSightPercent - playerStrainPercent;
        sightPercent = BETWEEN(0.0, sightPercent, restPercent);
        restPercent -= sightPercent;
        PCT_t criticalStrikePercent = playerCriticalStrikePercent;
        criticalStrikePercent = BETWEEN(0.0, criticalStrikePercent, restPercent);
        restPercent -= criticalStrikePercent;
        if (roll < criticalStrikePercent) {
            return Table::DOUBLE;
        } else if (roll < criticalStrikePercent + restPercent) {
            return Table::HIT;
        } else if (roll < criticalStrikePercent + restPercent + sightPercent) {
            return Table::SIGHT;
        } else {
            return Table::MISS;
        }
    } else if (rollType == RollType::DOT) {
        PCT_t restPercent = 1.0;
        PCT_t criticalStrikePercent = playerCriticalStrikePercent;
        criticalStrikePercent = BETWEEN(0.0, criticalStrikePercent, restPercent);
        restPercent -= criticalStrikePercent;
        PCT_t sightPercent = targetSightPercent - playerStrainPercent;
        sightPercent = BETWEEN(0.0, sightPercent, restPercent);
        restPercent -= sightPercent;
        if (roll < criticalStrikePercent) {
            return Table::DOUBLE;
        } else if (roll < criticalStrikePercent + restPercent) {
            return Table::HIT;
        } else {
            return Table::SIGHT;
        }
    } else /* if (rollType == RollType::SUB) */ {
        PCT_t restPercent = 1.0;
        PCT_t criticalStrikePercent = playerCriticalStrikePercent;
        criticalStrikePercent = BETWEEN(0.0, criticalStrikePercent, restPercent);
        restPercent -= criticalStrikePercent;
        if (roll < criticalStrikePercent) {
            return Table::DOUBLE;
        } else {
            return Table::HIT;
        }
    }
}
#endif // JX3DPS_MAJOR_VERSION >= 110

Table Buff::GetPhysicsRollResult(const Attr &attr) const
{
    return Roll(attr.GetPhysicsCriticalStrikePercent());
}

Damage Buff::GetPhysicsDamage(const Attr &attr, const Target &target, Table table, const std::string &subName, int level) const
{
    PCT_t damagePercent = (1 + m_player->attr->GetPhysicsOvercomePercent()) *
                          (1 + target.GetDamageAddPercent()) *
                          (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams.at(subName)[level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParams.at(subName)[level].weaponDamageBinPercent >> 10) * m_player->attr->GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams.at(subName)[level].attackDamagePercent * attr.GetPhysicsAttack() * damagePercent);
    return Damage(GET_FINAL_DAMAGE(fixedDamage, table, attr.GetPhysicsCriticalStrikePowerPercent()),
                  GET_FINAL_DAMAGE(weaponDamage, table, attr.GetPhysicsCriticalStrikePowerPercent()),
                  GET_FINAL_DAMAGE(attackDamage, table, attr.GetPhysicsCriticalStrikePowerPercent()));
}

void Buff::UpdatePhysicsStats(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    Damage damage = GetPhysicsDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::DEFALUT_TYPE] += damage.Sum();
    (this->*m_statsDetail)(attr, target, table, subName, level, damage, stats);
}

Table Buff::GetMagicRollResult(const Attr &attr) const
{
    return Roll(attr.GetMagicCriticalStrikePercent());
}

Damage Buff::GetMagicDamage(const Attr &attr, const Target &target, Table table, const std::string &subName, int level) const
{
    PCT_t damagePercent = (1 + m_player->attr->GetMagicOvercomePercent()) *
                          (1 + target.GetDamageAddPercent()) *
                          (1 - target.GetMagicResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams.at(subName)[level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParams.at(subName)[level].weaponDamageBinPercent >> 10) * m_player->attr->GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams.at(subName)[level].attackDamagePercent * attr.GetMagicAttack() * damagePercent);
    return Damage(GET_FINAL_DAMAGE(fixedDamage, table, attr.GetMagicCriticalStrikePowerPercent()),
                  GET_FINAL_DAMAGE(weaponDamage, table, attr.GetMagicCriticalStrikePowerPercent()),
                  GET_FINAL_DAMAGE(attackDamage, table, attr.GetMagicCriticalStrikePowerPercent()));
}

void Buff::UpdateMagicStats(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    Damage damage = GetMagicDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::DEFALUT_TYPE] += damage.Sum();
    (this->*m_statsDetail)(attr, target, table, subName, level, damage, stats);
}

void Buff::StatsNoneDetail(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, const Damage &damage, Stats &stats) const
{

}

void Buff::StatsDetail(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, const Damage &damage, Stats &stats) const
{
    stats.damageStats[target.GetId()][m_id][subName][level][table].first++;
    stats.damageStats[target.GetId()][m_id][subName][level][table].second += damage;
    (this->*m_statsWeaponAttackBonus)(attr, target, table, subName, level, stats);
    (this->*m_statsAttackBaseBonus)(attr, target, table, subName, level, stats);
    (this->*m_statsCriticalStrikePowerBonus)(attr, target, table, subName, level, stats);
    (this->*m_statsOvercomeBaseBonus)(attr, target, table, subName, level, stats);
    (this->*m_statsStrainBonus)(attr, target, table, subName, level, stats);
    (this->*m_statsSurplusBonus)(attr, target, table, subName, level, stats);
}

void Buff::StatsNoneBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{

}

void Buff::StatsPhysicsWeaponAttackBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    m_attrSnap->AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    Damage damage = GetPhysicsDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.Sum();
    m_player->attr->AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    m_attrSnap->AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
}

void Buff::StatsMagicWeaponAttackBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    m_attrSnap->AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    Damage damage = GetMagicDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.Sum();
    m_player->attr->AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    m_attrSnap->AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
}

void Buff::StatsPhysicsAttackBaseBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddPhysicsAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    m_attrSnap->AddPhysicsAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    Damage damage = GetPhysicsDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::ATTACK_BASE] += damage.Sum();
    m_player->attr->AddPhysicsAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    m_attrSnap->AddPhysicsAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
}

void Buff::StatsMagicAttackBaseBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddMagicAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    m_attrSnap->AddMagicAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    Damage damage = GetMagicDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::ATTACK_BASE] += damage.Sum();
    m_player->attr->AddMagicAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    m_attrSnap->AddMagicAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
}

void Buff::StatsPhysicsCriticalStrikePowerBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddPhysicsCriticalStrikePower(g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    m_attrSnap->AddPhysicsCriticalStrikePower(g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    Damage damage = GetPhysicsDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.Sum();
    m_player->attr->AddPhysicsCriticalStrikePower(-g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    m_attrSnap->AddPhysicsCriticalStrikePower(-g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
}

void Buff::StatsMagicCriticalStrikePowerBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddMagicCriticalStrikePower(g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    m_attrSnap->AddMagicCriticalStrikePower(g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    Damage damage = GetMagicDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.Sum();
    m_player->attr->AddMagicCriticalStrikePower(-g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    m_attrSnap->AddMagicCriticalStrikePower(-g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
}

void Buff::StatsPhysicsOvercomeBaseBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddPhysicsOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    m_attrSnap->AddPhysicsOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    Damage damage = GetPhysicsDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::OVERCOME_BASE] += damage.Sum();
    m_player->attr->AddPhysicsOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    m_attrSnap->AddPhysicsOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
}

void Buff::StatsMagicOvercomeBaseBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddMagicOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    m_attrSnap->AddMagicOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    Damage damage = GetMagicDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::OVERCOME_BASE] += damage.Sum();
    m_player->attr->AddMagicOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    m_attrSnap->AddMagicOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
}

void Buff::StatsPhysicsStrainBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    m_attrSnap->AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    Damage damage = GetPhysicsDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::STRAIN] += damage.Sum();
    m_player->attr->AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
    m_attrSnap->AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
}

void Buff::StatsMagicStrainBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    m_attrSnap->AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    Damage damage = GetMagicDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::STRAIN] += damage.Sum();
    m_player->attr->AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
    m_attrSnap->AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
}

void Buff::StatsPhysicsStrainBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    m_attrSnap->AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    Damage damage = GetPhysicsDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::SURPLUS] += damage.Sum();
    m_player->attr->AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
    m_attrSnap->AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
}

void Buff::StatsPhysicsStrainBonus(const Attr &attr, const Target &target, Table table, const std::string &subName, int level, Stats &stats) const
{
    m_player->attr->AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    m_attrSnap->AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    Damage damage = GetMagicDamage(attr, target, table, subName, level);
    stats.bonusStats[AttrType::SURPLUS] += damage.Sum();
    m_player->attr->AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
    m_attrSnap->AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
}

}
