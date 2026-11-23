#ifndef JX3DPS_RUNTIME_MACRO_HPP
#define JX3DPS_RUNTIME_MACRO_HPP

#include <charconv>
#include <cmath>
#include <ranges>
#include <stdexcept>
#include <type_traits>

#include "../runtime.hpp"

namespace JX3DPS::runtime {

using Pc                       = std::uint32_t;
inline constexpr Pc ACCEPT     = std::numeric_limits<Pc>::max();
inline constexpr Pc REJECT     = ACCEPT - 1;
inline constexpr Pc LINE_ENTRY = ACCEPT - 2;

enum class Comparator : std::uint8_t
{
    LESS,
    LESS_EQUAL,
    EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL
};
enum class ValueKind : std::uint8_t
{
    BUFF_DURATION,
    BUFF_STACKS,
    SKILL_COOLDOWN,
    SKILL_ENERGY,
    QIDIAN,
    ENERGY,
    RAGE,
    LAST_SKILL,
    LIFE,
    MANA,
    TARGET_LIFE
};
enum class Command : std::uint8_t
{
    CAST,
    SEQUENTIAL_CAST,
    FORCE_CAST,
    SEQUENTIAL_FORCE_CAST
};

struct Condition
{
    double     value      = 0;
    Pc         on_true    = ACCEPT;
    Pc         on_false   = REJECT;
    Slot       slot       = INVALID_SLOT;
    ValueKind  kind       = ValueKind::QIDIAN;
    Comparator comparator = Comparator::EQUAL;
    bool       target     = false;
};

struct MacroLine
{
    Pc      condition_begin = 0;
    Pc      condition_count = 0;
    Slot    action_slot     = INVALID_SLOT;
    Command command         = Command::CAST;
};

static_assert(std::is_trivially_copyable_v<Condition> && sizeof(Condition) <= 24);
static_assert(std::is_trivially_copyable_v<MacroLine> && sizeof(MacroLine) <= 16);

struct MacroProgram
{
    std::vector<MacroLine> lines;
    std::vector<Condition> conditions;
    std::size_t            skill_count = 0;
    std::size_t            buff_count  = 0;
    std::string            source_text; // Original UTF-8 input for engine session requests.
};

struct LineSource
{
    std::uint32_t            line = 0;
    std::string              text;
    std::string              action;
    std::vector<std::string> conditions;
};

struct CompiledMacro
{
    MacroProgram            program;
    // Indexed by instruction, never interleaved with hot instructions.
    std::vector<LineSource> source;
};

struct CompileError
{
    std::uint32_t line = 0;
    std::string   message;
};

struct CompileOptions
{
    int                                   frames_per_second = FRAMES_PER_SECOND;
    bool                                  keep_debug_info   = true;
    std::size_t                           skill_count       = 0;
    std::size_t                           buff_count        = 0;
    std::function<Slot(std::string_view)> resolve_skill;
    std::function<Slot(std::string_view)> resolve_buff;

    struct ResourceName
    {
        std::string_view name{};
        bool             signed_value = false;
    };

    std::array<ResourceName, 3> resources{};
};

class MacroCompiler
{
public:
    static std::pair<CompiledMacro, std::vector<CompileError>> Compile(std::string_view text, CompileOptions options = {})
    {
        MacroCompiler compiler(std::move(options));
        compiler.CompileText(text);
        compiler.result_.program.source_text = text;
        // A partial program must never execute after a syntax/name error.
        if (!compiler.errors_.empty()) {
            compiler.result_ = {};
        }
        return { std::move(compiler.result_), std::move(compiler.errors_) };
    }

private:
    explicit MacroCompiler(CompileOptions options) : options_(std::move(options))
    {
        result_.program.skill_count = options_.skill_count;
        result_.program.buff_count  = options_.buff_count;
    }

