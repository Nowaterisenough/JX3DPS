#include "src/core/runtime.hpp"
#include "src/core/runtime/descriptors.hpp"
#include "fixture_rules.hpp"
#include "src/core/runtime/batch.hpp"
#include <cstdlib>
#include <iostream>
#include <new>
#include <random>

static bool count_allocations = false;
static std::size_t allocations = 0;
void *operator new(std::size_t size) {
    if (count_allocations) ++allocations;
    if (auto *p = std::malloc(size ? size : 1)) return p;
    throw std::bad_alloc();
}
void *operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { std::free(p); }
void operator delete[](void *p, std::size_t) noexcept { std::free(p); }

using namespace JX3DPS;
using namespace JX3DPS::runtime;

#define CHECK(condition) do { if (!(condition)) { std::cerr << __LINE__ << ": " #condition "\n"; std::exit(1); } } while (false)
template <typename Exception, typename Fn> void Throws(Fn &&fn) {
    try { fn(); } catch (const Exception &) { return; }
    CHECK(false);
}

inline constexpr DescriptorTable<SkillDescriptor, 3> skills{{{
    {32705, "strike", 24, 24, 0}, {999999, "filler", 0, 16, 0}, {65536, "burst", 160, 0, 0}
}}};
inline constexpr DescriptorTable<BuffDescriptor, 2> buffs{{{
    {88888, "power", 160, 0, 3}, {32705, "dot", 96, 16, 5}
}}};
static_assert(skills.Valid() && buffs.Valid());
static_assert(skills.FindId(999999) == 1 && buffs.FindId(32705) == 1);
static_assert(sizeof(FastMacroExecutor) < sizeof(DebugMacroExecutor));
static_assert(sizeof(Simulation<FixtureRules>) < sizeof(Simulation<FixtureRules, true>));

CompileOptions Options(bool debug = true) {
    CompileOptions options;
    options.skill_count = skills.entries.size();
    options.buff_count = buffs.entries.size();
    options.resolve_skill = [](auto name) { return skills.FindName(name); };
    options.resolve_buff = [](auto name) { return buffs.FindName(name); };
    options.keep_debug_info = debug;
    return options;
}
CompiledMacro Compile(std::string_view text, bool debug = true) {
    auto [macro, errors] = MacroCompiler::Compile(text, Options(debug));
    for (const auto &e : errors) std::cerr << e.line << ": " << e.message << '\n';
    CHECK(errors.empty());
    return macro;
}
State MakeState() {
    State s;
    s.Resize(skills.entries.size(), buffs.entries.size());
    return s;
}
bool Action(State &state, const MacroLine &line) {
    if (state.skill_ready_at[line.action_slot] > state.now) return false;
    if (line.action_slot == 0 && state.qidian < 3) return false;
    state.qidian += line.action_slot == 0 ? -3 : 1;
    state.skill_ready_at[line.action_slot] = state.now + skills.entries[line.action_slot].cooldown;
    return true;
}

