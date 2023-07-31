/**
 * Project: JX3DPS
 * File: Buff.h
 * Description:
 * Created Date: 2023-07-21 08:20:30
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-30 00:48:11
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_BUFF_H__
#define __JX3DPS_BUFF_H__

#include "Attribute/Attribute.hpp"
#include "Global/Defs.h"
#include "Global/Id.h"
#include "Global/Types.h"

#define BUFF_DEFAULT_FUNCTION(class_name)                                                     \
                                                                                              \
public:                                                                                       \
    class_name(JX3DPS::Player *player, Targets *targets);                                     \
    class_name(const class_name &other) : Buff(other) { }                                     \
    ~class_name() { }                                                                         \
    class_name &operator=(const class_name &other)                                            \
    {                                                                                         \
        if (this == &other) {                                                                 \
            return *this;                                                                     \
        }                                                                                     \
        JX3DPS::Buff::operator=(other);                                                       \
        return *this;                                                                         \
    }                                                                                         \
    class_name *Clone() const override                                                        \
    {                                                                                         \
        return new class_name(*this);                                                         \
    }                                                                                         \
    void Trigger() override;                                                                  \
    void Add(Id_t targetId, int stackNum, Frame_t durationMin, Frame_t durationMax) override; \
    void Clear(Id_t targetId, int stackNum) override;

namespace JX3DPS {

class Player;

class Buff
{
    struct Snapshot
    {
        /* 持续时间 */
        Frame_t duration = 0;

        /* 作用间隔 内置cd */
        Frame_t interval = JX3DPS_INVALID_FRAMES_SET;

        /* 层数 */
        int stackNum = 0;

        /* 快照攻击 */
        Value_t attackPower = 0;

        /* 快照会心 */
        Value_t criticalStrike = 0;

        /* 快照会心率 判定需要 */
        PctFloat_t criticalStrikePercent = 0;

        /* 快照会效 */
        Value_t criticalStrikePower = 0;

        /* 快照加速 */
        PctFloat_t hastePercent = 0.0;

        /* 快照无双 */
        Value_t strainBase = 0;

        /* 快照效果会心 */
        BPInt_t effectCriticalStrikeAdditionalBasisPointInt = 0;

        /* 快照效果会效 */
        PctInt_t effectCriticalStrikePowerAdditionalPercentInt = 0;

        /* 快照增伤 */
        PctInt_t effectDamageAdditionalPercentInt = 0;

        /* 快照无双加成 */
        PctInt_t strainBaseAdditionalPercentInt = 0;

        /* 快照收益攻击 */
        Value_t attackPowerGain = 0;

        /* 快照收益会效 */
        Value_t criticalStrikePowerGain = 0;

        /* 快照收益无双 */
        Value_t strainBaseGain = 0;

        void Snap(Attribute &attribute,
                  BPInt_t    effectCriticalStrikeAdditionalBasisPointInt,
                  PctInt_t   effectCriticalStrikePowerAdditionalPercentInt,
                  PctInt_t   effectDamageAdditionalPercentInt)
        {
            attackPower                    = attribute.GetPhysicsAttackPower();
            criticalStrike                 = attribute.GetPhysicsCriticalStrike();
            criticalStrikePercent          = attribute.GetPhysicsCriticalStrikePercent();
            criticalStrikePower            = attribute.GetPhysicsCriticalStrikePower();
            hastePercent                   = attribute.GetHastePercent();
            strainBase                     = attribute.GetStrainBase();
            strainBaseAdditionalPercentInt = attribute.GetStrainBaseAdditionalPercentInt();

            this->effectCriticalStrikeAdditionalBasisPointInt = effectCriticalStrikeAdditionalBasisPointInt;
            this->effectCriticalStrikePowerAdditionalPercentInt = effectCriticalStrikePowerAdditionalPercentInt;
            this->effectDamageAdditionalPercentInt = effectDamageAdditionalPercentInt;

            // 收益属性
            attribute.AddPhysicsAttackPowerBaseAdditional(0);
            attribute.AddPhysicsCriticalStrikePower(0);
            attribute.AddStrainBase(0);

            attackPowerGain         = attribute.GetPhysicsAttackPower();
            criticalStrikePowerGain = attribute.GetPhysicsCriticalStrikePower();
            strainBaseGain          = attribute.GetStrainBase();

            attribute.AddPhysicsAttackPowerBaseAdditional(-0);
            attribute.AddPhysicsCriticalStrikePower(-0);
            attribute.AddStrainBase(-0);
        }
    };

    using Snapshots = std::unordered_map<Id_t, Snapshot>;

