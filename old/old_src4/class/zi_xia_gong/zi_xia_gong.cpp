#include "zi_xia_gong.h"

#include "buff3rd.h"
#include "zi_xia_gong_buff.h"
#include "zi_xia_gong_skill.h"
#include "target.hpp"

namespace JX3DPS {

namespace ZiXiaGong {

Player::Player() : JX3DPS::Player()
{
    attribute.SetClassType(ClassType::ZI_XIA_GONG);
}

Player::Player(const Player &other) : JX3DPS::Player(other)
{
    this->buffs[BUFF_CLASS_FEATURE]->Add();

}

void Player::Init()
{
}

} // namespace ZiXiaGong

} // namespace JX3DPS