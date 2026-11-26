#pragma once

#include <queue>

#include "src/core/runtime/combat_rolls.hpp"
#include "src/core/runtime/simulation.hpp"

#include "damage.hpp"

namespace JX3DPS::runtime::season50 {

class Rules
{
    using Timer = DeadlineQueue::TimerId;

    struct AuraTimers
    {
        Timer    expiry, tick;
        unsigned snapshot   = 0;
        int      remaining  = 0;
        int      newest     = -1;
        tick_t   last_pulse = 0;
        double   placed_at = 0.; // Target position along the encounter's distance axis at application.
        std::vector<tick_t> pending_pulses;
    };

    Preparation             data_;
    SplitMixRolls           rng_;
    TargetTimeline          timeline_;
    std::vector<Timer>      cooldown_, regeneration_;
    std::vector<tick_t>     recharge_at_;
    std::vector<std::vector<tick_t>> queued_charge_times_;
    std::vector<AuraTimers> timers_;
    std::array<Timer, 8>    gcd_;
    Timer                   cast_, delayed_timer_;

    struct Deferred
    {
        tick_t   frame;
        unsigned sequence;
        Slot     effect;
        int      rank = 1;

        bool operator>(const Deferred &r) const { return frame != r.frame ? frame > r.frame : sequence > r.sequence; }
    };

    std::priority_queue<Deferred, std::vector<Deferred>, std::greater<Deferred>> delayed_;
    unsigned                                                                     delay_sequence_ = 0;
    std::vector<unsigned>                                                        combo_;
    std::vector<tick_t>                                                          combo_until_;
    std::vector<Stats>                                                           versions_;
    std::vector<unsigned>                                                        last_snapshot_;
    Slot                                                                         pending_       = INVALID_SLOT;
    tick_t                                                                       pulse_spacing_ = 0;
    std::array<tick_t, 9>                                                        distinct_until_{};
    tick_t channel_start_ = 0, channel_duration_ = 0;
    int    channel_pulses_    = 0;
    int    pulses_left_       = 0;
    int    channel_steps_     = 0;
    bool   last_critical_     = false;
    bool   captured_          = false;
    bool   empowered_channel_ = false;
    int    completed_pulses_ = 0;
    tick_t finish_cd_         = 0;
    tick_t deferred_swing_due_ = -1;
    tick_t wrist_ready_ = 0, shoe_ready_ = 0, belt_ready_ = 0;
    tick_t bracer_ready_ = 0, adaptive_ready_ = 0, trinket_ready_ = 0;
    tick_t speed_dot_ready_ = 0, trial_shoe_ready_ = 0;
    tick_t pollen_ready_ = 0;
    int pollen_window_ = -1, pollen_hits_ = 0;

    const Book &book() const { return *data_->config.book; }

    tick_t Hasted(tick_t frames) const { return frames ? std::max(1, static_cast<int>(frames / data_->haste)) : 0; }

    double TargetDistance(const State &s, Slot origin) const
    {
        if (origin == INVALID_SLOT || !Stacks(s, origin)) return s.target_distance;
        return std::abs(s.target_distance - timers_[origin].placed_at);
    }

    tick_t ChannelOffset(const Action &a, int count) const
    {
        if (!a.distribute_pulses && a.channel_buff == INVALID_SLOT)
            return channel_duration_ * count / channel_pulses_;
        const int remainder = channel_duration_ % channel_pulses_;
        const int extra = a.later_remainder ? std::max(0, count - (channel_pulses_ - remainder)) : std::min(count, remainder);
        return count * (channel_duration_ / channel_pulses_) + extra;
    }

    tick_t StanceTime(const State &s, tick_t frames) const
    {
        if (book().haste_stance == INVALID_SLOT || !Stacks(s, book().haste_stance)) {
            return Hasted(frames);
        }
        return frames ? std::max(1, static_cast<int>(frames / (data_->haste + data_->stance_haste / 1024.))) : 0;
    }

    tick_t PulseInterval(const State &s, Slot slot) const
    {
        const auto &a    = book().auras[slot];
        const auto  base = data_->base_intervals[slot];
        return (a.stance_timing ? StanceTime(s, base) : a.hasted ? Hasted(base) : base) + a.interval_padding;
    }

    static int &ResourceAt(State &s, Slot i)
    {
        if (i == 0) {
            return s.qidian;
        }
        if (i == 1) {
            return s.energy;
        }
        return s.rage;
    }

    int Stacks(const State &s, Slot b) const
    {
        return book().auras[b].target ? s.target_buff_stacks[b] : s.self_buff_stacks[b];
    }

    unsigned Version(const Stats &stats)
    {
        const auto it = std::ranges::find_if(versions_, [&](const Stats &cached) { return cached == stats; });
        if (it != versions_.end()) {
            return static_cast<unsigned>(it - versions_.begin());
        }
        versions_.push_back(stats);
        return static_cast<unsigned>(versions_.size() - 1);
    }

    unsigned Snapshot(const State &s, Slot effect, int rank = 1)
    {
        auto stats = data_->passive[effect];
        if (data_->config.raid_full) {
            const auto now = s.now;
            // Windows are half-open and evaluated at the actual impact or application frame.
            if (now % 1920 < 480) stats.strain_flat += 2600; // 仙王蛊鼎
            if (now % 480 < 240) stats.strain_flat += 3 * data_->raid_hanxiao_stacks;
            if (now % 2880 < 128) stats.strain_flat += 4100; // 圣浴明心
            if (now < 960) stats.strain_flat += now < 480 ? 3400 : 1700;
            if (now % 800 < 480) stats.strain_flat += 1000; // 飘黄
            if (now % 1120 < 160) stats.power += 51; // 皎素
            stats.fragile += 21; // 戒火
            if (book().hits[effect].physical) {
                stats.shield_flat -= 372; // 劲风
                stats.target_shield_percent -= 51 + (now % 480 < 160 ? 102 : 0); // 虚弱、破甲
            } else {
                stats.target_shield_percent -= 154; // 立地成佛
            }
            switch (data_->raid_formation) {
                case Prepared::RaidFormation::LingXue: if (now % 400 < 224) stats.attack += 102; break;
                case Prepared::RaidFormation::PengLai: if (now % 160 < 64) stats.attack += 72; break;
                case Prepared::RaidFormation::YiJin:
                    // Model the published 97% uptime: rebuild one layer per GCD after a lapse every 100 s.
                    stats.attack += 21 * std::min(5, 1 + (now % 1600) / 24);
                    break;
                default: break;
            }
        }
        if (data_->legendary_strain_per_stack) stats.strain_flat += data_->legendary_strain_per_stack * Stacks(s, book().legendary_strain);
        const auto active = [&](Slot aura) { return aura != INVALID_SLOT && Stacks(s, aura); };
        const int extra_crit = (active(data_->ring_crit) ? 313 : 0) + (active(data_->adaptive_crit) ? 595 : 0) +
                               (active(data_->shoe_crit) ? 974 : 0);
        if (extra_crit) {
            const auto base = data_->config.attributes.crit;
            const auto rate = [](value_t rating) { return static_cast<int>(rating * 10000. / (9.609 * 990)); };
            stats.critical += rate(base + extra_crit) - rate(base);
        }
        if (active(data_->ring_overcome)) stats.overcome_flat += 313;
        if (active(data_->adaptive_overcome)) stats.overcome_flat += 595;
        if (active(data_->shoe_overcome)) stats.overcome_flat += 974;
        if (active(data_->attack_trinket)) stats.attack_flat += 189;
        if (data_->water != INVALID_SLOT) {
            stats.attack_flat += 22 * Stacks(s, data_->water);
        }
        for (const auto &m : data_->modifiers[effect]) {
            if (m.max_distance && s.target_distance > m.max_distance) continue;
            if (m.resource != INVALID_SLOT) {
                const int value = m.resource == 0 ? s.qidian : m.resource == 1 ? s.energy : s.rage;
                if (value < m.low || value > m.high) continue;
            } else if (m.aura != INVALID_SLOT) {
                const auto stacks = Stacks(s, m.aura);
                if (!stacks || (m.stacks && stacks != m.stacks)) continue;
            }
            Add(stats, m.stats);
            if (m.channel_scale != 1.) {
                const int channel = stats.channel_override >= 0 ? stats.channel_override : book().hits[effect].Channel(rank);
                stats.channel_override = static_cast<int>(channel * m.channel_scale);
            }
        }
        auto &last = last_snapshot_[effect];
        if (last < versions_.size() && versions_[last] == stats) return last;
        last = Version(stats);
        return last;
    }

