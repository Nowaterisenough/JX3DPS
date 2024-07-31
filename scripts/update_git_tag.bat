@echo off
for /f "tokens=2 delims= " %%a in ('findstr /C:"project(" CMakeLists.txt') do set version=%%a
git tag -a "v%version%" -m "Version %version%"
git push origin "v%version%"