    static std::string_view Trim(std::string_view value)
    {
        // Macro separators are ASCII. A locale's whitespace bytes can occur inside UTF-8 skill names.
        constexpr std::string_view whitespace = " \t\n\r\f\v";
        const auto first = value.find_first_not_of(whitespace);
        if (first == std::string_view::npos) {
            return {};
        }
        return value.substr(first, value.find_last_not_of(whitespace) - first + 1);
    }

    bool Fail(std::string message)
    {
        errors_.push_back({ line_number_, std::move(message) });
        return false;
    }

    Slot Resolve(std::string_view name, bool buff)
    {
        auto &resolver = buff ? options_.resolve_buff : options_.resolve_skill;
        auto &registry = buff ? buffs_ : skills_;
        auto &count    = buff ? result_.program.buff_count : result_.program.skill_count;
        if (name.empty()) {
            Fail("missing skill/buff name");
            return INVALID_SLOT;
        }
        const Slot slot = resolver ? resolver(name) : registry.Intern(name);
        if (slot == INVALID_SLOT || (resolver && slot >= count)) {
            Fail("unknown or out-of-range name: " + std::string(name));
            return INVALID_SLOT;
        }
        if (!resolver) {
            count = registry.Size();
        }
        return slot;
    }

    bool ParseCondition(std::string_view text, Condition &condition)
    {
        text = Trim(text);
        if (text.empty() || text.find_first_of("[]()") != std::string_view::npos) {
            return Fail("empty or unsupported condition");
        }
        const auto op_start = text.find_first_of("<>=!");
        const auto COLON    = text.find(':');
        const bool NAMED = COLON != std::string_view::npos && (op_start == std::string_view::npos || COLON < op_start);
        const auto TYPE_END = NAMED ? COLON : op_start;
        auto       type     = Trim(text.substr(0, TYPE_END));
        constexpr std::array<std::string_view, 3> resource_fields{ "qidian", "energy", "rage" };
        bool                                      signed_resource = false;
        for (std::size_t i = 0; i < resource_fields.size(); ++i) {
            const auto &alias = options_.resources[i];
            if (!alias.name.empty() && (type == alias.name || type == resource_fields[i])) {
                type            = resource_fields[i];
                signed_resource = alias.signed_value;
                break;
            }
        }
        const auto NAME =
            NAMED ? Trim(text.substr(COLON + 1, op_start == std::string_view::npos ? std::string_view::npos : op_start - COLON - 1))
                  : std::string_view{};
        const bool HAS_COMPARATOR = op_start != std::string_view::npos;
        double     value          = 0;
        if (HAS_COMPARATOR) {
            auto                                                             number = text.substr(op_start);
            constexpr std::array<std::pair<std::string_view, Comparator>, 7> COMPARATORS{
                { { "<=", Comparator::LESS_EQUAL },
                 { ">=", Comparator::GREATER_EQUAL },
                 { "!=", Comparator::NOT_EQUAL },
                 { "==", Comparator::EQUAL },
                 { "<", Comparator::LESS },
                 { ">", Comparator::GREATER },
                 { "=", Comparator::EQUAL } }
            };
            bool matched = false;
            for (auto [symbol, comparator] : COMPARATORS) {
                if (number.starts_with(symbol)) {
                    condition.comparator = comparator;
                    number               = Trim(number.substr(symbol.size()));
                    matched              = true;
                    break;
                }
            }
            if (!matched || number.empty()) {
                return Fail("invalid comparator/value");
            }
            const auto PARSED = std::from_chars(number.data(), number.data() + number.size(), value);
            if (PARSED.ec != std::errc{} || PARSED.ptr != number.data() + number.size() || !std::isfinite(value) ||
                (value < 0 && !signed_resource) || value < std::numeric_limits<int>::min() ||
                value > std::numeric_limits<tick_t>::max())
            {
                return Fail("invalid numeric value");
            }
        }
        condition.value       = value;
        bool needs_comparator = false;
        bool needs_integer    = false;
        bool time_value       = false;
        bool buff_slot        = false;
        bool skill_slot       = false;
        if (type == "buff" || type == "nobuff" || type == "tbuff" || type == "tnobuff") {
            condition.kind    = ValueKind::BUFF_STACKS;
            condition.target  = type == "tbuff" || type == "tnobuff";
            const bool absent = type == "nobuff" || type == "tnobuff";
            if (absent && HAS_COMPARATOR) {
                return Fail("nobuff/tnobuff do not accept a comparator");
            }
            if (!HAS_COMPARATOR) {
                condition.value      = 0;
                condition.comparator = absent ? Comparator::EQUAL : Comparator::GREATER;
            }
            buff_slot = needs_integer = true;
        } else if (type == "bufftime" || type == "tbufftime") {
            condition.kind   = ValueKind::BUFF_DURATION;
            condition.target = type == "tbufftime";
            buff_slot = time_value = needs_comparator = true;
        } else if (type == "cd" || type == "skill_cd" || type == "skill_notin_cd") {
            condition.kind = ValueKind::SKILL_COOLDOWN;
            skill_slot = time_value = true;
            if (!HAS_COMPARATOR) {
                condition.comparator = type == "skill_notin_cd" ? Comparator::EQUAL : Comparator::GREATER;
            }
        } else if (type == "skill_energy") {
            condition.kind = ValueKind::SKILL_ENERGY;
            skill_slot = needs_comparator = needs_integer = true;
        } else if (type == "last_skill" || type == "notlast_skill") {
            if (HAS_COMPARATOR) {
                return Fail("last_skill expects a skill name without a comparator");
            }
            condition.kind       = ValueKind::LAST_SKILL;
            condition.comparator = type == "last_skill" ? Comparator::EQUAL : Comparator::NOT_EQUAL;
            condition.value      = 1;
            skill_slot           = true;
        } else if (type == "qidian" || type == "energy" || type == "rage") {
            if (NAMED) {
                return Fail("resource condition does not take a name");
            }
            condition.kind   = type == "qidian"   ? ValueKind::QIDIAN
                               : type == "energy" ? ValueKind::ENERGY
                                                  : ValueKind::RAGE;
            needs_comparator = needs_integer = true;
        } else if (type == "life" || type == "mana" || type == "tlife") {
            if (NAMED) {
                return Fail("explicit target ids are not supported by this macro compiler");
            }
            condition.kind = type == "life"   ? ValueKind::LIFE
                             : type == "mana" ? ValueKind::MANA
                                              : ValueKind::TARGET_LIFE;
            if (value > 1) {
                return Fail("life/mana must be in [0,1]");
            }
            needs_comparator = true;
        } else {
            return Fail("unknown condition: " + std::string(type));
        }
        if (needs_comparator && !HAS_COMPARATOR) {
            return Fail("condition requires a comparator");
        }
        if (needs_integer && value != std::floor(value)) {
            return Fail("stack/resource value must be an integer");
        }
        if (time_value) {
            condition.value = value * options_.frames_per_second;
            if (condition.value > std::numeric_limits<tick_t>::max()) {
                return Fail("time value overflows frame range");
            }
        }
        if (buff_slot || skill_slot) {
            if (!NAMED) {
                return Fail("condition requires a named skill/buff");
            }
            condition.slot = Resolve(NAME, buff_slot);
            if (condition.slot == INVALID_SLOT) {
                return false;
            }
        }
        return true;
    }