    template <class E>
    void EquipmentHit(E &e, Slot slot, int base_override = -1)
    {
        if (!data_->pre_damage[slot].empty()) Apply(e, slot, data_->pre_damage[slot]);
        const auto version = Snapshot(e.GetState(), slot);
        const auto base = base_override >= 0 ? base_override : book().hits[slot].fixed;
        e.Log().Record(e.GetState().now, slot, 0, 1, 0, base, 0, RollResult::HIT, slot, 0, 1, version, version);
        if (!data_->post_damage[slot].empty()) {
            const bool prior = last_critical_;
            last_critical_ = false;
            Apply(e, slot, data_->post_damage[slot]);
            last_critical_ = prior;
        }
    }

    template <class E>
    void TriggerEquipment(E &e, bool periodic, bool critical, int overcome_percent)
    {
        const auto now = e.GetState().now;
        const auto &s = e.GetState();
        if (data_->raid_pollen != INVALID_SLOT && now % 800 < 480 && now >= pollen_ready_) {
            if (pollen_window_ != now / 800) {
                pollen_window_ = now / 800;
                pollen_hits_ = 0;
            }
            if (pollen_hits_ < 12) {
                ++pollen_hits_;
                pollen_ready_ = now + 16;
                EquipmentHit(e, data_->raid_pollen, data_->raid_pollen_damage);
            }
        }
        if (data_->bracer_hit != INVALID_SLOT && now >= bracer_ready_ && s.life > .75) {
            bracer_ready_ = now + 160;
            EquipmentHit(e, data_->bracer_hit);
        }
        if (s.target_life < s.life) {
            if (data_->ring_overcome != INVALID_SLOT) SetAura(e, data_->ring_overcome, 1);
            if (data_->ring_crit != INVALID_SLOT) SetAura(e, data_->ring_crit, 1);
        }
        const Slot shoe = data_->shoe_crit != INVALID_SLOT ? data_->shoe_crit : data_->shoe_overcome;
        if (shoe != INVALID_SLOT && now >= trial_shoe_ready_) {
            trial_shoe_ready_ = now + 320;
            SetAura(e, shoe, 1);
        }
        if (data_->adaptive_crit != INVALID_SLOT && now >= adaptive_ready_) {
            adaptive_ready_ = now + 320;
            const auto &a = data_->config.attributes;
            const auto rating = [&](Slot slot, int value) { return slot != INVALID_SLOT ? value * Stacks(s, slot) : 0; };
            const auto crit = a.crit + rating(data_->ring_crit, 313) + rating(data_->shoe_crit, 974);
            const auto overcome = (a.overcome + rating(data_->ring_overcome, 313) + rating(data_->shoe_overcome, 974)) * (1024 + overcome_percent) / 1024;
            SetAura(e, crit < overcome ? data_->adaptive_crit : data_->adaptive_overcome, 1);
        }
        if (data_->attack_trinket != INVALID_SLOT && now >= trinket_ready_) {
            trinket_ready_ = now + 320;
            SetAura(e, data_->attack_trinket, 1, data_->attack_trinket_duration);
        }
        if (data_->trinket_hit != INVALID_SLOT && now >= trinket_ready_) {
            trinket_ready_ = now + 160;
            EquipmentHit(e, data_->trinket_hit);
        }
        if (data_->speed_dot != INVALID_SLOT && now >= speed_dot_ready_) {
            speed_dot_ready_ = now + 640;
            SetAura(e, data_->speed_dot, 1);
        }
        if (data_->legendary_strain_per_stack) SetAura(e, book().legendary_strain, 1);
        if (!periodic && data_->water != INVALID_SLOT) {
            SetAura(e, data_->water, 1);
        }
        if (!periodic && data_->wrist_proc != INVALID_SLOT && now >= wrist_ready_ && rng_.Next() % 100 < 10) {
            wrist_ready_ = now + 160;
            EquipmentHit(e, data_->wrist_proc);
        }
        if (critical && data_->shoe_proc != INVALID_SLOT && now >= shoe_ready_) {
            shoe_ready_ = now + 160;
            EquipmentHit(e, data_->shoe_proc);
        }
        if (!periodic && data_->belt_low != INVALID_SLOT && now >= belt_ready_ && rng_.Next() % 100 < 20) {
            belt_ready_ = now + 480;
            SetAura(e, rng_.Next() % 100 < 30 ? data_->belt_low : data_->belt_high, 1);
        }
    }

    template <class E>
    void Strike(E &e, Slot effect, int rank = 1, std::optional<unsigned> dot_snapshot = {}, int multiplier = 1, int final_bonus = 0)
    {
        if (!data_->pre_damage[effect].empty()) Apply(e, effect, data_->pre_damage[effect]);
        const bool periodic = dot_snapshot.has_value();
        unsigned snapshot = periodic ? *dot_snapshot : Snapshot(e.GetState(), effect, rank);
        if (final_bonus) {
            auto stats = versions_[snapshot];
            stats.final_damage += final_bonus;
            snapshot = Version(stats);
        }
        const auto &hit    = book().hits[effect];
        const auto  roll   = rng_.Next();
        const auto  chance = std::clamp(data_->crit_basis_points + versions_[snapshot].critical, 0, 10000);
        last_critical_     = static_cast<int>(roll % 10000) < chance;
        const auto [fixed, spread] = hit.rank_base_spread.empty() ? std::pair{hit.fixed, hit.spread} : hit.rank_base_spread.at(rank - 1);
        const auto base = fixed + (spread ? rng_.Next() % (spread + 1) : 0);
        const auto live    = periodic ? Snapshot(e.GetState(), effect, rank) : snapshot;
        e.Log().Record(e.GetState().now,
                       effect,
                       0,
                       static_cast<std::uint16_t>(rank),
                       roll,
                       base,
                       0,
                       last_critical_ ? RollResult::DOUBLE : RollResult::HIT,
                       effect,
                       0,
                       static_cast<std::uint16_t>(multiplier),
                       snapshot,
                       live);
        if (hit.triggers_equipment) {
            TriggerEquipment(e, periodic, last_critical_, versions_[live].overcome);
        }
        if (!data_->post_damage[effect].empty()) {
            const bool critical = last_critical_;
            Apply(e, effect, data_->post_damage[effect]);
            last_critical_ = critical;
        }
    }

    template <class E>
    void StartCooldown(E &e, Slot slot, tick_t cd)
    {
        auto       &s = e.MutableState();
        const auto &a = *data_->actions[slot];
        if (cd && a.charges > 1) {
            if (a.stance_cooldown && s.skill_energy[slot] < a.charges) queued_charge_times_[slot].push_back(cd);
            if (a.shared_repayment || s.skill_energy[slot] == a.charges) {
                recharge_at_[slot] = (a.shared_repayment ? std::max(s.now, recharge_at_[slot]) : s.now) + cd;
                e.Timers().ArmAt(cooldown_[slot], recharge_at_[slot]);
            }
            --s.skill_energy[slot];
            e.Log().RecordSkillEnergy(s.now, slot, s.skill_energy[slot]);
            if (!s.skill_energy[slot]) {
                s.skill_ready_at[slot] = recharge_at_[slot];
                e.Log().RecordSkillCooldown(s.now, slot, recharge_at_[slot]);
            }
        } else if (cd) {
            s.skill_ready_at[slot] = s.now + cd;
            e.Log().RecordSkillCooldown(s.now, slot, s.now + cd);
            e.Timers().Arm(cooldown_[slot], cd);
        }
    }

