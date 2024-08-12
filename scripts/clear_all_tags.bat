@echo off
setlocal EnableDelayedExpansion

echo Deleting all local tags...
for /f "tokens=*" %%i in ('git tag') do (
    git tag -d %%i
)

echo Deleting all remote tags...
for /f "tokens=2" %%i in ('git ls-remote --tags origin') do (
    set "tag=%%i"
    set "tag=!tag:refs/tags/=!"
    git push --delete origin !tag!
)

echo All tags have been deleted locally and remotely.

pause