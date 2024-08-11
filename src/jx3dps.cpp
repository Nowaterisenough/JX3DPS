#include "jx3dps.h"

#include <cstdarg>

#include "Error.h"
#include "Log.h"
#include "proxy.h"
#include "SimulatorImpl.h"
#include "Version.h"

using namespace JX3DPS;

const char *jx3dps_version()
{
    return VERSION;
}

const char *jx3dps_jx3_version()
{
    return JX3_VERSIONS.back();
}

int jx3dps_simulate(const char *const in, ...)
{
    va_list args;
    va_start(args, in);

    // 确定调用哪个函数
    char *out = va_arg(args, char *);
    if (out == nullptr) {
        // 没有输出参数，执行异步模拟
        va_end(args);
        return SimulatorImpl::GetInstance()->SimulateAsync(in);
    }

    // 检查是否还有进度回调函数参数
    void *next_arg = va_arg(args, void *);
    if (next_arg == nullptr) {
        // 没有进度回调函数，只有输出参数
        va_end(args);
        return SimulatorImpl::GetInstance()->SimulateSync(in, out);
    }

    // 使用进度回调函数
    va_end(args);
    return SimulatorImpl::GetInstance()->SimulateSyncWithProgressInfo(in, out, (void (*)(double))next_arg);
    return 0;
}

const char *jx3dps_get_result()
{
    return SimulatorImpl::result.dump().c_str();
    return "";
}

double jx3dps_get_progress()
{
    return SimulatorImpl::progress;
    return 0.0;
}

int jx3dps_debug(const char *const in, ...)
{
    return 0;
}

const char *jx3dps_debugger_step_in()
{
    JX3DPS::json j;
    return j.dump().c_str();
    return "";
}

const char *jx3dps_debugger_step_over()
{
    JX3DPS::json j;
    return j.dump().c_str();
    return "";
}

const char *jx3dps_debugger_continue()
{
    JX3DPS::json j;
    return j.dump().c_str();
    return "";
}

void jx3dps_debugger_set_breakpoint(const char *const in) { }