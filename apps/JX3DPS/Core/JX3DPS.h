/**
 * Project: JX3DPS
 * File: JX3DPS.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-01 02:22:43
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 * 2023-05-30	难为水	 增加导入导出符号
 */

#ifndef JX3DPS_CORE_H_
#define JX3DPS_CORE_H_

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_JX3DPS // CMake add_definitions
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

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define JX3DPS_API     __attribute__((visibility("default")))
#        define JX3DPS_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define JX3DPS_API
#        define JX3DPS_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define JX3DPS_API     __attribute__((visibility("default")))
#    define JX3DPS_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#ifdef __cplusplus
extern "C"
{
#endif

    JX3DPS_API int JX3DPSSimulate(const char *const in, char *out, void *obj, void (*memberFunction)(void *, double));

    JX3DPS_API const char *const JX3DPSVersion();

#ifdef __cplusplus
}
#endif

#endif // JX3DPS_CORE_H_