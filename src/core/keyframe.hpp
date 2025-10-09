#ifndef JX3DPS_CORE_KEYFRAME_HPP
#define JX3DPS_CORE_KEYFRAME_HPP

#include "src/global/types.h"
#include <list>
#include <utility>

namespace JX3DPS {

/**
 * @brief 无效tick标记
 */
constexpr tick_t INVALID_TICK = -1;

/**
 * @brief 关键帧类型
 */
enum class KeyFrameType {
    EVENT = 0,    // 宏事件（强制时间点）
    SKILL = 1,    // 技能冷却结束
    BUFF  = 2,    // BUFF触发/结束
    EMPTY = 3     // 空帧
};

/**
 * @brief 关键帧 = (时间点, 事件列表)
 *
 * first: 距离当前时间的帧数
 * second: 该时间点发生的事件列表 [(类型, ID)]
 */
using KeyFrame = std::pair<tick_t, std::list<std::pair<KeyFrameType, jx3id_t>>>;

/**
 * @brief 关键帧序列（按时间排序）
 */
using KeyFrameSequence = std::list<KeyFrame>;

/**
 * @brief 插入关键帧到序列（自动合并相同时间点）
 */
inline void InsertKeyFrame(KeyFrameSequence &sequence, KeyFrame &keyFrame) {
    if (sequence.empty()) {
        sequence.push_back(keyFrame);
        return;
    }

    for (auto it = sequence.begin(); it != sequence.end(); ++it) {
        if (it->first == keyFrame.first) {
            // 相同时间点，合并事件列表
            it->second.insert(it->second.end(), keyFrame.second.begin(), keyFrame.second.end());
            return;
        } else if (it->first > keyFrame.first) {
            // 找到插入位置
            sequence.insert(it, keyFrame);
            return;
        }
    }

    // 添加到末尾
    sequence.push_back(keyFrame);
}

/**
 * @brief 更新关键帧序列（减去跳过的时间）
 */
inline void UpdateKeyFrameSequence(KeyFrameSequence &sequence, tick_t elapsed) {
    for (auto &frame : sequence) {
        if (frame.first != INVALID_TICK) {
            frame.first -= elapsed;
        }
    }
}

} // namespace JX3DPS

#endif // JX3DPS_CORE_KEYFRAME_HPP
