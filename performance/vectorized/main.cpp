#include <cstdint>
#if defined(__x86_64__) || defined(_M_X64)
#    include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
#    include <arm_neon.h>
#endif

#include <limits>
#include <vector>

#include <benchmark/benchmark.h>

using tick_t = uint16_t;

// 假设的 TickCache 结构体
struct TickCache
{
    std::vector<tick_t> data;

    TickCache(size_t size) : data(size, 0xFFFF) { }
};

#if defined(__x86_64__) || defined(_M_X64)
// x86_64 向量化实现
void update_vectorized(TickCache &frame_cache, tick_t tick)
{
    constexpr int    AVX_VECTOR_WIDTH = 256;
    constexpr size_t INCREMENT        = AVX_VECTOR_WIDTH / sizeof(tick_t);
    const size_t     TOTAL_ELEMENTS   = frame_cache.data.size();

    const __m256i FRAME_VEC = _mm256_set1_epi16(tick);
    const __m256i ZERO_VEC  = _mm256_setzero_si256();
    const __m256i MAX_VEC   = _mm256_set1_epi16(std::numeric_limits<tick_t>::max());
    const __m256i ALL_ONES  = _mm256_set1_epi32(-1);

    // 计算能够被向量化处理的元素数量
    const size_t VECTORIZED_SIZE = (TOTAL_ELEMENTS / INCREMENT) * INCREMENT;

    for (size_t i = 0; i < VECTORIZED_SIZE; i += INCREMENT) {
        const __m256i DATA       = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(&frame_cache.data[i]));
        const __m256i IS_MAX     = _mm256_cmpeq_epi16(DATA, MAX_VEC);
        const __m256i IS_NOT_MAX = _mm256_xor_si256(IS_MAX, ALL_ONES);
        __m256i       result     = _mm256_sub_epi16(DATA, FRAME_VEC);
        const __m256i MASK       = _mm256_and_si256(_mm256_cmpgt_epi16(result, ZERO_VEC), IS_NOT_MAX);
        result                   = _mm256_blendv_epi8(ZERO_VEC, result, MASK);
        result                   = _mm256_blendv_epi8(DATA, result, IS_NOT_MAX);
        _mm256_storeu_si256(reinterpret_cast<__m256i *>(&frame_cache.data[i]), result);
    }
}
#elif defined(__aarch64__) || defined(_M_ARM64)
// ARM64 向量化实现
void update_vectorized(TickCache &frame_cache, tick_t tick)
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
void update_normal(TickCache &frame_cache, tick_t tick)
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
    const size_t CACHE_SIZE = state.range(0);
    TickCache    frame_cache(CACHE_SIZE);
    const tick_t TICK = 100;

    for (const auto _ : state) {
        update_vectorized(frame_cache, TICK);
        benchmark::DoNotOptimize(frame_cache);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * CACHE_SIZE);
}

static void BM_UpdateNormal(benchmark::State &state)
{
    const size_t CACHE_SIZE = state.range(0);
    TickCache    frame_cache(CACHE_SIZE);
    const tick_t TICK = 100;

    for (const auto _ : state) {
        update_normal(frame_cache, TICK);
        benchmark::DoNotOptimize(frame_cache);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * CACHE_SIZE);
}

BENCHMARK(BM_UpdateVectorized)->Range(8, 8 << 10);
BENCHMARK(BM_UpdateNormal)->Range(8, 8 << 10);

BENCHMARK_MAIN();
