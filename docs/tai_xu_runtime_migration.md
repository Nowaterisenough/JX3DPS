# Tai Xu runtime migration: core loop, fields and explosions

Behavior source: `ba960f776dad2b30c8d43b529cd06d4fcaed5c2c` (level 120).
Implementation: `src/class/tai_xu_jian_yi/runtime_data.hpp` and `runtime_rules.hpp`.
Entry point: `apps/cli/tai_xu_runtime.cpp`. This adapter is independent of Qt and
the existing C API. Its output identifies itself as `ba960f7-core-subset`.
The Qt adapter now uses the same rules for batches, single traces and debugger
sessions. See `docs/desktop_workbench.md` for its controls and validation.

The natural-qidian gap found by the [first benchmark](legacy_runtime_performance.md)
is fixed. `ClassFeature` adds one point during fight initialization, then one
every 16 frames from a seeded 0..15-frame initial phase, capped at ten. Phase zero
also ticks before the first macro evaluation. Phase sampling is independent of
critical/proc rolls, and the timer persists while capped so its phase is retained.
Resource changes enter the same replay log used by the debugger. See the
[follow-up comparison](natural_qidian_validation.md): 128 controlled complete
fight pairs match the old engine, including the six-line macro and DieRen/ShenMai.
These configurations do not establish full-ruleset parity.

## Supported rules

| Rule | Coverage |
| --- | --- |
| Natural qidian / ClassFeature | Initial +1, seeded phase 0..15, +1 every 16 frames, cap ten, same-frame macro wakeup |
| WuWoWuJian (alias `wuwu`) | 1..10 points, consumes points, independent PoZhao roll for >=6 points, shared GCD |
| BaHuangGuiYuan (`bahuang`) | Two independent rolls, target-life coefficient level, 192-frame non-hasted CD |
| SanHuanTaoYue (`sanhuan`) | Three independent hits per cast, 32-frame hasted CD, shared GCD |
| SanChaiJianFa (`sanchai`) | Independent 22-frame hasted cooldown, leaves last_skill unchanged |
| ZiQiDongLai (`ziqi`) | Two charges, 1200-frame serial recharge, adds 160 frames of buff duration per cast |
| SuiXingChen (`suixing`) | Hasted 16-frame preparation, CD starts at completion, creates a 384-frame field with 48-frame pulses |
| ShengTaiJi (`shengtaiji`) | Preparation recipes, completion-time CD and point gain, creates a 384-frame field |
| TunRiYue (`tunriyue`) | Hasted preparation, creates a 384-frame field with 20-frame target-debuff refreshes |
| WanJianGuiZong (`wanjian`) | Range 8, 192-frame non-hasted CD, one point per target, XuJi/TongGen hooks |
| RenJianHeYi (`renjian`) | Range 6, consumes fields in creation order, one independent hit per field, coupled 320-frame CD |
| RenJian variant (`renjian_suixing`) | Coupled CD with RenJian; stops consuming at the second SuiXing field |
| YunZhongJian (`yunzhongjian`) | Optional per-field recipe effects; each consumed SuiXing/ShengTaiJi/TunRiYue field creates a 128-frame, 16-frame periodic effect |
| JingHuaYing (`jinghua`) | JingHua talent proc from WanJian against TunRiYue; 128-frame window, immediate hit and a 48-frame preparation fragment |
| WanXiangGuiYuan (`wanxiang` target Buff) | `weapon_cw` equipment effect: three stacks, 48-frame interval, ten snapshot ticks, stack-scaled damage |
| JianMing (`set_attribute` equipment effect) | Eligible direct skills refresh a 96-frame Buff; later snapshots gain +400 crit basis points and +41 crit power |
| PoZhao | Internal triggered damage, independently rolled; cannot be cast in macros |
| DieRen (`dieren` Buff) | Five stacks, seven with LieYun; refresh snapshot, preserve next tick, eight remaining ticks, last tick before expiry |
| ChiYing (`chiying` Buff) | ChangSheng grants up to three stacks, 240-frame expiry, one triggered hit/stack per eligible cast |
| RenJian DOT (`renjian` target Buff) | Recipe proc, four hasted 48-frame ticks; every application resets next tick and snapshot |
| JianRu (`jianru` talent) | Purple-gated 98-frame self effect; WanJian activates 16-frame direct ticks and BaHuang can trigger the stronger immediate hit |

