#ifndef JX3DPS_WORKER_H
#define JX3DPS_WORKER_H

#include <immintrin.h>

#include <cstdint>
#include <limits>
#include <type_traits>

#include "player.h"
#include "target.h"
#include "type.h"

template <typename T>
concept is_16bit = std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>;

template <typename T>
concept is_32bit = std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>;

namespace JX3DPS {

class Worker
{
public:
    void Init();
    void Run();

    static thread_local Player  player;
    static thread_local Targets targets;

    struct FrameCache
    {
        constexpr static size_t UNIT_SIZE = 8;
        constexpr static size_t TYPE_SIZE = 64;

        frame_t skill_cooldown[TYPE_SIZE]           = { std::numeric_limits<frame_t>::max() };
        frame_t skill_prepare[TYPE_SIZE]            = { std::numeric_limits<frame_t>::max() };
        frame_t buff_cooldown[TYPE_SIZE][UNIT_SIZE] = { std::numeric_limits<frame_t>::max() };
        frame_t buff_duration[TYPE_SIZE][UNIT_SIZE] = { std::numeric_limits<frame_t>::max() };
        frame_t buff_interval[TYPE_SIZE][UNIT_SIZE] = { std::numeric_limits<frame_t>::max() };

        // AVX256命令快速更新帧缓存
        void Update(frame_t frame) noexcept
        {
            constexpr size_t TOTAL_ELEMENTS   = sizeof(FrameCache) / sizeof(frame);
            constexpr int    AVX_VECTOR_WIDTH = 256; // 定义256位的常量
            constexpr size_t INCREMENT        = AVX_VECTOR_WIDTH / sizeof(frame_t);

            __m256i frame_vec;
            if constexpr (is_16bit<frame_t>) {
                frame_vec = _mm256_set1_epi16(frame);
            } else if constexpr (is_32bit<frame_t>) {
                frame_vec = _mm256_set1_epi32(frame);
            }

            for (size_t idx = 0; idx < TOTAL_ELEMENTS; idx += INCREMENT) {
                auto *ptr  = reinterpret_cast<__m256i *>(skill_cooldown) + idx;
                auto  data = _mm256_loadu_si256(ptr);
                if constexpr (is_16bit<frame_t>) {
                    data = _mm256_sub_epi16(data, frame_vec);
                } else if constexpr (is_32bit<frame_t>) {
                    data = _mm256_sub_epi32(data, frame_vec);
                }
                _mm256_storeu_si256(ptr, data);
            }
        }
    };  
};

} // namespace JX3DPS

#endif // JX3DPS_WORKER_H