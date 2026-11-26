#ifndef JX3DPS_RUNTIME_SIMULATION_HPP
#define JX3DPS_RUNTIME_SIMULATION_HPP

#include "../runtime.hpp"

namespace JX3DPS::runtime {

// A time comparison can flip between timer events. Store the distance from its
// deadline to each transition, so a wake only needs the current absolute deadline.
class MacroTimeBoundaries
{
    struct Boundary
    {
        tick_t offset;
        Slot slot;
        ValueKind kind;
        bool target;
    };
    std::vector<Boundary> entries_;

public:
    explicit MacroTimeBoundaries(const MacroProgram &program)
    {
        for (const auto &c : program.conditions) {
            if (c.kind != ValueKind::SKILL_COOLDOWN && c.kind != ValueKind::BUFF_DURATION) {
                continue;
            }
            const auto add = [&](tick_t offset) { entries_.push_back({ offset, c.slot, c.kind, c.target }); };
            const auto floor = static_cast<tick_t>(std::floor(c.value));
            switch (c.comparator) {
                case Comparator::GREATER:
                case Comparator::LESS_EQUAL: add(floor); break;
                case Comparator::GREATER_EQUAL:
                case Comparator::LESS:
                    if (c.value > 0) add(static_cast<tick_t>(std::ceil(c.value)) - 1);
                    break;
                case Comparator::EQUAL:
                case Comparator::NOT_EQUAL:
                    if (c.value == floor) {
                        add(floor);
                        if (floor > 0) add(floor - 1);
                    }
                    break;
            }
        }
    }

    bool Empty() const { return entries_.empty(); }

    std::optional<tick_t> Next(const State &state) const
    {
        std::optional<tick_t> next;
        for (const auto &entry : entries_) {
            const auto deadline = entry.kind == ValueKind::SKILL_COOLDOWN ? state.SkillReadyAt(entry.slot)
                                : (entry.target ? state.target_buff_expires_at : state.self_buff_expires_at)[entry.slot];
            if (deadline > state.now) {
                const auto frame = static_cast<std::int64_t>(deadline) - entry.offset;
                if (frame > state.now && frame <= std::numeric_limits<tick_t>::max() && (!next || frame < *next)) {
                    next = static_cast<tick_t>(frame);
                }
            }
        }
        return next;
    }
};

inline std::optional<tick_t> NextMacroBoundary(const MacroProgram &program, const State &state)
{
    return MacroTimeBoundaries(program).Next(state);
}

struct SimulationDebugControl
{
    std::vector<std::uint8_t> breakpoints;
    void (*observer)(void *, const MacroObservation &) = nullptr;
    void            *observer_context                  = nullptr;
    std::uint64_t    ordinal                           = 0;
    ActionDiagnostic action;
    bool             paused           = false;
    bool             budget_exhausted = false;
    bool             new_pass         = false;
};

// Rules is selected when constructing the simulator. Setup creates timer
// handles once; Reset initializes a fight; Cast/OnTimer/Reduce are statically
// dispatched. Rules must immediately apply procs which affect later actions.
template <typename Rules, bool Debug = false, bool Measure = false>
class Simulation
{
    static constexpr bool FilterPrimaryGcd = !Debug && requires(Slot skill) {
        Rules::UsesPrimaryGcd(skill);
    } && !requires(Rules &rules, Simulation &sim, const MacroLine &line) { rules.BeforeMacroLine(sim, line); };

    struct PrimaryGcdScan
    {
        std::vector<Slot> skills;
        std::vector<Pc>   remaining_lines;

        bool Applies(const State &state) const
        {
            // The mapping in State is mutable. Use the full macro if a skill
            // has moved to another cooldown group.
            return !skills.empty() && state.gcd_ready_at[0] > state.now &&
                   std::ranges::all_of(skills, [&](Slot s) { return state.skill_gcd_group[s] == 0; });
        }
    };

