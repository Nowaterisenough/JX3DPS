#ifndef JX3DPS_H
#define JX3DPS_H
#include <stdint.h>
#if defined(_WIN32) && defined(JX3DPS_SHARED)
#    ifdef JX3DPS_BUILD
#        define JX3DPS_API __declspec(dllexport)
#    else
#        define JX3DPS_API __declspec(dllimport)
#    endif
#elif defined(JX3DPS_SHARED) && defined(__GNUC__)
#    define JX3DPS_API __attribute__((visibility("default")))
#else
#    define JX3DPS_API
#endif
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct jx3dps_engine jx3dps_engine;
    JX3DPS_API uint32_t          jx3dps_abi_version(void);
    JX3DPS_API jx3dps_engine    *jx3dps_create(void);
    JX3DPS_API void              jx3dps_destroy(jx3dps_engine *engine);
    /* UTF-8 JSON. The result belongs to this handle until its next call or destruction.
       Serialize calls on one handle. Separate handles may run concurrently.
       Failures return {"error":"..."}. Never destroy a handle during a call. */
    JX3DPS_API const char       *jx3dps_call(jx3dps_engine *engine, const char *request);
    /* May be called from another thread during a native batch. Use a new handle after cancellation. */
    JX3DPS_API void              jx3dps_cancel(jx3dps_engine *engine);
    JX3DPS_API uint64_t          jx3dps_completed(const jx3dps_engine *engine);
#ifdef __cplusplus
}
#endif
#endif
