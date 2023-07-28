/**
 * Project: JX3DPS
 * File: TaiXuJianYiSkill.h
 * Description:
 * Created Date: 2023-07-28 12:49:46
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-28 20:49:44
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILL_H__
#define __JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILL_H__

#include "Skill.h"
#include "TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class PoZhao : public Skill
{
    SKILL_DEFAULT_FUNCTION(PoZhao)

public:
    void TriggerDamage(Id_t targetId, int sub, int level);
};

class WuWoWuJian : public Skill
{
    SKILL_DEFAULT_FUNCTION(WuWoWuJian)

public:
    void TriggerWuYi(const Params &params);
    void TriggerWuYiClear(const Params &params);

    void TriggerWuYu(const Params &params);

    void TriggerFengShiAdd();
    void TriggerFengShiClear();

    void SubEffect();
};

class BaHuangGuiYuan : public Skill
{
    SKILL_DEFAULT_FUNCTION(BaHuangGuiYuan)

public:
    void SubEffect();

    void ResetCooldown();
    void ClearCooldown();
};

class SanHuanTaoYue : public Skill
{
    SKILL_DEFAULT_FUNCTION(SanHuanTaoYue)

public:
    void SubEffect();

    int m_index = 0;
};

class WanJianGuiZong : public Skill
{
    SKILL_DEFAULT_FUNCTION(WanJianGuiZong)

public:
    void SubEffect();
};

class RenJianHeYi : public Skill
{
    SKILL_DEFAULT_FUNCTION(RenJianHeYi)

public:
    void SubEffect();
};

class RenJianHeYiSuiXingChen : public Skill
{
    SKILL_DEFAULT_FUNCTION(RenJianHeYiSuiXingChen)

public:
    void SubEffect();
};

class RenJianHeYiTunRiYue : public Skill
{
    SKILL_DEFAULT_FUNCTION(RenJianHeYiTunRiYue)

public:
    void SubEffect();
};

class SanChaiJianFa : public Skill
{
    SKILL_DEFAULT_FUNCTION(SanChaiJianFa)

public:
    void SubEffect();
};

class SuiXingChen : public Skill
{
    SKILL_DEFAULT_FUNCTION(SuiXingChen)

public:
    void SubEffect();
};

class ShengTaiJi : public Skill
{
    SKILL_DEFAULT_FUNCTION(ShengTaiJi)

public:
    void SubEffect();
};

class TunRiYue : public Skill
{
    SKILL_DEFAULT_FUNCTION(TunRiYue)

public:
    void SubEffect();
};

class ZiQiDongLai : public Skill
{
    SKILL_DEFAULT_FUNCTION(ZiQiDongLai)

public:
    void SubEffect();
};

class JingHuaYing : public Skill
{
    SKILL_DEFAULT_FUNCTION(JingHuaYing)

public:
    void TriggerAddJingHuaYing();
    void TriggerClearJingHuaYing();

    void SubEffect();
    void SubEffectSui();
};

} // namespace TaiXuJianYi

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILL_H__