[CmdletBinding()]
param (
    [Parameter(Mandatory=$true)]
    [string] $archiveName,
    [Parameter(Mandatory=$true)]
    [string] $targetName
)

# 设置错误操作首选项
$ErrorActionPreference = "Stop"

# 日志函数
function Write-Log {
    param([string]$message)
    Write-Host "$(Get-Date -Format "yyyy-MM-dd HH:mm:ss") - $message"
}

# 主函数
function Main {
    try {
        Write-Log "开始打包过程..."
        Write-Log "Archive Name: $archiveName"
        Write-Log "Target Name: $targetName"

        $currentDir = Get-Location
        Write-Log "当前目录: $currentDir"

        # 创建归档目录
        if (Test-Path $archiveName) {
            Remove-Item -Path $archiveName -Recurse -Force
        }
        New-Item -ItemType Directory $archiveName | Out-Null
        Write-Log "创建目录: $archiveName"

        # 拷贝构建文件
        Write-Log "正在拷贝构建文件..."
        Copy-Item build\* $archiveName\ -Recurse

        # 运行 windeployqt
        Write-Log "正在运行 windeployqt..."
        $windeployqtArgs = @(
            "--qmldir", ".",
            "--plugindir", "$archiveName\plugins",
            "--no-translations",
            "--compiler-runtime",
            "$archiveName\$targetName"
        )
        & windeployqt $windeployqtArgs
        if ($LASTEXITCODE -ne 0) {
            throw "windeployqt 执行失败"
        }

        # 删除不必要的文件
        Write-Log "正在清理不必要的文件..."
        $excludeList = @("*.qmlc", "*.ilk", "*.exp", "*.lib", "*.pdb")
        foreach ($pattern in $excludeList) {
            Remove-Item -Path $archiveName -Include $pattern -Recurse -Force
        }

        # 打包 zip
        Write-Log "正在创建 ZIP 归档..."
        $zipPath = "$archiveName.zip"
        if (Test-Path $zipPath) {
            Remove-Item -Path $zipPath -Force
        }
        Compress-Archive -Path $archiveName -DestinationPath $zipPath

        Write-Log "打包过程完成。输出文件: $zipPath"
    }
    catch {
        Write-Log "错误: $_"
        exit 1
    }
}

# 参数验证
if ([string]::IsNullOrWhiteSpace($archiveName) -or [string]::IsNullOrWhiteSpace($targetName)) {
    Write-Log "错误: 缺少必要参数。ArchiveName: '$archiveName', TargetName: '$targetName'"
    exit 1
}

# 执行主函数
Main