# JX3DPS 调试输出格式规范

## 概述

本文档定义了调试器每一步执行后返回给 Code Editor 的 JSON 格式。调试器通过 `context` 逐步执行宏，每执行一步后返回当前状态。

## 执行流程

```
Code Editor -> jx3dps_debug_step() -> Context执行一步 -> 返回JSON状态 -> Code Editor显示
```

## 完整输出示例

```json
{
  "version": "1.0",
  "state": "paused",
  "execution": {
    "current_line": 3,
    "step_type": "skill_cast",
    "current_condition_index": -1,
    "current_condition": null,
    "condition_result": null,
    "last_skill": "人剑合一",
    "skill_success": true,
    "finished": false
  },
  "frame": {
    "current": 16,
    "time": 1.0
  },
  "player": {
    "life_percent": 1.0,
    "mana_percent": 0.95,
    "qidian": 12,
    "rage": 0,
    "energy": 0,
    "buffs": [
      {
        "name": "玄门",
        "id": 12345,
        "stack": 3,
        "duration": 34.0,
        "interval": 0.0
      },
      {
        "name": "风逝",
        "id": 12346,
        "stack": 1,
        "duration": 8.0,
        "interval": 0.0
      }
    ],
    "cooldowns": [
      {
        "skill": "人剑合一",
        "remaining": 12.0
      },
      {
        "skill": "无我无剑",
        "remaining": 5.0
      },
      {
        "skill": "八荒归元",
        "remaining": 0.0
      }
    ]
  },
  "target": {
    "id": 1,
    "life_percent": 0.98,
    "buffs": []
  },
  "damage": {
    "last_damage": 125000,
    "is_critical": true,
    "total_damage": 523000,
    "total_count": 3,
    "critical_count": 2
  }
}
```

## 字段说明

### 1. version (必需, string)

格式版本号，当前为 `"1.0"`。

### 2. state (必需, string)

调试器状态：
- `"running"` - 运行中
- `"paused"` - 已暂停（单步或断点）
- `"finished"` - 执行完成
- `"error"` - 错误

### 3. execution (必需, object)

当前执行状态。

#### 3.1 current_line (必需, int)

当前执行的宏行号（1-based）。

#### 3.2 step_type (必需, string)

当前步骤类型：
- `"line"` - 在指令行开始
- `"condition"` - 在条件判定中
- `"skill_cast"` - 在技能施放中

#### 3.3 current_condition_index (必需, int)

当前条件索引（0-based），`-1` 表示未在条件判定中。

#### 3.4 current_condition (可选, string | null)

当前正在评估的条件文本，如 `"qidian > 20"`。

#### 3.5 condition_result (可选, bool | null)

当前条件的评估结果，`null` 表示尚未评估。

#### 3.6 last_skill (可选, string)

最后施放的技能名称。

#### 3.7 skill_success (可选, bool)

最后技能是否成功施放。

#### 3.8 finished (必需, bool)

是否执行完成。

#### 3.9 error_message (可选, string)

错误信息（仅当 `state` 为 `"error"` 时）。

### 4. frame (必需, object)

游戏帧信息。

#### 4.1 current (必需, int)

当前游戏帧数。

#### 4.2 time (必需, float)

当前游戏时间（秒）。

### 5. player (必需, object)

玩家状态。

#### 5.1 life_percent (必需, float)

生命百分比（0.0 - 1.0）。

#### 5.2 mana_percent (必需, float)

内力百分比（0.0 - 1.0）。

#### 5.3 qidian (必需, int)

气点数。

#### 5.4 rage (可选, int)

怒气值。

#### 5.5 energy (可选, int)

能量值。

#### 5.6 buffs (必需, array[object])

玩家身上的BUFF列表。

```json
{
  "name": "玄门",
  "id": 12345,
  "stack": 3,
  "duration": 34.0,
  "interval": 0.0
}
```

- **name** (string): BUFF名称
- **id** (int): BUFF ID
- **stack** (int): 层数
- **duration** (float): 剩余持续时间（秒）
- **interval** (float): 剩余间隔时间（秒）

#### 5.7 cooldowns (必需, array[object])

技能冷却列表。

```json
{
  "skill": "人剑合一",
  "remaining": 12.0
}
```

- **skill** (string): 技能名称
- **remaining** (float): 剩余CD时间（秒）

### 6. target (必需, object)

目标状态。

#### 6.1 id (必需, int)

目标ID。

#### 6.2 life_percent (必需, float)

目标生命百分比。

#### 6.3 buffs (必需, array[object])

目标身上的BUFF列表，格式同 `player.buffs`。

### 7. damage (可选, object)

伤害统计。

#### 7.1 last_damage (可选, int)

最后一次伤害数值。

#### 7.2 is_critical (可选, bool)

最后一次是否会心。

#### 7.3 total_damage (必需, int)

总伤害。

#### 7.4 total_count (必需, int)

总次数。

#### 7.5 critical_count (必需, int)

会心次数。

## 不同步骤类型的输出示例

### 1. 指令行开始 (step_type: "line")

```json
{
  "version": "1.0",
  "state": "paused",
  "execution": {
    "current_line": 1,
    "step_type": "line",
    "current_condition_index": -1,
    "current_condition": null,
    "condition_result": null,
    "last_skill": null,
    "finished": false
  },
  "frame": {
    "current": 0,
    "time": 0.0
  },
  "player": {
    "life_percent": 1.0,
    "mana_percent": 1.0,
    "qidian": 10,
    "buffs": [],
    "cooldowns": []
  },
  "target": {
    "id": 1,
    "life_percent": 1.0,
    "buffs": []
  },
  "damage": {
    "total_damage": 0,
    "total_count": 0,
    "critical_count": 0
  }
}
```

