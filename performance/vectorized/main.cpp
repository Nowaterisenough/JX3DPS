#include <immintrin.h>

#include <limits>
#include <vector>

#include <benchmark/benchmark.h>

using frame_t = uint16_t;

// 假设的 FrameCache 结构体
struct FrameCache
{
    std::vector<frame_t> data;

    FrameCache(size_t size) : data(size, 0xFFFF) { }
};

// 向量化实现
void update_vectorized(FrameCache &frame_cache, frame_t frame)
{
    constexpr int    AVX_VECTOR_WIDTH = 256;
    constexpr size_t INCREMENT        = AVX_VECTOR_WIDTH / sizeof(frame_t);
    const size_t     TOTAL_ELEMENTS   = frame_cache.data.size();

    __m256i frame_vec = _mm256_set1_epi16(frame);
    __m256i zero_vec  = _mm256_setzero_si256();
    __m256i max_vec   = _mm256_set1_epi16(std::numeric_limits<frame_t>::max());
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

// 普通实现
void update_normal(FrameCache &frame_cache, frame_t frame)
{
    for (auto &value : frame_cache.data) {
        if (value != std::numeric_limits<frame_t>::max()) {
            if (value > frame) {
                value -= frame;
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
    const frame_t frame = 100;

    for (auto _ : state) {
        update_vectorized(frame_cache, frame);
        benchmark::DoNotOptimize(frame_cache);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * cache_size);
}

static void BM_UpdateNormal(benchmark::State &state)
{
    const size_t  cache_size = state.range(0);
    FrameCache    frame_cache(cache_size);
    const frame_t frame = 100;

    for (auto _ : state) {
        update_normal(frame_cache, frame);
        benchmark::DoNotOptimize(frame_cache);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * cache_size);
}

BENCHMARK(BM_UpdateVectorized)->Range(8, 8 << 10);
BENCHMARK(BM_UpdateNormal)->Range(8, 8 << 10);

BENCHMARK_MAIN();