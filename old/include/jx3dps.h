#ifndef JX3DPS_H
#define JX3DPS_H

#if defined _WIN32 || defined __CYGWIN__
#    ifdef EXPORT_JX3DPS // CMake add_definitions
#        ifdef __GNUC__
#            define JX3DPS_API __attribute__((dllexport))
#        else
#            define JX3DPS_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define JX3DPS_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_JX3DPS_LIB
#                define JX3DPS_API __declspec(dllimport)
#            else
#                define JX3DPS_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_JX3DPS
#    define JX3DPS_PRIVATE

#elif defined __GNUC__ || defined __clang__
#    define JX3DPS_API     __attribute__((visibility("default")))
#    define JX3DPS_PRIVATE __attribute__((visibility("hidden")))

#elif defined __EMSCRIPTEN__
#    include <emscripten.h>
#    ifdef EXPORT_JX3DPS
#        define JX3DPS_API EMSCRIPTEN_KEEPALIVE
#    else
#        define JX3DPS_API
#    endif
#    define JX3DPS_PRIVATE

#else
#    define JX3DPS_API
#    define JX3DPS_PRIVATE
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief 获取版本号
     * @return 版本号
     */
    JX3DPS_API const char *jx3dps_version();

    /**
     * @brief 获取JX3版本号
     * @return 版本号
     */
    JX3DPS_API const char *jx3dps_jx3_version();

    /**
     * @brief 模拟API
     * @param in 输入, JSON格式
     * @param ... 可变参，可以是输出的指针，也可以是进度回调函数
     * @return 错误码 0: 成功, 其他: 失败
     */
    JX3DPS_API int jx3dps_simulate(const char *const in, ...);

    /**
     * @brief 获取结果
     * @return 结果, JSON格式
     */
    JX3DPS_API const char *jx3dps_get_result();

    /**
     * @brief 获取进度
     * @return 进度
     */
    JX3DPS_API double jx3dps_get_progress();

    /**
     * @brief 调试
     * @param in 输入, JSON格式
     * @param ... 可变参，可以是输出的指针，也可以是进度回调函数
     * @return 错误码 0: 成功, 其他: 失败
     */
    JX3DPS_API int jx3dps_debug(const char *const in, ...);

    /**
     * @brief 单步进入
     * @return 调试信息
     */
    JX3DPS_API const char *jx3dps_debugger_step_in();

    /**
     * @brief 单步越过
     * @return 调试信息
     */
    JX3DPS_API const char *jx3dps_debugger_step_over();

    /**
     * @brief 继续执行到下一个断点或程序结束
     * @return 调试信息
     */
    JX3DPS_API const char *jx3dps_debugger_continue();

    /**
     * @brief 设置断点
     */
    JX3DPS_API void jx3dps_debugger_set_breakpoint(const char *const in);

#ifdef __cplusplus
}
#endif

#endif // JX3DPS_H