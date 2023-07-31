/**
 * Project: JX3DPS
 * File: TaiXuJianYiSkill.h
 * Description:
 * Created Date: 2023-07-28 12:49:46
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-01 01:41:18
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
    void TriggerDamage(Id_t targetId, int sub, int level);
};

class Gong : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(Gong)

public:
    void SubEffect();
};

class BianGong : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(BianGong)

public:
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

class BianZhi : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(BianZhi)

public:
    void SubEffect();
};

class Zhi : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(Zhi)

public:
    void SubEffect();
};

class Yu : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(Yu)

public:
    void SubEffect();
};

class GaoShanLiuShui : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(GaoShanLiuShui)

public:
    void SubEffect();
};

class YangChunBaiXue : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(YangChunBaiXue)

public:
    void SubEffect();
};

class ShuYingHengXie : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ShuYingHengXie)

public:
    void SubEffect();
};

class ShuYingHuaShuang : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ShuYingHuaShuang)

public:
    void SubEffect();
};

class ZhengLvHeMing : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(ZhengLvHeMing)

public:
    void SubEffect();
};

} // namespace Skill

} // namespace MoWen

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_MO_WEN_SKILL_H__