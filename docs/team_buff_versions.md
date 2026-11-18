# Tai Xu team buffs and observed attribute versions

The Tai Xu runtime now records immutable resolved attributes on demand, replacing
the exhaustive damage and critical-chance combination tables. This migration is
based on `ba960f776dad2b30c8d43b529cd06d4fcaed5c2c`, level 120. The desktop equipment
data may describe a newer game version; these combat rules are explicitly the
pinned legacy rules. Mo Wen retains its existing class implementation.

## Representation and cost

`Prepared` contains 120 skill formulas, fixed coefficients and skill modifiers.
Each worker owns an attribute arena. A capture publishes a version only when
the observed class aura bits or aggregate team attributes have changed since
the preceding capture. No theoretical combinations are enumerated. Returning
to an earlier state may append another version; this is a chronological arena,
not global interning. Several changes within a frame can produce several IDs.

- A version occupies 104 bytes on both tested Windows x64 compilers. It contains
  resolved combat values, critical chance and rounded intermediate factors.
- Each DOT owns one 32-bit version ID. Refreshing it changes that ID and preserves
  the existing class-specific tick scheduling rules.
- Each damage event carries frozen and live version IDs. `DamageIntent` is now
  64 bytes, previously 56. The old `snapshot` field remains a class-profile label
  for trace compatibility and is not the arena index.
- Each worker reserves 4096 versions (416 KiB) by default, resets their count
  between fights, and reuses the allocation. Exhaustion raises an error. Increase
  `Config::attribute_version_capacity` / CLI `--version-capacity` for long runs.
- A 1024-entry, 24 KiB cache stores only observed damage results, keyed by fight
  generation, frozen ID, live ID, effect and outcome. Full key comparisons make
  collisions harmless. It is local to the worker and never grows. Generation
  changes invalidate it in constant time; wraparound clears the entries.
- The shared `Prepared` object shrinks from 522,536 to 8,496 bytes. Arena and
  cache memory are per worker, so this is not a claim that total process memory
  always falls. Damage/mutation logs remain the larger reserved buffers.

The number of versions follows states actually captured in a fight, not the
Cartesian product of all buff flags and stack counts. An unused intermediate
state needs no version. Extending the team catalog adds dense slots and attribute
modifiers, without adding another table dimension.

## Snapshot and live inputs

| Input | DOT application/refresh version | Version at the damage event |
| --- | --- | --- |
| Attack, crit chance, crit power, strain | Yes | No |
| Skill-specific damage/crit modifiers | Prepared static constants | Prepared static constants |
| Overcome and weapon damage | No | Yes |
| Shield, shield ignore, vulnerability | No | Yes |
| PvE bonus | No | Yes (currently constant) |
| YouRen's additional crit chance | No | Live at the causal roll, preserving the pinned implementation |

For example, JiLei's attack increase survives in an existing DOT after JiLei
expires, but its overcome increase does not. A later JiLei can improve that
DOT's live overcome without replacing its old attack. The reducer performs each
integer rounding stage in legacy order; it does not multiply all buffs into one
floating-point coefficient. Crit/proc feedback still executes during simulation.

## Supported schedule

The legacy source entry points relevant to the discussed aliases are
`Buff3rd::HaoLingSanJun` and `Buff3rd::NongMei`. The catalog keeps the names from
that source, rather than substituting unverified current-season coefficients.

Percent values below use the game's 1024 denominator; crit basis points use
10000. The English aliases and Chinese source names are accepted by the parser.

| Alias / legacy class | Default duration | Max initial stacks | Physical damage effect |
| --- | ---: | ---: | --- |
| `han_ru_lei` / HanRuLei | 1800 s | 1 | Base attack percent +51 |
| `po_feng` / PoFeng | 14 s | 1 | Target shield -1150 |
| `jing_feng` / JingFeng | 14 s | 1 | Target shield -1397 |
| `jie_huo` / JieHuo | 15 s | 1 | Target vulnerability +20 |
| `chao_sheng` / ChaoSheng | 8 s | 24 | Strain +400 per stack |
| `sheng_yu_ming_xin` / ShengYuMingXin | 8 s | 24 | Strain +600 per stack |
| `zhen_fen` / ZhenFen | 10 s | 125 | Base overcome +60 per stack |
| `han_xiao_qian_jun` / HanXiaoQianJun | 15 s | 1 | Base overcome percent +256 |
| `shu_kuang` / ShuKuang | 12 s | 1 | Base attack percent +307 |
| `han_chang_lin_li` / HanChangLinLi | 5 s | 1 | Crit basis points +1000 |
| `ji_lei` / JiLei | 6 s | 1 | Base attack and overcome percent +205 |
| `nong_mei` / NongMei | 6 s | 1 | Base overcome +700, shield ignore +205 |
| `she_shen_hong_fa` / SheShenHongFa | 20 s | 36 | Strain +400 per stack |
| `hao_ling_san_jun` / HaoLingSanJun | 60 s fixed | 48 | Strain +440 per stack, halve stacks after 30 s |

Applications replace stacks and refresh the duration. Zero stacks remove the
whole effect. HaoLingSanJun instead ignores applications while active and uses
its fixed 30/60-second phases. The pinned implementation subtracts inconsistent
amounts for odd initial stacks; those inputs are rejected rather than silently
introducing a negative residual strain bonus. Even initial stacks 2..48 and
removal are supported. XiuQi's primary-stat conversion and other team effects
are outside this migration.

