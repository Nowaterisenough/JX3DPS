# Legacy / runtime benchmark

This standalone project compares the pinned KeyFrame engine at
`ba960f776dad2b30c8d43b529cd06d4fcaed5c2c` with the current, uncommitted Tai Xu
runtime. Both consume the same UTF-8 Chinese macro files. It does not build Qt.

## Build and run (PowerShell)

Run from the new-runtime worktree. Preserve a working Git executable before
adding the compiler directory to PATH; some MSYS2 installations contain several
different Git builds.

```powershell
$benchmarkGit = (Get-Command git).Source
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
cmake -S tests/performance/legacy_compare -B build-legacy-compare -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/clang++.exe `
  -DLEGACY_ROOT=C:/Users/NoWats/Workspace/Personal/JX3DPS-worktrees/ba960f7
cmake --build build-legacy-compare -j 4
powershell -NoProfile -ExecutionPolicy Bypass -File tests/performance/legacy_compare/validate.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File tests/performance/legacy_compare/run.ps1 `
  -GitExecutable $benchmarkGit
```

`run.ps1` writes a fresh timestamped result directory by default. Pass that
directory to `summarize.ps1 -Directory ...` to generate median/range summaries.
For a full 100,000-fight six-line run, add `-Iterations 100000 -Repeats 1 -Cases rotation`.
`-Affinity 1` (default) pins all children to logical CPU 0; `-Affinity 0` lets
Windows schedule freely. Each executable itself is single-threaded. Never run
compilation or another benchmark concurrently with measurement.

## Scope

- Each fight lasts 300 seconds / 4,800 frames, with zero configured input delay,
  one stationary level-124 target, shield 30,000, distance 4, full life.
- Resolved player stats: attack 10,000 + 2,500, weapon 2,000 (zero random range),
  critical rating 20,000, critical-power rating 15,000, overcome 25,000, strain
  10,000, surplus 3,000, zero haste, initial qidian 10. The new config explicitly
  includes the old Tai Xu intrinsic PvE multiplier of 184/1024.
- No recipes, equipment or team effects. `rotation_dot` enables DieRen and
  ShenMai in both engines; other cases enable no talents.
- Old `framePrecision` is 16 for normal cases, 1 for `cooldowns_precise`.
  Precision 16 can delay ordinary cooldown casts: the old
  `Skill::GetNextKeyFrame()` does not return their cooldown expiry. New timers
  wake at their actual expiry. Do not assume identical timelines at precision 16.
- Timed: old `Simulate()` (including clone, per-fight macro copies, KeyFrame,
  damage/stat accumulation, per-fight summary and destruction) plus `Sum()`;
  new `Start()` / `Run()` / `TotalDamage()` using reusable worker storage.
  Both aggregate per-fight totals. Macro compilation, prototype/config
  preparation, 20 warmup fights, process startup and JSON/CSV output are outside
  native timing. `process_wall_s` also records the enclosing process duration.
- This measures steady-state simulation. It excludes the old pool's submission
  of all tasks/futures and cross-fight detailed-stat merges, and the desktop UI.
  It is not a startup benchmark or the full application's end-to-end timing.
- No debugger/trace recording in timed runs. New uses `Simulation<Rules, false>`
  and `MacroOptions(false)`. Normal damage/state records remain active.
- Old uses `random_device`-seeded static `mt19937`; new uses per-fight SplitMix
  with `IterationSeed(0, i)`. They do not share a random sequence. Legacy's RNG
  has no seeding interface and is not safe to share between concurrent threads,
  so these results do not use its native thread pool. The separate draw
  validation executables described below do share recorded draws.
- Natural qidian is enabled in both engines: +1 at initialization, then +1 every
  16 frames with phase 0..15. New phase sampling uses a separate seeded stream.
  The `2026-09-19` results predate this fix; `2026-09-19-qidian` contains the
  follow-up. Neither suite establishes equivalence for every supported rule.

## Validation and diagnostic target

`validate.ps1` uses critical rating 1,000,000 (all hits critical), fixed weapon
damage, old precision 1 and phase 7. It compares every positive-damage event,
preserving duplicate hits and sorting within each frame, ignoring enum labels
and zero-damage activation records. SanChai, the three-line cooldown macro and
the six-line rotation must match in frame, canonical name, damage and outcome.
The legacy label `破` is normalized to `破招`; `Dot·` prefixes are removed.