    struct NoGcdScan
    {
    };

public:
    Simulation(const MacroProgram &program, Rules rules = Rules{}, std::size_t damage_capacity = 4096, std::size_t mutation_capacity = 8192) :
        program_(program), time_boundaries_(program), rules_(std::move(rules)), macro_(program), log_(damage_capacity, mutation_capacity)
    {
        state_.Resize(program.skill_count, program.buff_count);
        initial_ = scratch_ = state_;
        if constexpr (!Debug && requires(const MacroLine &line) { Rules::MayActWhileBusy(line); }) {
            scan_while_busy_ = std::any_of(program.lines.begin(), program.lines.end(), Rules::MayActWhileBusy);
        }
        if constexpr (Debug) {
            debug_.breakpoints.resize(program.lines.size());
        }
        rules_.Setup(*this);
        if constexpr (FilterPrimaryGcd) {
            // Sequential and force commands can block or interrupt before a cast.
            if (std::ranges::all_of(program.lines, [](const MacroLine &l) { return l.command == Command::CAST; })) {
                for (Pc i = 0; i < program.lines.size(); ++i) {
                    const auto SKILL = program.lines[i].action_slot;
                    if (!Rules::UsesPrimaryGcd(SKILL)) {
                        gcd_scan_.remaining_lines.push_back(i);
                    } else if (std::ranges::find(gcd_scan_.skills, SKILL) == gcd_scan_.skills.end()) {
                        gcd_scan_.skills.push_back(SKILL);
                    }
                }
            }
        }
    }

    Simulation(const Simulation &)            = delete;
    Simulation &operator=(const Simulation &) = delete;
    Simulation(Simulation &&)                 = delete;
    Simulation &operator=(Simulation &&)      = delete;

    void Start(tick_t duration, std::uint64_t seed)
    {
        const PhaseScope<Measure> FRAME("Simulation::Start");
        if (duration < 0) {
            throw std::invalid_argument("negative fight duration");
        }
        end_      = duration;
        finished_ = duration == 0;
        state_.Reset();
        queue_.Reset();
        log_.Clear();
        macro_.Reset();
        if constexpr (Debug) {
            debug_.paused = debug_.budget_exhausted = debug_.new_pass = false;
            debug_.ordinal                                            = 0;
            debug_.action                                             = {};
        }
        rules_.Reset(*this, seed);
        initial_ = state_;
        log_.Clear();
        dispatch_         = true;
        actions_at_frame_ = 0;
    }

    void Run()
        requires (!Debug)
    {
        const PhaseScope<Measure> FRAME("Simulation::Run");
        while (!finished_) {
            Pump(false);
        }
    }

    bool StepInto()
        requires Debug
    {
        debug_.paused = debug_.budget_exhausted = false;
        if (finished_) {
            return false;
        }
        Pump(false);
        return true;
    }

    bool StepOver()
        requires Debug
    {
        if (finished_) {
            return false;
        }
        const auto FRAME       = state_.now;
        const auto INSTRUCTION = macro_.GetDebugState().instruction;
        do {
            StepInto();
        } while (!finished_ && state_.now == FRAME && macro_.GetDebugState().instruction == INSTRUCTION && !debug_.new_pass);
        return true;
    }

    bool Continue(std::size_t max_steps = 100000)
        requires Debug
    {
        const bool RESUMING = debug_.paused;
        debug_.paused = debug_.budget_exhausted = false;
        for (std::size_t i = 0; i < max_steps && !finished_; ++i) {
            Pump(i != 0 || !RESUMING);
            if (debug_.paused) {
                return true;
            }
        }
        debug_.budget_exhausted = !finished_;
        return !finished_;
    }

    void SetBreakpoint(Pc instruction, bool enabled = true)
        requires Debug
    {
        debug_.breakpoints.at(instruction) = enabled;
    }

    using CastEffectScope               = PhaseScope<Measure>;
    static constexpr bool debug_enabled = Debug;

    void SetDebugObserver(void (*observer)(void *, const MacroObservation &), void *context)
        requires Debug
    {
        debug_.observer         = observer;
        debug_.observer_context = context;
    }

