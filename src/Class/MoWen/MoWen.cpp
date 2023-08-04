/**
 * Project: JX3DPS
 * File: MoWen.cpp
 * Description:
 * Created Date: 2023-07-31 16:03:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-02 23:32:32
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	----- ----------------------------------------------------------
 */

#include "MoWen.h"

#include "MoWenBuff.h"
#include "MoWenSkill.h"
#include "Target.hpp"

namespace JX3DPS {

namespace MoWen {

Player::Player()
{
    attribute.SetClassType(ClassType::MO_WEN);
}

void Player::Init()
{
    if (talents[TALENT_XIAN_FENG]) {
        buffs.emplace(BUFF_XIAN_FENG, new Buff::XianFeng(this, nullptr));
        buffs.emplace(BUFF_XIAN_FENG_BIAO_JI, new Buff::XianFengBiaoJi(this, nullptr));

        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                             std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                              std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                            std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                            std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                           std::bind(&TriggerXianFeng, std::placeholders::_1));

        skills[SKILL_GAO_SHAN_LIU_SHUI]->AddTriggerEffect(
            TRIGGER_XIAN_FENG_BIAO_JI,
            std::bind(&TriggerXianFengBiaoJi, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_XIAN_FENG_BIAO_JI,
            std::bind(&TriggerXianFengBiaoJi, std::placeholders::_1));
        skills[SKILL_YI_XING_HUAN_YING]->AddTriggerEffect(
            TRIGGER_XIAN_FENG_BIAO_JI,
            std::bind(&TriggerXianFengBiaoJi, std::placeholders::_1));
    } else {
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                             std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                           std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_GAO_SHAN_LIU_SHUI]->AddTriggerEffect(TRIGGER_XIAN_FENG_BIAO_JI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_XIAN_FENG_BIAO_JI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YI_XING_HUAN_YING]->AddTriggerEffect(TRIGGER_XIAN_FENG_BIAO_JI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_YUN_HAN]) {
        buffs.emplace(BUFF_YUN_HAN, new Buff::YunHan(this, nullptr));

        skills[SKILL_SHU_YING_HENG_XIE]->AddTriggerEffect(TRIGGER_YUN_HAN,
                                                          std::bind(&TriggerYunHan, std::placeholders::_1));
    } else {
        skills[SKILL_SHU_YING_HENG_XIE]->AddTriggerEffect(TRIGGER_YUN_HAN,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_CAN_LIAN]) {
        buffs.emplace(BUFF_CAN_LIAN, new Buff::CanLian(this, nullptr));

        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_CAN_LIAN_ADD,
            std::bind(&TriggerCanLianAdd, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_CAN_LIAN_CLEAR,
            std::bind(&TriggerCanLianClear, std::placeholders::_1));
    } else {
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_CAN_LIAN_ADD,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_CAN_LIAN_CLEAR,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_SHU_LI]) {
        buffs.emplace(BUFF_SHU_LI, new Buff::ShuLi(this, nullptr));

        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SHU_LI, std::bind(&TriggerShuLi, std::placeholders::_1));
    } else {
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SHU_LI, std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_SHI_XIANG]) {
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SHI_XIANG,
                                           std::bind(&TriggerShiXiang, std::placeholders::_1));
    } else {
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SHI_XIANG,
                                           std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_ZHI_ZHI]) {
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ZHI_ZHI,
                                                  std::bind(&TriggerZhiZhi, std::placeholders::_1));
    } else {
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ZHI_ZHI,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));
    }
}

void Player::TriggerXianFeng(const Params &params)
{
    static_cast<Buff::XianFeng *>(params.player->buffs[BUFF_XIAN_FENG])->TriggerAdd();
}

void Player::TriggerXianFengBiaoJi(const Params &params)
{
    if (params.player->buffs[BUFF_XIAN_FENG]->GetDurationCurrent() > 0) {
        static_cast<Buff::XianFengBiaoJi *>(params.player->buffs[BUFF_XIAN_FENG_BIAO_JI])->TriggerAdd();
    }
}

void Player::TriggerXianFengDamage(const Params &params)
{
    if (params.player->buffs[BUFF_XIAN_FENG_BIAO_JI]->GetDurationCurrent() > 0) {
        static_cast<Buff::XianFeng *>(params.player->buffs[BUFF_XIAN_FENG])
            ->TriggerDamage(params.targetId, params.stackNum);
    }
}

void Player::TriggerYunHan(const Params &params)
{
    static_cast<Buff::YunHan *>(params.player->buffs[BUFF_YUN_HAN])
        ->TriggerAdd(params.stackNum * static_cast<int>(params.type));
}

void Player::TriggerCanLianAdd(const Params &params)
{
    static_cast<Buff::CanLian *>(params.player->buffs[BUFF_CAN_LIAN])->TriggerAdd();
}

void Player::TriggerCanLianClear(const Params &params)
{
    static_cast<Buff::CanLian *>(params.player->buffs[BUFF_CAN_LIAN])->TriggerClear();
}

void Player::TriggerShuLi(const Params &params)
{
    static_cast<Buff::ShuLi *>(params.player->buffs[BUFF_SHU_LI])->TriggerAdd();
}

void Player::TriggerShiXiang(const Params &params)
{
    static_cast<Skill::Zhi *>(params.player->skills[SKILL_ZHI])->TriggerShiXiang();
    static_cast<Skill::BianZhi *>(params.player->skills[SKILL_BIAN_ZHI])->TriggerShiXiang();
    static_cast<Skill::Yu *>(params.player->skills[SKILL_YU])->TriggerShiXiang();
}

void Player::TriggerZhiZhi(const Params &params)
{
    if (params.player->buffs[BUFF_SHANG]->GetDurationCurrent(params.targetId) > 0) {
        static_cast<Buff::Shang *>(params.player->buffs[BUFF_SHANG])->TriggerAdd(params.targetId);
    }
    if (params.player->buffs[BUFF_JUE]->GetDurationCurrent(params.targetId) > 0) {
        static_cast<Buff::Jue *>(params.player->buffs[BUFF_JUE])->TriggerAdd(params.targetId);
    }

    // TODO
}

} // namespace MoWen

} // namespace JX3DPS