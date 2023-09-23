/**
 * Project: JX3DPS
 * File: MoWenBuff.cpp
 * Description:
 * Created Date: 2023-08-01 23:06:41
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-24 03:13:22
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "MoWenBuff.h"

#include "Damage/Damage.hpp"

#include "MoWenSkill.h"
#include "Skill.h"
#include "Target.hpp"

namespace JX3DPS {

namespace MoWen {

namespace Buff {

Shang::Shang(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id          = BUFF_SHANG;
    m_name        = "Dot·商";
    m_interval    = 48;
    m_effectCount = 6;

    m_damageParams[0].emplace_back((19 + 19) / 2, 0, 263);

    if (m_player->recipes[RECIPE_SHANG_CRITICAL_STRIKE_2]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 200;
    }

    if (m_player->recipes[RECIPE_SHANG_CRITICAL_STRIKE_3]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 300;
    }

    if (m_player->recipes[RECIPE_SHANG_CRITICAL_STRIKE_4]) {
        m_effectCriticalStrikeAdditionalBasisPointInt += 400;
    }
    
    if (m_player->recipes[RECIPE_SHANG_DAMAGE_3]) {
        // m_effectDamageAdditionalPercentInt += 31;
        m_damageParams[0][0].attackDamagePercentInt * 1.03;
    }

    if (m_player->recipes[RECIPE_SHANG_DAMAGE_4]) {
        // m_effectDamageAdditionalPercentInt += 41;
        m_damageParams[0][0].attackDamagePercentInt * 1.04;
    }

    if (m_player->recipes[RECIPE_SHANG_DAMAGE_5]) {
        // m_effectDamageAdditionalPercentInt += 51;
        m_damageParams[0][0].attackDamagePercentInt * 1.05;
    }
}

void Shang::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) {
            iter->second.interval = m_interval * iter->second.hastePercent;
            SubEffect(iter->first);
        }
        if (iter->second.duration == 0) {
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Shang::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在Dot
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration =
            m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
            m_snapshots[targetId].interval; // 对齐Dot保证每次刷新都是6跳，且消失时最后一跳
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void Shang::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(targetId) != m_snapshots.end()) {
        m_snapshots.erase(targetId);
    }
}

void Shang::TriggerAdd(Id_t targetId)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在Dot
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].duration =
        m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
        m_snapshots[targetId].interval; // 对齐Dot保证每次刷新都是6跳，且消失时最后一跳

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);
}

void Shang::SubEffect(Id_t targetId)
{
    RollResult  rollResult = GetDotRollResult(targetId);
    GainsDamage damage     = CalcMagicDotDamage(targetId, rollResult, 0, 0, 1);
    Record(m_id, targetId, rollResult, damage, 0, 0);
}

Jue::Jue(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id          = BUFF_JUE;
    m_name        = "Dot·角";
    m_interval    = 48;
    m_effectCount = 6;

    m_damageParams[0].emplace_back((19 + 19) / 2, 0, 256);
}

void Jue::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) {
            iter->second.interval = m_interval * iter->second.hastePercent;
            SubEffect(iter->first);
        }
        if (iter->second.duration == 0) {
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Jue::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在Dot
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration =
            m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
            m_snapshots[targetId].interval; // 对齐Dot保证每次刷新都是6跳，且消失时最后一跳
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void Jue::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(targetId) != m_snapshots.end()) {
        m_snapshots.erase(targetId);
    }
}

void Jue::TriggerAdd(Id_t targetId)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在Dot
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].duration =
        m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
        m_snapshots[targetId].interval; // 对齐Dot保证每次刷新都是6跳，且消失时最后一跳

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);
}

void Jue::SubEffect(Id_t targetId)
{
    RollResult  rollResult = GetDotRollResult(targetId);
    GainsDamage damage     = CalcMagicDotDamage(targetId, rollResult, 0, 0, 1);
    Record(m_id, targetId, rollResult, damage, 0, 0);
}

XianFeng::XianFeng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_XIAN_FENG;
    m_name     = "弦风";
    m_stackNum = 5;
    m_duration = 10 * 16;

    m_damageParams[0].emplace_back((40 + 40 + 2) / 2, 0, 40);
}

void XianFeng::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void XianFeng::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void XianFeng::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        if (m_snapshots[PLAYER_ID].stackNum <= 0) {
            m_snapshots.erase(PLAYER_ID);
        }
    }
}

void XianFeng::TriggerAdd()
{
    m_snapshots[PLAYER_ID].stackNum++;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void XianFeng::TriggerClear()
{
    m_snapshots.erase(PLAYER_ID);
}

void XianFeng::TriggerDamage(Id_t targetId, int stackNum)
{
    Params params;
    params.player = m_player;

    for (int i = 0; i < stackNum; ++i) {
        m_triggerEffects[TRIGGER_SET_ATTRIBUTE](params);
        m_triggerEffects[TRIGGER_WEAPON_CW](params);
        m_triggerEffects[TRIGGER_WEAPON_WATER](params);
        RollResult  rollResult = GetMagicRollResult();
        GainsDamage damage     = CalcMagicDamage(targetId, rollResult, 0, 0);
        Record(m_id, targetId, rollResult, damage, 0, 0);
    }
}

XianFengBiaoJi::XianFengBiaoJi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_XIAN_FENG_BIAO_JI;
    m_name     = "弦风标记";
    m_duration = 3 * 16;
}

void XianFengBiaoJi::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void XianFengBiaoJi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void XianFengBiaoJi::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void XianFengBiaoJi::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = m_duration;
}

YunHan::YunHan(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_YUN_HAN;
    m_name     = "云汉";
    m_stackNum = 9;
    m_duration = 100 * 16;
}

void YunHan::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        int stack = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots.erase(PLAYER_ID);
        SubEffectAdd(-stack);
    }
}

void YunHan::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    stack = m_snapshots[PLAYER_ID].stackNum - stack;
    SubEffectAdd(stack);
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void YunHan::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        m_snapshots[PLAYER_ID].stackNum  = std::max(m_snapshots[PLAYER_ID].stackNum, 0);
        stack                            = m_snapshots[PLAYER_ID].stackNum - stack;
        SubEffectAdd(stack);
        if (m_snapshots[PLAYER_ID].stackNum <= 0) {
            m_snapshots.erase(PLAYER_ID);
        }
    }
}

void YunHan::TriggerAdd(int stackNum)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].duration  = m_duration;
    if (m_snapshots[PLAYER_ID].stackNum <= 0) {
        m_snapshots.erase(PLAYER_ID);
    }
    SubEffectAdd(m_snapshots[PLAYER_ID].stackNum - stack);
}

void YunHan::SubEffectAdd(int stackNum)
{
    m_player->effectDamageAdditionalPercentInt += stackNum * 51;
}

CanLian::CanLian(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id   = BUFF_CAN_LIAN;
    m_name = "参连";
}

void CanLian::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void CanLian::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    SubEffectAdd();
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void CanLian::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        SubEffectClear();
        m_snapshots.erase(PLAYER_ID);
    }
}

void CanLian::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
    SubEffectAdd();
}

void CanLian::TriggerClear()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void CanLian::SubEffectAdd()
{
    m_player->attribute.AddMagicAttackPowerBaseAdditionalPercentInt(102);
}

void CanLian::SubEffectClear()
{
    m_player->attribute.AddMagicAttackPowerBaseAdditionalPercentInt(-102);
}

MingJin::MingJin(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_MING_JIN;
    m_name     = "书离";
    m_duration = 8 * 16;
}

void MingJin::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void MingJin::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration += m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration += RandomUniform(durationMin, durationMax);
    }
}

void MingJin::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        SubEffectClear();
        m_snapshots.erase(PLAYER_ID);
    }
}

void MingJin::TriggerAdd()
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration += m_duration;
}

void MingJin::TriggerClear()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void MingJin::SubEffectAdd()
{
    m_player->skills[SKILL_GONG]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_BIAN_GONG]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_SHANG]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_JUE]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_ZHI]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_BIAN_ZHI]->AddDamageAdditionalPercentInt(205);
    m_player->skills[SKILL_YU]->AddDamageAdditionalPercentInt(205);

    m_player->buffs[BUFF_SHANG]->AddDamageAdditionalPercentInt(205);
    m_player->buffs[BUFF_JUE]->AddDamageAdditionalPercentInt(205);
}

void MingJin::SubEffectClear()
{
    m_player->skills[SKILL_GONG]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_BIAN_GONG]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_SHANG]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_JUE]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_ZHI]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_BIAN_ZHI]->AddDamageAdditionalPercentInt(-205);
    m_player->skills[SKILL_YU]->AddDamageAdditionalPercentInt(-205);

    m_player->buffs[BUFF_SHANG]->AddDamageAdditionalPercentInt(-205);
    m_player->buffs[BUFF_JUE]->AddDamageAdditionalPercentInt(-205);
}

LiuZhao::LiuZhao(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id                                     = BUFF_LIU_ZHAO;
    m_name                                   = "流照";
    m_duration                               = 25 * 16;
    m_stackNum                               = 32;
    m_effectShieldIgnoreAdditionalPercentInt = 922;

    m_damageParams[0].emplace_back((107 + 107 + 27) / 2, 0, 61);

    m_damageParams[1].emplace_back(0,
                                   0,
                                   JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE *
                                       (0.125 * 0.5 * 1.3 * 1.2 * 0.5 * 1.11 - 1));
}

void LiuZhao::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void LiuZhao::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    stack = m_snapshots[PLAYER_ID].stackNum - stack;
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void LiuZhao::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        m_snapshots[PLAYER_ID].stackNum  = std::max(m_snapshots[PLAYER_ID].stackNum, 0);
        stack                            = m_snapshots[PLAYER_ID].stackNum - stack;
        if (m_snapshots[PLAYER_ID].stackNum <= 0) {
            m_snapshots.erase(PLAYER_ID);
        }
    }
}

void LiuZhao::TriggerAdd(int stackNum)
{
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void LiuZhao::TriggerDamage(int stackNum)
{
    Params params;
    params.player = m_player;

    for (int i = 0; i < stackNum; ++i) {
        m_triggerEffects[TRIGGER_SET_ATTRIBUTE](params);
        m_triggerEffects[TRIGGER_WEAPON_CW](params);
        m_triggerEffects[TRIGGER_WEAPON_WATER](params);
        RollResult  rollResult = GetMagicRollResult();
        GainsDamage damage = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
        Record(m_id, m_player->GetTargetId(), rollResult, damage, 0, 0);
    }
}

void LiuZhao::TriggerSurplusDamage()
{
    for (int i = 0; i < m_snapshots[PLAYER_ID].stackNum; ++i) {
        RollResult rollResult = GetMagicRollResult();
        GainsDamage damage = CalcMagicSurplusDamage(m_player->GetTargetId(), rollResult, 1, 0);
        SurplusRecord(m_player->GetTargetId(), rollResult, damage, 0, 0);
    }
    m_snapshots.erase(PLAYER_ID);
}

JX3DPS::Damage LiuZhao::GetMagicSurplusDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    Value_t    surplus,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain)
{
    Damage damage;

    PctInt_t surplusCoefficientInt = m_damageParams.at(sub)[level].attackDamagePercentInt;
    Value_t surplusDamage = SurplusDamage(surplus, surplusCoefficientInt, JX3_PLAYER_LEVEL);

    int      playerLevel                = JX3_PLAYER_LEVEL;
    int      targetLevel                = (*m_targets)[targetId]->GetLevel();
    Value_t  shieldBase                 = (*m_targets)[targetId]->GetPhysicsShield();
    Value_t  shieldAdditional           = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attribute.GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldAdditionalPercentInt = 0;
    int      rollResultInt                    = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePowerAdditionalPercentInt =
        (m_effectCriticalStrikePowerAdditionalPercentInt +
         m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt());
    PctInt_t strainPercentInt = m_player->attribute.GetStrainBaseAdditionalPercentInt();
    PctInt_t pveDamageAdditionalPercentInt = m_player->attribute.GetPVEDamageAdditionalPercentInt();
    PctInt_t vulnerablePercentInt = (*m_targets)[targetId]->GetDamageAdditionalPercentInt();

    damage.surplusDamage = FinalMagicDamage(
        playerLevel,
        targetLevel,
        surplusDamage,
        shieldBase,
        shieldAdditional,
        ignoreShieldBasePercentInt,
        ignoreShieldAdditionalPercentInt,
        overcome,
        rollResultInt,
        criticalStrikePower,
        effectCriticalStrikePowerAdditionalPercentInt,
        strain,
        strainPercentInt,
        pveDamageAdditionalPercentInt,
        vulnerablePercentInt);

    return damage;
}

JX3DPS::GainsDamage LiuZhao::CalcMagicSurplusDamage(Id_t targetId, RollResult rollResult, int sub, int level)
{
    GainsDamage gainsDamage;

    std::array<Attribute::Type, 6> types = {
        {{ Attribute::Type::DEFAULT },
         { Attribute::Type::SURPLUS_VALUE_BASE },
         { Attribute::Type::CRITICAL_STRIKE_POWER },
         { Attribute::Type::OVERCOME_BASE },
         { Attribute::Type::STRAIN_BASE }}
    };

    for (const auto &type : types) {
        Value_t surplus             = m_player->attribute.GetSurplusValueBase();
        Value_t criticalStrikePower = m_player->attribute.GetMagicCriticalStrikePower();
        Value_t strain              = m_player->attribute.GetStrainBase();
        Value_t overcome            = m_player->attribute.GetMagicOvercome();
        Value_t weaponDamage        = m_player->attribute.GetWeaponDamage();

        switch (type) {
            case Attribute::Type::SURPLUS_VALUE_BASE:
                m_player->attribute.AddSurplusValueBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                surplus = m_player->attribute.GetSurplusValueBase();
                m_player->attribute.AddSurplusValueBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::CRITICAL_STRIKE_POWER:
                m_player->attribute.AddMagicCriticalStrikePower(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                criticalStrikePower = m_player->attribute.GetMagicCriticalStrikePower();
                m_player->attribute.AddMagicCriticalStrikePower(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::OVERCOME_BASE:
                m_player->attribute.AddMagicOvercomeBaseAdditional(
                    Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                overcome = m_player->attribute.GetMagicOvercome();
                m_player->attribute.AddMagicOvercomeBaseAdditional(
                    -Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            case Attribute::Type::STRAIN_BASE:
                m_player->attribute.AddStrainBase(Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                strain = m_player->attribute.GetStrainBase();
                m_player->attribute.AddStrainBase(-Attribute::ATTRIBUTE_GAIN_BY_BASE.at(type));
                break;
            default: break;
        }

        gainsDamage[type] =
            GetMagicSurplusDamage(targetId, rollResult, sub, level, surplus, criticalStrikePower, overcome, strain);
    }
    gainsDamage[Attribute::Type::WEAPON_DAMAGE_BASE] = gainsDamage[Attribute::Type::DEFAULT];
    gainsDamage[Attribute::Type::ATTACK_POWER_BASE] = gainsDamage[Attribute::Type::DEFAULT];

    return gainsDamage;
}

void LiuZhao::SurplusRecord(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub, int level)
{
    for (const auto &[type, damage] : gainsDamage) {
        m_stats.gainStats[type][targetId][BUFF_PO_ZHAO_LIU_ZHAO][sub][level][rollResult].first++;
        m_stats.gainStats[type][targetId][BUFF_PO_ZHAO_LIU_ZHAO][sub][level][rollResult]
            .second.damage += damage.damage;
        m_stats.gainStats[type][targetId][BUFF_PO_ZHAO_LIU_ZHAO][sub][level][rollResult]
            .second.surplusDamage += damage.surplusDamage;
    }
}

GaoShanLiuShui::GaoShanLiuShui(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_GAO_SHAN_LIU_SHUI;
    m_name     = "高山流水";
    m_duration = 12 * 16;
}

void GaoShanLiuShui::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void GaoShanLiuShui::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void GaoShanLiuShui::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        SubEffectClear();
        m_snapshots.erase(PLAYER_ID);
    }
}

void GaoShanLiuShui::TriggerAdd()
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void GaoShanLiuShui::TriggerClear()
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        SubEffectClear();
        m_snapshots.erase(PLAYER_ID);
    }
}

void GaoShanLiuShui::SubEffectAdd()
{
    m_player->attribute.AddHasteBaseAdditionalPercentInt(205);
}

void GaoShanLiuShui::SubEffectClear()
{
    m_player->attribute.AddHasteBaseAdditionalPercentInt(-205);
}

QuFeng::QuFeng(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_QU_FENG;
    m_name     = "曲风";
    m_duration = JX3DPS_INVALID_FRAMES_SET;
    m_stackNum = 9;

    m_snapshots[PLAYER_ID].stackNum = 0;
    m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
}

void QuFeng::Trigger() { }

void QuFeng::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    if (m_snapshots[PLAYER_ID].stackNum == m_stackNum) {
        m_snapshots[PLAYER_ID].stackNum = 0;
        SubEffect();
    } else if (m_snapshots[PLAYER_ID].stackNum > m_stackNum) {
        m_snapshots[PLAYER_ID].stackNum = 0;
    }
}

void QuFeng::Clear(Id_t targetId, int stackNum)
{
    m_snapshots[PLAYER_ID].stackNum = 0;
}

void QuFeng::TriggerAdd(int stackNum)
{
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    if (m_snapshots[PLAYER_ID].stackNum == m_stackNum) {
        m_snapshots[PLAYER_ID].stackNum = 0;
        SubEffect();
    } else if (m_snapshots[PLAYER_ID].stackNum > m_stackNum) {
        m_snapshots[PLAYER_ID].stackNum = 0;
    }
}

void QuFeng::TriggerClear()
{
    m_snapshots[PLAYER_ID].stackNum = 0;
}

void QuFeng::TriggerSet(int stackNum)
{
    m_snapshots[PLAYER_ID].stackNum = stackNum;
}

void QuFeng::SubEffect()
{
    static_cast<MoWen::Skill::PoZhao *>(m_player->skills[SKILL_PO_ZHAO])
        ->TriggerDamage(m_player->GetTargetId());
    if (m_player->buffs[BUFF_ZHENG_LV_HE_MING]->GetDurationCurrent() > 0) {
        static_cast<ZhiYinHeMing *>(m_player->buffs[BUFF_ZHI_YIN_HE_MING])->TriggerAdd();
    }
}

YangChunBaiXue::YangChunBaiXue(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_YANG_CHUN_BAI_XUE;
    m_name     = "阳春白雪";
    m_duration = 6 * 16;
    m_stackNum = 3;
}

void YangChunBaiXue::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        int stackNum = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear(stackNum);
    }
}

void YangChunBaiXue::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_stackNum, m_snapshots[PLAYER_ID].stackNum);
    stack = m_snapshots[PLAYER_ID].stackNum - stack;
    SubEffectAdd(stack);
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void YangChunBaiXue::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        m_snapshots[PLAYER_ID].stackNum  = std::max(0, m_snapshots[PLAYER_ID].stackNum);
        stack                            = stack - m_snapshots[PLAYER_ID].stackNum;
        SubEffectClear(stack);
        m_snapshots.erase(PLAYER_ID);
    }
}

void YangChunBaiXue::TriggerAdd()
{
    SubEffectAdd(1);
    m_snapshots[PLAYER_ID].duration = m_duration;
    m_snapshots[PLAYER_ID].stackNum++;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_stackNum, m_snapshots[PLAYER_ID].stackNum);
}

void YangChunBaiXue::TriggerClear()
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stackNum = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear(stackNum);
    }
}

void YangChunBaiXue::SubEffectAdd(int stackNum)
{
    m_player->skills[SKILL_GONG]->AddShieldIgnoreAdditionalPercentInt(307 * stackNum);
    m_player->skills[SKILL_ZHI]->AddShieldIgnoreAdditionalPercentInt(307 * stackNum);
}

void YangChunBaiXue::SubEffectClear(int stackNum)
{
    m_player->skills[SKILL_GONG]->AddShieldIgnoreAdditionalPercentInt(-307 * stackNum);
    m_player->skills[SKILL_ZHI]->AddShieldIgnoreAdditionalPercentInt(-307 * stackNum);
}

ZhiYinMiaoYi::ZhiYinMiaoYi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_ZHI_YIN_MIAO_YI;
    m_name     = "知音妙意";
    m_duration = 15 * 16;
    m_stackNum = 3;
}

void ZhiYinMiaoYi::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        int stackNum = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear(stackNum);
    }
}

void ZhiYinMiaoYi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_stackNum, m_snapshots[PLAYER_ID].stackNum);
    stack = m_snapshots[PLAYER_ID].stackNum - stack;
    SubEffectAdd(stack);
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void ZhiYinMiaoYi::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        m_snapshots[PLAYER_ID].stackNum  = std::max(0, m_snapshots[PLAYER_ID].stackNum);
        stack                            = stack - m_snapshots[PLAYER_ID].stackNum;
        SubEffectClear(stack);
        m_snapshots.erase(PLAYER_ID);
    }
}

void ZhiYinMiaoYi::TriggerAdd(int stackNum)
{
    int stack                        = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    m_snapshots[PLAYER_ID].stackNum = std::min(m_stackNum, m_snapshots[PLAYER_ID].stackNum);
    stack = m_snapshots[PLAYER_ID].stackNum - stack;
    SubEffectAdd(stack);
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void ZhiYinMiaoYi::TriggerClear()
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        int stackNum = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear(stackNum);
    }
}

void ZhiYinMiaoYi::SubEffectAdd(int stackNum)
{
    m_player->attribute.AddMagicCriticalStrikePowerAdditionalPercentInt(102.5 * stackNum);
}

void ZhiYinMiaoYi::SubEffectClear(int stackNum)
{
    m_player->attribute.AddMagicCriticalStrikePowerAdditionalPercentInt(-102.5 * stackNum);
}

GuYingHuaShuang::GuYingHuaShuang(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_GU_YING_HUA_SHUANG;
    m_name     = "孤影化双";
    m_duration = 7 * 16;
}

void GuYingHuaShuang::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void GuYingHuaShuang::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void GuYingHuaShuang::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void GuYingHuaShuang::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void GuYingHuaShuang::TriggerClear()
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void GuYingHuaShuang::SubEffectClear()
{
    static_cast<MoWen::Skill::GuYingHuaShuang *>(m_player->skills[SKILL_GU_YING_HUA_SHUANG])
        ->Reset();
}

ZhengLvHeMing::ZhengLvHeMing(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_ZHENG_LV_HE_MING;
    m_name     = "正律和鸣";
    m_duration = 20 * 16;
}

void ZhengLvHeMing::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        static_cast<ZhiYinHeMing *>(m_player->buffs[BUFF_ZHI_YIN_HE_MING])->level = 3;
        SubEffectClear();
    }
}

void ZhengLvHeMing::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
    SubEffectAdd();
}

void ZhengLvHeMing::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void ZhengLvHeMing::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = m_duration;
    SubEffectAdd();
}

void ZhengLvHeMing::TriggerClear()
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void ZhengLvHeMing::SubEffectAdd()
{
    static_cast<ZhiYinHeMing *>(m_player->buffs[BUFF_ZHI_YIN_HE_MING])->TriggerAdd();
}

void ZhengLvHeMing::SubEffectClear()
{
    static_cast<ZhiYinHeMing *>(m_player->buffs[BUFF_ZHI_YIN_HE_MING])->TriggerClear();
}

ZhiYinHeMing::ZhiYinHeMing(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_ZHI_YIN_HE_MING;
    m_name     = "知音和鸣";
    m_duration = 20 * 16;
    m_stackNum = 5;

    m_damageParams[0].emplace_back((40 + 40 + 6) / 2, 0, 1786);
    m_damageParams[0].emplace_back((40 + 40 + 6) / 2, 0, 1453);
    m_damageParams[0].emplace_back((40 + 40 + 6) / 2, 0, 775);
}

void ZhiYinHeMing::Trigger()
{
    if (m_snapshots[PLAYER_ID].interval == 0) {
        m_snapshots[PLAYER_ID].interval = JX3DPS_INVALID_FRAMES_SET;
        m_player->SetCast(false);
    }
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots.erase(PLAYER_ID);
        level = 3;
    }
}

void ZhiYinHeMing::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_snapshots[PLAYER_ID].stackNum += stackNum;
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
    SubEffect();
}

void ZhiYinHeMing::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
    }
}

void ZhiYinHeMing::TriggerAdd()
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        randoms                         = { 2, 3, 5, 6 };
        m_snapshots[PLAYER_ID].stackNum = 0;
    } else {
        m_snapshots[PLAYER_ID].stackNum++;
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
    SubEffect();
}

void ZhiYinHeMing::TriggerClear()
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectDamage();
    }
}

void ZhiYinHeMing::SubEffect()
{
    if (m_snapshots[PLAYER_ID].stackNum == 5) {
        static_cast<ZhengLvHeMing *>(m_player->buffs[BUFF_ZHENG_LV_HE_MING])->TriggerClear();
    } else if (m_snapshots[PLAYER_ID].stackNum == 4) {
        static_cast<QuFeng *>(m_player->buffs[BUFF_QU_FENG])->TriggerSet(4);
    } else {
        // int index    = RandomUniform(0, static_cast<int>(randoms.size() - 1));
        int index    = 0;
        int stackNum = randoms[index];
        randoms.erase(randoms.begin() + index);
        static_cast<QuFeng *>(m_player->buffs[BUFF_QU_FENG])->TriggerSet(stackNum);
        if (stackNum == 5 && randoms.size() != 0) {
            lastIsGong = true;
        } else if (lastIsGong) {
            lastIsGong                      = false;
            m_snapshots[PLAYER_ID].interval = RandomUniform(8, 16);
            m_player->SetCast(true);
        }
    }
}

void ZhiYinHeMing::SubEffectDamage()
{
    static_cast<ZhiYinMiaoYi *>(m_player->buffs[BUFF_ZHI_YIN_MIAO_YI])->TriggerAdd(4 - level);
    for (int i = 0; i < 5; ++i) {
        RollResult rollResult = GetMagicRollResult();
        GainsDamage damage = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, level - 1);
        RecordDamage(m_player->GetTargetId(), rollResult, damage, 0);
    }
}

void ZhiYinHeMing::RecordDamage(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub)
{
    for (const auto &[type, damage] : gainsDamage) {
        m_stats
            .gainStats[type][targetId][BUFF_ZHI_YIN_XING_JIN][sub][level - 1][rollResult]
            .first++;
        m_stats
            .gainStats[type][targetId][BUFF_ZHI_YIN_XING_JIN][sub][level - 1][rollResult]
            .second.damage += damage.damage;
        m_stats
            .gainStats[type][targetId][BUFF_ZHI_YIN_XING_JIN][sub][level - 1][rollResult]
            .second.surplusDamage += damage.surplusDamage;
    }
}

YingZi::YingZi(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_YING_ZI;
    m_name     = "影子";
    m_duration = 25 * 16;
    m_stackNum = 6;
    m_interval = 2 * 16;

    m_damageParams[0].emplace_back((160 + 160 + 40) / 2, 0, 61);

    if (m_player->talents[TALENT_KE_MENG]) {
        m_effectCriticalStrikeAdditionalBasisPointInt   += 1000;
        m_effectCriticalStrikePowerAdditionalPercentInt += 102;
    }
}

void YingZi::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) {
            iter->second.interval = m_interval * m_player->attribute.GetHastePercent();
            SubEffect(iter->first);
        }
        if (iter->second.duration == 0) {
            iter = m_snapshots.erase(iter);
            ids.pop_front();
            Params params;
            params.player   = m_player;
            params.stackNum = 1;
            params.type     = Params::Type::CLEAR;
            m_triggerEffects[TRIGGER_YUN_HAN](params);
        } else {
            ++iter;
        }
    }
}

void YingZi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    Params params;
    params.stackNum = 1;
    params.type     = Params::Type::ADD;
    for (int i = 0; i < stackNum; ++i) {
        id = static_cast<Id_t>(id + 1);
        ids.push_back(id);
        m_snapshots[id].interval = m_interval * m_player->attribute.GetHastePercent();

        m_triggerEffects[TRIGGER_YUN_HAN](params);

        // 快照属性
        m_snapshots[id].SnapMagic(
            m_player->attribute,
            m_effectCriticalStrikeAdditionalBasisPointInt,
            m_effectCriticalStrikePowerAdditionalPercentInt +
                m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt(),
            m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

        if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_snapshots[id].duration = m_duration;
        } else [[unlikely]] {
            m_snapshots[id].duration = RandomUniform(durationMin, durationMax);
        }
    }

    int size    = m_snapshots.size() - 6;
    params.type = Params::Type::CLEAR;
    for (int i = 0; i < size; ++i) {
        m_triggerEffects[TRIGGER_YUN_HAN](params);
        m_snapshots.erase(m_snapshots.begin());
        ids.pop_front();
    }
}

void YingZi::Clear(Id_t targetId, int stackNum)
{
    Params params;
    params.stackNum = 1;
    params.type     = Params::Type::CLEAR;
    for (int i = 0; i < stackNum; ++i) {
        m_triggerEffects[TRIGGER_YUN_HAN](params);
        m_snapshots.erase(ids.front());
        ids.pop_front();
    }
}

void YingZi::TriggerAdd(int count)
{
    Params params;
    params.player   = m_player;
    params.stackNum = 1;
    params.type     = Params::Type::ADD;
    for (int i = 0; i < count; ++i) {
        id = static_cast<Id_t>(id + 1);
        ids.push_back(id);
        m_snapshots[static_cast<Id_t>(id)].interval =
            m_interval * m_player->attribute.GetHastePercent();

        m_triggerEffects[TRIGGER_YUN_HAN](params);

        // 快照属性
        m_snapshots[id].SnapMagic(
            m_player->attribute,
            m_effectCriticalStrikeAdditionalBasisPointInt,
            m_effectCriticalStrikePowerAdditionalPercentInt +
                m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt(),
            m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

        m_snapshots[id].duration = m_duration;
    }

    int size    = m_snapshots.size() - 6;
    params.type = Params::Type::CLEAR;
    for (int i = 0; i < size; ++i) {
        m_triggerEffects[TRIGGER_YUN_HAN](params);
        m_snapshots.erase(m_snapshots.begin());
        ids.pop_front();
    }
}

void YingZi::TriggerClear()
{
    Params params;
    params.player   = m_player;
    params.stackNum = 1;
    params.type     = Params::Type::CLEAR;
    m_triggerEffects[TRIGGER_YUN_HAN](params);
    m_snapshots.erase(ids.front());
    ids.pop_front();
}

void YingZi::SubEffect(Id_t id)
{
    Id_t        targetId   = m_player->GetTargetId();
    RollResult  rollResult = GetDotRollResult(id);
    GainsDamage damage     = CalcMagicYingZiDamage(id, rollResult, 0, 0, 1);
    RecordYingZi(targetId, rollResult, damage, 1, 0);
}

JX3DPS::Damage YingZi::GetMagicYingZiDamage(
    Id_t       targetId,
    RollResult rollResult,
    int        sub,
    int        level,
    int        effectCount,
    Value_t    attack,
    Value_t    weaponDamage,
    Value_t    criticalStrikePower,
    Value_t    overcome,
    Value_t    strain)
{
    Damage damage;

    PctFloat_t magicDamageCoefficient =
        MagicDamageCoefficient(m_damageParams.at(sub)[level].attackDamagePercentInt, 0);
    PctInt_t weaponDamageCoefficientInt = m_damageParams.at(sub)[level].weaponDamagePercentInt;
    Value_t fixedDamage = m_damageParams.at(sub)[level].fixedDamage;
    PctInt_t effectDamageAdditionalPercentInt = m_snapshots.at(targetId).effectDamageAdditionalPercentInt;
    Value_t effectDamage =
        effectCount *
        EffectDamageAll(attack, magicDamageCoefficient, weaponDamage, weaponDamageCoefficientInt, fixedDamage, effectDamageAdditionalPercentInt);

    int      playerLevel      = JX3_PLAYER_LEVEL;
    int      targetLevel      = (*m_targets)[m_player->GetTargetId()]->GetLevel();
    Value_t  shieldBase       = (*m_targets)[m_player->GetTargetId()]->GetMagicShield();
    Value_t  shieldAdditional = 0;
    PctInt_t ignoreShieldBasePercentInt = m_player->attribute.GetShieldIgnorePercentInt();
    PctInt_t ignoreShieldAdditionalPercentInt = m_effectShieldIgnoreAdditionalPercentInt;
    int      rollResultInt                    = static_cast<int>(rollResult);
    PctInt_t effectCriticalStrikePowerAdditionalPercentInt =
        m_snapshots.at(targetId).effectCriticalStrikePowerAdditionalPercentInt;
    PctInt_t strainPercentInt = m_snapshots.at(targetId).strainBaseAdditionalPercentInt;
    PctInt_t pveDamageAdditionalPercentInt = m_player->attribute.GetPVEDamageAdditionalPercentInt();
    PctInt_t vulnerablePercentInt =
        (*m_targets)[m_player->GetTargetId()]->GetDamageAdditionalPercentInt();

    damage.damage = FinalMagicDamage(
        playerLevel,
        targetLevel,
        effectDamage,
        shieldBase,
        shieldAdditional,
        ignoreShieldBasePercentInt,
        ignoreShieldAdditionalPercentInt,
        overcome,
        rollResultInt,
        criticalStrikePower,
        effectCriticalStrikePowerAdditionalPercentInt,
        strain,
        strainPercentInt,
        pveDamageAdditionalPercentInt,
        vulnerablePercentInt);

    return damage;
}

JX3DPS::GainsDamage YingZi::CalcMagicYingZiDamage(Id_t targetId, RollResult rollResult, int sub, int level, int effectCount)
{
    GainsDamage gainsDamage;

    std::array<Attribute::Type, 6> types = {
        {{ Attribute::Type::DEFAULT },
         { Attribute::Type::ATTACK_POWER_BASE },
         { Attribute::Type::CRITICAL_STRIKE_POWER },
         { Attribute::Type::OVERCOME_BASE },
         { Attribute::Type::STRAIN_BASE }}
    };

    for (const auto &type : types) {
        Value_t attack              = m_snapshots.at(targetId).attackPower;
        Value_t criticalStrikePower = m_snapshots.at(targetId).criticalStrikePower;
        Value_t strain              = m_snapshots.at(targetId).strainBase;
        Value_t overcome            = m_snapshots.at(targetId).overcome;
        Value_t weaponDamage        = m_player->attribute.GetWeaponDamage();

        switch (type) {
            case Attribute::Type::ATTACK_POWER_BASE:
                attack = m_snapshots.at(targetId).attackPowerGain;
                break;
            case Attribute::Type::CRITICAL_STRIKE_POWER:
                criticalStrikePower = m_snapshots.at(targetId).criticalStrikePowerGain;
                break;
            case Attribute::Type::OVERCOME_BASE:
                overcome = m_snapshots.at(targetId).overcomeGain;
                break;
            case Attribute::Type::STRAIN_BASE:
                strain = m_snapshots.at(targetId).strainBaseGain;
                break;
            default: break;
        }

        gainsDamage[type] =
            GetMagicYingZiDamage(targetId, rollResult, sub, level, effectCount, attack, weaponDamage, criticalStrikePower, overcome, strain);
    }
    gainsDamage[Attribute::Type::WEAPON_DAMAGE_BASE] = gainsDamage[Attribute::Type::DEFAULT];
    gainsDamage[Attribute::Type::SURPLUS_VALUE_BASE] = gainsDamage[Attribute::Type::DEFAULT];

    return gainsDamage;
}

void YingZi::RecordYingZi(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub, int level)
{
    for (const auto &[type, damage] : gainsDamage) {
        m_stats.gainStats[type][targetId][SKILL_GONG][sub][level][rollResult].first++;
        m_stats.gainStats[type][targetId][SKILL_GONG][sub][level][rollResult].second.damage +=
            damage.damage;
        m_stats.gainStats[type][targetId][SKILL_GONG][sub][level][rollResult].second.surplusDamage +=
            damage.surplusDamage;
    }
}

SetAttribute::SetAttribute(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_SET_ATTRIBUTE;
    m_name     = "套装·挥散";
    m_duration = 16 * 6;
}

void SetAttribute::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void SetAttribute::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void SetAttribute::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void SetAttribute::TriggerAdd()
{
    if (RandomUniform(1, 100) <= 10) {
        if (m_snapshots.empty()) {
            SubEffectAdd();
        }
        m_snapshots[PLAYER_ID].duration = m_duration;
    }
}

void SetAttribute::SubEffectAdd()
{
    m_player->attribute.AddMagicCriticalStrikeAdditionalBasisPointInt(400);
    m_player->attribute.AddMagicCriticalStrikePowerAdditionalPercentInt(41);
}

void SetAttribute::SubEffectClear()
{
    m_player->attribute.AddMagicCriticalStrikeAdditionalBasisPointInt(-400);
    m_player->attribute.AddMagicCriticalStrikePowerAdditionalPercentInt(-41);
}

WeaponEffectCW::WeaponEffectCW(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_WEAPON_EFFECT_CW;
    m_name            = "武器·橙武特效";
    m_duration        = 16 * 6;
    m_cooldownCurrent = m_cooldown = 16 * 30;
}

void WeaponEffectCW::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration != 0) {
        return;
    }
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void WeaponEffectCW::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.empty()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void WeaponEffectCW::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void WeaponEffectCW::TriggerAdd()
{
    if (m_cooldownCurrent <= 0) {
        m_snapshots[PLAYER_ID].duration = m_duration;
        m_cooldownCurrent               = m_cooldown;
        SubEffectAdd();
    }
}

void WeaponEffectCW::SubEffectAdd()
{
    static_cast<MoWen::Skill::Zhi *>(m_player->skills[SKILL_ZHI])->SetEnergyCooldownCurrent(0);
    static_cast<MoWen::Skill::BianZhi *>(m_player->skills[SKILL_BIAN_ZHI])->SetEnergyCooldownCurrent(0);
    static_cast<MoWen::Skill::Yu *>(m_player->skills[SKILL_YU])->SetEnergyCooldownCurrent(0);
    static_cast<MoWen::Skill::Gong *>(m_player->skills[SKILL_GONG])->ClearPrepareFrames();
    static_cast<MoWen::Skill::BianGong *>(m_player->skills[SKILL_BIAN_GONG])->ClearPrepareFrames();
}

void WeaponEffectCW::SubEffectClear()
{
    static_cast<MoWen::Skill::Gong *>(m_player->skills[SKILL_GONG])->ResetPrepareFrames();
    static_cast<MoWen::Skill::BianGong *>(m_player->skills[SKILL_BIAN_GONG])->ResetPrepareFrames();
}

ShenBingGong::ShenBingGong(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id          = BUFF_SHEN_BING_GONG;
    m_name        = "神兵·宫";
    m_interval    = 16 * 3;
    m_stackNum    = 3;
    m_effectCount = 10;

    m_damageParams[0].emplace_back(0, 0, 618 * 1);
    m_damageParams[0].emplace_back(0, 0, 618 * 2);
    m_damageParams[0].emplace_back(0, 0, 618 * 3);
}

void ShenBingGong::Trigger()
{
    for (auto iter = m_snapshots.begin(); iter != m_snapshots.end();) {
        if (iter->second.interval == 0) {                      // 叠刃生效一次
            int level             = iter->second.stackNum - 1; // 层数
            iter->second.interval = m_interval * iter->second.hastePercent;
            SubEffect(iter->first, level);
        }
        if (iter->second.duration == 0) { // 叠刃消失
            iter = m_snapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void ShenBingGong::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在叠刃
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].stackNum += stackNum;
    m_snapshots[targetId].stackNum = std::min(m_snapshots[targetId].stackNum, m_stackNum);

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[targetId].duration =
            m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
            m_snapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
    } else [[unlikely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void ShenBingGong::Clear(Id_t targetId, int stackNum)
{
    m_snapshots[targetId].stackNum -= stackNum;
    int stack                       = m_snapshots[targetId].stackNum;
    if (stack <= 0) [[likely]] { // 叠刃消失
        m_snapshots.erase(targetId);
    }
}

void ShenBingGong::TriggerAdd(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) { // 不存在叠刃
        m_snapshots[targetId].interval = m_interval * m_player->attribute.GetHastePercent();
    }
    m_snapshots[targetId].stackNum += stackNum;
    m_snapshots[targetId].stackNum = std::min(m_snapshots[targetId].stackNum, m_stackNum);

    // 快照属性
    m_snapshots[targetId].SnapMagic(
        m_player->attribute,
        m_effectCriticalStrikeAdditionalBasisPointInt,
        m_effectCriticalStrikePowerAdditionalPercentInt +
            m_player->attribute.GetMagicCriticalStrikePowerAdditionalPercentInt(),
        m_effectDamageAdditionalPercentInt + m_player->effectDamageAdditionalPercentInt);

    m_snapshots[targetId].duration =
        m_interval * m_player->attribute.GetHastePercent() * (m_effectCount - 1) +
        m_snapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
}

void ShenBingGong::SubEffect(Id_t targetId, int stackNum)
{
    RollResult  rollResult = GetDotRollResult(targetId);
    GainsDamage damage     = CalcMagicDotDamage(targetId, rollResult, 0, stackNum, 1);
    Record(m_id, targetId, rollResult, damage, 0, stackNum);
}

} // namespace Buff

} // namespace MoWen

} // namespace JX3DPS