    void RejectAction(ActionBlockReason reason, double actual = 0, double required = 0, Slot subject = INVALID_SLOT)
        requires Debug
    {
        debug_.action.reason   = reason;
        debug_.action.actual   = actual;
        debug_.action.required = required;
        debug_.action.subject  = subject;
    }

    void NoteChannelInterrupt(Slot skill)
        requires Debug
    {
        debug_.action.interrupted = skill;
    }

    const SimulationDebugControl &DebugControl() const
        requires Debug
    {
        return debug_;
    }

    const DebugState &MacroDebugState() const
        requires Debug
    {
        return macro_.GetDebugState();
    }

    bool Finished() const { return finished_; }

    State &MutableState() { return state_; }

    const State &GetState() const { return state_; }
    const State &StartingState() const { return initial_; }

    const Rules &GetRules() const { return rules_; }

    auto &Timers() { return queue_; }

    auto &Log() { return log_; }

    const auto &Log() const { return log_; }

    value_t TotalDamage()
    {
        return TotalDamage([](const DamageIntent &, value_t) { });
    }

    template <class Observer>
    value_t TotalDamage(Observer observer)
    {
        const PhaseScope<Measure> TIMER(FightPhase::DAMAGE, "Simulation::TotalDamage");
        if (!finished_) {
            throw std::logic_error("fight must finish before deferred reduction");
        }
        if constexpr (requires(const DamageIntent &hit) { rules_.Reduce(hit); }) {
            value_t total = 0;
            for (const auto &hit : log_.Intents()) {
                const auto DAMAGE = [&] {
                    const PhaseScope<Measure> FRAME("Rules::Reduce");
                    return rules_.Reduce(hit);
                }();
                observer(hit, DAMAGE);
                total += DAMAGE;
            }
            return total;
        } else {
            return log_.Reduce(initial_, scratch_, [this, &observer](const DamageIntent &hit, const State &state) {
                const auto DAMAGE = rules_.Reduce(hit, state);
                observer(hit, DAMAGE);
                return DAMAGE;
            });
        }
    }

private:
    void Pump(bool honor_breakpoints)
    {
        const PhaseScope<Measure> FRAME("Simulation::Pump");
        if constexpr (Debug) {
            debug_.new_pass = false;
        }
        if (dispatch_) {
            const PhaseScope<Measure> TIMER(FightPhase::QUEUE);
            queue_.DispatchDue([this](auto handle, auto id) {
                const PhaseScope<Measure> effect(FightPhase::TIMER_EFFECT);
                rules_.OnTimer(*this, handle, id);
            });
            dispatch_ = false;
        }
        const auto cast = [this](State &, const MacroLine &line) {
            const PhaseScope<Measure> TIMER(FightPhase::AVAILABILITY);
            return rules_.Cast(*this, line);
        };
        const auto BEFORE_LINE = [this](const MacroLine &line) {
            if constexpr (requires { rules_.BeforeMacroLine(*this, line); }) {
                rules_.BeforeMacroLine(*this, line);
            } else {
                (void)this;
            }
        };
        bool success = false;
        if constexpr (Debug) {
            const auto &cursor = macro_.GetDebugState();
            if (honor_breakpoints && cursor.phase == DebugPhase::LINE && debug_.breakpoints[cursor.instruction] &&
                !macro_.Consumed()[cursor.instruction])
            {
                debug_.paused = true;
                return;
            }
            const auto BEFORE  = cursor;
            const bool SKIPPED = BEFORE.phase == DebugPhase::LINE && macro_.Consumed()[BEFORE.instruction];
            debug_.action      = {};
            macro_.Step(state_, cast, BEFORE_LINE);
            // Publish before BeginPass resets the executed condition/action.
            if (BEFORE.phase == DebugPhase::CONDITION || BEFORE.phase == DebugPhase::ACTION || SKIPPED ||
                debug_.action.interrupted != INVALID_SLOT)
            {
                MacroObservation observation;
                observation.ordinal     = ++debug_.ordinal;
                observation.frame       = state_.now;
                observation.instruction = BEFORE.instruction;
                observation.condition   = BEFORE.condition;
                observation.phase       = BEFORE.phase;
                observation.skipped     = SKIPPED;
                observation.action      = debug_.action;
                if (BEFORE.phase == DebugPhase::CONDITION) {
                    observation.actual    = cursor.condition_value;
                    observation.result    = cursor.condition_result;
                    const auto &line      = program_.lines[BEFORE.instruction];
                    const auto &condition = program_.conditions[line.condition_begin + BEFORE.condition];
                    observation.next      = observation.result ? condition.on_true : condition.on_false;
                } else {
                    observation.result = cursor.action_success;
                }
                if (debug_.observer) {
                    debug_.observer(debug_.observer_context, observation);
                }
            }
            if (cursor.phase != DebugPhase::FINISHED) {
                return;
            }
            success = cursor.action_success;
            macro_.BeginPass();
        } else {
            (void)honor_breakpoints;
            const PhaseScope<Measure> TIMER(FightPhase::MACRO);
            const auto                AVAILABLE = [this](const MacroLine &line) {
                const PhaseScope<Measure> CHECK(FightPhase::AVAILABILITY, "Rules::CanAttempt");
                if constexpr (requires { rules_.CanAttempt(state_, line); }) {
                    return rules_.CanAttempt(state_, line);
                } else {
                    return true;
                }
            };
            const bool SCAN = scan_while_busy_ || (state_.preparing_skill == INVALID_SLOT && state_.casting_skill == INVALID_SLOT);
            if (!SCAN) {
                success = false;
            } else if constexpr (FilterPrimaryGcd) {
                success = gcd_scan_.Applies(state_)
                              ? macro_.EvaluateFastLines(state_, cast, BEFORE_LINE, AVAILABLE, gcd_scan_.remaining_lines)
                              : macro_.EvaluateFast(state_, cast, BEFORE_LINE, AVAILABLE);
            } else {
                success = macro_.EvaluateFast(state_, cast, BEFORE_LINE, AVAILABLE);
            }
        }
        if constexpr (Debug) {
            debug_.new_pass = true;
        }
        if (success) {
            if (++actions_at_frame_ > 10000) {
                throw std::runtime_error("same-frame action budget exceeded");
            }
            dispatch_ = true;
            return;
        }
        const PhaseScope<Measure> ADVANCE(FightPhase::ADVANCE);
        auto                      next = queue_.NextDelta();
        if (has_time_boundaries_) {
            if (auto boundary = time_boundaries_.Next(state_); boundary && (!next || *boundary - state_.now < *next)) {
                next = *boundary - state_.now;
            }
        }
        if (!next || *next >= end_ - state_.now) {
            state_.now = end_;
            finished_  = true;
            return;
        }
        queue_.Advance(*next);
        state_.now        = queue_.Now();
        dispatch_         = true;
        actions_at_frame_ = 0;
    }

    const MacroProgram   &program_;
    MacroTimeBoundaries   time_boundaries_;
    Rules                 rules_;
    static constexpr bool TrackLastSkill = [] {
        if constexpr (requires { Rules::owns_last_skill; }) {
            return !Rules::owns_last_skill;
        } else {
            return true;
        }
    }();
    MacroExecutor<Debug, TrackLastSkill, Measure> macro_;
    State                                         state_, initial_, scratch_;
    BasicDeadlineQueue<Measure>                   queue_;
    BasicDamageIntentLog<Measure>                 log_;
    tick_t                                        end_              = 0;
    std::size_t                                   actions_at_frame_ = 0;
    bool                                          dispatch_ = true, finished_ = true;
    const bool                                    has_time_boundaries_ = !time_boundaries_.Empty();
    bool                                          scan_while_busy_ = true;
    [[no_unique_address]] std::conditional_t<FilterPrimaryGcd, PrimaryGcdScan, NoGcdScan> gcd_scan_;
    [[no_unique_address]] std::conditional_t<Debug, SimulationDebugControl, NoDebugState> debug_;
};

} // namespace JX3DPS::runtime
#endif
