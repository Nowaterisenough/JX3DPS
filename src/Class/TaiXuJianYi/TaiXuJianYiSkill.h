/**
 * Project: 
 * File: Skill.h
 * Description:
 * Created Date: 2023-07-24 13:57:40
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-27 21:04:42
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILL_H__
#define __JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILL_H__

#include "Skill.h"
#include "TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

namespace Skill {

class WuWoWuJian : public Skill
{
    SKILL_DEFAULT_FUNCTION(WuWoWuJian)

public:
    void SubEffect();
    void TriggerWuYi(const Params &params);
    void TriggerWuYiClear(const Params &params);
    void TriggerWuYu(const Params &params);
};

class BaHuangGuiYuan : public Skill
{
    SKILL_DEFAULT_FUNCTION(BaHuangGuiYuan)

public:
    void SubEffect();
};

} // namespace Skill

} // namespace TaiXuJianYi

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILL_H__