    bool ParseConditions(std::string_view text, MacroLine &line, LineSource &source)
    {
        auto &conditions     = result_.program.conditions;
        line.condition_begin = static_cast<Pc>(conditions.size());
        std::vector<Pc> group_starts{ 0 };
        std::size_t     start = 0;
        for (std::size_t i = 0; i <= text.size(); ++i) {
            if (i != text.size() && text[i] != '&' && text[i] != '|') {
                continue;
            }
            Condition  condition;
            const auto TOKEN = Trim(text.substr(start, i - start));
            if (!ParseCondition(TOKEN, condition)) {
                return false;
            }
            conditions.push_back(condition);
            if (options_.keep_debug_info) {
                source.conditions.emplace_back(TOKEN);
            }
            ++line.condition_count;
            if (i != text.size() && text[i] == '|') {
                group_starts.push_back(line.condition_count);
            }
            start = i + 1;
        }
        // Lower OR-of-AND groups to forward branches. No operators or AST are
        // traversed at runtime; failed AND terms skip the rest of their group.
        for (std::size_t g = 0; g < group_starts.size(); ++g) {
            const Pc END = g + 1 < group_starts.size() ? group_starts[g + 1] : line.condition_count;
            for (Pc i = group_starts[g]; i < END; ++i) {
                auto &c    = conditions[line.condition_begin + i];
                c.on_true  = i + 1 == END ? ACCEPT : i + 1;
                c.on_false = END == line.condition_count ? REJECT : END;
            }
        }
        return true;
    }

