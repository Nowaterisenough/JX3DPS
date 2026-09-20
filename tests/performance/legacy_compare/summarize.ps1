param([Parameter(Mandatory = $true)][string]$Directory)
$ErrorActionPreference = 'Stop'
$Directory = (Resolve-Path -LiteralPath $Directory).Path
$rows = @(Get-Content -LiteralPath (Join-Path $Directory 'timings.jsonl') -Encoding UTF8 | ForEach-Object { $_ | ConvertFrom-Json })
function Median($values) {
    $sorted = @($values | Sort-Object)
    $middle = [int][Math]::Floor($sorted.Count / 2)
    if ($sorted.Count % 2) { return $sorted[$middle] }
    return ($sorted[$middle - 1] + $sorted[$middle]) / 2
}
$summary = @()
$table = @('| Case | Fights/run | Original old (s) | Old DPS-only diagnostic (s) | New (s) | Old/new | Diagnostic/new |',
    '| --- | ---: | ---: | ---: | ---: | ---: | ---: |')
foreach ($case in @($rows.case | Select-Object -Unique)) {
    $engines = [ordered]@{}
    foreach ($engine in @($rows.binary | Select-Object -Unique)) {
        $samples = @($rows | Where-Object { $_.case -eq $case -and $_.binary -eq $engine })
        if (-not $samples.Count) { continue }
        $durations = @($samples | ForEach-Object { [double]$_.result.elapsed_s })
        $engines[$engine] = [ordered]@{
            repeats = $samples.Count
            iterations = $samples[0].result.iterations
            median_s = (Median $durations)
            min_s = ($durations | Measure-Object -Minimum).Minimum
            max_s = ($durations | Measure-Object -Maximum).Maximum
            mean_dps = ($samples | ForEach-Object { $_.result.mean_dps } | Measure-Object -Average).Average
        }
    }
    $speedup = $engines.legacy.median_s / $engines.runtime.median_s
    $diagnosticSpeedup = $null
    $diagnosticTime = '-'
    $diagnosticRatio = '-'
    if ($engines.Contains('legacy_dps')) {
        $diagnosticSpeedup = $engines.legacy_dps.median_s / $engines.runtime.median_s
        $diagnosticTime = '{0:F4}' -f $engines.legacy_dps.median_s
        $diagnosticRatio = '{0:F2}x' -f $diagnosticSpeedup
    }
    $summary += [ordered]@{ case = $case; engines = $engines; old_over_new = $speedup; diagnostic_over_new = $diagnosticSpeedup }
    $table += '| {0} | {1} | {2:F4} | {3} | {4:F4} | {5:F2}x | {6} |' -f $case, $engines.runtime.iterations, $engines.legacy.median_s, $diagnosticTime, $engines.runtime.median_s, $speedup, $diagnosticRatio
}
$utf8 = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllText((Join-Path $Directory 'summary.json'), ($summary | ConvertTo-Json -Depth 8), $utf8)
[IO.File]::WriteAllLines((Join-Path $Directory 'summary.md'), $table, $utf8)
$table
