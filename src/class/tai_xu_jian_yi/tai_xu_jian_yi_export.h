#ifndef JX3DPS_TAI_XU_JIAN_YI_EXPORT_H
#define JX3DPS_TAI_XU_JIAN_YI_EXPORT_H

// ============================================================================
// Export/Import Macros for Dynamic Library
// ============================================================================

#if defined(_WIN32) || defined(_WIN64)
    // Windows DLL export/import
    #ifdef TXJY_BUILD_SHARED
        #ifdef TXJY_EXPORT
            #define TXJY_API __declspec(dllexport)
        #else
            #define TXJY_API __declspec(dllimport)
        #endif
    #else
        #define TXJY_API
    #endif
#elif defined(__GNUC__) && __GNUC__ >= 4
    // GCC 4+ visibility attributes
    #ifdef TXJY_BUILD_SHARED
        #define TXJY_API __attribute__((visibility("default")))
    #else
        #define TXJY_API
    #endif
#else
    #define TXJY_API
#endif

// C linkage for C API functions
#ifdef __cplusplus
    #define TXJY_EXTERN_C_BEGIN extern "C" {
    #define TXJY_EXTERN_C_END }
    #define TXJY_EXTERN_C extern "C"
#else
    #define TXJY_EXTERN_C_BEGIN
    #define TXJY_EXTERN_C_END
    #define TXJY_EXTERN_C
#endif

// Calling convention (Windows)
#if defined(_WIN32) || defined(_WIN64)
    #define TXJY_CALL __cdecl
#else
    #define TXJY_CALL
#endif

#endif // JX3DPS_TAI_XU_JIAN_YI_EXPORT_H
