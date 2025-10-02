#ifndef RESOURCE_H
#define RESOURCE_H

#if defined _WIN32 || defined __CYGWIN__
#    if defined RESOURCES_API_EXPORT // CMake add_definitions
#        ifdef __GNUC__
#            define RESOURCES_API __attribute__((dllexport))
#        else
#            define RESOURCES_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define RESOURCES_API __attribute__((dllimport))
#        else
#            ifndef RESOURCES_STATIC_IMPORT
#                define RESOURCES_API __declspec(dllimport)
#            else
#                define RESOURCES_API
#            endif
#        endif // __GNUC__
#    endif     // RESOURCES_API_EXPORT
#    define RESOURCES_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define RESOURCES_API     __attribute__((visibility("default")))
#        define RESOURCES_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define RESOURCES_API
#        define RESOURCES_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define RESOURCES_API     __attribute__((visibility("default")))
#    define RESOURCES_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

class QFont;

class RESOURCES_API Resources
{
public:
    /**
     * @brief 初始化资源
     * @return int 初始化是否成功
     */
    static int InitResources();

    static QFont &Font();

    // 代码编辑器专用等宽字体（霞鹜文楷等宽 + Monaco）
    static QFont &MonoFont();

    Resources() = delete;
};

#endif // RESOURCE_H