    void CompileText(std::string_view text)
    {
        if (options_.frames_per_second <= 0 || options_.skill_count > INVALID_SLOT || options_.buff_count > INVALID_SLOT)
        {
            Fail("invalid frame rate or slot count");
            return;
        }
        while (!text.empty()) {
            const auto NEWLINE      = text.find('\n');
            const auto RAW          = Trim(text.substr(0, NEWLINE));
            text                    = NEWLINE == std::string_view::npos ? std::string_view{} : text.substr(NEWLINE + 1);
            const auto CURRENT_LINE = line_number_++;
            if (RAW.empty() || RAW.starts_with('#')) {
                continue;
            }
            --line_number_;
            ParseLine(RAW, CURRENT_LINE);
            ++line_number_;
        }
    }

    void ParseLine(std::string_view raw, std::uint32_t number)
    {
        const auto COMMAND_END = raw.find_first_of(" \t");
        const auto COMMAND     = raw.substr(0, COMMAND_END);
        MacroLine  line;
        LineSource source;
        if (COMMAND == "/cast") {
            line.command = Command::CAST;
        } else if (COMMAND == "/scast") {
            line.command = Command::SEQUENTIAL_CAST;
        } else if (COMMAND == "/fcast") {
            line.command = Command::FORCE_CAST;
        } else if (COMMAND == "/sfcast") {
            line.command = Command::SEQUENTIAL_FORCE_CAST;
        } else {
            Fail("unknown command: " + std::string(COMMAND));
            return;
        }
        if (COMMAND_END == std::string_view::npos) {
            Fail("missing action");
            return;
        }
        auto body = Trim(raw.substr(COMMAND_END));
        if (body.starts_with('[')) {
            const auto CLOSE = body.find(']');
            if (CLOSE == std::string_view::npos) {
                Fail("missing closing bracket");
                return;
            }
            if (!ParseConditions(body.substr(1, CLOSE - 1), line, source)) {
                return;
            }
            body = Trim(body.substr(CLOSE + 1));
        }
        if (body.empty() || body.find_first_of("[]") != std::string_view::npos) {
            Fail("missing or invalid action");
            return;
        }
        line.action_slot = Resolve(body, false);
        if (line.action_slot == INVALID_SLOT) {
            return;
        }
        result_.program.lines.push_back(line);
        if (options_.keep_debug_info) {
            source.line   = number;
            source.text   = raw;
            source.action = body;
            result_.source.push_back(std::move(source));
        }
    }

