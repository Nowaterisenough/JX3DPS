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


推送到 `master` 后，五种桌面构建及 WASM/Web 构建和性能校验全部通过时，GitHub Actions 会自动创建 Release，上传发行包并展示分类变更日志和性能图。推送 Git 标签也可以按该标签发布。需要构建旧版独立动态库时，请使用[合并前的主分支源码](https://github.com/Nowaterisenough/JX3DPS/tree/03d529ff4e7077f9b6fafce28d8fec8c7b643bd0)。

## 代码与入口

| 目录 | 职责 |
|---|---|
| `include/jx3dps.h`、`src/engine/` | 会话式 C ABI、配置解析和批次结果 |
| `src/core/runtime/` | 宏编译、模拟、事件队列、记录、批跑与属性收益 |
| `src/class/` | 主工作台 20 个苍生铸世旗舰心法的逐帧施法、资源、DOT 和增益规则。[使用与覆盖范围](docs/season50_runtime.md) · [默认宏、面板与在线木桩实测](docs/season50_presets.md) · [分山与各心法循环复盘](docs/season50_rotations.md) |
| `apps/desktop/`、`apps/cli/`、`apps/web/` | Qt 工作台、命令行和浏览器工作台 |
| `tests/engine/`、`tests/runtime/`、`tests/desktop/` | C 客户端、战斗内核和桌面集成回归 |
| `tools/release/` | 发布包基准、跨端校验与性能图生成 |

Web 版在浏览器 Worker 内执行相同的 C++ 引擎。用 Emscripten 构建后可直接部署静态文件。每次发布附带五种桌面构建与 WASM/Web 的批跑耗时图、原始数据和伤害校验值。[Web 构建与使用](apps/web/README.md) · [引擎接口](docs/engine_api.md)

## For Developer
1. git代码仓
```git
git clone --recurse-submodules git@github.com:Nowaterisenough/JX3DPS.git
```
2. CMake编译

桌面版可选用 GCC Release 的 PGO。运行 `python tools/desktop_pgo.py` 自动构建、训练并生成优化版，加上 `--benchmark` 可以与普通 Release 逐项对照。[桌面 PGO 构建与验证](docs/desktop_pgo.md)

