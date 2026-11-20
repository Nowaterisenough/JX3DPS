#pragma once

#include <bit>
#include <span>

#include "damage_log.hpp"

namespace JX3DPS::runtime {

struct FullDamageGrouping
{
    using Key = DamageIntent;

    static Key MakeKey(DamageIntent hit)
    {
        hit.frame      = 0;
        hit.sequence   = 0;
        hit.random_key = 0;
        return hit;
    }

    static std::size_t Hash(const DamageIntent &hit)
    {
        std::uint64_t hash = 0;
        const auto    ADD  = [&](std::uint64_t value) {
            value  = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
            value  = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
            hash  ^= (value ^ (value >> 31)) + 0x9e3779b97f4a7c15ULL + (hash << 6) + (hash >> 2);
        };
        ADD(static_cast<std::uint64_t>(hit.snapshot_version) << 32 | hit.live_version);
        ADD(static_cast<std::uint64_t>(hit.effect) << 48 | static_cast<std::uint64_t>(hit.snapshot) << 32 |
            static_cast<std::uint64_t>(hit.level) << 16 | hit.sub);
        ADD(static_cast<std::uint64_t>(hit.skill) << 32 | static_cast<std::uint64_t>(hit.target) << 16 |
            static_cast<unsigned>(hit.outcome));
        ADD(static_cast<std::uint64_t>(hit.base));
        ADD(std::hash<cof_t>{}(hit.coefficient));
        return static_cast<std::size_t>(hash);
    }
};

// Group by every input read by the module's damage reducer.
template <class Grouping>
class GroupedDamageStatistics
{
public:
    struct Entry
    {
        typename Grouping::Key hit;
        std::uint32_t          count = 0;
    };

    void Reserve(std::size_t capacity)
    {
        if ((capacity == 0u) || capacity > std::numeric_limits<std::uint32_t>::max() ||
            capacity > std::numeric_limits<std::size_t>::max() / 4)
        {
            throw std::invalid_argument("invalid damage statistics capacity");
        }
        entries_.reserve(capacity);
        entries_.clear();
        capacity_ = capacity;
        slots_.assign(std::bit_ceil(capacity * 2), HashSlot{});
        generation_ = 1;
    }

    void Clear()
    {
        entries_.clear();
        if (++generation_ == 0) {
            std::fill(slots_.begin(), slots_.end(), HashSlot{});
            generation_ = 1;
        }
    }

    void Record(const DamageIntent &intent)
    {
        if (slots_.empty()) {
            throw std::logic_error("damage statistics storage is not reserved");
        }
        const auto hit   = Grouping::MakeKey(intent);
        auto       index = Grouping::Hash(hit) & (slots_.size() - 1);
        while (slots_[index].generation == generation_) {
            auto &entry = entries_[slots_[index].entry];
            if (entry.hit == hit) {
                if (entry.count == std::numeric_limits<std::uint32_t>::max()) {
                    throw std::overflow_error("damage statistics count exhausted");
                }
                ++entry.count;
                return;
            }
            index = (index + 1) & (slots_.size() - 1);
        }
        if (entries_.size() == capacity_) {
            throw std::length_error("damage statistics capacity exhausted");
        }
        slots_[index] = { generation_, static_cast<std::uint32_t>(entries_.size()) };
        entries_.push_back({ hit, 1 });
    }

    void Collect(std::span<const DamageIntent> hits)
    {
        Clear();
        for (const auto &hit : hits) {
            Record(hit);
        }
    }

    std::span<const Entry> Entries() const { return entries_; }

    template <class Reducer>
    value_t Reduce(const Reducer &reducer) const
    {
        value_t total = 0;
        for (const auto &entry : entries_) {
            total += reducer.Reduce(entry.hit) * entry.count;
        }
        return total;
    }

private:
    struct HashSlot
    {
        std::uint32_t generation = 0, entry = 0;
    };

    std::vector<Entry>    entries_;
    std::vector<HashSlot> slots_;
    std::uint32_t         generation_ = 0;
    std::size_t           capacity_   = 0;
};

using DamageStatistics = GroupedDamageStatistics<FullDamageGrouping>;

template <class Module>
auto MakeDamageStatistics()
{
    if constexpr (requires { typename Module::DamageGrouping; }) {
        return GroupedDamageStatistics<typename Module::DamageGrouping>{};
    } else {
        return DamageStatistics{};
    }
}

} // namespace JX3DPS::runtime
