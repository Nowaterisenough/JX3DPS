#include <proxy,h>

#include "Context.h"

namespace JX3DPS {

class SkillNotinCdProxy
{
public:
    bool operator()(id_t skill_id) const { return context.player.GetSkillCooldown(skill_id) == 0; }
};

class BuffExistProxy
{
public:
    bool operator()(id_t buff_id) const { return context.player.HasBuff(buff_id); }
};

} // namespace JX3DPS

namespace spec {

PRO_DEF_MEM_DISPATCH(MemEvaluate, operator());

template <typename Ret, typename... Args>
struct Evaluatable : pro::facade_builder::add_convention<MemEvaluate, Ret(Args...) const>::build
{
};

} // namespace spec

auto SkillNotinCd = pro::make_proxy<spec::Evaluatable<bool, id_t>>(SkillNotinCdProxy());
auto BuffExist    = pro::make_proxy<spec::Evaluatable<bool, id_t>>(BuffExistProxy());

extern decltype(pro::make_proxy<spec::Evaluatable<bool, id_t>>(SkillNotinCdProxy())) SkillNotinCd;
extern decltype(pro::make_proxy<spec::Evaluatable<bool, id_t>>(BuffExistProxy()))    BuffExist;