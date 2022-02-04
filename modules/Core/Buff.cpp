#include "Buff.h"

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
    if (g_settings.simMode == SIM_MODE::DEFALUT_TYPE && g_settings.classType % 2 == 0) {    // ClassType % 2 == 0 即外功
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
    } else if (g_settings.simMode == SIM_MODE::DEFALUT_TYPE && g_settings.classType % 2 == 1) {
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
    if (g_settings.simMode == SIM_MODE::DEFALUT_TYPE && g_settings.classType % 2 == 0) {    // ClassType % 2 == 0 即外功
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
    } else if (g_settings.simMode == SIM_MODE::DEFALUT_TYPE && g_settings.classType % 2 == 1) {
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

const std::string &Buff::GetSubName(const int subIndex) const
{
    return m_subNames[subIndex];
}

const std::string &Buff::GetLevelName(const int levelIndex) const
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

#if JX3_MAJOR_VERSION >= 110
TableRes Buff::Roll(const Pct_t playerCriticalStrikePercent) const
{
    return RandBetween(0.0, 1.0) < playerCriticalStrikePercent ? TableRes::DOUBLE : TableRes::HIT;
}
#else
TableRes Buff::Roll(const Pct_t playerHitPercent, const Pct_t playerCriticalStrikePercent, const Pct_t playerStrainPercent,
                    const Pct_t targetMissPercent, const Pct_t targetSightPercent, const RollType rollType = RollType::COMMON);
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
            return TableRes::HIT;
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
            return TableRes::HIT;
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
            return TableRes::HIT;
        }
    }
}
#endif // JX3_MAJOR_VERSION >= 110

TableRes Buff::GetPhysicsRollResult(const Attr &attr) const
{
    return Roll(attr.GetPhysicsCriticalStrikePercent());
}

Damage Buff::GetPhysicsDamage(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level) const
{
    Pct_t damagePercent = (1 + m_player->GetAttr().GetPhysicsOvercomePercent()) *
                          (1 + target.GetDamageAddPercent()) *
                          (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams.at(subName)[level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParams.at(subName)[level].weaponDamageBinPercent >> 10) * m_player->GetAttr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams.at(subName)[level].attackDamagePercent * attr.GetPhysicsAttack() * damagePercent);
    return Damage(GET_FINAL_DAMAGE(fixedDamage, tableRes, attr.GetPhysicsCriticalStrikePowerPercent()),
                  GET_FINAL_DAMAGE(weaponDamage, tableRes, attr.GetPhysicsCriticalStrikePowerPercent()),
                  GET_FINAL_DAMAGE(attackDamage, tableRes, attr.GetPhysicsCriticalStrikePowerPercent()));
}

void Buff::UpdatePhysicsStats(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    Damage damage = GetPhysicsDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::DEFALUT_TYPE] += damage.SumDamage();
    (this->*m_statsDetail)(attr, target, tableRes, subName, level, damage, stats);
}

TableRes Buff::GetMagicRollResult(const Attr &attr) const
{
    return Roll(attr.GetMagicCriticalStrikePercent());
}

Damage Buff::GetMagicDamage(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level) const
{
    Pct_t damagePercent = (1 + m_player->GetAttr().GetMagicOvercomePercent()) *
                          (1 + target.GetDamageAddPercent()) *
                          (1 - target.GetMagicResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams.at(subName)[level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParams.at(subName)[level].weaponDamageBinPercent >> 10) * m_player->GetAttr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams.at(subName)[level].attackDamagePercent * attr.GetMagicAttack() * damagePercent);
    return Damage(GET_FINAL_DAMAGE(fixedDamage, tableRes, attr.GetMagicCriticalStrikePowerPercent()),
                  GET_FINAL_DAMAGE(weaponDamage, tableRes, attr.GetMagicCriticalStrikePowerPercent()),
                  GET_FINAL_DAMAGE(attackDamage, tableRes, attr.GetMagicCriticalStrikePowerPercent()));
}

void Buff::UpdateMagicStats(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    Damage damage = GetMagicDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::DEFALUT_TYPE] += damage.SumDamage();
    (this->*m_statsDetail)(attr, target, tableRes, subName, level, damage, stats);
}

void Buff::StatsNoneDetail(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, const Damage &damage, Stats &stats) const
{

}

