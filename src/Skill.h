﻿/**
 * Project: 
 * File: Skill.h
 * Description:
 * Created Date: 2023-07-21 08:20:23
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-27 20:46:44
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_SKILL_H__
#define __JX3DPS_SKILL_H__

#include "Global/Defs.h"
#include "Global/Id.h"
#include "Global/Types.h"

#define SKILL_DEFAULT_FUNCTION(class_name)                 \
                                                           \
public:                                                    \
    class_name(JX3DPS::Player *player, Targets *targets);  \
    class_name(const class_name &other) : Skill(other) { } \
    ~class_name() { }                                      \
    class_name &operator=(const class_name &other)         \
    {                                                      \
        if (this == &other) {                              \
            return *this;                                  \
        }                                                  \
        JX3DPS::Skill::operator=(other);                   \
        return *this;                                      \
    }                                                      \
    class_name *Clone() const                              \
    {                                                      \
        return new class_name(*this);                      \
    }                                                      \
    void Trigger() override;                               \
    void Cast() override;                                  \
    void Stop() override;

namespace JX3DPS {

class Player;

class Skill
{
public:
    Skill(Player *player, Targets *targets);
    virtual ~Skill();

    Skill(const Skill &other);
    virtual Skill &operator=(const Skill &other);

    virtual Skill *Clone() const = 0;
    virtual void   Trigger()     = 0;
    virtual void   Cast()        = 0;
    virtual void   Stop()        = 0;

    void SetPlayer(Player *player);
    void SetTargets(Targets *targets);

    Frame_t GetNextKeyFrame() const;
    void    UpdateKeyFrame(Frame_t frame);

    Frame_t GetTriggerFrame() const;
    Frame_t GetCooldownCurrent() const;              // 用于宏条件判定

    void    SetEnergyCooldownCurrent(Frame_t frame); // 用于事件语句设置技能冷却
    Frame_t GetEnergyCooldownCurrent() const;        // 用于宏条件判定
    int     GetEnergyCountCurrent() const;           // 用于宏条件判定
    double  GetRange() const;                        // 用于宏条件判定

    inline Stats &GetStats() { return m_stats; }

    RollResult GetPhysicsRollResult() const;

    Damage GetPhysicsDamage(
        Id_t       targetId,
        RollResult rollResult,
        int        sub,
        int        level,
        Value_t    attack,
        Value_t    weaponDamage,
        Value_t    criticalStrikePower,
        Value_t    overcome,
        Value_t    strain);

    GainsDamage CalcPhysicsDamage(Id_t targetId, RollResult rollResult, int sub = 0, int level = 0);

    void Record(Id_t               targetId,
                RollResult         rollResult  = RollResult::HIT,
                const GainsDamage &gainsDamage = GainsDamage(),
                int                sub         = 0,
                int                level       = 0);

protected:
    Player  *m_player  = nullptr;
    Targets *m_targets = nullptr;

    /* ID */
    Id_t m_id = SKILL_DEFAULT;

    /* 名称 */
    std::string_view m_name = "";

    /* 施法距离 */
    double m_range = JX3DPS_UNLIMITED_RANGE;

    /* 目标数量上限 */
    int m_targetLimit = 0;

    /* 公共冷却 */
    Frame_t *m_globalCooldown     = nullptr;
    Frame_t  m_noneGlobalCooldown = 0;

    /* 技能冷却 */
    Frame_t m_cooldown        = 0;
    Frame_t m_cooldownCurrent = 0;

    /* 蓄力时间 */
    Frame_t m_prepareFrames        = JX3DPS_INVALID_FRAMES_SET;
    Frame_t m_prepareFramesCurrent = JX3DPS_INVALID_FRAMES_SET;

    /* 充能数 */
    int m_energyCount        = 1;
    int m_energyCountCurrent = JX3DPS_INVALID_ENERGY_SET;

    /* 技能会心加成 */
    PctInt_t m_effectCriticalStrikeAdditionalBasisPointInt = 0;

    /* 技能会心效果加成 */
    PctInt_t m_effectCriticalStrikePowerAdditionalPercentInt = 0;

    /* 技能伤害加成 */
    PctInt_t m_effectDamageAdditionalPercentInt = 0;

    /* 技能忽视加成 */
    PctInt_t m_effectShieldIgnoreAdditionalPercentInt = 0;

    /* 技能气点加成 */
    int m_effectQidianAdditional = 0;

    /* 伤害参数 */
    DamageParams m_damageParams;

    /* 统计 */
    Stats m_stats;

};

} // namespace JX3DPS

#endif // __JX3DPS_SKILL_H__