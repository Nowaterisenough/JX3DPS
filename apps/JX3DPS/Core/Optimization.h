/**
 * Project: JX3DPS
 * File: OptimizationUtils.h
 * Description:
 * Created Date: 2023-06-20 12:37:35
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-11 23:10:13
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef JX3DPS_OPTIMIZATION_H_
#define JX3DPS_OPTIMIZATION_H_

#include "JX3Params.h"

/*------------------ 优化 - 条件跳转 ------------------*/

// 负数为1，非负数为0
#define CHECK_NEGATIVE(x) (((x) >> (sizeof(x) * 8 - 1)) & 1)

#define OPTIMIZE_SET (3)
#ifndef OPTIMIZE_SET
#    define INVALID_SET      (JX3DPS::MAX_FRAMES_CONST)
#    define MIN_FRAMES(x, y) ((x) < (y) ? (x) : (y))
#    define MAX_FRAMES(x, y) ((x) > (y) ? (x) : (y))
#elif OPTIMIZE_SET == 1
#    define INVALID_SET                        (-1)
#    define MIN_FRAMES(x, y)                   ((1 & (~(((x) - (y)) >> 31))) * (x) - (~(((x) - (y)) >> 31)) * (y))
#    define MAX_FRAMES(x, y)                   ((1 & (~(((y) - (x)) >> 31))) * (x) - (~(((y) - (x)) >> 31)) * (y))
#    define MINUS_1_TO_MAX_ELSE_NOTHING(x)     (0x7fffffff & (x))
#    define MINUS_1_TO_0_ELSE_MINUS_1(x)       (~((x) >> 31))
#    define IF_1ST_MINUS_1_TO_0_ELSE_2ND(x, y) ((~((x) >> 31)) & y)
#    define POS_INT_TO_0_ELSE_MINUS_1(x)       (((x)-1) >> 31)
#    define IF_1ST_POS_TO_0_ELSE_2ND(x, y)     ((((x)-1) >> 31) & y)
#    define MINUS_TO_0_ELSE_1(x)               ((((x) >> 31) + 1) * (x))
#elif OPTIMIZE_SET == 2
#    define INVALID_SET                  (JX3DPS::MAX_FRAMES_CONST)
#    define MIN_FRAMES(x, y)             ((1 & (~(((x) - (y)) >> 31))) * (x) - (~(((x) - (y)) >> 31)) * (y))
#    define MAX_FRAMES(x, y)             ((1 & (~(((y) - (x)) >> 31))) * (x) - (~(((y) - (x)) >> 31)) * (y))
#    define POS_INT_TO_0_ELSE_MINUS_1(x) (((x)-1) >> 31)
#    define MINUS_TO_0_ELSE_1(x)         ((((x) >> 31) + 1) * (x))
#endif // OPTIMIZE_SET

/* 取最大最小整数 */
#define GET_MAX_INT(x, y)              ((1 & (~(((x) - (y)) >> 31))) * (x) - (~(((y) - (x)) >> 31)) * (y))
#define GET_MIN_INT(x, y)              ((1 & (~(((y) - (x)) >> 31))) * (x) - (~(((x) - (y)) >> 31)) * (y))

/* skill调息结合充能判定 - 1st为正整数时, 返回0, 否则返回2nd */
#define IF_1ST_POS_TO_0_ELSE_2ND(x, y) ((((x)-1) >> 31) & (y))

/* 1st为0时, 返回0, 否则返回2nd */
#define IF_1ST_0_TO_0_ELSE_2ND(x, y)   (~(((x)-1) >> 31) & (y))

/* 1st为0时, 返回2nd, 否则返回3rd */
#define IF_1ST_0_TO_2ND_ELSE_3RD(a, b, c) \
    ((1 & (~(-((a)-1) >> 31))) * (c) - (~(((a)-1) >> 31)) * (b))

/* 判定伤害 */
#if JX3_LEVEL_VERSION >= 110
#    define GET_FINAL_DAMAGE(NormalDamage, RollResult, CriticalStrikePowerPercent)       \
        (static_cast<long long>((NormalDamage) * (1 + (static_cast<int>((RollResult))) * \
                                                          ((CriticalStrikePowerPercent)-1.0))))
#else
#    define GET_FINAL_DAMAGE(NormalDamage, RollResult, CriticalStrikePowerPercent)         \
        (static_cast<long long>(((NormalDamage) >> (static_cast<int>((RollResult)) / 2)) * \
                                (1 + (static_cast<int>((RollResult)) & 1) * ((CriticalStrikePowerPercent)-1.0))))
#endif // JX3_LEVEL_VERSION >= 110

#define JX3DPS_DAMAGE_ADD_PERCENT_SKILL
#define JX3DPS_DAMAGE_ADD_PERCENT_SHIELD
#define JX3DPS_DAMAGE_ADD_PERCENT_OVERCOME
#define JX3DPS_DAMAGE_ADD_PERCENT_CRITICAL
#define JX3DPS_DAMAGE_ADD_PERCENT_LEVEL
#define JX3DPS_DAMAGE_ADD_PERCENT_STRAIN
#define JX3DPS_DAMAGE_ADD_PERCENT_CLASS
#define JX3DPS_DAMAGE_ADD_PERCENT_VULNERABILITY

#endif // JX3DPS_OPTIMIZATION_H_