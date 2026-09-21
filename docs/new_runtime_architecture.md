# Runtime architecture and migration status

## Status

The new runtime is an independently buildable, tested foundation in
`src/core/runtime/`. It is not yet the production Tai Xu Jian Yi simulator.
`tests/runtime/fixture_rules.hpp` contains a deliberately synthetic ruleset.
No game coefficients or complete talent behavior are claimed for that fixture.

The first actual Tai Xu rules adapter is now implemented in
`src/class/tai_xu_jian_yi/runtime_rules.hpp`, with a standalone CLI and pinned
legacy arithmetic tests. It covers twelve castable skills, three-instance field
ownership and explosions, PoZhao/DieRen/ChiYing/RenJian DOT, JianRu and the
three YunZhongJian periodic effects, JingHuaYing's triggered preparation, plus
nineteen selected talent hooks.
See `tai_xu_runtime_migration.md` for exact
coverage, supported inputs, preserved quirks, debugger commands and limitations.
It does not yet replace full production simulation.

The earlier C API experiment used an always-successful action callback. That
experiment has been removed: `src/jx3dps.cpp` remains at the develop baseline.
Its existing debugger is still incomplete. Core debugger/engine integration is
verified using real state-changing test rules, not fake successful casts.
Connecting the C API and Qt UI awaits a verified game-rules adapter.

## Three stages, not just compile time versus runtime

| Stage | Work |
| --- | --- |
| C++ compilation | constexpr skill/Buff descriptors, formula coefficient tables, fixed slot identities, template rules, debug/batch specialization |
| Configuration initialization | Equipment/talent/recipe selection, derived constants, macro parsing, name resolution, source map, timer registration, buffer allocation |
| Each fight | Macro decisions, timers, live Buff/resource changes, causal random rolls, damage intents, reduction |

User-entered equipment, talents and macro text cannot become C++ constexpr
values without building a separate executable for every configuration. Resolve
these once per batch into immutable data, shared by every worker. Avoid a
combinatorial template instantiation for all talent/recipe combinations. Use
static specialization for class and execution mode; build selected effect lists
once during initialization.

`DescriptorTable` demonstrates constexpr name/id-to-slot mapping. Game ids such
as 32705 or 999999 never index the state arrays. The core uses separate contiguous
arrays for Buff stacks, expiry frames, tick frames, cooldowns and charges. The
current State is sized once using vectors. Fixed-size per-class std::array state
is a further specialization to measure after the real rules are migrated.

## Debugger and fast execution

```cpp
Simulation<TaiXuRules, false> batch(program, rules);
Simulation<TaiXuRules, true> interactive(program, rules);
```

These are distinct template instantiations. They share macro bytecode, condition
semantics, state transitions and game rules. Batch objects have no breakpoint
array, resumable debug cursor or snapshot strings. `if constexpr` removes the
instrumentation from that instantiation. The fast macro loop branches directly
through conditions; the debug executor exposes the same condition destinations
one step at a time. Differential tests compare both modes.

The bytecode has no std::string, source line, AST node, virtual function or JSON.
`MacroLine` is 12 bytes and `Condition` is 24 bytes in the measured Windows x64
build. `CompiledMacro::source` is a separate cold source map; compilation with
`keep_debug_info=false` omits it entirely. A normal release executable can contain
both execution modes. Native PDB/DWARF symbols are distinct from live simulator
instrumentation and do not require adding per-event checks to the batch loop.

The core macro debugger supports source lines beyond 4096, named-action
breakpoints, step into, step over and continue. A breakpoint pauses before line
execution and resuming skips that stop once. The simulation debugger supports
instruction breakpoints across repeated macro passes; a UI maps source lines
through the source map. Neither debugger manufactures a successful action.

Implemented macro behavior:

- Multiline /cast, /fcast, /scast and /sfcast instructions.
- OR groups of AND terms, matching old Regex.cpp parsing. Conditions compile to
  forward jumps, so failed AND groups and satisfied OR groups skip unused terms.
- /scast and /sfcast stop on failure and consume the successful step until reset.
- Buff/TBuff existence and stacks, absent Buffs, remaining times, cooldowns,
  skill charges, resources, previous skill, current player/target health ratios.
