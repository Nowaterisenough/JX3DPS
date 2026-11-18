# Mo Wen runtime migration

Reference: `ba960f776dad2b30c8d43b529cd06d4fcaed5c2c`, level 120,
16 frames/second, one stationary target. This describes the migrated simulator
and CLI. Desktop still selects Tai Xu; Mo Wen desktop configuration, completion,
state panels and result labels are not connected yet.

## Implemented behavior

- Gong/BianGong preparation; Zhi/BianZhi three-segment channels, HaoQing's
  initial segment, force interruption and active YangChun during a cast.
- Independent style, active-skill and common cooldown groups; Yu/ShuYing
  charges and the shared LiuZhao Zhi/BianZhi pool; ShiXiang recharge reduction.
- Shang/Jue six-tick DOTs, refresh without resetting the pending tick, snapshot
  replacement, final tick before expiry; ShenBing's ten ticks and three stacks.
- QuFeng's exact-nine surplus trigger and overshoot reset; XianFeng marking
  and segment damage; LiuZhao accumulation and surplus consumption.
- Six independent shadows, their creation/expiry order, KeMeng, YunHan and
  snapshot damage; live haste when scheduling subsequent shadow ticks.
- GuYing snapshots remaining recharge/cooldown durations and restores them
  relative to the restore frame. Expiry starts its long cooldown.
- CanLian, MingJin, ZhengMing, ZhiZhi, ZhengLv/ZhiYin; the legacy MiaoYi
  critical-power remainder at its displayed stack cap.
- Orange weapon, set attributes and set skill bonus; shoes/wrist/belt enchants,
  water weapon stacks, YouRen and JingMiao. Trigger ordering distinguishes
  channels, ordinary direct damage, orange subdamage and utility skills.
- Gong/Shang/Zhi/Yu recipes, Chinese skill/buff names and English aliases.
  Bare `buff:name` means `buff:name>0`; `tbuff` addresses target arrays.

ChangQing and XiuQi are accepted as legacy flags: that revision names them but
does not implement their effects. Talent flags are independent, useful for
mechanics tests; this is not a validation of a legal in-game talent loadout.
Negative descriptor IDs are internal report labels, not game IDs.

## Performance architecture

Descriptors, skill slots and formulas are compile-time data. Preparing a
configuration computes haste profiles, recipe modifiers, critical chances and
integer-rounded damage tables once. Workers share the immutable result.

Fight state uses arrays of buff stacks/deadlines, charge state and six shadow
slots. Each timer has one heap entry; changing a deadline updates that entry.
The fight writes damage intents and state changes into reusable reserved logs.
Damage reduction runs after the fight. Ordinary hits use cached table values;
the legacy MiaoYi remainder and JingMiao critical power use prepared pre-critical
damage and a short final reduction, preserving legacy integer rounding.

The base cache has 256 profiles. Water adds 11 possible stack states and belt
adds three bonus states (absent, +10, +51 in 1024-based units), only when those
effects are enabled. The maximum is 8,448 profiles, approximately 7.5 MiB of
shared tables. It is not duplicated per fight or worker. Overflow bounds are
checked before preparing damage tables and against the requested log capacity.

`Simulation<Rules, false>` and `Simulation<Rules, true>` are separate template
instantiations. The batch loop has no breakpoint checks, debug cursor or source
map. Rules and macro transitions are shared, including the optional line-entry
hook needed for legacy force-cast interruption before evaluating conditions.

## CLI

