#ifndef JX3DPS_RUNTIME_SIMULATION_HPP
#define JX3DPS_RUNTIME_SIMULATION_HPP

#include "../runtime.hpp"

namespace JX3DPS::runtime {

// Compute the first integer frame at which a time comparison can change.
// This is necessary even when there is no expiry/tick event at that frame.
inline std::optional<tick_t> NextMacroBoundary(const MacroProgram &program, const State &state) {
    std::optional<tick_t> result;
    auto consider = [&](std::int64_t frame) {
        if (frame > state.now && frame <= std::numeric_limits<tick_t>::max() && (!result || frame < *result)) {
            result = static_cast<tick_t>(frame);
        }
    };
    for (const auto &c : program.conditions) {
        tick_t deadline;
        if (c.kind == ValueKind::SkillCooldown) deadline = state.SkillReadyAt(c.slot);
        else if (c.kind == ValueKind::BuffDuration) {
            const auto &expiry = c.target ? state.target_buff_expires_at : state.self_buff_expires_at;
            deadline = expiry[c.slot];
        } else continue;
        if (deadline <= state.now) continue;
        const auto floor = static_cast<std::int64_t>(std::floor(c.value));
        const auto ceil = static_cast<std::int64_t>(std::ceil(c.value));
        switch (c.comparator) {
            case Comparator::Greater:
            case Comparator::LessEqual: consider(deadline - floor); break;
            case Comparator::GreaterEqual:
            case Comparator::Less:
                if (c.value > 0) consider(deadline - ceil + 1);
                break;
            case Comparator::Equal:
            case Comparator::NotEqual:
                if (floor == ceil) {
                    consider(deadline - floor);
                    if (c.value > 0) consider(deadline - floor + 1);
                }
                break;
        }
    }
    return result;
}

struct SimulationDebugControl {
    std::vector<std::uint8_t> breakpoints;
    void (*observer)(void *, const MacroObservation &) = nullptr;
    void *observer_context = nullptr;
    std::uint64_t ordinal = 0;
    ActionDiagnostic action;
    bool paused = false;
    bool budget_exhausted = false;
    bool new_pass = false;
};

// Rules is selected when constructing the simulator. Setup creates timer
// handles once; Reset initializes a fight; Cast/OnTimer/Reduce are statically
// dispatched. Rules must immediately apply procs which affect later actions.
template <typename Rules, bool Debug = false>
class Simulation {
public:
    Simulation(const MacroProgram &program, Rules rules = {}, std::size_t damage_capacity = 4096,
               std::size_t mutation_capacity = 8192)
        : program_(program), rules_(std::move(rules)), macro_(program), log_(damage_capacity, mutation_capacity) {
        state_.Resize(program.skill_count, program.buff_count);
        initial_ = scratch_ = state_;
        if constexpr (Debug) debug_.breakpoints.resize(program.lines.size());
        rules_.Setup(*this);
    }
    Simulation(const Simulation &) = delete;
    Simulation &operator=(const Simulation &) = delete;
    Simulation(Simulation &&) = delete;
    Simulation &operator=(Simulation &&) = delete;

