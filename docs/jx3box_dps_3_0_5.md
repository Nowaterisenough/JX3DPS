# 魔盒收录的在线 DPS 计算器：苍生铸世 3.0.5 技能与 Buff 分析

核对日期为 **2026 年 11 月 20 日**。魔盒的 DPS 页面按心法列出计算器并跳转到作者作品。本报告分析作品 79885「在线配装计算器」及其生产站 `dps.btcsg.top`。从当日页面的 Webpack 资源清单读取了 **18 个旗舰心法和 5 个无界心法**，并检查各模块导出的技能系数、增益和模拟器状态。[魔盒收录接口](https://cms.jx3box.com/api/cms/bps/dps/registry?client=std) · [作者发布页](https://www.jx3box.com/bps/79885) · [线上资源清单](https://dps.btcsg.top/static/js/runtime.f81ceb48.js)

**按技能查询**可使用同目录的 [3.0.5 技能与 Buff 索引](jx3box_dps_3_0_5_index.json)。其中有 **1,077 条伤害事件定义、1,594 条按心法去重的增益定义、473 条模拟器初始状态定义**。伤害事件包含同一招的不同段、宠物、装备和公共伤害，数量不能当成玩家的可施放技能数。每个模块均附实际线上文件地址和 SHA-256。

## 版本从哪里确定

| 日期 | 正式服与计算器的变化 | 分析时的含义 |
|---|---|---|
| 11 月 8 日 | 作者公开 Git 仓库最后提交为 `54d8ccd`，更新日志首项为 3.0.2。 | TypeScript 便于阅读共同架构，11 月中旬后的数值和触发行为以线上发布物为准。[公开源码日志](https://github.com/jx3dps-online/jx3dps-online-public/blob/54d8ccdb7528ba2b305a31f77c5710152d37bd9b/src/更新日志.tsx) |
| 11 月 12 日 | 正式服 1.6.0.10080 实装武学调整。计算器 3.0.3 更新非侠、神兵、部分奇穴及预设循环。 | 需要分别检查奇穴描述、伤害系数、Buff 属性和模拟器事件。[正式服武学调整](https://jx3.xoyo.com/index/index.html#/article-details?kid=1335924) · [计算器版本记录](https://dps.btcsg.top/static/js/3723.484b287d.chunk.js) |
| 11 月 16 日 | 3.0.4 修改孤锋弱点检测间隔（4 帧改 5 帧）、移除模拟器触石雨、重新生成紫武和橙武循环；笑尘起手醉意改为 3，无方更新 JCL 解析。 | 使用老循环配新系数仍可能算出不同结果。[计算器版本记录](https://dps.btcsg.top/static/js/3723.484b287d.chunk.js) |
| 11 月 19 日 | 3.0.5 同步武器与腰坠无双，50 级橙武「神兵·无双」五档每层改为 57、64、72、77、84。正式服 1.6.0.10090 同日还有武学修复及绝境战场专用气劲调整。 | 木桩模型应取常规 PvE 条目。装备更新可由计算器版本记录确认，游戏修复是否进入指定模拟路径，须逐条找执行证据。[正式服更新](https://jx3.xoyo.com/index/index.html#/article-details?kid=1335936) · [计算器版本记录](https://dps.btcsg.top/static/js/3723.484b287d.chunk.js) |

## 一次技能如何变成 DPS

1. **先确定本击有没有发生。** 循环模拟器按帧处理读条、命中、后续伤害和到期事件，每秒 16 帧。自身与目标分别有状态表。每个心法自行决定何时加层、刷新、扣层、取消后续跳数、生成追击，或改变资源和调息。一个状态可以只改变下一招是否可用，完全没有直接增伤字段。[线上孤锋状态表](https://dps.btcsg.top/static/js/6668.60222904.chunk.js) · [线上孤锋执行代码](https://dps.btcsg.top/static/js/908.c8a2109c.chunk.js)
2. **为实际发生的伤害记录当时的标签。** 单次施放可以产生本体、DOT、武器伤害或神兵等多个事件。循环数据按伤害名称、等级、DOT 跳数、伤害层数和增益组合保存次数。预设循环或 JCL 已提供次数时，结算器直接消费这些分组。比如线上孤锋「紫武_长短轴」的一档记录有 268 次孤锋破浪，其中一个含流岚等快照的组合出现 4 次，另一个含灭影追风等标签的组合出现 10 次。这两个数字是不同组合的次数，不能解释成某个 Buff 的总覆盖次数。[线上孤锋预设](https://dps.btcsg.top/static/js/xinfa-gfj.6c2cb459.chunk.js)
3. **筛选本技能能吃的增益。** 秘籍、奇穴、装备和团队配置先决定哪些规则可用。已启用的「全局启用」作用于该技能的所有次数；「部分启用」需匹配该组伤害携带的增益名称，还会按规则检查奇穴、装备、阵眼或团队条件。命中同名字符串不保证加成生效。某些装备、团队效果已有快照时会跳过相应均摊项，避免双计。[线上伤害模块](https://dps.btcsg.top/static/js/dps.df38050f.chunk.js) · [可读源码：增益筛选](https://github.com/jx3dps-online/jx3dps-online-public/blob/54d8ccdb7528ba2b305a31f77c5710152d37bd9b/src/计算模块/统一工具函数/增益计算函数.ts)
4. **按组计算期望，再除以战斗时间。** 结算先应用装备和团队基础加成，再应用技能常驻增益以及快照组的共同增益，最后应用各组的独有增益。没有落入快照组的剩余次数只用其应有的常驻项。在默认郭氏路径中，会心与普通命中按期望合并，最终 DPS 对总伤除以秒数取整。固定循环内改变会心率，通常改变单次期望；毒经食髓等显式分支还会改变发生次数。[线上结算](https://dps.btcsg.top/static/js/dps.df38050f.chunk.js) · [可读源码：循环秒伤](https://github.com/jx3dps-online/jx3dps-online-public/blob/54d8ccdb7528ba2b305a31f77c5710152d37bd9b/src/计算模块/循环秒伤计算.ts) · [可读源码：默认入口](https://github.com/jx3dps-online/jx3dps-online-public/blob/54d8ccdb7528ba2b305a31f77c5710152d37bd9b/src/计算模块/执行秒伤计算.ts)

**数值单位依字段而定。** 无方非侠在属性表已经存成 `379/1024 ≈ 0.370117`，而天罗千机之威的 `atAllShieldIgnorePercent` 存整数 `389`，由对应公式按 1024 换算。直接把所有原始数统一除以 1024，会把前者重复缩小。会心增加 5 个百分点、基础攻击提高 5%、最终伤害提高 5% 也会进入不同计算环节。[无方属性定义](https://dps.btcsg.top/static/js/xinfa-wf.f00fd058.chunk.js) · [天罗属性定义](https://dps.btcsg.top/static/js/xinfa-tlgd.4e1efa98.chunk.js)

## 技改中能直接复核的数值

| 心法 | 技改前 → 线上 3.0.5 | 读取位置 |
|---|---|---|
| 天罗诡道 | 千机之威无视防御 50% → 约 38%（389/1024）；神兵·暴雨梨花针系数 347 → 191；千秋万劫及飞天变体系数 1136 → 378。正式服另规定千秋分三段，每段最多结算 8 跳化血，只有系数不足以证明三段调度正确。 | [技能与增益](https://dps.btcsg.top/static/js/xinfa-tlgd.4e1efa98.chunk.js) · [正式服规则](https://jx3.xoyo.com/index/index.html#/article-details?kid=1335924) |
| 无方 | 非侠 297 → 379（分母 1024）；鉴魄证身系数 440 → 1276。后者是橙武独立伤害事件。 | [技能与增益](https://dps.btcsg.top/static/js/xinfa-wf.f00fd058.chunk.js) · [触发路径](https://dps.btcsg.top/static/js/6098.41c3b223.chunk.js) |
| 分山劲 | 非侠 174 → 154（分母 1024）。孤旌层数的实装差异见下节。 | [技能与增益](https://dps.btcsg.top/static/js/xinfa-fsj.456d72a5.chunk.js) |
| 北傲诀 | 非侠 195 → 174；醉斩白蛇的橙武**增伤项** 2048 → 1485（两者分母均为 1024）。线上醉斩本体技能系数另为 191。 | [技能与增益](https://dps.btcsg.top/static/js/xinfa-baj.9fbaa204.chunk.js) |
| 山海心诀 | 饮羽簇在橙武窗口追加的月弦激星系数 531 → 744。 | [系数](https://dps.btcsg.top/static/js/xinfa-shxj.f62bb0eb.chunk.js) · [追加判定](https://dps.btcsg.top/static/js/4292.90440d17.chunk.js) |
| 笑尘诀 | 苍龙系数 649 → 584；苍龙戾天 2450 → 2205。两个伤害名分别取值。 | [技能与增益](https://dps.btcsg.top/static/js/xinfa-xcj.cf5365d6.chunk.js) |
| 周天功、幽罗引 | 非侠分别由 246 → 266、532 → 553（分母 1024）。 | [周天功](https://dps.btcsg.top/static/js/xinfa-ztg.55989a4c.chunk.js) · [幽罗引](https://dps.btcsg.top/static/js/xinfa-yly.65b62ff2.chunk.js) |

表中旧值取自作者 3.0.3 变更记录，新值另查 3.0.5 线上模块。下节的给定层数和时间仅指明所核对的具体路径。[计算器版本记录](https://dps.btcsg.top/static/js/3723.484b287d.chunk.js)

## 公告与运行代码存在的差异

- **分山劲的「战绝」窗口仍用旧增伤。** 11 月 12 日公告将初始 27%、每 3 秒衰减 9 个百分点改成初始 18%、每次衰减 6 个百分点。线上施放偃守孤旌仍写入三层「孤旌」，伤害时按层选 `孤旌增伤·1/2/3`，每层取 `92/1024`。因此斩刀等命中这一路径时，三档约为 **27%、18%、9%**，公告应为 **18%、12%、6%**。只改非侠乘区无法纠正窗口内的偏差。[正式服规则](https://jx3.xoyo.com/index/index.html#/article-details?kid=1335924) · [线上加层与扣层](https://dps.btcsg.top/static/js/7390.b2924503.chunk.js) · [线上层数系数](https://dps.btcsg.top/static/js/xinfa-fsj.456d72a5.chunk.js)
- **孤锋诀的「威声」需区分两个时窗。** 公告保留原有 2 秒机制，另加灭影追风后 12 秒内下两次孤锋破浪会心提高 15 个百分点。线上奇穴描述已有新文案，查到的模拟器威声状态仍为一层、32 帧；灭影加一层，孤锋释放后扣一层。孤锋伤害表里的「灭影追风」会心为 10 个百分点，也没有独立的「威声」增益项。在所查路径中未找到新两次会心效果，不能把新版描述当作该效果已经参与 DPS 的证据。[正式服规则](https://jx3.xoyo.com/index/index.html#/article-details?kid=1335924) · [上线状态](https://dps.btcsg.top/static/js/6668.60222904.chunk.js) · [灭影处理](https://dps.btcsg.top/static/js/3297.24109b43.chunk.js) · [孤锋处理](https://dps.btcsg.top/static/js/908.c8a2109c.chunk.js) · [伤害定义](https://dps.btcsg.top/static/js/xinfa-gfj.6c2cb459.chunk.js)
- **太玄经的间隔有代码证据，11 月 19 日的修复需另验证。** 线上自动三才间隔常量已是 2 秒，队列按该间隔排五次，与 11 月 12 日的间隔调整一致。11 月 19 日官方修复了踏斗、连极阵自动撞击在特定条件下丢伤害和知微的问题。没有该条件下修复前后的实战记录，本次只能确认事件间隔和知微状态定义。[线上排程](https://dps.btcsg.top/static/js/4101.3a14ed1e.chunk.js) · [知微定义](https://dps.btcsg.top/static/js/2639.07ec769d.chunk.js) · [11 月 19 日公告](https://jx3.xoyo.com/index/index.html#/article-details?kid=1335936)

## 旗舰端 18 个心法

| 心法 | 技能触发与状态变化 | 怎样进入 DPS | 3.0.5 代码 |
|---|---|---|---|
| 傲血战意 | **龙牙**。骑乘、有龙驭奇穴且身上有龙驭时，这一击带龙驭标签，随后扣一层。若夜征生效且扣层后还有龙驭，再打夜征并再扣一层。站立命中走获得龙驭的分支。 | 龙驭记在龙牙的易伤乘区，值约 45%。有龙血奇穴且目标存在流血时，另记一次「龙牙·龙血」。 | [执行](https://dps.btcsg.top/static/js/9657.ca89b925.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-axzy.a5e659e3.chunk.js) · [状态](https://dps.btcsg.top/static/js/8379.e76ea08d.chunk.js) |
| 北傲诀 | **刀啸风吟**。命中扣气劲，读条结束才读取蓄势层数登记刀啸伤害。有绝期奇穴时，在此次伤害之后加绝期。 | 蓄势最高五层，基础时长 30 秒，每层通用增伤 82/1024。醉斩白蛇的橙武增伤独立取 1485/1024。 | [执行](https://dps.btcsg.top/static/js/531.fbdeb577.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-baj.9fbaa204.chunk.js) · [状态](https://dps.btcsg.top/static/js/6305.3165156b.chunk.js) |
| 冰心诀 | **江海凝光**。先登记江海凝光，再扣一层满堂、消耗三层盈袖并调用急曲引爆。这样本击能吃到消耗前的满堂，后续事件再读取新层数。 | 满堂每层给 8 个百分点内功会心，另给 21 点郭氏会效。急曲自然跳和引爆有各自的伤害条目。 | [执行](https://dps.btcsg.top/static/js/3814.a0cf34b6.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-bxj.96c14460.chunk.js) · [状态](https://dps.btcsg.top/static/js/4060.aa8fa10f.chunk.js) |
| 毒经 | **幻击·食髓**。循环结算对这项单独递推次数。取得会心概率 p 后，按 6 秒和 12 秒两种后继触发时间计算期望，再将次数按 12/30、18/30 分配给带与不带灵蛇献祭标签的两组。 | 嗜蛊同时涉及无视防御、无双、内功攻击，定义另有常驻和快照分项。11 月 12 日另调了怀泽蛊的泽靡；在本心法的独立伤害事件表中未找到名为「泽靡」的条目。 | [执行](https://dps.btcsg.top/static/js/dps.df38050f.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-dj.dfc87bbd.chunk.js) · [状态](https://dps.btcsg.top/static/js/6718.9e290ad9.chunk.js) |
| 分山劲 | **偃守孤旌 → 斩刀**。施放获得战绝和三层孤旌，回怒事件逐次消去一层孤旌，伤害事件写入当时的层数。战绝和孤旌的初始模板都是 9 秒。 | 非侠已更新为 154/1024。斩刀的孤旌增伤仍按每层 92/1024，三层约 27%，与 11 月 12 日公告不符。 | [执行](https://dps.btcsg.top/static/js/7390.b2924503.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-fsj.456d72a5.chunk.js) · [状态](https://dps.btcsg.top/static/js/5312.6837f299.chunk.js) |
| 焚影圣诀 | **净世破魔击**。模拟器依日灵、月灵选取伤害分支，消耗满灵并处理回灵；净世的日、月伤害和相应武器伤害分别登记。 | 日破的明光·日增加内功攻击，取 184/1024。统计时要保留日、月事件名，才能命中各自的增益表。 | [执行](https://dps.btcsg.top/static/js/1638.10850794.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-fysj.1e54bd4b.chunk.js) · [状态](https://dps.btcsg.top/static/js/679.0fc24f55.chunk.js) |
| 孤锋诀 | **孤锋破浪**。先打本次孤锋、扣一层橙武孤锋，再按目标长溯层数逐次追加孤锋，每一段都走同一套扣层逻辑，最后清空长溯。 | 橙武孤锋对相应段增加 1331/1024 通用伤害。11 月 16 日另修弱点检测并重生成默认循环；威声的新会心窗口见下文。 | [执行](https://dps.btcsg.top/static/js/908.c8a2109c.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-gfj.6c2cb459.chunk.js) · [状态](https://dps.btcsg.top/static/js/6668.60222904.chunk.js) |
| 花间游 | **玉石俱焚 → 商阳指**。玉石先统计钟林、商阳、兰摧还有哪几种 DOT，再调用各自的引爆。商阳用剩余跳数乘当前层数，在引爆时刻登记伤害，带上已保存的 DOT 快照，随后清空待生效数据。 | 基础商阳状态模板为每 48 帧一跳，共 6 跳。流离减玉石调息按尚存 DOT 的种类数计算，每种 1.25 秒。 | [执行](https://dps.btcsg.top/static/js/5688.24b92ae2.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-hjy.a6564356.chunk.js) · [状态](https://dps.btcsg.top/static/js/5373.0963c135.chunk.js) |
| 凌海诀 | **海运南冥**。命中先判断海碧、抟空，并消耗已有的抟空；满足条件时减调息或标记重置。随后按鸿轨判定登记近段伤害，并处理青冥引爆。 | 远、近是不同伤害事件。游仙对海运远段可提供 307/1024 外功攻击，游仙状态模板为 15 秒。 | [执行](https://dps.btcsg.top/static/js/4637.ca292c0d.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-lhj.eb0651eb.chunk.js) · [状态](https://dps.btcsg.top/static/js/3683.f4ded893.chunk.js) |
| 山海心诀 | **饮羽簇 → 月弦激星**。饮羽结算时先记本体；此时身上有橙武才再记一笔月弦激星。佩弦或橙武还参与是否读条的判断。 | 技改后的 744 是月弦激星的技能系数。饮羽本体另可吃乘黄之威的 5 个百分点外功会心和 51 点郭氏会效。 | [执行](https://dps.btcsg.top/static/js/4292.90440d17.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-shxj.f62bb0eb.chunk.js) · [状态](https://dps.btcsg.top/static/js/2433.cc636346.chunk.js) |
| 太玄经 | **连极阵 → 纵横三才**。满足灯存在判定后，创建自动三才，先调一次触发函数，再排入后续五个事件。线上间隔常量已改为 2 秒。 | 知微作为单独的目标 DOT 管理，模板最多两层、15 跳。11 月 19 日官方另修部分情形漏撞及漏知微，当前线上版本日志未单列这个修复。 | [执行](https://dps.btcsg.top/static/js/4101.3a14ed1e.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-txj.c6136bb2.chunk.js) · [状态](https://dps.btcsg.top/static/js/2639.07ec769d.chunk.js) |
| 天罗诡道 | **鬼斧神工、千机之威**。鬼斧读条命中添加扬威和 15 层鬼斧弹药；有波诡云谲再给 9 层备弹。千机之威在伤害表拆为完整、常驻、快照三种记录。 | 完整条目给 389/1024 无视防御和 154/1024 基础攻击。千秋万劫及飞天变体的单段系数均为 378，神兵暴雨额外段为 191。 | [执行](https://dps.btcsg.top/static/js/3402.fc3d0371.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-tlgd.4e1efa98.chunk.js) · [状态](https://dps.btcsg.top/static/js/56.196d2e07.chunk.js) |
| 无方 | **养荣、鉴魄证身**。以川乌射罔为例，养荣全量含内功破防与无双，常驻分项只含破防，快照分项只含无双。橙武触发路径另记鉴魄证身。 | 养荣两项各为 256/1024。非侠为 379/1024；技改后的 1276 是鉴魄证身的系数，商陆缀寒·神兵另有独立条目。 | [执行](https://dps.btcsg.top/static/js/6098.41c3b223.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-wf.f00fd058.chunk.js) · [状态](https://dps.btcsg.top/static/js/2457.5c572b78.chunk.js) |
| 笑尘诀 | **龙跃于渊、苍龙**。龙跃命中时扣内力并给目标挂灼烧；苍龙触发函数还要检查自身苍龙状态。灼烧、苍龙和苍龙戾天各自登记。 | 苍龙、苍龙戾天系数分别为 584、2205。3.0.4 将模拟器起手醉意默认值改为 3，复算循环时起手值也要相同。 | [执行](https://dps.btcsg.top/static/js/805.ea61c8bf.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-xcj.cf5365d6.chunk.js) · [状态](https://dps.btcsg.top/static/js/8543.905d57d4.chunk.js) |
| 易筋经 | **拿云式**。按禅那选择段位，再检查目标斩杀状态和净果奇穴，满足时选用斩杀·净果伤害事件。技能武器伤害另外登记；命中后还要处理禅那及贪破。 | 3.0.3 新增全程斩杀预设。相同面板换到该循环，拿云事件的构成便可能改变。 | [执行](https://dps.btcsg.top/static/js/8701.176442df.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-yjj.c26e5e07.chunk.js) · [状态](https://dps.btcsg.top/static/js/9518.b27efea8.chunk.js) |
| 幽罗引 | **勾线 → 缠绞**。消耗心络并排入引导事件。每次引导伤害先记勾线，再给目标叠一层缠绞，因此刚增加的一层供后续伤害读取。 | 缠绞最多五层，基础时长 12 秒。勾线·伶影对应五层易伤为 25%；心法非侠增伤已更新为 553/1024。 | [执行](https://dps.btcsg.top/static/js/7757.80d98e75.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-yly.65b62ff2.chunk.js) · [状态](https://dps.btcsg.top/static/js/8700.e7c10df5.chunk.js) |
| 周天功 | **截阳、封府**。截阳登记本体伤害，并按分支添加目标绝脉；封府的截阳链和断脉链分别记录在自身，满足前一招的链状态才派发封府追击。 | 截阳的玉枕条目给 512/1024 无视防御。心法非侠系数更新为 266/1024。 | [执行](https://dps.btcsg.top/static/js/4932.18c4c4c2.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-ztg.55989a4c.chunk.js) · [状态](https://dps.btcsg.top/static/js/7176.8e853590.chunk.js) |
| 紫霞功 | **两仪化形**。释放时检查气点，命中以当前 1 至 10 点选择对应伤害条目，再耗尽当前气点。选择回豆秘籍后回 2 点，开启模拟器的会心回豆选项还会单独回 2 点。 | 各档两仪读取对应的增益组合。会神写入突破上限的会效字段；固定回豆开关会直接影响下一发的档位。 | [执行](https://dps.btcsg.top/static/js/6357.cce6b1c1.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-zxg.7216a4cf.chunk.js) · [状态](https://dps.btcsg.top/static/js/8848.0e15d3f2.chunk.js) |

## 无界 5 个心法

| 心法 | 技能触发与状态变化 | 怎样进入 DPS | 3.0.5 代码 |
|---|---|---|---|
| 分山劲·悟 | **劫刀·悟**。先检查盾飞·悟，造成伤害时写入劫刀流血增伤；选中盾飞绝篇「重置调息」才另加盾飞绝篇劫刀增伤。 | 当前这条伤害路径传入的是固定流血标签，对应 123/1024；绝篇另给 100% 通用增伤。11 月调整的偃守绝篇是防御效果。 | [执行](https://dps.btcsg.top/static/js/7691.532ec638.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-w_fsj.1682143c.chunk.js) · [状态](https://dps.btcsg.top/static/js/2298.e7819748.chunk.js) |
| 孤锋诀·悟 | **孤风破浪·悟**。按锐意判断满锐标记，登记本体；按奇穴处理截辕，再执行消耗 60 锐意。伤害表采用线上条目名称「孤风破浪·悟」。 | 孤锋满锐给本击额外 10 个百分点外功会心。灭影追风·悟另有会心、会效、增伤、无双四个字段。 | [执行](https://dps.btcsg.top/static/js/6445.fc812ebc.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-w_gfj.03b0bb0b.chunk.js) · [状态](https://dps.btcsg.top/static/js/4428.e3a73a11.chunk.js) |
| 太玄经·悟 | **返闭惊魂 → 天斗旋·悟**。返闭释放即给自身挂返闭增伤、返闭无双，释放后减绝技调息 4 秒。后续天斗旋按命中时携带的标签取值。 | 返闭增伤的初始时长 15 秒，对天斗旋是 205/1024 通用增伤。该心法的实际路由是 w_txj；模块内部的简写与周天功·悟重名。 | [执行](https://dps.btcsg.top/static/js/3326.03de9b96.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-w_txj.937f1386.chunk.js) · [状态](https://dps.btcsg.top/static/js/2829.7a0c7a9b.chunk.js) |
| 无方·悟 | **钩吻断肠·悟·追加**。需要已有钩吻追加层数。本次追加先造成伤害；释放后先挂钩吻断肠绝章、扣一层追加，最后改变温寒，使随后中和产生的中毒有机会取得新快照。 | 中毒基础为每 16 帧一跳、共 6 跳。把绝章提前至追加伤害之前，会改变哪一笔伤害受益。 | [执行](https://dps.btcsg.top/static/js/1706.ac3ed3e8.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-w_wf.b730172d.chunk.js) · [状态](https://dps.btcsg.top/static/js/9570.b8be7687.chunk.js) |
| 周天功·悟 | **引窍·悟**。普通读条结束时，先挂引窍会效和读条增伤，再打伤害并卸掉一阳指增伤。经脉循行下有瞬发分支，额外传入无视防御和经脉循行增伤标签。 | 引窍会效初始时长 15 秒，给 100 点郭氏内功会效。11 月调整的雾刃飞光·悟属限时对阵招式。 | [执行](https://dps.btcsg.top/static/js/2874.17092730.chunk.js) · [系数](https://dps.btcsg.top/static/js/xinfa-w_ztg.cb646045.chunk.js) · [状态](https://dps.btcsg.top/static/js/7179.550a9219.chunk.js) |

## 如何查某个技能的全部 Buff

打开[技能与 Buff 索引](jx3box_dps_3_0_5_index.json)，先按路由简写找心法，再定位「伤害事件」中的具体技能和等级。事件的「增益组」指向本心法的一个有序下标列表，列表中的每个下标指向一条「增益定义」。同名而数值或适用条件不同的定义分别保留。「模拟器初始状态定义」可查目标归属、最大层数、帧数、伤害频率与自然消失层数；技能在运行中可能覆盖模板。按跳数变化的系数和按等级变化的函数均在其定义范围内展开为数值。

以下查询会打印天罗「暴雨梨花针」能使用的三条千机之威规则，以及每条规则实际改动的属性。在项目根目录运行即可。

```python
import json
from pathlib import Path

catalog = json.loads(Path("docs/jx3box_dps_3_0_5_index.json").read_text(encoding="utf-8"))
spec = catalog["心法"]["tlgd"]
skill = next(row for row in spec["伤害事件"] if row["技能名称"] == "暴雨梨花针")
for index in spec["增益组"][skill["增益组"]]:
    buff = spec["增益定义"][index]
    if buff["增益名称"].startswith("千机之威"):
        print(buff["增益名称"], buff["增益类型"], buff["增益集合"])
```

## 太虚剑意、莫问与当前工作区

作者将太虚剑意、莫问列为独立部署。这两个心法没有出现在本次线上主站的 23 个模块中。正式服 11 月 12 日已将太虚剑意非侠增伤由 58% 调到 67%，并下调橙武期间八荒归元的伤害加成；莫问下调的是橙武期间「羽」附带的阴性内功伤害。11 月 19 日莫问还有武学助手循环及徵、变徵动作修复。主站数据无法证明两个独立计算器的实现状态。[作者发布页](https://www.jx3box.com/bps/79885) · [主站模块清单](https://dps.btcsg.top/static/js/runtime.f81ceb48.js) · [11 月 12 日公告](https://jx3.xoyo.com/index/index.html#/article-details?kid=1335924) · [11 月 19 日公告](https://jx3.xoyo.com/index/index.html#/article-details?kid=1335936)

本报告记录对上游发布物和正式服公告的核查。后续工作区已将 20 个旗舰心法接入 50 级主工作台，太虚与莫问的来源和当前支持范围见[本项目运行规则](season50_runtime.md)。
