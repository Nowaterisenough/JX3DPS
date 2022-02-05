/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-05 14:56:22
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
    m_levelNames(skill.m_levelNames), m_publicCooldown(nullptr),
    m_isPublicCooldown(skill.m_isPublicCooldown), m_cooldown(skill.m_cooldown),
    m_prepareFrames(skill.m_prepareFrames), m_intervalFrames(skill.m_intervalFrames),
    m_effectCount(skill.m_effectCount), m_energyCount(skill.m_energyCount),
    m_skillCooldownAdd(skill.m_skillCooldownAdd),
    m_skillCriticalStrikePercentAdd(skill.m_skillCriticalStrikePercentAdd),
    m_skillCriticalStrikePowerPercentAdd(skill.m_skillCriticalStrikePowerPercentAdd),
    m_skillDamageBinPercentAdd(skill.m_skillDamageBinPercentAdd),
    m_skillHitValuePercentAdd(skill.m_skillHitValuePercentAdd), m_damageParams(skill.m_damageParams)
{
    if (g_settings.simMode == SIM_MODE::DEFALUT_TYPE && g_settings.classType % 2 == 0)
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
    } else if (g_settings.simMode == SIM_MODE::DEFALUT_TYPE && g_settings.classType % 2 == 1) {
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
    m_publicCooldown                     = nullptr;
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
    if (g_settings.simMode == SIM_MODE::DEFALUT_TYPE && g_settings.classType % 2 == 0)
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
    } else if (g_settings.simMode == SIM_MODE::DEFALUT_TYPE && g_settings.classType % 2 == 1) {
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
    m_player         = &player;
    m_publicCooldown = &player.publicCooldown;
}

void Skill::Interrupt(TargetsMap &targetsMap, Stats &stats) {}

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
    nextTime         = GET_MIN_INT(nextTime, GET_MAX_INT(m_isPublicCooldown, *m_publicCooldown));
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

const std::string &Skill::GetSubName(const int subNum) const
{
    return m_subNames[subNum];
}

const std::string &Skill::GetLevelName(const int levelNum) const
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

void Skill::UpdateSkillCriticalStrikePercent(const Pct_t percent)
{
    m_skillCriticalStrikePercentAdd += percent;
}

void Skill::UpdateSkillCriticalStrikePowerPercent(const Pct_t percent)
{
    m_skillCriticalStrikePowerPercentAdd += percent;
}

void Skill::UpdateSkillDamageBinPercent(const BinPct_t binPercent)
{
    m_skillDamageBinPercentAdd += binPercent;
}

void Skill::SetDelay(const int delayMin, const int delayMax)
{
    s_delayMin = delayMin;
    s_delayMax = delayMax;
}

TableRes Skill::Roll(const Pct_t playerCriticalStrikePercent) const
{
    return Random(0.0, 1.0) < playerCriticalStrikePercent ? TableRes::DOUBLE : TableRes::HIT;
}

TableRes Skill::GetPhysicsRollResult() const
{
    return Roll(m_player->Attr().GetPhysicsCriticalStrikePercent() +
                m_skillCriticalStrikePercentAdd);
}

Damage Skill::GetPhysicsDamage(const Target      &target,
                               const TableRes     tableRes,
                               const std::string &subName,
                               const int          level) const
{
    Pct_t damagePercent =
        ((CONST_BIN_PCT + m_skillDamageBinPercentAdd) / static_cast<double>(CONST_BIN_PCT)) *
        (1 + m_player->Attr().GetPhysicsOvercomePercent()) * (1 + target.GetDamageAddPercent()) *
        (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams.at(subName)[level].fixedDamage * damagePercent);
    int weaponDamage =
        static_cast<int>((m_damageParams.at(subName)[level].weaponDamageBinPercent >> 10) *
                         m_player->Attr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams.at(subName)[level].attackDamagePercent *
                                        m_player->Attr().GetPhysicsAttack() * damagePercent);
    return Damage(GET_FINAL_DAMAGE(fixedDamage,
                                   tableRes,
                                   (m_player->Attr().GetPhysicsCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)),
                  GET_FINAL_DAMAGE(weaponDamage,
                                   tableRes,
                                   (m_player->Attr().GetPhysicsCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)),
                  GET_FINAL_DAMAGE(attackDamage,
                                   tableRes,
                                   (m_player->Attr().GetPhysicsCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)));
}