    CompileOptions            options_;
    CompiledMacro             result_;
    NameRegistry              skills_, buffs_;
    std::vector<CompileError> errors_;
    std::uint32_t             line_number_ = 1;
};

inline bool Compare(double lhs, Comparator op, double rhs)
{
    switch (op) {
        case Comparator::LESS: return lhs < rhs;
        case Comparator::LESS_EQUAL: return lhs <= rhs;
        case Comparator::EQUAL: return lhs == rhs;
        case Comparator::NOT_EQUAL: return lhs != rhs;
        case Comparator::GREATER: return lhs > rhs;
        case Comparator::GREATER_EQUAL: return lhs >= rhs;
    }
    return false;
}

inline double ReadValue(const State &state, const Condition &c)
{
    switch (c.kind) {
        case ValueKind::QIDIAN: return state.qidian;
        case ValueKind::ENERGY: return state.energy;
        case ValueKind::RAGE: return state.rage;
        case ValueKind::LIFE: return state.life;
        case ValueKind::MANA: return state.mana;
        case ValueKind::TARGET_LIFE: return state.target_life;
        case ValueKind::LAST_SKILL: return static_cast<double>(state.last_skill == c.slot);
        case ValueKind::SKILL_ENERGY: return state.skill_energy[c.slot];
        case ValueKind::SKILL_COOLDOWN: return std::max<tick_t>(0, state.SkillReadyAt(c.slot) - state.now);
        case ValueKind::BUFF_DURATION:
        {
            const auto &expiry = c.target ? state.target_buff_expires_at : state.self_buff_expires_at;
            return std::max<tick_t>(0, expiry[c.slot] - state.now);
        }
        case ValueKind::BUFF_STACKS:
        {
            const auto &stacks = c.target ? state.target_buff_stacks : state.self_buff_stacks;
            return stacks[c.slot];
        }
    }
    return 0;
}

inline bool EvaluateCondition(const State &state, const Condition &c)
{
    return Compare(ReadValue(state, c), c.comparator, c.value);
}

inline bool IsSequential(Command c)
{
    return c == Command::SEQUENTIAL_CAST || c == Command::SEQUENTIAL_FORCE_CAST;
}

inline bool IsForceCast(Command c)
{
    return c == Command::FORCE_CAST || c == Command::SEQUENTIAL_FORCE_CAST;
}

enum class DebugPhase : std::uint8_t
{
    LINE,
    CONDITION,
    ACTION,
    FINISHED
};

struct DebugState
{
    DebugPhase phase            = DebugPhase::LINE;
    Pc         instruction      = 0;
    Pc         condition        = LINE_ENTRY;
    Pc         last_instruction = ACCEPT;
    Pc         last_condition   = ACCEPT;
    double     condition_value  = 0;
    bool       condition_result = false;
    bool       action_success   = false;
    bool       paused           = false;
    bool       budget_exhausted = false;
};

struct NoDebugState
{
};

struct NoLineHook
{
    void operator()(const MacroLine &) const { }
};

struct AllowMacroLine
{
    bool operator()(const MacroLine &) const { return true; }
};

enum class ActionBlockReason : std::uint8_t
{
    NONE,
    PREPARING,
    CHANNELING,
    SKILL_COOLDOWN,
    GLOBAL_COOLDOWN,
    QIDIAN,
    CHARGES,
    DISTANCE,
    MISSING_TALENT,
    MISSING_BUFF
};

struct ActionDiagnostic
{
    ActionBlockReason reason = ActionBlockReason::NONE;
    double            actual = 0, required = 0;
    Slot              subject     = INVALID_SLOT;
    Slot              interrupted = INVALID_SLOT;
};

struct MacroObservation
{
    std::uint64_t    ordinal     = 0;
    tick_t           frame       = 0;
    Pc               instruction = 0, condition = LINE_ENTRY, next = REJECT;
    DebugPhase       phase  = DebugPhase::LINE;
    double           actual = 0;
    bool             result = false, skipped = false;
    ActionDiagnostic action;
};

// Both specializations execute the same transitions. Only the debug
// specialization retains a resumable cursor and source-level observations.
template <bool Debug, bool TrackLastSkill = true, bool Measure = false>
class MacroExecutor
{
public:
    explicit MacroExecutor(const MacroProgram &program) : program_(program), consumed_(program.lines.size())
    {
        if constexpr (Debug) {
            BeginPass();
        }
    }

    void Reset()
    {
        std::fill(consumed_.begin(), consumed_.end(), 0);
        if constexpr (Debug) {
            BeginPass();
        }
    }

    void BeginPass()
        requires Debug
    {
        debug_ = {};
        if (program_.lines.empty()) {
            debug_.phase = DebugPhase::FINISHED;
        }
    }

