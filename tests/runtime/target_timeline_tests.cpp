#include <cstdlib>
#include <iostream>
#include <new>
#include <stdexcept>
#include <utility>
#include <vector>

#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/attribute_gains.hpp"

#include "check.hpp"

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;
namespace mw = JX3DPS::runtime::mo_wen;

static bool        counting    = false;
static std::size_t allocations = 0;

void *operator new(std::size_t n)
{
    if (counting) {
        ++allocations;
    }
    if (auto *p = std::malloc((n != 0u) ? n : 1)) {
        return p;
    }
    throw std::bad_alloc();
}

void *operator new[](std::size_t n)
{
    return ::operator new(n);
}

void operator delete(void *p) noexcept
{
    std::free(p);
}

void operator delete[](void *p) noexcept
{
    std::free(p);
}

void operator delete(void *p, std::size_t) noexcept
{
    std::free(p);
}

void operator delete[](void *p, std::size_t) noexcept
{
    std::free(p);
}

template <class F>
void Rejects(F f)
{
    try {
        f();
    } catch (const std::invalid_argument &) {
        return;
    }
    CHECK(false);
}

void ValidationAndPriority()
{
    ValidateTargetEvents({});
    for (const auto &event :
         std::vector<TargetEvent>{
             { 0,                    5000,  {}     },
             { -1,                   {},    300    },
             { 1,                    {},    {}     },
             { 1,                    99,    {}     },
             { 1,                    10001, {}     },
             { 1,                    {},    -1     },
             { 1,                    {},    100001 },
             { 1,                    {},    {}, 10001, {} },
             { 1,                    {},    {}, {}, 0 },
             { 1,                    {},    {}, {}, 10001 },
             { MaximumFlowFrame + 1, 100,   {}     }
    })
    {
        tx::Config a;
        a.target_events = { event };
        Rejects([&] { tx::Prepare(a); });
        mw::Config b;
        b.target_events = { event };
        Rejects([&] { mw::Prepare(b); });
    }
    std::vector<TargetEvent> reversed{
        { 20, 100, {} },
        { 10, 200, {} }
    };
    Rejects([&] { ValidateTargetEvents(reversed); });
    std::vector<TargetEvent> many(257, { 1, 100, {} });
    Rejects([&] { ValidateTargetEvents(many); });
    many.resize(256);
    ValidateTargetEvents(many);

    struct Engine
    {
        State           state;
        DamageIntentLog log{ 8, 16 };
        DeadlineQueue   queue;

        State &MutableState() { return state; }

        DamageIntentLog &Log() { return log; }

        DeadlineQueue &Timers() { return queue; }
    } e;

    // Even a skill timer created first must observe the target change in this frame.
    const auto               SKILL = e.queue.Create({ TimerKind::SKILL_PREPARE }, 1);
    TargetTimeline           timeline;
    std::vector<TargetEvent> events{
        { 5, 5000, {}, 8000, 1500 },
        { 5, 2500, 200, {}, 1500 },
        { 6, 100,  {}  }
    };
    timeline.Setup(e, events);
    timeline.Reset(e, events);
    e.queue.ArmAt(SKILL, 5);
    e.queue.Advance(5);
    e.state.now = 5;
    e.queue.DispatchDue([&](auto handle, auto) {
        if (handle.kind == TimerKind::TARGET_CHANGE) {
            timeline.Dispatch(e, events);
        } else {
            CHECK(e.state.target_life == .25 && e.state.target_distance == 2 && e.state.life == .5);
            e.log.Record(5, 0, 0, 0, 0, 0, 0);
        }
    });
    State initial;
    State scratch;
    int   hits = 0;
    e.log.Reduce(initial, scratch, [&](const auto &, const auto &state) {
        CHECK(state.target_life == .25 && state.target_distance == 2 && state.life == .5);
        ++hits;
        return 1;
    });
    CHECK(hits == 1 && e.log.Mutations().size() == 5);
    CHECK(e.queue.NextDelta() == 1);
}