void Skill::UpdatePhysicsStats(const Target      &target,
                               const TableRes     tableRes,
                               const std::string &subName,
                               const int          level,
                               Stats             &stats) const
{
    Damage damage = GetPhysicsDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::DEFALUT_TYPE] += damage.SumDamage();
    (this->*m_statsDetail)(target, tableRes, subName, level, damage, stats);
}

TableRes Skill::GetMagicRollResult() const
{
    return Roll(m_player->Attr().GetMagicCriticalStrikePercent() + m_skillCriticalStrikePercentAdd);
}

Damage Skill::GetMagicDamage(const Target      &target,
                             const TableRes     tableRes,
                             const std::string &subName,
                             const int          level) const
{
    Pct_t damagePercent =
        ((CONST_BIN_PCT + m_skillDamageBinPercentAdd) / static_cast<double>(CONST_BIN_PCT)) *
        (1 + m_player->Attr().GetMagicOvercomePercent()) * (1 + target.GetDamageAddPercent()) *
        (1 - target.GetMagicResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams.at(subName)[level].fixedDamage * damagePercent);
    int weaponDamage =
        static_cast<int>((m_damageParams.at(subName)[level].weaponDamageBinPercent >> 10) *
                         m_player->Attr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams.at(subName)[level].attackDamagePercent *
                                        m_player->Attr().GetMagicAttack() * damagePercent);
    return Damage(GET_FINAL_DAMAGE(fixedDamage,
                                   tableRes,
                                   (m_player->Attr().GetMagicCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)),
                  GET_FINAL_DAMAGE(weaponDamage,
                                   tableRes,
                                   (m_player->Attr().GetMagicCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)),
                  GET_FINAL_DAMAGE(attackDamage,
                                   tableRes,
                                   (m_player->Attr().GetMagicCriticalStrikePowerPercent() +
                                    m_skillCriticalStrikePowerPercentAdd)));
}

void Skill::UpdateMagicStats(const Target      &target,
                             const TableRes     tableRes,
                             const std::string &subName,
                             const int          level,
                             Stats             &stats) const
{
    Damage damage = GetMagicDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::DEFALUT_TYPE] += damage.SumDamage();
    (this->*m_statsDetail)(target, tableRes, subName, level, damage, stats);
}

void Skill::StatsNoneDetail(const Target      &target,
                            const TableRes     tableRes,
                            const std::string &subName,
                            const int          level,
                            const Damage      &damage,
                            Stats             &stats) const
{
}

void Skill::StatsDetail(const Target      &target,
                        const TableRes     tableRes,
                        const std::string &subName,
                        const int          level,
                        const Damage      &damage,
                        Stats             &stats) const
{
    stats.damageStats[target.GetId()][m_id][subName][level][tableRes].first++;
    stats.damageStats[target.GetId()][m_id][subName][level][tableRes].second += damage;
    (this->*m_statsWeaponAttackBonus)(target, tableRes, subName, level, stats);
    (this->*m_statsAttackBaseBonus)(target, tableRes, subName, level, stats);
    (this->*m_statsCriticalStrikePowerBonus)(target, tableRes, subName, level, stats);
    (this->*m_statsOvercomeBaseBonus)(target, tableRes, subName, level, stats);
    (this->*m_statsStrainBonus)(target, tableRes, subName, level, stats);
    (this->*m_statsSurplusBonus)(target, tableRes, subName, level, stats);
}

void Skill::StatsNoneBonus(const Target      &target,
                           const TableRes     tableRes,
                           const std::string &subName,
                           const int          level,
                           Stats             &stats) const
{
}

void Skill::StatsPhysicsWeaponAttackBonus(const Target      &target,
                                          const TableRes     tableRes,
                                          const std::string &subName,
                                          const int          level,
                                          Stats             &stats) const
{
    m_player->Attr().AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    Damage damage = GetPhysicsDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.SumDamage();
    m_player->Attr().AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
}

void Skill::StatsMagicWeaponAttackBonus(const Target      &target,
                                        const TableRes     tableRes,
                                        const std::string &subName,
                                        const int          level,
                                        Stats             &stats) const
{
    m_player->Attr().AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    Damage damage = GetMagicDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.SumDamage();
    m_player->Attr().AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
}

