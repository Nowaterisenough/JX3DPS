param(
    [string]$BuildDirectory = (Join-Path $PSScriptRoot '../../../build-legacy-compare'),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot '../../../build-legacy-compare/validation')
)
$ErrorActionPreference = 'Stop'
$BuildDirectory = (Resolve-Path -LiteralPath $BuildDirectory).Path
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
$null = New-Item -ItemType Directory -Path $OutputDirectory -Force
$utf8 = New-Object Text.UTF8Encoding($false)
[Console]::OutputEncoding = $utf8
$OutputEncoding = $utf8
$summaries = @()
foreach ($macroName in @('sanchai', 'cooldowns', 'rotation')) {
    $samples = [ordered]@{}
    $events = @{}
    foreach ($engine in @('legacy', 'legacy_dps', 'runtime')) {
        $trace = Join-Path $OutputDirectory "$engine-$macroName.csv"
        $raw = & (Join-Path $BuildDirectory "${engine}_bench.exe") --macro (Join-Path $PSScriptRoot "macros/$macroName.txt") --iterations 1 --seconds 300 --warmup 5 --crit 1000000 --precision 1 --phase 7 --trace $trace
        if ($LASTEXITCODE -ne 0) { throw "Validation failed: $engine / $macroName" }
        $sample = $raw | ConvertFrom-Json
        $samples[$engine] = $sample
        # Normalize emission order within a frame; compare every damage event,
        # including duplicate hits, without relying on the engines' enum labels.
        $events[$engine] = @(Import-Csv -Encoding UTF8 $trace | Where-Object { [long]$_.damage -gt 0 } |
            ForEach-Object {
                $name = $_.name -replace '^Dot\u00b7', ''
                if ($name -eq [string][char]0x7834) { $name += [char]0x62db }
                '{0:D8},{1},{2},{3}' -f [int]$_.frame, $name, $_.damage, $_.outcome
            } | Sort-Object)
    }
    $differences = [ordered]@{}
    foreach ($other in @('legacy_dps', 'runtime')) {
        $diff = @(Compare-Object $events['legacy'] $events[$other])
        $differences[$other] = @{ different_entries = $diff.Count; first_differences = @($diff | Select-Object -First 12) }
        if ($diff.Count -ne 0) { throw "Expected exact timeline parity for $macroName / $other" }
    }
    $summaries += [ordered]@{ macro = $macroName; phase = 7; forced_critical = $true; precision = 1; samples = $samples; differences = $differences }
    Write-Host "$macroName : legacy=$($samples.legacy.mean_damage), runtime=$($samples.runtime.mean_damage), differing events=$($differences.runtime.different_entries)"
}
[IO.File]::WriteAllText((Join-Path $OutputDirectory 'validation.json'), ($summaries | ConvertTo-Json -Depth 15), $utf8)