Chinese skill/Buff names are accepted. English aliases are for CLI tests and
examples. External IDs label records; dense slots index the actual arrays.
Purple adds base-attack percent, crit chance and crit power. Every 16 frames it
adds two points, including the expiry tick. RuoShui changes charges to three and
recharge to 960 frames. FengShi lasts 80 frames and adds 307/1024 to WuWo damage.

Migrated talent hooks: DieRen, WuYi, ShenMai, RuoShui, FengShi, WuYu, HuanYue,
QieYu, XinGu, TongGen, FuYin, GuChang, QiSheng, ChangSheng, XuanMen, LieYun,
XuJi, JianRu and JingHuaYing. These flags expose supported source-level effects;
this is not yet a validator for legal twelve-slot talent loadouts. The macro can
use only the twelve migrated castable skills. Selecting a supported talent does
not imply that its effects on an unmigrated skill are implemented.

Recipes: WuWo supports crit 2/3/4, damage 3/4/5 and +1 point; BaHuang supports
damage 3/4/5, +1 point and -16-frame CD; SanHuan supports crit 2/3/4, damage 3/4/5
and -8-frame CD. More than four recipes on one skill and unsupported combinations
of recipe type and skill are rejected. Unknown talent/CLI flags and skill names
also fail explicitly.

ShengTaiJi supports `prepare1`, `prepare2`, `prepare3` (each subtracts two base
prepare frames before haste) and `cooldown` (-32 frames after field creation).
Its range recipes are not yet supported. XuanMen changes its base CD to 400
frames in the pinned source, or 368 with the cooldown recipe. Unknown flags are rejected.

The CLI accepts `--equipment weapon_cw,set_attribute`. This binds the pinned
orange-weapon effect during preparation. Equipping it adds 51/1024 effect damage
to WuWo and BaHuang, including BaHuang's weapon subeffect. Each eligible hit
rolls a 31/1024 proc chance, even during the 480-frame internal cooldown; that
cooldown is also active at fight initialization. The 96-frame proc window clears
BaHuang's cooldown and lets it recast on the global cooldown. Each BaHuang cast
in the window adds one target WanXiang stack;
the target snapshot is refreshed, the next tick is preserved, and the 10-tick
window ends nine intervals after that next tick. Every BaHuang cast while the
weapon is equipped also rolls a separate 307/1024 chance for its weapon hit.
`set_attribute` adds the pinned JianMing set effect. Eligible direct skills
refresh its 96-frame Buff after their damage is recorded; while active it adds
400 crit basis points and 41 crit-power points to later snapshots.
RenJian's two variants share recipes: `cooldown` (-80 frames), `range1/2/3`
(+1 each), `damage40` (+409/1024), `damage60` (+614/1024) and `dot`.
The damage recipes apply to direct explosions, not the triggered DOT. The three
`yunzhong_*` recipes attach YunZhongJian to their matching consumed field type.
YunZhongJian keeps its Buff and timer live for 128 frames, records a hit every
16 frames, and activates JianRu on its first tick. Its damage is recorded only
when the target is within 6 chi; an out-of-range target keeps the Buff and its
future timer state but produces no YunZhongJian hit.

## Prepared fields and related Buffs

Fields use three fixed reusable instance slots, each with its own absolute expiry,
next pulse and creation order. The three field types share this capacity; duplicate
types may coexist. Replacing a field cancels its timer before reusing its slot.
Creation and pulses require no allocation or map lookup.

The GCD begins when preparation starts. While preparing, all casts, including
SanChai, ZiQi and `/fcast`, are blocked, matching old `IsCast`. On completion the
skill gains two points, records last_skill, starts its non-hasted skill CD and
creates the field. Its zero-damage action record is timestamped at completion.
Resetting a fight cancels pending preparation and clears field ownership. The
shared runtime now tracks one channel owner and persists channel start,
interruption and completion in the event log. Normal actions wait for that
owner; `/fcast` and `/sfcast` interrupt it and take ownership. The migrated Tai
Xu skills in this slice use preparation rather than channels, so this path is
ready for later channel skills without changing their current timeline.

- SuiXing immediately grants a 64-frame self Buff, refreshed every 48 frames by
  each SuiXing field. It gives +500 crit basis points and +102/1024 crit power.
- FuYin changes the field lifetime to
  576 frames and the self Buff bonuses to +1000 and +205 respectively.
