#pragma once
#include <cstdint>

namespace JX3DPS::runtime {
struct SplitMixRolls
{
    std::uint64_t state = 0;

    void Reset(std::uint64_t seed) { state = seed; }

    std::uint64_t Next()
    {
        auto z = (state += 0x9e3779b97f4a7c15ULL);
        z      = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z      = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }
};
} // namespace JX3DPS::runtime
