# JX3DPS WebAssembly 应用

这是 JX3DPS 的 WebAssembly 版本，可以在浏览器中运行。

## 构建要求

1. **Emscripten SDK**: 用于将 C++ 代码编译为 WebAssembly
   - 下载并安装: https://emscripten.org/docs/getting_started/downloads.html
   - 激活环境: `source /path/to/emsdk/emsdk_env.sh` (Linux/Mac) 或 `emsdk_env.bat` (Windows)

2. **CMake**: 版本 3.11 或更高

## 构建步骤

### 1. 激活 Emscripten 环境

**Linux/Mac:**
```bash
source /path/to/emsdk/emsdk_env.sh
```

**Windows:**
```cmd
call C:\path\to\emsdk\emsdk_env.bat
```

### 2. 配置并构建

```bash
# 创建构建目录
mkdir build-wasm
cd build-wasm

# 使用 Emscripten 工具链配置
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
emmake make -j4
```

### 3. 运行

构建完成后，WebAssembly 产物将位于 `build-wasm/out` 目录：
- `jx3dps.js` - Emscripten 生成的 JavaScript 胶水代码
- `jx3dps.wasm` - WebAssembly 二进制文件
- `index.html` - Web 应用界面
- `coi-serviceworker.js` - 跨域隔离 Service Worker

使用本地 HTTP 服务器运行：

```bash
cd build-wasm/out

# Python 3
python -m http.server 8000

# Python 2
python -m SimpleHTTPServer 8000

# Node.js (需要安装 http-server)
npx http-server -p 8000
```

然后在浏览器中访问: http://localhost:8000

## 功能特性

- **版本信息**: 查看 JX3DPS 和游戏数据版本
- **DPS 模拟**: 输入技能宏并运行 DPS 模拟计算
- **宏调试**: 支持断点、单步执行等调试功能
- **控制台输出**: 实时查看程序输出和日志

## 技术说明

### 跨域隔离 (Cross-Origin Isolation)

WebAssembly 的多线程支持需要 SharedArrayBuffer，这要求页面具有跨域隔离特性。本应用包含一个 Service Worker (`coi-serviceworker.js`) 来自动添加必要的 HTTP 头部：
- `Cross-Origin-Opener-Policy: same-origin`
- `Cross-Origin-Embedder-Policy: require-corp`

首次访问时可能需要刷新页面以激活 Service Worker。

### 导出的 C 函数

以下 JX3DPS 函数已导出供 JavaScript 调用：
- `jx3dps_version()` - 获取库版本
- `jx3dps_jx3_version()` - 获取游戏数据版本
- `jx3dps_simulate()` - 运行 DPS 模拟
- `jx3dps_get_result()` - 获取模拟结果
- `jx3dps_debug()` - 启动调试会话
- `jx3dps_debugger_step_in()` - 单步进入
- `jx3dps_debugger_step_over()` - 单步越过
- `jx3dps_debugger_continue()` - 继续执行

## 故障排除

### 问题: SharedArrayBuffer is not defined

**解决方案**: 确保通过 HTTP 服务器访问（而非 file:// 协议），并等待 Service Worker 激活后刷新页面。

### 问题: WebAssembly 模块加载失败

**解决方案**:
1. 检查浏览器控制台的错误信息
2. 确保所有文件（.js, .wasm, .html）都在同一目录
3. 清除浏览器缓存并重新加载

### 问题: 构建时出现 Emscripten 错误

**解决方案**:
1. 确认已正确激活 Emscripten 环境
2. 检查 Emscripten 版本是否为最新
3. 清除构建目录并重新构建

## 浏览器兼容性

- Chrome/Edge: 92+
- Firefox: 95+
- Safari: 15.2+

推荐使用最新版本的 Chrome 或 Edge 浏览器以获得最佳性能。
