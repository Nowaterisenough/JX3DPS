/**
 * Project: JX3DPS
 * File: Buff3rd.CPP
 * Description:
 * Created Date: 2023-08-19 12:41:54
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-20 16:07:24
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Buff3rd.h"

#include "Player.h"

namespace JX3DPS {

namespace Buff3rd {

EnchantShoesPhysics::EnchantShoesPhysics(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_ENCHANT_SHOES;
    m_name            = "大附魔·鞋";
    m_cooldown        = 16 * 10;
    m_cooldownCurrent = 0;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 60);
}

void EnchantShoesPhysics::Trigger() { }

void EnchantShoesPhysics::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_cooldownCurrent = m_cooldown;
    SubEffect();
}

void EnchantShoesPhysics::Clear(Id_t targetId, int stackNum) { }

void EnchantShoesPhysics::TriggerDamage()
{
    if (m_cooldownCurrent <= 0) {
        m_cooldownCurrent = m_cooldown;
        SubEffect();
    }
}

void EnchantShoesPhysics::SubEffect()
{
    RollResult rollResult = GetPhysicsRollResult();
    GainsDamage gainsDamage = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, gainsDamage, 0, 0);
}

EnchantWristPhysics::EnchantWristPhysics(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_ENCHANT_WRIST;
    m_name            = "大附魔·腕";
    m_cooldown        = 16 * 10;
    m_cooldownCurrent = 0;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 75);
}

void EnchantWristPhysics::Trigger() { }

void EnchantWristPhysics::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_cooldownCurrent = m_cooldown;
    SubEffect();
}

void EnchantWristPhysics::Clear(Id_t targetId, int stackNum) { }

void EnchantWristPhysics::TriggerDamage()
{
    if (m_cooldownCurrent <= 0 && RandomUniform(1, 100) <= 10) {
        m_cooldownCurrent = m_cooldown;
        SubEffect();
    }
}

void EnchantWristPhysics::SubEffect()
{
    RollResult rollResult = GetPhysicsRollResult();
    GainsDamage gainsDamage = CalcPhysicsDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, gainsDamage, 0, 0);
}

EnchantShoesMagic::EnchantShoesMagic(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_ENCHANT_SHOES;
    m_name            = "大附魔·鞋";
    m_cooldown        = 16 * 10;
    m_cooldownCurrent = 0;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 60);
}

void EnchantShoesMagic::Trigger() { }

void EnchantShoesMagic::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_cooldownCurrent = m_cooldown;
    SubEffect();
}

void EnchantShoesMagic::Clear(Id_t targetId, int stackNum) { }

void EnchantShoesMagic::TriggerDamage()
{
    if (m_cooldownCurrent <= 0) {
        m_cooldownCurrent = m_cooldown;
        SubEffect();
    }
}

void EnchantShoesMagic::SubEffect()
{
    RollResult  rollResult  = GetMagicRollResult();
    GainsDamage gainsDamage = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, gainsDamage, 0, 0);
}

EnchantWristMagic::EnchantWristMagic(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_ENCHANT_WRIST;
    m_name            = "大附魔·腕";
    m_cooldown        = 16 * 10;
    m_cooldownCurrent = 0;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 75);
}

void EnchantWristMagic::Trigger() { }

void EnchantWristMagic::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    m_cooldownCurrent = m_cooldown;
    SubEffect();
}

void EnchantWristMagic::Clear(Id_t targetId, int stackNum) { }

void EnchantWristMagic::TriggerDamage()
{
    if (m_cooldownCurrent <= 0 && RandomUniform(1, 100) <= 10) {
        m_cooldownCurrent = m_cooldown;
        SubEffect();
    }
}

void EnchantWristMagic::SubEffect()
{
    RollResult  rollResult  = GetMagicRollResult();
    GainsDamage gainsDamage = CalcMagicDamage(m_player->GetTargetId(), rollResult, 0, 0);
    Record(m_player->GetTargetId(), rollResult, gainsDamage, 0, 0);
}

EnchantBelt::EnchantBelt(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_ENCHANT_BELT;
    m_name            = "大附魔·腰";
    m_cooldown        = 16 * 30;
    m_duration        = 16 * 8;
    m_cooldownCurrent = 0;
}

void EnchantBelt::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) { // buff时间结束
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void EnchantBelt::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_cooldownCurrent = m_cooldown;
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void EnchantBelt::Clear(Id_t targetId, int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) != m_snapshots.end()) {
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear();
    }
}

void EnchantBelt::TriggerAdd()
{
    if (m_cooldownCurrent <= 0 && RandomUniform(1, 100) <= 20) {
        m_cooldownCurrent = m_cooldown;
        if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
            SubEffectAdd();
        }
        m_snapshots[PLAYER_ID].duration = m_duration;
    }
}

void EnchantBelt::SubEffectAdd()
{
    if (RandomUniform(1, 100) <= 70) {
        m_70                                        = true;
        m_player->effectDamageAdditionalPercentInt += 51;
    } else {
        m_player->effectDamageAdditionalPercentInt += 10;
    }
}

void EnchantBelt::SubEffectClear()
{
    if (m_70 == true) {
        m_player->effectDamageAdditionalPercentInt -= 51;
    } else {
        m_player->effectDamageAdditionalPercentInt -= 10;
    }
    m_70 = false;
}

JX3DPS::Buff3rd::WeaponEffectWater::WeaponEffectWater(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_WEAPON_EFFECT_WATER;
    m_name     = "水·斩流";
    m_duration = 16 * 6;
    m_stackNum = 10;
}

void JX3DPS::Buff3rd::WeaponEffectWater::Trigger()
{
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(stackNum);
}

void JX3DPS::Buff3rd::WeaponEffectWater::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        m_snapshots[PLAYER_ID].stackNum =
            std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        SubEffectAdd(m_snapshots[PLAYER_ID].stackNum);
    } else {
        int stack = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum += stackNum;
        m_snapshots[PLAYER_ID].stackNum =
            std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        stack = m_snapshots[PLAYER_ID].stackNum - stack;
        SubEffectAdd(stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JX3DPS::Buff3rd::WeaponEffectWater::Clear(Id_t targetId, int stackNum)
{
    int stack = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(stack);
}

void JX3DPS::Buff3rd::WeaponEffectWater::TriggerAdd(int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        SubEffectAdd(m_snapshots[PLAYER_ID].stackNum);
    } else {
        int stack = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum += stackNum;
        m_snapshots[PLAYER_ID].stackNum =
            std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        stack = m_snapshots[PLAYER_ID].stackNum - stack;
        SubEffectAdd(stack);
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void JX3DPS::Buff3rd::WeaponEffectWater::SubEffectAdd(int stackNum)
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditional(88 * stackNum);
}

void JX3DPS::Buff3rd::WeaponEffectWater::SubEffectClear(int stackNum)
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditional(-88 * stackNum);
}

} // namespace Buff3rd

} // namespace JX3DPS