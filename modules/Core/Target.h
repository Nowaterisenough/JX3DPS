/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:07:17
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 13:16:23
 * @FilePath    : \JX3DPS\modules\Core\Target.h
 */

#ifndef TARGET_H
#define TARGET_H

#include <map>
#include <list>

#include "Core/Global.h"

namespace JX3DPS {

class Buff;

class Target
{
public:
    Target(Id_t id, int level, Value_t shield, Pct_t missPercent, Pct_t sightPercent, Class classType);
    ~Target();

    /* 基础信息 */
    Id_t  GetId();
    int   GetLevel();
    Pct_t GetMissPercent();
    Pct_t GetSightPercent();

    /* 生命值 */
    BinPct_t SetLifeBinPercent(BinPct_t binPercent);
    BinPct_t GetLifeBinPercent();
    Pct_t    GetLifePercent();

    /* 忽视防御 */
    BinPct_t SetShieldIgnoreBinPercent(BinPct_t binPercent);

    /* 外防 */
    Value_t  AddPhysicsShield(Value_t value);
    BinPct_t AddPhysicsShieldBinPercent(BinPct_t binPercent);
    Pct_t    GetPhysicsResistPercent();

    /* 内防 */
    Value_t  AddMagicShield(Value_t value);
    BinPct_t AddMagicShieldBinPercent(BinPct_t binPercent);
    Pct_t    GetMagicResistPercent();

    /* 易伤 */
    BinPct_t AddDamageBinPercent(BinPct_t binPercent);
    Pct_t    GetDamagePercentAdd();

    /* Buff */
    std::map<Id_t, Buff *> m_buffMap;

private:
    /* 初始化心法TBuff */
    void InitClassTBuff(Class classType);

    /* 生命值 */
    void UpdateLife();

    /* 外功免伤 */
    void UpdatePhysicsResistPercent();

    /* 内功免伤 */
    void UpdateMagicResistPercent();

    /* 易伤 */
    void UpdateDamagePercentAdd();

    /* 基础信息 */
    Id_t  m_id;
    int   m_level;
    Pct_t m_missPercent;
    Pct_t m_sightPercent;

    /* 生命值 */
    BinPct_t m_lifeBinPercent;
    Pct_t    m_lifePercent;

    /* 忽视防御 */
    BinPct_t m_shieldIgnoreBinPercent;

    /* 外防 */
    Value_t  m_physicsShield;
    BinPct_t m_physicsShieldBinPercent;
    Pct_t    m_physicsResistPercent;

    /* 内防 */
    Value_t  m_magicShield;
    BinPct_t m_magicShieldBinPercent;
    Pct_t    m_magicResistPercent;

    /* 易伤 */
    BinPct_t m_damageBinPercent;
    Pct_t    m_damagePercentAdd;
};

} // namespace JX3DPS

#endif // TARGET_H