External applications have priority over skill completion and DOT ticks at a
shared frame. External expiry/second-drum transitions precede DOT ticks and macro
dispatch. A refresh at the expiry frame re-arms the existing timer; no stale
expiry can remove the refreshed effect. Existing class DOT final ticks keep
their original tick-before-expiry ordering. Legacy unordered-container edge
orders are not a universal specification; tests cover the declared stable order.

The desktop adds an external-buff table under Tai Xu equipment options with
enable, start time, duration and stacks. Existing configuration snapshots gain
disabled defaults for the new fields; persistence, import and invalid values
are tested. Multiple applications of one buff can be expressed via the C++
configuration or repeatable CLI schedules:

```powershell
tai_xu_runtime --attribute overcome_base=25000 --team-buffs 'ji_lei:0:96:1;nong_mei:48:96:1;zhen_fen:64:160:30;hao_ling_san_jun:0:0:48'
```

The format is `name:start_frame:duration_frames:stacks`, at 16 frames/second;
duration 0 selects the legacy default. Desktop times are shown in seconds.
Percentage overcome effects require an explicit base/class overcome split.

## Validation

Both GCC 16.2.0 and Clang 22.1.8 pass the 13 runtime/CLI tests, including about
7.37 million independent legacy arithmetic comparisons, mode parity, repeated
fight reset, worker determinism and zero allocations in Start/Run/TotalDamage.
The 34 Qt desktop tests pass, including team configuration migration and input
validation. On this machine the offscreen Qt plugin path must be supplied when
running tests outside the deployed GUI environment.

For each compiler, 288 original random-draw replay fights cover baseline,
talents and weapon procs. Another 48 team fights compare every positive damage
event, frame, outcome, total and RNG consumption against the unchanged old
implementation: 37,696 damage events per compiler. They cover single effects,
overlap, replacement, refresh, removal, both drum phases and same-frame events.
The harness uses old `Buff3rd` implementations and expression events, without
editing the old checkout.

The GCC run exposed zero-effect entries whose default members were not preserved
by its constexpr array initialization. Explicitly filling with `Formula{}` and
asserting the zero-effect descriptors fixes this and prevents reintroducing it.

## Performance protocol

Measurements use an i7-12700, one fixed logical CPU (affinity mask 1), Release
`-O3 -DNDEBUG`, no LTO or native-CPU flags, 20 warmup fights and sequential runs.
Each sample contains 5000 fights of 300 seconds; tables report the median of
three repetitions. Per-fight reset, simulation, logging and final reduction are
included. Macro/config preparation and UI/legacy thread-pool dispatch are outside
the main timer. Preparation is measured separately. Raw metadata includes source
and binary SHA-256 hashes, compiler identity, flags and all timings.

Three implementations are distinguished: original `ba960f7`, the pre-change
runtime at `47a453a` with exhaustive tables, and this attribute-version runtime.
The pre-change runtime has no scheduled team-buff equivalent. The optional old
DPS-only diagnostic disables the old engine's additional attribute-gain damage
variants; it is not represented as the unmodified old engine.

The default old frame precision of 16 can delay macro evaluation. Precision 1
is used for the weapon/DOT/team trace-matching scenarios. These speedup ratios
should not be interpreted as one universal ratio for every rotation or workload.
Normal timed runs use each engine's native random generator; exact equivalence
is checked separately through recorded draws.

### GCC 16.2.0

| Case | Original `ba960f7` | Attribute-version runtime | Old/new |
| --- | ---: | ---: | ---: |
| rotation | 6.2822 s | 0.3036 s | 20.69x |
| weapon + DOT | 21.9153 s | 0.4261 s | 51.43x |
| team + DOT | 34.0328 s | 0.4815 s | 70.68x |
| team + weapon + DOT | 36.4604 s | 0.5378 s | 67.79x |

### Clang 22.1.8

| Case | Original `ba960f7` | Attribute-version runtime | Old/new |
| --- | ---: | ---: | ---: |
| rotation | 6.4816 s | 0.3793 s | 17.09x |
| weapon + DOT | 21.2943 s | 0.4754 s | 44.79x |
| team + DOT | 33.7265 s | 0.5622 s | 59.99x |
| team + weapon + DOT | 37.4339 s | 0.6275 s | 59.65x |

Each row is the median of three 5000-fight samples. The corresponding pre-change
runtime medians were 0.2874/0.3693 s with GCC and 0.4779/0.5344 s with Clang
for rotation/weapon+DOT. The new version is faster in the ordinary rotation
case after the bounded cache, while weapon+DOT can be slightly slower because
it now resolves mixed frozen/live attributes instead of indexing one exhaustive
table. The team cases have no pre-change equivalent; they are compared directly
with the old engine and exact replay validation.

Preparation is measured separately over 1000 configurations: old exhaustive
`Prepared` construction is 0.86 s (Clang median) / 0.70 s (GCC median),
while the new formula preparation is 0.0045 s / 0.0019 s. `Prepared` shrinks
from 522,536 to 8,496 bytes. Results are retained in
`.cache/team-buffs-20260920/clang-measured-final/`,
`.cache/team-buffs-20260920/gcc-measured-final/`,
`.cache/team-buffs-20260920/clang-draws/`, and
`.cache/team-buffs-20260920/gcc-draws/`; the raw team replay matrix is in the
matching `*-team-final/` directories.
