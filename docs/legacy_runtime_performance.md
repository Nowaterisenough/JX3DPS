# Same-macro legacy / runtime comparison

This report preserves the measurements **before** natural qidian was migrated.
The gap is now fixed; use the [follow-up report](natural_qidian_validation.md)
for current correctness and performance. Historical raw results remain unchanged.

For the shared six-line macro, 100,000 300-second fights take **116.3342 s in
the original old engine and 4.5661 s in the new runtime (25.48x throughput)**.
Removing attribute-gain work in a modified old diagnostic still takes 63.8218 s
(13.98x the new time). These are measured single-core results, not extrapolations.
The new adapter still lacks natural qidian regeneration, so the representative
rotation is not yet behaviorally equivalent; see the timeline checks below.

Measured on 2026-09-19, Intel Core i7-12700 (12 cores / 20 logical processors),
Windows build 26200. Both executables use Clang 22.1.8, C++20, Release
`-O3 -DNDEBUG`, without LTO or `-march=native`. Single-threaded processes are pinned
to the same logical CPU 0 and run sequentially. The machine is not an isolated
benchmark host; timing ranges across repetitions are retained in the raw data.

Old source is `ba960f776dad2b30c8d43b529cd06d4fcaed5c2c`. New source is the current
`codex/new-runtime` worktree, based on `edaf66b` with uncommitted migration files;
the base commit alone does not identify the tested new implementation. Result
metadata records SHA-256 hashes of relevant source, macros, and executable files.

## Workloads and measurement boundary

The representative six-line macro is shared as a single input file:

```text
/cast [nobuff:紫气东来&qidian<6] 紫气东来
/cast [qidian>=8] 无我无剑
/cast 八荒归元
/cast 三环套月
/cast 三柴剑法
/cast [qidian>=1] 无我无剑
```

The cooldown control uses just BaHuang, SanHuan, SanChai in that order. Each fight
lasts 300 seconds. Player stats, stationary target, recipes and effects are
normalized across engines; `rotation_dot` adds DieRen and ShenMai talents. See
the [harness README](../tests/performance/legacy_compare/README.md) for every
attribute, compiler compatibility adjustment and exact timing boundary.

Native timing includes per-fight initialization, simulation, damage reduction
and total accumulation. Old per-fight cloning/copies, skill/Buff stats and
summary are included; new worker arrays/log buffers are reused. Both exclude
macro compilation, one-time config preparation, warmup, process launch and
JSON/CSV formatting. Debugging and detailed timeline output are disabled.
This is steady-state engine timing, not the desktop's end-to-end latency or
the old thread pool's submission/future/cross-fight merge cost.

## Measurements

Full 100,000-fight six-line run, one repetition per executable:

| Engine | Native simulation (s) | Whole process (s) | Fights/s |
| --- | ---: | ---: | ---: |
| Original old | 116.3342 | 116.4113 | 860 |
| Old DPS-only diagnostic | 63.8218 | 63.8543 | 1,567 |
| New | 4.5661 | 4.5827 | 21,900 |

The new native duration is 96.1% lower than the original old duration. The
10,000-fight repeated results below establish that this is consistent with the
shorter batches; the 100,000-fight run itself was not repeated three times.

Each row below uses 10,000 fights per run and reports the median of three runs.
The diagnostic column is a deliberately modified old engine, explained below.

| Shared macro / old precision | Original old (s) | Old DPS-only diagnostic (s) | New (s) | Old/new | Diagnostic/new |
| --- | ---: | ---: | ---: | ---: | ---: |
| Three-line cooldowns / 16 | 9.8266 | 5.1138 | 0.2708 | 36.28x | 18.88x |
| Three-line cooldowns / 1 | 29.6346 | 24.4982 | 0.2730 | 108.53x | 89.72x |
| Six-line rotation / 16 | 11.6359 | 6.3371 | 0.4574 | 25.44x | 13.85x |
| Six-line rotation + DieRen/ShenMai / 16 | 13.4137 | 6.8429 | 0.5194 | 25.83x | 13.18x |

The six-line original-old runs span 11.5737..11.6980 s; the new runs span
0.4572..0.4607 s. Default precision and exact-frame precision are separate rows:
the 108.53x control does not describe default old behavior.

Average DPS across the three normal-critical runs demonstrates why timing alone
must not be used as proof of migration correctness:

| Workload | Original old DPS | New DPS | Interpretation |
| --- | ---: | ---: | --- |
| Three-line cooldowns / 16 | 27,219.81 | 28,250.15 | Default old polling changes casts |
| Three-line cooldowns / 1 | 28,242.25 | 28,250.15 | Different RNG; all-critical event control matches |
| Six-line rotation / 16 | 30,600.41 | 28,591.88 | New is about 6.56% lower; known resource-rule gap |
| Six-line + DieRen/ShenMai / 16 | 37,417.70 | 37,496.39 | Similar mean does not prove matching rotations |

New runs repeat their deterministic seeds, so the three new damage totals are
identical within each case; their repetition measures timing variability, not
three independent random samples. Legacy repeats use independently initialized
random generators.

## Correctness checks and limits on the speedup claim

The validation run forces every hit to be critical (rating 1,000,000), uses
fixed weapon damage, and sets old precision to 1 frame. It compares every
positive-damage event by frame, name, damage and outcome, preserving duplicate
hits and ignoring order within the same frame. This is separate from the
normal-crit throughput runs.