- GuChang adds 614/1024 base shield ignore while that self Buff exists.
- QiSheng is refreshed for 48 frames by SuiXing/ShengTaiJi field creation and
  pulses, and adds 102/1024 base attack. The old auxiliary SuiXing-QiSheng timer
  is folded into the owning field's pulse, with the same period and lifetime.
- TunRiYue applies a 20-frame target Buff immediately and every 20 frames. Like
  the old TODO-marked code, the current single target receives it regardless of
  distance. Field skills themselves have unlimited cast range.
- ChangSheng adds one ChiYing stack on field completion. WuWo consumes one after
  its PoZhao/WuYu processing, BaHuang after QieYu, and SanHuan after FengShi.
  SanChai does not consume it. ChiYing has its own independent critical roll.

Field retirement does not immediately remove the already-applied Buff. For
example, a SuiXing completed at frame 16 expires at 400, pulses once at 400, and
its final self Buff remains until 464. A TunRiYue field's last pulse is at 396,
so its target Buff remains until 416 even though the field expires at 400.

Field aliases `field_suixing`, `field_shengtaiji`, `field_tunriyue` expose active
instance counts and the next instance expiry via macro Buff queries. Their
Chinese names use the `field` prefix from the original source. Negative
descriptor IDs are explicitly internal labels, not invented external game IDs.

Two ordering choices are explicit migration boundaries. Eviction uses the oldest
instance globally, including duplicate types; the old code chooses the oldest
type from a list but erases an unspecified `unordered_map::begin()` instance of
that type. Likewise, old field-duration queries return an unspecified instance,
whereas this adapter reports the nearest expiry. Same-frame processing orders
preparation completion, field pulses/retirement, DOT events, then aura expiry;
pulses happen before their own field retirement and refreshes before aura expiry.
Full old-engine replay is still needed to assess differences at such collisions.

## Field explosions, XuanMen and LieYun

RenJian consumes the oldest field instance, publishes the new field count,
rolls a hit, triggers ShenMai, refreshes its recipe DOT, then records direct
damage. It repeats this for each field, consumes one ChiYing stack afterward,
then adds one XuanMen stack per consumed field. Both RenJian skills enter the
same cooldown even with no fields. The old macro compiler's range gate applies
before casting, so an out-of-range attempt does not consume fields or cooldowns.

The `renjian_suixing` variant is named after the pinned skill
`RenJianHeYiSuiXingChen`. After consuming one SuiXing, encountering another at
the front stops consumption and leaves that instance and every later field
intact. This preserves the old loop's repeated `continue` without a pop.
Removing fields leaves their already-applied auras to expire normally. Hidden
LieYun is independent of the three normal field slots and is never exploded.

XuanMen lasts 640 frames, caps at three stacks, and adds 204/1024 base overcome
and 300 crit basis points per stack. Its bonuses are applied after all RenJian
explosions and ChiYing, including after the recipe DOT's snapshot. Casting with
no fields still refreshes XuanMen, even at zero stacks. The macro contract is
`buff:name` equivalent to `buff:name>0`, and `nobuff:name` equivalent to zero
stacks. A zero-stack XuanMen is therefore absent for these conditions even when
its recorded duration remains positive; `bufftime:name` queries that duration.

XuanMen requires explicit split overcome attributes. The final value is
`base * (1024 + base_percent + 204 * stacks) / 1024 + by_class`.
Multiplying an already-resolved overcome value would incorrectly boost the
class component and change integer rounding. The old `overcome` input remains
supported when XuanMen is absent and no split is supplied.

LieYun raises DieRen's stack cap to seven. At four or more stacks, adding
DieRen creates or refreshes a hidden target effect for 384 frames, pulsing every
eight frames. Creation immediately grants a 16-frame aura within distance 15;
each pulse refreshes it. The aura grants +154/1024 crit power. The initial aura
is applied before DieRen captures its new snapshot. Dropping below four on an
update removes the hidden effect, while the granted aura lingers to expiry.
Natural DieRen expiry and QieYu call the LieYun hook before erasing DieRen in
the old code, so they can refresh hidden LieYun for another 384 frames. HuanYue
erases first and therefore removes it. Both orders have regression coverage.

JianRu is added by BaHuang only while Purple is active. It remains inactive
until WanJian calls its activation hook. Once active, it records the normal
JianRu hit every 16 frames until the 98-frame duration expires. A BaHuang hit
while it is already active records the separate stronger `sub=1` hit and keeps
the periodic effect alive. Its two coefficients are cached independently.