void Skill::StatsPhysicsAttackBaseBonus(const Target      &target,
                                        const TableRes     tableRes,
                                        const std::string &subName,
                                        const int          level,
                                        Stats             &stats) const
{
    m_player->Attr().AddPhysicsAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    Damage damage = GetPhysicsDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::ATTACK_BASE] += damage.SumDamage();
    m_player->Attr().AddPhysicsAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
}

void Skill::StatsMagicAttackBaseBonus(const Target      &target,
                                      const TableRes     tableRes,
                                      const std::string &subName,
                                      const int          level,
                                      Stats             &stats) const
{
    m_player->Attr().AddMagicAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    Damage damage = GetMagicDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::ATTACK_BASE] += damage.SumDamage();
    m_player->Attr().AddMagicAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
}

void Skill::StatsPhysicsCriticalStrikePowerBonus(const Target      &target,
                                                 const TableRes     tableRes,
                                                 const std::string &subName,
                                                 const int          level,
                                                 Stats             &stats) const
{
    m_player->Attr().AddPhysicsCriticalStrikePower(
        g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    Damage damage = GetPhysicsDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.SumDamage();
    m_player->Attr().AddPhysicsCriticalStrikePower(
        -g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
}

void Skill::StatsMagicCriticalStrikePowerBonus(const Target      &target,
                                               const TableRes     tableRes,
                                               const std::string &subName,
                                               const int          level,
                                               Stats             &stats) const
{
    m_player->Attr().AddMagicCriticalStrikePower(
        g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    Damage damage = GetMagicDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.SumDamage();
    m_player->Attr().AddMagicCriticalStrikePower(
        -g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
}

void Skill::StatsPhysicsOvercomeBaseBonus(const Target      &target,
                                          const TableRes     tableRes,
                                          const std::string &subName,
                                          const int          level,
                                          Stats             &stats) const
{
    m_player->Attr().AddPhysicsOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    Damage damage = GetPhysicsDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::OVERCOME_BASE] += damage.SumDamage();
    m_player->Attr().AddPhysicsOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
}

void Skill::StatsMagicOvercomeBaseBonus(const Target      &target,
                                        const TableRes     tableRes,
                                        const std::string &subName,
                                        const int          level,
                                        Stats             &stats) const
{
    m_player->Attr().AddMagicOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    Damage damage = GetMagicDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::OVERCOME_BASE] += damage.SumDamage();
    m_player->Attr().AddMagicOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
}

void Skill::StatsPhysicsStrainBonus(const Target      &target,
                                    const TableRes     tableRes,
                                    const std::string &subName,
                                    const int          level,
                                    Stats             &stats) const
{
    m_player->Attr().AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    Damage damage = GetPhysicsDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::STRAIN] += damage.SumDamage();
    m_player->Attr().AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
}

void Skill::StatsMagicStrainBonus(const Target      &target,
                                  const TableRes     tableRes,
                                  const std::string &subName,
                                  const int          level,
                                  Stats             &stats) const
{
    m_player->Attr().AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    Damage damage = GetMagicDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::STRAIN] += damage.SumDamage();
    m_player->Attr().AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
}

void Skill::StatsPhysicsSurplusBonus(const Target      &target,
                                     const TableRes     tableRes,
                                     const std::string &subName,
                                     const int          level,
                                     Stats             &stats) const
{
    m_player->Attr().AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    Damage damage = GetPhysicsDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::SURPLUS] += damage.SumDamage();
    m_player->Attr().AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
}

void Skill::StatsMagicSurplusBonus(const Target      &target,
                                   const TableRes     tableRes,
                                   const std::string &subName,
                                   const int          level,
                                   Stats             &stats) const
{
    m_player->Attr().AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    Damage damage = GetMagicDamage(target, tableRes, subName, level);
    stats.bonusStats[AttrType::SURPLUS] += damage.SumDamage();
    m_player->Attr().AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
}

void Skill::ResetPublicCooldown()
{
    int     rand        = Random(s_delayMin, s_delayMax);
    Frame_t delayFrames = rand / CONST_DELAY;
    *m_publicCooldown   = s_publicCooldown * m_player->Attr().GetHastePercent() + delayFrames;
}

} // namespace JX3DPS
