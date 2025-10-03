# JX3DPS 构建问题排查

## GitHub Actions 子模块错误修复

### 错误信息

```
fatal: No url found for submodule path '3rd/benchmark' in .gitmodules
The process 'C:\Program Files\Git\bin\git.exe' failed with exit code 128
```

### 根本原因

Git 仓库中追踪了 **7 个子模块**（通过 `git ls-tree HEAD 3rd/` 可见 mode 160000），但 `.gitmodules` 文件中只配置了 4 个。

**缺失配置的子模块**：
- ❌ `3rd/benchmark` (Google Benchmark)
- ❌ `3rd/QCustomPlot` (QCustomPlot library)
- ❌ `3rd/proxy` (Microsoft Proxy) ← **关键遗漏**

**已配置的子模块**：
- ✅ `3rd/json` (nlohmann/json)
- ✅ `3rd/googletest` (Google Test)
- ✅ `3rd/spdlog` (spdlog)
- ✅ `3rd/qwindowkit` (QWindowKit)

### 完整修复方案

#### 1. 补全 `.gitmodules` 配置

添加所有缺失的子模块：

```ini
[submodule "3rd/benchmark"]
	path = 3rd/benchmark
	url = https://github.com/google/benchmark.git

[submodule "3rd/QCustomPlot"]
	path = 3rd/QCustomPlot
	url = https://github.com/dimv36/QCustomPlot-library.git

[submodule "3rd/proxy"]
	path = 3rd/proxy
	url = https://github.com/microsoft/proxy.git
```

#### 2. 提交历史

**第一次修复**（不完整）：
```
commit f894c28
修复：补全 .gitmodules 中缺失的子模块配置
- 添加 benchmark 和 QCustomPlot 配置
- ❌ 遗漏了 proxy 子模块
```

**完整修复**：
```
commit 5ec5b6f
修复：补全 3rd/proxy 子模块配置
- 添加 proxy 子模块配置
- ✅ 现在所有 7 个子模块都已配置
```

### 验证方法

#### 本地验证

```bash
# 1. 清理子模块缓存
git submodule deinit -f --all

# 2. 重新初始化
git submodule update --init --recursive

# 3. 检查子模块状态（应显示 7 个）
git submodule status
```

#### 检查 Git 树与配置一致性

```bash
# 列出 Git 追踪的子模块（mode 160000）
git ls-tree HEAD 3rd/ | grep "^160000"

# 对比 .gitmodules 配置
git config -f .gitmodules --get-regexp path

# 完整验证（无输出表示一致）
diff <(git ls-tree HEAD 3rd/ | grep "^160000" | awk '{print $4}' | sort) \
     <(git config -f .gitmodules --get-regexp path | awk '{print $2}' | sort)
```

### GitHub Actions 工作流

**文件**：`.github/workflows/windows-qt6.9.yml`

**关键配置**：
```yaml
- name: Checkout code
  uses: actions/checkout@v4
  with:
    submodules: recursive  # 递归初始化所有子模块
    fetch-depth: 1
```

**Qt 6.9.2 自定义构建**：
```yaml
QT_DOWNLOAD_URL: https://github.com/Nowaterisenough/QtBuild/releases/download/qt6.9.2-native-20250901-1356/qt6.9.2-windows-x86_64-mingwgcc15.2.0_ucrt-shared_release.7z
```

### 修复时间线

| 提交 | 内容 | 结果 |
|------|------|------|
| f894c28 | 添加 benchmark + QCustomPlot | ❌ 仍失败（遗漏 proxy） |
| 5ec5b6f | 添加 proxy 配置 | ✅ 完全修复 |

### 关键教训

1. **不要依赖本地目录判断子模块**：
   - `ls 3rd/` 无法区分子模块和普通目录
   - 必须用 `git ls-tree HEAD 3rd/ | grep "^160000"` 查看 Git 对象

2. **CI/CD 错误提示不精确**：
   - 错误只提到第一个失败的子模块
   - 但可能有多个子模块缺少配置
   - 需要全面排查所有子模块

3. **验证配置完整性**：
   - 使用 diff 对比 Git 树和 .gitmodules
   - 确保每个 mode 160000 的路径都有配置

### 完整子模块列表

1. **3rd/benchmark** - Google Benchmark（性能测试框架）
2. **3rd/QCustomPlot** - QCustomPlot 图表库
3. **3rd/proxy** - Microsoft Proxy（代理模式库）
4. **3rd/json** - nlohmann/json（JSON 解析）
5. **3rd/googletest** - Google Test（单元测试框架）
6. **3rd/spdlog** - spdlog（日志库）
7. **3rd/qwindowkit** - QWindowKit（窗口框架）

### 验证构建

推送后查看 GitHub Actions：
```
https://github.com/Nowaterisenough/JX3DPS/actions/workflows/windows-qt6.9.yml
```

预期结果：
- ✅ Checkout 步骤成功（所有子模块正常初始化）
- ✅ 构建成功生成可执行文件
- ✅ 打包生成发布文件

---

**最后更新**：2025-10-03
**问题状态**：已完全修复
**修复提交**：5ec5b6f
**验证链接**：https://github.com/Nowaterisenough/JX3DPS/actions
