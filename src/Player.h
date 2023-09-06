/**
 * Project: JX3DPS
 * File: Player.h
 * Description:
 * Created Date: 2023-07-20 02:39:34
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-06 17:34:13
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_PLAYER_H__
#define __JX3DPS_PLAYER_H__

#include <unordered_set>

#include "Attribute/Attribute.hpp"
#include "Global/Defs.h"
#include "Global/JX3.h"
#include "Utils/Utils.hpp"

namespace JX3DPS {

class Player
{
public:
    static Player *PlayerFactoryGenerate(ClassType classType);

    Player();
    virtual ~Player();

    Player(const Player &other);
    virtual Player &operator=(const Player &other);

    virtual Player *Clone() const = 0;
    virtual void    Init()        = 0;

    virtual Frame_t GetNextGlobalCooldown() const
    {
        Frame_t frame = JX3DPS_INVALID_FRAMES_SET;
        if (globalCooldownCurrent > 0) {
            frame = globalCooldownCurrent;
        }
        return frame;
    }

    void SetTargets(Targets *targets);

    inline Targets *GetTargets() const { return m_targets; }

    void AddBuff3rds(const std::unordered_set<Id_t> &buff3rds);

    inline ClassType GetClassType() const { return this->attribute.GetClassType(); }

    inline virtual void UpdateGlobalCooldown(Frame_t next)
    {
        globalCooldownCurrent -= next;
        if (globalCooldownCurrent < 0) {
            globalCooldownCurrent = 0;
        }
    }

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

    inline int GetQidian() const { return m_qidian; }

    inline void SetQidian(int qidian) { m_qidian = qidian; }

    inline void AddQidian(int qidian)
    {
        this->m_qidian += qidian;
        this->m_qidian  = std::min(this->m_qidian, m_qidianLimit);
    }

    inline int GetRage() const { return m_rage; }

    inline void SetRage(int rage) { m_rage = rage; }

    inline void AddRage(int rage)
    {
        this->m_rage += rage;
        this->m_rage  = std::min(this->m_rage, m_rageLimit);
        this->m_rage  = std::max(this->m_rage, 0);
    }

    inline int GetEnergy() const { return m_energy; }

    inline void SetEnergy(int energy) { m_energy = energy; }

    inline void AddEnergy(int energy)
    {
        this->m_energy += energy;
        this->m_energy  = std::min(this->m_energy, m_energyLimit);
    }

    inline int GetSun() const { return m_sun; }

    inline void SetSun(int sun) { m_sun = sun; }

    inline void AddSun(int sun)
    {
        this->m_sun += sun;
        this->m_sun  = std::min(this->m_sun, m_sunLimit);
    }

    inline int GetMoon() const { return m_moon; }

    inline void SetMoon(int moon) { m_moon = moon; }

    inline void AddMoon(int moon)
    {
        this->m_moon += moon;
        this->m_moon  = std::min(this->m_moon, m_moonLimit);
    }

    inline bool IsCast() const { return m_cast; }

    inline void SetCast(bool cast) { m_cast = cast; }

    inline bool IsReCast() const { return m_reCast; }

    inline void SetReCast(bool reCast) { m_reCast = reCast; }

    inline bool IsStop() const { return m_stop; }

    inline void SetStop(bool stop) { m_stop = stop; }

    inline Id_t GetTargetId() const { return m_targetId; }

    inline void SetTargetId(Id_t targetId)
    {
        if (m_targets->find(targetId) == m_targets->end()) {
            m_targetId = TARGET_PLACE_HOLDERS_1;
            return;
        }
        m_targetId = targetId;
    }

    inline Id_t GetLastCastSkill() const { return m_lastCastSkill; }

    inline void SetLastCastSkill(Id_t lastCastSkill) { m_lastCastSkill = lastCastSkill; }

    inline Id_t GetReCastSkill() const { return m_reCastSkill; }

    inline void SetReCastSkill(Id_t reCastSkill) { m_reCastSkill = reCastSkill; }

    inline Id_t GetCastSkill() const { return m_castSkill; }

    inline void SetCastSkill(Id_t castSkill) { m_castSkill = castSkill; }

    inline Frame_t DelayFrames() const
    {
        return RandomNormal(m_delayMin, m_delayMax + 1) / JX3DPS_DELAY;
    }

    inline void SetDelay(int delayMin, int delayMax)
    {
        m_delayMin = delayMin;
        m_delayMax = delayMax;
    }

    inline static void TriggerVoid(const Params &params) { }

    static void TriggerWeaponWater(const Params &params);

public:
    Frame_t globalCooldown        = 24; // 冷却
    Frame_t globalCooldownCurrent = 0;  // 当前冷却

    PctInt_t effectDamageAdditionalPercentInt = 0; // 一些奇穴或者秘籍效果的全局伤害加成

    Attribute attribute; // 属性
    ClassType teamCore = ClassType::DEFAULT;

    Talents      talents;      // 奇穴列表
    Recipes      recipes;      // 秘籍列表
    Skills       skills;       // 技能列表
    Buffs        buffs;        // Buff列表
    EquipEffects equipEffects; // 装备效果列表

private:
    Targets *m_targets = nullptr;

    PctFloat_t m_lifePercent = 1.0;
    PctFloat_t m_manaPercent = 1.0;

    int m_qidian      = 10;  // 气点
    int m_qidianLimit = 10;  // 气点上限
    int m_rage        = 0;   // 怒气
    int m_rageLimit   = 100; // 怒气上限
    int m_energy      = 0;   // 元气
    int m_energyLimit = 100; // 元气上限
    int m_sun         = 0;   // 日精
    int m_sunLimit    = 100; // 日精上限
    int m_moon        = 0;   // 月精
    int m_moonLimit   = 100; // 月精上限

    bool m_cast   = false; // 是否正在蓄力(正读条)
    bool m_reCast = false; // 是否正在倒读条
    bool m_stop   = false; // 是否停手

    Id_t m_targetId      = TARGET_PLACE_HOLDERS_DEFAULT; // 目标ID
    Id_t m_lastCastSkill = SKILL_DEFAULT;                // 上次施放技能ID
    Id_t m_reCastSkill   = SKILL_DEFAULT;                // 正在倒读条技能ID
    Id_t m_castSkill     = SKILL_DEFAULT;                // 正在正读条技能ID

    int m_delayMin = 0; // 最小延迟
    int m_delayMax = 0; // 最大延迟
};

} // namespace JX3DPS

#endif // __JX3DPS_PLAYER_H__