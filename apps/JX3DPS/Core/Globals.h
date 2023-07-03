/**
 * Project: JX3DPS
 * File: Global.h
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-28 02:55:41
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef JX3DPS_GLOBALS_H_
#define JX3DPS_GLOBALS_H_

#include "Defs.h"

namespace JX3DPS {

/*----------------------- 错误码 -----------------------*/

/* 成功 */
const Error_t JX3DPS_SUCCESS = 0;

/* 成功 */
const Error_t JX3DPS_SUCCESS_SKILL = 1;

/* 成功 */
const Error_t JX3DPS_SUCCESS_EVENT = 2;

/* 成功 */
const Error_t JX3DPS_SUCCESS_EVENT_SKILL = 3;

/* 未知错误 */
const Error_t JX3DPS_ERROR_UNKNOWN = -1;

/* 无效参数 */
const Error_t JX3DPS_ERROR_INVALID_PARAM = -2;

/* 无效参数 */
const Error_t JX3DPS_ERROR_INVALID_MACRO = -3;

/* 无效参数 */
const Error_t JX3DPS_ERROR_INVALID_MACRO_TYPE = -3;

/* 无效参数 */
const Error_t JX3DPS_ERROR_INVALID_MACRO_CONDITION = -3;

/* 无效参数 */
const Error_t JX3DPS_ERROR_INVALID_MACRO_SKILL = -3;

/* 无效参数 */
const Error_t JX3DPS_ERROR_INVALID_JSON = -100;

/*----------------------- 常量定义 -----------------------*/

/* 延迟常数: (PING / JX3DPS_DELAY) 即 延迟帧数 */
const int JX3DPS_DELAY = 35;

/* 玩家角色 */
const Id_t JX3DPS_PLAYER = 0;

/* 无限制释放距离 */
const int JX3DPS_UNLIMITED_RANGE = INT_MAX;

/* 帧数变量无效值 */
const Frame_t JX3DPS_INVALID_FRAMES_SET = INT_MAX;

/* 默认持续时间 */
const Frame_t JX3DPS_DEFAULT_DURATION_FRAMES = -1;

/* 充能数无效值 */
const int JX3DPS_INVALID_ENERGY_SET = 0;

} // namespace JX3DPS

#endif // JX3DPS_GLOBALS_H_