void CompilerTests() {
    auto macro = Compile("# comment\n\n/cast [ qidian > 5 & tbuff:dot>=2 | nobuff:power ] strike\r\n/cast filler");
    CHECK(macro.source[0].line == 3);
    CHECK(macro.program.skill_count == 3 && macro.program.buff_count == 2);
    CHECK(sizeof(MacroLine) <= 16 && sizeof(Condition) <= 24);
    CHECK(Compile("/cast strike", false).source.empty());
    for (auto text : {"/cast [qidian>5&] strike", "/cast [] strike", "/cast [buff:unknown] strike", "/cast unknown",
                      "/cast [qidian20] strike", "/cast [qidian>2.1] strike", "/cast [bufftime:power>2147483647] strike",
                      "/cast [qidian>nan] strike", "/cast [tnobuff:power=1] strike", "/cast [buff:power strike",
                      "/cast [qidian>5||rage>1] strike", "macro one", "/cast [tlife:1<0.4] strike"}) {
        auto [bad, errors] = MacroCompiler::Compile(text, Options());
        CHECK(!errors.empty());
        CHECK(bad.program.lines.empty() && bad.program.conditions.empty());
    }
    auto invalid = Options(); invalid.frames_per_second = 0;
    CHECK(!MacroCompiler::Compile("/cast strike", invalid).second.empty());
    invalid = Options(); invalid.skill_count = 1;
    CHECK(!MacroCompiler::Compile("/cast filler", invalid).second.empty());

    auto times = Compile("/cast [bufftime:power>0.125&skill_cd:strike<=1.5] filler");
    CHECK(times.program.conditions[0].value == 2 && times.program.conditions[1].value == 24);
    auto state = MakeState(); state.now = 16;
    state.self_buff_expires_at[0] = 19; state.skill_ready_at[0] = 40;
    FastMacroExecutor vm(times.program);
    CHECK(vm.EvaluateFast(state, Action));
    state.self_buff_expires_at[0] = 18;
    CHECK(!vm.EvaluateFast(state, Action));

    auto existence = Compile("/cast [buff:power&tnobuff:dot] strike");
    state = MakeState(); state.self_buff_stacks[0] = 1;
    FastMacroExecutor exists(existence.program);
    CHECK(exists.EvaluateFast(state, Action));
    state.skill_ready_at[0] = 0; state.target_buff_stacks[1] = 1;
    CHECK(!exists.EvaluateFast(state, Action));

    auto last = Compile("/cast [last_skill:strike] filler");
    FastMacroExecutor last_vm(last.program);
    CHECK(last_vm.EvaluateFast(state, Action));
    CHECK(!last_vm.EvaluateFast(state, Action));
}

void TruthTableTests() {
    auto macro = Compile("/cast [qidian=1|energy=1&rage=1] strike");
    FastMacroExecutor fast(macro.program);
    DebugMacroExecutor debug(macro.program);
    auto action = [](State &, const MacroLine &) { return true; };
    for (int a = 0; a != 2; ++a) for (int b = 0; b != 2; ++b) for (int c = 0; c != 2; ++c) {
        auto state = MakeState(); state.qidian = a; state.energy = b; state.rage = c;
        auto other = state;
        CHECK(fast.EvaluateFast(state, action) == bool(a || (b && c)));
        debug.BeginPass();
        std::vector<Pc> visited;
        while (debug.GetDebugState().phase != DebugPhase::Finished) {
            if (debug.GetDebugState().phase == DebugPhase::Condition) visited.push_back(debug.GetDebugState().condition);
            debug.Step(other, action);
        }
        CHECK(other.last_skill == state.last_skill);
        CHECK(visited.size() == (a ? 1u : b ? 3u : 2u));
    }
    macro = Compile("/cast [qidian=1&energy=1|rage=1] strike");
    FastMacroExecutor and_or(macro.program);
    for (int a = 0; a != 2; ++a) for (int b = 0; b != 2; ++b) for (int c = 0; c != 2; ++c) {
        auto state = MakeState(); state.qidian = a; state.energy = b; state.rage = c;
        CHECK(and_or.EvaluateFast(state, action) == bool((a && b) || c));
    }
}

void ModeParityTests() {
    auto macro = Compile("/scast [qidian>=5] burst\n/cast [tbuff:dot>=2|buff:power&qidian>5] strike\n/cast filler");
    FastMacroExecutor fast(macro.program);
    DebugMacroExecutor debug(macro.program);
    std::mt19937 rng(17);
    for (int iteration = 0; iteration < 1000; ++iteration) {
        fast.Reset(); debug.Reset();
        auto state = MakeState(), other = state;
        for (int pass = 0; pass < 20; ++pass) {
            state.now = pass * 16;
            state.qidian = static_cast<int>(rng() % 11);
            state.self_buff_stacks[0] = static_cast<int>(rng() % 2);
            state.target_buff_stacks[1] = static_cast<int>(rng() % 5);
            other = state;
            const bool success = fast.EvaluateFast(state, Action);
            debug.BeginPass();
            while (debug.Step(other, Action)) {}
            CHECK(success == debug.GetDebugState().action_success);
            CHECK(state.last_skill == other.last_skill && state.qidian == other.qidian);
            CHECK(state.skill_ready_at == other.skill_ready_at);
            CHECK(fast.Consumed() == debug.Consumed());
        }
    }
    auto sequential = Compile("/scast [qidian>5] burst\n/cast filler");
    FastMacroExecutor vm(sequential.program);
    auto state = MakeState(); state.qidian = 0;
    CHECK(!vm.EvaluateFast(state, Action));
    state.qidian = 10; state.skill_ready_at[2] = 10;
    CHECK(!vm.EvaluateFast(state, Action));
    state.now = 10;
    CHECK(vm.EvaluateFast(state, Action) && state.last_skill == 2);
    CHECK(vm.EvaluateFast(state, Action) && state.last_skill == 1);
}

