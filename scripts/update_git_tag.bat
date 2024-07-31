@echo off
setlocal EnableDelayedExpansion

rem 检查是否为自动模式
set AUTO_MODE=0
if "%1"=="auto" set AUTO_MODE=1

rem 设置是否使用 "v" 前缀（0 表示不使用，1 表示使用）
set USE_V_PREFIX=0

rem 保存当前目录
set "original_dir=%cd%"

rem 切换到脚本所在目录的父目录（即项目根目录）
cd /d "%~dp0.."

rem 检查 CMakeLists.txt 是否存在
if not exist "CMakeLists.txt" (
    echo Error: CMakeLists.txt not found in the project root directory.
    goto :exit
)

rem 创建构建目录（如果不存在）
if not exist "build" mkdir build

rem 切换到构建目录
cd build

rem 运行 CMake 配置并获取版本号
cmake -DCMAKE_BUILD_TYPE=Release .. > cmake_output.txt

rem 从 CMake 输出中提取版本号
set "version="
for /f "tokens=2 delims==" %%a in ('findstr /C:"JX3DPS_VERSION=" cmake_output.txt') do (
    set "version=%%a"
)

rem 如果没有找到版本号，报错并退出
if "!version!"=="" (
    echo Error: Could not extract version number from CMake output.
    goto :exit
)

rem 切换回项目根目录
cd ..

rem 根据 USE_V_PREFIX 设置决定是否添加 "v" 前缀
if %USE_V_PREFIX%==1 (
    set "tag_version=v!version!"
) else (
    set "tag_version=!version!"
)

rem 检查标签是否已存在
git rev-parse "!tag_version!" >nul 2>&1
if not errorlevel 1 (
    if %AUTO_MODE%==1 (
        echo Tag !tag_version! already exists. Skipping tag creation and push.
        goto :exit
    ) else (
        echo Tag !tag_version! already exists. Do you want to force update it? (Y/N)
        set /p confirm=
        if /i "!confirm!" neq "Y" goto :exit
        set "force_flag=-f"
    )
) else (
    set "force_flag="
)

rem 创建 Git 标签
git tag -a !force_flag! "!tag_version!" -m "Version !version!"

rem 推送标签到远程仓库（无论是否为自动模式）
git push origin "!tag_version!"
echo Tag !tag_version! has been created and pushed to remote.

:exit
rem 返回到原始目录
cd /d "%original_dir%"

if %AUTO_MODE%==0 (
    echo Script completed.
    pause
)

endlocal