    template <class E>
    void Recharge(E &e, Slot slot)
    {
        auto       &s = e.MutableState();
        const auto &a = *data_->actions[slot];
        if (a.charges > 1 && s.skill_energy[slot] < a.charges) {
            s.skill_energy[slot] = a.shared_repayment ? a.charges : s.skill_energy[slot] + 1;
            e.Log().RecordSkillEnergy(s.now, slot, s.skill_energy[slot]);
            s.skill_ready_at[slot] = s.now;
            e.Log().RecordSkillCooldown(s.now, slot, s.now);
            if (s.skill_energy[slot] < a.charges) {
                auto cd = data_->skills[slot].cooldown;
                if (a.stance_cooldown) {
                    cd = queued_charge_times_[slot].front();
                    queued_charge_times_[slot].erase(queued_charge_times_[slot].begin());
                } else if (a.hasted_cd) cd = Hasted(cd);
                recharge_at_[slot] = s.now + cd;
                e.Timers().ArmAt(cooldown_[slot], recharge_at_[slot]);
            }
        }
    }

    template <class E>
    void ChangeResource(E &e, Slot slot, int delta)
    {
        auto       &s     = e.MutableState();
        const auto &r     = data_->resources[slot];
        int        &value = ResourceAt(s, slot);
        const int   next  = std::clamp(value + delta, r.low, r.high);
        if (value == next) {
            return;
        }
        const int spent = value - next;
        value           = next;
        if (r.mana) {
            s.mana = static_cast<double>(next) / r.high;
        }
        e.Log().RecordResources(s.now, s.qidian, s.energy, s.rage);
        if (r.spent_hit != INVALID_SLOT && spent > 0) {
            for (int i = 0; i < spent / r.spent_per_point; ++i) {
                Strike(e, r.spent_hit);
            }
        }
        if (r.on_spend != INVALID_SLOT && spent >= r.spent_per_point &&
            std::ranges::find(data_->config.talents, r.spend_talent) != data_->config.talents.end())
        {
            ChangeResource(e, r.on_spend, spent / r.spent_per_point);
        }
    }

    template <class E>
    void SetAura(E &e, Slot slot, int change, tick_t duration_override = 0)
    {
        auto       &s       = e.MutableState();
        const auto &a       = book().auras[slot];
        const auto &def     = book().buffs[slot];
        auto       &timer   = timers_[slot];
        auto       &stacks  = (a.target ? s.target_buff_stacks : s.self_buff_stacks)[slot];
        auto       &expires = (a.target ? s.target_buff_expires_at : s.self_buff_expires_at)[slot];
        auto       &tick    = (a.target ? s.target_buff_tick_at : s.self_buff_tick_at)[slot];
        if (!stacks && change <= 0) {
            return;
        }
        const bool refresh = change > 0;
        stacks             = std::clamp(stacks + change, 0, data_->aura_stack_caps[slot]);
        if (!stacks) {
            expires = tick = timer.remaining = 0;
            timer.pending_pulses.clear();
            e.Timers().Cancel(timer.expiry);
            e.Timers().Cancel(timer.tick);
        } else if (refresh) {
            timer.placed_at = s.target_distance;
            const int interval = PulseInterval(s, slot);
            tick_t    duration = duration_override ? duration_override : def.duration;
            if (data_->dot_effects[slot] != INVALID_SLOT) {
                timer.snapshot = Snapshot(s, data_->dot_effects[slot]);
                timer.remaining =
                    a.max_remaining_ticks ? std::min(a.max_remaining_ticks, timer.remaining + data_->dot_ticks[slot])
                                          : data_->dot_ticks[slot];
                if (!timer.remaining) {
                    throw std::logic_error("periodic effect has no ticks");
                }
                duration = interval * timer.remaining;
            }
            if (interval > 0) {
                if (!a.keep_next_tick || tick <= s.now) {
                    tick = s.now + (a.first_tick ? a.first_tick : interval);
                }
                if (data_->dot_effects[slot] != INVALID_SLOT) {
                    duration = tick - s.now + (timer.remaining - 1) * interval;
                }
                if (a.scheduled_pulses) {
                    // Refresh retains outstanding frames; only additional ticks use the new application interval.
                    auto &pending = timer.pending_pulses;
                    if (pending.size() > static_cast<std::size_t>(a.scheduled_pulses)) pending.resize(a.scheduled_pulses);
                    while (pending.size() < static_cast<std::size_t>(a.scheduled_pulses))
                        pending.push_back((pending.empty() ? s.now : pending.back()) + interval);
                    tick = pending.front();
                    duration = pending.back() - s.now;
                }
                e.Timers().ArmAt(timer.tick, tick);
            }
            expires = duration ? s.now + duration : 0;
            if (duration) {
                e.Timers().ArmAt(timer.expiry, expires);
            } else {
                e.Timers().Cancel(timer.expiry);
            }
        }
        e.Log().RecordBuff(s.now, a.target, slot, expires, tick, stacks);
        if (refresh && a.scheduled_pulses) Apply(e, INVALID_SLOT, data_->pulses[slot]);
    }

    template <class E>
    void RecordDistinct(E &e, const Step &step, int id)
    {
        const auto &s = e.GetState();
        if (distinct_until_.at(id) > s.now) {
            for (unsigned i = 0; i < distinct_until_.size(); ++i) {
                if (step.clear_repeated & (1u << i)) {
                    distinct_until_[i] = 0;
                }
            }
        } else {
            distinct_until_[id] = step.duration ? s.now + step.duration : std::numeric_limits<tick_t>::max();
            if (std::ranges::count_if(distinct_until_, [&](tick_t at) { return at > s.now; }) == step.maximum)
            {
                distinct_until_.fill(0);
                SetAura(e, step.slot, 1);
            }
        }
    }

    template <class E>
    struct SchoolFrame final : EffectContext
    {
        Rules &rules;
        E &engine;
        SchoolFrame(Rules &r, E &e, Slot action) : EffectContext(e.GetState(), action == INVALID_SLOT ? nullptr : r.data_->actions[action]), rules(r), engine(e) {}
        int Stacks(Slot slot) const override { return rules.Stacks(state, slot); }
        int Resource(Slot slot) const override { return ResourceAt(engine.MutableState(), slot); }
        void ChangeResource(Slot slot, int delta) override { rules.ChangeResource(engine, slot, delta); }
        void SetAura(Slot slot, int delta, tick_t duration) override { rules.SetAura(engine, slot, delta, duration); }
        void Hit(Slot slot, int rank) override { rules.Strike(engine, slot, rank); }
        void HitWithBonus(Slot slot, int rank, int bonus) override
        {
            rules.Strike(engine, slot, rank, {}, 1, bonus);
        }
        bool TryStartGcd(Slot group, tick_t frames) override
        {
            if (state.gcd_ready_at[group] > state.now) return false;
            const auto ready = state.now + rules.Hasted(frames);
            SetGcd(engine.MutableState(), engine.Log(), group, ready);
            engine.Timers().ArmAt(rules.gcd_[group], ready);
            return true;
        }
        void StartCooldown(Slot slot) override { rules.StartCooldown(engine, slot, rules.data_->skills[slot].cooldown); }
        uint64_t Roll() override { return rules.rng_.Next(); }
        void RecordDistinct(const Step &step, int id) override { rules.RecordDistinct(engine, step, id); }
    };

