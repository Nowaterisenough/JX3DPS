# Legacy random draw replay and orange-weapon corrections

On 2026-09-19, 576 paired 300-second fights match the pinned legacy engine at
`ba960f776dad2b30c8d43b529cd06d4fcaed5c2c` in total damage and every positive
damage event (frame, canonical parent skill, damage and critical outcome).
The new engine consumes the exact old critical/proc draws, with no leftover or
exhausted tape. The pinned legacy worktree remains unchanged.

## Corrections established by replay

The original core and talent cases matched with mixed hits/criticals. Extending
the comparison to the orange weapon exposed these migration errors:

- Its first internal cooldown starts at 480 frames, matching the old Buff
  constructor. Successful procs then start another 480-frame cooldown.
- Eligible skills still draw the 31/1024 proc while the internal cooldown is
  active; the cooldown gates applying the effect. Skipping those draws changed
  all subsequent seeded outcomes. This is relevant only when the weapon is enabled.
- The weapon adds 51/1024 effect damage to WuWo as well as BaHuang. Both bonuses
  are included when preparing the damage table, with no added per-hit arithmetic.
- BaHuang adds WanXiang only during the active six-second weapon window, including
  a window just triggered by that cast. Owning the weapon alone is insufficient.
- WanXiang refresh preserves its next tick. Expiry is that next tick plus nine
  intervals, giving exactly ten remaining ticks, rather than extending from now.
- WanXiang precedes DieRen when their ticks coincide in these legacy rotations.
  Swapping their draws produced different critical hits even when draw counts and
  event frames matched. Its deadline priority now preserves that order.

The first saved orange-weapon sample failed at draw 44 before correction.
After correcting proc timing and draw consumption it consumed all 1,280 draws,
but damage still differed: legacy 12,073,145 versus new 11,858,877. Fixing the
missing WuWo bonus and simultaneous DOT ordering made this same saved fight
match exactly. Fresh recordings and the larger matrix then passed.

## Validation method and scope

The standalone harness builds `legacy_record` and `runtime_replay` in addition
to the normal three benchmark executables. Recording calls the unchanged old
random distributions, observes their return values, and adds a generated
timeline hook to the original Buff stat recorder. Runtime replay is a template
policy. Default `SplitMixRolls` retains its direct integer-to-unit conversion;
it has no tape, trace branches, virtual calls or recorder allocations.

Floating-point samples round-trip exactly through JSON and a 64-bit bit cast.
For integer proc draws, `(value-1)/1024 < numerator/1024` preserves the old
`value <= numerator` result. The consumer checks draw kind and tape length.
Unknown nonconstant random ranges are rejected. Fixed initial natural-qidian
phase uses the existing independent phase adapter.

All tests use fixed weapon damage, one stationary target, critical rating
20,000, old precision 1, natural phases 0..15, initial qidian 0/10, and two
independently recorded old fights per combination. Nine configurations are
covered, giving 64 fight pairs per row:

| Configuration | Positive damage events | Random draws |
| --- | ---: | ---: |
| SanChai | 14,016 | 14,016 |
| Cooldown macro | 43,584 | 43,584 |
| Six-line rotation | 41,984 | 41,984 |
| Rotation + DieRen/ShenMai | 50,176 | 50,176 |
| Core talent interactions | 48,544 | 48,544 |
| Core + HuanYue | 64,096 | 64,096 |
| Core + LieYun/XuJi | 48,544 | 48,544 |
| Orange weapon + DieRen/ShenMai | 53,493 | 82,355 |
| Orange weapon + core/LieYun/XuJi | 52,300 | 82,266 |

Each engine's trace must sum to its reported damage. Comparison preserves
duplicates and ignores only intra-frame ordering and zero-damage activation
records. Old DOT/PoZhao labels are normalized, and BaHuang's ShenBing subeffect
is grouped with BaHuang, as in the new report. Same-frame ordering is exercised
indirectly through exact draw replay, not claimed identical for all event types.
Combined talents exercise interactions and need not be a selectable game build.

Regression tests cover exact 480-frame initial cooldown, draw consumption during
cooldown, no WanXiang without a proc, active-window damage, preserved refresh
phase, the final tick/end-exclusive boundary, weapon damage cache arithmetic,
fast/debug event equality, restart, worker determinism and zero hot allocations.
Release and Debug runtime suites pass 8/8 each; the desktop app and its 20 Qt
checks build and pass.

Artifacts are in `tests/performance/legacy_compare/results/2026-09-19-draw-replay`.
`draws-summary.json` is directly readable; `validation-fixtures.zip` contains
both engines' JSON, complete CSV traces, original random tapes and source/binary
metadata. Extract it into a fresh directory and run
`validate-draws.ps1 -OutputDirectory <directory> -Repeats 2 -ReplayExisting`.

## Performance after correction

Each row uses the same macro and configuration in all three engines, 10,000
fights of 300 simulated seconds per run, three repetitions, one thread pinned
to logical CPU 0 on an i7-12700, Clang 22.1.8 Release. Table entries are median
native times. Normal RNGs are used; recorder/replayer/debugging are disabled.

| Workload | Old frame precision | Original old (s) | Old DPS-only diagnostic (s) | New (s) | Old/new |
| --- | ---: | ---: | ---: | ---: | ---: |
| Six-line macro | 16 | 13.0236 | 7.0011 | 0.7430 | 17.53x |
| Same macro + DieRen/ShenMai + orange weapon | 1 | 44.7549 | 36.5594 | 0.9693 | 46.17x |

The orange-weapon row uses precision 1 to match the validated event timing.
That setting causes more polling in the old engine and contributes substantially
to its higher cost; 46.17x is specific to this workload and precision. The
diagnostic removes old attribute-gain arithmetic/statistics but retains its
KeyFrame algorithm. The new deadline scheduler processes actual deadlines.
Normal-macro results use the historical production-like precision 16; this is
not a claim of identical legacy decisions at arbitrary precision settings.

New timings span 0.7354..0.7635 s for the normal macro and 0.9409..0.9829 s for
the weapon macro. Across the three runs, mean DPS is 30,595.34 old versus
30,602.77 new for the normal macro, and 40,798.43 versus 40,809.33 for the weapon
macro. Throughput runs use independent random generators; exact equivalence is
established by the separate recorded-draw tests above. These are 10,000-fight
measurements, not new 100,000- or million-fight measurements.

Raw timing JSONL, compiler/CPU settings, source/binary hashes and summaries are
in `tests/performance/legacy_compare/results/2026-09-19-draw-performance`.
Macro/config preparation, warmup and output are excluded from native timing;
Start/Simulate, damage reduction and per-fight accumulation are included, as in
the earlier comparison. Desktop rendering and old pool submission are excluded.

## Remaining coverage

Full field rotations, team effects, arbitrary recipes, target timelines,
multiple targets, remaining talents and stat-gain reporting are outside this
matrix. The runtime is still a migrated subset; production C API replacement
is not part of this change.
