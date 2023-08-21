/**
 * Project: JX3DPS
 * File: Buff3rd.h
 * Description:
 * Created Date: 2023-08-19 12:39:42
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-20 16:05:01
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

} // namespace Buff3rd

} // namespace JX3DPS