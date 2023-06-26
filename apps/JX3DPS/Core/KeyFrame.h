/**
 * Project: JX3DPS
 * File: KeyFrame.h
 * Description:
 * Created Date: 2023-06-19 16:26:49
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-20 15:33:51
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef JX3DPS_KEYFRAME_H_
#define JX3DPS_KEYFRAME_H_

#include "Buff.h"
#include "Json.h"
#include "Macro.h"
#include "Player.h"
#include "Skill.h"
#include "Target.h"

namespace JX3DPS {

namespace KeyFrame {

enum class KeyFrameType
{
    EVENT = 0,
    SKILL = 1,
    BUFF  = 2,
};

using KeyFrame         = std::pair<Frame_t, std::list<std::pair<KeyFrameType, Id_t>>>;
using KeyFrameSequence = std::list<KeyFrame>;

inline KeyFrame operator+(KeyFrame &lhs, KeyFrame &rhs)
{
    KeyFrame result;
    result.first = lhs.first;
    result.second.insert(result.second.end(), lhs.second.begin(), lhs.second.end());
    result.second.insert(result.second.end(), rhs.second.begin(), rhs.second.end());
    return result;
}

inline bool operator==(KeyFrame &lhs, KeyFrame &rhs)
{
    return lhs.first == rhs.first;
}

inline bool operator<(KeyFrame &lhs, KeyFrame &rhs)
{
    return lhs.first < rhs.first;
}

void InsertKeyFrame(KeyFrameSequence &keyFrameSequence, KeyFrame &keyFrame);

void GenerateKeyFrameSequence(KeyFrameSequence &keyFrameSequence, Player *player, ExprEvents &exprEvents);

void UpdateKeyFrameSequence(KeyFrameSequence &keyFrameSequence, Player *player, Frame_t next);

void KeyFrameAdvance(KeyFrameSequence &keyFrameSequence, Player *player, Targets *targets, ExprEvents &exprEvents, ExprSkills &exprSkills);

} // namespace KeyFrame

} // namespace JX3DPS

#endif // JX3DPS_KEYFRAME_H_