When a YunZhongJian tick activates JianRu, it uses the same activation and
periodic timer as WanJian. The YunZhongJian effect itself remains independent,
so its 16-frame tick can coexist with JianRu's 16-frame tick and both events
are retained in the intent log in scheduler order.

JingHuaYing is available only with the `jinghua` talent. A WanJian hit while
the target's TunRiYue aura is active refreshes a 128-frame JingHua Buff and
opens the `jinghua` cast. The cast records its immediate effect, then blocks
the macro for a 48-frame preparation before recording the `sub=1` fragment.
The Buff is cleared by its own expiry; casting does not create a separate
cooldown timer, so only a later qualifying WanJian can reopen it.

XuJi subtracts 32 frames from WanJian's CD, adds one DieRen stack after its hit,
and multiplies DieRen's already-truncated per-stack channel coefficient by 1.2,
truncating again. The pinned WanJian constructor adds both 1000 and 102 to crit
power, giving +1102/1024; it does not add 1000 crit basis points. These source
behaviors, including XuanMen's 400-frame ShengTaiJi CD, are preserved explicitly.

## Preserved behavior and boundaries

- WuYi changes WuWo's crit stats only. It does not modify PoZhao or DOT snapshots.
- ShenMai adds two points on any hit in the pinned implementation, despite its
  comment saying critical hits. This migration preserves the implementation.
- FengShi's old clear wrapper adds one DieRen stack even if FengShi was absent.
- WuYu reduces BaHuang's own cooldown by `(consumed_points + 1) * 8` when DieRen
  already exists, before adding the current WuWo's stacks. It does not reduce GCD.
- HuanYue adds four points, rolls three PoZhao hits, then triggers up to three
  immediate DieRen ticks while shortening its duration.
- QieYu computes `int((remaining_frames / 48) * haste)` with integer division
  first. Detonation multiplies the initial damage before mitigation/rounding,
  uses one crit roll, then removes DieRen.
- Hasted DieRen expiry is `next_tick + int(48 * haste * 7)`, whereas each interval
  is `int(48 * haste)`. These differ from multiplying a truncated interval by 7.
- BaHuang's old life-level expression reaches index 10 at life <=1%. This adapter
  accepts life in [0.01, 1] and clamps the resulting index to 9, avoiding old OOB
  behavior. Target death and evolving target health are not modeled.
- Simulation horizon is `[0, duration)`: events at the exact endpoint are excluded.
  Buff ticks precede their own expiry; independent Buffs use a stable priority.
  General cross-Buff ordering of the old container-based scheduler still requires
  full timeline replay when more interacting effects are migrated.

## Preparation, recording and reduction

Compilation fixes dense slots, descriptors, all coefficient variants and rule
dispatch. User configuration is resolved once per batch into an immutable
`Prepared` object shared by workers. The September 2026 team-buff migration
replaces the exhaustive damage/probability tables with 120 prepared formulas,
skill modifiers and coefficients. There is no table dimension for combinations
of team buffs. Class aura bits remain an internal change detector and a legacy
trace label; they are not an attribute version ID.

During a fight, macros, cooldowns, charge recovery, stacks and causal rolls stay
live. When a hit or DOT application needs attributes, the worker publishes an
immutable version if a damage-relevant aura has changed. Consecutive captures
with unchanged attributes reuse the version. Versions contain resolved numbers,
critical chance and rounded intermediate factors, not copies of buff arrays.
Refreshing a DOT stores one 32-bit version ID; each tick records this ID and
the live version ID. Attack, crit power, strain and crit chance use the DOT
version. Overcome, weapon, shield ignore, PvE bonus and target mitigation or
vulnerability use the live version. This also handles one buff affecting both
snapshot and live attributes (JiLei attack/overcome, XuanMen crit/overcome).
YouRen's legacy crit bonus remains live at the roll point.

A trace's `snapshot` column is retained as the combined class-profile label.
The appended `snapshot_version` and `live_version` columns are the actual arena
indices. They are local to one fight and are not timestamps. Multiple versions
can exist within the same frame. Final reduction reads both versions and applies
the remaining integer rounding stages in the original order, without buff replay.
Rules with complete damage snapshots use the one-argument `Reduce(hit)` overload
and skip historical state replay. Generic rules can still use `Reduce(hit,state)`.
Mutation records are currently retained in both modes for diagnostics; eliminating
those writes in batch mode is a further optimization to measure, not a claimed
property of this port.