    template <class E>
    void Apply(E &e, Slot action, const std::vector<Step> &steps)
    {
        auto &s = e.MutableState();
        for (const auto &step : steps) {
            if (step.range) {
                const double distance = TargetDistance(s, step.range_origin);
                if ((step.range > 0 && distance > step.range) || (step.range < 0 && distance <= -step.range)) continue;
            }
            if ((step.captured && !captured_) || (step.empowered_channel && !empowered_channel_)) {
                continue;
            }
            if (step.blocked_by != INVALID_SLOT && Stacks(s, step.blocked_by)) continue;
            if (step.condition != INVALID_SLOT && ((Stacks(s, step.condition) > 0) == step.absent)) {
                continue;
            }
            if ((step.exact_stacks && Stacks(s, step.condition) != step.exact_stacks) ||
                (step.min_stacks && Stacks(s, step.condition) < step.min_stacks))
            {
                continue;
            }
            if (step.minimum_aura_time && s.self_buff_expires_at[step.condition] - s.now < step.minimum_aura_time) continue;
            if (step.resource != INVALID_SLOT &&
                (ResourceAt(s, step.resource) < step.minimum || ResourceAt(s, step.resource) > step.maximum))
            {
                continue;
            }
            const int life = static_cast<int>(std::lround(s.target_life * 10000));
            if (life <= step.life_low || life > step.life_high) {
                continue;
            }
            const Slot slot = step.slot;
            const int  n    = step.amount;
            switch (step.op) {
                case Op::RecordDistinct: RecordDistinct(e, step, n); break;
                case Op::School: {
                    SchoolFrame<E> frame(*this, e, action);
                    step.effect(frame, step);
                    break;
                }
                case Op::SpendStackGroups:
                {
                    const int count = Stacks(s, slot) / n;
                    if (count) {
                        SetAura(e, slot, -n * count);
                        for (int i = 0; i < count; ++i) {
                            Strike(e, step.extra);
                        }
                    }
                    break;
                }
                case Op::StackStageHit:
                {
                    const int stage = Stacks(s, step.extra);
                    if (stage > 0) {
                        const auto effect = static_cast<Slot>(slot + stage - 1);
                        Strike(e, effect);
                    }
                    break;
                }
                case Op::StackRankHit:
                {
                    const int rank = Stacks(s, static_cast<Slot>(n));
                    if (rank) {
                        Strike(e, slot, rank);
                    }
                    break;
                }
                case Op::HitsPerPoint:
                case Op::HitsPerStack:
                case Op::Hit:
                {
                    const int count = step.op == Op::Hit            ? n
                                      : step.op == Op::HitsPerStack ? Stacks(s, static_cast<Slot>(n))
                                                                    : ResourceAt(s, static_cast<Slot>(n));
                    for (int i = 0; i < count; ++i) {
                        Strike(e, slot);
                    }
                    break;
                }
                case Op::DelayedHit:
                {
                    const int rank = step.extra == INVALID_SLOT ? 1 : Stacks(s, step.extra) + step.rank_offset;
                    if (!rank) break;
                    delayed_.push({ s.now + n, delay_sequence_++, slot, rank });
                    e.Timers().ArmAt(delayed_timer_, delayed_.top().frame);
                    break;
                }
                case Op::PulseHit:
                {
                    const auto &a    = *data_->actions[action];
                    const int   rank = completed_pulses_ + 1;
                    Strike(e, slot, rank);
                    break;
                }
                case Op::ProcHit:
                    if (static_cast<int>(rng_.Next() & 1023) < n) {
                        Strike(e, slot);
                    }
                    break;
                case Op::Add: SetAura(e, slot, n, step.duration); break;
                case Op::RefreshExisting:
                    if (Stacks(s, slot)) SetAura(e, slot, 1);
                    break;
                case Op::AddOnCrit:
                    if (last_critical_) {
                        SetAura(e, slot, n);
                    }
                    break;
                case Op::ResourceAura:
                {
                    const auto r = static_cast<Slot>(n);
                    SetAura(e, slot, 1, book().buffs[slot].duration * ResourceAt(s, r) / data_->resources[r].high);
                    break;
                }
                case Op::ResourceStacks:
                {
                    const int delta = ResourceAt(s, static_cast<Slot>(n)) - Stacks(s, slot);
                    if (delta) {
                        SetAura(e, slot, delta);
                    }
                    break;
                }
                case Op::Remove: SetAura(e, slot, -n); break;
                case Op::Resource: ChangeResource(e, slot, n); break;
                case Op::Burst:
                {
                    auto     &t      = timers_[slot];
                    const int stacks = Stacks(s, slot);
                    if (stacks && t.remaining) {
                        Strike(e, data_->dot_effects[slot], 1, t.snapshot, t.remaining * stacks);
                        SetAura(e, slot, -stacks);
                    }
                    break;
                }
                case Op::ManaQuarters:
                {
                    const int value = std::clamp(static_cast<int>(std::lround(s.mana * 400)) + n, 0, 400);
                    ChangeResource(e, slot, value / 4 - ResourceAt(s, slot));
                    s.mana = value / 400.;
                    break;
                }
                // Convert an active periodic effect to its faster tick period, preserving its expiry frame.
                case Op::CompressPeriodic:
                {
                    const auto end = s.target_buff_expires_at[slot];
                    if (!Stacks(s, slot) || end <= s.now) break;
                    SetAura(e, slot, -Stacks(s, slot));
                    const Slot next = step.extra;
                    SetAura(e, next, 1);
                    const int interval = PulseInterval(s, next);
                    timers_[next].remaining = (end - s.now + interval - 1) / interval;
                    // The last tick lands on the original expiry, including a partial first interval.
                    const auto first = end - (timers_[next].remaining - 1) * interval;
                    s.target_buff_expires_at[next] = end;
                    s.target_buff_tick_at[next] = first;
                    e.Timers().ArmAt(timers_[next].expiry, end);
                    e.Timers().ArmAt(timers_[next].tick, first);
                    e.Log().RecordBuff(s.now, true, next, end, first, 1);
                    break;
                }
                case Op::AdvanceAndRestartTicks:
                case Op::AdvanceTicks:
                {
                    auto       &t     = timers_[slot];
                    const auto &a     = book().auras[slot];
                    const int   count = std::min(t.remaining, n), stacks = Stacks(s, slot);
                    if (!stacks || !count) {
                        break;
                    }
                    for (int i = 0; i < count; ++i) {
                        Strike(e, data_->dot_effects[slot], 1, t.snapshot, stacks);
                    }
                    t.remaining -= count;
                    if (!t.remaining) {
                        SetAura(e, slot, -stacks);
                    } else {
                        auto      &next     = (a.target ? s.target_buff_tick_at : s.self_buff_tick_at)[slot];
                        const auto interval = PulseInterval(s, slot);
                        if (step.op == Op::AdvanceAndRestartTicks) {
                            next = s.now + interval;
                            e.Timers().ArmAt(t.tick, next);
                        }
                        auto &expiry = (a.target ? s.target_buff_expires_at : s.self_buff_expires_at)[slot];
                        expiry       = next + (t.remaining - 1) * interval;
                        e.Timers().ArmAt(t.expiry, expiry);
                        e.Log().RecordBuff(s.now, a.target, slot, expiry, next, stacks);
                    }
                    break;
                }
                case Op::ExtraTick:
                {
                    const auto stacks = Stacks(s, slot);
                    if (stacks && timers_[slot].remaining > 0) {
                        Strike(e, data_->dot_effects[slot], 1, timers_[slot].snapshot, stacks);
                    }
                    break;
                }
                case Op::ResourceHit:
                {
                    const auto level = ResourceAt(s, static_cast<Slot>(n));
                    if (level > 0) {
                        const Slot effect = static_cast<Slot>(slot + level - 1);
                        Strike(e, effect);
                    }
                    break;
                }
                case Op::CycleHit:
                {
                    if (s.now >= combo_until_[action]) {
                        combo_[action] = 0;
                    }
                    const Slot effect = static_cast<Slot>(slot + combo_[action]);
                    Strike(e, effect);
                    combo_[action]       = (combo_[action] + 1) % n;
                    combo_until_[action] = s.now + data_->actions[action]->chain_window;
                    break;
                }
                case Op::ProlongAura:
                {
                    if (!Stacks(s, slot)) {
                        break;
                    }
                    auto &expires = (book().auras[slot].target ? s.target_buff_expires_at : s.self_buff_expires_at)[slot];
                    auto &pending = timers_[slot].pending_pulses;
                    if (!pending.empty()) {
                        const int interval = PulseInterval(s, slot);
                        const int count = std::max(1, (n + interval / 2) / interval);
                        for (int i = 0; i < count; ++i) pending.push_back(pending.back() + interval);
                        expires = pending.back();
                    } else expires = step.duration ? std::min(expires + n, s.now + step.duration) : expires + n;
                    e.Timers().ArmAt(timers_[slot].expiry, expires);
                    const auto next = (book().auras[slot].target ? s.target_buff_tick_at : s.self_buff_tick_at)[slot];
                    e.Log().RecordBuff(s.now, book().auras[slot].target, slot, expires, next, Stacks(s, slot));
                    break;
                }
                case Op::SpawnField:
                case Op::ExtendLatestField:
                {
                    // Each field owns its existing tick clock. Only the newest one may be extended.
                    auto &index = timers_[slot].newest;
                    if (step.op == Op::SpawnField) {
                        index = (index + 1) % n;
                    }
                    if (index < 0) {
                        break;
                    }
                    const Slot field    = static_cast<Slot>(slot + index);
                    const int  interval = book().buffs[field].interval;
                    if (step.op == Op::SpawnField) {
                        SetAura(e, field, 1, step.duration);
                    } else {
                        const auto next = s.target_buff_tick_at[field];
                        if (!Stacks(s, field) || next <= s.now) {
                            break;
                        }
                        auto &end = s.target_buff_expires_at[field];
                        end       = std::min(end + step.duration, next + (step.maximum - 1) * interval);
                        e.Timers().ArmAt(timers_[field].expiry, end);
                        e.Log().RecordBuff(s.now, true, field, end, next, 1);
                    }
                    const auto end = std::max(s.self_buff_expires_at[step.extra], s.target_buff_expires_at[field]);
                    SetAura(e, step.extra, 1, end - s.now);
                    break;
                }
                case Op::RetimePulse:
                {
                    if (!Stacks(s, slot)) {
                        break;
                    }
                    const auto next           = std::max(s.now, timers_[slot].last_pulse + PulseInterval(s, slot));
                    s.self_buff_tick_at[slot] = next;
                    e.Timers().ArmAt(timers_[slot].tick, next);
                    e.Log().RecordBuff(s.now, false, slot, s.self_buff_expires_at[slot], next, Stacks(s, slot));
                    break;
                }
                case Op::ConvertStacks:
                {
                    const int count = Stacks(s, slot);
                    if (count) {
                        SetAura(e, slot, -count);
                        SetAura(e, step.extra, count);
                    }
                    break;
                }
                case Op::ToggleAura: SetAura(e, slot, Stacks(s, slot) ? -1 : 1, step.duration); break;
                case Op::BuildCharge:
                {
                    if (Stacks(s, step.extra) < book().buffs[step.extra].max_stacks) {
                        if (Stacks(s, slot) + 1 >= n) {
                            SetAura(e, slot, -n);
                            SetAura(e, step.extra, 1);
                        } else {
                            SetAura(e, slot, 1);
                        }
                    }
                    break;
                }
                case Op::ChanceAura:
                    if (static_cast<int>(rng_.Next() % 10000) < n) {
                        SetAura(e, slot, 1);
                    }
                    break;
                case Op::SpendCharge:
                    StartCooldown(e, slot, data_->skills[slot].cooldown);
                    break;
                case Op::RemainingTimeHit:
                {
                    const auto left = std::max(0, s.self_buff_expires_at[step.extra] - s.now);
                    const int rank = std::clamp((left + 15) / 16, 1, static_cast<int>(book().hits[slot].rank_channels.size()));
                    Strike(e, slot, rank);
                    break;
                }
                case Op::ExtendAura:
                {
                    const auto end = (book().auras[slot].target ? s.target_buff_expires_at : s.self_buff_expires_at)[slot];
                    const auto remaining = std::max<tick_t>(0, end - s.now);
                    const auto duration = remaining + step.duration;
                    if (duration <= 0) SetAura(e, slot, -Stacks(s, slot));
                    else SetAura(e, slot, 1, n > 1 ? std::min(duration, n) : duration);
                    break;
                }
                case Op::ResetGcd:
                case Op::ShortenGcd:
                    if (s.gcd_ready_at[slot] > s.now) {
                        const auto ready = step.op == Op::ResetGcd ? s.now : std::max(s.now, s.gcd_ready_at[slot] - n);
                        SetGcd(s, e.Log(), slot, ready);
                        e.Timers().ArmAt(gcd_[slot], ready);
                    }
                    break;
                case Op::ApproachTarget:
                    if (s.target_distance > n) {
                        s.target_distance = 4.;
                        e.Log().RecordTarget(s.now, MutationKind::TARGET_DISTANCE, 400);
                    }
                    break;
                case Op::ShortenRemainingCd:
                case Op::SuppressCooldown:
                case Op::ShortenCd:
                {
                    int refund = step.op == Op::ShortenRemainingCd ? std::max(0, s.skill_ready_at[slot] - s.now) * n / 100 : n;
                    if (step.op == Op::SuppressCooldown) {
                        const auto end = data_->actions[slot]->charges > 1 ? recharge_at_[slot] : s.skill_ready_at[slot];
                        const auto remaining = std::max<tick_t>(0, end - s.now);
                        const auto window    = std::max<tick_t>(0, s.self_buff_expires_at[step.extra] - s.now);
                        refund               = std::min(window, remaining) * n / 1024;
                        if (!refund) {
                            break;
                        }
                    }
                    const auto charges = data_->actions[slot]->charges;
                    if (charges > 1) {
                        if (s.skill_energy[slot] >= charges) {
                            break;
                        }
                        // A refund advances the entire serial recharge queue, including the
                        // following charge if the first one finishes during the refund.
                        tick_t credit = refund;
                        while (s.skill_energy[slot] < charges) {
                            const auto left = std::max<tick_t>(0, recharge_at_[slot] - s.now);
                            if (credit < left) {
                                recharge_at_[slot] -= credit;
                                if (!s.skill_energy[slot]) {
                                    s.skill_ready_at[slot] = recharge_at_[slot];
                                    e.Log().RecordSkillCooldown(s.now, slot, recharge_at_[slot]);
                                }
                                e.Timers().ArmAt(cooldown_[slot], recharge_at_[slot]);
                                break;
                            }
                            credit -= left;
                            e.Timers().Cancel(cooldown_[slot]);
                            Recharge(e, slot);
                        }
                        break;
                    }
                    const auto ready = std::max(s.now, s.skill_ready_at[slot] - refund);
                    if (s.skill_ready_at[slot] > s.now) {
                        s.skill_ready_at[slot] = ready;
                        if (ready == s.now) {
                            e.Timers().Cancel(cooldown_[slot]);
                        } else {
                            e.Timers().ArmAt(cooldown_[slot], ready);
                        }
                        e.Log().RecordSkillCooldown(s.now, slot, ready);
                    }
                    break;
                }
                case Op::RollResetCd:
                    if (s.skill_ready_at[slot] <= s.now || static_cast<int>(rng_.Next() % 10000) >= n) {
                        break;
                    }
                    [[fallthrough]];
                case Op::ResetCd:
                {
                    s.skill_ready_at[slot] = s.now;
                    e.Timers().Cancel(cooldown_[slot]);
                    recharge_at_[slot] = 0;
                    if (data_->actions[slot]->stance_cooldown) queued_charge_times_[slot].clear();
                    if (data_->actions[slot]->charges > 1) {
                        s.skill_energy[slot] = data_->actions[slot]->charges;
                        e.Log().RecordSkillEnergy(s.now, slot, s.skill_energy[slot]);
                    }
                    e.Log().RecordSkillCooldown(s.now, slot, s.now);
                    break;
                }
            }
        }
    }

