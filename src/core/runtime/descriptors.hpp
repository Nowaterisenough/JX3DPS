#ifndef JX3DPS_RUNTIME_DESCRIPTORS_HPP
#define JX3DPS_RUNTIME_DESCRIPTORS_HPP

#include "../runtime.hpp"

namespace JX3DPS::runtime {

struct SkillDescriptor {
    jx3id_t id;
    std::string_view name;
    tick_t cooldown;
    tick_t gcd;
    tick_t cast_time;
};
struct BuffDescriptor {
    jx3id_t id;
    std::string_view name;
    tick_t duration;
    tick_t interval;
    int max_stacks;
};

template <typename Descriptor, std::size_t Count>
struct DescriptorTable {
    static_assert(Count < INVALID_SLOT);
    std::array<Descriptor, Count> entries;

    constexpr Slot FindId(jx3id_t id) const {
        for (Slot i = 0; i < Count; ++i) if (entries[i].id == id) return i;
        return INVALID_SLOT;
    }
    constexpr Slot FindName(std::string_view name) const {
        for (Slot i = 0; i < Count; ++i) if (entries[i].name == name) return i;
        return INVALID_SLOT;
    }
    constexpr bool Valid() const {
        for (std::size_t i = 0; i < Count; ++i) {
            if (entries[i].name.empty()) return false;
            for (std::size_t j = 0; j < i; ++j) {
                if (entries[i].id == entries[j].id || entries[i].name == entries[j].name) return false;
            }
        }
        return true;
    }
};

} // namespace JX3DPS::runtime
#endif