Each worker reserves 4096 attribute versions by default and reuses the storage
on Start; no global cache grows between fights and no lock is needed per hit.
Exhaustion throws explicitly. The CLI's `--version-capacity` or Config's
`attribute_version_capacity` can raise this bound for unusually long schedules.
The target remains single and stationary. See
[team buff design and measurements](team_buff_versions.md) for the supported
catalog, ordering, compatibility restrictions and GCC/Clang results. Mo Wen
retains its existing class implementation; the generic version arena is reusable.

`Simulation<Rules,false>` and `Simulation<Rules,true>` share these rules but are
separate template instantiations. Batch execution has no breakpoint checks or
debug cursor; its macro is compiled without source text. The same executable can
contain both modes. Names, std::function resolvers and config parsing are outside
the fight loop. Tests observe zero heap allocations during repeated Start/Run/
TotalDamage calls after construction.

## Running and debugging

Build with `JX3DPS_RUNTIME_ONLY=ON` as described in `new_runtime_architecture.md`.
The CLI target is `build-runtime-verified/tests/runtime/tai_xu_runtime.exe`.

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
./build-runtime-verified/tests/runtime/tai_xu_runtime.exe --help
./build-runtime-verified/tests/runtime/tai_xu_runtime.exe `
  --iterations 100000 --workers 4 --seconds 300 --seed 42 `
  --talents dieren,wuyi,shenmai,fengshi,wuyu,xingu
./build-runtime-verified/tests/runtime/tai_xu_runtime.exe `
  --macro my_macro.txt --debug --seed 42 --replay 123 `
  --talents dieren,wuyi --trace build-runtime-verified/fight.csv
./build-runtime-verified/tests/runtime/tai_xu_runtime.exe `
  --macro tests/runtime/macros/tai_xu_fields.txt --debug --seed 42 `
  --talents dieren,wuyi,fuyin,guchang,qisheng,changsheng `
  --recipes-shengtaiji prepare1,prepare2,prepare3,cooldown
./build-runtime-verified/tests/runtime/tai_xu_runtime.exe `
  --macro tests/runtime/macros/tai_xu_explosions.txt --iterations 100000 --workers 4 `
  --seconds 300 --seed 42 `
  --talents dieren,wuyi,shenmai,fengshi,wuyu,xingu,fuyin,guchang,qisheng,changsheng,xuanmen,lieyun,xuji `
  --attribute overcome_base=20000 --attribute overcome_by_class=5000 `
  --recipes-renjian cooldown,dot,damage40,damage60
