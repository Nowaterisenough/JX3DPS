#ifndef JX3DPS_TYPE_H
#define JX3DPS_TYPE_H

#include <cstdint>
#include <functional>
#include <map>
#include <string_view>
#include <unordered_map>

#define CONSTANT inline static thread_local constinit
#define PROPERTY inline static thread_local

namespace JX3DPS {

using pctf_t = double;

using pctn_t = int;

using bpn_t = int;

using frame_t = uint16_t;

using value_t = int;

using cof_t = double;

using jx3id_t = int;

using str_t = std::string_view;

template <typename T, int COUNT>
using array_t = std::array<T, COUNT>;

template <typename T>
using vector_t = std::vector<T>;

template <typename Key, typename Value>
using hash_t = std::unordered_map<Key, Value>;

template <typename Key, typename Value>
using rbtree_t = std::map<Key, Value>;

template <typename T>
using func_t = std::function<T>;

template <typename T>
concept is_16bit = std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>;

template <typename T>
concept is_32bit = std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>;

} // namespace JX3DPS

#endif // JX3DPS_TYPE_H
