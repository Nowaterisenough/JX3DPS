# JX3DPS

[![Desktop CI](https://github.com/Nowaterisenough/JX3DPS/actions/workflows/desktop.yml/badge.svg?branch=develop)](https://github.com/Nowaterisenough/JX3DPS/actions/workflows/desktop.yml)

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


## 代码与入口

| 目录 | 职责 |
|---|---|
| `include/jx3dps.h`、`src/engine/` | 会话式 C ABI、配置解析和批次结果 |
| `src/core/runtime/` | 宏编译、模拟、事件队列、记录、批跑与属性收益 |
| `src/class/{tai_xu_jian_yi,mo_wen}/` | 两种心法的 `skills.hpp` 技能与 Buff 目录、`runtime_data.hpp` 配置及系数、`runtime_rules.hpp` 战斗行为 |
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