```

Debugger commands: `break 2`, `continue`, `step` (one macro phase/condition),
`next` (finish the current instruction), `state`, `clear 2`, `quit`. A breakpoint
stops before evaluating its macro line. Resume skips that stop once. Source line
numbers refer to the supplied UTF-8 macro file. The CLI also accepts a UTF-8 BOM.
State output includes the currently preparing skill, remaining preparation,
active field instances and their next pulses/expiry, relevant auras and cooldowns.
The field example expects `qisheng` to be enabled; otherwise its `nobuff:qisheng`
line deliberately keeps trying ShengTaiJi whenever it is ready.

Seeds are derived from `(base_seed, iteration_index)`, independent of worker
count. `--seed 42 --replay 123` reproduces batch iteration 123. `--trace` without
`--replay` uses iteration 0. This is a new deterministic SplitMix draw stream;
it does not reproduce the old RNG from the same numerical seed. The RNG policy
is a template parameter; the separate legacy draw replay executable adds no
runtime instrumentation to batch simulation.

Defaults are example resolved combat attributes, not imported equipment:

| Attribute | Default |
| --- | --- |
| attack_base / attack_by_class / weapon | 10000 / 2500 / 2000 |
| crit / crit_power / overcome | 20000 / 15000 / 25000 |
| strain / surplus / haste | 10000 / 3000 / 0 |
| All integer percentage bonuses | 0 |
| Target level / shield / life / distance | 124 / 30000 / 1.0 / 4.0 |
| Initial points / fixed delay | 10 / 0 frames |
| Talents / recipes | None |

Use repeated `--attribute name=integer` to replace an attribute. In addition to
the values above, names are `attack_percent`, `haste_percent`, `crit_basis_points`,
`crit_power_percent`, `damage_percent`, `strain_percent`, `pve_percent`, and
`ignore_shield`. Percent values use 1024, crit basis points use 10000. Target life
and distance use decimals. Base attack must remain separate from attack granted
by the class because Purple modifies only the base component. Combat ratings
are bounded to 1,000,000 and configuration is checked for total-damage overflow
against the reserved log capacity. Capacity exhaustion reports an error.

For XuanMen, supply `overcome_base` (mandatory), `overcome_by_class` (default 0)
and `overcome_percent` (default 0). An explicit nonnegative base selects the split
formula in place of the resolved `overcome` input. Without a base, split bonuses
and XuanMen are rejected instead of estimating the components.

## Validation and remaining migration

Historical core-slice performance before adding fields, observed on 2026-09-18,
Windows x64, Clang 22.1.8 Release, default
resolved attributes, included macro, 4800-frame fights, seed 42 and the six flags
in the batch example:

| Workload | Workers | Elapsed |
| --- | --- | --- |
| 100,000 fights | 1 | 4.730 s |
| 100,000 fights | 4 | 1.581 s |
| 1,000,000 fights | 1 | 47.116 s |
| 1,000,000 fights | 4 | 19.714 s |

These are individual measurements on one machine, not speed guarantees. The
measurements predate the optional `weapon_cw` and `set_attribute` paths and do
not include their extra proc, aura and target-tick traffic.
Million-fight checksums match exactly across
worker counts: `13375359734383792542`. The workload uses the core macro without
optional equipment, team, or field effects. No speedup over the old engine is claimed.
Interactive debug time also includes time spent waiting for commands and should
not be compared with batch timing.

After the field migration and cached-profile update, the same 100k core workload
took 5.137 s on one worker with its previous checksum unchanged. With
`tests/runtime/macros/tai_xu_fields.txt` and the original six flags plus
`fuyin,guchang,qisheng,changsheng`, 100k 4800-frame fights took 10.357 s on one
worker and 4.210 s on four workers. The field-workload checksum was
`12694479673689373511` for both worker counts. These individual measurements
include field pulses and preparation, and do not establish an old-engine speedup.
The larger ruleset still has zero tracked hot allocations. Million-fight field
timing has not been measured.

The test oracle compiles byte-for-byte old damage headers under a separate
namespace. Skill parameter expressions are extracted from the original
constructors, independently of the new coefficient table. Source hashes and a
regeneration script live in `tests/runtime/legacy_reference` and
`tests/runtime/capture_legacy_reference.py`.

Checks include 3,686,520 old-arithmetic comparisons (random attributes, all cache
entries and extreme valid values), source-based timing/proc expectations, DOT
refresh and snapshot boundaries, GCD macro thresholds, charge recovery, the
unchanged last_skill for SanChai, Chinese name resolution, unsupported-option
errors, 300 seeded fast/debug event comparisons (100 core, 100 with fields and
100 with explosions),
preparation completion/force-cast blocking, FIFO field ownership/slot reuse,
lingering auras, live GuChang and XuanMen with historical DOT stats, ChiYing ordering,
RenJian prefix consumption/coupled cooldowns/four-tick recipe DOT, YunZhongJian
128-frame lifetime/range behavior, JingHuaYing's TunRiYue trigger and preparation,
the orange-weapon proc window and ten-tick WanXiangGuiYuan refresh, JianMing
set-effect refresh and expiry,
XuanMen's
post-explosion application and equivalent implicit/explicit stack predicates,
LieYun threshold snapshots,
seven-stack cap and the differing natural/QieYu/HuanYue clear orders,
per-event old-formula fight
totals, zero tracked hot allocations and worker-count checksum equivalence.
Release and Debug with libstdc++ assertions are the validation configurations.

Recorded/replayed legacy random draws now match 576 complete 300-second fight
pairs across nine core/talent/orange-weapon configurations, every natural phase
and initial qidian 0/10. This comparison found and fixed the orange weapon's
initial internal cooldown, roll consumption during cooldown, WuWo damage bonus,
WanXiang activation/refresh boundaries and simultaneous DOT ordering. Details,
reproducible draw tapes and performance results are in
`docs/random_draw_validation.md`.

These establish agreement for the listed slice, not complete legacy combat
equivalence. Full-field, team, recipe and remaining talent combinations still
need whole-engine random replay before replacing production result reporting.

Next work: migrate a real channel skill and its completion effects; expand remaining talent/Buff
rules; migrate additional equipment, team effects, timeline changes and multi-target
semantics; restore stat-gain reporting; connect verified sessions to the C API.
The old C API production entry points remain untouched.
