/**
 * Project: JX3DPS
 * File: MoWen.cpp
 * Description: 
 * Created Date: 2023-07-31 16:03:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-01 00:55:34
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
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

void Player::Init() { }

} // namespace MoWen

} // namespace JX3DPS