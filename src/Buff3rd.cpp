/**
 * Project: JX3DPS
 * File: Buff3rd.CPP
 * Description:
 * Created Date: 2023-08-19 12:41:54
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-17 14:39:05
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Buff3rd.h"

#include "Player.h"
#include "Target.hpp"

namespace JX3DPS {

namespace Buff3rd {

EnchantShoesPhysics::EnchantShoesPhysics(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_ENCHANT_SHOES;
    m_name            = "大附魔·鞋";
    m_cooldown        = 16 * 10;
    m_cooldownCurrent = 0;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 50);
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
    Record(m_id, m_player->GetTargetId(), rollResult, gainsDamage, 0, 0);
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
    Record(m_id, m_player->GetTargetId(), rollResult, gainsDamage, 0, 0);
}

EnchantShoesMagic::EnchantShoesMagic(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_ENCHANT_SHOES;
    m_name            = "大附魔·鞋";
    m_cooldown        = 16 * 10;
    m_cooldownCurrent = 0;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 100);
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
    Record(m_id, m_player->GetTargetId(), rollResult, gainsDamage, 0, 0);
}

EnchantWristMagic::EnchantWristMagic(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id              = BUFF_ENCHANT_WRIST;
    m_name            = "大附魔·腕";
    m_cooldown        = 16 * 10;
    m_cooldownCurrent = 0;

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 90);
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
    Record(m_id, m_player->GetTargetId(), rollResult, gainsDamage, 0, 0);
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
        m_70 = true;
        m_player->attribute.AddDamageAdditionalPercentInt(51);
    } else {
        m_player->attribute.AddDamageAdditionalPercentInt(10);
    }
}

void EnchantBelt::SubEffectClear()
{
    if (m_70 == true) {
        m_player->attribute.AddDamageAdditionalPercentInt(-51);
    } else {
        m_player->attribute.AddDamageAdditionalPercentInt(-10);
    }
    m_70 = false;
}

WeaponEffectWater::WeaponEffectWater(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_WEAPON_EFFECT_WATER;
    m_name     = "水·斩流";
    m_duration = 16 * 6;
    m_stackNum = 10;
}

void WeaponEffectWater::Trigger()
{
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(stackNum);
}

void WeaponEffectWater::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        SubEffectAdd(m_snapshots[PLAYER_ID].stackNum);
    } else {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum += stackNum;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        stack = m_snapshots[PLAYER_ID].stackNum - stack;
        SubEffectAdd(stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void WeaponEffectWater::Clear(Id_t targetId, int stackNum)
{
    int stack = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(stack);
}

void WeaponEffectWater::TriggerAdd(int stackNum)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        SubEffectAdd(m_snapshots[PLAYER_ID].stackNum);
    } else {
        int stack                        = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum += stackNum;
        m_snapshots[PLAYER_ID].stackNum = std::min(m_snapshots[PLAYER_ID].stackNum, m_stackNum);
        stack = m_snapshots[PLAYER_ID].stackNum - stack;
        SubEffectAdd(stack);
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void WeaponEffectWater::SubEffectAdd(int stackNum)
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditional(88 * stackNum);
    m_player->attribute.AddMagicAttackPowerBaseAdditional(88 * stackNum);
}

void WeaponEffectWater::SubEffectClear(int stackNum)
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditional(-88 * stackNum);
    m_player->attribute.AddMagicAttackPowerBaseAdditional(-88 * stackNum);
}

JiaoSu::JiaoSu(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_ENCHANT_BELT;
    m_name     = "3rd·皎素";
    m_interval = 16 * 30;
    m_duration = 16 * 5;
}

void JiaoSu::Trigger()
{
    m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
    SubEffectClear();
}

void JiaoSu::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots[PLAYER_ID].duration == JX3DPS_INVALID_FRAMES_SET) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JiaoSu::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void JiaoSu::SubEffectAdd()
{
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(51);
}

void JiaoSu::SubEffectClear()
{
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(-51);
}

HanRuLei::HanRuLei(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_HAN_RU_LEI;
    m_name     = "3rd·撼如雷";
    m_duration = 16 * 60 * 30;
}

void HanRuLei::Trigger()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void HanRuLei::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
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

void HanRuLei::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void HanRuLei::SubEffectAdd()
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(51);
}

void HanRuLei::SubEffectClear()
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(-51);
}

XiuQi::XiuQi(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_XIU_QI;
    m_name     = "3rd·袖气";
    m_duration = 16 * 60 * 30;
}

void XiuQi::Trigger()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void XiuQi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
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

void XiuQi::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void XiuQi::SubEffectAdd()
{
    m_player->attribute.AddAgilityBaseAdditional(244);
    m_player->attribute.AddSpiritBaseAdditional(244);
    m_player->attribute.AddStrengthBaseAdditional(244);
    m_player->attribute.AddSpunkBaseAdditional(244);
}

void XiuQi::SubEffectClear()
{
    m_player->attribute.AddAgilityBaseAdditional(-244);
    m_player->attribute.AddSpiritBaseAdditional(-244);
    m_player->attribute.AddStrengthBaseAdditional(-244);
    m_player->attribute.AddSpunkBaseAdditional(-244);
}

PoFeng::PoFeng(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_PO_FENG;
    m_name     = "3rd·破风";
    m_duration = 16 * 14;
}

void PoFeng::Trigger()
{
    for (auto it = m_snapshots.begin(); it != m_snapshots.end();) {
        if (it->second.duration == 0) {

            SubEffectClear(it->first);
            it = m_snapshots.erase(it);
        } else {
            ++it;
        }
    }
}

void PoFeng::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) {
        SubEffectAdd(targetId);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[unlikely]] {
        m_snapshots[targetId].duration = m_duration;
    } else [[likely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void PoFeng::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(targetId);
    SubEffectClear(targetId);
}

void PoFeng::SubEffectAdd(Id_t targetId)
{
    (*m_targets)[targetId]->AddPhysicsShield(-1150);
}

void PoFeng::SubEffectClear(Id_t targetId)
{
    (*m_targets)[targetId]->AddPhysicsShield(1150);
}

JingFeng::JingFeng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_JING_FENG;
    m_name     = "3rd·劲风";
    m_duration = 16 * 14;
}

void JingFeng::Trigger()
{
    for (auto it = m_snapshots.begin(); it != m_snapshots.end();) {
        if (it->second.duration == 0) {
            SubEffectClear(it->first);
            it = m_snapshots.erase(it);
        } else {
            ++it;
        }
    }
}

void JingFeng::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) {
        SubEffectAdd(targetId);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[unlikely]] {
        m_snapshots[targetId].duration = m_duration;
    } else [[likely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void JingFeng::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(targetId);
    SubEffectClear(targetId);
}

void JingFeng::SubEffectAdd(Id_t targetId)
{
    (*m_targets)[targetId]->AddPhysicsShield(-1397);
}

void JingFeng::SubEffectClear(Id_t targetId)
{
    (*m_targets)[targetId]->AddPhysicsShield(1397);
}

JieHuo::JieHuo(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_JIE_HUO;
    m_name     = "3rd·戒火";
    m_duration = 16 * 15;
}

void JieHuo::Trigger()
{
    for (auto it = m_snapshots.begin(); it != m_snapshots.end();) {
        if (it->second.duration == 0) {
            SubEffectClear(it->first);
            it = m_snapshots.erase(it);

        } else {
            ++it;
        }
    }
}

void JieHuo::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) {
        SubEffectAdd(targetId);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[unlikely]] {
        m_snapshots[targetId].duration = m_duration;
    } else [[likely]] {
        m_snapshots[targetId].duration = RandomUniform(durationMin, durationMax);
    }
}

void JieHuo::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(targetId);
    SubEffectClear(targetId);
}

void JieHuo::SubEffectAdd(Id_t targetId)
{
    (*m_targets)[targetId]->AddDamageAdditionalPercentInt(20);
}

void JieHuo::SubEffectClear(Id_t targetId)
{
    (*m_targets)[targetId]->AddDamageAdditionalPercentInt(-20);
}

HaoLingSanJun::HaoLingSanJun(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_HAO_LING_SAN_JUN;
    m_name     = "3rd·号令三军";
    m_duration = 16 * 60;
    m_interval = 16 * 30;
    m_stackNum = 48;
}

void HaoLingSanJun::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) { // 消失
        int stackNum = m_snapshots[PLAYER_ID].stackNum;
        m_snapshots.erase(PLAYER_ID);
        SubEffectClear(PLAYER_ID, stackNum);
    } else { // 一鼓结束，二鼓开始
        m_snapshots[PLAYER_ID].interval  = m_interval;
        int stackNum                     = m_snapshots[PLAYER_ID].stackNum / 2;
        m_snapshots[PLAYER_ID].stackNum -= stackNum;
        SubEffectClear(PLAYER_ID, m_snapshots[PLAYER_ID].stackNum);
    }
}

void HaoLingSanJun::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum = std::min(stackNum, m_stackNum);
        m_snapshots[PLAYER_ID].duration = m_duration;
        m_snapshots[PLAYER_ID].interval = m_interval;
        SubEffectAdd(PLAYER_ID);
    }
}

void HaoLingSanJun::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_snapshots[PLAYER_ID].stackNum);
    m_snapshots[PLAYER_ID].stackNum -= stackNum;
    if (m_snapshots[PLAYER_ID].stackNum == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
    SubEffectClear(PLAYER_ID, stackNum);
}

void HaoLingSanJun::SubEffectAdd(Id_t targetId)
{
    m_player->attribute.AddStrainBase(m_snapshots[PLAYER_ID].stackNum * 440);
}

void HaoLingSanJun::SubEffectClear(Id_t targetId, int stackNum)
{
    m_player->attribute.AddStrainBase(-stackNum * 440);
}

SheShenHongFa::SheShenHongFa(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_SHE_SHEN_HONG_FA;
    m_name     = "3rd·舍身弘法";
    m_duration = 16 * 20;
    m_stackNum = 36;
}

void SheShenHongFa::Trigger()
{
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID, stackNum);
}

void SheShenHongFa::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum = std::min(stackNum, m_stackNum);
        SubEffectAdd(PLAYER_ID, m_snapshots[PLAYER_ID].stackNum);
    } else { // 已有buff,刷新层数
        stackNum                        = std::min(stackNum, m_stackNum);
        int stack                       = stackNum - m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        SubEffectAdd(PLAYER_ID, stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void SheShenHongFa::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_snapshots[PLAYER_ID].stackNum);
    m_snapshots[PLAYER_ID].stackNum -= stackNum;
    if (m_snapshots[PLAYER_ID].stackNum == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
    SubEffectClear(PLAYER_ID, stackNum);
}

void SheShenHongFa::SubEffectAdd(Id_t targetId, int stackNum)
{
    m_player->attribute.AddStrainBase(stackNum * 400);
}

void SheShenHongFa::SubEffectClear(Id_t targetId, int stackNum)
{
    m_player->attribute.AddStrainBase(-stackNum * 400);
}

ChaoSheng::ChaoSheng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_CHAO_SHENG;
    m_name     = "3rd·朝圣";
    m_duration = 16 * 8;
    m_stackNum = 24;
}

void ChaoSheng::Trigger()
{
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID, stackNum);
}

void ChaoSheng::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum = std::min(stackNum, m_stackNum);
        SubEffectAdd(PLAYER_ID, m_snapshots[PLAYER_ID].stackNum);
    } else { // 已有buff,刷新层数
        stackNum                        = std::min(stackNum, m_stackNum);
        int stack                       = stackNum - m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        SubEffectAdd(PLAYER_ID, stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void ChaoSheng::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_snapshots[PLAYER_ID].stackNum);
    m_snapshots[PLAYER_ID].stackNum -= stackNum;
    if (m_snapshots[PLAYER_ID].stackNum == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
    SubEffectClear(PLAYER_ID, stackNum);
}

void ChaoSheng::SubEffectAdd(Id_t targetId, int stackNum)
{
    m_player->attribute.AddStrainBase(stackNum * 400);
}

void ChaoSheng::SubEffectClear(Id_t targetId, int stackNum)
{
    m_player->attribute.AddStrainBase(-stackNum * 400);
}

ShengYuMingXin::ShengYuMingXin(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_SHENG_YU_MING_XIN;
    m_name     = "3rd·圣浴明心";
    m_duration = 16 * 8;
    m_stackNum = 24;
}

void ShengYuMingXin::Trigger()
{
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID, stackNum);
}

void ShengYuMingXin::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum = std::min(stackNum, m_stackNum);
        SubEffectAdd(PLAYER_ID, m_snapshots[PLAYER_ID].stackNum);
    } else { // 已有buff,刷新层数
        stackNum                        = std::min(stackNum, m_stackNum);
        int stack                       = stackNum - m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        SubEffectAdd(PLAYER_ID, stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void ShengYuMingXin::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_snapshots[PLAYER_ID].stackNum);
    m_snapshots[PLAYER_ID].stackNum -= stackNum;
    if (m_snapshots[PLAYER_ID].stackNum == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
    SubEffectClear(PLAYER_ID, stackNum);
}

void ShengYuMingXin::SubEffectAdd(Id_t targetId, int stackNum)
{
    m_player->attribute.AddStrainBase(stackNum * 600);
}

void ShengYuMingXin::SubEffectClear(Id_t targetId, int stackNum)
{
    m_player->attribute.AddStrainBase(-stackNum * 600);
}

ZhenFen::ZhenFen(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_ZHEN_FEN;
    m_name     = "3rd·振奋";
    m_duration = 16 * 10;
    m_stackNum = 125;
}

void ZhenFen::Trigger()
{
    int stackNum = m_snapshots[PLAYER_ID].stackNum;
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID, stackNum);
}

void ZhenFen::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        m_snapshots[PLAYER_ID].stackNum = std::min(stackNum, m_stackNum);
        SubEffectAdd(PLAYER_ID, m_snapshots[PLAYER_ID].stackNum);
    } else { // 已有buff,刷新层数
        stackNum                        = std::min(stackNum, m_stackNum);
        int stack                       = stackNum - m_snapshots[PLAYER_ID].stackNum;
        m_snapshots[PLAYER_ID].stackNum = stackNum;
        SubEffectAdd(PLAYER_ID, stack);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void ZhenFen::Clear(Id_t targetId, int stackNum)
{
    stackNum = std::min(stackNum, m_snapshots[PLAYER_ID].stackNum);
    m_snapshots[PLAYER_ID].stackNum -= stackNum;
    if (m_snapshots[PLAYER_ID].stackNum == 0) {
        m_snapshots.erase(PLAYER_ID);
    }
    SubEffectClear(PLAYER_ID, stackNum);
}

void ZhenFen::SubEffectAdd(Id_t targetId, int stackNum)
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditional(stackNum * 60);
    m_player->attribute.AddMagicOvercomeBaseAdditional(stackNum * 60);
}

void ZhenFen::SubEffectClear(Id_t targetId, int stackNum)
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditional(-stackNum * 60);
    m_player->attribute.AddMagicOvercomeBaseAdditional(-stackNum * 60);
}

HanXiaoQianJun::HanXiaoQianJun(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_HAN_XIAO_QIAN_JUN;
    m_name     = "3rd·寒啸千军";
    m_duration = 16 * 15;
}

void HanXiaoQianJun::Trigger()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID);
}

void HanXiaoQianJun::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd(PLAYER_ID);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else [[unlikely]] {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void HanXiaoQianJun::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID);
}

void HanXiaoQianJun::SubEffectAdd(Id_t targetId)
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditionalPercentInt(256);
    m_player->attribute.AddMagicOvercomeBaseAdditionalPercentInt(256);
}

void HanXiaoQianJun::SubEffectClear(Id_t targetId)
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditionalPercentInt(-256);
    m_player->attribute.AddMagicOvercomeBaseAdditionalPercentInt(-256);
}

ShuKuang::ShuKuang(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_SHU_KUANG;
    m_name     = "3rd·疏狂";
    m_duration = 16 * 12;
}

void ShuKuang::Trigger()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID);
}

void ShuKuang::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd(PLAYER_ID);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void ShuKuang::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID);
}

void ShuKuang::SubEffectAdd(Id_t targetId)
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(307);
}

void ShuKuang::SubEffectClear(Id_t targetId)
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(-307);
}

HanChangLinLi::HanChangLinLi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_HAN_CHANG_LIN_LI;
    m_name     = "3rd·酣畅淋漓";
    m_duration = 16 * 5;
}

void HanChangLinLi::Trigger()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID);
}

void HanChangLinLi::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd(PLAYER_ID);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void HanChangLinLi::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID);
}

void HanChangLinLi::SubEffectAdd(Id_t targetId)
{
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(1000);
}

void HanChangLinLi::SubEffectClear(Id_t targetId)
{
    m_player->attribute.AddPhysicsCriticalStrikeAdditionalBasisPointInt(-1000);
}

JiLei::JiLei(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_JI_LEI;
    m_name     = "3rd·激雷";
    m_duration = 16 * 6;
}

void JiLei::Trigger()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID);
}

void JiLei::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd(PLAYER_ID);
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void JiLei::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear(PLAYER_ID);
}

void JiLei::SubEffectAdd(Id_t targetId)
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditionalPercentInt(205);
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(205);
}

void JiLei::SubEffectClear(Id_t targetId)
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditionalPercentInt(-205);
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(-205);
}

NongMei::NongMei(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_3RD_NONG_MEI;
    m_name     = "3rd·弄梅";
    m_duration = 16 * 6;
}

void NongMei::Trigger()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void NongMei::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    if (durationMin == JX3DPS_DEFAULT_DURATION_FRAMES) {
        m_snapshots[PLAYER_ID].duration = m_duration;
    } else {
        m_snapshots[PLAYER_ID].duration = RandomUniform(durationMin, durationMax);
    }
}

void NongMei::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void NongMei::SubEffectAdd()
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditional(700);
    m_player->attribute.AddMagicOvercomeBaseAdditional(700);
    m_player->attribute.AddPhysicsShieldIgnorePercentInt(205);
    m_player->attribute.AddMagicShieldIgnorePercentInt(205);
}

void NongMei::SubEffectClear()
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditional(-700);
    m_player->attribute.AddMagicOvercomeBaseAdditional(-700);
    m_player->attribute.AddPhysicsShieldIgnorePercentInt(-205);
    m_player->attribute.AddMagicShieldIgnorePercentInt(-205);
}

PendantOvercome::PendantOvercome(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_PENDANT_OVERCOME;
    m_name     = "腰坠·破防";
    m_duration = 16 * 15;
}

void PendantOvercome::Trigger()
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void PendantOvercome::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
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

void PendantOvercome::Clear(Id_t targetId, int stackNum)
{
    m_snapshots.erase(PLAYER_ID);
    SubEffectClear();
}

void PendantOvercome::TriggerAdd()
{
    if (m_snapshots.find(PLAYER_ID) == m_snapshots.end()) {
        SubEffectAdd();
    }
    m_snapshots[PLAYER_ID].duration = m_duration;
}

void PendantOvercome::SubEffectAdd()
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditional(8330);
    m_player->attribute.AddMagicOvercomeBaseAdditional(8330);
}

void PendantOvercome::SubEffectClear()
{
    m_player->attribute.AddPhysicsOvercomeBaseAdditional(-8330);
    m_player->attribute.AddMagicOvercomeBaseAdditional(-8330);
}

TeamCoreShanHaiXinJueYouRen::TeamCoreShanHaiXinJueYouRen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_PENDANT_OVERCOME;
    m_name     = "苍梧引灵阵·游刃";
    m_duration = 6 * 16;
}

void TeamCoreShanHaiXinJueYouRen::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
        SubEffectClear();
    }
    if (m_snapshots[PLAYER_ID].interval == 0) {
        m_snapshots[PLAYER_ID].interval = RandomUniform(96, 170);
        m_snapshots[PLAYER_ID].duration = m_duration;
        SubEffectAdd();
    }
}

void TeamCoreShanHaiXinJueYouRen::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
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

void TeamCoreShanHaiXinJueYouRen::Clear(Id_t targetId, int stackNum)
{
    SubEffectClear();
}

void TeamCoreShanHaiXinJueYouRen::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
    m_snapshots[PLAYER_ID].interval = RandomUniform(80, 90);
}

void TeamCoreShanHaiXinJueYouRen::SubEffectAdd()
{
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(150);
    m_player->attribute.AddMagicCriticalStrikePowerAdditionalPercentInt(150);
}

void TeamCoreShanHaiXinJueYouRen::SubEffectClear()
{
    m_player->attribute.AddPhysicsCriticalStrikePowerAdditionalPercentInt(-150);
    m_player->attribute.AddMagicCriticalStrikePowerAdditionalPercentInt(-150);
}

TeamCoreYinLongJueYouRen::TeamCoreYinLongJueYouRen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id       = BUFF_PENDANT_OVERCOME;
    m_name     = "龙皇雪风阵·游刃";
    m_duration = 5 * 16;
}

void TeamCoreYinLongJueYouRen::Trigger()
{
    if (m_snapshots[PLAYER_ID].duration == 0) {
        m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
        SubEffectClear();
    }
    if (m_snapshots[PLAYER_ID].interval == 0) {
        m_snapshots[PLAYER_ID].interval = RandomUniform(140, 160);
        m_snapshots[PLAYER_ID].duration = m_duration;
        SubEffectAdd();
    }
}

void TeamCoreYinLongJueYouRen::Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax)
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

void TeamCoreYinLongJueYouRen::Clear(Id_t targetId, int stackNum)
{
    SubEffectClear();
}

void TeamCoreYinLongJueYouRen::TriggerAdd()
{
    m_snapshots[PLAYER_ID].duration = JX3DPS_INVALID_FRAMES_SET;
    m_snapshots[PLAYER_ID].interval = RandomUniform(90, 102);
}

void TeamCoreYinLongJueYouRen::SubEffectAdd()
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(102);
}

void TeamCoreYinLongJueYouRen::SubEffectClear()
{
    m_player->attribute.AddPhysicsAttackPowerBaseAdditionalPercentInt(-102);
}

} // namespace Buff3rd

} // namespace JX3DPS