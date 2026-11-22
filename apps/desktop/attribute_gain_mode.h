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

inline std::array<int, 8> GainComparisonIncrements(GainComparisonMode mode, bool magical, bool historical = false)
{
    if (mode == GainComparisonMode::SAME_VALUE) {
        return { 100, 100, 100, 100, 100, 100, 100, 100 };
    }
    // Same grade and slot: level-six five-colour stone third effect, Cangsheng.
    if (!historical) {
        return { 208, 208, 208, 208, 208, 208, 208, 35 };
    }
    // Level-120 budget samples linked by JX3BOX bps/71692, IcyTide/Simulator base/constant.py.
    // These are a common comparison budget, not one equipment-score point.
    return { magical ? 475 : 398, 883, 883, 883, 883, 883, 883, 597 };
}

inline QString GainComparisonDescription(GainComparisonMode mode)
{
    return mode == GainComparisonMode::SAME_COST
               ? QStringLiteral("同属性费用：按本赛季六级五彩石第三属性的同档参考值，攻击和属性等级 +208、武器伤害 "
                                "+35；破招按同档等级增量比较。")
               : QStringLiteral("同数值：每项基础数值或属性等级分别增加 100 点，不是增加 100% 面板属性。");
}
} // namespace desktop
