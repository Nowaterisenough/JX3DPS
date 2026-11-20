# JX3DPS Web 工作台

网页通过 Web Worker 加载 `jx3dps.mjs` 与 `jx3dps.wasm`。普通批跑、单步调试和断点调用 `include/jx3dps.h` 的会话接口。每场使用与原生版相同的随机种子派生、心法规则和伤害结算，校验值按十进制字符串传递。四种预设支持修改宏、战斗时长、场数、种子与基础属性。当前 Web 批跑使用一个 Worker，按每 128 场报告进度。

## 本地构建

需要 Emscripten SDK（本次验证 7.0.0）、CMake 3.26+ 和 Ninja。先初始化 JSON 子模块并激活 SDK 环境，再从仓库根目录运行：

```sh
git submodule update --init 3rd/json
emcmake cmake -S . -B build/web -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/web --target jx3dps_web
python -m http.server 8080 --bind 127.0.0.1 --directory build/web/out
```

打开 http://127.0.0.1:8080/ 。整个 `build/web/out/` 可以部署到静态服务器，也支持子路径。服务器应为 `.wasm` 返回 `application/wasm`，为 `.mjs` 返回 JavaScript MIME。Worker 在页面内运行，无需后台计算服务。

## 源码

- `index.html`、`ui/` 提供参数、宏编辑、运行记录、调试控件和图表。
- `wasm/module.cpp` 和本目录 CMake 构建共用引擎的 WASM 发布包。
- `src/engine/`、`src/core/runtime/` 和 `src/class/` 提供跨端接口、事件调度及心法逻辑。
