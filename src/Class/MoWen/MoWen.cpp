/**
 * Project: JX3DPS
 * File: MoWen.cpp
 * Description:
 * Created Date: 2023-07-31 16:03:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-02 00:56:00
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

} // namespace MoWen

} // namespace JX3DPS