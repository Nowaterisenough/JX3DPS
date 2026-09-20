#ifndef JX3DPS_RUNTIME_MACRO_HPP
#define JX3DPS_RUNTIME_MACRO_HPP

#include "../runtime.hpp"
#include <charconv>
#include <cmath>
#include <stdexcept>
#include <type_traits>

namespace JX3DPS::runtime {

using Pc = std::uint32_t;
inline constexpr Pc ACCEPT = std::numeric_limits<Pc>::max();
inline constexpr Pc REJECT = ACCEPT - 1;
inline constexpr Pc LINE_ENTRY = ACCEPT - 2;

enum class Comparator : std::uint8_t { Less, LessEqual, Equal, NotEqual, Greater, GreaterEqual };
enum class ValueKind : std::uint8_t {
    BuffDuration, BuffStacks, SkillCooldown, SkillEnergy, Qidian, Energy, Rage,
    LastSkill, Life, Mana, TargetLife
};
enum class Command : std::uint8_t { Cast, SequentialCast, ForceCast, SequentialForceCast };

struct Condition {
    double value = 0;
    Pc on_true = ACCEPT;
    Pc on_false = REJECT;
    Slot slot = INVALID_SLOT;
    ValueKind kind = ValueKind::Qidian;
    Comparator comparator = Comparator::Equal;
    bool target = false;
};

struct MacroLine {
    Pc condition_begin = 0;
    Pc condition_count = 0;
    Slot action_slot = INVALID_SLOT;
    Command command = Command::Cast;
};
static_assert(std::is_trivially_copyable_v<Condition> && sizeof(Condition) <= 24);
static_assert(std::is_trivially_copyable_v<MacroLine> && sizeof(MacroLine) <= 16);

struct MacroProgram {
    std::vector<MacroLine> lines;
    std::vector<Condition> conditions;
    std::size_t skill_count = 0;
    std::size_t buff_count = 0;
};

struct LineSource {
    std::uint32_t line = 0;
    std::string text;
    std::string action;
    std::vector<std::string> conditions;
};

struct CompiledMacro {
    MacroProgram program;
    // Indexed by instruction, never interleaved with hot instructions.
    std::vector<LineSource> source;
};

struct CompileError {
    std::uint32_t line = 0;
    std::string message;
};

struct CompileOptions {
    int frames_per_second = 16;
    bool keep_debug_info = true;
    std::size_t skill_count = 0;
    std::size_t buff_count = 0;
    std::function<Slot(std::string_view)> resolve_skill;
    std::function<Slot(std::string_view)> resolve_buff;
};

class MacroCompiler {
public:
    static std::pair<CompiledMacro, std::vector<CompileError>> Compile(
        std::string_view text, CompileOptions options = {}) {
        MacroCompiler compiler(std::move(options));
        compiler.CompileText(text);
        // A partial program must never execute after a syntax/name error.
        if (!compiler.errors_.empty()) compiler.result_ = {};
        return {std::move(compiler.result_), std::move(compiler.errors_)};
    }

private:
    explicit MacroCompiler(CompileOptions options) : options_(std::move(options)) {
        result_.program.skill_count = options_.skill_count;
        result_.program.buff_count = options_.buff_count;
    }