void Buff::StatsDetail(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, const Damage &damage, Stats &stats) const
{
    stats.damageStats[target.GetId()][m_id][subName][level][tableRes].first++;
    stats.damageStats[target.GetId()][m_id][subName][level][tableRes].second += damage;
    (this->*m_statsWeaponAttackBonus)(attr, target, tableRes, subName, level, stats);
    (this->*m_statsAttackBaseBonus)(attr, target, tableRes, subName, level, stats);
    (this->*m_statsCriticalStrikePowerBonus)(attr, target, tableRes, subName, level, stats);
    (this->*m_statsOvercomeBaseBonus)(attr, target, tableRes, subName, level, stats);
    (this->*m_statsStrainBonus)(attr, target, tableRes, subName, level, stats);
    (this->*m_statsSurplusBonus)(attr, target, tableRes, subName, level, stats);
}

void Buff::StatsNoneBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{

}

void Buff::StatsPhysicsWeaponAttackBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    m_attrSnap->AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    Damage damage = GetPhysicsDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.SumDamage();
    m_player->GetAttr().AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    m_attrSnap->AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
}

void Buff::StatsMagicWeaponAttackBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    m_attrSnap->AddWeaponAttack(g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    Damage damage = GetMagicDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.SumDamage();
    m_player->GetAttr().AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
    m_attrSnap->AddWeaponAttack(-g_settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
}

void Buff::StatsPhysicsAttackBaseBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddPhysicsAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    m_attrSnap->AddPhysicsAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    Damage damage = GetPhysicsDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::ATTACK_BASE] += damage.SumDamage();
    m_player->GetAttr().AddPhysicsAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    m_attrSnap->AddPhysicsAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
}

void Buff::StatsMagicAttackBaseBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddMagicAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    m_attrSnap->AddMagicAttackBase(g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    Damage damage = GetMagicDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::ATTACK_BASE] += damage.SumDamage();
    m_player->GetAttr().AddMagicAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
    m_attrSnap->AddMagicAttackBase(-g_settings.bonusSets[AttrType::ATTACK_BASE]->value);
}

void Buff::StatsPhysicsCriticalStrikePowerBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddPhysicsCriticalStrikePower(g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    m_attrSnap->AddPhysicsCriticalStrikePower(g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    Damage damage = GetPhysicsDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.SumDamage();
    m_player->GetAttr().AddPhysicsCriticalStrikePower(-g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    m_attrSnap->AddPhysicsCriticalStrikePower(-g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
}

void Buff::StatsMagicCriticalStrikePowerBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddMagicCriticalStrikePower(g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    m_attrSnap->AddMagicCriticalStrikePower(g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    Damage damage = GetMagicDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.SumDamage();
    m_player->GetAttr().AddMagicCriticalStrikePower(-g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
    m_attrSnap->AddMagicCriticalStrikePower(-g_settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
}

void Buff::StatsPhysicsOvercomeBaseBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddPhysicsOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    m_attrSnap->AddPhysicsOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    Damage damage = GetPhysicsDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::OVERCOME_BASE] += damage.SumDamage();
    m_player->GetAttr().AddPhysicsOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    m_attrSnap->AddPhysicsOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
}

void Buff::StatsMagicOvercomeBaseBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddMagicOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    m_attrSnap->AddMagicOvercomeBase(g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    Damage damage = GetMagicDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::OVERCOME_BASE] += damage.SumDamage();
    m_player->GetAttr().AddMagicOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
    m_attrSnap->AddMagicOvercomeBase(-g_settings.bonusSets[AttrType::OVERCOME_BASE]->value);
}

void Buff::StatsPhysicsStrainBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    m_attrSnap->AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    Damage damage = GetPhysicsDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::STRAIN] += damage.SumDamage();
    m_player->GetAttr().AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
    m_attrSnap->AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
}

void Buff::StatsMagicStrainBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    m_attrSnap->AddStrain(g_settings.bonusSets[AttrType::STRAIN]->value);
    Damage damage = GetMagicDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::STRAIN] += damage.SumDamage();
    m_player->GetAttr().AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
    m_attrSnap->AddStrain(-g_settings.bonusSets[AttrType::STRAIN]->value);
}

void Buff::StatsPhysicsStrainBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    m_attrSnap->AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    Damage damage = GetPhysicsDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::SURPLUS] += damage.SumDamage();
    m_player->GetAttr().AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
    m_attrSnap->AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
}

void Buff::StatsPhysicsStrainBonus(const Attr &attr, const Target &target, const TableRes tableRes, const std::string &subName, const int level, Stats &stats) const
{
    m_player->GetAttr().AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    m_attrSnap->AddSurplusBase(g_settings.bonusSets[AttrType::SURPLUS]->value);
    Damage damage = GetMagicDamage(attr, target, tableRes, subName, level);
    stats.bonusStats[AttrType::SURPLUS] += damage.SumDamage();
    m_player->GetAttr().AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
    m_attrSnap->AddSurplusBase(-g_settings.bonusSets[AttrType::SURPLUS]->value);
}

}
