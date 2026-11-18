# JX3DPS

[![Desktop CI](https://github.com/Nowaterisenough/JX3DPS/actions/workflows/desktop.yml/badge.svg?branch=master)](https://github.com/Nowaterisenough/JX3DPS/actions/workflows/desktop.yml)

|[License][license-link]| [Release][release-link]|[Download][download-link]|[Issues][issues-link]|
|-----------------|-----------------|-----------------|-----------------|
|![license-badge] |![release-badge] | ![download-badge]|![issues-badge]|

[release-link]: https://github.com/Nowaterisenough/JX3DPS/releases "Release status"
[release-badge]: https://img.shields.io/github/release/Nowaterisenough/JX3DPS.svg?style=flat-square "Release status"

[download-link]: https://github.com/Nowaterisenough/JX3DPS/releases/latest "Download status"
[download-badge]: https://img.shields.io/github/downloads/Nowaterisenough/JX3DPS/total.svg?style=flat-square "Download status"

[license-link]: https://github.com/Nowaterisenough/JX3DPS/blob/master/LICENSE "LICENSE"
[license-badge]: https://img.shields.io/badge/license-MIT-blue.svg "MIT"

[issues-link]: https://github.com/Nowaterisenough/JX3DPS/issues "Issues"
[issues-badge]: https://img.shields.io/badge/github-issues-red.svg?maxAge=60 "Issues"


桌面版在推送 Git 标签时发布。需要构建旧版独立动态库时，请使用[合并前的主分支源码](https://github.com/Nowaterisenough/JX3DPS/tree/03d529ff4e7077f9b6fafce28d8fec8c7b643bd0)。

## For Developer
1. git代码仓
```git
git clone --recurse-submodules git@github.com:Nowaterisenough/JX3DPS.git
```
2. CMake编译

桌面版可选用 GCC Release 的 PGO。运行 `python tools/desktop_pgo.py` 自动构建、训练并生成优化版，加上 `--benchmark` 可以与普通 Release 逐项对照。[桌面 PGO 构建与验证](docs/desktop_pgo.md)