    static std::string_view Trim(std::string_view value) {
        while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) value.remove_prefix(1);
        while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) value.remove_suffix(1);
        return value;
    }

    bool Fail(std::string message) {
        errors_.push_back({line_number_, std::move(message)});
        return false;
    }

    Slot Resolve(std::string_view name, bool buff) {
        auto &resolver = buff ? options_.resolve_buff : options_.resolve_skill;
        auto &registry = buff ? buffs_ : skills_;
        auto &count = buff ? result_.program.buff_count : result_.program.skill_count;
        if (name.empty()) { Fail("missing skill/buff name"); return INVALID_SLOT; }
        const Slot slot = resolver ? resolver(name) : registry.Intern(name);
        if (slot == INVALID_SLOT || (resolver && slot >= count)) {
            Fail("unknown or out-of-range name: " + std::string(name));
            return INVALID_SLOT;
        }
        if (!resolver) count = registry.Size();
        return slot;
    }

    bool ParseCondition(std::string_view text, Condition &condition) {
        text = Trim(text);
        if (text.empty() || text.find_first_of("[]()") != text.npos) return Fail("empty or unsupported condition");
        const auto op_start = text.find_first_of("<>=!");
        const auto colon = text.find(':');
        const bool named = colon != text.npos && (op_start == text.npos || colon < op_start);
        const auto type_end = named ? colon : op_start;
        const auto type = Trim(text.substr(0, type_end));
        const auto name = named ? Trim(text.substr(colon + 1, op_start == text.npos ? text.npos : op_start - colon - 1)) : std::string_view{};
        const bool has_comparator = op_start != text.npos;
        double value = 0;
        if (has_comparator) {
            auto number = text.substr(op_start);
            constexpr std::array<std::pair<std::string_view, Comparator>, 7> comparators{{
                {"<=", Comparator::LessEqual}, {">=", Comparator::GreaterEqual}, {"!=", Comparator::NotEqual},
                {"==", Comparator::Equal}, {"<", Comparator::Less}, {">", Comparator::Greater}, {"=", Comparator::Equal}
            }};
            bool matched = false;
            for (auto [symbol, comparator] : comparators) {
                if (number.starts_with(symbol)) {
                    condition.comparator = comparator;
                    number = Trim(number.substr(symbol.size()));
                    matched = true;
                    break;
                }
            }
            if (!matched || number.empty()) return Fail("invalid comparator/value");
            const auto parsed = std::from_chars(number.data(), number.data() + number.size(), value);
            if (parsed.ec != std::errc{} || parsed.ptr != number.data() + number.size() ||
                !std::isfinite(value) || value < 0 || value > std::numeric_limits<tick_t>::max()) {
                return Fail("invalid numeric value");
            }
        }
        condition.value = value;
        bool needs_comparator = false;
        bool needs_integer = false;
        bool time_value = false;
        bool buff_slot = false;
        bool skill_slot = false;
        if (type == "buff" || type == "nobuff" || type == "tbuff" || type == "tnobuff") {
            condition.kind = ValueKind::BuffStacks;
            condition.target = type == "tbuff" || type == "tnobuff";
            const bool absent = type == "nobuff" || type == "tnobuff";
            if (absent && has_comparator) return Fail("nobuff/tnobuff do not accept a comparator");
            if (!has_comparator) {
                condition.value = 0;
                condition.comparator = absent ? Comparator::Equal : Comparator::Greater;
            }
            buff_slot = needs_integer = true;
        } else if (type == "bufftime" || type == "tbufftime") {
            condition.kind = ValueKind::BuffDuration;
            condition.target = type == "tbufftime";
            buff_slot = time_value = needs_comparator = true;
        } else if (type == "cd" || type == "skill_cd" || type == "skill_notin_cd") {
            condition.kind = ValueKind::SkillCooldown;
            skill_slot = time_value = true;
            if (!has_comparator) condition.comparator = type == "skill_notin_cd" ? Comparator::Equal : Comparator::Greater;
        } else if (type == "skill_energy") {
            condition.kind = ValueKind::SkillEnergy;
            skill_slot = needs_comparator = needs_integer = true;
        } else if (type == "last_skill" || type == "notlast_skill") {
            if (has_comparator) return Fail("last_skill expects a skill name without a comparator");
            condition.kind = ValueKind::LastSkill;
            condition.comparator = type == "last_skill" ? Comparator::Equal : Comparator::NotEqual;
            condition.value = 1;
            skill_slot = true;
        } else if (type == "qidian" || type == "energy" || type == "rage") {
            if (named) return Fail("resource condition does not take a name");
            condition.kind = type == "qidian" ? ValueKind::Qidian : type == "energy" ? ValueKind::Energy : ValueKind::Rage;
            needs_comparator = needs_integer = true;
        } else if (type == "life" || type == "mana" || type == "tlife") {
            if (named) return Fail("explicit target ids are not supported by this macro compiler");
            condition.kind = type == "life" ? ValueKind::Life : type == "mana" ? ValueKind::Mana : ValueKind::TargetLife;
            if (value > 1) return Fail("life/mana must be in [0,1]");
            needs_comparator = true;
        } else {
            return Fail("unknown condition: " + std::string(type));
        }
        if (needs_comparator && !has_comparator) return Fail("condition requires a comparator");
        if (needs_integer && value != std::floor(value)) return Fail("stack/resource value must be an integer");
        if (time_value) {
            condition.value = value * options_.frames_per_second;
            if (condition.value > std::numeric_limits<tick_t>::max()) return Fail("time value overflows frame range");
        }
        if (buff_slot || skill_slot) {
            if (!named) return Fail("condition requires a named skill/buff");
            condition.slot = Resolve(name, buff_slot);
            if (condition.slot == INVALID_SLOT) return false;
        }
        return true;
    }

    bool ParseConditions(std::string_view text, MacroLine &line, LineSource &source) {
        auto &conditions = result_.program.conditions;
        line.condition_begin = static_cast<Pc>(conditions.size());
        std::vector<Pc> group_starts{0};
        std::size_t start = 0;
        for (std::size_t i = 0; i <= text.size(); ++i) {
            if (i != text.size() && text[i] != '&' && text[i] != '|') continue;
            Condition condition;
            auto token = Trim(text.substr(start, i - start));
            if (!ParseCondition(token, condition)) return false;
            conditions.push_back(condition);
            if (options_.keep_debug_info) source.conditions.emplace_back(token);
            ++line.condition_count;
            if (i != text.size() && text[i] == '|') group_starts.push_back(line.condition_count);
            start = i + 1;
        }
        // Lower OR-of-AND groups to forward branches. No operators or AST are
        // traversed at runtime; failed AND terms skip the rest of their group.
        for (std::size_t g = 0; g < group_starts.size(); ++g) {
            const Pc end = g + 1 < group_starts.size() ? group_starts[g + 1] : line.condition_count;
            for (Pc i = group_starts[g]; i < end; ++i) {
                auto &c = conditions[line.condition_begin + i];
                c.on_true = i + 1 == end ? ACCEPT : i + 1;
                c.on_false = end == line.condition_count ? REJECT : end;
            }
        }
        return true;
    }

    void CompileText(std::string_view text) {
        if (options_.frames_per_second <= 0 || options_.skill_count > INVALID_SLOT || options_.buff_count > INVALID_SLOT) {
            Fail("invalid frame rate or slot count"); return;
        }
        while (!text.empty()) {
            const auto newline = text.find('\n');
            auto raw = Trim(text.substr(0, newline));
            text = newline == text.npos ? std::string_view{} : text.substr(newline + 1);
            const auto current_line = line_number_++;
            if (raw.empty() || raw.starts_with('#')) continue;
            --line_number_;
            ParseLine(raw, current_line);
            ++line_number_;
        }
    }

    void ParseLine(std::string_view raw, std::uint32_t number) {
        const auto command_end = raw.find_first_of(" \t");
        const auto command = raw.substr(0, command_end);
        MacroLine line;
        LineSource source;
        if (command == "/cast") line.command = Command::Cast;
        else if (command == "/scast") line.command = Command::SequentialCast;
        else if (command == "/fcast") line.command = Command::ForceCast;
        else if (command == "/sfcast") line.command = Command::SequentialForceCast;
        else { Fail("unknown command: " + std::string(command)); return; }
        if (command_end == raw.npos) { Fail("missing action"); return; }
        auto body = Trim(raw.substr(command_end));
        if (body.starts_with('[')) {
            const auto close = body.find(']');
            if (close == body.npos) { Fail("missing closing bracket"); return; }
            if (!ParseConditions(body.substr(1, close - 1), line, source)) return;
            body = Trim(body.substr(close + 1));
        }
        if (body.empty() || body.find_first_of("[]") != body.npos) { Fail("missing or invalid action"); return; }
        line.action_slot = Resolve(body, false);
        if (line.action_slot == INVALID_SLOT) return;
        result_.program.lines.push_back(line);
        if (options_.keep_debug_info) {
            source.line = number;
            source.text = raw;
            source.action = body;
            result_.source.push_back(std::move(source));
        }
    }

    CompileOptions options_;
    CompiledMacro result_;
    NameRegistry skills_, buffs_;
    std::vector<CompileError> errors_;
    std::uint32_t line_number_ = 1;
};

