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

        # 检查 build 目录中是否存在目标文件
        $sourcePath = "build\$targetName"
        if (-not (Test-Path $sourcePath)) {
            throw "错误: 在 build 目录中找不到 $targetName"
        }

        # 创建临时目录
        $tempDir = New-Item -ItemType Directory -Path "$archiveName" -Force
        Write-Log "创建临时目录: $tempDir"

        # 复制目标文件到临时目录
        Copy-Item $sourcePath $tempDir
        Write-Log "复制 $targetName 到临时目录"

        # 打包 zip
        Write-Log "正在创建 ZIP 归档..."
        $zipPath = "$archiveName.zip"
        if (Test-Path $zipPath) {
            Remove-Item -Path $zipPath -Force
        }
        Compress-Archive -Path $tempDir\* -DestinationPath $zipPath

        # 清理临时目录
        Remove-Item -Path $tempDir -Recurse -Force
        Write-Log "清理临时目录"

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