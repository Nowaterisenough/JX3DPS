/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-06 20:22:22
 * @Update      : NoWats
 * @LastTime    : 2022-02-07 23:54:03
 * @FilePath    : \JX3DPS\modules\JX3DPS\JX3DPS.h
 */

#ifndef JX3DPS_H
#define JX3DPS_H

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_JX3DPS // CMake add_definitions
#        ifdef __GNUC__
#            define JX3DPS_API __attribute__(dllexport)
#        else
#            define JX3DPS_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define JX3DPS_API __attribute__(dllimport)
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

namespace JX3DPS {

namespace Simulator {

JX3DPS_API void        Init();
JX3DPS_API const char *Version();
JX3DPS_API const char *Branch();

} // namespace Simulator

} // namespace JX3DPS

#endif // JX3DPS_H