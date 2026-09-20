#ifndef JX3DPS_RUNTIME_DAMAGE_LOG_HPP
#define JX3DPS_RUNTIME_DAMAGE_LOG_HPP

#include "../runtime.hpp"
#include <stdexcept>

namespace JX3DPS::runtime {

struct DamageIntent {
    tick_t frame = 0;
    std::uint32_t sequence = 0;
    Slot skill = INVALID_SLOT;
    Slot target = INVALID_SLOT;
    std::uint16_t level = 0;
    RollResult outcome = RollResult::HIT;
    std::uint64_t random_key = 0;
    value_t base = 0;
    cof_t coefficient = 0;
    std::uint16_t effect = 0;
    std::uint16_t snapshot = 0;
    std::uint16_t sub = 0;
    bool operator==(const DamageIntent &) const = default;
};

enum class MutationKind : std::uint8_t { Buff, SkillCooldown, Resources, Gcd, SkillEnergy, Prepare, Casting };
struct StateMutation {
    tick_t frame = 0;
    std::uint32_t sequence = 0;
    MutationKind kind = MutationKind::Buff;
    Slot slot = INVALID_SLOT;
    bool target = false;
    // Buff: expires_at, tick_at, stacks. Cooldown: ready_at. Resources: qidian, energy, rage.
    // Prepare/Casting: absolute completion frame in a; active flag in b.
    int a = 0, b = 0, c = 0;
    bool operator==(const StateMutation &) const = default;
};

// A single owner sequences damage and mutations, including events in the same
// frame. Capacity exhaustion is explicit, never a silent allocation or drop.
class DamageIntentLog {
public:
    explicit DamageIntentLog(std::size_t damage_capacity = 4096, std::size_t mutation_capacity = 4096) {
        intents_.reserve(damage_capacity);
        mutations_.reserve(mutation_capacity);
    }
    void Clear() {
        intents_.clear();
        mutations_.clear();
        next_sequence_ = 0;
        last_frame_ = 0;
    }
    void Record(tick_t frame, Slot skill, Slot target, std::uint16_t level,
                std::uint64_t random_key, value_t base, cof_t coefficient,
                RollResult outcome = RollResult::HIT, std::uint16_t effect = 0, std::uint16_t snapshot = 0,
                std::uint16_t sub = 0) {
        CheckCapacity(intents_);
        intents_.push_back({frame, Sequence(frame), skill, target, level, outcome, random_key, base, coefficient, effect, snapshot, sub});
    }
    void RecordBuff(tick_t frame, bool target, Slot slot, tick_t expires_at, tick_t tick_at, int stacks) {
        CheckCapacity(mutations_);
        mutations_.push_back({frame, Sequence(frame), MutationKind::Buff, slot, target, expires_at, tick_at, stacks});
    }
    void RecordSkillCooldown(tick_t frame, Slot slot, tick_t ready_at) {
        CheckCapacity(mutations_);
        mutations_.push_back({frame, Sequence(frame), MutationKind::SkillCooldown, slot, false, ready_at});
    }
    void RecordResources(tick_t frame, int qidian, int energy, int rage) {
        CheckCapacity(mutations_);
        mutations_.push_back({frame, Sequence(frame), MutationKind::Resources, INVALID_SLOT, false, qidian, energy, rage});
    }
    void RecordGcd(tick_t frame, Slot group, tick_t ready_at) {
        CheckCapacity(mutations_);
        mutations_.push_back({frame, Sequence(frame), MutationKind::Gcd, group, false, ready_at});
    }
    void RecordSkillEnergy(tick_t frame, Slot slot, int energy) {
        CheckCapacity(mutations_);
        mutations_.push_back({frame, Sequence(frame), MutationKind::SkillEnergy, slot, false, energy});
    }
    void RecordPrepare(tick_t frame, Slot slot, tick_t ready_at, bool active) {
        CheckCapacity(mutations_);
        mutations_.push_back({frame, Sequence(frame), MutationKind::Prepare, slot, false, ready_at, active});
    }
    void RecordCasting(tick_t frame, Slot slot, tick_t ready_at, bool active) {
        CheckCapacity(mutations_);
        mutations_.push_back({frame, Sequence(frame), MutationKind::Casting, slot, false, ready_at, active});
    }
    template <typename Fn>
    value_t Reduce(const State &initial, State &scratch, Fn &&reducer) const {
        if (&initial == &scratch) throw std::invalid_argument("replay scratch must not alias initial state");
        // Give each worker a scratch State of the same shape; vector copy
        // assignment then reuses its storage for every iteration.
        scratch = initial;
        std::size_t cursor = 0;
        value_t total = 0;
        for (const auto &intent : intents_) {
            while (cursor < mutations_.size() && mutations_[cursor].sequence < intent.sequence) {
                Apply(scratch, mutations_[cursor++]);
            }
            scratch.now = intent.frame;
            total += reducer(intent, scratch);
        }
        return total;
    }
    const std::vector<DamageIntent> &Intents() const { return intents_; }
    const std::vector<StateMutation> &Mutations() const { return mutations_; }
    std::size_t Size() const { return intents_.size(); }
    std::size_t DamageCapacity() const { return intents_.capacity(); }

private:
    template <typename T>
    static void CheckCapacity(const std::vector<T> &buffer) {
        if (buffer.size() == buffer.capacity()) throw std::length_error("event log capacity exhausted");
    }
    std::uint32_t Sequence(tick_t frame) {
        if (frame < last_frame_) throw std::invalid_argument("event frames must be monotonic");
        if (next_sequence_ == std::numeric_limits<std::uint32_t>::max()) throw std::overflow_error("event sequence exhausted");
        last_frame_ = frame;
        return next_sequence_++;
    }
public:
    // Shared by damage reduction and desktop history replay.
    static void Apply(State &state, const StateMutation &m) {
        switch (m.kind) {
            case MutationKind::Buff: {
                auto &expiry = m.target ? state.target_buff_expires_at : state.self_buff_expires_at;
                auto &tick = m.target ? state.target_buff_tick_at : state.self_buff_tick_at;
                auto &stacks = m.target ? state.target_buff_stacks : state.self_buff_stacks;
                expiry.at(m.slot) = m.a;
                tick.at(m.slot) = m.b;
                stacks.at(m.slot) = m.c;
                break;
            }
            case MutationKind::SkillCooldown: state.skill_ready_at.at(m.slot) = m.a; break;
            case MutationKind::Resources: state.qidian = m.a; state.energy = m.b; state.rage = m.c; break;
            case MutationKind::Gcd: state.gcd_ready_at.at(m.slot) = m.a; break;
            case MutationKind::SkillEnergy: state.skill_energy.at(m.slot) = m.a; break;
            case MutationKind::Prepare:
                state.skill_prepare_at.at(m.slot) = m.a;
                state.preparing_skill = m.b ? m.slot : INVALID_SLOT;
                break;
            case MutationKind::Casting:
                state.skill_casting_at.at(m.slot) = m.a;
                state.casting_skill = m.b ? m.slot : INVALID_SLOT;
                break;
        }
    }
private:
    std::vector<DamageIntent> intents_;
    std::vector<StateMutation> mutations_;
    std::uint32_t next_sequence_ = 0;
    tick_t last_frame_ = 0;
};

inline void SetBuff(State &state, DamageIntentLog &log, bool target, Slot slot,
                    tick_t expires_at, tick_t tick_at, int stacks) {
    auto &expiry = target ? state.target_buff_expires_at : state.self_buff_expires_at;
    auto &tick = target ? state.target_buff_tick_at : state.self_buff_tick_at;
    auto &layers = target ? state.target_buff_stacks : state.self_buff_stacks;
    if (slot >= expiry.size() || slot >= tick.size() || slot >= layers.size() || stacks < 0) {
        throw std::out_of_range("invalid buff slot or stack count");
    }
    log.RecordBuff(state.now, target, slot, expires_at, tick_at, stacks);
    expiry[slot] = expires_at;
    tick[slot] = tick_at;
    layers[slot] = stacks;
}
inline void SetSkillCooldown(State &state, DamageIntentLog &log, Slot slot, tick_t ready_at) {
    if (slot >= state.skill_ready_at.size()) throw std::out_of_range("invalid skill slot");
    log.RecordSkillCooldown(state.now, slot, ready_at);
    state.skill_ready_at[slot] = ready_at;
}
inline void SetResources(State &state, DamageIntentLog &log, int qidian, int energy, int rage) {
    log.RecordResources(state.now, qidian, energy, rage);
    state.qidian = qidian;
    state.energy = energy;
    state.rage = rage;
}
inline void SetGcd(State &state, DamageIntentLog &log, Slot group, tick_t ready_at) {
    if (group >= state.gcd_ready_at.size()) throw std::out_of_range("invalid GCD group");
    log.RecordGcd(state.now, group, ready_at);
    state.gcd_ready_at[group] = ready_at;
}
inline void SetSkillEnergy(State &state, DamageIntentLog &log, Slot slot, int energy) {
    if (slot >= state.skill_energy.size() || energy < 0) throw std::out_of_range("invalid skill energy");
    log.RecordSkillEnergy(state.now, slot, energy);
    state.skill_energy[slot] = energy;
}
inline void SetPrepare(State &state, DamageIntentLog &log, Slot slot, tick_t ready_at, bool active) {
    if (slot >= state.skill_prepare_at.size()) throw std::out_of_range("invalid prepare slot");
    log.RecordPrepare(state.now, slot, ready_at, active);
    state.skill_prepare_at[slot] = ready_at;
    state.preparing_skill = active ? slot : INVALID_SLOT;
}
inline void SetCasting(State &state, DamageIntentLog &log, Slot slot, tick_t ready_at, bool active) {
    if (slot >= state.skill_casting_at.size()) throw std::out_of_range("invalid casting skill slot");
    log.RecordCasting(state.now, slot, ready_at, active);
    state.skill_casting_at[slot] = ready_at;
    state.casting_skill = active ? slot : INVALID_SLOT;
}

} // namespace JX3DPS::runtime
#endif
