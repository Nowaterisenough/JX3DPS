param(
    [string]$BuildDirectory = (Join-Path $PSScriptRoot '../../../build-legacy-compare'),
    [string]$LegacyRoot = '',
    [string]$OutputDirectory = '',
    [ValidateRange(1, 1000000)][int]$Iterations = 10000,
    [ValidateRange(1, 20)][int]$Repeats = 3,
    [string[]]$Cases = @('cooldowns', 'cooldowns_precise', 'rotation', 'rotation_dot'),
    [string[]]$Engines = @('legacy', 'legacy_dps', 'runtime'),
    [string]$GitExecutable = 'git',
    [long]$Affinity = 1
)
$ErrorActionPreference = 'Stop'
$BuildDirectory = (Resolve-Path -LiteralPath $BuildDirectory).Path
if (-not $LegacyRoot) {
    foreach ($candidate in @('../../../../ba960f7', '../../../../JX3DPS-worktrees/ba960f7')) {
        $candidatePath = Join-Path $PSScriptRoot $candidate
        if (Test-Path -LiteralPath (Join-Path $candidatePath 'src/KeyFrame.cpp')) {
            $LegacyRoot = $candidatePath
            break
        }
    }
    if (-not $LegacyRoot) { throw 'Specify -LegacyRoot with the pinned ba960f7 worktree path.' }
}
$LegacyRoot = (Resolve-Path -LiteralPath $LegacyRoot).Path
$RuntimeRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '../../..')).Path
if (-not $OutputDirectory) { $OutputDirectory = Join-Path $BuildDirectory ('results/' + (Get-Date -Format 'yyyyMMdd-HHmmss')) }
$OutputDirectory = [IO.Path]::GetFullPath($OutputDirectory)
if (Test-Path -LiteralPath (Join-Path $OutputDirectory 'timings.jsonl')) { throw 'Choose a fresh output directory.' }
$null = New-Item -ItemType Directory -Path $OutputDirectory -Force
$utf8 = New-Object Text.UTF8Encoding($false)
[Console]::OutputEncoding = $utf8
$OutputEncoding = $utf8
$definitions = @{
    cooldowns_precise = @{ macro = 'cooldowns'; precision = 1; dot = 0 }
    cooldowns = @{ macro = 'cooldowns'; precision = 16; dot = 0 }
    rotation = @{ macro = 'rotation'; precision = 16; dot = 0 }
    rotation_dot = @{ macro = 'rotation'; precision = 16; dot = 1 }
    weapon_cw = @{ macro = 'rotation'; precision = 1; dot = 1; weapon = 1 }
    sanchai = @{ macro = 'sanchai'; precision = 16; dot = 0 }
}
foreach ($case in $Cases) { if (-not $definitions.ContainsKey($case)) { throw "Unknown case: $case" } }
foreach ($engine in $Engines) {
    if ($engine -notin @('legacy', 'legacy_dps', 'runtime')) { throw "Unknown engine: $engine" }
    if (-not (Test-Path -LiteralPath (Join-Path $BuildDirectory "${engine}_bench.exe"))) { throw "Missing binary: $engine" }
}
$hashes = [ordered]@{}
$files = @(Get-ChildItem (Join-Path $RuntimeRoot 'src/core/runtime') -File) +
    @(Get-Item (Join-Path $RuntimeRoot 'src/core/runtime.hpp')) +
    @(Get-ChildItem (Join-Path $RuntimeRoot 'src/class/tai_xu_jian_yi') -Filter 'runtime*.hpp') +
    @(Get-ChildItem $PSScriptRoot -File) + @(Get-ChildItem (Join-Path $PSScriptRoot 'macros') -File)
