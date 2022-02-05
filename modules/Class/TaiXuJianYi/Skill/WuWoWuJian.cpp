#include "WuWoWuJian.h"

#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"
#include "Class/TaiXuJianYi/Buff/DieRen.h"
#include "Class/TaiXuJianYi/Buff/FengShi.h"
#include "Class/TaiXuJianYi/Skill/BaHuangGuiYuan.h"

namespace JX3DPS {

namespace TaiXuJianYi {

WuWoWuJian::WuWoWuJian(Player &player) :
    Skill(player)
{
    InitBaseParams();
    InitDamageParams();
}

WuWoWuJian::WuWoWuJian(const WuWoWuJian &skill) :
    Skill(skill),
    m_skillQidianAdd(skill.m_skillQidianAdd),
    m_qidian(skill.m_qidian)
{

}

WuWoWuJian::~WuWoWuJian()
{

}

WuWoWuJian *WuWoWuJian::Clone()
{
    return new WuWoWuJian(*this);
}

WuWoWuJian &WuWoWuJian::operator=(const WuWoWuJian &skill)
{
    Skill::operator=(skill);
    m_skillQidianAdd = skill.m_skillQidianAdd;
    m_qidian = skill.m_qidian;
    return *this;
}

void WuWoWuJian::Cast(TargetsMap &targetsMap, Stats &stats, CastType castType)
{
    m_qidian = static_cast<TaiXuJianYi *>(m_player)->GetQidian();
    TableRes tableRes = GetPhysicsRollResult(*targetsMap[NORMAL].front());
    UpdatePhysicsStats(*targetsMap[NORMAL].front(), stats, settings, tableRes, m_subNames[0], m_qidian);
    SubEffect(targetsMap, stats, settings, tableRes);
}

void WuWoWuJian::UpdateSkillQidian(int num)
{
    m_skillQidianAdd += num;
}

void WuWoWuJian::InitBaseParams()
{
    m_id = SKI_WU_WO_WU_JIAN;
    m_name = "无我无剑";
    m_subNames.push_back("");
    m_subNames.push_back("白虹");
    m_levelNames.push_back("1气点");
    m_levelNames.push_back("2气点");
    m_levelNames.push_back("3气点");
    m_levelNames.push_back("4气点");
    m_levelNames.push_back("5气点");
    m_levelNames.push_back("6气点");
    m_levelNames.push_back("7气点");
    m_levelNames.push_back("8气点");
    m_levelNames.push_back("9气点");
    m_levelNames.push_back("10气点");
    m_publicCooldown = &m_player->m_publicCooldown;
    m_isPublicCooldown = PUBLIC_SKILL;
    m_skillQidianAdd = 0;
    m_qidian = 0;
}

void WuWoWuJian::InitDamageParams()
{
    m_damageParams[m_subNames[0]].push_back(DamageParam((22 + 24) / 2, 2048, 691));
    m_damageParams[m_subNames[0]].push_back(DamageParam((44 + 49) / 2, 2048, 1405));
    m_damageParams[m_subNames[0]].push_back(DamageParam((67 + 74) / 2, 2048, 2119));
    m_damageParams[m_subNames[0]].push_back(DamageParam((89 + 99) / 2, 2048, 2833));
    m_damageParams[m_subNames[0]].push_back(DamageParam((112 + 124) / 2, 2048, 3547));
    m_damageParams[m_subNames[0]].push_back(DamageParam((134 + 149) / 2, 2048, 4261));
    m_damageParams[m_subNames[0]].push_back(DamageParam((157 + 174) / 2, 2048, 4975));
    m_damageParams[m_subNames[0]].push_back(DamageParam((179 + 198) / 2, 2048, 5689));
    m_damageParams[m_subNames[0]].push_back(DamageParam((202 + 223) / 2, 2048, 6403));
    m_damageParams[m_subNames[0]].push_back(DamageParam((224 + 248) / 2, 2048, 7117));

    m_damageParams[m_subNames[1]].push_back(DamageParam((22 + 24) / 2, 2048, 691));
    m_damageParams[m_subNames[1]].push_back(DamageParam((44 + 49) / 2, 2048, 1405));
    m_damageParams[m_subNames[1]].push_back(DamageParam((67 + 74) / 2, 2048, 2119));
    m_damageParams[m_subNames[1]].push_back(DamageParam((89 + 99) / 2, 2048, 2833));
    m_damageParams[m_subNames[1]].push_back(DamageParam((112 + 124) / 2, 2048, 3547));
    m_damageParams[m_subNames[1]].push_back(DamageParam((134 + 149) / 2, 2048, 4261));
    m_damageParams[m_subNames[1]].push_back(DamageParam((157 + 174) / 2, 2048, 4975));
    m_damageParams[m_subNames[1]].push_back(DamageParam((179 + 198) / 2, 2048, 5689));
    m_damageParams[m_subNames[1]].push_back(DamageParam((202 + 223) / 2, 2048, 6403));
    m_damageParams[m_subNames[1]].push_back(DamageParam((224 + 248) / 2, 2048, 7117));
}

TableRes WuWoWuJian::GetPhysicsRollResultBaiHong(Target &target, RollType rollType)
{
    TableRes tableRes = Roll(m_player->Attr().GetPhysicsHitValuePercent(),
                             m_player->Attr().GetPhysicsCriticalStrikePercent(),
                             m_player->Attr().GetStrainPercent(),
                             target.GetMissPercent(),
                             target.GetSightPercent(),
                             rollType);
    return tableRes;
}

Damage WuWoWuJian::GetPhysicsDamageBaiHong(Target &target, TableRes tableRes, std::string &subName, int level)
{
    Pct_t damagePercent = ((MAX_BIN_PCT_CONST + m_skillDamageBinPercentAdd) / static_cast<double>(MAX_BIN_PCT_CONST)) *
                          (1 + m_player->Attr().GetPhysicsOvercomePercent()) *
                          (1 + target.GetDamagePercentAdd()) *
                          (1 - target.GetPhysicsResistPercent());
    int fixedDamage = static_cast<int>(m_damageParams[subName][level].fixedDamage * damagePercent);
    int weaponDamage = static_cast<int>((m_damageParams[subName][level].weaponDamageBinPercent >> 10) *
                                        m_player->Attr().GetWeaponAttack() * damagePercent);
    int attackDamage = static_cast<int>(m_damageParams[subName][level].attackDamagePercent *
                                        m_player->Attr().GetPhysicsAttack() * damagePercent);
    int tableResNum = tableRes;
    return Damage(GET_FINAL_DAMAGE(fixedDamage,
                                        tableResNum,
                                        (m_player->Attr().GetPhysicsCriticalStrikePowerPercent())),
                       GET_FINAL_DAMAGE(weaponDamage,
                                        tableResNum,
                                        (m_player->Attr().GetPhysicsCriticalStrikePowerPercent())),
                       GET_FINAL_DAMAGE(attackDamage,
                                        tableResNum,
                                        (m_player->Attr().GetPhysicsCriticalStrikePowerPercent())));
}

void WuWoWuJian::UpdatePhysicsStatsBaiHong(Target &target, Stats &stats, TableRes tableRes, std::string &subName, int level)
{
    Damage damage;
    switch (static_cast<int>(settings.mode)) {
    case SIM_MODE::DEFAULT :
        damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
        stats.damageStats[target.GetId()][m_id][subName][level][tableRes].first++;
        stats.damageStats[target.GetId()][m_id][subName][level][tableRes].second += damage;
        stats.bonusStats[AttrType::DEFAULT] += damage.SumDamage();
        if (settings.bonusSets[AttrType::WEAPON_ATTACK]->set) {
            m_player->Attr().AddWeaponAttack(settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
            damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
            stats.bonusStats[AttrType::WEAPON_ATTACK] += damage.SumDamage();
            m_player->Attr().AddWeaponAttack(-settings.bonusSets[AttrType::WEAPON_ATTACK]->value);
        }
        if (settings.bonusSets[AttrType::ATTACK_BASE]->set) {
            m_player->Attr().AddPhysicsAttackBase(settings.bonusSets[AttrType::ATTACK_BASE]->value);
            damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
            stats.bonusStats[AttrType::ATTACK_BASE] += damage.SumDamage();
            m_player->Attr().AddPhysicsAttackBase(-settings.bonusSets[AttrType::ATTACK_BASE]->value);
        }
        if (settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->set) {
            m_player->Attr().AddPhysicsCriticalStrikePower(settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
            damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
            stats.bonusStats[AttrType::CRITICAL_STRIKE_POWER] += damage.SumDamage();
            m_player->Attr().AddPhysicsCriticalStrikePower(-settings.bonusSets[AttrType::CRITICAL_STRIKE_POWER]->value);
        }
        if (settings.bonusSets[AttrType::OVERCOME_BASE]->set) {
            m_player->Attr().AddPhysicsOvercomeBase(settings.bonusSets[AttrType::OVERCOME_BASE]->value);
            damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
            stats.bonusStats[AttrType::OVERCOME_BASE] += damage.SumDamage();
            m_player->Attr().AddPhysicsOvercomeBase(-settings.bonusSets[AttrType::OVERCOME_BASE]->value);
        }
        break;
    case SIM_MODE::PRIMARY :
        damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
        stats.bonusStats[AttrType::PRIMARY] += damage.SumDamage();
        break;
    case SIM_MODE::HIT_VALUE :
        damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
        stats.bonusStats[AttrType::HIT_VALUE] += damage.SumDamage();
        break;
    case SIM_MODE::CRITICAL_STRIKE :
        damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
        stats.bonusStats[AttrType::CRITICAL_STRIKE] += damage.SumDamage();
        break;
    case SIM_MODE::HASTE :
        damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
        stats.bonusStats[AttrType::HASTE] += damage.SumDamage();
        break;
    case SIM_MODE::STRAIN :
        damage = GetPhysicsDamageBaiHong(target, tableRes, subName, level);
        stats.bonusStats[AttrType::STRAIN] += damage.SumDamage();
        break;
    }
}

void WuWoWuJian::SubEffect(TargetsMap &targetsMap, Stats &stats, TableRes tableRes)
{
    static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(-10);
    if (Random(0.0, 1.0) < 0.25 && m_player->talents[KUN_WU] && m_qidian == 10) {
        static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(6);
    }
    if (tableRes != TableRes::MISS) {
        if (m_player->talents[FENG_SHI]) {
            m_player->buffs[BUF_FENG_SHI]->Clean(targetsMap, stats, settings, tableRes);
        }
        static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(m_skillQidianAdd);
        if (m_player->buffs[BUF_CLASS_CW]->IsExist()) {
            static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(10);
        }
        if (tableRes == TableRes::DOUBLE) {
            if (m_player->talents[SHEN_MAI]) {
                static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(2);
            }
            if (m_player->talents[DIE_REN]) {
                static_cast<DieRen *>(targetsMap[NORMAL].front()->buffs[TBUF_DIE_REN])->Refresh();
            }
        }
        if (targetsMap[NORMAL].front()->buffs[TBUF_DIE_REN]->IsExist() && m_player->talents[WU_YU]) {
            static_cast<BaHuangGuiYuan *>(m_player->skills[SKI_BA_HUANG_GUI_YUAN])->UpdateCooldown(8 * m_qidian);
        }
        if (m_player->talents[BAI_HONG]) {
            for (auto it : targetsMap[NORMAL]) {
                if (it != targetsMap[NORMAL].front()) {
                    TableRes tableRes = GetPhysicsRollResultBaiHong(*targetsMap[NORMAL].front());
                    UpdatePhysicsStatsBaiHong(*targetsMap[NORMAL].front(), stats, settings, tableRes, m_subNames[0], m_qidian);
                    SubEffectBaiHong(targetsMap, stats, settings, tableRes);
                }
            }
        }
    } else {
        static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(6);
    }
    if (m_player->GetGearSetAttack() && Random(0.0, 1.0) < 0.1) {
        static_cast<Buff *>(m_player->buffs[BUF_CLASS_ATTACK])->Refresh();
    }
}

void WuWoWuJian::SubEffectBaiHong(TargetsMap &targetsMap, Stats &stats, TableRes tableRes)
{
    if (m_player->talents[SHEN_MAI] == TableRes::DOUBLE) {
        static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(2);
    }
}

}

}
