param (
    [string]$Version = "13.2.0"
)

$ErrorActionPreference = "Stop"

function Write-Log {
    param([string]$message)
    Write-Host "$(Get-Date -Format "yyyy-MM-dd HH:mm:ss") - $message"
}

function Locate-Choco {
    $path = Get-Command 'choco' -ErrorAction SilentlyContinue
    if ($path) {
        return $path.Path
    } else {
        return Join-Path ${env:ProgramData} 'chocolatey' 'bin' 'choco'
    }
}

function Install-MinGW {
    param([string]$Version)
    
    $choco = Locate-Choco
    Write-Log "Installing MinGW $Version"
    & $choco upgrade mingw --version $Version -y --no-progress --allow-downgrade
    
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to install MinGW $Version"
    }
}

function Set-EnvPath {
    $mingwBin = "C:\ProgramData\chocolatey\lib\mingw\tools\install\mingw64\bin"
    Write-Log "Adding MinGW bin to PATH: $mingwBin"
    $env:PATH = "$mingwBin;$env:PATH"
    echo $mingwBin >> $env:GITHUB_PATH
}

try {
    Install-MinGW -Version $Version
    Set-EnvPath
    
    Write-Log "Verifying GCC installation"
    $gccVersion = & gcc --version
    Write-Log "Installed GCC version: $gccVersion"
    
    $prefix = "x86_64-w64-mingw32"
    $gcc = "$prefix-gcc"
    $gxx = "$prefix-g++"
    $windres = "$prefix-windres"
    
    echo "prefix=$prefix" >> $env:GITHUB_OUTPUT
    echo "gcc=$gcc" >> $env:GITHUB_OUTPUT
    echo "gxx=$gxx" >> $env:GITHUB_OUTPUT
    echo "windres=$windres" >> $env:GITHUB_OUTPUT
    
    Write-Log "GCC $Version installation completed successfully"
} catch {
    Write-Log "Error: $_"
    exit 1
}