    const DebugState &GetDebugState() const
        requires Debug
    {
        return debug_;
    }

    DebugState &MutableDebugState()
        requires Debug
    {
        return debug_;
    }

    const std::vector<std::uint8_t> &Consumed() const { return consumed_; }

    template <typename Action, typename BeforeLine = NoLineHook, typename Available = AllowMacroLine>
    bool EvaluateFast(State &state, Action &&action, BeforeLine before_line = {}, Available available = {})
        requires (!Debug)
    {
        return EvaluateFastLines(state,
                                 action,
                                 before_line,
                                 available,
                                 std::views::iota(Pc{ 0 }, static_cast<Pc>(program_.lines.size())));
    }

    template <class Action, class BeforeLine, class Available, class Lines>
    bool EvaluateFastLines(State &state, Action &action, BeforeLine &before_line, Available &available, const Lines &lines)
        requires (!Debug)
    {
        const PhaseScope<Measure> FRAME("MacroExecutor::EvaluateFast");
        for (const Pc LINE_INDEX : lines) {
            if (consumed_[LINE_INDEX]) {
                continue;
            }
            const auto &line = program_.lines[LINE_INDEX];
            before_line(line);
            if (!available(line)) {
                if (IsSequential(line.command)) {
                    return false;
                }
                continue;
            }
            Pc pc = line.condition_count ? 0 : ACCEPT;
            while (pc < REJECT) {
                const auto               &c = program_.conditions[line.condition_begin + pc];
                const PhaseScope<Measure> CONDITION("EvaluateCondition");
                pc = EvaluateCondition(state, c) ? c.on_true : c.on_false;
            }
            const bool SUCCESS = pc == ACCEPT && action(state, line);
            if (SUCCESS) {
                CommitAction(state, LINE_INDEX);
                return true;
            }
            if (IsSequential(line.command)) {
                return false;
            }
        }
        return false;
    }

    template <typename Action, typename BeforeLine = NoLineHook>
    bool Step(State &state, Action &&action, BeforeLine before_line = {})
        requires Debug
    {
        if (debug_.phase == DebugPhase::FINISHED) {
            return false;
        }
        const auto  INDEX = debug_.instruction;
        const auto &line  = program_.lines[INDEX];
        debug_.paused = debug_.budget_exhausted = false;
        debug_.last_instruction                 = INDEX;
        if (debug_.phase == DebugPhase::LINE) {
            if (consumed_[INDEX]) {
                AdvanceLine();
                return true;
            }
            before_line(line);
            debug_.condition = line.condition_count ? 0 : ACCEPT;
            debug_.phase     = line.condition_count ? DebugPhase::CONDITION : DebugPhase::ACTION;
        } else if (debug_.phase == DebugPhase::CONDITION) {
            debug_.last_condition   = debug_.condition;
            const auto &c           = program_.conditions[line.condition_begin + debug_.condition];
            debug_.condition_value  = ReadValue(state, c);
            debug_.condition_result = Compare(debug_.condition_value, c.comparator, c.value);
            debug_.condition        = debug_.condition_result ? c.on_true : c.on_false;
            if (debug_.condition == ACCEPT) {
                debug_.phase = DebugPhase::ACTION;
            } else if (debug_.condition == REJECT) {
                if (IsSequential(line.command)) {
                    debug_.phase = DebugPhase::FINISHED;
                } else {
                    AdvanceLine();
                }
            }
        } else {
            debug_.action_success = action(state, line);
            if (debug_.action_success) {
                CommitAction(state, INDEX);
            }
            if (debug_.action_success || IsSequential(line.command)) {
                debug_.phase = DebugPhase::FINISHED;
            } else {
                AdvanceLine();
            }
        }
        return true;
    }

private:
    void CommitAction(State &state, Pc line_index)
    {
        const auto &line = program_.lines[line_index];
        if constexpr (TrackLastSkill) {
            state.last_skill = line.action_slot;
        }
        if (IsSequential(line.command)) {
            consumed_[line_index] = 1;
        }
    }