inline bool Compare(double lhs, Comparator op, double rhs) {
    switch (op) {
        case Comparator::Less: return lhs < rhs;
        case Comparator::LessEqual: return lhs <= rhs;
        case Comparator::Equal: return lhs == rhs;
        case Comparator::NotEqual: return lhs != rhs;
        case Comparator::Greater: return lhs > rhs;
        case Comparator::GreaterEqual: return lhs >= rhs;
    }
    return false;
}

inline double ReadValue(const State &state, const Condition &c) {
    switch (c.kind) {
        case ValueKind::Qidian: return state.qidian;
        case ValueKind::Energy: return state.energy;
        case ValueKind::Rage: return state.rage;
        case ValueKind::Life: return state.life;
        case ValueKind::Mana: return state.mana;
        case ValueKind::TargetLife: return state.target_life;
        case ValueKind::LastSkill: return state.last_skill == c.slot;
        case ValueKind::SkillEnergy: return state.skill_energy[c.slot];
        case ValueKind::SkillCooldown: return std::max<tick_t>(0, state.SkillReadyAt(c.slot) - state.now);
        case ValueKind::BuffDuration: {
            const auto &expiry = c.target ? state.target_buff_expires_at : state.self_buff_expires_at;
            return std::max<tick_t>(0, expiry[c.slot] - state.now);
        }
        case ValueKind::BuffStacks: {
            const auto &stacks = c.target ? state.target_buff_stacks : state.self_buff_stacks;
            return stacks[c.slot];
        }
    }
    return 0;
}

