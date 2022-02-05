/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:37
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 21:35:14
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
    publicCooldown -= frames;
    if (publicCooldown < 0) {
        publicCooldown = 0;
    }
#else
    publicCooldown -= frames;
    publicCooldown = MINUS_TO_0_ELSE_1(publicCooldown) * publicCooldown;
#endif
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

Attr &Player::Attr()
{
    return *m_attr;
}

SkillHash &Player::Skill()
{
    return m_skillHash;
}

BuffHash &Player::Buff()
{
    return m_buffHash;
}

bool Player::GetGearSetCW() const
{
    return m_gearSetCW;
}

bool Player::GetGearSetAttack() const
{
    return m_gearSetAttack;
}

} // namespace JX3DPS