void DebuggerTests() {
    const MacroProgram empty;
    DebugMacroExecutor empty_debug(empty);
    auto empty_state = MakeState();
    CHECK(!empty_debug.Step(empty_state, Action));
    auto macro = Compile(std::string(5000, '\n') + "/cast [qidian>5&tbuff:dot>=2] strike\n/cast filler");
    Debugger debugger(std::move(macro));
    debugger.SetActionCallback([](void *, State &state, const MacroLine &line) { return Action(state, line); });
    CHECK(debugger.SetBreakpoint(5001));
    CHECK(!debugger.SetBreakpoint(2));
    CHECK(debugger.Continue() && debugger.GetDebugState().paused);
    CHECK(debugger.GetState().last_skill == INVALID_SLOT);
    CHECK(!debugger.Continue());
    CHECK(debugger.GetState().last_skill == 1);
    debugger.Reset();
    CHECK(debugger.StepOver());
    CHECK(debugger.GetDebugState().instruction == 1);
    CHECK(debugger.StepOver());
    CHECK(debugger.GetDebugState().phase == DebugPhase::Finished);
    CHECK(!debugger.StepInto());
    debugger.Reset();
    CHECK(debugger.Continue(0) && debugger.GetDebugState().budget_exhausted);
    CHECK(!debugger.GetDebugState().paused);
    debugger.BeginPass();
    CHECK(debugger.StepInto());
    CHECK(debugger.GetDebugState().phase == DebugPhase::Condition);
    CHECK(debugger.StepInto());
    CHECK(debugger.GetDebugState().last_condition == 0 && debugger.GetDebugState().condition_result);
}

