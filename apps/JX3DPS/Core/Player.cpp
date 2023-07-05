/**
 * Project: JX3DPS
 * File: Player.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-05 10:16:16
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Player.h"

#include <spdlog/spdlog.h>

#include "Buff.h"
#include "Skill.h"
#include "TaiXuJianYi.h"

JX3DPS::Frame_t JX3DPS::Player::s_globalCooldown = 16 + 8;

JX3DPS::Player::Player() { }

JX3DPS::Player::~Player() { }

JX3DPS::Player::Player(const Player &other)
{
    classType = other.classType;
    qidian    = other.qidian;
    mana      = other.mana;
    rage      = other.rage;
    energy    = other.energy;
    sun       = other.sun;
    moon      = other.moon;

    life = other.life;
    mane = other.mane;

    globalCooldown      = other.globalCooldown;
    globalCooldownFixed = other.globalCooldownFixed;

    targetId = other.targetId;

    lastCastSkill = other.lastCastSkill;

    isCast   = other.isCast;
    isReCast = other.isReCast;

    delayMin = other.delayMin;
    delayMax = other.delayMax;

    weaponCW      = other.weaponCW;
    classSetBuff  = other.classSetBuff;
    classSetSkill = other.classSetSkill;

    enchantShoes  = other.enchantShoes;
    enchantWrist  = other.enchantWrist;
    enchantBelt   = other.enchantBelt;
    enchantJacket = other.enchantJacket;
    enchantHat    = other.enchantHat;

    end  = other.end;
    stop = other.stop;

    damageAddPercentInt = other.damageAddPercentInt;
    skillDamageAddPercentInt = other.skillDamageAddPercentInt;

    for (auto &buff : other.buffs) {
        std::unique_ptr<Buff> ptr(buff.second->Clone());
        ptr->SetPlayer(this);
        buffs.emplace(buff.first, std::move(ptr));
    }
    for (auto &skill : other.skills) {
        std::unique_ptr<Skill> ptr(skill.second->Clone());
        ptr->SetPlayer(this);
        skills.emplace(skill.first, std::move(ptr));
    }
    for (auto &talent : other.talents) {
        talents.emplace(talent.first, talent.second);
    }
    for (auto &secret : other.secrets) {
        std::vector<bool> vec = secret.second;
        secrets.emplace(secret.first, vec);
    }

    attr          = std::make_unique<Attr>(other.classType);
    *(this->attr) = *(other.attr);
}

JX3DPS::Player &JX3DPS::Player::operator=(const Player &other)
{
    if (this == &other) {
        return *this;
    }

    classType = other.classType;
    qidian    = other.qidian;
    mana      = other.mana;
    rage      = other.rage;
    energy    = other.energy;
    sun       = other.sun;
    moon      = other.moon;

    life = other.life;
    mane = other.mane;

    globalCooldown      = other.globalCooldown;
    globalCooldownFixed = other.globalCooldownFixed;

    targetId = other.targetId;

    lastCastSkill = other.lastCastSkill;

    isCast   = other.isCast;
    isReCast = other.isReCast;

    delayMin = other.delayMin;
    delayMax = other.delayMax;

    weaponCW      = other.weaponCW;
    classSetBuff  = other.classSetBuff;
    classSetSkill = other.classSetSkill;

    enchantShoes  = other.enchantShoes;
    enchantWrist  = other.enchantWrist;
    enchantBelt   = other.enchantBelt;
    enchantJacket = other.enchantJacket;
    enchantHat    = other.enchantHat;

    end  = other.end;
    stop = other.stop;

    damageAddPercentInt = other.damageAddPercentInt;
    skillDamageAddPercentInt = other.skillDamageAddPercentInt;

    for (auto &buff : other.buffs) {
        std::unique_ptr<Buff> ptr(buff.second->Clone());
        ptr->SetPlayer(this);
        buffs.emplace(buff.first, std::move(ptr));
    }
    for (auto &skill : other.skills) {
        std::unique_ptr<Skill> ptr(skill.second->Clone());
        ptr->SetPlayer(this);
        skills.emplace(skill.first, std::move(ptr));
    }
    for (auto &talent : other.talents) {
        talents.emplace(talent.first, talent.second);
    }
    for (auto &secret : other.secrets) {
        std::vector<bool> vec = secret.second;
        secrets.emplace(secret.first, vec);
    }
    attr          = std::make_unique<Attr>(other.classType);
    *(this->attr) = *(other.attr);

    return *this;
}

std::shared_ptr<JX3DPS::Player> JX3DPS::Player::PlayerFactoryGenerate(JX3DPS::Class classType)
{
    switch (classType) {
        case JX3DPS::Class::TAI_XU_JIAN_YI: return std::make_shared<JX3DPS::TaiXuJianYi::Player>();
    }
    return nullptr;
}

void JX3DPS::Player::SetTargets(Targets *targets)
{
    for (auto &skill : skills) {
        skill.second->SetTargets(targets);
    }
    for (auto &buff : buffs) {
        buff.second->SetTargets(targets);
    }
}

void JX3DPS::Player::UpdateGlobalCooldown(Frame_t next)
{
    globalCooldown -= next;
    globalCooldown < 0 ? 0 : globalCooldown;
}

void JX3DPS::Player::AddQidian(int qidian)
{
    this->qidian += qidian;
    this->qidian  = this->qidian > 10 ? 10 : this->qidian;
}
