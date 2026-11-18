[CmdletBinding()]
param(
    [int]$TalentVersion = 20260427,
    [string]$Output = "data/jx3box/tai_xu_jian_yi_202604.json",
    [string]$Header = ""
)

$ErrorActionPreference = "Stop"
$utf8 = New-Object System.Text.UTF8Encoding($false)

function Write-Utf8Json($Value, [string]$Path) {
    $parent = Split-Path -Parent $Path
    if ($parent) { New-Item -ItemType Directory -Force $parent | Out-Null }
    [IO.File]::WriteAllText([IO.Path]::GetFullPath($Path), ($Value | ConvertTo-Json -Depth 40), $utf8)
}

function Strip-Html([string]$Html) {
    if ([string]::IsNullOrWhiteSpace($Html)) { return "" }
    $text = [System.Net.WebUtility]::HtmlDecode([regex]::Replace($Html, '<[^>]+>', ' '))
    return ($text -replace '\s+', ' ').Trim()
}

function Get-TalentRows($Groups) {
    $rows = @()
    for ($groupIndex = 0; $groupIndex -lt @($Groups).Count; ++$groupIndex) {
        foreach ($entry in @($Groups[$groupIndex])) {
            if ($null -eq $entry) { continue }
            $rows += [ordered]@{
                group = $groupIndex + 1
                id = [int]$entry.id
                max = [int]$entry.max
                name = [string]$entry.name
                type = [string]$entry.type
                icon_id = [int]$entry.icon
                descriptions = @($entry.desc)
            }
        }
    }
    return @($rows)
}

