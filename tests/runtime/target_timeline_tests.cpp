#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/core/runtime/attribute_gains.hpp"
#include <cstdlib>
#include <iostream>
#include <new>

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;
namespace mw = JX3DPS::runtime::mo_wen;
#define CHECK(x) do { if (!(x)) { std::cerr << __LINE__ << ": " #x "\n"; std::exit(1); } } while (false)

static bool counting = false;
static std::size_t allocations = 0;
void *operator new(std::size_t n) {
    if (counting) ++allocations;
    if (auto p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void *operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { std::free(p); }
void operator delete[](void *p, std::size_t) noexcept { std::free(p); }

template <class F> void Rejects(F f) {
    try { f(); } catch (const std::invalid_argument &) { return; }
    CHECK(false);
}

void ValidationAndPriority() {
    ValidateTargetEvents({});
    for (const auto &event : std::vector<TargetEvent>{{0, 5000, {}}, {-1, {}, 300}, {1, {}, {}},
            {1, 99, {}}, {1, 10001, {}}, {1, {}, -1}, {1, {}, 100001}, {MaximumFlowFrame + 1, 100, {}}}) {
        tx::Config a; a.target_events = {event}; Rejects([&] { tx::Prepare(a); });
        mw::Config b; b.target_events = {event}; Rejects([&] { mw::Prepare(b); });
    }
    std::vector<TargetEvent> reversed{{20, 100, {}}, {10, 200, {}}};
    Rejects([&] { ValidateTargetEvents(reversed); });
    std::vector<TargetEvent> many(257, {1, 100, {}});
    Rejects([&] { ValidateTargetEvents(many); });
    many.resize(256); ValidateTargetEvents(many);

    struct Engine {
        State state;
        DamageIntentLog log{8, 16};
        DeadlineQueue queue;
        State &MutableState() { return state; }
        DamageIntentLog &Log() { return log; }
        DeadlineQueue &Timers() { return queue; }
    } e;
    // Even a skill timer created first must observe the target change in this frame.
    auto skill = e.queue.Create({TimerKind::SkillPrepare}, 1);
    TargetTimeline timeline;
    std::vector<TargetEvent> events{{5, 5000, {}}, {5, 2500, 200}, {6, 100, {}}};
    timeline.Setup(e, events);
    timeline.Reset(e, events); e.queue.ArmAt(skill, 5);
    e.queue.Advance(5); e.state.now = 5;
    e.queue.DispatchDue([&](auto handle, auto) {
        if (handle.kind == TimerKind::TargetChange) timeline.Dispatch(e, events);
        else {
            CHECK(e.state.target_life == .25 && e.state.target_distance == 2);
            e.log.Record(5, 0, 0, 0, 0, 0, 0);
        }
    });
    State initial, scratch;
    int hits = 0;
    e.log.Reduce(initial, scratch, [&](const auto &, const auto &state) {
        CHECK(state.target_life == .25 && state.target_distance == 2); ++hits; return 1;
    });
    CHECK(hits == 1 && e.log.Mutations().size() == 3);
    CHECK(e.queue.NextDelta() == 1);
}

template <class Config>
void SpecializationSchedule(Config config, CompileOptions options, const char *skill) {
    const auto [macro, errors] = MacroCompiler::Compile(std::string("/cast [tlife<0.5] ") + skill, options);
    CHECK(errors.empty());
    using Rules = typename Config::RuntimeModule::Rules;
    config.target.distance = 100;
    config.target_events = {{7, 2500, {}}, {13, {}, 300}, {32, 8000, 10000}, {64, 100, 100}};
    Simulation<Rules> fast(macro.program, LoadSpecialization(config), 20000, 50000);
    Simulation<Rules, true> debug(macro.program, LoadSpecialization(config), 20000, 50000);
    fast.Start(64, 42); fast.Run();
    debug.Start(64, 42); while (!debug.Finished()) debug.Continue();
    CHECK(!fast.Log().Intents().empty());
    CHECK(fast.Log().Intents().front().frame == 13);
    CHECK(fast.Log().Intents() == debug.Log().Intents());
    CHECK(fast.Log().Mutations() == debug.Log().Mutations());
    CHECK(fast.TotalDamage() == debug.TotalDamage());
    CHECK(fast.GetState().target_life == .8 && fast.GetState().target_distance == 100);
    const auto mutations = fast.Log().Mutations();
    const auto damage = fast.TotalDamage();
    allocations = 0; counting = true;
    for (int repeat = 0; repeat < 5; ++repeat) {
        fast.Start(64, 42); fast.Run();
        CHECK(fast.TotalDamage() == damage && fast.Log().Mutations() == mutations);
    }
    counting = false; CHECK(allocations == 0);

    // A life-only event wakes tlife even with no skill ready/expiry timer at this frame.
    config.target.distance = 3;
    config.target_events = {{7, 2500, {}}};
    Simulation<Rules> wake(macro.program, LoadSpecialization(config), 20000, 50000);
    wake.Start(8, 42); wake.Run();
    CHECK(!wake.Log().Intents().empty() && wake.Log().Intents().front().frame == 7);
    wake.Start(7, 42); wake.Run();
    CHECK(wake.Log().Intents().empty() && wake.GetState().target_life == 1);
    wake.Start(0, 42); wake.Run(); CHECK(wake.Log().Mutations().empty());

    BatchOptions batch;
    batch.duration = 64; batch.iterations = 8; batch.workers = 1; batch.seed = 73;
    batch.damage_capacity = 20000; batch.mutation_capacity = 50000;
    const auto single = RunBatch(macro.program, batch, LoadSpecialization(config));
    batch.workers = 3;
    const auto multi = RunBatch(macro.program, batch, LoadSpecialization(config));
    CHECK(single.checksum == multi.checksum);
    BatchControl control;
    const auto gains = RunAttributeGainsControlled(macro.program, batch, control, config);
    CHECK(gains.primary.checksum == multi.checksum);
    for (int i = 0; i < int(AttributeGains.size()); ++i) {
        const auto changed = ApplyAttributeGain(config, i);
        CHECK(changed.target_events == config.target_events);
        const auto expected = RunBatch(macro.program, batch, LoadSpecialization(changed));
        CHECK(gains.gains[i] && gains.gains[i]->checksum == expected.checksum);
    }
}

int main() {
    ValidationAndPriority();
    SpecializationSchedule(tx::Config{}, tx::MacroOptions(), "sanchai");
    SpecializationSchedule(mw::Config{}, mw::MacroOptions(), "yu");
    std::cout << "Target timeline validation, order, wakeups, replay, reset, parity, gains and allocation tests passed\n";
}