template <class Config>
void SpecializationSchedule(Config config, CompileOptions options, const char *skill)
{
    const auto [MACRO, ERRORS] = MacroCompiler::Compile(std::string("/cast [tlife<0.5] ") + skill, options);
    CHECK(ERRORS.empty());
    using Rules            = typename Config::RuntimeModule::Rules;
    config.target.distance = 100;
    config.target_events   = {
        { 7,  2500, {}    },
        { 13, {},   300   },
        { 32, 8000, 10000 },
        { 64, 100,  100   }
    };
    Simulation<Rules>       fast(MACRO.program, LoadSpecialization(config), 20000, 50000);
    Simulation<Rules, true> debug(MACRO.program, LoadSpecialization(config), 20000, 50000);
    fast.Start(64, 42);
    fast.Run();
    debug.Start(64, 42);
    while (!debug.Finished()) {
        debug.Continue();
    }
    CHECK(!fast.Log().Intents().empty());
    CHECK(fast.Log().Intents().front().frame == 13);
    CHECK(fast.Log().Intents() == debug.Log().Intents());
    CHECK(fast.Log().Mutations() == debug.Log().Mutations());
    CHECK(fast.TotalDamage() == debug.TotalDamage());
    CHECK(fast.GetState().target_life == .8 && fast.GetState().target_distance == 100);
    const auto MUTATIONS = fast.Log().Mutations();
    const auto DAMAGE    = fast.TotalDamage();
    allocations          = 0;
    counting             = true;
    for (int repeat = 0; repeat < 5; ++repeat) {
        fast.Start(64, 42);
        fast.Run();
        CHECK(fast.TotalDamage() == DAMAGE && fast.Log().Mutations() == MUTATIONS);
    }
    counting = false;
    CHECK(allocations == 0);

    // A life-only event wakes tlife even with no skill ready/expiry timer at this frame.
    config.target.distance = 3;
    config.target_events   = {
        { 7, 2500, {} }
    };
    Simulation<Rules> wake(MACRO.program, LoadSpecialization(config), 20000, 50000);
    wake.Start(8, 42);
    wake.Run();
    CHECK(!wake.Log().Intents().empty() && wake.Log().Intents().front().frame == 7);
    wake.Start(7, 42);
    wake.Run();
    CHECK(wake.Log().Intents().empty() && wake.GetState().target_life == 1);
    wake.Start(0, 42);
    wake.Run();
    CHECK(wake.Log().Mutations().empty());

    BatchOptions batch;
    batch.duration          = 64;
    batch.iterations        = 8;
    batch.workers           = 1;
    batch.seed              = 73;
    batch.damage_capacity   = 20000;
    batch.mutation_capacity = 50000;
    const auto SINGLE       = RunBatch(MACRO.program, batch, LoadSpecialization(config));
    batch.workers           = 3;
    const auto MULTI        = RunBatch(MACRO.program, batch, LoadSpecialization(config));
    CHECK(SINGLE.checksum == MULTI.checksum);
    BatchControl control;
    const auto   GAINS = RunAttributeGainsControlled(MACRO.program, batch, control, config);
    CHECK(GAINS.primary.checksum == MULTI.checksum);
    for (int i = 0; std::cmp_less(i, AttributeGains.size()); ++i) {
        const auto CHANGED = ApplyAttributeGain(config, i);
        CHECK(CHANGED.target_events == config.target_events);
        const auto EXPECTED = RunBatch(MACRO.program, batch, LoadSpecialization(CHANGED));
        CHECK(GAINS.gains[i] && GAINS.gains[i]->checksum == EXPECTED.checksum);
    }
}

int main()
{
    ValidationAndPriority();
    SpecializationSchedule(tx::Config{}, tx::MacroOptions(), "sanchai");
    SpecializationSchedule(mw::Config{}, mw::MacroOptions(), "yu");
    std::cout
        << "Target timeline validation, order, wakeups, replay, reset, parity, gains and allocation tests passed\n";
}