function Cpp-String([object]$Value) {
    if ($null -eq $Value) { return '""' }
    $text = [string]$Value
    $text = $text.Replace('\', '\\').Replace('"', '\"').Replace("`r", '\r').Replace("`n", '\n')
    return '"' + $text + '"'
}

function Write-HistoryHeader($Snapshot, [string]$Path) {
    $lines = [System.Collections.Generic.List[string]]::new()
    $lines.Add('#ifndef JX3DPS_CLASS_TAI_XU_JIAN_YI_202604_DATA_H')
    $lines.Add('#define JX3DPS_CLASS_TAI_XU_JIAN_YI_202604_DATA_H')
    $lines.Add('')
    $lines.Add('#include <array>')
    $lines.Add('#include "src/global/types.h"')
    $lines.Add('')
    $lines.Add('namespace JX3DPS::太虚剑意::April2026 {')
    $lines.Add('')
    $lines.Add('struct TalentInfo {')
    $lines.Add('    int group;')
    $lines.Add('    jx3id_t id;')
    $lines.Add('    int max;')
    $lines.Add('    int icon;')
    $lines.Add('    const char *name;')
    $lines.Add('    const char *type;')
    $lines.Add('    const char *description;')
    $lines.Add('};')
    $lines.Add('')
    $lines.Add("inline constexpr const char *DATA_VERSION = $(Cpp-String $Snapshot.data_version);")
    $lines.Add("inline constexpr const char *RELEASE = $(Cpp-String $Snapshot.release);")
    $lines.Add("inline constexpr int TALENT_VERSION = $([int]$Snapshot.archive.talent_version);")
    $lines.Add("inline constexpr int OFFICIAL_CHANGELOG_ID = $([int]$Snapshot.formal_changelog.id);")
    $lines.Add("inline constexpr int APRIL_PATCH_ID = $([int]$Snapshot.april_30_patch.id);")
    $lines.Add('')
    $rows = @($Snapshot.archive.groups)
    $lines.Add("inline constexpr std::array<TalentInfo, $($rows.Count)> TALENTS = {{")
    foreach ($row in $rows) {
        $description = @($row.descriptions) -join "`n"
        $lines.Add("    { $([int]$row.group), $([int]$row.id), $([int]$row.max), $([int]$row.icon_id), $(Cpp-String $row.name), $(Cpp-String $row.type), $(Cpp-String $description) },")
    }
    $lines.Add('}};')
    $lines.Add('')
    $lines.Add('} // namespace JX3DPS::太虚剑意::April2026')
    $lines.Add('')
    $lines.Add('#endif // JX3DPS_CLASS_TAI_XU_JIAN_YI_202604_DATA_H')

    $parent = Split-Path -Parent $Path
    if ($parent) { New-Item -ItemType Directory -Force $parent | Out-Null }
    [IO.File]::WriteAllLines([IO.Path]::GetFullPath($Path), $lines, $utf8)
}

$talentUrl = "https://node.jx3box.com/api/node/talent-origin/$TalentVersion"
$changelogUrl = "https://cms.jx3box.com/api/cms/pve/skill/changelog/91"
$pureYangUrl = "https://jx3.xoyo.com/api.php?op=search_api&action=get_article_detail&catid=2466&id=7268&game=jx3"
$patchUrl = "https://jx3.xoyo.com/api.php?op=search_api&action=get_customer_article_detail&kid=1335493&game=jx3"
$patchId = 1335493

$talentResponse = Invoke-RestMethod -Uri $talentUrl -Method Get
if ($talentResponse.code -ne 0 -or $null -eq $talentResponse.data.detail.'剑宗') {
    throw "魔盒没有返回 $TalentVersion 的剑宗历史数据。"
}
$changelog = (Invoke-RestMethod -Uri $changelogUrl -Method Get).data
$pureYang = (Invoke-RestMethod -Uri $pureYangUrl -Method Get).data[0]
$patch = (Invoke-RestMethod -Uri $patchUrl -Method Get).data

$snapshot = [ordered]@{
    schema_version = 1
    game_version = "std"
    release = "暗影千机"
    target = "太虚剑意"
    data_version = ([string]$TalentVersion).Insert(4, "-").Insert(7, "-")
    version_source = "魔盒 talent-origin 历史归档 + 官方正式服公告"
    source_urls = [ordered]@{
        talent_archive = $talentUrl
        changelog = $changelogUrl
        pure_yang_detail = $pureYangUrl
        april_30_patch = $patchUrl
        icon = "https://icon.jx3box.com/icon/{id}.png"
    }
    archive = [ordered]@{
        version = [string]$talentResponse.data.version
        talent_version = [int]$talentResponse.data.talent_version
        school = "剑宗"
        mapping = "魔盒历史归档中的剑宗对应太虚剑意"
        groups = Get-TalentRows $talentResponse.data.detail.'剑宗'
    }
    formal_changelog = [ordered]@{
        id = [int]$changelog.id
        date = [string]$changelog.date
        title = [string]$changelog.title
        updated_at = [string]$changelog.updated_at
        summary = Strip-Html $changelog.content
    }
    pure_yang_detail = [ordered]@{
        id = [int]$pureYang.id
        category_id = [int]$pureYang.catid
        title = [string]$pureYang.title
        inputtime = [string]$pureYang.inputtime
        updatetime = [string]$pureYang.updatetime
        url = [string]$pureYang.url
        html = [string]$pureYang.content
    }
    april_30_patch = [ordered]@{
        id = $patchId
        title = [string]$patch.title
        asktime = [string]$patch.asktime
        text = Strip-Html $patch.content
    }
    notes = @(
        "2026-04-23 是暗影千机正式武学调整公告。",
        "2026-04-27 是魔盒目前可取得的 4 月末奇穴历史归档。",
        "2026-04-30 正式服补丁将太虚剑意第五重匣中鸣的匣中剑一至匣中剑四伤害降低 15%。",
        "技能接口 /skills 没有历史版本参数，技能基线以官方纯阳调整全文为准，避免将当前技能数据混入 4 月快照。"
    )
}

Write-Utf8Json $snapshot $Output
if (-not [string]::IsNullOrWhiteSpace($Header)) {
    Write-HistoryHeader $snapshot $Header
    Write-Host "已生成 4 月版本编译期数据表：$Header"
}
Write-Host "已保存 $TalentVersion 太虚剑意历史快照：$Output"
