/**
 * @Description : 目标类
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 13:44:40
 * @FilePath    : \JX3DPS\modules\Core\Target.h
 */

#ifndef TARGET_H
#define TARGET_H

#include "Common/ConstVal.h"

namespace JX3DPS {

class Target
{
public:
    Target(Player &player, Id_t id, int level, Value_t shield);
    ~Target();

    Target(const Target &target);
    Target &operator=(const Target &target);

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
    PCT_t GetLifePercent() const;
    void  SetLifePercent(PCT_t percent);
    void  AddLifePercent(PCT_t percent);

    /* 忽视防御 */
    BinPCT_t GetShieldIgnoreBinPercent() const;
    void     SetShieldIgnoreBinPercent(BinPCT_t binPercent);

    /* 外防 */
    Value_t  GetPhysicsShield() const;
    void     SetPhysicsShield(Value_t value);
    void     AddPhysicsShield(Value_t value);
    BinPCT_t GetPhysicsShieldBinPercent() const;
    void     SetPhysicsShieldBinPercent(BinPCT_t binPercent);
    void     AddPhysicsShieldBinPercent(BinPCT_t binPercent);
    PCT_t    GetPhysicsResistPercent() const;

    /* 内防 */
    Value_t  GetMagicShield() const;
    void     SetMagicShield(Value_t value);
    void     AddMagicShield(Value_t value);
    BinPCT_t GetMagicShieldBinPercent() const;
    void     SetMagicShieldBinPercent(BinPCT_t binPercent);
    void     AddMagicShieldBinPercent(BinPCT_t binPercent);
    PCT_t    GetMagicResistPercent() const;

    /* 易伤 */
    BinPCT_t GetDamageAddBinPercent() const;
    void     SetDamageAddBinPercent(BinPCT_t binPercent);
    void     AddDamageAddBinPercent(BinPCT_t binPercent);
    PCT_t    GetDamageAddPercent() const;

    /* Buff */
    Buffs buffs;

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

    /* 生命值 */
    PCT_t m_lifePercent = 0.0;

    /* 忽视防御 */
    BinPCT_t m_shieldIgnoreBinPercent = 0;

    /* 外防 */
    Value_t  m_physicsShield           = 0;
    BinPCT_t m_physicsShieldBinPercent = 0;
    PCT_t    m_physicsResistPercent    = 0.0;

    /* 内防 */
    Value_t  m_magicShield           = 0;
    BinPCT_t m_magicShieldBinPercent = 0;
    PCT_t    m_magicResistPercent    = 0.0;

    /* 易伤 */
    BinPCT_t m_damageAddBinPercent = 0;
    PCT_t    m_damageAddPercent    = 0.0;
};

} // namespace JX3DPS

#endif // TARGET_H
