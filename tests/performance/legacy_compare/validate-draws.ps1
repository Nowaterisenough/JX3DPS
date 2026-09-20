param(
    [string]$BuildDirectory = (Join-Path $PSScriptRoot '../../../build-legacy-compare'),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '../../../build-legacy-compare/draw-validation'),
    [int[]]$Phases = @(0..15),
    [int[]]$InitialQidian = @(0, 10),
    [int]$Repeats = 1,
    [switch]$ReplayExisting
)
$ErrorActionPreference = 'Stop'
$BuildDirectory = (Resolve-Path -LiteralPath $BuildDirectory).Path
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
$summaryPath = Join-Path $OutputDirectory 'draws-summary.json'
if ($Repeats -lt 1 -or @($Phases | Where-Object { $_ -lt 0 -or $_ -gt 15 }).Count -or
    @($InitialQidian | Where-Object { $_ -lt 0 -or $_ -gt 10 }).Count) { throw 'Invalid validation matrix.' }
if ((Test-Path -LiteralPath $OutputDirectory) -and !$ReplayExisting) { throw 'Choose a fresh output directory or use -ReplayExisting.' }
$null = New-Item -ItemType Directory -Path $OutputDirectory -Force
$utf8 = New-Object Text.UTF8Encoding($false)
[Console]::OutputEncoding = $utf8
$OutputEncoding = $utf8
$runtimeRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../../..')).Path
$sources = @(Get-ChildItem (Join-Path $runtimeRoot 'src/core/runtime') -File) +
    @(Get-Item (Join-Path $runtimeRoot 'src/core/runtime.hpp')) +
    @(Get-ChildItem (Join-Path $runtimeRoot 'src/class/tai_xu_jian_yi') -Filter 'runtime*.hpp') +
    @(Get-ChildItem $PSScriptRoot -File) + @(Get-ChildItem (Join-Path $PSScriptRoot 'macros') -File)