    template <class E>
    void CastState(E &e, Slot slot, tick_t frame, bool channel)
    {
        auto &s                                                   = e.MutableState();
        (channel ? s.skill_casting_at : s.skill_prepare_at)[slot] = frame;
        (channel ? s.casting_skill : s.preparing_skill)           = frame ? slot : INVALID_SLOT;
        if (channel) {
            const auto marker=data_->actions[slot]->clear_on_channel_end;
            if (!frame && marker!=INVALID_SLOT) SetAura(e,marker,-Stacks(s,marker));
            e.Log().RecordCasting(s.now, slot, frame, frame != 0);
        } else {
            e.Log().RecordPrepare(s.now, slot, frame, frame != 0);
        }
    }

public:
    explicit Rules(Preparation p) : data_(std::move(p)) { }

    const Prepared &Data() const { return *data_; }

    const auto &Versions() const { return versions_; }

    value_t Reduce(const DamageIntent &h) const
    {
        return Damage(data_->config, h, versions_[h.snapshot_version], versions_[h.live_version]);
    }

    template <class E>
    void Setup(E &e)
    {
        const auto &b = book();
        if (e.GetState().skill_ready_at.size() != b.skills.size() || e.GetState().self_buff_stacks.size() != b.buffs.size())
        {
            throw std::invalid_argument("macro does not match the specialization");
        }
        for (Slot i = 0; i < b.skills.size(); ++i) {
            cooldown_.push_back(e.Timers().Create({ TimerKind::SKILL_COOLDOWN, i }, 2));
        }
        for (Slot i = 0; i < b.buffs.size(); ++i) {
            timers_.push_back({ e.Timers().Create({ TimerKind::BUFF_DURATION, i }, 20),
                                e.Timers().Create({ TimerKind::BUFF_INTERVAL, i }, 10) });
        }
        for (Slot i = 0; i < b.resources.size(); ++i) {
            regeneration_.push_back(e.Timers().Create({ TimerKind::RESOURCE_REGEN, i }, 5));
        }
        for (auto &timer : gcd_) {
            timer = e.Timers().Create({ TimerKind::MACRO_WAKE }, 2);
        }
        cast_          = e.Timers().Create({ TimerKind::SKILL_CASTING }, 15);
        delayed_timer_ = e.Timers().Create({ TimerKind::FIELD }, 12);
        last_snapshot_.resize(b.skills.size());
        combo_.resize(b.skills.size());
        combo_until_.resize(b.skills.size());
        recharge_at_.resize(b.skills.size());
        queued_charge_times_.resize(b.skills.size());
        timeline_.Setup(e, data_->config.target_events);
    }