void SchedulerTests() {
    DeadlineQueue queue;
    auto a = queue.Create({TimerKind::BuffDuration, 1, true}, 10);
    auto b = queue.Create({TimerKind::BuffInterval, 1, true}, 0);
    CHECK(!queue.NextDelta());
    queue.Arm(a, 0); queue.Arm(b, 0);
    CHECK(queue.NextDelta() == 0);
    std::vector<DeadlineQueue::TimerId> fired;
    queue.DispatchDue([&](auto, auto id) { fired.push_back(id); });
    CHECK((fired == std::vector<DeadlineQueue::TimerId>{b, a}));
    for (int i = 0; i < 1000000; ++i) queue.Arm(a, 10 + i % 2);
    CHECK(queue.Pending() == 1 && queue.NextDelta() == 11);
    queue.Arm(a, 5); queue.Arm(b, 10); queue.Cancel(a);
    CHECK(queue.NextDelta() == 10);
    Throws<std::logic_error>([&] { queue.Advance(11); });
    queue.Advance(10);
    queue.DispatchDue([&](auto, auto) { queue.Arm(a, 2); });
    CHECK(queue.NextDelta() == 2);
    queue.Reset(); CHECK(!queue.NextDelta() && queue.Now() == 0);
    queue.Arm(a, 0);
    Throws<std::runtime_error>([&] { queue.DispatchDue([&](auto, auto id) { queue.Arm(id, 0); }, 5); });
    queue.Reset();
    Throws<std::out_of_range>([&] { queue.Arm(a, -1); });
    queue.Advance(std::numeric_limits<tick_t>::max());
    Throws<std::out_of_range>([&] { queue.Arm(a, 1); });

    struct Ref { int at = -1; std::uint64_t order = 0; };
    std::array<Ref, 32> ref{};
    DeadlineQueue randomized;
    for (Slot i = 0; i < ref.size(); ++i) randomized.Create({TimerKind::SkillCooldown, i, false});
    std::mt19937 rng(88);
    std::uint64_t order = 0;
    for (int step = 0; step < 10000; ++step) {
        auto id = rng() % ref.size();
        if (rng() % 4 == 0) { randomized.Cancel(static_cast<Slot>(id)); ref[id].at = -1; }
        else {
            auto at = randomized.Now() + static_cast<int>(rng() % 50);
            ref[id] = {at, order++};
            randomized.ArmAt(static_cast<Slot>(id), at);
        }
        int earliest = -1;
        for (std::size_t i = 0; i < ref.size(); ++i) {
            if (ref[i].at >= 0 && (earliest < 0 || ref[i].at < ref[earliest].at ||
                (ref[i].at == ref[earliest].at && ref[i].order < ref[earliest].order))) earliest = static_cast<int>(i);
        }
        CHECK((earliest < 0) == !randomized.NextDelta());
        if (earliest >= 0) CHECK(randomized.NextDelta() == ref[earliest].at - randomized.Now());
        if (earliest >= 0 && step % 3 == 0) {
            randomized.Advance(*randomized.NextDelta());
            randomized.DispatchDue([&](auto, auto fired_id) {
                int expected = -1;
                for (std::size_t i = 0; i < ref.size(); ++i) {
                    if (ref[i].at == randomized.Now() && (expected < 0 || ref[i].order < ref[expected].order)) expected = static_cast<int>(i);
                }
                CHECK(expected == static_cast<int>(fired_id)); ref[fired_id].at = -1;
            });
        }
    }
}

void DamageTests() {
    auto initial = MakeState(), state = initial, scratch = initial;
    DamageIntentLog log(4, 8);
    state.now = 100;
    SetBuff(state, log, true, 1, 200, 116, 3);
    log.Record(100, 0, 0, 1, 42, 10, 1, RollResult::DOUBLE);
    SetBuff(state, log, true, 1, 0, 0, 0);
    log.Record(100, 0, 0, 1, 43, 10, 1);
    SetResources(state, log, 2, 4, 5);
    log.Record(120, 1, 0, 1, 44, 10, 1);
    std::vector<value_t> damages;
    auto total = log.Reduce(initial, scratch, [&](const DamageIntent &intent, const State &historical) {
        CHECK(historical.now == intent.frame);
        const auto damage = intent.base * (1 + historical.target_buff_stacks[1]) * (intent.outcome == RollResult::DOUBLE ? 2 : 1);
        damages.push_back(damage);
        if (intent.frame == 120) CHECK(historical.qidian == 2 && historical.energy == 4);
        return damage;
    });
    CHECK(total == 100 && (damages == std::vector<value_t>{80, 10, 10}));
    CHECK(state.target_buff_stacks[1] == 0 && initial.qidian == 10);
    Throws<std::invalid_argument>([&] { log.Record(99, 0, 0, 0, 0, 0, 0); });
    log.Clear();
    auto *storage = log.Intents().data();
    for (int i = 0; i < 4; ++i) log.Record(i, 0, 0, 0, 0, 1, 0);
    Throws<std::length_error>([&] { log.Record(4, 0, 0, 0, 0, 1, 0); });
    CHECK(log.Size() == 4 && log.Intents().data() == storage);
    log.Clear(); log.Record(0, 0, 0, 0, 0, 1, 0);
    CHECK(log.Intents()[0].sequence == 0);
    Throws<std::out_of_range>([&] { SetBuff(state, log, true, 32705, 0, 0, 1); });
}

