/**
 * Project: 
 * File: Buff.cpp
 * Description:
 * Created Date: 2023-07-22 08:33:14
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-27 20:43:01
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Buff.h"

JX3DPS::Buff::Buff(Player *player, Targets *targets)
{
    this->m_player  = player;
    this->m_targets = targets;
}

JX3DPS::Buff::~Buff() { }

JX3DPS::Buff::Buff(const Buff &other)
{
    m_id             = other.m_id;
    m_name           = other.m_name;
    m_range          = other.m_range;
    m_snapshots      = other.m_snapshots;
    m_duration       = other.m_duration;
    m_interval       = other.m_interval;
    m_stackNum       = other.m_stackNum;
    m_effectCount    = other.m_effectCount;
    m_damageParams   = other.m_damageParams;
    m_stats          = other.m_stats;
    m_effectCriticalStrikeAdditionalBasisPointInt = other.m_effectCriticalStrikeAdditionalBasisPointInt;
    m_effectCriticalStrikePowerAdditionalPercentInt = other.m_effectCriticalStrikePowerAdditionalPercentInt;
    m_effectDamageAdditionalPercentInt       = other.m_effectDamageAdditionalPercentInt;
    m_effectShieldIgnoreAdditionalPercentInt = other.m_effectShieldIgnoreAdditionalPercentInt;
}

JX3DPS::Buff &JX3DPS::Buff::operator=(const Buff &other)
{
    if (this == &other) {
        return *this;
    }

    m_id             = other.m_id;
    m_name           = other.m_name;
    m_range          = other.m_range;
    m_snapshots      = other.m_snapshots;
    m_duration       = other.m_duration;
    m_interval       = other.m_interval;
    m_stackNum       = other.m_stackNum;
    m_effectCount    = other.m_effectCount;
    m_damageParams   = other.m_damageParams;
    m_stats          = other.m_stats;
    m_effectCriticalStrikeAdditionalBasisPointInt = other.m_effectCriticalStrikeAdditionalBasisPointInt;
    m_effectCriticalStrikePowerAdditionalPercentInt = other.m_effectCriticalStrikePowerAdditionalPercentInt;
    m_effectDamageAdditionalPercentInt       = other.m_effectDamageAdditionalPercentInt;
    m_effectShieldIgnoreAdditionalPercentInt = other.m_effectShieldIgnoreAdditionalPercentInt;

    return *this;
}

void JX3DPS::Buff::SetPlayer(Player *player)
{
    m_player = player;
}

void JX3DPS::Buff::SetTargets(Targets *targets)
{
    m_targets = targets;
}

void JX3DPS::Buff::Remove(Id_t targetId)
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) {
        return;
    }
    m_snapshots.erase(targetId);
}

JX3DPS::Frame_t JX3DPS::Buff::GetNextKeyFrame() const
{
    Frame_t next = JX3DPS_INVALID_FRAMES_SET;
    for (auto &snapshot : m_snapshots) {
        next = std::min(next, snapshot.second.interval);
        next = std::min(next, snapshot.second.duration);
    }
    return next;
}

void JX3DPS::Buff::UpdateKeyFrame(Frame_t frame)
{
    for (auto &snapshot : m_snapshots) {
        if (snapshot.second.duration != JX3DPS_INVALID_FRAMES_SET) {
            snapshot.second.duration -= frame;
        }
        if (snapshot.second.interval != JX3DPS_INVALID_FRAMES_SET) {
            snapshot.second.interval -= frame;
        }
    }
}

JX3DPS::Frame_t JX3DPS::Buff::GetDurationCurrent(Id_t targetId) const
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) {
        return 0;
    }
    return m_snapshots.at(targetId).duration;
}

int JX3DPS::Buff::GetStackNum(Id_t targetId) const
{
    if (m_snapshots.find(targetId) == m_snapshots.end()) {
        return 0;
    }
    return m_snapshots.at(targetId).stackNum;
}

double JX3DPS::Buff::GetRange() const
{
    return m_range;
}