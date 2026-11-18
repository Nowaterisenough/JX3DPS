[CmdletBinding()]
param(
    [string]$Output = "data/jx3box/tai_xu_jian_yi_latest.json",
    [string]$Header = "src/class/tai_xu_jian_yi/tai_xu_jian_yi_live_data.h",
    [switch]$SkipNetwork
)

$ErrorActionPreference = "Stop"
$utf8 = New-Object System.Text.UTF8Encoding($false)

function Read-Utf8Json([string]$Path) {
    return (Get-Content -Raw -Encoding utf8 $Path | ConvertFrom-Json)
}

function Write-Utf8Json($Value, [string]$Path) {
    $parent = Split-Path -Parent $Path
    if ($parent) { New-Item -ItemType Directory -Force $parent | Out-Null }
    [IO.File]::WriteAllText([IO.Path]::GetFullPath($Path), ($Value | ConvertTo-Json -Depth 30), $utf8)
}

function Get-FirstArray($Value) {
    if ($null -eq $Value) { return @() }
    if ($Value -is [array]) { return @($Value) }
    foreach ($key in @("data", "list", "items", "result")) {
        if ($Value.PSObject.Properties.Name -contains $key) {
            $found = Get-FirstArray $Value.$key
            if ($found.Count -gt 0) { return $found }
        }
    }
    return @($Value)
}

function Get-AllItemRecords([string]$BaseUrl) {
    $first = Invoke-RestMethod -Uri ($BaseUrl + "&page=1") -Method Get
    $payload = $first.data
    $rows = @($payload.data)
    $perPage = [int]$payload.per_page
    $total = [int]$payload.total
    if ($perPage -le 0) { $perPage = 10 }
    $pageCount = [math]::Ceiling($total / $perPage)
    for ($page = 2; $page -le $pageCount; ++$page) {
        $next = Invoke-RestMethod -Uri ($BaseUrl + "&page=$page") -Method Get
        $rows += @($next.data.data)
    }
    return @($rows)
}