    void AdvanceLine()
        requires Debug
    {
        ++debug_.instruction;
        debug_.condition = LINE_ENTRY;
        debug_.phase     = debug_.instruction == program_.lines.size() ? DebugPhase::FINISHED : DebugPhase::LINE;
    }

    const MacroProgram                                                       &program_;
    std::vector<std::uint8_t>                                                 consumed_;
    [[no_unique_address]] std::conditional_t<Debug, DebugState, NoDebugState> debug_;
};

using FastMacroExecutor  = MacroExecutor<false>;
using DebugMacroExecutor = MacroExecutor<true>;

class Debugger
{
public:
    using ActionCallback = bool (*)(void *, State &, const MacroLine &);

    explicit Debugger(CompiledMacro macro) :
        macro_(std::move(macro)), executor_(macro_.program), breakpoints_(macro_.program.lines.size())
    {
        if (macro_.source.size() != macro_.program.lines.size()) {
            throw std::invalid_argument("debug source map is required");
        }
        state_.Resize(macro_.program.skill_count, macro_.program.buff_count);
        executor_.BeginPass();
    }

    Debugger(const Debugger &)            = delete;
    Debugger &operator=(const Debugger &) = delete;
    Debugger(Debugger &&)                 = delete;
    Debugger &operator=(Debugger &&)      = delete;

    void SetActionCallback(ActionCallback action, void *user = nullptr)
    {
        action_ = action;
        user_   = user;
    }

    State &MutableState() { return state_; }

    const State &GetState() const { return state_; }

    const DebugState &GetDebugState() const { return executor_.GetDebugState(); }

    const CompiledMacro &Program() const { return macro_; }

    void Reset()
    {
        state_.Reset();
        executor_.Reset();
    }

    void BeginPass() { executor_.BeginPass(); }

    bool SetBreakpoint(std::uint32_t line, bool enabled = true)
    {
        for (std::size_t i = 0; i < macro_.source.size(); ++i) {
            if (macro_.source[i].line == line) {
                breakpoints_[i] = static_cast<unsigned char>(enabled);
                return true;
            }
        }
        return false;
    }

    void SetBreakpointAction(std::string_view action, bool enabled = true)
    {
        for (std::size_t i = 0; i < macro_.source.size(); ++i) {
            if (macro_.source[i].action == action) {
                breakpoints_[i] = static_cast<unsigned char>(enabled);
            }
        }
    }

    bool StepInto()
    {
        if (action_ == nullptr) {
            throw std::logic_error("a simulator action callback is required");
        }
        return executor_.Step(state_, [this](State &state, const MacroLine &line) { return action_(user_, state, line); });
    }

    bool StepOver()
    {
        const auto INDEX = GetDebugState().instruction;
        if (!StepInto()) {
            return false;
        }
        while (GetDebugState().phase != DebugPhase::FINISHED && GetDebugState().instruction == INDEX) {
            StepInto();
        }
        return true;
    }

    bool Continue(std::size_t max_steps = 100000)
    {
        auto &debug  = executor_.MutableDebugState();
        bool  skip   = debug.paused;
        debug.paused = debug.budget_exhausted = false;
        for (std::size_t i = 0; i < max_steps && debug.phase != DebugPhase::FINISHED; ++i) {
            if (!skip && debug.phase == DebugPhase::LINE && (breakpoints_[debug.instruction] != 0u) &&
                (executor_.Consumed()[debug.instruction] == 0u))
            {
                debug.paused = true;
                return true;
            }
            skip = false;
            StepInto();
        }
        debug.budget_exhausted = debug.phase != DebugPhase::FINISHED;
        return debug.budget_exhausted;
    }

private:
    CompiledMacro             macro_;
    State                     state_;
    DebugMacroExecutor        executor_;
    std::vector<std::uint8_t> breakpoints_;
    ActionCallback            action_ = nullptr;
    void                     *user_   = nullptr;
};

} // namespace JX3DPS::runtime
#endif
