#pragma once

#include <array>

#include <QString>

namespace desktop {
enum class GainComparisonMode
{
    SAME_VALUE,
    SAME_COST
};

inline QString GainComparisonName(GainComparisonMode mode)
{
    return mode == GainComparisonMode::SAME_COST ? QStringLiteral("同属性费用") : QStringLiteral("同数值");
}

inline std::array<int, 8> GainComparisonIncrements(GainComparisonMode mode, bool magical)
{
    if (mode == GainComparisonMode::SAME_VALUE) {
        return { 100, 100, 100, 100, 100, 100, 100, 100 };
    }
    // Level-120 budget samples linked by JX3BOX bps/71692, IcyTide/Simulator base/constant.py.
    // These are a common comparison budget, not one equipment-score point.
    return { magical ? 475 : 398, 883, 883, 883, 883, 883, 883, 597 };
}

inline QString GainComparisonDescription(GainComparisonMode mode)
{
    return mode == GainComparisonMode::SAME_COST
               ? QStringLiteral("同属性费用：按 120 级模型的同预算参考增量分别计算，外功基础攻击 +398、内功基础攻击 "
                                "+475、会心/会效/破防/无双/破招/加速等级 +883、武器伤害 +597。此预算对应参考主属性 "
                                "+198，不是 1 装分。来源：JX3BOX 文章 71692 所链接的模拟器。")
               : QStringLiteral("同数值：每项基础数值或属性等级分别增加 100 点，不是增加 100% 面板属性。");
}
} // namespace desktop