void AllocationTests() {
    auto macro = Compile("/cast [qidian>5&tnobuff:dot] strike\n/cast filler", false);
    FastMacroExecutor executor(macro.program);
    auto state = MakeState(), initial = state, scratch = state;
    DamageIntentLog log(4, 4);
    DeadlineQueue queue;
    auto timer = queue.Create({});
    allocations = 0; count_allocations = true;
    value_t checksum = 0;
    for (int i = 0; i < 10000; ++i) {
        state.Reset(); executor.Reset(); log.Clear(); queue.Reset();
        CHECK(executor.EvaluateFast(state, Action));
        SetBuff(state, log, false, 0, 16, 0, 1);
        log.Record(0, 0, 0, 0, 0, 1, 0);
        checksum += log.Reduce(initial, scratch, [](const auto &hit, const auto &s) { return hit.base + s.self_buff_stacks[0]; });
        queue.Arm(timer, 1); queue.Arm(timer, 2);
        queue.Advance(2); queue.DispatchDue([](auto, auto) {});
    }
    count_allocations = false;
    CHECK(checksum == 20000 && allocations == 0);
}

void BoundaryTests() {
    auto state = MakeState();
    state.skill_ready_at[0] = 32;
    MacroProgram program;
    program.conditions.push_back({});
    auto &c = program.conditions[0]; c.slot = 0; c.kind = ValueKind::SkillCooldown;
    for (int op = 0; op < 6; ++op) for (int v = 0; v < 70; ++v) for (int now = 0; now < 35; ++now) {
        c.comparator = static_cast<Comparator>(op); c.value = v / 2.0; state.now = now;
        const bool original = EvaluateCondition(state, c);
        std::optional<tick_t> expected;
        for (int future = now + 1; future < 36; ++future) {
            auto check = state; check.now = future;
            if (EvaluateCondition(check, c) != original) { expected = future; break; }
        }
        CHECK(NextMacroBoundary(program, state) == expected);
    }
}

void SimulationTests() {
    auto macro = Compile("/scast [bufftime:power<9.5] strike");
    Simulation<FixtureRules> threshold(macro.program);
    threshold.Start(200, 7); threshold.Run();
    CHECK(threshold.Log().Size() == 1 && threshold.Log().Intents()[0].frame == 9);
    CHECK(threshold.GetState().self_buff_stacks[0] == 0);

    macro = Compile("/cast [qidian>=6] strike\n/cast [skill_cd:burst=0] burst\n/cast filler");
    Simulation<FixtureRules> fast(macro.program);
    Simulation<FixtureRules, true> debug(macro.program);
    for (std::uint64_t seed = 0; seed < 100; ++seed) {
        fast.Start(4800, seed); debug.Start(4800, seed);
        fast.Run(); debug.Continue(100000);
        CHECK(fast.Finished() && debug.Finished());
        CHECK(fast.Log().Intents() == debug.Log().Intents());
        CHECK(fast.Log().Mutations() == debug.Log().Mutations());
        CHECK(fast.TotalDamage() == debug.TotalDamage());
        CHECK(fast.TotalDamage() == fast.GetRules().immediate_total);
        CHECK(fast.Log().Intents()[0].frame == 0);
        CHECK(fast.Log().Intents().back().frame < 4800);
    }
    debug.Start(100, 3);
    debug.SetBreakpoint(0);
    CHECK(debug.Continue() && debug.DebugControl().paused);
    CHECK(debug.GetState().now == 0 && debug.Log().Size() == 0);
    debug.StepOver();
    CHECK(debug.Log().Size() == 1);
    CHECK(debug.Continue() && debug.DebugControl().paused);
    debug.SetBreakpoint(0, false);
    debug.Continue(); CHECK(debug.Finished());
    fast.Start(0, 0); fast.Run(); CHECK(fast.Log().Size() == 0 && fast.TotalDamage() == 0);
    fast.Start(4800, 9);
    allocations = 0; count_allocations = true;
    fast.Run();
    const auto total = fast.TotalDamage();
    count_allocations = false;
    CHECK(allocations == 0 && total > 0);
}

