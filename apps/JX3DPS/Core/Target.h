/**
 * Project: JX3DPS
 * File: Target.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-20 00:01:52
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef JX3DPS_TARGET_H_
#define JX3DPS_TARGET_H_

#include "Globals.h"

namespace JX3DPS {

class Target
{
public:
    Target();
    ~Target();

    /* ID */
    void SetId(Id_t id);
    Id_t GetId() const;

    /* 等级 */
    void SetLevel(int level);
    int  GetLevel() const;

    /* 距离 */
    void  SetDistance(float distance);
    float GetDistance() const;

    /* 生命值 */
    PctFloat_t GetLifePercent() const;
    void       SetLifePercent(const PctFloat_t percent);
    void       AddLifePercent(const PctFloat_t percent);

    PctFloat_t GetManaPercent() const;
    void       SetManaPercent(const PctFloat_t percent);
    void       AddManaPercent(const PctFloat_t percent);

    /* 忽视防御 */
    PctInt_t GetShieldIgnoreBinPercent() const;
    void     SetShieldIgnoreBinPercent(const PctInt_t binPercent);

    /* 外防 */
    Value_t    GetPhysicsShield() const;
    void       SetPhysicsShield(const Value_t value);
    void       AddPhysicsShield(const Value_t value);
    PctInt_t   GetPhysicsShieldBinPercent() const;
    void       SetPhysicsShieldBinPercent(const PctInt_t binPercent);
    void       AddPhysicsShieldBinPercent(const PctInt_t binPercent);
    PctFloat_t GetPhysicsResistPercent() const;

    /* 内防 */
    Value_t    GetMagicShield() const;
    void       SetMagicShield(const Value_t value);
    void       AddMagicShield(const Value_t value);
    PctInt_t   GetMagicShieldBinPercent() const;
    void       SetMagicShieldBinPercent(const PctInt_t binPercent);
    void       AddMagicShieldBinPercent(const PctInt_t binPercent);
    PctFloat_t GetMagicResistPercent() const;

    /* 易伤 */
    PctInt_t   GetDamageAddBinPercent() const;
    void       SetDamageAddBinPercent(const PctInt_t binPercent);
    void       AddDamageAddBinPercent(const PctInt_t binPercent);
    PctFloat_t GetDamageAddPercent() const;

private:
    /* 外功免伤 */
    void UpdatePhysicsResistPercent();

    /* 内功免伤 */
    void UpdateMagicResistPercent();

    /* 易伤 */
    void UpdateDamageAddPercent();

    /* ID */
    Id_t m_id = 0;

    /* 等级 */
    int m_level = 0;

    /* 距离 */
    float m_distance = 0.0;

    /* 生命值 */
    PctFloat_t m_lifePercent = 1.0;

    /* 生命值 */
    PctFloat_t m_manaPercent = 1.0;

    /* 忽视防御 */
    PctInt_t m_shieldIgnoreBinPercent = 0;

    /* 外防 */
    Value_t    m_physicsShield           = 0;
    PctInt_t   m_physicsShieldBinPercent = 0;
    PctFloat_t m_physicsResistPercent    = 0.0;

    /* 内防 */
    Value_t    m_magicShield           = 0;
    PctInt_t   m_magicShieldBinPercent = 0;
    PctFloat_t m_magicResistPercent    = 0.0;

    /* 易伤 */
    PctInt_t   m_damageAddBinPercent = 0;
    PctFloat_t m_damageAddPercent    = 0.0;
};

} // namespace JX3DPS

#endif // JX3DPS_TARGET_H_