function Cpp-String([object]$Value) {
    if ($null -eq $Value) { return '""' }
    $text = [string]$Value
    $text = $text.Replace('\', '\\').Replace('"', '\"').Replace("`r", '\r').Replace("`n", '\n')
    return '"' + $text + '"'
}

function Icon-Id([object]$Icon) {
    if ($null -eq $Icon) { return 0 }
    $match = [regex]::Match([string]$Icon, '/(\d+)\.png')
    if ($match.Success) { return [int]$match.Groups[1].Value }
    return 0
}

function Set-SnapshotProperty($Object, [string]$Name, $Value) {
    if ($Object -is [System.Collections.IDictionary]) {
        $Object[$Name] = $Value
    } else {
        $Object | Add-Member -NotePropertyName $Name -NotePropertyValue $Value -Force
    }
}

function Normalize-Snapshot($Snapshot) {
    $skillRoot = @($Snapshot.skill) | Where-Object { [string]$_.kungfuId -eq "4" -or $_.kungfu -eq "taixu" } | Select-Object -First 1
    if ($skillRoot) { Set-SnapshotProperty $Snapshot "skill" $skillRoot }
    $talentRoot = @($Snapshot.talent) | Where-Object { [string]$_.kungfuId -eq "4" -or $_.kungfu -eq "太虚剑意" } | Select-Object -First 1
    if ($talentRoot) { Set-SnapshotProperty $Snapshot "talent" $talentRoot }

    # skill.json contains several Pure Yang kungfu branches. Keep the entries
    # whose kungfuIds include 太虚剑意 (4), then deduplicate by skill ID.
    $skillById = @{}
    foreach ($remark in @($Snapshot.skill.remarks)) {
        foreach ($skill in @($remark.forceSkills)) {
            if (@($skill.kungfuIds) -contains "4" -and $skill._id) {
                $skillById[[string]$skill._id] = $skill
            }
        }
    }
    $remarkMap = @{}
    foreach ($skill in $skillById.Values) {
        $remarkName = if ($skill.skillName -in @("大道无术", "剑冲阴阳", "无我无剑", "万剑归宗", "人剑合一", "三环套月", "八荒归元")) { "天道剑势" } else { "纯阳诀" }
        if (-not $remarkMap.ContainsKey($remarkName)) {
            $remarkMap[$remarkName] = [ordered]@{ forceSkills = @(); remark = $remarkName }
        }
        $remarkMap[$remarkName].forceSkills = @($remarkMap[$remarkName].forceSkills) + @($skill)
    }
    Set-SnapshotProperty $Snapshot.skill "remarks" @($remarkMap.Values)

    # talent.json returns a general Pure Yang table followed by the current
    # 太虚剑意 table. The last row for each tier is the current one.
    $talentByLevel = @{}
    foreach ($level in @($Snapshot.talent.kungfuLevel)) {
        $talentByLevel[[string]$level.level] = $level
    }
    $currentTalents = @($talentByLevel.Values | Sort-Object { [int]$_.level })
    Set-SnapshotProperty $Snapshot.talent "kungfuLevel" $currentTalents

    $normalizedItems = @()
    $gearTypes = @("帽子", "头部", "上衣", "腰带", "护腕", "护手", "下装", "裤子", "鞋子", "项链", "腰坠", "戒指", "武器", "短兵", "神兵")
    foreach ($item in @(Get-FirstArray $Snapshot.equipment_catalog)) {
        $itemType = if ($item.type) { [string]$item.type } else { [string]$item.TypeLabel }
        $isRecommended = ([string]$item.recommend).Contains("太虚剑意") -or ([string]$item.Recommend).Contains("太虚剑意")
        if ($item.IsEquip -eq $false -or ($itemType -notin $gearTypes -and -not $isRecommended)) { continue }
        $normalized = [ordered]@{
            id = if ($item.id) { $item.id } else { "8_$($item.SourceID)" }
            source_id = if ($item.source_id) { $item.source_id } else { $item.SourceID }
            name = if ($item.name) { $item.name } else { $item.Name }
            icon_id = if ($item.icon_id) { $item.icon_id } else { $item.IconID }
            level = if ($item.level) { $item.level } else { $item.Level }
            quality = if ($item.quality) { $item.quality } else { $item.Quality }
            type = if ($item.type) { $item.type } else { $item.TypeLabel }
            magic_type = if ($item.magic_type) { $item.magic_type } else { $item.MagicType }
            recommend = if ($item.recommend) { $item.recommend } else { $item.Recommend }
            max_refine = if ($item.max_refine) { $item.max_refine } else { $item.MaxStrengthLevel }
            set = if ($item.set) { $item.set } else { $item.Set }
            attributes = if ($item.attributes) { @($item.attributes) } else { @() }
            diamonds = if ($item.diamonds) { @($item.diamonds) } else { @($item.Diamonds) }
        }
        # Preserve optional tooltip metadata without treating absent values as zero.
        $detailFields = [ordered]@{
            require_level = 'RequireLevel'; max_durability = 'MaxDurability'; source = 'GetSource'
            refine_level = 'refineLevel'; durability = 'Durability'; score = 'Score'
            level_refine_bonus = 'level_refine_bonus'; score_refine_bonus = 'score_refine_bonus'
            score_enchant_bonus = 'score_enchant_bonus'; enchantments = 'enchantments'; effects = 'effects'
        }
        foreach ($key in $detailFields.Keys) {
            $value = if ($null -ne $item.$key) { $item.$key } else { $item.($detailFields[$key]) }
            if ($null -ne $value) { $normalized[$key] = $value }
        }
        $normalizedItems += $normalized
    }
    Set-SnapshotProperty $Snapshot "equipment_catalog" @($normalizedItems)
    return $Snapshot
}

function Generate-LiveHeader($Snapshot, [string]$Path) {
    $skills = @()
    foreach ($remark in @($Snapshot.skill.remarks)) { $skills += @($remark.forceSkills) }
    $talents = @($Snapshot.talent.kungfuLevel | ForEach-Object { $_.kungfuSkills })
    $recipes = @()
    foreach ($skill in $skills) {
        foreach ($recipe in @($skill.cheasts)) {
            if ($recipe.name) {
                $recipes += [pscustomobject]@{
                    name = $recipe.name
                    desc = $recipe.desc
                    icon = (Icon-Id $skill.icon.FileName)
                }
            }
        }
    }
    $recipes = @($recipes | Group-Object name | ForEach-Object { $_.Group[0] })
    $items = @(Get-FirstArray $Snapshot.equipment_catalog)

    $lines = [Collections.Generic.List[string]]::new()
    $lines.Add('#ifndef JX3DPS_CLASS_TAI_XU_JIAN_YI_LIVE_DATA_H')
    $lines.Add('#define JX3DPS_CLASS_TAI_XU_JIAN_YI_LIVE_DATA_H')
    $lines.Add('')
    $lines.Add('#include <array>')
    $lines.Add('#include "src/global/types.h"')
    $lines.Add('')
    $lines.Add('namespace JX3DPS::太虚剑意::LiveData {')
    $lines.Add('struct SkillInfo { jx3id_t id; int icon; const char *name; const char *description; };')
    $lines.Add('struct TalentInfo { int tier; jx3id_t id; int icon; const char *name; const char *description; };')
    $lines.Add('struct RecipeInfo { int icon; const char *name; const char *description; };')
    $lines.Add('struct EquipmentInfo { jx3id_t id; int icon; int level; int quality; const char *slot; const char *name; const char *set_name; };')
    $lines.Add("inline constexpr std::array<SkillInfo, $($skills.Count)> SKILLS = {{")
    foreach ($skill in $skills) {
        $id = [int]$skill._id
        $icon = Icon-Id $skill.icon.FileName
        $lines.Add("    { $id, $icon, $(Cpp-String $skill.skillName), $(Cpp-String $skill.desc) },")
    }
    $lines.Add('}};')
    $lines.Add("inline constexpr std::array<TalentInfo, $($talents.Count)> TALENTS = {{")
    foreach ($talent in $talents) {
        $id = [int]$talent.skill_id
        $icon = Icon-Id $talent.icon.FileName
        $lines.Add("    { $([int]$talent.level), $id, $icon, $(Cpp-String $talent.name), $(Cpp-String $talent.desc) },")
    }
    $lines.Add('}};')
    $lines.Add("inline constexpr std::array<RecipeInfo, $($recipes.Count)> RECIPES = {{")
    foreach ($recipe in $recipes) {
        $lines.Add("    { $([int]$recipe.icon), $(Cpp-String $recipe.name), $(Cpp-String $recipe.desc) },")
    }
    $lines.Add('}};')
    $lines.Add("inline constexpr std::array<EquipmentInfo, $($items.Count)> EQUIPMENT = {{")
    foreach ($item in $items) {
        $id = if ($item.source_id) { [int]$item.source_id } else { 0 }
        $icon = if ($item.icon_id) { [int]$item.icon_id } else { 0 }
        $setName = if ($item.set -and $item.set.name) { $item.set.name } else { '' }
        $slot = if ($item.type) { $item.type } else { '' }
        $lines.Add("    { $id, $icon, $([int]$item.level), $([int]$item.quality), $(Cpp-String $slot), $(Cpp-String $item.name), $(Cpp-String $setName) },")
    }
    $lines.Add('}};')
    $lines.Add('} // namespace JX3DPS::太虚剑意::LiveData')
    $lines.Add('')
    $lines.Add('#endif // JX3DPS_CLASS_TAI_XU_JIAN_YI_LIVE_DATA_H')

    $parent = Split-Path -Parent $Path
    if ($parent) { New-Item -ItemType Directory -Force $parent | Out-Null }
    [IO.File]::WriteAllLines((Join-Path (Get-Location) $Path), $lines, $utf8)
}

$skillUrl = "https://data.jx3box.com/bps/std/4/skill.json"
$talentUrl = "https://data.jx3box.com/bps/std/4/talent.json"
$itemUrl = "https://node.jx3box.com/api/node/item/search?keyword=太虚&client=std"
$changelogUrl = "https://cms.jx3box.com/api/cms/pve/skill/changelog/98"

if ($SkipNetwork -and (Test-Path $Output)) {
    $snapshot = Read-Utf8Json $Output
} else {
    $snapshot = [ordered]@{
        schema_version = 1
        game_version = "std"
        data_version = (Get-Date -Format "yyyy-MM-dd")
        source = "JX3BOX / 魔盒"
        sources = [ordered]@{
            skill = $skillUrl
            talent = $talentUrl
            recipe = "https://data.jx3box.com/bps/std/4/skill.json#cheasts"
            item_search = $itemUrl
            icon = "https://icon.jx3box.com/icon/{id}.png"
            changelog = $changelogUrl
        }
        skill = Invoke-RestMethod -Uri $skillUrl -Method Get
        talent = Invoke-RestMethod -Uri $talentUrl -Method Get
        equipment_catalog = @(Get-AllItemRecords $itemUrl)
        changelog = (Invoke-RestMethod -Uri $changelogUrl -Method Get).data |
            Select-Object id, date, title, zlp, updated_at
    }
}

$snapshot = Normalize-Snapshot $snapshot

$recipes = @()
foreach ($remark in @($snapshot.skill.remarks)) {
    foreach ($skill in @($remark.forceSkills)) {
        foreach ($recipe in @($skill.cheasts)) {
            if ($recipe.name) {
                $recipes += [pscustomobject][ordered]@{
                    name = $recipe.name
                    description = $recipe.desc
                    icon_id = Icon-Id $skill.icon.FileName
                }
            }
        }
    }
}
$recipes = @($recipes | Group-Object name | ForEach-Object { $_.Group[0] })
Set-SnapshotProperty $snapshot "recipe_catalog" @($recipes)
Write-Utf8Json $snapshot $Output

Generate-LiveHeader $snapshot $Header
Write-Host "已同步魔盒数据：$Output"
Write-Host "已生成编译期快照头文件：$Header"
