#ifndef DESKTOP_RUNTIME_BACKEND_H
#define DESKTOP_RUNTIME_BACKEND_H

#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/attribute_gains.hpp"

#include "runtime_catalog.h"

namespace desktop {
using rt::ApplyAttributeGain;
using rt::AttributeGainKind;
using rt::AttributeGains;
using rt::DamageOnlyGainIndexes;
using rt::RotationGainIndexes;

template <class Config>
auto MakeRules(const Config &config)
{
    return rt::LoadSpecialization(config);
}

inline QString AttributeGainName(std::size_t index)
{
    static const std::array<QString, rt::AttributeGains.size()> names{
        QStringLiteral("基础攻击"), QStringLiteral("会心等级"), QStringLiteral("会效等级"), QStringLiteral("破防等级"),
        QStringLiteral("无双等级"), QStringLiteral("破招等级"), QStringLiteral("加速等级"), QStringLiteral("武器伤害")
    };
    return names.at(index);
}
} // namespace desktop
#endif
