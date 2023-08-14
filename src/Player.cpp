/**
 * Project: JX3DPS
 * File: Player.cpp
 * Description:
 * Created Date: 2023-07-20 02:39:38
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-14 09:52:00
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Player.h"

#include <unordered_set>

#include "Class/MoWen/MoWen.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

#include "Buff.h"
#include "Skill.h"

namespace JX3DPS {

JX3DPS::Player::Player() { }

Player *Player::PlayerFactoryGenerate(ClassType classType)
{
    switch (classType) {
        case ClassType::TAI_XU_JIAN_YI: return new TaiXuJianYi::Player();
        case ClassType::ZI_XIA_GONG: return nullptr;
        case ClassType::FEN_SHAN_JING: return nullptr;
        case ClassType::MO_WEN: return new MoWen::Player();
        default: return nullptr;
    }
    return nullptr;
}

Player::~Player()
{
    std::unordered_set<Skill *> skillPtrs;
    for (auto &[id, skill] : this->skills) {
        skillPtrs.emplace(skill);
    }
    for (auto &ptr : skillPtrs) {
        delete ptr;
    }
    std::unordered_set<Buff *> buffPtrs;
    for (auto &[id, buff] : this->buffs) {
        buffPtrs.emplace(buff);
    }
    for (auto &ptr : buffPtrs) {
        delete ptr;
    }
}

JX3DPS::Player::Player(const Player &other)
{
    m_qidian      = other.m_qidian;
    m_qidianLimit = other.m_qidianLimit;
    m_rage        = other.m_rage;
    m_rageLimit   = other.m_rageLimit;
    m_energy      = other.m_energy;
    m_energyLimit = other.m_energyLimit;
    m_sun         = other.m_sun;
    m_sunLimit    = other.m_sunLimit;
    m_moon        = other.m_moon;
    m_moonLimit   = other.m_moonLimit;

    m_lifePercent = other.m_lifePercent;
    m_manaPercent = other.m_manaPercent;

    m_cast   = other.m_cast;
    m_reCast = other.m_reCast;
    m_stop   = other.m_stop;

    m_targetId      = other.m_targetId;
    m_lastCastSkill = other.m_lastCastSkill;

    globalCooldown        = other.globalCooldown;
    globalCooldownCurrent = other.globalCooldownCurrent;

    effectDamageAdditionalPercentInt = other.effectDamageAdditionalPercentInt;

    attribute = other.attribute;

    triggerEffects = other.triggerEffects;

    for (auto &[id, buff] : other.buffs) {
        Buff *b = buff->Clone();
        b->SetPlayer(this);
        buffs.emplace(id, b);
    }
    for (auto &[id, skill] : other.skills) {
        Skill *s = skill->Clone();
        s->SetPlayer(this);
        skills.emplace(id, s);
    }
    for (auto &[id, talent] : other.talents) {
        talents.emplace(id, talent);
    }
    for (auto &[id, secret] : other.recipes) {
        recipes.emplace(id, secret);
    }
    for (auto &[id, equipEffect] : other.equipEffects) {
        recipes.emplace(id, equipEffect);
    }
}

JX3DPS::Player &JX3DPS::Player::operator=(const Player &other)
{
    if (this == &other) {
        return *this;
    }

    m_qidian      = other.m_qidian;
    m_qidianLimit = other.m_qidianLimit;
    m_rage        = other.m_rage;
    m_rageLimit   = other.m_rageLimit;
    m_energy      = other.m_energy;
    m_energyLimit = other.m_energyLimit;
    m_sun         = other.m_sun;
    m_sunLimit    = other.m_sunLimit;
    m_moon        = other.m_moon;
    m_moonLimit   = other.m_moonLimit;

    m_lifePercent = other.m_lifePercent;
    m_manaPercent = other.m_manaPercent;

    m_cast   = other.m_cast;
    m_reCast = other.m_reCast;
    m_stop   = other.m_stop;

    m_targetId      = other.m_targetId;
    m_lastCastSkill = other.m_lastCastSkill;

    globalCooldown        = other.globalCooldown;
    globalCooldownCurrent = other.globalCooldownCurrent;

    effectDamageAdditionalPercentInt = other.effectDamageAdditionalPercentInt;

    attribute = other.attribute;

    triggerEffects = other.triggerEffects;

    for (auto &[id, buff] : other.buffs) {
        Buff *b = buff->Clone();
        b->SetPlayer(this);
        buffs.emplace(id, b);
    }
    for (auto &[id, skill] : other.skills) {
        Skill *s = skill->Clone();
        s->SetPlayer(this);
        skills.emplace(id, s);
    }
    for (auto &[id, talent] : other.talents) {
        talents.emplace(id, talent);
    }
    for (auto &[id, secret] : other.recipes) {
        recipes.emplace(id, secret);
    }
    for (auto &[id, equipEffect] : other.equipEffects) {
        recipes.emplace(id, equipEffect);
    }

    return *this;
}

void JX3DPS::Player::SetTargets(Targets *targets)
{
    m_targets = targets;
    for (auto &[id, skill] : skills) {
        skill->SetTargets(targets);
    }
    for (auto &[id, buff] : buffs) {
        buff->SetTargets(targets);
    }
}

void Player::AddBuff3rds(const std::list<Id_t> &buff3rds) { }

void TriggerVoid(const Params &params) { }

} // namespace JX3DPS