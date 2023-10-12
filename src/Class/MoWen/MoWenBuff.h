/**
 * Project: JX3DPS
 * File: MoWenBuff.h
 * Description:
 * Created Date: 2023-08-01 00:55:43
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-12 14:54:37
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_CLASS_MO_WEN_BUFF_H__
#define __JX3DPS_CLASS_MO_WEN_BUFF_H__

#include "Buff.h"
#include "MoWen.h"

namespace JX3DPS {

namespace MoWen {

namespace Buff {

class Shang : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(Shang)

public:
    void TriggerAdd(Id_t targetId);

    void SubEffect(Id_t targetId);
};

class Jue : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(Jue)

public:
    void TriggerAdd(Id_t targetId);

    void SubEffect(Id_t targetId);
};

class XianFeng : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(XianFeng)

public:
    void TriggerAdd();
    void TriggerClear();
    void TriggerDamage(Id_t targetId, int stackNum);
};

class XianFengBiaoJi : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(XianFengBiaoJi)

public:
    void TriggerAdd();
};

class YunHan : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(YunHan)

public:
    void TriggerAdd(int stackNum);

    void SubEffectAdd(int stackNum);
};

class CanLian : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(CanLian)

public:
    void TriggerAdd();
    void TriggerClear();

    void SubEffectAdd();
    void SubEffectClear();
};

class MingJin : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(MingJin)

public:
    void TriggerAdd();
    void TriggerClear();

    void SubEffectAdd();
    void SubEffectClear();
};

class LiuZhao : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(LiuZhao)

public:
    void TriggerAdd(int stackNum);
    void TriggerDamage(int stackNum);
    void TriggerSurplusDamage();

    Damage GetMagicSurplusDamage(
        Id_t       targetId,
        RollResult rollResult,
        int        sub,
        int        level,
        Value_t    surplus,
        Value_t    criticalStrikePower,
        Value_t    overcome,
        Value_t    strain);

    GainsDamage CalcMagicSurplusDamage(Id_t targetId, RollResult rollResult, int sub = 0, int level = 0);

    void SurplusRecord(Id_t               targetId,
                       RollResult         rollResult  = RollResult::HIT,
                       const GainsDamage &gainsDamage = GainsDamage(),
                       int                sub         = 0,
                       int                level       = 0);
};

class QuFeng : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(QuFeng)

public:
    void TriggerAdd(int stackNum);
    void TriggerClear();
    void TriggerSet(int stackNum);

    void SubEffect();
};

class GaoShanLiuShui : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(GaoShanLiuShui)

public:
    void TriggerAdd();
    void TriggerClear();

    void SubEffectAdd();
    void SubEffectClear();
};

class YangChunBaiXue : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(YangChunBaiXue)

public:
    void TriggerAdd();
    void TriggerClear();

    void SubEffectAdd(int stackNum);
    void SubEffectClear(int stackNum);
};

class ZhiYinMiaoYi : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(ZhiYinMiaoYi)

public:
    void TriggerAdd(int stackNum);
    void TriggerClear();

    void SubEffectAdd(int stackNum);
    void SubEffectClear(int stackNum);
};

class GuYingHuaShuang : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(GuYingHuaShuang)

public:
    void TriggerAdd();
    void TriggerClear();

    void SubEffectClear();
};

class ZhengLvHeMing : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(ZhengLvHeMing)

public:
    void TriggerAdd();
    void TriggerClear();

    void SubEffectAdd();
    void SubEffectClear();
};

class ZhiYinHeMing : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(ZhiYinHeMing)

public:
    void TriggerAdd();
    void TriggerClear();

    void SubEffect();
    void SubEffectDamage();

    void RecordDamage(Id_t targetId, RollResult rollResult, const GainsDamage &gainsDamage, int sub = 0);
    
    int level = 1;

    std::vector<int> randoms = { 2, 3, 5, 6 };

    bool lastIsGong = false;
};

class YingZi : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(YingZi)

public:
    void TriggerAdd(int count = 1);
    void TriggerClear();

    void SubEffect(Id_t id);
    void SubEffectClear(int stackNum);

    Id_t            id = TARGET_PLACE_HOLDERS_END;
    std::list<Id_t> ids;

    Damage GetMagicYingZiDamage(
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

    GainsDamage CalcMagicYingZiDamage(Id_t targetId, RollResult rollResult, int sub = 0, int level = 0, int effectCount = 1);

    void RecordYingZi(Id_t               targetId,
                      RollResult         rollResult  = RollResult::HIT,
                      const GainsDamage &gainsDamage = GainsDamage(),
                      int                sub         = 0,
                      int                level       = 0);
};

class SetAttribute : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(SetAttribute)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

class WeaponEffectCW : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(WeaponEffectCW)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

class TeamCoreMoWenYouRen : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(TeamCoreMoWenYouRen)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

class TeamCoreMoWenJingMiao : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(TeamCoreMoWenJingMiao)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class ShenBingGong : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(ShenBingGong)

public:
    void TriggerAdd(Id_t targetId, int stackNum);
    void SubEffect(Id_t targetId, int stackNum);
};

} // namespace Buff

} // namespace MoWen

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_MO_WEN_BUFF_H__