#pragma once
#include <stdexcept>
#include <type_traits>
#include <vector>

// Force-included only in the separate recording target. Keep the pinned RNG's
// implementation and return values unchanged, observing draws at its boundary.
#define RandomUniform LegacyNativeRandomUniform
#include "Utils/Utils.hpp"
#undef RandomUniform

namespace legacy_draws {
inline bool active = false;
inline std::vector<double> samples;
inline std::vector<int> kinds;
inline std::size_t integer_procs = 0;
inline void Start() { samples.clear(); kinds.clear(); integer_procs = 0; active = true; }
}

namespace JX3DPS {
template <typename T> T RandomUniform(T a, T b) {
    const T value = LegacyNativeRandomUniform(a, b);
    if (!legacy_draws::active || a == b) return value;
    if constexpr (std::is_floating_point_v<T>) {
        if (a != 0 || b != 1) throw std::runtime_error("unsupported floating-point draw range");
        legacy_draws::samples.push_back(value);
        legacy_draws::kinds.push_back(0);
    } else if (a == 1 && b == 1024) {
        legacy_draws::samples.push_back((value - 1) / 1024.0);
        legacy_draws::kinds.push_back(1024);
        ++legacy_draws::integer_procs;
    } else if (!(a == 0 && b == 15)) {
        if (a < 0 || b > 1024) throw std::runtime_error("unsupported integer draw range");
        legacy_draws::samples.push_back(value);
        legacy_draws::kinds.push_back(-1-static_cast<int>(a)*1024-static_cast<int>(b));
        ++legacy_draws::integer_procs;
    }
    return value;
}
}
