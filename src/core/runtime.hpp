#ifndef JX3DPS_CORE_RUNTIME_HPP
#define JX3DPS_CORE_RUNTIME_HPP

#include "src/global/types.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace JX3DPS::runtime {

using Slot = std::uint16_t;
constexpr Slot INVALID_SLOT = std::numeric_limits<Slot>::max();

// A name is resolved once while compiling a macro. Runtime code only sees slots.
class NameRegistry {
public:
    Slot Intern(std::string_view name) {
        const auto it = by_name_.find(std::string(name));
        if (it != by_name_.end()) {
            return it->second;
        }
        if (names_.size() >= INVALID_SLOT) {
            return INVALID_SLOT;
        }
        const Slot slot = static_cast<Slot>(names_.size());
        names_.emplace_back(name);
        by_name_.emplace(names_.back(), slot);
        return slot;
    }

    Slot Find(std::string_view name) const {
        const auto it = by_name_.find(std::string(name));
        return it == by_name_.end() ? INVALID_SLOT : it->second;
    }

    std::string_view Name(Slot slot) const {
        return slot < names_.size() ? std::string_view(names_[slot]) : std::string_view{};
    }

    std::size_t Size() const { return names_.size(); }

private:
    std::vector<std::string> names_;
    std::unordered_map<std::string, Slot> by_name_;
};

// Dense state used by both modes. All timer arrays store absolute frames.
// Expiry events clear stacks before the next macro pass; no timer sweep is needed.
struct State {
    tick_t now = 0;
    int qidian = 10;
    int energy = 0;
    int rage = 0;
    pctf_t life = 1.0;
    pctf_t mana = 1.0;
    pctf_t target_life = 1.0;
    Slot last_skill = INVALID_SLOT;
    Slot preparing_skill = INVALID_SLOT;
    // At most one channel owns the player at a time.  Force-cast actions may
    // interrupt this owner; normal actions must wait for its completion.
    Slot casting_skill = INVALID_SLOT;

    std::vector<tick_t> skill_ready_at;
    std::vector<Slot> skill_gcd_group;
    std::array<tick_t, 8> gcd_ready_at{};
    std::vector<int> skill_energy;
    std::vector<tick_t> skill_prepare_at;
    std::vector<tick_t> skill_casting_at;
    std::vector<tick_t> self_buff_expires_at;
    std::vector<tick_t> self_buff_tick_at;
    std::vector<int> self_buff_stacks;
    std::vector<tick_t> target_buff_expires_at;
    std::vector<tick_t> target_buff_tick_at;
    std::vector<int> target_buff_stacks;

    void Resize(std::size_t skill_count, std::size_t buff_count) {
        skill_ready_at.assign(skill_count, 0);
        skill_gcd_group.assign(skill_count, INVALID_SLOT);
        skill_energy.assign(skill_count, 0);
        skill_prepare_at.assign(skill_count, 0);
        skill_casting_at.assign(skill_count, 0);
        self_buff_expires_at.assign(buff_count, 0);
        self_buff_tick_at.assign(buff_count, 0);
        self_buff_stacks.assign(buff_count, 0);
        target_buff_expires_at.assign(buff_count, 0);
        target_buff_tick_at.assign(buff_count, 0);
        target_buff_stacks.assign(buff_count, 0);
        last_skill = INVALID_SLOT;
        preparing_skill = INVALID_SLOT;
        casting_skill = INVALID_SLOT;
        gcd_ready_at.fill(0);
    }

    void Reset() {
        now = 0;
        qidian = 10;
        energy = 0;
        rage = 0;
        life = 1.0;
        mana = 1.0;
        target_life = 1.0;
        last_skill = INVALID_SLOT;
        preparing_skill = INVALID_SLOT;
        casting_skill = INVALID_SLOT;
        gcd_ready_at.fill(0);
        std::fill(skill_ready_at.begin(), skill_ready_at.end(), 0);
        std::fill(skill_energy.begin(), skill_energy.end(), 0);
        std::fill(skill_prepare_at.begin(), skill_prepare_at.end(), 0);
        std::fill(skill_casting_at.begin(), skill_casting_at.end(), 0);
        std::fill(self_buff_expires_at.begin(), self_buff_expires_at.end(), 0);
        std::fill(self_buff_tick_at.begin(), self_buff_tick_at.end(), 0);
        std::fill(self_buff_stacks.begin(), self_buff_stacks.end(), 0);
        std::fill(target_buff_expires_at.begin(), target_buff_expires_at.end(), 0);
        std::fill(target_buff_tick_at.begin(), target_buff_tick_at.end(), 0);
        std::fill(target_buff_stacks.begin(), target_buff_stacks.end(), 0);
    }

    tick_t SkillReadyAt(Slot slot) const {
        const auto group = skill_gcd_group[slot];
        return group == INVALID_SLOT ? skill_ready_at[slot] : std::max(skill_ready_at[slot], gcd_ready_at[group]);
    }
};

} // namespace JX3DPS::runtime

#include "runtime/damage_log.hpp"
#include "runtime/scheduler.hpp"
#include "runtime/macro.hpp"

#endif // JX3DPS_CORE_RUNTIME_HPP