foreach ($file in $files) { $hashes[$file.FullName.Substring($RuntimeRoot.Length + 1)] = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash }
$binaries = [ordered]@{}
foreach ($engine in $Engines) { $binaries[$engine] = (Get-FileHash -LiteralPath (Join-Path $BuildDirectory "${engine}_bench.exe")).Hash }
$cpuInfo = Get-CimInstance Win32_Processor | Select-Object -First 1
$legacyCommit = & $GitExecutable -C $LegacyRoot rev-parse HEAD
if ($LASTEXITCODE -ne 0) { throw 'Cannot read legacy revision; pass -GitExecutable with a working Git installation.' }
$runtimeCommit = & $GitExecutable -C $RuntimeRoot rev-parse HEAD
if ($LASTEXITCODE -ne 0) { throw 'Cannot read runtime revision.' }
$metadata = [ordered]@{
    started = (Get-Date -Format o)
    cpu = @{ name = $cpuInfo.Name.ToString(); cores = [int]$cpuInfo.NumberOfCores; logical_processors = [int]$cpuInfo.NumberOfLogicalProcessors }
    os = [Environment]::OSVersion.VersionString
    affinity_mask = $Affinity
    legacy_commit = $legacyCommit
    legacy_status = @(& $GitExecutable -C $LegacyRoot status --porcelain)
    runtime_head = $runtimeCommit
    runtime_status = @(& $GitExecutable -C $RuntimeRoot status --porcelain)
    cmake_cache = @(Get-Content -LiteralPath (Join-Path $BuildDirectory 'CMakeCache.txt') | Where-Object { $_ -match '^CMAKE_(CXX_COMPILER:|CXX_FLAGS_RELEASE:|BUILD_TYPE:)' } | ForEach-Object { $_.ToString() })
    compiler = @(& clang++ --version)
    iterations = $Iterations
    repeats = $Repeats
    seconds = 300
    warmup = 20
    crit = 20000
    cases = $Cases
    engines = $Engines
    source_sha256 = $hashes
    binary_sha256 = $binaries
}
[IO.File]::WriteAllText((Join-Path $OutputDirectory 'metadata.json'), ($metadata | ConvertTo-Json -Depth 8), $utf8)
$process = [Diagnostics.Process]::GetCurrentProcess()
$previousAffinity = $process.ProcessorAffinity
try {
    if ($Affinity -gt 0) { $process.ProcessorAffinity = [IntPtr]$Affinity }
    for ($repeat = 1; $repeat -le $Repeats; $repeat++) {
        # Alternate order to reduce systematic warm-up / thermal bias.
        $order = @($Engines)
        if ($repeat % 2 -eq 0) { [Array]::Reverse($order) }
        foreach ($case in $Cases) {
            $config = $definitions[$case]
            foreach ($engine in $order) {
                $exe = Join-Path $BuildDirectory "${engine}_bench.exe"
                $macro = Join-Path $PSScriptRoot ('macros/' + $config.macro + '.txt')
                $timer = [Diagnostics.Stopwatch]::StartNew()
                $raw = & $exe --macro $macro --iterations $Iterations --seconds 300 --warmup 20 --crit 20000 --precision $config.precision --dot $config.dot --weapon-cw ([int]$config.weapon)
                $timer.Stop()
                if ($LASTEXITCODE -ne 0) { throw "Benchmark failed: $case / $engine ($LASTEXITCODE)" }
                $result = $raw | ConvertFrom-Json
                $row = [ordered]@{ case = $case; repeat = $repeat; binary = $engine; process_wall_s = $timer.Elapsed.TotalSeconds; result = $result }
                [IO.File]::AppendAllText((Join-Path $OutputDirectory 'timings.jsonl'), ($row | ConvertTo-Json -Depth 12 -Compress) + "`n", $utf8)
                Write-Host ('{0} [{1}/{2}] {3}: {4:N4} s, {5:N0} fights/s' -f $case, $repeat, $Repeats, $engine, $result.elapsed_s, $result.fights_per_s)
            }
        }
    }
} finally { $process.ProcessorAffinity = $previousAffinity }
Write-Host "Results: $OutputDirectory"