| Controlled 300-second fight | Old | New | Timeline result |
| --- | ---: | ---: | --- |
| SanChai only: hits / damage | 219 / 1,741,269 | 219 / 1,741,269 | Every event matches |
| Three-line cooldowns: hits / damage | 681 / 14,628,705 | 681 / 14,628,705 | Every event matches |
| Six-line rotation: damage | 15,138,252 | 14,150,958 | Different rotation/resources |
| Six-line rotation: WuWo hits | 91 | 84 | Different |
| Six-line rotation: SanHuan damage hits | 252 | 273 | Three hits per cast |
| Six-line rotation: PoZhao hits | 44 | 38 | Different |

The diagnostic old executable also passes the two exact timeline controls.
One validation fight is not a general parity proof. Normal throughput runs
use different random generators: old static `mt19937` seeded by `random_device`,
new per-fight SplitMix with `IterationSeed(0, i)`. Normal-crit totals cannot be
expected to match bit-for-bit, and the same legacy run cannot be seeded exactly
through its public interface. Old native multi-threaded RNG use has a data race;
this report does not extrapolate these single-core ratios to that thread pool.

Two concrete behavioral differences matter:

1. Old `ClassFeature` adds one qidian every 16 frames, beginning at a random
   phase in 0..15. Its player clone activates this effect at fight start. The
   new `Rules::Setup` / `Reset` have no corresponding timer. Even at precision
   1 and all-critical hits, the six-line trace already differs in WuWo damage
   at frame 72: old 71,494 versus new 58,873. This is a missing migration rule,
   not random critical noise. The six-line speed ratio includes this work and
   rotation difference.
2. At its default `framePrecision = 16`, the old scheduler polls ordinary skill
   cooldowns instead of always waking on their exact expiry. `Skill::GetNextKeyFrame`
   returns preparation completion for ordinary skills. This can delay SanHuan
   and change the cooldown macro's DPS. The new deadline queue fires at exact
   expiry. Use the precision-1 control for output parity; do not present its
   ratio as the default old user's latency.

The benchmark also aligns the old class's intrinsic PvE damage bonus of
184/1024 with the new resolved-attribute config. Omitting it produces an
approximately 18% per-hit difference even when the damage formula matches.
That alignment is made in the harness, without changing application behavior.

## What the diagnostic explains

Old `Skill.cpp` / `Buff.cpp` calculate and store several attribute-gain damage
variants for each hit even when `Options.gainSwitch` is empty. The latter only
controls additional simulation batches. Comparing this entire output workload
against new total-DPS reduction overstates any gain attributed solely to layout.

The separately labeled `legacy_dps_bench` uses generated copies limiting those
loops to DEFAULT and removing duplicated gain entries. It retains the old
rotation, allocation, virtual skill dispatch, maps, KeyFrame scheduling and
summary machinery. This is a modified diagnostic, not the pinned original.
Its timing difference measures the combined cost of extra gain calculations
and associated statistics; it is not an arithmetic-only profiler measurement.

The remaining improvement is consistent with the architectural differences:
reused arrays and log buffers instead of cloning a graph every fight; resolved
macro slots and static rule dispatch; actual timer deadlines instead of repeated
list scans and empty polling frames; precomputed damage profiles and sequential
final reduction instead of nested hash-map aggregation. These changes have not
been individually ablated, so this report assigns no percentage to each one.

For the six-line case, removing gain work reduces old median time from 11.6359 s
to 6.3371 s (about 45.5%), while the new engine still runs 13.85x faster than that
diagnostic. Thus extra gain work is significant but does not explain the entire
measured difference. The remaining comparison still includes the rule gap and
different statistics/initialization implementations.

Next priorities are to migrate natural qidian regeneration and establish
controlled-roll parity for the real rotation; make gain/stat collection an
explicit output policy; then profile the remaining scheduler/recording costs
under a matched output workload. The current evidence supports a large speed
improvement, but does not establish that the new engine is a complete behavioral
replacement for the old one.

## Reproduction and evidence

- [Build, run and validation instructions](../tests/performance/legacy_compare/README.md).
- [Three-repeat native timings](../tests/performance/legacy_compare/results/2026-09-19/repeated/timings.jsonl).
- [Generated median/range summary](../tests/performance/legacy_compare/results/2026-09-19/repeated/summary.json).
- [Full 100,000-fight native timings](../tests/performance/legacy_compare/results/2026-09-19/100k/timings.jsonl).
- [Full 100,000-fight metadata](../tests/performance/legacy_compare/results/2026-09-19/100k/metadata.json).
- [Repeated-run metadata and source/binary hashes](../tests/performance/legacy_compare/results/2026-09-19/repeated/metadata.json).
- [Validation summary](../tests/performance/legacy_compare/results/2026-09-19/validation/validation.json).
- [Old six-line damage trace](../tests/performance/legacy_compare/results/2026-09-19/validation/legacy-rotation.csv).
- [New six-line damage trace](../tests/performance/legacy_compare/results/2026-09-19/validation/runtime-rotation.csv).

The pinned old worktree remains clean. The benchmark adds an independent CMake
project; it does not modify the production runtime or desktop code.
