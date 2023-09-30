/**
 * Project: 
 * File: FenShanJing.cpp
 * Description: 
 * Created Date: 2023-08-25 20:44:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-30 11:59:28
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */


#include "FenShanJing.h"

#include "Buff3rd.h"
#include "FenShanJingBuff.h"
#include "FenShanJingSkill.h"
#include "Target.hpp"

namespace JX3DPS {

namespace FenShanJing {

Player::Player() : JX3DPS::Player()
{
    attribute.SetClassType(ClassType::FEN_SHAN_JING);
}

Player::Player(const Player &other) : JX3DPS::Player(other)
{
    this->buffs[BUFF_CLASS_FEATURE]->Add();
}

void Player::Init()
{
    skills.emplace(SKILL_PO_ZHAO, static_cast<JX3DPS::Skill *>(new Skill::PoZhao(this, nullptr)));
}

void Player::TriggerWuYi(const Params &params)
{
    if (params.level >= 6) {
        params.player->skills[SKILL_WU_WO_WU_JIAN]->AddCriticalStrikeAdditionalBasisPointInt(
            1000 * static_cast<int>(params.type));
        params.player->skills[SKILL_WU_WO_WU_JIAN]->AddCriticalStrikePowerAdditionalPercentInt(
            307 * static_cast<int>(params.type));
    }
}

} // namespace FenShanJing

} // namespace JX3DPS