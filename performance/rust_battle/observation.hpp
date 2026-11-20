#pragma once
#include <fstream>

#include "src/core/runtime/batch.hpp"

namespace battle_bench {
using namespace JX3DPS;
using namespace JX3DPS::runtime;

struct Audit
{
    std::ofstream stream;

    template <class... Args>
    void write(const Args &...args)
    {
        (stream.write(reinterpret_cast<const char *>(&args), sizeof(args)), ...);
    }
};

template <class E>
void dump(E &e, Audit &a)
{
    for (auto &h : e.Log().Intents()) {
        a.write('H',
                h.frame,
                h.sequence,
                h.skill,
                h.target,
                h.level,
                h.outcome,
                h.random_key,
                h.base,
                h.coefficient,
                h.effect,
                h.snapshot,
                h.sub,
                h.snapshot_version,
                h.live_version,
                e.GetRules().Reduce(h));
    }
    for (auto &m : e.Log().Mutations()) {
        a.write('M', m.frame, m.sequence, m.kind, m.slot, m.target, m.a, m.b, m.c);
    }
}

struct Skill
{
    int64_t                 damage = 0;
    std::array<uint64_t, 4> outcomes{};

    void add(value_t d, RollResult r)
    {
        damage += d;
        ++outcomes[r == RollResult::HIT ? 0 : r == RollResult::DOUBLE ? 1 : r == RollResult::INSIGHT ? 2 : 3];
    }
};

struct alignas(64) Result
{
    BatchStats         batch;
    std::vector<Skill> skills;
};

} // namespace battle_bench
