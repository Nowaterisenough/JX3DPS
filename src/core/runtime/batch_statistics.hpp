#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

#include "src/global/types.h"

namespace JX3DPS::runtime {

inline std::uint64_t IterationSeed(std::uint64_t base, std::uint64_t iteration)
{
    auto value = base + (0x9e3779b97f4a7c15ULL * (iteration + 1));
    value      = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
    value      = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31);
}

struct BatchStats
{
    std::uint64_t count       = 0;
    double        mean_damage = 0;
    double        m2_damage   = 0;
    value_t       min_damage  = std::numeric_limits<value_t>::max();
    value_t       max_damage  = 0;
    std::uint64_t checksum    = 0;

    void Add(value_t damage, std::uint64_t iteration)
    {
        ++count;
        const double DELTA  = static_cast<double>(damage) - mean_damage;
        mean_damage        += DELTA / static_cast<double>(count);
        m2_damage          += DELTA * (static_cast<double>(damage) - mean_damage);
        min_damage          = std::min(min_damage, damage);
        max_damage          = std::max(max_damage, damage);
        checksum           ^= IterationSeed(static_cast<std::uint64_t>(damage), iteration);
    }

    void Merge(const BatchStats &other)
    {
        if (other.count == 0) {
            return;
        }
        if (count == 0) {
            *this = other;
            return;
        }
        const auto   COMBINED = count + other.count;
        const double DELTA    = other.mean_damage - mean_damage;
        m2_damage += other.m2_damage + (DELTA * DELTA * static_cast<double>(count) * static_cast<double>(other.count) /
                                        static_cast<double>(COMBINED));
        mean_damage += DELTA * static_cast<double>(other.count) / static_cast<double>(COMBINED);
        count        = COMBINED;
        min_damage   = std::min(min_damage, other.min_damage);
        max_damage   = std::max(max_damage, other.max_damage);
        checksum    ^= other.checksum;
    }

    double PopulationDeviation() const { return (count != 0u) ? std::sqrt(m2_damage / static_cast<double>(count)) : 0; }
};

} // namespace JX3DPS::runtime
