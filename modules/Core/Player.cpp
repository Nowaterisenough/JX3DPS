/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:37
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 13:49:51
 * @FilePath    : \JX3DPS\modules\Core\Player.cpp
 */

#include "Player.h"

#include "Attr.h"
#include "Skill.h"
#include "Buff.h"

namespace JX3DPS {

Player::Player(JX3Class classType) : m_classType(classType)
{
    attr = new JX3DPS::Attr(classType);
}

Player::~Player()
{
    delete attr;
    attr = nullptr;
}

JX3Class Player::GetClass()
{
    return m_classType;
}

void Player::UpdateTime(Frame_t frames)
{
    publicCooldown -= frames;
    if (publicCooldown < 0) {
        publicCooldown = 0;
    }
}

void Player::SetCast(bool isCast)
{
    m_isCast = isCast;
}

bool Player::IsCast() const
{
    return m_isCast;
}

void Player::SetReCast(bool isReCast)
{
    m_isReCast = isReCast;
}

bool Player::IsReCast() const
{
    return m_isReCast;
}

void Player::SetLastSkill(Id_t skillId)
{
    m_lastSkill = skillId;
}

Id_t Player::GetLastSkill() const
{
    return m_lastSkill;
}

void Player::SetStop(bool stop)
{
    m_stop = stop;
}

bool Player::GetStop() const
{
    return m_stop;
}

void Player::SetGearSetCW(bool gearSetCW) {}

bool Player::GetGearSetCW() const
{
    return m_gearSetCW;
}

void Player::SetGearSetClass(bool gearSetClass) {}

void Player::SetGearSetAttack(bool gearSetAttack) {}

bool Player::GetGearSetAttack() const
{
    return m_gearSetAttack;
}

} // namespace JX3DPS
