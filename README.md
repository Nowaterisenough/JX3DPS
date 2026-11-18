# JX3DPS

| [Windows][win-link]| [Ubuntu][ubuntu-link]|[MacOS][macos-link]|
|---------------|---------------|-----------------|
| ![win-badge]  | ![ubuntu-badge]      | ![macos-badge] |


|[License][license-link]| [Release][release-link]|[Download][download-link]|[Issues][issues-link]|
|-----------------|-----------------|-----------------|-----------------|
|![license-badge] |![release-badge] | ![download-badge]|![issues-badge]|

[win-link]: https://github.com/Nowaterisenough/JX3DPS/actions?query=workflow%3AWindows "WindowsAction"
[win-badge]: https://github.com/Nowaterisenough/JX3DPS/workflows/Windows/badge.svg  "Windows"

[ubuntu-link]: https://github.com/Nowaterisenough/JX3DPS/actions?query=workflow%3AUbuntu "UbuntuAction"
[ubuntu-badge]: https://github.com/Nowaterisenough/JX3DPS/workflows/Ubuntu/badge.svg "Ubuntu"

[macos-link]: https://github.com/Nowaterisenough/JX3DPS/actions?query=workflow%3AMacOS "MacOSAction"
[macos-badge]: https://github.com/Nowaterisenough/JX3DPS/workflows/MacOS/badge.svg "MacOS"

[release-link]: https://github.com/Nowaterisenough/JX3DPS/releases "Release status"
[release-badge]: https://img.shields.io/github/release/Nowaterisenough/JX3DPS.svg?style=flat-square "Release status"

[download-link]: https://github.com/Nowaterisenough/JX3DPS/releases/latest "Download status"
[download-badge]: https://img.shields.io/github/downloads/Nowaterisenough/JX3DPS/total.svg?style=flat-square "Download status"

[license-link]: https://github.com/Nowaterisenough/JX3DPS/blob/master/LICENSE "LICENSE"
[license-badge]: https://img.shields.io/badge/license-MIT-blue.svg "MIT"

[issues-link]: https://github.com/Nowaterisenough/JX3DPS/issues "Issues"
[issues-badge]: https://img.shields.io/badge/github-issues-red.svg?maxAge=60 "Issues"


## For Developer
1. git代码仓
```git
git clone --recurse-submodules git@github.com:Nowaterisenough/JX3DPS.git
```
2. CMake编译

桌面版可选用 GCC Release 的 PGO。运行 `python tools/desktop_pgo.py` 自动构建、训练并生成优化版，加上 `--benchmark` 可以与普通 Release 逐项对照。[桌面 PGO 构建与验证](docs/desktop_pgo.md)