`validate-phases.ps1` expands this to 128 fight pairs: all 16 phases, initial
qidian 0 and 10, and four workloads (SanChai, cooldowns, rotation, rotation with
DieRen/ShenMai). Both totals and event multisets must match. It retains the raw
CSV files and writes `phases.json`. Pass a fresh `-OutputDirectory` on each run.
`--phase -1` is the normal randomized benchmark path; `--phase 0..15` overrides
only the initial phase through a new-runtime policy / old Buff subclass.
The original old `Buff::Record` does not emit DOT timeline entries; in traced
DieRen validation fights only, a subclass observes each tick's stats delta to
emit those entries. It copies the initialized Buff's triggers and snapshots,
and calls the unchanged old tick implementation. Normal throughput runs do
not use this trace adapter. These are controlled all-critical checks; arbitrary
critical/proc sequences and other talent combinations remain outside their scope.

## Recorded draw validation

`legacy_record` and `runtime_replay` are separate validation binaries, never
throughput benchmarks. `legacy_record` calls the pinned RNG unchanged and saves
each floating-point critical sample and integer 1..1024 equipment-proc sample.
The fixed natural phase is excluded; constant-range draws have no stochastic
effect. Unknown random ranges fail explicitly. Generated `Buff.record.cpp`
adds timeline emission to `Buff::Record`, including natural and detonated DOT
hits, while retaining the original damage/stat operations. No pinned source is
edited. The older DieRen subclass adapter is disabled in this executable to
avoid duplicate events.

`runtime_replay` uses a template RNG policy. Floating-point samples round-trip
through JSON and are carried as their exact 64-bit representation, avoiding
53-bit quantization of the old distribution. Integer sample `v` maps to
`(v-1)/1024`, preserving the old `v <= numerator` condition. Critical/proc kinds,
exhaustion and unused draws are checked. The default fast RNG compiles without
recording, tape storage or a replay branch.

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tests/performance/legacy_compare/validate-draws.ps1 `
  -OutputDirectory build-legacy-compare/my-draws -Repeats 2
# Reuse exactly the saved old fights without drawing new random values:
powershell -NoProfile -ExecutionPolicy Bypass -File tests/performance/legacy_compare/validate-draws.ps1 `
  -OutputDirectory build-legacy-compare/my-draws -Repeats 2 -ReplayExisting
```

The default matrix has nine configurations, all 16 phases and initial points
0/10: 288 fight pairs per repetition, critical rating 20,000, precision 1,
300 seconds each. It includes core rotations, DieRen/ShenMai, WuYi/RuoShui/
FengShi/WuYu/XinGu/TongGen, HuanYue, LieYun/XuJi, and orange-weapon effects with
and without the core talents. These are mechanical interaction tests; the
combined flags are not a claim about a selectable in-game talent build.
Events retain duplicates and compare frame/name/damage/outcome after sorting
within a frame. `Dot` prefixes and PoZhao names are normalized as before;
`BaHuangGuiYuan-ShenBing` is grouped under BaHuang to match the new report's
parent-skill grouping. Each trace must sum to its engine's complete damage.
JSON configuration, CSV traces, tapes, checksums and summaries are retained.

The `2026-09-19-draw-replay` result bundle contains 576 passing fight pairs.
Extract `validation-fixtures.zip` into a fresh directory, then pass it to
`validate-draws.ps1 -Repeats 2 -ReplayExisting` to reproduce the comparison.
See `docs/random_draw_validation.md` for fixes and validation limits.
`run.ps1 -Cases weapon_cw` measures the normal uninstrumented engines with
DieRen/ShenMai and the orange weapon at precision 1.

## Original and diagnostic targets

`legacy_bench` retains original combat/stat logic. Old damage calculators
unconditionally compute attribute-gain variants, even with `gainSwitch` empty.
`legacy_dps_bench` is explicitly a **modified diagnostic**, not the original old
engine: generated copies of `Skill.cpp` / `Buff.cpp` restrict the eight attribute
loops to DEFAULT and remove duplicated gain entries. It still uses old allocation,
maps, cloning, virtual skills, KeyFrame and summary code. The speed difference
isolates the effect of removing this extra output workload (including its map
updates); it is not a CPU-profiler breakdown of arithmetic alone.

The pinned checkout is never edited. Build compatibility changes are confined to
the standalone target: required standard headers, DLL export defines, disabling
old fmt's incompatible `consteval` wrapper on Clang, and a generated event-parser
copy changing `std::stable_sort(list.begin(), list.end(), ...)` to `list.sort(...)`.
That parser is not called by the timed simulation path. `Regex::ParseToExprSkill`
still compiles the old macro outside timing.

Raw JSON includes the last fight's per-skill sample, totals across all fights,
compiler/CPU/flags, revisions, dirty-worktree status, binary hashes, and relevant
runtime/source/macro hashes. Sample skill hit counts include sub-hits and are not
universally the same as skill cast counts.
