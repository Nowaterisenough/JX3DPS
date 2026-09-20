# Natural qidian migration and repeat benchmark

The natural-qidian omission found in the first legacy comparison is fixed in
the shared Tai Xu rules used by desktop batches, single traces, CLI and debugger.
The old worktree remains unchanged.

With this behavior included, the same six-line macro completes 100,000 fights
in **6.8414 s** versus **115.2289 s** for the original old engine: **16.84x**
throughput. The controlled phase matrix now matches, replacing the historical
comparison that measured the incomplete resource rules.

## Implemented behavior

- Initial qidian is the pre-ClassFeature value: fight initialization adds one
  point, capped at ten, just as the old player's copy constructor does.
- A seeded phase in 0..15 starts a 16-frame resource timer. Phase zero ticks
  before the first macro evaluation. Skill use, Purple and full qidian do not
  restart the natural timer.
- The timer wakes macro evaluation even when no skill or Buff expires. A
  qidian-threshold macro can act at the exact resource-change frame.
- The phase uses an independent SplitMix stream; sampling it does not consume
  critical/proc rolls. Restart and worker-count reproducibility are retained.
- Changed resource values enter the existing mutation log for replay/debugging.
  Capped ticks retain their deadline but avoid redundant resource records.
  The phase generator is a compile-time policy, with no debugger branch added
  to the batch loop.

## Validation

The direct old/new matrix uses 300-second fights, old precision 1, critical
rating 1,000,000 and fixed weapon damage, varying:

| Dimension | Values |
| --- | --- |
| Macro | SanChai; three-line cooldowns; six-line rotation; six-line + DieRen/ShenMai |
| Initial qidian (before ClassFeature activation) | 0, 10 |
| Natural phase (frames) | All integers 0..15 |

**All 128 fight pairs match in total damage and every positive-damage event**
(frame, canonical skill name, damage, critical outcome). Duplicate hits are
preserved; order within one frame and zero-damage activation rows are ignored.
Legacy PoZhao / DOT display names are normalized. The old timeline omits DOT
hits, so a trace-only adapter observes the unchanged DieRen tick's statistics
delta; this does not run in measured batches.

At phase 7 and initial qidian 10, the six-line fight now produces **15,138,252**
damage in both engines, with **91 WuWo hits**, **44 PoZhao hits** and **252 SanHuan
damage hits**. Before this fix the new engine produced 14,150,958 damage and
84 WuWo hits in that controlled comparison.

Runtime checks also cover all phases at fight-end boundaries, initial +1,
phase-zero double gain before the macro, cap ten, exact threshold wakeup,
resource-record ordering before damage, restart, phase/critical independence,
debug/fast log equivalence, worker-count checksums and zero hot-loop allocations.
The runtime CTest suite passes 8/8; desktop builds and passes all 20 Qt checks.
The desktop test environment needs `QT_QPA_PLATFORM_PLUGIN_PATH` pointing to
the MSYS2 `share/qt6/plugins/platforms` directory, as documented in its build guide.

These results establish the listed configurations under controlled critical
outcomes. They do not establish full-game correctness, arbitrary proc replay,
or all talent/equipment combinations. Old default precision 16 can still delay
cooldown decisions; only precision-1 traces are used for exact event validation.

## Performance

Follow-up measurements use the same i7-12700, logical CPU 0, Clang 22.1.8,
C++20 Release `-O3 -DNDEBUG`, shared macros and resolved attributes as the first
comparison. Phase is randomized normally; traces and debugging are disabled.
Natural resource events are now included in the new engine's workload.

Actual 100,000-fight run (300 simulated seconds per fight, one repetition):

| Engine | Native time (s) | Mean DPS |
| --- | ---: | ---: |
| Original old | 115.2289 | 30,596.98 |
| Old DPS-only diagnostic | 63.2774 | 30,598.64 |
| Corrected new runtime | 6.8414 | 30,598.45 |

The new engine is 16.84x faster than the original and 9.25x faster than the
diagnostic. Normal-crit mean DPS differs from the original by approximately
0.0048%; different random generators prevent a bit-for-bit comparison here.

Three repetitions of 10,000 fights, median native time:

| Macro | Original old (s) | Old DPS-only diagnostic (s) | New (s) | Old/new | Diagnostic/new |
| --- | ---: | ---: | ---: | ---: | ---: |
| Six-line rotation | 11.6215 | 6.3897 | 0.6884 | 16.88x | 9.28x |
| Six-line + DieRen/ShenMai | 13.2724 | 6.8785 | 0.7432 | 17.86x | 9.26x |

Six-line new times span 0.6843..0.6898 s; old times span 11.6017..11.6282 s.
The old default precision is 16 for these throughput tests. The modified
DPS-only target still excludes attribute-gain calculations and associated
statistics, as described in the first report.

Mean DPS across those repetitions:

| Macro | Original old | New | Relative difference |
| --- | ---: | ---: | ---: |
| Six-line rotation | 30,604.64 | 30,602.77 | -0.0061% |
| Six-line + DieRen/ShenMai | 37,412.06 | 37,420.18 | +0.0217% |

Normal critical RNG sequences are different, and old repeats use independent
random initialization while new repeats reuse identical per-fight seeds. These
means are supporting evidence, not exact random-replay proofs. The all-critical
phase matrix provides the exact event checks.

Before the fix the six-line median was 0.4574 s, but its result lacked natural
qidian. The added resource ticks and macro wakeups increase new time by about
50.5%; the old/new ratio changes from 25.44x to 16.88x. Preserve the corrected
workload when assessing future optimizations. There are no per-tick heap
allocations, and capped resource ticks avoid redundant mutation records.

## Evidence and reproduction

- [Harness instructions](../tests/performance/legacy_compare/README.md).
- [128-pair comparison and raw traces](../tests/performance/legacy_compare/results/2026-09-19-qidian/phases-complete/phases.json).
- [Original/diagnostic/new smoke validation](../tests/performance/legacy_compare/results/2026-09-19-qidian/validation/validation.json).
- [Repeated timings](../tests/performance/legacy_compare/results/2026-09-19-qidian/repeated/timings.jsonl).
- [Repeated-run configuration and source hashes](../tests/performance/legacy_compare/results/2026-09-19-qidian/repeated/metadata.json).
- [Actual 100,000-fight results](../tests/performance/legacy_compare/results/2026-09-19-qidian/100k/timings.jsonl).
- [100,000-fight configuration and source hashes](../tests/performance/legacy_compare/results/2026-09-19-qidian/100k/metadata.json).
- [Pre-fix historical report](legacy_runtime_performance.md).

```powershell
$env:Path = 'C:\msys64\ucrt64\bin;' + $env:Path
powershell -NoProfile -ExecutionPolicy Bypass -File tests/performance/legacy_compare/validate-phases.ps1
```

Build the benchmark first following the harness README. The phase validation
fails on any event or total mismatch and retains diagnostics for investigation.