void ObservationTests() {
    auto macro = Compile("/scast [qidian<1&energy>0|qidian>=6] strike\n/cast filler");
    Simulation<FixtureRules, true> debug(macro.program);
    std::vector<MacroObservation> steps;
    debug.SetDebugObserver([](void *context, const MacroObservation &step) {
        static_cast<std::vector<MacroObservation> *>(context)->push_back(step);
    }, &steps);
    debug.Start(64, 1);
    debug.SetBreakpoint(0);
    debug.Continue();
    CHECK(debug.DebugControl().paused && steps.empty());
    debug.StepInto(); // Enter the line; no predicate has executed yet.
    CHECK(steps.empty());
    debug.StepInto();
    CHECK(steps.size() == 1 && steps[0].ordinal == 1 && steps[0].frame == 0);
    CHECK(steps[0].condition == 0 && steps[0].actual == 7 && !steps[0].result && steps[0].next == 2);
    debug.StepInto();
    CHECK(steps.size() == 2 && steps[1].condition == 2 && steps[1].result && steps[1].next == ACCEPT);
    debug.StepInto();
    CHECK(steps.size() == 3 && steps[2].phase == DebugPhase::Action && steps[2].result);
    CHECK(debug.DebugControl().new_pass && debug.MacroDebugState().phase == DebugPhase::Line);
    debug.StepInto();
    CHECK(steps.back().skipped && steps.back().instruction == 0);
    debug.Continue();
    CHECK(debug.Finished());
    Simulation<FixtureRules> fast(macro.program);
    fast.Start(64, 1); fast.Run();
    CHECK(debug.Log().Intents() == fast.Log().Intents());
    CHECK(debug.Log().Mutations() == fast.Log().Mutations());
    CHECK(debug.TotalDamage() == fast.TotalDamage());
    steps.clear();
    debug.Start(64, 1); debug.Continue();
    CHECK(debug.DebugControl().paused && steps.empty()); // Start retains the observer and breakpoints.
    debug.StepOver();
    CHECK(steps.front().ordinal == 1);

    auto timerMacro = Compile("/cast [buff:power] strike");
    Simulation<FixtureRules, true> timers(timerMacro.program);
    steps.clear();
    timers.SetDebugObserver([](void *context, const MacroObservation &step) {
        static_cast<std::vector<MacroObservation> *>(context)->push_back(step);
    }, &steps);
    timers.Start(161, 1); timers.Continue();
    CHECK(timers.Finished());
    CHECK(steps.back().frame == 160 && steps.back().phase == DebugPhase::Condition && steps.back().actual == 0);

    MacroProgram empty;
    empty.skill_count = 3; empty.buff_count = 2;
    Simulation<FixtureRules, true> idle(empty);
    steps.clear();
    idle.SetDebugObserver([](void *context, const MacroObservation &step) {
        static_cast<std::vector<MacroObservation> *>(context)->push_back(step);
    }, &steps);
    idle.Start(200, 1); idle.Continue();
    CHECK(idle.Finished() && steps.empty());
}

void CastingOwnershipTests() {
    CompileOptions options;
    options.skill_count = 1;
    options.buff_count = 0;
    options.resolve_skill = [](std::string_view name) { return name == "channel" ? Slot{0} : INVALID_SLOT; };
    options.keep_debug_info = true;
    auto [normal_macro, normal_errors] = MacroCompiler::Compile("/scast channel\n/scast channel", options);
    CHECK(normal_errors.empty());
    auto [force_macro, force_errors] = MacroCompiler::Compile("/scast channel\n/sfcast channel", options);
    CHECK(force_errors.empty());

    Simulation<ChannelFixtureRules> normal(normal_macro.program);
    Simulation<ChannelFixtureRules> forced(force_macro.program);
    normal.Start(9, 1); normal.Run();
    forced.Start(9, 1); forced.Run();
    CHECK(normal.Log().Size() == 1 && forced.Log().Size() == 1);
    CHECK(normal.Log().Intents()[0].frame == 8 && forced.Log().Intents()[0].frame == 8);
    CHECK(normal.GetState().casting_skill == 0); // The second normal cast starts at completion.
    CHECK(forced.GetState().casting_skill == INVALID_SLOT);
    CHECK(normal.Log().Mutations().size() == 3); // start, complete, second start
    CHECK(forced.Log().Mutations().size() == 4); // start, interrupt, start, complete

    Simulation<ChannelFixtureRules, true> debug(force_macro.program);
    debug.Start(9, 1); debug.Continue();
    CHECK(debug.Log().Intents() == forced.Log().Intents());
    CHECK(debug.Log().Mutations() == forced.Log().Mutations());
    CHECK(debug.TotalDamage() == forced.TotalDamage());
}