    template <class E>
    void Reset(E &e, std::uint64_t seed)
    {
        auto &s  = e.MutableState();
        s.life   = data_->config.player_life;
        s.qidian = s.energy = s.rage = 0;
        s.target_life                = data_->config.target.life;
        s.target_distance            = data_->config.target.distance;
        for (Slot i : book().castable) {
            s.skill_gcd_group[i] = data_->skills[i].gcd ? data_->actions[i]->gcd_group : INVALID_SLOT;
            s.skill_energy[i]    = data_->actions[i]->charges;
        }
        for (Slot i = 0; i < data_->resources.size(); ++i) {
            const auto &r    = data_->resources[i];
            ResourceAt(s, i) = i == 0 ? data_->config.initial_qidian.value_or(r.initial) : r.initial;
            if (r.mana) {
                s.mana = static_cast<double>(ResourceAt(s, i)) / r.high;
            }
            if (r.regen_interval) {
                e.Timers().Arm(regeneration_[i], r.first_regen ? r.first_regen : r.regen_interval);
            }
        }
        for (auto &t : timers_) {
            t.remaining  = 0;
            t.pending_pulses.clear();
            t.newest     = -1;
            t.last_pulse = 0;
            t.snapshot   = 0;
        }
        pending_       = INVALID_SLOT;
        pulses_left_   = 0;
        last_critical_ = false;
        versions_.clear();
        std::ranges::fill(last_snapshot_, std::numeric_limits<unsigned>::max());
        wrist_ready_ = shoe_ready_ = belt_ready_ = 0;
        bracer_ready_ = adaptive_ready_ = trinket_ready_ = 0;
        speed_dot_ready_ = trial_shoe_ready_ = 0;
        pollen_window_ = -1;
        pollen_ready_ = pollen_hits_ = 0;
        captured_                                = false;
        finish_cd_                               = 0;
        deferred_swing_due_ = -1;
        while (!delayed_.empty()) {
            delayed_.pop();
        }
        delay_sequence_ = 0;
        distinct_until_.fill(0);
        std::ranges::fill(combo_, 0);
        std::ranges::fill(combo_until_, 0);
        std::ranges::fill(recharge_at_, 0);
        for (auto &queue : queued_charge_times_) queue.clear();
        for (const auto aura : book().initial_auras) {
            SetAura(e, aura, 1);
        }
        rng_.Reset(seed);
        Apply(e, INVALID_SLOT, data_->opening);
        for (auto [slot, elapsed] : data_->opening_auras_elapsed) {
            SetAura(e, slot, 1);
            const auto &a = book().auras[slot];
            auto &expires = (a.target ? s.target_buff_expires_at : s.self_buff_expires_at)[slot];
            auto &tick = (a.target ? s.target_buff_tick_at : s.self_buff_tick_at)[slot];
            if (elapsed >= tick) throw std::logic_error("pre-pull aura must precede its first tick");
            e.Timers().ArmAt(timers_[slot].expiry, expires -= elapsed);
            e.Timers().ArmAt(timers_[slot].tick, tick -= elapsed);
            e.Log().RecordBuff(s.now, a.target, slot, expires, tick, 1);
        }
        for (auto [skill, remaining] : data_->opening_cooldowns) StartCooldown(e, skill, remaining);
        if (data_->opening_gcd > 0) {
            s.gcd_ready_at[0] = data_->opening_gcd;
            e.Timers().ArmAt(gcd_[0], data_->opening_gcd);
            e.Log().RecordGcd(s.now, 0, data_->opening_gcd);
        }
        timeline_.Reset(e, data_->config.target_events);
        if (data_->distant_target_aura != INVALID_SLOT && s.target_distance > data_->distant_threshold) SetAura(e, data_->distant_target_aura, 1);
    }

    bool CanAttempt(const State &s, const MacroLine &line) const
    {
        const auto slot = line.action_slot;
        const auto &a = *data_->actions[slot];
        if (pending_ != INVALID_SLOT && !a.cast_while_channeling && !IsForceCast(line.command)) {
            return false;
        }
        if (s.skill_ready_at[slot] > s.now && a.bypass_cooldown_aura == INVALID_SLOT) {
            return false;
        }
        return !data_->skills[slot].gcd || a.alternate_gcd_aura != INVALID_SLOT || s.gcd_ready_at[a.gcd_group] <= s.now;
    }

