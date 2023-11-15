#include "ZiXiaGongSkill.h"

#include "Buff.h"
#include "ZiXiaGongBuff.h"
#include "Target.hpp"
#include "TimeLine.hpp"

namespace JX3DPS {

namespace ZiXiaGong {

namespace Skill {

PoZhao::PoZhao(JX3DPS::Player *player, Targets *targets) : Skill(player, targets)
{
    m_id    = SKILL_PO_ZHAO;
    m_name  = "破招";
    m_range = 4;

    m_damageParams[0].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.065 - 1));
    m_damageParams[0].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.13 - 1));
    m_damageParams[0].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.23 - 1));

    m_damageParams[1].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.065 - 1));
    m_damageParams[1].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.065 - 1));
    m_damageParams[1].emplace_back(0, 0, JX3_PERCENT_INT_BASE * JX3_PERCENT_INT_BASE * (0.065 - 1));
}

void PoZhao::Cast() { }

void PoZhao::Trigger() { }

void PoZhao::TriggerDamage(Id_t targetId, int sub, int level)
{
    RollResult  rollResult = GetMagicRollResult();
    GainsDamage damage     = CalcMagicSurplusDamage(targetId, rollResult, sub, level);
    Record(m_id, targetId, rollResult, damage, sub, level);
}

} // namespace Skill

} // namespace ZiXiaGong

} // namespace JX3DPS