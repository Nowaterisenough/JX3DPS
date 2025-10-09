#if defined(__x86_64__) || defined(_M_X64)
#    include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
#    include <arm_neon.h>
#endif

#include <limits>
#include <vector>

#include <benchmark/benchmark.h>

using tick_t = uint16_t;

// 假设的 FrameCache 结构体
struct FrameCache
{
    std::vector<tick_t> data;

    FrameCache(size_t size) : data(size, 0xFFFF) { }
};

#if defined(__x86_64__) || defined(_M_X64)
// x86_64 向量化实现
void update_vectorized(FrameCache &frame_cache, tick_t tick)
{
    constexpr int    AVX_VECTOR_WIDTH = 256;
    constexpr size_t INCREMENT        = AVX_VECTOR_WIDTH / sizeof(tick_t);
    const size_t     TOTAL_ELEMENTS   = frame_cache.data.size();

    __m256i frame_vec = _mm256_set1_epi16(tick);
    __m256i zero_vec  = _mm256_setzero_si256();
    __m256i max_vec   = _mm256_set1_epi16(std::numeric_limits<tick_t>::max());
    __m256i all_ones  = _mm256_set1_epi32(-1);

    // 计算能够被向量化处理的元素数量
    size_t vectorized_size = (TOTAL_ELEMENTS / INCREMENT) * INCREMENT;

    for (size_t i = 0; i < vectorized_size; i += INCREMENT) {
        __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(&frame_cache.data[i]));
        __m256i is_max     = _mm256_cmpeq_epi16(data, max_vec);
        __m256i is_not_max = _mm256_xor_si256(is_max, all_ones);
        __m256i result     = _mm256_sub_epi16(data, frame_vec);
        __m256i mask       = _mm256_and_si256(_mm256_cmpgt_epi16(result, zero_vec), is_not_max);
        result             = _mm256_blendv_epi8(zero_vec, result, mask);
        result             = _mm256_blendv_epi8(data, result, is_not_max);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&frame_cache.data[i]), result);
    }
}
#elif defined(__aarch64__) || defined(_M_ARM64)
// ARM64 向量化实现
void update_vectorized(FrameCache &frame_cache, tick_t tick)
{
    constexpr int    NEON_VECTOR_WIDTH = 128;
    constexpr size_t INCREMENT         = NEON_VECTOR_WIDTH / sizeof(tick_t);
    const size_t     TOTAL_ELEMENTS    = frame_cache.data.size();

    uint16x8_t frame_vec = vdupq_n_u16(tick);
    uint16x8_t zero_vec  = vdupq_n_u16(0);
    uint16x8_t max_vec   = vdupq_n_u16(std::numeric_limits<tick_t>::max());

    // 计算能够被向量化处理的元素数量
    size_t vectorized_size = (TOTAL_ELEMENTS / INCREMENT) * INCREMENT;

    for (size_t i = 0; i < vectorized_size; i += INCREMENT) {
        uint16x8_t data       = vld1q_u16(&frame_cache.data[i]);
        uint16x8_t is_max     = vceqq_u16(data, max_vec);
        uint16x8_t is_not_max = vmvnq_u16(is_max);
        uint16x8_t result     = vsubq_u16(data, frame_vec);
        uint16x8_t mask       = vandq_u16(vcgtq_u16(result, zero_vec), is_not_max);
        result                = vbslq_u16(mask, result, zero_vec);
        result                = vbslq_u16(is_not_max, result, data);
        vst1q_u16(&frame_cache.data[i], result);
    }
}
#else
// 对于不支持 SIMD 的架构，使用普通实现
#    define update_vectorized update_normal
#endif

// 普通实现
void update_normal(FrameCache &frame_cache, tick_t tick)
{
    for (auto &value : frame_cache.data) {
        if (value != std::numeric_limits<tick_t>::max()) {
            if (value > tick) {
                value -= tick;
            } else {
                value = 0;
            }
        }
    }
}

static void BM_UpdateVectorized(benchmark::State &state)
{
    const size_t  cache_size = state.range(0);
    FrameCache    frame_cache(cache_size);
    const tick_t tick = 100;

    for (auto _ : state) {
        update_vectorized(frame_cache, tick);
        benchmark::DoNotOptimize(frame_cache);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * cache_size);
}

static void BM_UpdateNormal(benchmark::State &state)
{
    const size_t  cache_size = state.range(0);
    FrameCache    frame_cache(cache_size);
    const tick_t tick = 100;

    for (auto _ : state) {
        update_normal(frame_cache, tick);
        benchmark::DoNotOptimize(frame_cache);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * cache_size);
}

BENCHMARK(BM_UpdateVectorized)->Range(8, 8 << 10);
BENCHMARK(BM_UpdateNormal)->Range(8, 8 << 10);

BENCHMARK_MAIN();
