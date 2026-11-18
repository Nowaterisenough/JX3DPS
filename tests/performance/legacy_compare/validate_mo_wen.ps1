param(
    [string]$BuildDirectory = 'build-legacy-compare',
    [string]$OutputDirectory = 'tests/performance/legacy_compare/results/2026-09-19-mo-wen',
    [int]$Iterations = 1000,
    [int]$Rounds = 3
)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$build = [IO.Path]::GetFullPath((Join-Path $repo $BuildDirectory))
$output = [IO.Path]::GetFullPath((Join-Path $repo $OutputDirectory))
New-Item -ItemType Directory -Force -Path $output | Out-Null
$talents = 'xianfeng,liuzhao,zhenglv,canlian,yunhan,mingjin,zhizhi,haoqing,shixiang,haozhong,kemeng,feifan,zhengming'
function Run-Engine([string]$engine, [object[]]$arguments) {
    $text = & (Join-Path $build "$engine.exe") @arguments
    if ($LASTEXITCODE -ne 0) { throw "$engine failed ($LASTEXITCODE)" }
    return ($text | ConvertFrom-Json)
}
function Events([string]$path) {
    $names = @{'破'='破招'; '破·争鸣'='争鸣'; '破·流照'='破招·流照'; 'Dot·神兵·宫'='神兵·宫·DOT'; '变宫·神兵'='神兵·变宫'}
    foreach ($row in (Import-Csv $path)) {
        $name = $row.name
        if ($names.ContainsKey($name)) { $name = $names[$name] }
        '{0:D8},{1},{2},{3}' -f [int]$row.frame,$name,$row.damage,$row.outcome
    }
}
$validation = @()
foreach ($scenario in @('gong','zhi','rotation')) {
    $macro = Join-Path $PSScriptRoot "macros/mo_wen_$scenario.txt"
    $critical = if ($scenario -eq 'rotation') { 1000000 } else { 20000 }
    $argsForRun = @('--macro',$macro,'--iterations',1,'--seconds',300,'--warmup',0,
        '--phase',0,'--precision',1,'--crit',$critical,'--talents',$talents)
    $oldTrace = Join-Path $output "$scenario-legacy.csv"
    $newTrace = Join-Path $output "$scenario-runtime.csv"
    $draws = Join-Path $output "$scenario-draws.json"
    $old = Run-Engine 'mo_wen_legacy_record' ($argsForRun + @('--trace',$oldTrace,'--draws',$draws))
    $new = Run-Engine 'mo_wen_runtime_replay' ($argsForRun + @('--trace',$newTrace,'--draws',$draws))
    $old | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $output "$scenario-legacy.json")
    $new | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $output "$scenario-runtime.json")
    $oldEvents = @(Events $oldTrace)
    $newEvents = @(Events $newTrace)
    # Same-frame shadow iteration order was an unordered_map implementation detail.
    if ($scenario -eq 'rotation') {
        $oldEvents = @($oldEvents | Sort-Object)
        $newEvents = @($newEvents | Sort-Object)
    }
    if ($old.damage_sum -ne $new.damage_sum -or $new.unused_draws -ne 0 -or
        ($oldEvents -join "`n") -cne ($newEvents -join "`n")) {
        throw "Mo Wen validation mismatch: $scenario"
    }
    $validation += [PSCustomObject]@{ scenario=$scenario; frames=4800; crit=$critical;
        events=$newEvents.Count; draws=$new.draws; damage=$new.damage_sum;
        comparison= $(if ($scenario -eq 'rotation') { 'same-frame sorted, all critical' } else { 'strict sequential random replay' }) }
}
$validation | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $output 'validation.json')
if ($Iterations -lt 1 -or $Rounds -lt 1) { throw 'Iterations and Rounds must be positive' }
$timings = @()
for ($round=1; $round -le $Rounds; ++$round) {
    foreach ($engine in @('mo_wen_legacy_bench','mo_wen_runtime_bench')) {
        $sample = Run-Engine $engine @('--macro',(Join-Path $PSScriptRoot 'macros/mo_wen_rotation.txt'),
            '--iterations',$Iterations,'--seconds',300,'--warmup',5,'--precision',1,'--crit',1000000,'--talents',$talents)
        if ($sample.mean_damage -ne $validation[-1].damage) { throw "$engine timing workload changed" }
        $timings += [PSCustomObject]@{round=$round;engine=$engine;iterations=$Iterations;seconds=300;
            elapsed_s=$sample.elapsed_s;fights_per_s=$sample.fights_per_s;damage_sum=$sample.damage_sum}
    }
}
$timings | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $output 'timings.json')
$legacyTimes = @($timings | Where-Object engine -eq 'mo_wen_legacy_bench' | Sort-Object elapsed_s)
$runtimeTimes = @($timings | Where-Object engine -eq 'mo_wen_runtime_bench' | Sort-Object elapsed_s)
$middle = [int][Math]::Floor($Rounds/2)
$result = [PSCustomObject]@{iterations=$Iterations;rounds=$Rounds;seconds_per_fight=300;workers=1;
    legacy_median_s=$legacyTimes[$middle].elapsed_s;runtime_median_s=$runtimeTimes[$middle].elapsed_s;
    speedup=$legacyTimes[$middle].elapsed_s/$runtimeTimes[$middle].elapsed_s;
    scope='All migrated talent flags, one GaoShan activation, all critical; legacy includes attribute-gain calculations, runtime reduces DPS only.';
    validation=$validation}
$result | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 (Join-Path $output 'summary.json')
$result | ConvertTo-Json -Depth 8
