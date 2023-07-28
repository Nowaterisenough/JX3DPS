﻿/**
 * Project: 
 * File: TaiXuJianYi.h
 * Description:
 * Created Date: 2023-07-20 02:40:46
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-27 23:00:01
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "TaiXuJianYi.h"

#include "TaiXuJianYiSkill.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Player::Player()
{
    attribute.SetClassType(ClassType::TAI_XU_JIAN_YI);
}

void Player::Init()
{
    skills.emplace(SKILL_WU_WO_WU_JIAN, new Skill::WuWoWuJian(this, nullptr));

    if (talents[TALENT_SHEN_MAI]) {
        triggerEffects.emplace(TALENT_SHEN_MAI, std::bind(&TaiXuJianYi::Player::TriggerShenMai, this, std::placeholders::_1));
    } else {
        triggerEffects.emplace(TALENT_SHEN_MAI, std::bind(&TriggerVoid, std::placeholders::_1));
    }

    // if (talents[TALENT_WU_YI]) {
    //     triggerEffects.emplace(TALENT_WU_YI, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    //     triggerEffects.emplace(TALENT_WU_YI_CLEAR, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(TALENT_WU_YI, std::bind(&TriggerVoid, std::placeholders::_1));
    //     triggerEffects.emplace(TALENT_WU_YI_CLEAR, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[TALENT_WU_YU]) {
    //     triggerEffects.emplace(TALENT_WU_YU, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(TALENT_WU_YU, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[TALENT_DIE_REN]) {
    //     triggerEffects.emplace(TALENT_DIE_REN, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(TALENT_DIE_REN, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[EQUIP_EFFECT_ENCHANT_WRIST]) {
    //     triggerEffects.emplace(EQUIP_EFFECT_ENCHANT_WRIST, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(EQUIP_EFFECT_ENCHANT_WRIST, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[EQUIP_EFFECT_ENCHANT_SHOES]) {
    //     triggerEffects.emplace(EQUIP_EFFECT_ENCHANT_SHOES, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(EQUIP_EFFECT_ENCHANT_SHOES, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[TALENT_CHANG_SHENG]) {
    //     triggerEffects.emplace(TALENT_CHANG_SHENG, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(TALENT_CHANG_SHENG, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN]) {
    //     triggerEffects.emplace(BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[EQUIP_EFFECT_ENCHANT_BELT]) {
    //     triggerEffects.emplace(EQUIP_EFFECT_ENCHANT_BELT, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(EQUIP_EFFECT_ENCHANT_BELT, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[EQUIP_EFFECT_WEAPON_CW]) {
    //     triggerEffects.emplace(EQUIP_EFFECT_WEAPON_CW, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(EQUIP_EFFECT_WEAPON_CW, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[EQUIP_EFFECT_WEAPON_WATER]) {
    //     triggerEffects.emplace(EQUIP_EFFECT_WEAPON_WATER, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(EQUIP_EFFECT_WEAPON_WATER, std::bind(&TriggerVoid, std::placeholders::_1));
    // }

    // if (talents[EQUIP_EFFECT_SET_ATTRIBUTE]) {
    //     triggerEffects.emplace(EQUIP_EFFECT_SET_ATTRIBUTE, std::bind(&TaiXuJianYi::Skill::WuWoWuJian::TriggerWuYi, this->skills[SKILL_WU_WO_WU_JIAN], std::placeholders::_1));
    // } else {
    //     triggerEffects.emplace(EQUIP_EFFECT_SET_ATTRIBUTE, std::bind(&TriggerVoid, std::placeholders::_1));
    // }
}

void Player::TriggerShenMai(const Params &params)
{
    this->AddQidian(2);
}

} // namespace TaiXuJianYi

} // namespace JX3DPS