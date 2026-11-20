#pragma once

#include "src/global/types.h"
#include <limits>
#include <stdexcept>

namespace JX3DPS::runtime {
struct SkillTotals
{
    value_t damage = 0;
    std::uint64_t   count = 0, hit = 0, critical = 0, insight = 0, miss = 0;

    void Add(value_t value, RollResult outcome)
    {
        if (value < 0 || damage > std::numeric_limits<value_t>::max() - value) {
            throw std::overflow_error("skill damage statistics overflow");
        }
        damage += value;
        ++count;
        if (outcome == RollResult::HIT) {
            ++hit;
        } else if (outcome == RollResult::DOUBLE) {
            ++critical;
        } else if (outcome == RollResult::INSIGHT) {
            ++insight;
        } else {
            ++miss;
        }
    }

    void Merge(const SkillTotals &other)
    {
        if (damage > std::numeric_limits<value_t>::max() - other.damage) {
            throw std::overflow_error("skill damage statistics overflow");
        }
        damage   += other.damage;
        count    += other.count;
        hit      += other.hit;
        critical += other.critical;
        insight  += other.insight;
        miss     += other.miss;
    }
};

} // namespace JX3DPS::runtime