Build from the repository root:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
cmake -S . -B build-runtime-verified -G Ninja -DJX3DPS_RUNTIME_ONLY=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/clang++.exe
cmake --build build-runtime-verified -j 4
ctest --test-dir build-runtime-verified --output-on-failure
```

Examples:

```powershell
$cli = '.\build-runtime-verified\tests\runtime\mo_wen_runtime.exe'
& $cli --iterations 10000 --workers 4 --seconds 300 --seed 42
& $cli --macro macro.txt --talents liuzhao,haoqing,kemeng,yunhan --equipment weapon_cw,set_skill
& $cli --seed 42 --replay 27 --trace fight.csv
& $cli --seed 42 --replay 27 --debug --trace stepped-fight.csv
```

The default seven-line rotation is for GaoShan style. Use a custom macro when
selecting `--style yangchun`. Debug commands are `break LINE`, `clear LINE`,
`continue`, `step`, `next`, `state`, `quit`. State output includes cast/channel
state, style, QuFeng, charges, buff/target-buff timers and each active shadow.
CSV includes the skill label, effect, outcome, snapshot, extra critical power,
reduced damage and random key. Trace/debug/replay require one fight and worker.

`--help` lists talent, equipment, team and per-skill recipe flags. Repeated
`--attribute name=integer` accepts `attack_base`, `attack_by_class`, `weapon`,
`crit`, `crit_power`, `overcome`, `strain`, `surplus`, `haste`, and bonuses
`attack_percent`, `overcome_percent`, `haste_percent`, `crit_basis_points`,
`crit_power_percent`, `damage_percent`, `strain_percent`, `pve_percent`,
`ignore_shield`. Percent fields use a base of 1024 except critical chance,
which uses 10000. These are resolved attributes: include class/gear bonuses
in the supplied values. The legacy comparison harness supplies Mo Wen's
intrinsic `pve_percent=61`; the general CLI default is an example with zero
bonus. The `weapon` field is retained for configuration compatibility; these
legacy magic formulas have no weapon-damage term.

Durations must be positive whole frames, at most 86400 seconds. Unknown flags,
invalid style, non-finite numbers, invalid recipe combinations and out-of-range
attributes are rejected. UTF-8 macro files with or without BOM are accepted.

## Verification

`tests/runtime/mo_wen_tests.cpp` uses always-on checks, including Release:

- 4,000 generated damage inputs, both hit outcomes, compared with the pinned
  legacy damage functions; recipe rounding and skill modifiers.
- Exact timer sequences, DOT refresh/expiry, channels, charge recovery,
  GuYing, shadows, talent/equipment triggers and snapshots after buff expiry.
- Fast/debug event and mutation equality across 16 seeds and 300-second fights
  with all migrated effects enabled; no hot-path allocations.
- One/four worker checksums and extrema; force-cast interruption before a
  false condition, including breakpoint/step observations.
- CLI argument errors and byte-identical CSV after stepping and resuming
  the same replayed fight.

The runtime Release and Debug suites each contain 11 tests. The separate
legacy harness leaves the reference checkout untouched. Its generated recording
copies observe shadow/surplus events and RNG draws without changing damage.
Run it after building the targets in `tests/performance/legacy_compare`:

```powershell
cmake --build build-legacy-compare --target mo_wen_legacy_bench mo_wen_legacy_record mo_wen_runtime_bench mo_wen_runtime_replay -j 4
./tests/performance/legacy_compare/validate_mo_wen.ps1
```

The archived run in `results/2026-09-19-mo-wen` verifies:

| 300-second scenario | Events | RNG draws | Validation |
| --- | ---: | ---: | --- |
| BianGong | 171 | 171 | Exact sequential random replay |
| BianZhi | 160 | 160 | Exact sequential random replay |
| Seven-line rotation | 1930 | 1929 | All-critical, same-frame sorted event equality |

The rotation's total is 24,501,951 in both engines, with equal per-skill totals.
This is not yet a claim of complete random timeline equivalence for Mo Wen.

## Measured performance

Windows, i7-12700, Clang 22.1.8, Release, one worker. Three sequential rounds,
1,000 fights per round, 300 seconds/fight, five warmups. Both engines use the
same archived rotation, all migrated talent flags, all-critical attributes,
and one GaoShan activation. Legacy frame precision is 1.

| Engine | Median seconds / 1,000 fights |
| --- | ---: |
| Pinned legacy | 7.1669095 |
| New runtime | 0.1001310 |

The observed ratio is **71.58x**. Every timed round has the identical aggregate
damage 24,501,951,000. Raw round timings and correctness traces are archived
beside `summary.json`. These are end-to-end simulation-path timings excluding
configuration/macro preparation. Legacy includes its attribute-gain calculations;
new runtime only reduces DPS. Do not interpret this as a pure scheduler speedup,
or as a measurement of million-fight runs, arbitrary macros or desktop overhead.

## Remaining integration and parity work

- Desktop currently remains Tai Xu-only. Mo Wen needs ruleset dispatch and its
  own configuration, completion, diagnostics and display mapping.
- Legacy shadows iterate an unordered container; new shadows have stable timer
  ordering. A sequential tape can assign different critical rolls to same-frame
  shadows. The all-critical comparison removes this difference for mechanics
  validation without pretending randomized event order is identical.
- Legacy also orders coincident DOT ticks and cast completions according to its
  evolving keyframe list. New timers use explicit priorities. The orange-weapon
  BianGong tape exposed such a collision at frame 1972 in one recorded run:
  the old DOT consumed its random roll before the skill's proc roll. Strict
  orange replay is not yet accepted; more same-frame effect-order checks remain.
- The legacy GaoShan active can retain its initial private cooldown pointer,
  whose value is not decremented. New active cooldowns are always scheduled.
  The comparison macro uses one sequential activation so the measured workloads
  stay equal; the CLI default permits subsequent activations.
- General event scripts, moving/multiple targets and full gear import are
  outside this single-target runtime configuration.