    template <class E>
    bool Cast(E &e, const MacroLine &line, int pulse_limit = 0)
    {
        auto       &s    = e.MutableState();
        const auto  slot = line.action_slot;
        const auto &a    = *data_->actions[slot];
        const auto &d    = data_->skills[slot];
        const auto reject = [&](ActionBlockReason reason, double actual = 0, double required = 0, Slot subject = INVALID_SLOT) {
            if constexpr (E::debug_enabled) {
                e.RejectAction(reason, actual, required, subject);
            }
            return false;
        };
        if (!data_->enabled_actions[slot]) {
            return reject(ActionBlockReason::MISSING_TALENT);
        }
        const int range = a.range_aura != INVALID_SLOT && Stacks(s, a.range_aura) ? a.aura_range : data_->ranges[slot];
        if (range) {
            const double distance = TargetDistance(s, a.range_origin);
            if (distance > range) return reject(ActionBlockReason::DISTANCE, distance, range);
        }
        const bool bypass_cooldown_aura = a.bypass_cooldown_aura != INVALID_SLOT && Stacks(s, a.bypass_cooldown_aura);
        const bool weapon_free_cost = a.weapon_free_cost != INVALID_SLOT && Stacks(s, a.weapon_free_cost);
        if (s.skill_ready_at[slot] > s.now && !bypass_cooldown_aura) {
            return reject(ActionBlockReason::SKILL_COOLDOWN, s.skill_ready_at[slot] - s.now);
        }
        const int mana_cost = data_->mana_cost_quarters[slot];
        if (mana_cost && !weapon_free_cost && std::lround(s.mana * 400) < mana_cost) {
            return reject(ActionBlockReason::QIDIAN, s.mana * 100, mana_cost / 4.);
        }
        if (a.check_gcd_group != INVALID_SLOT && s.gcd_ready_at[a.check_gcd_group] > s.now) {
            return reject(ActionBlockReason::GLOBAL_COOLDOWN, s.gcd_ready_at[a.check_gcd_group] - s.now);
        }
        const bool alternate_group = a.alternate_gcd_aura != INVALID_SLOT && Stacks(s, a.alternate_gcd_aura);
        const auto group = alternate_group ? a.alternate_gcd_group : a.gcd_group;
        if (d.gcd && s.gcd_ready_at[group] > s.now) {
            return reject(ActionBlockReason::GLOBAL_COOLDOWN, s.gcd_ready_at[group] - s.now);
        }
        if ((a.required != INVALID_SLOT && Stacks(s, a.required) < a.required_layers && (a.execute_below < 0 || s.target_life * 10000 >= a.execute_below)) ||
            (a.forbidden != INVALID_SLOT && Stacks(s, a.forbidden)) ||
            (a.also_forbidden != INVALID_SLOT && Stacks(s, a.also_forbidden)))
        {
            return reject(ActionBlockReason::MISSING_BUFF);
        }
        if (a.minimum_aura_time && s.self_buff_expires_at[a.required] - s.now < a.minimum_aura_time) return reject(ActionBlockReason::MISSING_BUFF);
        if (a.resource != INVALID_SLOT && !weapon_free_cost && ResourceAt(s, a.resource) < a.cost &&
            (a.free_cost == INVALID_SLOT || !Stacks(s, a.free_cost)))
        {
            return reject(ActionBlockReason::QIDIAN, ResourceAt(s, a.resource), a.cost, a.resource);
        }
        if (pending_ != INVALID_SLOT && a.cast_while_channeling) {
            // These instant, independent buttons keep the current channel clock and captured state.
            if (!bypass_cooldown_aura) StartCooldown(e, slot, d.cooldown);
            if (d.gcd) {
                const auto lock = std::max(a.lock, StanceTime(s, d.gcd)) + data_->config.delay;
                s.gcd_ready_at[group] = s.now + lock;
                e.Log().RecordGcd(s.now, group, s.now + lock);
                e.Timers().Arm(gcd_[group], lock);
            }
            const bool saved_crit = last_critical_;
            Apply(e, slot, data_->pre_cast[slot]);
            Apply(e, slot, data_->impacts[slot]);
            Apply(e, slot, data_->post_cast[slot]);
            last_critical_ = saved_crit;
            return true;
        }
        if (pending_ != INVALID_SLOT) {
            if (!IsForceCast(line.command)) {
                return reject(s.casting_skill != INVALID_SLOT ? ActionBlockReason::CHANNELING : ActionBlockReason::PREPARING,
                              (s.casting_skill != INVALID_SLOT ? s.skill_casting_at : s.skill_prepare_at)[pending_] - s.now,
                              0,
                              pending_);
            }
            const bool releasing_same_charge = pending_ == slot && data_->actions[pending_]->release_charged;
            if constexpr (E::debug_enabled) {
                e.NoteChannelInterrupt(pending_);
            }
            if (data_->actions[pending_]->release_charged) {
                if (completed_pulses_) Apply(e, pending_, data_->post_cast[pending_]);
                if (finish_cd_) StartCooldown(e, pending_, finish_cd_);
                const auto swing = data_->actions[pending_]->defer_swing;
                if (deferred_swing_due_ >= 0 && swing != INVALID_SLOT && Stacks(s, swing)) {
                    const auto next = std::max(s.now, deferred_swing_due_);
                    s.self_buff_tick_at[swing] = next;
                    e.Timers().ArmAt(timers_[swing].tick, next);
                    e.Log().RecordBuff(s.now, false, swing, s.self_buff_expires_at[swing], next, Stacks(s, swing));
                }
            }
            CastState(e, pending_, 0, data_->actions[pending_]->pulses > 1);
            pending_ = INVALID_SLOT;
            e.Timers().Cancel(cast_);
            if (releasing_same_charge) return true;
        }
        tick_t cd = a.stance_cooldown ? StanceTime(s, d.cooldown) : a.hasted_cd ? Hasted(d.cooldown) : d.cooldown;
        cd = std::max(0, cd - data_->cooldown_reduction[slot]);
        if (a.cooldown_aura != INVALID_SLOT && Stacks(s, a.cooldown_aura)) {
            cd = a.aura_cooldown;
        }
        if (bypass_cooldown_aura || (a.free_cd != INVALID_SLOT && Stacks(s, a.free_cd))) {
            cd = 0;
        }
        captured_ = a.capture != INVALID_SLOT && Stacks(s, a.capture) > 0;
        const bool precast = a.precombat_cast && data_->config.precombat && !s.now && s.last_skill == INVALID_SLOT;
        const bool instant = precast ||
            (a.instant != INVALID_SLOT && Stacks(s, a.instant) > 0) ||
            (a.alternate_instant != INVALID_SLOT && Stacks(s, a.alternate_instant) > 0) ||
            (a.instant_at && a.timing_resource != INVALID_SLOT && ResourceAt(s, a.timing_resource) >= a.instant_at);
        const int points   = a.timing_resource == INVALID_SLOT ? 0 : ResourceAt(s, a.timing_resource);
        empowered_channel_ = a.channel_buff != INVALID_SLOT && Stacks(s, a.channel_buff);
        int       pulses   = empowered_channel_   ? a.buff_pulses
                             : a.pulses_per_point ? std::clamp(points / a.points_per_pulse, 1, a.pulses)
                                                  : a.pulses;
        const int spacing  = a.empowered_spacing && points > 0 ? a.empowered_spacing : a.spacing;
        channel_steps_ =
            a.channel_cost_step ? std::min(a.max_cost_steps, (ResourceAt(s, a.resource) - a.cost) / a.channel_cost_step) : 0;
        pulses                 += channel_steps_ * a.extra_pulses;
        const auto cast_time    = a.pulses > 1 ? spacing : d.cast_time;
        channel_pulses_         = pulses;
        const bool distributed  = a.channel_buff != INVALID_SLOT || a.distribute_pulses;
        channel_duration_       = distributed ? StanceTime(s, spacing * pulses) : Hasted(a.channel_duration);
        channel_start_          = s.now;
        const auto delay =
            instant             ? 0
            : channel_duration_ ? ChannelOffset(a, 1)
            : a.fixed_cast_time ? cast_time
                                : a.stance_cast ? StanceTime(s, cast_time) : Hasted(cast_time);
        finish_cd_ = 0;
        if (a.cooldown_on_finish && delay) {
            finish_cd_ = cd;
            cd         = 0;
        }
        StartCooldown(e, slot, cd);
        if (d.gcd && !precast) {
            const auto gcd              = alternate_group ? d.gcd : a.ground_gcd ? Hasted(d.gcd) : StanceTime(s, d.gcd);
            const auto lock             = std::max(a.lock, gcd - data_->gcd_reduction[slot]) + data_->config.delay;
            s.gcd_ready_at[group] = s.now + lock;
            e.Log().RecordGcd(s.now, group, s.now + lock);
            e.Timers().Arm(gcd_[group], lock);
        }
        Apply(e, slot, data_->pre_cast[slot]);
        if (delay) {
            deferred_swing_due_ = -1;
            if (a.defer_swing != INVALID_SLOT && Stacks(s, a.defer_swing)) {
                auto &next = s.self_buff_tick_at[a.defer_swing];
                const tick_t resume = s.now + (a.release_charged ? delay * pulses : delay);
                if (next < resume) {
                    deferred_swing_due_ = next;
                    next = resume;
                    e.Timers().ArmAt(timers_[a.defer_swing].tick, next);
                    e.Log().RecordBuff(s.now, false, a.defer_swing, 0, next, Stacks(s, a.defer_swing));
                }
            }
            if (a.channel_cost_step) {
                ChangeResource(e, a.resource, -(a.cost + channel_steps_ * a.channel_cost_step));
            }
            if (a.refund_channel_point != INVALID_SLOT && Stacks(s,a.refund_channel_point)) ChangeResource(e,a.timing_resource,1);
            pending_       = slot;
            pulse_spacing_ = delay;
            completed_pulses_ = 0;
            pulses_left_   = pulse_limit > 0 ? std::min(pulses, pulse_limit) : pulses;
            const auto end = channel_duration_ ? ChannelOffset(a, pulses_left_) : delay * pulses_left_;
            CastState(e, slot, s.now + end, a.pulses > 1);
            e.Timers().Arm(cast_, delay);
        } else {
            last_critical_ = false;
            const int ticks = precast && a.release_charged ? pulses : 1;
            for (int i = 0; i < ticks; ++i) Apply(e, slot, data_->impacts[slot]);
            Apply(e, slot, data_->post_cast[slot]);
        }
        return true;
    }

