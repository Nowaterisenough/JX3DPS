#ifndef JX3DPS_TEST_FIXTURE_RULES_HPP
#define JX3DPS_TEST_FIXTURE_RULES_HPP

#include "src/core/runtime/simulation.hpp"

// A synthetic combat ruleset for engine parity/performance tests. Its values
// are deliberately not presented as real game skill coefficients.
template <bool ValidateImmediate = true>
struct BasicFixtureRules {
    using TimerId = JX3DPS::runtime::DeadlineQueue::TimerId;
    std::array<TimerId, 3> skill_timers{};
    TimerId gcd_timer{}, buff_timer{};
    JX3DPS::tick_t gcd_at = 0;
    std::uint64_t random = 0;
    JX3DPS::value_t immediate_total = 0;

    template <typename Engine> void Setup(Engine &engine) {
        using namespace JX3DPS::runtime;
        for (Slot i = 0; i < skill_timers.size(); ++i) skill_timers[i] = engine.Timers().Create({TimerKind::SkillCooldown, i});
        gcd_timer = engine.Timers().Create({TimerKind::MacroWake});
        buff_timer = engine.Timers().Create({TimerKind::BuffDuration, 0});
    }
    template <typename Engine> void Reset(Engine &engine, std::uint64_t seed) {
        auto &s = engine.MutableState();
        s.qidian = 7;
        s.self_buff_stacks[0] = 1;
        s.self_buff_expires_at[0] = 160;
        engine.Timers().ArmAt(buff_timer, 160);
        gcd_at = 0;
        random = seed;
        immediate_total = 0;
    }
    template <typename Engine> bool Cast(Engine &engine, const JX3DPS::runtime::MacroLine &line) {
        using namespace JX3DPS;
        using namespace JX3DPS::runtime;
        auto &s = engine.MutableState();
        const auto slot = line.action_slot;
        if (s.now < gcd_at || s.now < s.skill_ready_at[slot] || (slot == 0 && s.qidian < 3)) return false;
        // Resolve the roll before its resource proc can affect the next macro.
        random += 0x9e3779b97f4a7c15ULL;
        auto key = random;
        key = (key ^ (key >> 30)) * 0xbf58476d1ce4e5b9ULL;
        key = (key ^ (key >> 27)) * 0x94d049bb133111ebULL;
        key ^= key >> 31;
        const auto outcome = key % 3 == 0 ? RollResult::DOUBLE : RollResult::HIT;
        engine.Log().Record(s.now, slot, 0, 1, key, 100 + slot * 20, 0, outcome);
        if constexpr (ValidateImmediate) immediate_total += Reduce(engine.Log().Intents().back(), s);
        const int resource = std::clamp(s.qidian + (slot == 0 ? -3 : 1) + (outcome == RollResult::DOUBLE ? 2 : 0), 0, 10);
        SetResources(s, engine.Log(), resource, s.energy, s.rage);
        SetSkillCooldown(s, engine.Log(), slot, s.now + (slot == 2 ? 96 : 24));
        engine.Timers().ArmAt(skill_timers[slot], s.skill_ready_at[slot]);
        gcd_at = s.now + 16;
        engine.Timers().ArmAt(gcd_timer, gcd_at);
        return true;
    }
    template <typename Engine> void OnTimer(Engine &engine, JX3DPS::runtime::TimerHandle handle, TimerId) {
        using namespace JX3DPS::runtime;
        if (handle.kind == TimerKind::BuffDuration) SetBuff(engine.MutableState(), engine.Log(), false, 0, 0, 0, 0);
    }
    JX3DPS::value_t Reduce(const JX3DPS::runtime::DamageIntent &hit, const JX3DPS::runtime::State &state) const {
        return hit.base * (1 + state.self_buff_stacks[0]) * (hit.outcome == JX3DPS::RollResult::DOUBLE ? 2 : 1);
    }
};

using FixtureRules = BasicFixtureRules<true>;
using BenchmarkRules = BasicFixtureRules<false>;

// Exercises the core's channel ownership independently of game-specific
// skills. A channel only resolves when its owner reaches the deadline; a
// force-cast cancels that owner and starts the replacement channel.
struct ChannelFixtureRules {
    using TimerId = JX3DPS::runtime::DeadlineQueue::TimerId;
    TimerId channel_timer = 0;

    template <typename Engine> void Setup(Engine &engine) {
        channel_timer = engine.Timers().Create({JX3DPS::runtime::TimerKind::SkillCasting, 0});
    }
    template <typename Engine> void Reset(Engine &, std::uint64_t) {}
    template <typename Engine> bool Cast(Engine &engine, const JX3DPS::runtime::MacroLine &line) {
        using namespace JX3DPS::runtime;
        auto &state = engine.MutableState();
        if (state.casting_skill != INVALID_SLOT) {
            if (!IsForceCast(line.command)) return false;
            SetCasting(state, engine.Log(), state.casting_skill, 0, false);
            engine.Timers().Cancel(channel_timer);
        }
        if (line.action_slot != 0) return false;
        SetCasting(state, engine.Log(), 0, state.now + 8, true);
        engine.Timers().ArmAt(channel_timer, state.skill_casting_at[0]);
        return true;
    }
    template <typename Engine> void OnTimer(Engine &engine, JX3DPS::runtime::TimerHandle handle, TimerId) {
        using namespace JX3DPS::runtime;
        auto &state = engine.MutableState();
        if (handle.kind != TimerKind::SkillCasting || state.casting_skill != handle.slot ||
            state.skill_casting_at[handle.slot] != state.now) return;
        SetCasting(state, engine.Log(), handle.slot, 0, false);
        engine.Log().Record(state.now, handle.slot, INVALID_SLOT, 0, 0, 1, 0);
    }
    JX3DPS::value_t Reduce(const JX3DPS::runtime::DamageIntent &hit) const { return hit.base; }
};

#endif