public:
    Buff(Player *player, Targets *targets);
    virtual ~Buff();

    Buff(const Buff &other);
    virtual Buff &operator=(const Buff &other);

    virtual Buff *Clone() const                                             = 0;
    virtual void  Trigger()                                                 = 0;
    virtual void  Add(Id_t    targetId    = PLAYER_ID,
                      int     stackNum    = 1,
                      Frame_t durationMin = JX3DPS_DEFAULT_DURATION_FRAMES,
                      Frame_t durationMax = JX3DPS_DEFAULT_DURATION_FRAMES) = 0;
    virtual void  Clear(Id_t targetId = PLAYER_ID, int stackNum = 1)        = 0;

    void SetPlayer(Player *player);
    void SetTargets(Targets *targets);

    void Remove(Id_t targetId);

    Frame_t GetNextKeyFrame() const;
    void    UpdateKeyFrame(Frame_t frame);

    Frame_t GetDurationCurrent(Id_t targetId = PLAYER_ID) const; // 用于宏条件判定
    int     GetStackNumCurrent(Id_t targetId = PLAYER_ID) const; // 用于宏条件判定
    double  GetRange() const;                                    // 用于宏条件判定

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

    RollResult GetDotRollResult(Id_t targetId) const;

    Damage GetPhysicsDotDamage(
        Id_t       targetId,
        RollResult rollResult,
        int        sub,
        int        level,
        int        effectCount,
        Value_t    attack,
        Value_t    weaponDamage,
        Value_t    criticalStrikePower,
        Value_t    overcome,
        Value_t    strain);

    GainsDamage CalcPhysicsDotDamage(Id_t targetId, RollResult rollResult, int sub = 0, int level = 0, int effectCount = 1);

    void Record(Id_t               targetId,
                RollResult         rollResult  = RollResult::HIT,
                const GainsDamage &gainsDamage = GainsDamage(),
                int                sub         = 0,
                int                level       = 0);

protected:
    Player  *m_player  = nullptr;
    Targets *m_targets = nullptr;

    /* ID */
    Id_t m_id = BUFF_DEFAULT;

    /* 名称 */
    std::string_view m_name = "";

    /* 施法距离 */
    double m_range = JX3DPS_UNLIMITED_RANGE;

    /* 目标数量上限 */
    int m_targetLimit = 0;

    /* 快照 */
    Snapshots m_snapshots;

    /* 持续时间 */
    Frame_t m_duration = 0;

    /* 作用间隔 */
    Frame_t m_interval = 0;

    /* 内置CD */
    Frame_t m_cooldown        = 0;
    Frame_t m_cooldownCurrent = 0;

    /* 层数 */
    int m_stackNum = 0;

    /* 作用次数 */
    int m_effectCount = 0;

    /* 会心加成 */
    PctInt_t m_effectCriticalStrikeAdditionalBasisPointInt = 0;

    /* 会心效果加成 */
    PctInt_t m_effectCriticalStrikePowerAdditionalPercentInt = 0;

    /* 伤害加成 */
    PctInt_t m_effectDamageAdditionalPercentInt = 0;

    /* 忽视加成 */
    PctInt_t m_effectShieldIgnoreAdditionalPercentInt = 0;

    /* 伤害参数 */
    DamageParams m_damageParams;

    /* 统计 */
    Stats m_stats;
};

} // namespace JX3DPS

#endif // __JX3DPS_BUFF_H__