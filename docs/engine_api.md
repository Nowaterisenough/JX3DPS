# JX3DPS 引擎接口

原生库目标为 `JX3DPS`。C 调用者只需包含 `include/jx3dps.h` 并链接该目标；WASM 从同一目标导出相同符号。默认输出静态库；原生构建加 `-DJX3DPS_SHARED_ENGINE=ON` 可生成 DLL、dylib 或 so。ABI 版本为 1。旧版依赖全局结果的 `jx3dps_simulate(...)` 等可变参数接口已经移除，旧调用方需要重新编译并改用会话句柄。[C 调用示例](../example/example_api.c)

## 生命周期

查询 `{"op":"schools"}` 可取得 50 级心法的代号、可施放招式、资源名、可用奇穴、秘籍、装备选项、默认宏、整数面板（`attributes`）、已选奇穴（`defaultTalents`）和秘籍（`defaultRecipes`）。将代号作为 `scenario` 即可通过同一会话执行或调试。[50 级配置与逐帧记录](season50_runtime.md)

1. `jx3dps_create()` 创建独立会话，分配失败返回空指针。
2. `jx3dps_call(handle, request)` 接收以零结尾的 UTF-8 JSON 并返回 UTF-8 JSON。出现错误时读取字符串字段 `error`。返回内存归会话所有，在下一次调用或销毁前复制需要保留的结果。
3. 不同句柄可以同时运行。对单个句柄串行调用 `call`。在原生批跑期间，另一线程可以读取 `jx3dps_completed` 或调用 `jx3dps_cancel`。取消后创建新句柄再执行；工作线程退出后才可销毁。
4. `jx3dps_destroy(handle)` 释放会话、日志、宏程序和返回字符串。

50 级分山默认预铺二档月照、满怒起手。凌海的默认起手保留战前撑伞、游仙与抟空，无方五寒起手，笑尘以三醉意、昊破和触类起手，易筋预铺金刚日轮。毒经预设提前一帧挂凤凰蛊，冰心以五剑舞和化冰起手。`config.precombat=false` 可让这些心法从无预铺的基础开场起步。原生 `batch` 的技能条目同时返回 `skill` 槽位和 `name`；`debug/trace` 的 `initial` 给出首条宏前的资源和气劲，`changes` 按事件顺序记录资源变动、技能就绪帧、充能及公共调息组。

50 级规则的 `schools` 响应还包含 `permanents`（按类别列出的可选名称）、`defaultEquipment` 和 `equipmentSlots`。同一类别只可选一项，同部位武器或腕、鞋的两档效果互斥。食物、药品、阵法默认均为空。可以在命令行的 `--config` 文件或 C API 的 `config` 中传入以下对象。省略 `base_major` 时，主属性加成以该心法的默认主属性为基数，自定义或导入面板时可以显式传入自己的主属性点数。

```json
{"permanents":{"药品增强":"巴山凝神丹·精（会心)","阵法":"刀宗阵"}}
```

## Web 与原生均可用的请求

`{"op":"defaults"}` 按心法代号返回 20 个旗舰心法的默认宏。以下请求设置一个批次：

```json
{"op":"start","scenario":"tx","macro":"/cast 三柴剑法","seconds":300,"iterations":1000,"seed":"42","attributes":{}}
```

- `scenario` 取 `schools` 返回的旗舰心法代号，太虚用 `tx`，莫问用 `mw`。内核以 16 帧/秒推进。种子使用无符号 64 位整数的十进制字符串，包含最大值的种子也能由 JavaScript 精确传递。
- `start` 支持 1～1800 秒、1～100000 场和最多 64 KiB 的宏。50 级场景省略 `macro` 时使用该心法的魔盒适配宏，`config.attributes` 按字段覆盖该心法预设。顶层 `attributes` 可覆盖 `attack`、`crit`、`haste`。每场由基础种子和迭代号独立派生。
- 循环调用 `{"op":"next"}`，每次最多推进 128 场。最后一个响应中包含 `skills`、`meanDps`、`sdDps`、最低和最高伤害、24 桶 DPS 分布及 `checksum`。伤害总量和校验值按十进制字符串返回。
- 在 `start` 增加 `"debug":true,"breakpoints":[2,4]`，即可用同一批次的第一个种子进入调试。后续调用 `{"op":"debug","action":"into"}`，或将 action 设为 `over`、`continue`。响应含完成状态、当前帧、源行、事件数及结束时的总伤害。继续执行有单次步数上限，可再次调用。

## 原生多线程批跑

```json
{"op":"batch","scenario":"mw","frames":4800,"iterations":100000,"workers":4,"seed":"42","collectSkills":true}
```

返回 `stats` 与 `skills`。统计字段包括 count、mean_damage、m2_damage、min_damage、max_damage 和字符串 checksum。各技能返回总伤害、次数、会心、命中、识破和偏离。可选 `increments` 接收八项正整数增量，按攻击、会心、会效、破防、无双、破招、加速、武器的顺序返回 gains；`hotspots:true` 附带分阶段耗时、合并调用树和线程样本数。用户配置在 C++ 客户端由 `src/engine/config_json.hpp` 编解码，保留装备、奇穴、秘籍、团队及目标时间线字段。

CLI 普通批跑及桌面批跑、收益、热点走这个 C 接口。原生客户端的交互式调试和逐事件历史视图还直接消费同一内核的 C++ 类型，以取得完整状态和断点观察记录。Web 调试通过会话接口调用同一调试器。

## Release 性能图

Release 正文先列出相对上一个已发布正式版本的 CHANGELOG。它按提交类型分类并统计数量，同一分类、作用域和标题的提交合并展示，保留每位作者和每个提交的链接。首次发布覆盖当前提交可达的历史，合并提交不重复计数。

桌面矩阵在每个实际工作台可执行文件上运行普通批跑，Web 作业通过 Chromium Worker 载入发行用的 WASM 文件。所有心法都使用自己的默认宏和配置，按 1 个工作线程、42 号种子、每场战斗 300 秒运行。每个心法先预热 256 场，再测三轮，每轮 10 万场。计时包含批跑和结果汇总，排除浏览器或桌面进程的启动。发布作业核对构建集合、提交、参数、总伤害和校验值，上传包含每轮秒数和硬件信息的 `performance.json`。

每种构建各有一张跨版本柱状图，使用与 CHANGELOG 相同的上一正式版，从两版的发行程序耗时取三轮中位数。柱尾是秒数，右栏是相对上版的变化百分比，负值表示耗时缩短。正文标出两次测量的 CPU、运行器、编译器或浏览器变化。首次发布、上版缺少数据或测试参数变更时，图上只绘制可用的耗时，变化栏显示「—」。

50 级配置支持 `{"raid_full":true}` 开启本心法「开荒拉满」；`schools[].raidPermanents` 给出该心法的默认阵眼和消耗品，`config.permanents` 中手选的同类选项优先。逐云寒蕊默认使用该心法预设装分，可用非负整数 `config.gear_score` 覆盖。属性收益沿用原有 `batch` 和 `increments` 接口。[副本时间轴与重算结果](season50_pve_attribute_audit.md#副本开荒拉满)
