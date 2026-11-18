#ifndef JX3DPS_CONCEPTS_H
#define JX3DPS_CONCEPTS_H

#include <array>
#include <concepts>
#include <type_traits>

namespace JX3DPS {

template <typename T>
concept IsArrayAP = requires(T t) {
    requires std::is_array_v<decltype(T::physics_ap)> || std::is_array_v<decltype(T::neutral_ap)> ||
                 std::is_array_v<decltype(T::poison_ap)> || std::is_array_v<decltype(T::solar_ap)> ||
                 std::is_array_v<decltype(T::lunar_ap)> || std::is_array_v<decltype(T::solar_and_lunar_ap)>;


};

} // namespace JX3DPS

#endif // JX3DPS_CONCEPTS_H