inline bool EvaluateCondition(const State &state, const Condition &c) {
    return Compare(ReadValue(state, c), c.comparator, c.value);
}
inline bool IsSequential(Command c) {
    return c == Command::SequentialCast || c == Command::SequentialForceCast;
}
inline bool IsForceCast(Command c) {
    return c == Command::ForceCast || c == Command::SequentialForceCast;
}

enum class DebugPhase : std::uint8_t { Line, Condition, Action, Finished };
struct DebugState {
    DebugPhase phase = DebugPhase::Line;
    Pc instruction = 0;
    Pc condition = LINE_ENTRY;
    Pc last_instruction = ACCEPT;
    Pc last_condition = ACCEPT;
    double condition_value = 0;
    bool condition_result = false;
    bool action_success = false;
    bool paused = false;
    bool budget_exhausted = false;
};
struct NoDebugState {};
struct NoLineHook { void operator()(const MacroLine &)const {} };

enum class ActionBlockReason : std::uint8_t {
    None, Preparing, Channeling, SkillCooldown, GlobalCooldown, Qidian, Charges,
    Distance, MissingTalent, MissingBuff
};
struct ActionDiagnostic {
    ActionBlockReason reason = ActionBlockReason::None;
    double actual = 0, required = 0;
    Slot subject = INVALID_SLOT;
    Slot interrupted = INVALID_SLOT;
};
struct MacroObservation {
    std::uint64_t ordinal = 0;
    tick_t frame = 0;
    Pc instruction = 0, condition = LINE_ENTRY, next = REJECT;
    DebugPhase phase = DebugPhase::Line;
    double actual = 0;
    bool result = false, skipped = false;
    ActionDiagnostic action;
};

// Both specializations execute the same transitions. Only the debug
// specialization retains a resumable cursor and source-level observations.
template <bool Debug, bool TrackLastSkill = true>
class MacroExecutor {
public:
    explicit MacroExecutor(const MacroProgram &program) : program_(program), consumed_(program.lines.size()) {
        if constexpr (Debug) BeginPass();
    }

    void Reset() {
        std::fill(consumed_.begin(), consumed_.end(), 0);
        if constexpr (Debug) BeginPass();
    }

    void BeginPass() requires Debug {
        debug_ = {};
        if (program_.lines.empty()) debug_.phase = DebugPhase::Finished;
    }

    const DebugState &GetDebugState() const requires Debug { return debug_; }
    DebugState &MutableDebugState() requires Debug { return debug_; }
    const std::vector<std::uint8_t> &Consumed() const { return consumed_; }

    template <typename Action, typename BeforeLine = NoLineHook>
    bool EvaluateFast(State &state, Action &&action, BeforeLine before_line = {}) requires (!Debug) {
        for (Pc line_index = 0; line_index < program_.lines.size(); ++line_index) {
            if (consumed_[line_index]) continue;
            const auto &line = program_.lines[line_index];
            before_line(line);
            Pc pc = line.condition_count ? 0 : ACCEPT;
            while (pc < REJECT) {
                const auto &c = program_.conditions[line.condition_begin + pc];
                pc = EvaluateCondition(state, c) ? c.on_true : c.on_false;
            }
            const bool success = pc == ACCEPT && action(state, line);
            if (success) {
                CommitAction(state, line_index);
                return true;
            }
            if (IsSequential(line.command)) return false;
        }
        return false;
    }

