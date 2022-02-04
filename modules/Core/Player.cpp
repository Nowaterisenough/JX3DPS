/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:37
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 13:14:19
 * @FilePath    : \JX3DPS\modules\Core\Player.cpp
 */

#include "Player.h"

namespace JX3DPS {

Player::Player(Class classType) : m_classType(classType)
{
    m_attr = new class Attr(classType);
}

Player::~Player()
{
    delete m_attr;
    m_attr = nullptr;
}

Class Player::GetClass()
{
    return m_classType;
}

void Player::UpdateTime(Frame_t frames)
{
#ifndef OPTIMIZE_SET
    m_publicCooldown -= frames;
    if (m_publicCooldown < 0) {
        m_publicCooldown = 0;
    }
#else
    m_publicCooldown -= frames;
    m_publicCooldown = MINUS_TO_0_ELSE_1(m_publicCooldown) * m_publicCooldown;
#endif
}

bool Player::SetCast(bool isCast)
{
    m_isCast = isCast;
    return m_isCast;
}

bool Player::IsCast()
{
    return m_isCast;
}

bool Player::SetReCast(bool isReCast)
{
    m_isReCast = isReCast;
    return m_isReCast;
}

bool Player::IsReCast()
{
    return m_isReCast;
}

Id_t Player::SetLastSkill(Id_t skillId)
{
    m_lastSkill = skillId;
    return m_lastSkill;
}

Id_t Player::GetLastSkill()
{
    return m_lastSkill;
}

bool Player::SetStop(bool stop)
{
    m_stop = stop;
    return m_stop;
}

bool Player::GetStop()
{
    return m_stop;
}

Attr &Player::Attr()
{
    return *m_attr;
}

bool Player::GetGearSetCW()
{
    return m_gearSetCW;
}

bool Player::GetGearSetAttack()
{
    return m_gearSetAttack;
}

} // namespace JX3DPS
