# Rust 与 C++ 战斗性能对照

## 四配置独立 Rust 整场

`full/main.rs` 是独立 Rust 程序的入口。它解析宏文本，生成随机数，调度同帧事件，执行太虚、莫问的技能、天赋和装备规则，记录完整状态变更，并按快照和实时属性计算逐击伤害。`full/catalog.rs` 是从游戏数据提取的只读技能系数和定时器描述。莫问伤害表由 Rust 的公式在启动时生成。程序只需要 Rust 标准库，可单独编译，无需构建任何 C++ 库。

```powershell
rustc --edition 2024 -C opt-level=3 -C codegen-units=1 -C panic=abort performance/rust_battle/full/main.rs -o full-rust.exe
.\full-rust.exe mw_equipped 100000 20
```

四种场景名为 `tx`、`tx_buffs`、`mw` 和 `mw_equipped`。运行一次可生成二进制逐事件记录，例如 `full-rust.exe tx_buffs 1 1 audit.bin tests/performance/pgo_holdout/tx_sequence.txt`。支持范围是这四套固定配装和仓库验收宏涉及的技能、条件及指令。每个场景都保留顺序施放、冷却检查和事件驱动的宏唤醒。对比基准使用已有的 GCC Release 通用引擎，原生 C++ 和纯 Rust 执行相同宏。

```powershell
$bench = '.cache/rust-battle'
python performance/rust_battle/build.py $bench
python performance/rust_battle/full/compare.py "$bench/battle.exe" "$bench/full-rust.exe" "$bench/full-results"
```

脚本核对默认宏各 512 场，以及 13 组验收宏各 64 场的逐字节轨迹，再交替计时十轮。单线程每轮 8192 场，20 线程每轮 10 万场，统计内容包括各技能伤害、命中类型和批次校验值。宏编译和共享只读数据的准备在计时前完成。输出目录包含逐轮秒数、逐技能校验数据和完整轨迹 SHA-256。

## 固定宏和跨语言接口对照

实验使用 Windows x86-64 GNU 工具链。构建需要 Python 3.11+、支持 C++26 的 GCC，以及支持 Rust 2024 edition 的 rustc。三个基准都从迭代号派生同一随机种子，每场模拟 4800 帧，单线程固定在逻辑 CPU 2。编译使用最高常规优化级别，关闭 PGO 和性能采样。

| 可执行程序 | 场景与计时范围 |
|---|---|
| `tx-rust.exe` | 独立的纯 Rust 太虚默认战斗，固定六行宏和无天赋、装备、团队增益的配置。Rust 执行全部事件、施放、随机判定、写日志、伤害结算和批跑。构建时从项目导出只读的公式输入、两个属性配置及定时器定义。 |
| `tx-cpp.exe` | 同样固定太虚默认宏和配置的 C++ 对照，复用项目的队列、日志和伤害公式。 |
| `battle.exe native` | 项目的通用 C++ Simulation，支持太虚默认、太虚团队、莫问默认和莫问多装备四组真实配置。 |
| `battle.exe rust` | Rust 实现战斗推进、同帧队列、宏扫描、条件比较和跳转。现有 C++ 提供宏编译、心法规则、状态与日志、伤害公式及批跑统计。 |
| `battle.exe control` | 经过相同 C ABI 回调的 C++ 引擎对照，与 Rust 引擎使用同一份心法与统计代码。 |
| `queue-rust.exe`、`queue-cpp.exe` | 两个独立程序，读取实际战斗中的定时器操作。预加载之后计时，执行重设、取消、推进和到期派发，逐次核对返回顺序。 |

在仓库根目录运行以下 PowerShell 命令。可用环境变量 `RUSTC` 和 `CXX` 指定编译器。基准需依次运行，以免相互争用 CPU。每次测量使用新的结果目录。

```powershell
$bench = '.cache/rust-battle'
python performance/rust_battle/build.py $bench
python performance/rust_battle/compare_default.py $bench "$bench/results"
python performance/rust_battle/compare.py "$bench/battle.exe" "$bench/results"
python performance/rust_battle/compare_queue.py $bench "$bench/results"
```

太虚专用对照先逐字节核对 512 场战斗的完整轨迹，再对三个程序交替计时 12 轮。四配置对照先比较四组默认宏各 128 场，以及长条件链、顺序施放、强制打断和 DOT 验收宏共 13 个配置组合、每组 16 场，再对三个引擎交替计时六轮。轨迹记录同帧回调顺序、成功的施放与宏行、逐击伤害和状态变更的所有字段。每个计时批次还比较总伤害校验值及各技能的伤害、命中、会心、识破和偏离次数。

单线程每轮 8192 场，20 线程每轮 10 万场。计时包含工作线程创建、战斗、结算和技能统计汇总。宏编译、进程启动、导表和 Qt 界面均在计时范围之外。队列实验各配置提取 64 场的操作，单核每轮重复回放 64 遍，八轮交替运行。