- Fractional-second comparisons retain the correct integer-frame boundary.
- Unknown names with configured resolvers and malformed expressions reject the
  whole program. No partial program can run. No invented qidian20 shorthand.

Force-cast interruption/readiness is a game rule, passed as Command to the
adapter; it is not implemented by merely ignoring all cooldowns. The core state
now has one explicit channel owner. A normal action waits while that owner is
active, while `/fcast` and `/sfcast` record an interruption, cancel the owner's
timer and take ownership. Start, interruption and completion are mutations, so
deferred reduction reconstructs the same channel history. The Tai Xu adapter
currently has no active channeling skill; this path is exercised by a synthetic
ruleset until a real channel is migrated. Macro switching, explicit target
selection and the legacy event script still require migration.

## Scheduling

`DeadlineQueue` is an indexed binary min-heap with one entry per timer. Re-arm and
cancel are O(log active timers); reading the next deadline is O(1). It does not
retain stale generations. All storage is reserved when registering timer handles.
Reset retains capacity. Same-frame order is explicit priority then insertion
order. The game adapter must encode and verify the legacy event priority.

State timers contain absolute frames. Reading remaining time computes
max(deadline - now, 0); advancing time never decrements every skill and Buff.
An empty queue is different from an event due now. The loop executes at frame 0,
dispatches due events before the next macro pass, and uses a [0, duration) horizon.
Same-frame action/event budgets detect a zero-time infinite loop.

Expiry events alone are insufficient. For example a Buff expiring at frame 160
can satisfy bufftime<9.5 seconds at frame 9 (16 fps). `NextMacroBoundary` computes
the first possible truth change for time comparisons and includes it in the next
wake time. Equality gets both entry and exit boundaries. The first implementation
scans compiled conditions after an unsuccessful macro pass; compiling dependency
lists per timer is a later optimization if profiles justify it.

## Deferred damage: causal boundary

Buff/TBuff stacks belong in arrays, but their updates stay live. In the old
TaiXuJianYi.cpp, TriggerDieRen uses rollResult to choose added stacks, and several
procs branch on DOUBLE. Moving those rolls and effects after the fight would
change the next macro decision. An effect depending on actual damage or target
HP must also be settled sufficiently early to preserve the decision sequence.

The live loop therefore resolves hit/crit/proc decisions that affect the future,
applies resources and Buff changes, and records the selected outcome. Pure
arithmetic and reporting may run later. The reducer must not draw a fresh random
number for a decision already used by the live simulation.

`DamageIntentLog` owns damage and mutation arrays and one monotonically ordered
sequence. At a single frame, a hit before Buff removal observes the Buff; a hit
after removal does not. A single owner prevents two unrelated sequence counters
from silently misordering same-frame effects. Reduction starts from the initial
state, replays mutations before each hit, and sets the exact hit frame. The final
live arrays are never used as a substitute for historical state.

The generic replay path covers Buff state, resources, cooldowns and casting.
Tai Xu now uses immutable, fight-local attribute versions instead of replaying
mutations for damage. Each DOT holds one version ID and each damage event holds
both snapshot and live version IDs. The rules select historical/live attributes
individually and retain immediate causal rolls. See [team buff versions](team_buff_versions.md).
There is one self and one current target; multi-target ownership and switching
remain migration requirements.

Buffers have explicit capacities. Exhaustion raises an error; it never silently
drops a hit or reallocates in the hot loop. A production boundary may grow buffers
and replay that iteration using its deterministic seed. State for reduction is
reused per worker. Do not retain all event logs for a million fights.

Deferred evaluation is not automatically faster: event writes, replay and cache
traffic can cost more than a short formula. Compare immediate arithmetic, replay
logs and compact damage snapshots on real skills before choosing the production
policy. The current log is a correctness baseline. Recording only damage-relevant
mutations and caching derived attributes when their inputs change should be
measured next. For cheap formulas, an immediate accumulation policy may win.

## Batches and reproducibility

太虚剑意和莫问共用 `Simulation<Rules>`、宏执行器、调度器、事件日志、批处理 worker 和伤害归约框架。心法适配器只负责装载本心法的技能、Buff、奇穴、秘籍、属性快照和公式；配置在边界通过 `std::variant` 选择适配器，进入模拟后不再复制公共执行流程。属性收益也使用同一入口：会心、无双、加速重新运行时间轴，其他属性复用已记录的伤害节点重算数值。

