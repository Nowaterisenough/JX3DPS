#ifndef JX3DPS_TYPE_H
#define JX3DPS_TYPE_H

#include <functional>
#include <map>
#include <unordered_map>
#include <cstdint>

namespace JX3DPS {

using pctf_t = double;

using pctn_t = int;

using bpn_t = int;

using frame_t = uint16_t;

using value_t = int;

using cof_t = double;

using id_t = int;

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

} // namespace JX3DPS

#endif // JX3DPS_TYPE_H