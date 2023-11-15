#ifndef __JX3DPS_CLASS_ZI_XIA_GONG_SKILL_H__
#define __JX3DPS_CLASS_ZI_XIA_GONG_SKILL_H__

#include "Skill.h"
#include "ZiXiaGong.h"

namespace JX3DPS {

namespace ZiXiaGong {

namespace Skill {

class PoZhao : public JX3DPS::Skill
{
    SKILL_DEFAULT_FUNCTION(PoZhao)

public:
    void TriggerDamage(Id_t targetId, int sub, int level);
};




} // namespace Skill

} // namespace ZiXiaGong

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_ZI_XIA_GONG_SKILL_H__