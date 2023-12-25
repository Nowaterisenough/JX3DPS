/**
 * Project: JX3DPS
 * File: TaiXuJianYiSkill.h
 * Description:
 * Created Date: 2023-07-28 12:49:46
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-13 10:03:50
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

namespace Skill {

class PoZhao : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(PoZhao)

public:
    void TriggerDamage(Id_t targetId, int sub, int level);
};

class WuWoWuJian : public JX3DPS::Skill
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

class BaHuangGuiYuan : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(BaHuangGuiYuan)

public:
void TriggerDamage();
    void SubEffect();

    void ResetCooldown();
    void ClearCooldown();
};

class SanHuanTaoYue : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(SanHuanTaoYue)

public:
    void SubEffect();

    int m_index = 0;
};

class WanJianGuiZong : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(WanJianGuiZong)

public:
    void SubEffect();
};

class RenJianHeYi : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(RenJianHeYi)

public:
    void SubEffect();
};

class RenJianHeYiSuiXingChen : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(RenJianHeYiSuiXingChen)

public:
    void SubEffect();
};

class SanChaiJianFa : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(SanChaiJianFa)

public:
    void SubEffect();
};

class SuiXingChen : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(SuiXingChen)

public:
    void SubEffect();
};

class ShengTaiJi : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ShengTaiJi)

public:
    void SubEffect();
};

class TunRiYue : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(TunRiYue)

public:
    void SubEffect();
};

class ZiQiDongLai : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ZiQiDongLai)

public:
    void SubEffect();
};

class JingHuaYing : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(JingHuaYing)

public:
    void TriggerAddJingHuaYing();
    void TriggerClearJingHuaYing();

    void SubEffect();
    void SubEffectSui();
};

class PendantOvercome : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(PendantOvercome)

public:
    void SubEffect();
};

} // namespace Skill

} // namespace TaiXuJianYi

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILL_H__