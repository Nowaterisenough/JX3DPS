/**
 * @Description: 目标类
 * @Create: 2020-03-11  难为水
 * @Update: 2020-09-13  难为水  1. 更新防御相关函数;
 *                             2. 更新注释.
 */

#ifndef TARGET_H
#define TARGET_H


#include "Core/Global.h"

namespace JX3DPS {

class Target
{
public:
    Target(Player &player, const Id_t id, const int level, const Value_t shield, const Class classType);
    ~Target();

    Target(const Target &target);
    Target& operator=(const Target &target);

    /**
     * @brief 设置角色函数
     * @param player 角色.
     */
    void SetPlayer(Player &player);

    /* 获取下次作用时间 */
    Frame_t GetNextTime() const;

    /**
     * @brief 更新下次作用时间
     * @param frames 更新帧数
     */
    void UpdateTime(const Frame_t frames);

    /* ID */
    Id_t GetId() const;

    /* 等级 */
    int GetLevel() const;

    /* 生命值 */
    Pct_t GetLifePercent() const;
    void SetLifePercent(const Pct_t percent);
    void AddLifePercent(const Pct_t percent);
    
    /* 忽视防御 */
    BinPct_t GetShieldIgnoreBinPercent() const;
    void SetShieldIgnoreBinPercent(const BinPct_t binPercent);

    /* 外防 */
    Value_t GetPhysicsShield() const;
    void SetPhysicsShield(const Value_t value);
    void AddPhysicsShield(const Value_t value);
    BinPct_t GetPhysicsShieldBinPercent() const;
    void SetPhysicsShieldBinPercent(const BinPct_t binPercent);
    void AddPhysicsShieldBinPercent(const BinPct_t binPercent);
    Pct_t GetPhysicsResistPercent() const;

    /* 内防 */
    Value_t GetMagicShield() const;
    void SetMagicShield(const Value_t value);
    void AddMagicShield(const Value_t value);
    BinPct_t GetMagicShieldBinPercent() const;
    void SetMagicShieldBinPercent(const BinPct_t binPercent);
    void AddMagicShieldBinPercent(const BinPct_t binPercent);
    Pct_t GetMagicResistPercent() const;

    /* 易伤 */
    BinPct_t GetDamageAddBinPercent() const;
    void SetDamageAddBinPercent(const BinPct_t binPercent);
    void AddDamageAddBinPercent(const BinPct_t binPercent);
    Pct_t GetDamageAddPercent() const;

public_var:
    /* Buff */
    Buffs buffs;

private:
    /* 外功免伤 */
    void UpdatePhysicsResistPercent();

    /* 内功免伤 */
    void UpdateMagicResistPercent();

    /* 易伤 */
    void UpdateDamageAddPercent();

private_var: 
    /* ID */
    Id_t m_id = 0;

    /* 等级 */
    int m_level = 0;

    /* 生命值 */
    Pct_t m_lifePercent = 0.0;

    /* 忽视防御 */
    BinPct_t m_shieldIgnoreBinPercent = 0;

    /* 外防 */
    Value_t m_physicsShield = 0;
    BinPct_t m_physicsShieldBinPercent = 0;
    Pct_t m_physicsResistPercent = 0.0;

    /* 内防 */
    Value_t m_magicShield = 0;
    BinPct_t m_magicShieldBinPercent = 0;
    Pct_t m_magicResistPercent = 0.0;

    /* 易伤 */
    BinPct_t m_damageAddBinPercent = 0;
    Pct_t m_damageAddPercent = 0.0;

};

}

#endif // TARGET_H
