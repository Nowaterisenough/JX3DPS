/**
 * Project: JX3DPS
 * File: TaiXuJianYiBuff.h
 * Description:
 * Created Date: 2023-07-28 20:55:52
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-10 15:13:56
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_CLASS_TAI_XU_JIAN_YI_BUFF_H__
#define __JX3DPS_CLASS_TAI_XU_JIAN_YI_BUFF_H__

#include "Buff.h"
#include "TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

namespace Buff {

class DieRen : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(DieRen)

public:
    void TriggerAdd(Id_t targetId, int stackNum);
    void TriggerQieYu(Id_t targetId);
    void TriggerHuanYue(Id_t targetId);

    void SubEffect(Id_t targetId, int stackNum);
    void SubEffectQieYun(Id_t targetId, int stackNum, int effectCount);
    void SubEffectAdd(Id_t targetId);
    void SubEffectClear(Id_t targetId);
};

class WanXiangGuiYuan : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(WanXiangGuiYuan)

public:
    void TriggerAdd(Id_t targetId, int stackNum);
    void SubEffect(Id_t targetId, int stackNum);
};

class RenJianHeYi : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(RenJianHeYi)

public:
    void TriggerAdd(Id_t targetId);
    void SubEffect(Id_t targetId);
};

class ZiQiDongLai : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(ZiQiDongLai)

public:
    void TriggerAdd();
    void SubEffectAdd();
    void SubEffectClear();
};

class XuanMen : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(XuanMen)

public:
    void TriggerAdd(int stackNum);
    void SubEffectAdd(int stackNum);
    void SubEffectClear(int stackNum);
};

class FieldSuiXingChen : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(FieldSuiXingChen)

public:
    Frame_t GetDurationCurrent(Id_t targetId = PLAYER_ID) const override;
    void    TriggerAdd(int stackNum);
    void    SubEffect();
    void    SubEffectAdd(int stackNum = 1);
    void    SubEffectClear(int stackNum = 1);
};

class FieldShengTaiJi : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(FieldShengTaiJi)

public:
    Frame_t GetDurationCurrent(Id_t targetId = PLAYER_ID) const override;
    void    TriggerAdd(int stackNum);
    void    SubEffect();
    void    SubEffectAdd(int stackNum = 1);
};

class FieldTunRiYue : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(FieldTunRiYue)

public:
    Frame_t GetDurationCurrent(Id_t targetId = PLAYER_ID) const override;
    void    TriggerAdd(int stackNum);
    void    SubEffect();
    void    SubEffectAdd(int stackNum = 1);
    void    SubEffectClear(int stackNum = 1);
};

class FieldSuiXingChenQiSheng : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(FieldSuiXingChenQiSheng)

public:
    void TriggerAdd(int stackNum);
    void SubEffect();
    void SubEffectClear(int stackNum = 1);
};

class SuiXingChen : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(SuiXingChen)

public:
    void TriggerAdd();
    void SubEffectAdd();
    void SubEffectClear();
};

class QiSheng : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(QiSheng)

public:
    void TriggerAdd();
    void SubEffectAdd();
    void SubEffectClear();
};

class JianRu : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(JianRu)

public:
    void TriggerAdd();
    void TriggerActive();
    bool IsActived();
    void TriggerDamage(int index);
};

class FengShi : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(FengShi)

public:
    void TriggerAdd();
    void TriggerClear();

    void SubEffectAdd();
    void SubEffectClear();
};

class FieldLieYun : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(FieldLieYun)

public:
    void TriggerAdd(Id_t targetId);
    void SubEffectAdd(Id_t targetId);
};

class LieYun : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(LieYun)

public:
    void TriggerAdd();
    void SubEffectAdd();
    void SubEffectClear();
};

class TunRiYue : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(TunRiYue)

public:
    void TriggerAdd(Id_t targetId);
    void SubEffectAdd();
};

class JingHuaYing : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(JingHuaYing)

public:
    void TriggerAdd();
    void SubEffectAdd();
    void SubEffectClear();
};

class ChiYing : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(ChiYing)

public:
    void TriggerAdd(int stackNum);
    void TriggerDamage();
};

class YunZhongJianSuiXingChen : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(YunZhongJianSuiXingChen)

public:
    void TriggerAdd();
    void SubEffect();
};

class YunZhongJianShengTaiJi : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(YunZhongJianShengTaiJi)

public:
    void TriggerAdd();
    void SubEffect();
};

class YunZhongJianTunRiYue : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(YunZhongJianTunRiYue)

public:
    void TriggerAdd();
    void SubEffect();
};

class ClassFeature : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(ClassFeature)

public:
    void SubEffectAdd();
};

class WeaponEffectCW : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(WeaponEffectCW)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

class SetAttribute : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(SetAttribute)

public:
    void TriggerAdd();

    void SubEffectAdd();
    void SubEffectClear();
};

class TeamCoreTaiXuJianYiYouRen : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(TeamCoreTaiXuJianYiYouRen)

public:
    void TriggerAdd();

    void SubEffectAdd(int stackNum);
    void SubEffectClear(int stackNum);
};

class TeamCoreTaiXuJianYiJingMiao : public JX3DPS::Buff
{
    BUFF_DEFAULT_FUNCTION(TeamCoreTaiXuJianYiJingMiao)

public:
    void SubEffect();
};

} // namespace Buff

} // namespace TaiXuJianYi

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_TAI_XU_JIAN_YI_BUFF_H__