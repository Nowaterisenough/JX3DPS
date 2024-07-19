#include "Context.h"

#include <immintrin.h>

using namespace JX3DPS;

// 定义全局thread_local的context变量
thread_local Context context = Context();

void Context::Update(frame_t frame) noexcept
{
    constexpr int    AVX_VECTOR_WIDTH = 256;
    constexpr size_t INCREMENT        = AVX_VECTOR_WIDTH / sizeof(frame_t);
    constexpr size_t TOTAL_ELEMENTS   = sizeof(FrameCache) / sizeof(frame_t);

    __m256i frame_vec;
    if constexpr (is_16bit<frame_t>) {
        frame_vec = _mm256_set1_epi16(frame);
    } else if constexpr (is_32bit<frame_t>) {
        frame_vec = _mm256_set1_epi32(frame);
    }
    __m256i zero_vec = _mm256_setzero_si256();

    for (size_t i = 0; i < TOTAL_ELEMENTS; i += INCREMENT) {
        __m256i data = _mm256_loadu_si256(
            reinterpret_cast<const __m256i *>(&reinterpret_cast<frame_t *>(&frame_cache)[i]));
        __m256i result = _mm256_sub_epi16(data, frame_vec);
        __m256i mask   = _mm256_cmpgt_epi16(result, zero_vec);
        result         = _mm256_and_si256(result, mask);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&reinterpret_cast<frame_t *>(&frame_cache)[i]), result);
    }
}

void Context::Reset() noexcept
{
    std::memset(static_cast<void *>(&context.frame_cache), 0xFF, sizeof(FrameCache));
    std::memset(static_cast<void *>(&context.skill_cooldown), 0xFF, sizeof(skill_cooldown));
    bind_cooldown_index = 0;
}