### 2. 条件判定中 (step_type: "condition")

```json
{
  "version": "1.0",
  "state": "paused",
  "execution": {
    "current_line": 1,
    "step_type": "condition",
    "current_condition_index": 0,
    "current_condition": "qidian > 20",
    "condition_result": false,
    "last_skill": null,
    "finished": false
  },
  "frame": {
    "current": 0,
    "time": 0.0
  },
  "player": {
    "qidian": 10,
    "buffs": [],
    "cooldowns": []
  }
}
```

### 3. 技能施放 (step_type: "skill_cast")

```json
{
  "version": "1.0",
  "state": "paused",
  "execution": {
    "current_line": 2,
    "step_type": "skill_cast",
    "current_condition_index": -1,
    "current_condition": null,
    "condition_result": null,
    "last_skill": "三环套月",
    "skill_success": true,
    "finished": false
  },
  "frame": {
    "current": 16,
    "time": 1.0
  },
  "player": {
    "life_percent": 1.0,
    "mana_percent": 0.95,
    "qidian": 11,
    "buffs": [],
    "cooldowns": [
      {
        "skill": "三环套月",
        "remaining": 8.0
      }
    ]
  },
  "target": {
    "id": 1,
    "life_percent": 0.99,
    "buffs": []
  },
  "damage": {
    "last_damage": 98000,
    "is_critical": false,
    "total_damage": 98000,
    "total_count": 1,
    "critical_count": 0
  }
}
```

### 4. 执行完成 (finished: true)

```json
{
  "version": "1.0",
  "state": "finished",
  "execution": {
    "current_line": 8,
    "step_type": "skill_cast",
    "last_skill": "三柴剑法",
    "skill_success": true,
    "finished": true
  },
  "frame": {
    "current": 1280,
    "time": 80.0
  },
  "player": {
    "life_percent": 1.0,
    "mana_percent": 0.85,
    "qidian": 15,
    "buffs": [
      {
        "name": "玄门",
        "stack": 3,
        "duration": 5.0
      }
    ],
    "cooldowns": []
  },
  "target": {
    "id": 1,
    "life_percent": 0.75,
    "buffs": []
  },
  "damage": {
    "total_damage": 5280000,
    "total_count": 42,
    "critical_count": 18
  }
}
```

### 5. 错误状态 (state: "error")

```json
{
  "version": "1.0",
  "state": "error",
  "execution": {
    "current_line": 3,
    "step_type": "line",
    "finished": false,
    "error_message": "未知技能: 未知技能名"
  }
}
```

## 使用场景

### 1. Code Editor 单步调试

每次用户点击"单步执行"后，调用 API 并显示返回的 JSON：

```cpp
// C++ (Desktop)
QString json = debugSession->StepOver();
auto state = QJsonDocument::fromJson(json.toUtf8());

// 更新 UI
UpdateCurrentLine(state["execution"]["current_line"].toInt());
UpdatePlayerState(state["player"]);
UpdateBuffList(state["player"]["buffs"]);
```

### 2. 断点检查

调试器检查是否命中断点：

```cpp
bool shouldBreak = false;

// 检查行断点
if (breakpoints.contains(currentLine)) {
    auto bp = breakpoints[currentLine];
    if (bp.enabled) {
        if (bp.condition.isEmpty()) {
            shouldBreak = true;
        } else {
            // 评估条件
            shouldBreak = EvaluateCondition(bp.condition, state);
        }
    }
}

if (shouldBreak) {
    return GenerateDebugJson(state, "paused");
}
```

### 3. 监视表达式

根据返回的 JSON 计算监视表达式的值：

```javascript
// 监视表达式: "bufftime:玄门"
function evaluateWatch(expression, state) {
  if (expression === "qidian") {
    return state.player.qidian;
  }

  if (expression.startsWith("bufftime:")) {
    const buffName = expression.substring(9);
    const buff = state.player.buffs.find(b => b.name === buffName);
    return buff ? buff.duration : 0;
  }

  if (expression.startsWith("cd:")) {
    const skillName = expression.substring(3);
    const cd = state.player.cooldowns.find(c => c.skill === skillName);
    return cd ? cd.remaining : 0;
  }
}
```

## API 接口

### C API

```c
// 单步执行，返回 JSON 字符串
const char* jx3dps_debugger_step_over();
const char* jx3dps_debugger_step_into();
const char* jx3dps_debugger_step_out();

// 继续执行直到断点或结束
const char* jx3dps_debugger_continue();

// 获取当前状态（不执行）
const char* jx3dps_debugger_get_state();
```

### C++ API (Qt Desktop)

```cpp
class DebugSession {
public:
    // 返回 JSON 字符串
    QString StepOver();
    QString StepInto();
    QString StepOut();
    QString Continue();
    QString GetCurrentState();
};
```

## 注意事项

1. **行号从1开始**: 所有行号均为 1-based
2. **浮点精度**: 时间和百分比使用浮点数，保留2位小数
3. **可选字段**: 某些字段在特定 `step_type` 下可能为空
4. **性能**: JSON 生成应尽量高效，避免不必要的序列化
