param(
    [string]$BuildDirectory = (Join-Path $PSScriptRoot '../../../build-legacy-compare'),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '../../../build-legacy-compare/phase-validation')
)
$ErrorActionPreference = 'Stop'
$BuildDirectory = (Resolve-Path -LiteralPath $BuildDirectory).Path
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
if (Test-Path -LiteralPath (Join-Path $OutputDirectory 'phases.json')) { throw 'Choose a fresh output directory.' }
$null = New-Item -ItemType Directory -Path $OutputDirectory -Force
$utf8 = New-Object Text.UTF8Encoding($false)
[Console]::OutputEncoding = $utf8
$OutputEncoding = $utf8
$summaries = @()
foreach ($initial in @(0, 10)) {
    foreach ($case in @('sanchai', 'cooldowns', 'rotation', 'rotation_dot')) {
        $macroName = $case -replace '_dot$', ''
        $dot = [int]($case -eq 'rotation_dot')
        for ($phase = 0; $phase -lt 16; ++$phase) {
            $samples = [ordered]@{}
            $events = @{}
            foreach ($engine in @('legacy', 'runtime')) {
                $trace = Join-Path $OutputDirectory "$case-q$initial-p$phase-$engine.csv"
                $raw = & (Join-Path $BuildDirectory "${engine}_bench.exe") --macro (Join-Path $PSScriptRoot "macros/$macroName.txt") --iterations 1 --seconds 300 --warmup 0 --crit 1000000 --precision 1 --phase $phase --initial-qidian $initial --dot $dot --trace $trace
                if ($LASTEXITCODE -ne 0) { throw "Validation failed: $engine / $case / $phase" }
                $samples[$engine] = $raw | ConvertFrom-Json
                $events[$engine] = @(Import-Csv -Encoding UTF8 $trace | Where-Object { [long]$_.damage -gt 0 } |
                    ForEach-Object {
                        $name = $_.name -replace '^Dot\u00b7', ''
                        if ($name -eq [string][char]0x7834) { $name += [char]0x62db }
                        '{0:D8},{1},{2},{3}' -f [int]$_.frame, $name, $_.damage, $_.outcome
                    } | Sort-Object)
            }
            $diff = @(Compare-Object $events.legacy $events.runtime)
            $summaries += [ordered]@{
                case = $case; initial_qidian = $initial; phase = $phase
                legacy_damage = $samples.legacy.damage_sum; runtime_damage = $samples.runtime.damage_sum
                legacy_hits = $events.legacy.Count; runtime_hits = $events.runtime.Count
                different_entries = $diff.Count; first_differences = @($diff | Select-Object -First 8)
            }
        }
        Write-Host "$case / initial qidian $initial : all 16 phases checked"
    }
}
[IO.File]::WriteAllText((Join-Path $OutputDirectory 'phases.json'), ($summaries | ConvertTo-Json -Depth 8), $utf8)
$failed = @($summaries | Where-Object { $_.different_entries -ne 0 -or $_.legacy_damage -ne $_.runtime_damage })
if ($failed.Count) { throw "$($failed.Count) of $($summaries.Count) timeline comparisons differ; see phases.json" }
Write-Host "All $($summaries.Count) complete fight timelines match."
