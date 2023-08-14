/**
 * Project: JX3DPS
 * File: TaiXuJianYiSkill.h
 * Description:
 * Created Date: 2023-07-28 12:49:46
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-15 03:55:24
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_CLASS_MO_WEN_SKILL_H__
#define __JX3DPS_CLASS_MO_WEN_SKILL_H__

#include "MoWen.h"
#include "Skill.h"

namespace JX3DPS {

namespace MoWen {

namespace Skill {

class PoZhao : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(PoZhao)

public:
    void TriggerDamage(Id_t targetId);
};

class Gong : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(Gong)

public:
    bool IsReady(bool fcast) override;
    void Sync();
    
    void SubEffect();
};

class BianGong : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(BianGong)

public:
    bool IsReady(bool fcast) override;
    void Sync();

    void SubEffect();
};

class Shang : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(Shang)

public:
    void SubEffect();
};

class Jue : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(Jue)

public:
    void SubEffect();
};

class Zhi : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(Zhi)

public:
    void Stop() override;
    bool IsReady(bool fcast) override;
    void Sync();
    
    void TriggerShiXiang();

    void SubEffect();

    int index = 0;
};

class BianZhi : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(BianZhi)

public:
    void Stop() override;
    bool IsReady(bool fcast) override;
    void Sync();
    
    void TriggerShiXiang();

    void SubEffect();

    int index = 0;
};

class Yu : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(Yu)

public:
    void TriggerShiXiang();

    void SubEffect();
};

class GaoShanLiuShui : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(GaoShanLiuShui)

public:
    void SetCooldown();
    void SubEffect();
};

class YangChunBaiXue : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(YangChunBaiXue)

public:
    bool IsReady(bool fcast) override;
    bool StopReCastSkill() override;

    void SetCooldown();

    void SubEffect();
};

class ShuYingHengXie : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ShuYingHengXie)

public:
    void SubEffect();

    int percent = 20;
};

class ShuYingHuaShuang : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ShuYingHuaShuang)

public:
    void SubEffect();
};

class ZhengLvHeMing1 : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ZhengLvHeMing1)

public:
    void SubEffect();
};

class ZhengLvHeMing2 : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ZhengLvHeMing2)

public:
    void SubEffect();
};

class ZhengLvHeMing3 : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ZhengLvHeMing3)

public:
    void SubEffect();
};

class YiXingHuanYing : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(YiXingHuanYing)

public:
    bool IsReady(bool fcast) override;
    void SubEffect();
};

} // namespace Skill

} // namespace MoWen

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_MO_WEN_SKILL_H__