void BatchTests() {
    auto macro = Compile("/cast [qidian>=6] strike\n/cast burst\n/cast filler");
    BatchOptions options;
    options.iterations = 101;
    options.duration = 480;
    options.seed = 100;
    auto one = RunBatch<FixtureRules>(macro.program, options);
    options.workers = 4;
    auto four = RunBatch<FixtureRules>(macro.program, options);
    CHECK(one.count == 101 && four.count == one.count && four.checksum == one.checksum);
    CHECK(one.min_damage == four.min_damage && one.max_damage == four.max_damage);
    CHECK(std::abs(one.mean_damage - four.mean_damage) < 1e-8);
    CHECK(std::abs(one.PopulationDeviation() - four.PopulationDeviation()) < 1e-8);
    BatchControl control;
    auto controlled = RunBatchControlled<FixtureRules>(macro.program, options, control);
    CHECK(controlled.count == four.count && controlled.checksum == four.checksum);
    CHECK(control.completed == controlled.count);
    CHECK(std::abs(controlled.mean_damage - four.mean_damage) < 1e-8);
    control.cancel_requested = true;
    CHECK(RunBatchControlled<FixtureRules>(macro.program, options, control).count == 0);
    CHECK(control.completed == 0);
    control.cancel_requested = false;
    struct CancellingRules : FixtureRules {
        BatchControl *control;
        int starts = 0;
        void Reset(Simulation<CancellingRules> &engine, std::uint64_t seed) {
            FixtureRules::Reset(engine, seed);
            if (++starts == 7) control->cancel_requested = true;
        }
    };
    options.workers = 1;
    CancellingRules cancelling;
    cancelling.control = &control;
    auto partial = RunBatchControlled(macro.program, options, control, cancelling);
    CHECK(partial.count == 7 && control.completed == 7);
    options.iterations = 7;
    auto first_seven = RunBatch<FixtureRules>(macro.program, options);
    CHECK(partial.checksum == first_seven.checksum && partial.mean_damage == first_seven.mean_damage);
    options.iterations = 101;
    options.workers = 4;
    // Reproduce a chosen batch iteration using the debuggable specialization.
    Simulation<FixtureRules, true> selected(macro.program);
    selected.Start(options.duration, IterationSeed(options.seed, 42));
    selected.Continue();
    Simulation<FixtureRules> reference(macro.program);
    reference.Start(options.duration, IterationSeed(options.seed, 42));
    reference.Run();
    CHECK(selected.Log().Intents() == reference.Log().Intents());
    options.damage_capacity = 1;
    Throws<std::length_error>([&] { RunBatch<FixtureRules>(macro.program, options); });
    control.cancel_requested = false;
    Throws<std::length_error>([&] { RunBatchControlled<FixtureRules>(macro.program, options, control); });
    CHECK(control.cancel_requested && control.completed == 0);
    options.workers = 0;
    Throws<std::invalid_argument>([&] { RunBatch<FixtureRules>(macro.program, options); });
}

int main() {
    CompilerTests(); TruthTableTests(); ModeParityTests(); DebuggerTests(); ObservationTests();
    SchedulerTests(); DamageTests(); AllocationTests();
    BoundaryTests(); SimulationTests(); CastingOwnershipTests(); BatchTests();
    std::cout << "runtime tests passed: compiler, short circuit, 20000 mode comparisons, debugger, scheduler, replay, casting ownership, 100 fight comparisons, threshold wakeups, zero hot allocations\n";
}
