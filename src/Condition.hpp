#ifndef JX3DPS_CONDITION_HPP
#define JX3DPS_CONDITION_HPP

#include <proxy,h>

#include "Context.h"

namespace JX3DPS {

class SkillNotinCdProxy
{
public:
    bool operator()(jx3id_t skill_id) const { return context.GetSkillCooldown(skill_id) == 0; }
};

class BuffExistProxy
{
public:
    bool operator()(jx3id_t buff_id) const { return context.HasBuff(buff_id); }
};

class NoBuffExistProxy
{
public:
    bool operator()(jx3id_t buff_id) const { return !context.HasBuff(buff_id); }
};

class TBuffExistProxy
{
public:
    bool operator()(jx3id_t buff_id) const { return context.HasTargetBuff(buff_id); }
};

class TNoBuffExistProxy
{
public:
    bool operator()(jx3id_t buff_id) const { return !context.HasTargetBuff(buff_id); }
};

class LastCastSkillProxy
{
public:
    bool operator()(jx3id_t skill_id) const { return context.GetLastCastSkill() == skill_id; }
};

class NotLastCastSkillProxy
{
public:
    bool operator()(jx3id_t skill_id) const { return context.GetLastCastSkill() != skill_id; }
};

class BuffStackNumLtProxy
{
public:
    bool operator()(jx3id_t buff_id, int stack_num) const
    {
        return context.GetBuffStackNum(buff_id) < stack_num;
    }
};

class BuffStackNumLeProxy
{
public:
    bool operator()(jx3id_t buff_id, int stack_num) const
    {
        return context.GetBuffStackNum(buff_id) <= stack_num;
    }
};

class BuffStackNumEqProxy
{
public:
    bool operator()(jx3id_t buff_id, int stack_num) const
    {
        return context.GetBuffStackNum(buff_id) == stack_num;
    }
};

class BuffStackNumNeProxy
{
public:
    bool operator()(jx3id_t buff_id, int stack_num) const
    {
        return context.GetBuffStackNum(buff_id) != stack_num;
    }
};

class BuffStackNumGeProxy
{
public:
    bool operator()(jx3id_t buff_id, int stack_num) const
    {
        return context.GetBuffStackNum(buff_id) > stack_num;
    }
};

class BuffStackNumGtProxy
{
public:
    bool operator()(jx3id_t buff_id, int stack_num) const
    {
        return context.GetBuffStackNum(buff_id) >= stack_num;
    }
};

} // namespace JX3DPS

namespace spec {

PRO_DEF_MEM_DISPATCH(MemEvaluate, operator());

template <typename Ret, typename... Args>
struct Evaluator : pro::facade_builder::add_convention<MemEvaluate, Ret(Args...) const>::build
{
};

} // namespace spec

auto SkillNotinCd = pro::make_proxy<spec::Evaluator<bool, jx3id_t>>(SkillNotinCdProxy());
auto BuffExist    = pro::make_proxy<spec::Evaluator<bool, jx3id_t>>(BuffExistProxy());

extern decltype(pro::make_proxy<spec::Evaluator<bool, jx3id_t>>(SkillNotinCdProxy())) SkillNotinCd;
extern decltype(pro::make_proxy<spec::Evaluator<bool, jx3id_t>>(BuffExistProxy()))    BuffExist;

#endif // JX3DPS_CONDITION_HPP