    void Start(tick_t duration, std::uint64_t seed) {
        if (duration < 0) throw std::invalid_argument("negative fight duration");
        end_ = duration;
        finished_ = duration == 0;
        state_.Reset(); queue_.Reset(); log_.Clear(); macro_.Reset();
        if constexpr (Debug) {
            debug_.paused = debug_.budget_exhausted = debug_.new_pass = false;
            debug_.ordinal = 0;
            debug_.action = {};
        }
        rules_.Reset(*this, seed);
        initial_ = state_;
        log_.Clear();
        dispatch_ = true;
        actions_at_frame_ = 0;
    }
    void Run() requires (!Debug) {
        while (!finished_) Pump(false);
    }
    bool StepInto() requires Debug {
        debug_.paused = debug_.budget_exhausted = false;
        if (finished_) return false;
        Pump(false);
        return true;
    }
    bool StepOver() requires Debug {
        if (finished_) return false;
        const auto frame = state_.now;
        const auto instruction = macro_.GetDebugState().instruction;
        do {
            StepInto();
        } while (!finished_ && state_.now == frame && macro_.GetDebugState().instruction == instruction && !debug_.new_pass);
        return true;
    }
    bool Continue(std::size_t max_steps = 100000) requires Debug {
        const bool resuming = debug_.paused;
        debug_.paused = debug_.budget_exhausted = false;
        for (std::size_t i = 0; i < max_steps && !finished_; ++i) {
            Pump(!(i == 0 && resuming));
            if (debug_.paused) return true;
        }
        debug_.budget_exhausted = !finished_;
        return !finished_;
    }
    void SetBreakpoint(Pc instruction, bool enabled = true) requires Debug {
        debug_.breakpoints.at(instruction) = enabled;
    }
    static constexpr bool debug_enabled = Debug;
    void SetDebugObserver(void (*observer)(void *, const MacroObservation &), void *context) requires Debug {
        debug_.observer = observer;
        debug_.observer_context = context;
    }
    void RejectAction(ActionBlockReason reason, double actual = 0, double required = 0,
                      Slot subject = INVALID_SLOT) requires Debug {
        debug_.action.reason = reason;
        debug_.action.actual = actual;
        debug_.action.required = required;
        debug_.action.subject = subject;
    }
    void NoteChannelInterrupt(Slot skill) requires Debug { debug_.action.interrupted = skill; }
    const SimulationDebugControl &DebugControl() const requires Debug { return debug_; }
    const DebugState &MacroDebugState() const requires Debug { return macro_.GetDebugState(); }
    bool Finished() const { return finished_; }
    State &MutableState() { return state_; }
    const State &GetState() const { return state_; }
    const Rules &GetRules() const { return rules_; }
    DeadlineQueue &Timers() { return queue_; }
    DamageIntentLog &Log() { return log_; }
    const DamageIntentLog &Log() const { return log_; }
    value_t TotalDamage() {
        if (!finished_) throw std::logic_error("fight must finish before deferred reduction");
        if constexpr (requires(const DamageIntent &hit) { rules_.Reduce(hit); }) {
            value_t total = 0;
            for (const auto &hit : log_.Intents()) total += rules_.Reduce(hit);
            return total;
        } else {
            return log_.Reduce(initial_, scratch_, [this](const DamageIntent &hit, const State &state) {
                return rules_.Reduce(hit, state);
            });
        }
    }

private:
    void Pump(bool honor_breakpoints) {
        if constexpr (Debug) debug_.new_pass = false;
        if (dispatch_) {
            queue_.DispatchDue([this](auto handle, auto id) { rules_.OnTimer(*this, handle, id); });
            dispatch_ = false;
        }
        auto cast = [this](State &, const MacroLine &line) { return rules_.Cast(*this, line); };
        auto before_line = [this](const MacroLine &line) {
            if constexpr (requires { rules_.BeforeMacroLine(*this, line); }) rules_.BeforeMacroLine(*this, line);
            else (void)this;
        };
        bool success;
        if constexpr (Debug) {
            const auto &cursor = macro_.GetDebugState();
            if (honor_breakpoints && cursor.phase == DebugPhase::Line &&
                debug_.breakpoints[cursor.instruction] && !macro_.Consumed()[cursor.instruction]) {
                debug_.paused = true;
                return;
            }
            const auto before = cursor;
            const bool skipped = before.phase == DebugPhase::Line && macro_.Consumed()[before.instruction];
            debug_.action = {};
            macro_.Step(state_, cast, before_line);
            // Publish before BeginPass resets the executed condition/action.
            if (before.phase == DebugPhase::Condition || before.phase == DebugPhase::Action || skipped ||
                debug_.action.interrupted != INVALID_SLOT) {
                MacroObservation observation;
                observation.ordinal = ++debug_.ordinal;
                observation.frame = state_.now;
                observation.instruction = before.instruction;
                observation.condition = before.condition;
                observation.phase = before.phase;
                observation.skipped = skipped;
                observation.action = debug_.action;
                if (before.phase == DebugPhase::Condition) {
                    observation.actual = cursor.condition_value;
                    observation.result = cursor.condition_result;
                    const auto &line = program_.lines[before.instruction];
                    const auto &condition = program_.conditions[line.condition_begin + before.condition];
                    observation.next = observation.result ? condition.on_true : condition.on_false;
                } else observation.result = cursor.action_success;
                if (debug_.observer) debug_.observer(debug_.observer_context, observation);
            }
            if (cursor.phase != DebugPhase::Finished) return;
            success = cursor.action_success;
            macro_.BeginPass();
        } else {
            (void)honor_breakpoints;
            success = macro_.EvaluateFast(state_, cast, before_line);
        }
        if constexpr (Debug) debug_.new_pass = true;
        if (success) {
            if (++actions_at_frame_ > 10000) throw std::runtime_error("same-frame action budget exceeded");
            dispatch_ = true;
            return;
        }
        auto next = queue_.NextDelta();
        if (auto boundary = NextMacroBoundary(program_, state_); boundary && (!next || *boundary - state_.now < *next)) {
            next = *boundary - state_.now;
        }
        if (!next || *next >= end_ - state_.now) {
            state_.now = end_;
            finished_ = true;
            return;
        }
        queue_.Advance(*next);
        state_.now = queue_.Now();
        dispatch_ = true;
        actions_at_frame_ = 0;
    }

    const MacroProgram &program_;
    Rules rules_;
    static constexpr bool TrackLastSkill = [] {
        if constexpr (requires { Rules::owns_last_skill; }) return !Rules::owns_last_skill;
        else return true;
    }();
    MacroExecutor<Debug, TrackLastSkill> macro_;
    State state_, initial_, scratch_;
    DeadlineQueue queue_;
    DamageIntentLog log_;
    tick_t end_ = 0;
    std::size_t actions_at_frame_ = 0;
    bool dispatch_ = true, finished_ = true;
    [[no_unique_address]] std::conditional_t<Debug, SimulationDebugControl, NoDebugState> debug_;
};

} // namespace JX3DPS::runtime
#endif
