/**
 * Project: JX3DPS
 * File: TimeLine.h
 * Description:
 * Created Date: 2023-09-13 03:02:14
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-13 04:12:35
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "TimeLine.hpp"

namespace JX3DPS {

std::function<void(int)> TimeLine::s_addTimeStampFunc = nullptr;

std::function<void(const std::string &name, RollResult rollResult, int damage, NodeType type)> TimeLine::s_recordFunc = nullptr;

} // namespace JX3DPS
