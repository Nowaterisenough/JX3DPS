/**
 * Project: JX3DPS
 * File: Buff3rd.h
 * Description:
 * Created Date: 2023-08-19 12:39:42
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-16 23:23:14
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#pragma once

#include "Buff.h"

namespace JX3DPS {

namespace Buff3rd {

class EnchantShoesPhysics : public Buff
{
    BUFF_DEFAULT_FUNCTION(EnchantShoesPhysics)

public:
    void TriggerDamage();
    void SubEffect();
};

class EnchantWristPhysics : public Buff
{
    BUFF_DEFAULT_FUNCTION(EnchantWristPhysics)

public:
    void TriggerDamage();
    void SubEffect();
};

class EnchantShoesMagic : public Buff
{
    BUFF_DEFAULT_FUNCTION(EnchantShoesMagic)

public:
    void TriggerDamage();
    void SubEffect();
};

class EnchantWristMagic : public Buff
{
    BUFF_DEFAULT_FUNCTION(EnchantWristMagic)

public:
    void TriggerDamage();
    void SubEffect();
};

class EnchantBelt : public Buff
{
    BUFF_DEFAULT_FUNCTION(EnchantBelt)

public:
    void TriggerAdd();
    void SubEffectAdd();
    void SubEffectClear();

private:
    bool m_70 = false;
};

class WeaponEffectWater : public Buff
{
    BUFF_DEFAULT_FUNCTION(WeaponEffectWater)

public:
    void TriggerAdd(int stackNum = 1);

    void SubEffectAdd(int stackNum);
    void SubEffectClear(int stackNum);
};

class JiaoSu : public Buff
{
    BUFF_DEFAULT_FUNCTION(JiaoSu)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class XiuQi : public Buff
{
    BUFF_DEFAULT_FUNCTION(XiuQi)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class HanRuLei : public Buff
{
    BUFF_DEFAULT_FUNCTION(HanRuLei)

public:
    void SubEffectAdd();
    void SubEffectClear();
};

class PoFeng : public Buff
{
    BUFF_DEFAULT_FUNCTION(PoFeng)

public:
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId);
};

class JingFeng : public Buff
{
    BUFF_DEFAULT_FUNCTION(JingFeng)

public:
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId);
};

class JieHuo : public Buff
{
    BUFF_DEFAULT_FUNCTION(JieHuo)

public:
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId);
};

class HaoLingSanJun : public Buff
{
    BUFF_DEFAULT_FUNCTION(HaoLingSanJun)

public:
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId, int stackNum);
};

class SheShenHongFa : public Buff
{
    BUFF_DEFAULT_FUNCTION(SheShenHongFa)

public:
    void SubEffectAdd(Id_t targetId, int stackNum);
    void SubEffectClear(Id_t targetId, int stackNum);
};

class ChaoSheng : public Buff
{
    BUFF_DEFAULT_FUNCTION(ChaoSheng)

public:
    void SubEffectAdd(Id_t targetId, int stackNum);
    void SubEffectClear(Id_t targetId, int stackNum);
};

class ShengYuMingXin : public Buff
{
    BUFF_DEFAULT_FUNCTION(ShengYuMingXin)

public:
    void SubEffectAdd(Id_t targetId, int stackNum);
    void SubEffectClear(Id_t targetId, int stackNum);
};

class ZhenFen : public Buff
{
    BUFF_DEFAULT_FUNCTION(ZhenFen)

public:
    void SubEffectAdd(Id_t targetId, int stackNum);
    void SubEffectClear(Id_t targetId, int stackNum);
};

class HanXiaoQianJun : public Buff
{
    BUFF_DEFAULT_FUNCTION(HanXiaoQianJun)

public:
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId);
};

class ShuKuang : public Buff
{
    BUFF_DEFAULT_FUNCTION(ShuKuang)

public:
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId);
};

class HanChangLinLi : public Buff
{
    BUFF_DEFAULT_FUNCTION(HanChangLinLi)

public:
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId);
};

class JiLei : public Buff
{
    BUFF_DEFAULT_FUNCTION(JiLei)

public:
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId);
};

class PendantOvercome : public Buff
{
    BUFF_DEFAULT_FUNCTION(PendantOvercome)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

class TeamCoreShanHaiXinJueYouRen : public Buff
{
    BUFF_DEFAULT_FUNCTION(TeamCoreShanHaiXinJueYouRen)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

class TeamCoreYinLongJueYouRen : public Buff
{
    BUFF_DEFAULT_FUNCTION(TeamCoreYinLongJueYouRen)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

} // namespace Buff3rd

} // namespace JX3DPS