`RunBatch<Rules>` compiles no macro and constructs no simulator per iteration.
Each worker owns one reusable simulator, scheduler, initial state, reduction
scratch and bounded log. The program is shared read-only. Online Welford moments
replace a vector of all DPS results; each worker returns count, mean, variance,
min/max and a checksum. Worker exceptions are joined and propagated.

Seeds derive from (batch seed, iteration index), not thread identity. Worker
counts produce the same fight results. Floating-point moment reduction may differ
in final rounding across worker counts; exact integer event checksums agree.
To investigate a particular iteration, rerun its seed with the debug specialization.

Changing RNG algorithms/streams from the old engine is not an exact-seed parity
proof. For the game migration, first use a compatible draw stream or record and
replay the old rolls, then compare event/state sequences, including same-frame
order, before accepting distribution-only comparisons.

## Measurements (2026-09-18)

Environment: Windows x64, MSYS2 UCRT64 Clang 22.1.8, CMake Release (-O3 -DNDEBUG).
Single machine, one run of the full synthetic batch; timings are observations,
not a stable performance guarantee. The fixture has three simple skills and one
expiring Buff, and has no production game formulas/equipment/talent callbacks.

| Workload | Measured time |
| --- | --- |
| 1,000,000 macro passes, fast, 5 samples | 16.55-17.16 ms |
| 1,000,000 macro passes, debug stepping, 5 samples | 19.91-20.40 ms |
| 1,000,000 synthetic 4800-frame fights, 1 worker | 20.139 s |
| 1,000,000 synthetic 4800-frame fights, 4 workers | 5.864 s |

The full-fight checksums for both worker counts were 9702113828277989130.
These synthetic results do not establish a speedup over the old KeyFrame engine.
The later [legacy comparison](legacy_runtime_performance.md) uses shared Chinese
macros, verifies selected damage timelines, and documents remaining rule/output
differences. Neither benchmark isolates deferred computation as the sole cause
of the performance gain.

## Reproduce

The standalone option avoids the unfinished develop application and its empty
worktree submodules. It adds no third-party dependency.

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
cmake -S . -B build-runtime-verified -G Ninja `
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/clang++.exe `
  -DCMAKE_BUILD_TYPE=Release -DJX3DPS_RUNTIME_ONLY=ON
cmake --build build-runtime-verified
ctest --test-dir build-runtime-verified --output-on-failure
.\build-runtime-verified\tests\runtime\runtime_benchmark.exe 1000000 1000000
```

Checks cover rejected malformed macros, high game ids mapped to small arrays,
short-circuit truth tables, 20,000 fast/debug macro comparisons, 100 complete
seeded fight comparisons, breakpoints and resume, fractional/equality wakeups,
a million timer refreshes, randomized scheduler ordering, same-frame damage/Buff
ordering, immediate-versus-deferred damage parity, thread-count reproducibility,
capacity errors and zero tracked hot-loop heap allocations. Release and Debug
with libstdc++ assertions have passed. These checks do not validate full-game DPS.

## Remaining production migration

1. Extend the ba960f7 Tai Xu adapter beyond its twelve castable skills.
   Shared GCDs, charges, haste, preparation, field ownership and mixed live/DOT
   snapshot attributes, WanJian/RenJian explosions, XuanMen and LieYun are
   implemented; other skills and a real channel completion path remain.
2. Add target ownership, timeline commands, macro switching, DOT snapshots and
   all damage-affecting state. Preserve legacy event priorities and rounding.
3. Bind talent/recipe/equipment effects once. Keep causal critical/proc effects
   live; compare ordered records against the old engine using compatible rolls.
4. Profile the same 1/1k/100k-fight configurations. Compare scheduler strategies
   at real active-timer counts, immediate/replay/snapshot damage policies, and
   derive the next optimization from measured allocation/cache/branch costs.
5. Replace the C API and Qt placeholder interpreters with persistent sessions
   owning Simulation<ActualRules, true>. Build JSON only when execution pauses.
   Batch entry points use Simulation<ActualRules, false> via RunBatch.

Until these items are verified, keep the old worktree as the executable behavior
reference and keep this runtime isolated from production result reporting.
