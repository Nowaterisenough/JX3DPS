/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 15:11:11
 * @FilePath    : \JX3DPS\modules\Core\Skill.cpp
 */
#include "Skill.h"

#include "Common/Utils.h"
#include "Core/Player.h"
#include "Core/Target.h"
#include "Core/Attr.h"

namespace JX3DPS {

int Skill::s_publicCooldown = 24;
int Skill::s_delayMin       = 0;
int Skill::s_delayMax       = 0;

Skill::Skill(const Skill &skill) :
    m_player(nullptr), m_id(skill.m_id), m_name(skill.m_name), m_subNames(skill.m_subNames),
    m_levelNames(skill.m_levelNames), publicCooldown(nullptr),
    m_isPublicCooldown(skill.m_isPublicCooldown), m_cooldown(skill.m_cooldown),
    m_prepareFrames(skill.m_prepareFrames), m_intervalFrames(skill.m_intervalFrames),
    m_effectCount(skill.m_effectCount), m_energyCount(skill.m_energyCount),
    m_skillCooldownAdd(skill.m_skillCooldownAdd),
    m_skillCriticalStrikePercentAdd(skill.m_skillCriticalStrikePercentAdd),
    m_skillCriticalStrikePowerPercentAdd(skill.m_skillCriticalStrikePowerPercentAdd),
    m_skillDamageBinPercentAdd(skill.m_skillDamageBinPercentAdd),
    m_skillHitValuePercentAdd(skill.m_skillHitValuePercentAdd), m_damageParams(skill.m_damageParams)
{
    if (g_settings.simMode == SimMode::DEFALUT_TYPE && g_settings.classType % 2 == 0)
    { // ClassType % 2 == 0 即外功
        m_statsDetail = &Skill::StatsDetail;
        if (g_settings.bonusSets[WEAPON_ATTACK]->set) {
            m_statsWeaponAttackBonus = &Skill::StatsPhysicsWeaponAttackBonus;
        }
        if (g_settings.bonusSets[ATTACK_BASE]->set) {
            m_statsAttackBaseBonus = &Skill::StatsPhysicsAttackBaseBonus;
        }
        if (g_settings.bonusSets[CRITICAL_STRIKE_POWER]->set) {
            m_statsCriticalStrikePowerBonus = &Skill::StatsPhysicsCriticalStrikePowerBonus;
        }
        if (g_settings.bonusSets[OVERCOME_BASE]->set) {
            m_statsOvercomeBaseBonus = &Skill::StatsPhysicsOvercomeBaseBonus;
        }
        if (g_settings.bonusSets[STRAIN]->set) {
            m_statsStrainBonus = &Skill::StatsPhysicsStrainBonus;
        }
        if (g_settings.bonusSets[SURPLUS]->set) {
            m_statsSurplusBonus = &Skill::StatsPhysicsSurplusBonus;
        }
    } else if (g_settings.simMode == SimMode::DEFALUT_TYPE && g_settings.classType % 2 == 1) {
        if (g_settings.bonusSets[WEAPON_ATTACK]->set) {
            m_statsWeaponAttackBonus = &Skill::StatsMagicWeaponAttackBonus;
        }
        if (g_settings.bonusSets[ATTACK_BASE]->set) {
            m_statsAttackBaseBonus = &Skill::StatsMagicAttackBaseBonus;
        }
        if (g_settings.bonusSets[CRITICAL_STRIKE_POWER]->set) {
            m_statsCriticalStrikePowerBonus = &Skill::StatsMagicCriticalStrikePowerBonus;
        }
        if (g_settings.bonusSets[OVERCOME_BASE]->set) {
            m_statsOvercomeBaseBonus = &Skill::StatsMagicOvercomeBaseBonus;
        }
        if (g_settings.bonusSets[STRAIN]->set) {
            m_statsStrainBonus = &Skill::StatsMagicStrainBonus;
        }
        if (g_settings.bonusSets[SURPLUS]->set) {
            m_statsSurplusBonus = &Skill::StatsMagicSurplusBonus;
        }
    }
}

Skill::~Skill() {}

Skill *Skill::Clone()
{
    return nullptr;
}

Skill &Skill::operator=(const Skill &skill)
{
    m_player                             = nullptr;
    m_id                                 = skill.m_id;
    m_name                               = skill.m_name;
    m_subNames                           = skill.m_subNames;
    m_levelNames                         = skill.m_levelNames;
    publicCooldown                       = nullptr;
    m_isPublicCooldown                   = skill.m_isPublicCooldown;
    m_cooldown                           = skill.m_cooldown;
    m_prepareFrames                      = skill.m_prepareFrames;
    m_intervalFrames                     = skill.m_intervalFrames;
    m_effectCount                        = skill.m_effectCount;
    m_energyCount                        = skill.m_energyCount;
    m_skillCooldownAdd                   = skill.m_skillCooldownAdd;
    m_skillCriticalStrikePercentAdd      = skill.m_skillCriticalStrikePercentAdd;
    m_skillCriticalStrikePowerPercentAdd = skill.m_skillCriticalStrikePowerPercentAdd;
    m_skillDamageBinPercentAdd           = skill.m_skillDamageBinPercentAdd;
    m_skillHitValuePercentAdd            = skill.m_skillHitValuePercentAdd;
    m_damageParams                       = skill.m_damageParams;
    if (g_settings.simMode == SimMode::DEFALUT_TYPE && g_settings.classType % 2 == 0)
    { // ClassType % 2 == 0 即外功
        m_statsDetail = &Skill::StatsDetail;
        if (g_settings.bonusSets[WEAPON_ATTACK]->set) {
            m_statsWeaponAttackBonus = &Skill::StatsPhysicsWeaponAttackBonus;
        }
        if (g_settings.bonusSets[ATTACK_BASE]->set) {
            m_statsAttackBaseBonus = &Skill::StatsPhysicsAttackBaseBonus;
        }
        if (g_settings.bonusSets[CRITICAL_STRIKE_POWER]->set) {
            m_statsCriticalStrikePowerBonus = &Skill::StatsPhysicsCriticalStrikePowerBonus;
        }
        if (g_settings.bonusSets[OVERCOME_BASE]->set) {
            m_statsOvercomeBaseBonus = &Skill::StatsPhysicsOvercomeBaseBonus;
        }
        if (g_settings.bonusSets[STRAIN]->set) {
            m_statsStrainBonus = &Skill::StatsPhysicsStrainBonus;
        }
        if (g_settings.bonusSets[SURPLUS]->set) {
            m_statsSurplusBonus = &Skill::StatsPhysicsSurplusBonus;
        }
    } else if (g_settings.simMode == SimMode::DEFALUT_TYPE && g_settings.classType % 2 == 1) {
        if (g_settings.bonusSets[WEAPON_ATTACK]->set) {
            m_statsWeaponAttackBonus = &Skill::StatsMagicWeaponAttackBonus;
        }
        if (g_settings.bonusSets[ATTACK_BASE]->set) {
            m_statsAttackBaseBonus = &Skill::StatsMagicAttackBaseBonus;
        }
        if (g_settings.bonusSets[CRITICAL_STRIKE_POWER]->set) {
            m_statsCriticalStrikePowerBonus = &Skill::StatsMagicCriticalStrikePowerBonus;
        }
        if (g_settings.bonusSets[OVERCOME_BASE]->set) {
            m_statsOvercomeBaseBonus = &Skill::StatsMagicOvercomeBaseBonus;
        }
        if (g_settings.bonusSets[STRAIN]->set) {
            m_statsStrainBonus = &Skill::StatsMagicStrainBonus;
        }
        if (g_settings.bonusSets[SURPLUS]->set) {
            m_statsSurplusBonus = &Skill::StatsMagicSurplusBonus;
        }
    }
    return *this;
}

void Skill::SetPlayer(Player &player)
{
    m_player       = &player;
    publicCooldown = &player.publicCooldown;
}

void Skill::Interrupt(Targets &targets, Stats &stats) {}

Frame_t Skill::GetNextTime() const
{
    Frame_t nextTime = m_cooldown;
    nextTime         = GET_MIN_INT(nextTime, m_prepareFrames);
    nextTime         = GET_MIN_INT(nextTime, m_intervalFrames);
    return nextTime;
}

Frame_t Skill::GetEnableTime() const
{
    Frame_t nextTime = IF_1ST_POS_TO_0_ELSE_2ND(m_energyCount, m_cooldown);
    nextTime         = GET_MIN_INT(nextTime, GET_MAX_INT(m_isPublicCooldown, *publicCooldown));
    return nextTime;
}

void Skill::UpdateTime(Frame_t frames)
{
    m_cooldown -= frames;
    m_prepareFrames -= frames;
    m_intervalFrames -= frames;
}

Id_t Skill::GetId() const
{
    return m_id;
}

const std::string &Skill::GetName() const
{
    return m_name;
}

const std::string &Skill::GetSubName(int subNum) const
{
    return m_subNames[subNum];
}

const std::string &Skill::GetLevelName(int levelNum) const
{
    return m_levelNames[levelNum];
}

int Skill::GetSubCount() const
{
    return m_subNames.size();
}

int Skill::GetLevelCount() const
{
    return m_levelNames.size();
}

int Skill::GetEnergyCount() const
{
    return m_energyCount;
}

Frame_t Skill::GetCooldown() const
{
    return m_cooldown;
}

bool Skill::IsPublicSkill() const
{
    return !m_isPublicCooldown;
}

void Skill::UpdateSkillCooldown(const Frame_t frames)
{
    m_cooldown -= frames;
}

void Skill::UpdateSkillCriticalStrikePercent(PCT_t percent)
{
    m_skillCriticalStrikePercentAdd += percent;
}

void Skill::UpdateSkillCriticalStrikePowerPercent(PCT_t percent)
{
    m_skillCriticalStrikePowerPercentAdd += percent;
}

void Skill::UpdateSkillDamageBinPercent(BinPCT_t binPercent)
{
    m_skillDamageBinPercentAdd += binPercent;
}

void Skill::SetDelay(int delayMin, int delayMax)
{
    s_delayMin = delayMin;
    s_delayMax = delayMax;
}

Table Skill::Roll(PCT_t playerCriticalStrikePercent) const
{
    return Random(0.0, 1.0) < playerCriticalStrikePercent ? Table::DOUBLE : Table::HIT;
}

Table Skill::GetPhysicsRollResult() const
{
    return Roll(m_player->attr->GetPhysicsCriticalStrikePercent() + m_skillCriticalStrikePercentAdd);
}

Damage
Skill::GetPhysicsDamage(const Target &target, Table table, const std::string &subName, int level) const
{
    PCT_t damagePercent =
        ((JX3DPS_BIN_PCT + m_skillDamageBinPercentAdd) / static_cast<double>(JX3DPS_BIN_PCT)) *
        (1 + m_player->attr->GetPhysicsOvercomePercent()) * (1 + target.GetDamageAddPercent()) *
        (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams.at(subName)[level].fixedDamage * damagePercent);
    int weaponDamage =
        static_cast<int>((m_damageParams.at(subName)[level].weaponDamageBinPercent >> 10) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams.at(subName)[level].attackDamagePercent *
                                        m_player->attr->GetPhysicsAttack() * damagePercent);
    return Damage(GET_FINAL_DAMAGE(fixedDamage,
                                   table,
                                   (m_player->attr->GetPhysicsCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)),
                  GET_FINAL_DAMAGE(weaponDamage,
                                   table,
                                   (m_player->attr->GetPhysicsCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)),
                  GET_FINAL_DAMAGE(attackDamage,
                                   table,
                                   (m_player->attr->GetPhysicsCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)));
}

void Skill::UpdatePhysicsStats(const Target      &target,
                               Table              table,
                               const std::string &subName,
                               int                level,
                               Stats             &stats) const
{
    Damage damage = GetPhysicsDamage(target, table, subName, level);
    stats.bonusStats[AttrType::DEFALUT_TYPE] += damage.Sum();
    (this->*m_statsDetail)(target, table, subName, level, damage, stats);
}

Table Skill::GetMagicRollResult() const
{
    return Roll(m_player->attr->GetMagicCriticalStrikePercent() + m_skillCriticalStrikePercentAdd);
}

Damage
Skill::GetMagicDamage(const Target &target, Table table, const std::string &subName, int level) const
{
    PCT_t damagePercent =
        ((JX3DPS_BIN_PCT + m_skillDamageBinPercentAdd) / static_cast<double>(JX3DPS_BIN_PCT)) *
        (1 + m_player->attr->GetMagicOvercomePercent()) * (1 + target.GetDamageAddPercent()) *
        (1 - target.GetMagicResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams.at(subName)[level].fixedDamage * damagePercent);
    int weaponDamage =
        static_cast<int>((m_damageParams.at(subName)[level].weaponDamageBinPercent >> 10) *
                         m_player->attr->GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams.at(subName)[level].attackDamagePercent *
                                        m_player->attr->GetMagicAttack() * damagePercent);
    return Damage(GET_FINAL_DAMAGE(fixedDamage,
                                   table,
                                   (m_player->attr->GetMagicCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)),
                  GET_FINAL_DAMAGE(weaponDamage,
                                   table,
                                   (m_player->attr->GetMagicCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)),
                  GET_FINAL_DAMAGE(attackDamage,
                                   table,
                                   (m_player->attr->GetMagicCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)));
}

void Skill::UpdateMagicStats(const Target      &target,
                             Table              table,
                             const std::string &subName,
                             int                level,
                             Stats             &stats) const
{
    Damage damage = GetMagicDamage(target, table, subName, level);
    stats.bonusStats[AttrType::DEFALUT_TYPE] += damage.Sum();
    (this->*m_statsDetail)(target, table, subName, level, damage, stats);
}

void Skill::StatsNoneDetail(const Target      &target,
                            Table              table,
                            const std::string &subName,
                            int                level,
                            const Damage      &damage,
                            Stats             &stats) const
{
}

void Skill::StatsDetail(const Target      &target,
                        Table              table,
                        const std::string &subName,
                        int                level,
                        const Damage      &damage,
                        Stats             &stats) const
{
    stats.damageStats[target.GetId()][m_id][subName][level][table].first++;
    stats.damageStats[target.GetId()][m_id][subName][level][table].second += damage;
    (this->*m_statsWeaponAttackBonus)(target, table, subName, level, stats);
    (this->*m_statsAttackBaseBonus)(target, table, subName, level, stats);
    (this->*m_statsCriticalStrikePowerBonus)(target, table, subName, level, stats);
    (this->*m_statsOvercomeBaseBonus)(target, table, subName, level, stats);
    (this->*m_statsStrainBonus)(target, table, subName, level, stats);
    (this->*m_statsSurplusBonus)(target, table, subName, level, stats);
}

void Skill::StatsNoneBonus(const Target      &target,
                           Table              table,
                           const std::string &subName,
                           int                level,
                           Stats             &stats) const
{
}

void Skill::StatsPhysicsWeaponAttackBonus(const Target      &target,
                                          Table              table,
                                          const std::string &subName,
                                          int                level,
                                          Stats             &stats) const
{
    m_player->attr->AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    Damage damage = GetPhysicsDamage(target, table, subName, level);
    stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.Sum();
    m_player->attr->AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
}

void Skill::StatsMagicWeaponAttackBonus(const Target      &target,
                                        Table              table,
                                        const std::string &subName,
                                        int                level,
                                        Stats             &stats) const
{
    m_player->attr->AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    Damage damage = GetMagicDamage(target, table, subName, level);
    stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.Sum();
    m_player->attr->AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
}

void Skill::StatsPhysicsAttackBaseBonus(const Target      &target,
                                        Table              table,
                                        const std::string &subName,
                                        int                level,
                                        Stats             &stats) const
{
    m_player->attr->AddPhysicsAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    Damage damage = GetPhysicsDamage(target, table, subName, level);
    stats.bonusStats[AttrType::ATTACK_BASE] += damage.Sum();
    m_player->attr->AddPhysicsAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
}

void Skill::StatsMagicAttackBaseBonus(const Target      &target,
                                      Table              table,
                                      const std::string &subName,
                                      int                level,
                                      Stats             &stats) const
{
    m_player->attr->AddMagicAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    Damage damage = GetMagicDamage(target, table, subName, level);
    stats.bonusStats[AttrType::ATTACK_BASE] += damage.Sum();
    m_player->attr->AddMagicAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
}

void Skill::StatsPhysicsCriticalStrikePowerBonus(const Target      &target,
                                                 Table              table,
                                                 const std::string &subName,
                                                 int                level,
                                                 Stats             &stats) const
{
    m_player->attr->AddPhysicsCriticalStrikePower(
        g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    Damage damage = GetPhysicsDamage(target, table, subName, level);
    stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.Sum();
    m_player->attr->AddPhysicsCriticalStrikePower(
        -g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
}

void Skill::StatsMagicCriticalStrikePowerBonus(const Target      &target,
                                               Table              table,
                                               const std::string &subName,
                                               int                level,
                                               Stats             &stats) const
{
    m_player->attr->AddMagicCriticalStrikePower(
        g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    Damage damage = GetMagicDamage(target, table, subName, level);
    stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.Sum();
    m_player->attr->AddMagicCriticalStrikePower(
        -g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
}

void Skill::StatsPhysicsOvercomeBaseBonus(const Target      &target,
                                          Table              table,
                                          const std::string &subName,
                                          int                level,
                                          Stats             &stats) const
{
    m_player->attr->AddPhysicsOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    Damage damage = GetPhysicsDamage(target, table, subName, level);
    stats.bonusStats[AttrType::OVERCOME_BASE] += damage.Sum();
    m_player->attr->AddPhysicsOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
}

void Skill::StatsMagicOvercomeBaseBonus(const Target      &target,
                                        Table              table,
                                        const std::string &subName,
                                        int                level,
                                        Stats             &stats) const
{
    m_player->attr->AddMagicOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    Damage damage = GetMagicDamage(target, table, subName, level);
    stats.bonusStats[AttrType::OVERCOME_BASE] += damage.Sum();
    m_player->attr->AddMagicOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
}

void Skill::StatsPhysicsStrainBonus(const Target      &target,
                                    Table              table,
                                    const std::string &subName,
                                    int                level,
                                    Stats             &stats) const
{
    m_player->attr->AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    Damage damage = GetPhysicsDamage(target, table, subName, level);
    stats.bonusStats[AttrType::STRAIN] += damage.Sum();
    m_player->attr->AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
}

void Skill::StatsMagicStrainBonus(const Target      &target,
                                  Table              table,
                                  const std::string &subName,
                                  int                level,
                                  Stats             &stats) const
{
    m_player->attr->AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    Damage damage = GetMagicDamage(target, table, subName, level);
    stats.bonusStats[AttrType::STRAIN] += damage.Sum();
    m_player->attr->AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
}

void Skill::StatsPhysicsSurplusBonus(const Target      &target,
                                     Table              table,
                                     const std::string &subName,
                                     int                level,
                                     Stats             &stats) const
{
    m_player->attr->AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    Damage damage = GetPhysicsDamage(target, table, subName, level);
    stats.bonusStats[AttrType::SURPLUS] += damage.Sum();
    m_player->attr->AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
}

void Skill::StatsMagicSurplusBonus(const Target      &target,
                                   Table              table,
                                   const std::string &subName,
                                   int                level,
                                   Stats             &stats) const
{
    m_player->attr->AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    Damage damage = GetMagicDamage(target, table, subName, level);
    stats.bonusStats[AttrType::SURPLUS] += damage.Sum();
    m_player->attr->AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
}

void Skill::ResetPublicCooldown()
{
    int     rand        = Random(s_delayMin, s_delayMax);
    Frame_t delayFrames = rand / JX3DPS_DELAY_CONST;
    *publicCooldown     = s_publicCooldown * m_player->attr->GetHastePercent() + delayFrames;
}

} // namespace JX3DPS