    template <class E>
    void OnTimer(E &e, TimerHandle h, Timer)
    {
        auto      &s    = e.MutableState();
        const auto slot = h.slot;
        switch (h.kind) {
            case TimerKind::SKILL_COOLDOWN:
            {
                Recharge(e, slot);
                break;
            }
            case TimerKind::FIELD:
                while (!delayed_.empty() && delayed_.top().frame == s.now) {
                    const auto event = delayed_.top();
                    delayed_.pop();
                    Strike(e, event.effect, event.rank);
                }
                if (!delayed_.empty()) {
                    e.Timers().ArmAt(delayed_timer_, delayed_.top().frame);
                }
                break;
            case TimerKind::TARGET_CHANGE:
            {
                const bool was_distant = s.target_distance > data_->distant_threshold;
                timeline_.Dispatch(e, data_->config.target_events, [&](int damage) {
                    if (damage <= 3000) return;
                    if (data_->shoe_crit != INVALID_SLOT) SetAura(e, data_->shoe_crit, -1);
                    if (data_->shoe_overcome != INVALID_SLOT) SetAura(e, data_->shoe_overcome, -1);
                });
                const bool distant = s.target_distance > data_->distant_threshold;
                if (data_->distant_target_aura != INVALID_SLOT && distant != was_distant)
                    SetAura(e, data_->distant_target_aura, 1, distant ? 0 : data_->distant_linger);
                break;
            }
            case TimerKind::RESOURCE_REGEN:
            {
                const auto &r = data_->resources[slot];
                ChangeResource(e, slot, r.regen);
                e.Timers().Arm(regeneration_[slot], r.regen_interval);
                break;
            }
            case TimerKind::BUFF_DURATION:
                SetAura(e, slot, -Stacks(s, slot));
                Apply(e, INVALID_SLOT, data_->expires[slot]);
                break;
            case TimerKind::BUFF_INTERVAL:
            {
                const auto &a = book().auras[slot];
                auto       &t = timers_[slot];
                if (!Stacks(s, slot)) {
                    break;
                }
                if (data_->dot_effects[slot] != INVALID_SLOT && t.remaining > 0) {
                    const int stacks = Stacks(s, slot);
                    if (book().hits[data_->dot_effects[slot]].fixed_equipment_damage) {
                        EquipmentHit(e, data_->dot_effects[slot]);
                    } else if (a.separate_stack_hits) {
                        for (int i = 0; i < stacks; ++i) {
                            Strike(e, data_->dot_effects[slot], 1, t.snapshot, 1);
                        }
                    } else {
                        Strike(e, data_->dot_effects[slot], 1, t.snapshot, stacks);
                    }
                    --t.remaining;
                }
                if (!t.pending_pulses.empty()) t.pending_pulses.erase(t.pending_pulses.begin());
                t.last_pulse = s.now;
                Apply(e, INVALID_SLOT, data_->pulses[slot]);
                if (!Stacks(s, slot)) {
                    break;
                }
                auto      &tick    = (a.target ? s.target_buff_tick_at : s.self_buff_tick_at)[slot];
                const auto expires = (a.target ? s.target_buff_expires_at : s.self_buff_expires_at)[slot];
                tick = a.scheduled_pulses ? (t.pending_pulses.empty() ? 0 : t.pending_pulses.front()) : s.now + PulseInterval(s, slot);
                if (tick && (data_->dot_effects[slot] == INVALID_SLOT || t.remaining > 0) && (!expires || tick <= expires)) {
                    e.Timers().ArmAt(t.tick, tick);
                } else {
                    tick = 0;
                }
                e.Log().RecordBuff(s.now, a.target, slot, expires, tick, Stacks(s, slot));
                break;
            }
            case TimerKind::SKILL_CASTING:
            {
                const auto casting = pending_;
                if (casting == INVALID_SLOT) {
                    break;
                }
                const auto &a  = *data_->actions[casting];
                last_critical_ = false;
                Apply(e, casting, data_->impacts[casting]);
                ++completed_pulses_;
                if (--pulses_left_ > 0) {
                    if (channel_duration_) {
                        const int  index = completed_pulses_ + 1;
                        const auto elapsed = ChannelOffset(a, index);
                        e.Timers().ArmAt(cast_, channel_start_ + elapsed);
                    } else {
                        e.Timers().Arm(cast_, pulse_spacing_);
                    }
                } else {
                    if (a.finisher != INVALID_SLOT && completed_pulses_ == channel_pulses_) {
                        Strike(e, a.finisher, channel_steps_ + 1);
                    }
                    Apply(e, casting, data_->post_cast[casting]);
                    if (finish_cd_) {
                        StartCooldown(e, casting, finish_cd_);
                    }
                    CastState(e, casting, 0, a.pulses > 1);
                    pending_ = INVALID_SLOT;
                }
                break;
            }
            default: break;
        }
    }
};

class DamageReducer
{
    Preparation               data_;
    const std::vector<Stats> *versions_ = nullptr;

public:
    explicit DamageReducer(Preparation data) : data_(std::move(data)) { }

    void Setup(std::size_t) { }

    void Bind(const Rules &rules) { versions_ = &rules.Versions(); }

    value_t Reduce(const DamageIntent &h) const
    {
        return Damage(data_->config, h, versions_->at(h.snapshot_version), versions_->at(h.live_version));
    }
};

struct Module
{
    using Rules                         = season50::Rules;
    using DamageReducer                 = season50::DamageReducer;
    static constexpr bool replay_strain = true;

    static auto Prepare(const Config &c) { return season50::Prepare(c); }
};
} // namespace JX3DPS::runtime::season50