    template <typename Action, typename BeforeLine = NoLineHook>
    bool Step(State &state, Action &&action, BeforeLine before_line = {}) requires Debug {
        if (debug_.phase == DebugPhase::Finished) return false;
        const auto index = debug_.instruction;
        const auto &line = program_.lines[index];
        debug_.paused = debug_.budget_exhausted = false;
        debug_.last_instruction = index;
        if (debug_.phase == DebugPhase::Line) {
            if (consumed_[index]) { AdvanceLine(); return true; }
            before_line(line);
            debug_.condition = line.condition_count ? 0 : ACCEPT;
            debug_.phase = line.condition_count ? DebugPhase::Condition : DebugPhase::Action;
        } else if (debug_.phase == DebugPhase::Condition) {
            debug_.last_condition = debug_.condition;
            const auto &c = program_.conditions[line.condition_begin + debug_.condition];
            debug_.condition_value = ReadValue(state, c);
            debug_.condition_result = Compare(debug_.condition_value, c.comparator, c.value);
            debug_.condition = debug_.condition_result ? c.on_true : c.on_false;
            if (debug_.condition == ACCEPT) debug_.phase = DebugPhase::Action;
            else if (debug_.condition == REJECT) {
                if (IsSequential(line.command)) debug_.phase = DebugPhase::Finished;
                else AdvanceLine();
            }
        } else {
            debug_.action_success = action(state, line);
            if (debug_.action_success) CommitAction(state, index);
            if (debug_.action_success || IsSequential(line.command)) debug_.phase = DebugPhase::Finished;
            else AdvanceLine();
        }
        return true;
    }

private:
    void CommitAction(State &state, Pc line_index) {
        const auto &line = program_.lines[line_index];
        if constexpr (TrackLastSkill) state.last_skill = line.action_slot;
        if (IsSequential(line.command)) consumed_[line_index] = 1;
    }
    void AdvanceLine() requires Debug {
        ++debug_.instruction;
        debug_.condition = LINE_ENTRY;
        debug_.phase = debug_.instruction == program_.lines.size() ? DebugPhase::Finished : DebugPhase::Line;
    }
    const MacroProgram &program_;
    std::vector<std::uint8_t> consumed_;
    [[no_unique_address]] std::conditional_t<Debug, DebugState, NoDebugState> debug_;
};

using FastMacroExecutor = MacroExecutor<false>;
using DebugMacroExecutor = MacroExecutor<true>;

class Debugger {
public:
    using ActionCallback = bool (*)(void *, State &, const MacroLine &);
    explicit Debugger(CompiledMacro macro)
        : macro_(std::move(macro)), executor_(macro_.program), breakpoints_(macro_.program.lines.size()) {
        if (macro_.source.size() != macro_.program.lines.size()) throw std::invalid_argument("debug source map is required");
        state_.Resize(macro_.program.skill_count, macro_.program.buff_count);
        executor_.BeginPass();
    }
    Debugger(const Debugger &) = delete;
    Debugger &operator=(const Debugger &) = delete;
    Debugger(Debugger &&) = delete;
    Debugger &operator=(Debugger &&) = delete;

    void SetActionCallback(ActionCallback action, void *user = nullptr) { action_ = action; user_ = user; }
    State &MutableState() { return state_; }
    const State &GetState() const { return state_; }
    const DebugState &GetDebugState() const { return executor_.GetDebugState(); }
    const CompiledMacro &Program() const { return macro_; }
    void Reset() { state_.Reset(); executor_.Reset(); }
    void BeginPass() { executor_.BeginPass(); }

    bool SetBreakpoint(std::uint32_t line, bool enabled = true) {
        for (std::size_t i = 0; i < macro_.source.size(); ++i) {
            if (macro_.source[i].line == line) { breakpoints_[i] = enabled; return true; }
        }
        return false;
    }
    void SetBreakpointAction(std::string_view action, bool enabled = true) {
        for (std::size_t i = 0; i < macro_.source.size(); ++i) {
            if (macro_.source[i].action == action) breakpoints_[i] = enabled;
        }
    }
    bool StepInto() {
        if (!action_) throw std::logic_error("a simulator action callback is required");
        return executor_.Step(state_, [this](State &state, const MacroLine &line) { return action_(user_, state, line); });
    }
    bool StepOver() {
        const auto index = GetDebugState().instruction;
        if (!StepInto()) return false;
        while (GetDebugState().phase != DebugPhase::Finished && GetDebugState().instruction == index) StepInto();
        return true;
    }
    bool Continue(std::size_t max_steps = 100000) {
        auto &debug = executor_.MutableDebugState();
        bool skip = debug.paused;
        debug.paused = debug.budget_exhausted = false;
        for (std::size_t i = 0; i < max_steps && debug.phase != DebugPhase::Finished; ++i) {
            if (!skip && debug.phase == DebugPhase::Line && breakpoints_[debug.instruction] &&
                !executor_.Consumed()[debug.instruction]) {
                debug.paused = true;
                return true;
            }
            skip = false;
            StepInto();
        }
        debug.budget_exhausted = debug.phase != DebugPhase::Finished;
        return debug.budget_exhausted;
    }

private:
    CompiledMacro macro_;
    State state_;
    DebugMacroExecutor executor_;
    std::vector<std::uint8_t> breakpoints_;
    ActionCallback action_ = nullptr;
    void *user_ = nullptr;
};

} // namespace JX3DPS::runtime
#endif
