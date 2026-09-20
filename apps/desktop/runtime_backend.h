#ifndef DESKTOP_RUNTIME_BACKEND_H
#define DESKTOP_RUNTIME_BACKEND_H
#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"

#include "runtime_catalog.h"

namespace desktop {
// Dispatch once at the desktop boundary; each fight retains its concrete rules type.
inline auto MakeRules(const rt::tai_xu::Config &config)
{
    return rt::tai_xu::Rules(rt::tai_xu::Prepare(config));
}

inline auto MakeRules(const rt::mo_wen::Config &config)
{
    return rt::mo_wen::Rules(rt::mo_wen::Prepare(config));
}
} // namespace desktop
#endif
