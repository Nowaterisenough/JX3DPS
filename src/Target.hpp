/**
 * Project: JX3DPS
 * File: Target.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-03 15:53:43
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_TARGET_H__
#define __JX3DPS_TARGET_H__

#include <utility>

#include "Global/Id.h"
#include "Global/Types.h"

namespace JX3DPS {

class Target
{
public:
    Target() { }

    ~Target() { }

    /* ID */
    inline void SetId(Id_t id) { m_id = id; }

    inline Id_t GetId() const { return m_id; }

    /* 等级 */
    inline void SetLevel(int level) { m_level = level; }

    inline int GetLevel() const { return m_level; }

    /* 距离 */
    inline void SetDistance(double distance) { m_distance = distance; }

    inline double GetDistance() const { return m_distance; }

    /* 生命时间 */
    inline void SetLifeTime(Frame_t lifeTime) { m_lifeTime = lifeTime; }

    inline Frame_t GetLifeTime() const { return m_lifeTime; }

    /* 生命值 */
    inline PctFloat_t GetLifePercent() const { return m_lifePercent; }

    inline void SetLifePercent(const PctFloat_t percent) { m_lifePercent = percent; }

    inline void AddLifePercent(const PctFloat_t percent)
    {
        m_lifePercent += percent;
        m_lifePercent  = std::min(m_lifePercent, 1.0);
        m_lifePercent  = std::max(m_lifePercent, 0.0);
    }

    inline PctFloat_t GetManaPercent() const { return m_manaPercent; }

    inline void SetManaPercent(const PctFloat_t percent) { m_manaPercent = percent; }

    inline void AddManaPercent(const PctFloat_t percent)
    {
        m_manaPercent += percent;
        m_manaPercent  = std::min(m_manaPercent, 1.0);
        m_manaPercent  = std::max(m_manaPercent, 0.0);
    }

    /* 外防 */
    Value_t GetPhysicsShield() const { return m_physicsShield; }

    void SetPhysicsShield(const Value_t value) { m_physicsShield = value; }

    void AddPhysicsShield(const Value_t value) { m_physicsShield += value; }

    /* 内防 */
    Value_t GetMagicShield() const { return m_magicShield; }

    void SetMagicShield(const Value_t value) { m_magicShield = value; }

    void AddMagicShield(const Value_t value) { m_magicShield += value; }

    /* 易伤 */
    PctInt_t GetDamageAdditionalPercentInt() const
    {
        return m_damageAdditionalPercentInt;
    }

    void SetDamageAdditionalPercentInt(const PctInt_t PercentInt)
    {
        m_damageAdditionalPercentInt = PercentInt;
    }

    void AddDamageAdditionalPercentInt(const PctInt_t PercentInt)
    {
        m_damageAdditionalPercentInt += PercentInt;
    }

private:
    /* ID */
    Id_t m_id = TARGET_PLACE_HOLDERS_DEFAULT;

    /* 等级 */
    int m_level = 0;

    /* 距离 */
    double m_distance = 0.0;

    Frame_t m_lifeTime = JX3DPS_UNLIMITED_LIFE_TIME;

    /* 生命值 */
    PctFloat_t m_lifePercent = 1.0;

    /* 能量值 */
    PctFloat_t m_manaPercent = 1.0;

    /* 外防 */
    Value_t m_physicsShield = 0;

    /* 内防 */
    Value_t m_magicShield = 0;

    /* 易伤 */
    PctInt_t m_damageAdditionalPercentInt = 0;
};

} // namespace JX3DPS

#endif // __JX3DPS_TARGET_H__