#include "Context.h"

#if defined(__x86_64__) || defined(_M_X64)
#    include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
#    include <arm_neon.h>
#endif

using namespace JX3DPS;

// 定义全局thread_local的context变量
thread_local Context context = Context();

void Context::Update(tick_t tick) noexcept
{
#if defined(__x86_64__) || defined(_M_X64)
    constexpr int    AVX_VECTOR_WIDTH = 256;
    constexpr size_t INCREMENT        = AVX_VECTOR_WIDTH / sizeof(tick_t);
    constexpr size_t TOTAL_ELEMENTS   = sizeof(FrameCache) / sizeof(tick_t);

    __m256i frame_vec;
    if constexpr (is_16bit<tick_t>) {
        frame_vec = _mm256_set1_epi16(tick);
    } else if constexpr (is_32bit<tick_t>) {
        frame_vec = _mm256_set1_epi32(tick);
    }
    __m256i zero_vec = _mm256_setzero_si256();

    for (size_t i = 0; i < TOTAL_ELEMENTS; i += INCREMENT) {
        __m256i data =
            _mm256_loadu_si256(reinterpret_cast<const __m256i *>(&reinterpret_cast<tick_t *>(&frame_cache)[i]));
        __m256i result = _mm256_sub_epi16(data, frame_vec);
        __m256i mask   = _mm256_cmpgt_epi16(result, zero_vec);
        result         = _mm256_and_si256(result, mask);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&reinterpret_cast<tick_t *>(&frame_cache)[i]), result);
    }
#elif defined(__aarch64__) || defined(_M_ARM64)
    constexpr int    NEON_VECTOR_WIDTH = 128;
    constexpr size_t INCREMENT         = NEON_VECTOR_WIDTH / sizeof(tick_t);
    constexpr size_t TOTAL_ELEMENTS    = sizeof(FrameCache) / sizeof(tick_t);

    if constexpr (is_16bit<tick_t>) {
        uint16x8_t frame_vec = vdupq_n_u16(tick);
        uint16x8_t zero_vec  = vdupq_n_u16(0);

        for (size_t i = 0; i < TOTAL_ELEMENTS; i += INCREMENT) {
            uint16x8_t data =
                vld1q_u16(reinterpret_cast<const uint16_t *>(&reinterpret_cast<tick_t *>(&frame_cache)[i]));
            uint16x8_t result = vsubq_u16(data, frame_vec);
            uint16x8_t mask   = vcgtq_u16(result, zero_vec);
            result            = vandq_u16(result, mask);
            vst1q_u16(reinterpret_cast<uint16_t *>(&reinterpret_cast<tick_t *>(&frame_cache)[i]), result);
        }
    } else if constexpr (is_32bit<tick_t>) {
        uint32x4_t frame_vec = vdupq_n_u32(tick);
        uint32x4_t zero_vec  = vdupq_n_u32(0);

        for (size_t i = 0; i < TOTAL_ELEMENTS; i += INCREMENT) {
            uint32x4_t data =
                vld1q_u32(reinterpret_cast<const uint32_t *>(&reinterpret_cast<tick_t *>(&frame_cache)[i]));
            uint32x4_t result = vsubq_u32(data, frame_vec);
            uint32x4_t mask   = vcgtq_u32(result, zero_vec);
            result            = vandq_u32(result, mask);
            vst1q_u32(reinterpret_cast<uint32_t *>(&reinterpret_cast<tick_t *>(&frame_cache)[i]), result);
        }
    }
#else
    // 通用实现，不使用 SIMD 指令
    for (size_t i = 0; i < sizeof(FrameCache) / sizeof(tick_t); ++i) {
        tick_t &value = reinterpret_cast<tick_t *>(&frame_cache)[i];
        if (value > tick) {
            value -= tick;
        } else {
            value = 0;
        }
    }
#endif
}

void Context::Reset() noexcept
{
    std::memset(static_cast<void *>(&context.frame_cache), 0xFF, sizeof(FrameCache));
    std::memset(static_cast<void *>(&context.skill_cooldown), 0xFF, sizeof(skill_cooldown));
    bind_cooldown_index = 0;
}
