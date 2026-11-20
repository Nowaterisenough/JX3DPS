# JX3DPS 引擎接口

原生库目标为 `JX3DPS`。C 调用者只需包含 `include/jx3dps.h` 并链接该目标；WASM 从同一目标导出相同符号。默认输出静态库；原生构建加 `-DJX3DPS_SHARED_ENGINE=ON` 可生成 DLL、dylib 或 so。ABI 版本为 1。旧版依赖全局结果的 `jx3dps_simulate(...)` 等可变参数接口已经移除，旧调用方需要重新编译并改用会话句柄。[C 调用示例](../example/example_api.c)

## 生命周期

1. `jx3dps_create()` 创建独立会话，分配失败返回空指针。
2. `jx3dps_call(handle, request)` 接收以零结尾的 UTF-8 JSON 并返回 UTF-8 JSON。出现错误时读取字符串字段 `error`。返回内存归会话所有，在下一次调用或销毁前复制需要保留的结果。
3. 不同句柄可以同时运行。对单个句柄串行调用 `call`。在原生批跑期间，另一线程可以读取 `jx3dps_completed` 或调用 `jx3dps_cancel`。取消后创建新句柄再执行；工作线程退出后才可销毁。
4. `jx3dps_destroy(handle)` 释放会话、日志、宏程序和返回字符串。

## Web 与原生均可用的请求

`{"op":"defaults"}` 返回太虚和莫问的默认宏。以下请求设置一个批次：

```json
{"op":"start","scenario":"tx","macro":"/cast 三柴剑法","seconds":300,"iterations":1000,"seed":"42","attributes":{}}
```

- `scenario` 取 `tx`、`tx_buffs`、`mw` 或 `mw_equipped`。内核以 16 帧/秒推进。种子使用无符号 64 位整数的十进制字符串，包含最大值的种子也能由 JavaScript 精确传递。
- `start` 支持 1～1800 秒、1～100000 场和最多 64 KiB 的宏。可选 `attributes` 可覆盖 `attack`、`crit`、`haste`。每场由基础种子和迭代号独立派生。
- 循环调用 `{"op":"next"}`，每次最多推进 128 场。最后一个响应中包含 `skills`、`meanDps`、`sdDps`、最低和最高伤害、24 桶 DPS 分布及 `checksum`。伤害总量和校验值按十进制字符串返回。
- 在 `start` 增加 `"debug":true,"breakpoints":[2,4]`，即可用同一批次的第一个种子进入调试。后续调用 `{"op":"debug","action":"into"}`，或将 action 设为 `over`、`continue`。响应含完成状态、当前帧、源行、事件数及结束时的总伤害。继续执行有单次步数上限，可再次调用。

## 原生多线程批跑

```json
{"op":"batch","scenario":"mw","macro":"/cast 变徵","frames":4800,"iterations":100000,"workers":4,"seed":"42","collectSkills":true}
```

返回 `stats` 与 `skills`。统计字段包括 count、mean_damage、m2_damage、min_damage、max_damage 和字符串 checksum。各技能返回总伤害、次数、会心、命中、识破和偏离。可选 `increments` 接收八项正整数增量，按攻击、会心、会效、破防、无双、破招、加速、武器的顺序返回 gains；`hotspots:true` 附带分阶段耗时、合并调用树和线程样本数。用户配置在 C++ 客户端由 `src/engine/config_json.hpp` 编解码，保留装备、奇穴、秘籍、团队及目标时间线字段。

CLI 普通批跑及桌面批跑、收益、热点走这个 C 接口。原生客户端的交互式调试和逐事件历史视图还直接消费同一内核的 C++ 类型，以取得完整状态和断点观察记录。Web 调试通过会话接口调用同一调试器。

## Release 性能图

Release 正文先列出相对上一个已发布正式版本的 CHANGELOG。它按提交类型分类并统计数量，同一分类、作用域和标题的提交合并展示，保留每位作者和每个提交的链接。首次发布覆盖当前提交可达的历史，合并提交不重复计数。

桌面矩阵在每个实际工作台可执行文件上运行普通批跑，Web 作业通过 Chromium Worker 载入发行用的 WASM 文件。各自使用 1 个工作线程、42 号种子和 300 秒战斗，四种预设各预热 256 场后再运行三轮 10 万场。计时包含批跑和结果汇总，排除浏览器或桌面进程的启动。发布作业核对构建集合、提交、参数、总伤害和校验值后生成 PNG，写入 Release 正文，并上传包含每轮秒数和硬件信息的 JSON。
