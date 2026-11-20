#pragma once

#include "../../legacy_log.hpp"

// Only the pinned ba960f7 sources include this historical header.
namespace spdlog {

inline void debug(const auto &format, const auto &...args)
{
    legacy_compare::Logger().debug(format, args...);
}

inline void info(const auto &format, const auto &...args)
{
    legacy_compare::Logger().info(format, args...);
}

inline void error(const auto &format, const auto &...args)
{
    legacy_compare::Logger().error(format, args...);
}

} // namespace spdlog