$hashes = [ordered]@{}
foreach ($file in $sources) { $hashes[$file.FullName.Substring($runtimeRoot.Length + 1)] = (Get-FileHash -LiteralPath $file.FullName).Hash }
$metadata = [ordered]@{
    created = (Get-Date -Format o); replay_existing = [bool]$ReplayExisting
    phases = $Phases; initial_qidian = $InitialQidian; repeats = $Repeats
    source_sha256 = $hashes
    binary_sha256 = @{
        legacy_record = (Get-FileHash (Join-Path $BuildDirectory 'legacy_record.exe')).Hash
        runtime_replay = (Get-FileHash (Join-Path $BuildDirectory 'runtime_replay.exe')).Hash
    }
    cmake_cache = @(Get-Content (Join-Path $BuildDirectory 'CMakeCache.txt') | Where-Object { $_ -match '^(CMAKE_(CXX_COMPILER|CXX_FLAGS_RELEASE|BUILD_TYPE)|LEGACY_ROOT):' })
}
$metadataName = if ($ReplayExisting) { 'replay-metadata.json' } else { 'record-metadata.json' }
[IO.File]::WriteAllText((Join-Path $OutputDirectory $metadataName), ($metadata | ConvertTo-Json -Depth 8), $utf8)
function Read-Events([string]$Path) {
    @(Import-Csv -Encoding UTF8 $Path | Where-Object { [long]$_.damage -gt 0 } | ForEach-Object {
        $name = $_.name -replace '^Dot\u00b7', ''
        $name = $name -replace '\u00b7\u795e\u5175$', ''
        if ($name -eq [string][char]0x7834) { $name += [char]0x62db }
        '{0:D8},{1},{2},{3}' -f [int]$_.frame, $name, $_.damage, $_.outcome
    } | Sort-Object)
}
$cases = @(
    @{ name = 'sanchai'; macro = 'sanchai'; dot = 0; talents = '' },
    @{ name = 'cooldowns'; macro = 'cooldowns'; dot = 0; talents = '' },
    @{ name = 'rotation'; macro = 'rotation'; dot = 0; talents = '' },
    @{ name = 'rotation_dot'; macro = 'rotation'; dot = 1; talents = '' },
    @{ name = 'core_talents'; macro = 'rotation'; dot = 1; talents = 'wuyi,ruoshui,fengshi,wuyu,xingu,tonggen' },
    @{ name = 'huanyue'; macro = 'rotation'; dot = 1; talents = 'wuyi,ruoshui,fengshi,wuyu,xingu,tonggen,huanyue' },
    @{ name = 'lieyun'; macro = 'rotation'; dot = 1; talents = 'wuyi,ruoshui,fengshi,wuyu,xingu,tonggen,lieyun,xuji' },
    @{ name = 'weapon_cw'; macro = 'rotation'; dot = 1; talents = ''; weapon = 1 },
    @{ name = 'weapon_cw_talents'; macro = 'rotation'; dot = 1; talents = 'wuyi,ruoshui,fengshi,wuyu,xingu,tonggen,lieyun,xuji'; weapon = 1 }
)
$summaries = @()
foreach ($case in $cases) {
    foreach ($initial in $InitialQidian) { foreach ($phase in $Phases) { for ($repeat = 0; $repeat -lt $Repeats; ++$repeat) {
        $stem = "$($case.name)-q$initial-p$phase-r$repeat"
        $tape = Join-Path $OutputDirectory "$stem-draws.json"
        $legacyTrace = Join-Path $OutputDirectory "$stem-legacy.csv"
        $runtimeTrace = Join-Path $OutputDirectory "$stem-runtime.csv"
        $legacyResult = Join-Path $OutputDirectory "$stem-legacy.json"
        $runtimeResult = Join-Path $OutputDirectory "$stem-runtime.json"
        $arguments = @('--macro', (Join-Path $PSScriptRoot "macros/$($case.macro).txt"), '--iterations', 1,
            '--seconds', 300, '--warmup', 0, '--crit', 20000, '--precision', 1, '--phase', $phase,
            '--initial-qidian', $initial, '--dot', $case.dot, '--draws', $tape)
        if ($case.talents) { $arguments += @('--talents', $case.talents) }
        if ($case.weapon) { $arguments += @('--weapon-cw', 1) }
        if (!$ReplayExisting) {
            $raw = & (Join-Path $BuildDirectory 'legacy_record.exe') @arguments --trace $legacyTrace
            if ($LASTEXITCODE -ne 0) { throw "Legacy recording failed: $stem" }
            [IO.File]::WriteAllText($legacyResult, $raw, $utf8)
        }
        $raw = & (Join-Path $BuildDirectory 'runtime_replay.exe') @arguments --trace $runtimeTrace
        if ($LASTEXITCODE -ne 0) { throw "Runtime replay failed: $stem (tape and legacy trace retained)" }
        [IO.File]::WriteAllText($runtimeResult, $raw, $utf8)
        $legacy = Get-Content -Raw -Encoding UTF8 $legacyResult | ConvertFrom-Json
        $runtime = $raw | ConvertFrom-Json
        foreach ($key in @('seconds', 'crit', 'dot', 'legacy_precision', 'phase', 'initial_qidian', 'weapon_cw')) {
            if ($legacy.$key -ne $runtime.$key) { throw "Mismatched recorded configuration: $stem / $key" }
        }
        if (($legacy.talents -join ',') -ne ($runtime.talents -join ',')) { throw "Mismatched recorded talents: $stem" }
        $oldEvents = Read-Events $legacyTrace
        $newEvents = Read-Events $runtimeTrace
        foreach ($entry in @(@{ path = $legacyTrace; total = $legacy.damage_sum }, @{ path = $runtimeTrace; total = $runtime.damage_sum })) {
            $traceTotal = (Import-Csv -Encoding UTF8 $entry.path | Measure-Object -Property damage -Sum).Sum
            if ($traceTotal -ne $entry.total) { throw "Incomplete damage trace: $($entry.path)" }
        }
        $diff = @(Compare-Object $oldEvents $newEvents)
        $summaries += [ordered]@{
            case = $case.name; initial_qidian = $initial; phase = $phase; repeat = $repeat
            legacy_damage = $legacy.damage_sum; runtime_damage = $runtime.damage_sum
            legacy_hits = $oldEvents.Count; runtime_hits = $newEvents.Count
            draws = $runtime.draws; unused_draws = $runtime.unused_draws
            different_entries = $diff.Count; first_differences = @($diff | Select-Object -First 8)
        }
    } } }
    Write-Host "$($case.name): recorded draws replayed and timelines compared"
}
[IO.File]::WriteAllText($summaryPath, ($summaries | ConvertTo-Json -Depth 8), $utf8)
$failed = @($summaries | Where-Object { $_.different_entries -ne 0 -or $_.legacy_damage -ne $_.runtime_damage -or $_.unused_draws -ne 0 })
if ($failed.Count) { throw "$($failed.Count) of $($summaries.Count) comparisons differ; see draws-summary.json" }
Write-Host "All $($summaries.Count) recorded/replayed fight timelines match."
