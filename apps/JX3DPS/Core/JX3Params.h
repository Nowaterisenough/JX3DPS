/**
 * Project: JX3DPS
 * File: JX3Params.h
 * Description:
 * Created Date: 2023-06-01 19:56:32
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-12 07:03:53
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef JX3PARAMS_H
#define JX3PARAMS_H

#include <string>
#include <vector>

#include "JX3DPSTypes.h"
#include "Version.h"

namespace JX3DPS {

inline const std::vector<std::string> &CLASS = {
    "太虚剑意", "紫霞功", "惊羽诀", "天罗诡道", "傲血战意", "冰心诀", "分山劲",
    "焚影圣诀", "问水诀", "花间游", "凌海诀",   "莫问",     "笑尘诀", "易筋经",
    "北傲诀",   "无方",   "隐龙诀", "太玄经",   "孤锋诀",   "毒经"
};

/*----------------------- 游戏常数 -----------------------*/

/* 整型百分数基数100% */
const PctInt_t JX3_PCT_INT_BASE = 1024;

/* 浮点型百分数基数100% */
const PctFloat_t JX3_PCT_FLOAT_BASE = 1.0;

/* 会心百分数基数100% */
const PctFloat_t JX3_CRITICAL_STRIKE_PERCENT_BASE = 10000.0;

/* 每秒帧数 */
const Frame_t JX3_FRAMES_PER_SECOND = 16;

/* 高等级等级差减伤比例 */
const PctFloat_t JX3_LEVEL_DAMAGE_REDUCTION = 0.05;

/* 低等级等级差增伤比例 */
const PctFloat_t JX3_LEVEL_DAMAGE_INCREASE = 0.15;

/* 最大等级差 */
const int JX3_LEVEL_DAMAGE_MAX_LEVEL_DIFF = 10;

/* 外功伤害系数 */
const PctFloat_t JX3_PHYSICS_DAMAGE_PARAM = 10;

/* 内功伤害系数 */
const PctFloat_t JX3_MAGIC_DAMAGE_PARAM = 12;

/* 伤害常数 */
const PctFloat_t JX3_DAMAGE_CONST_PARAM = 16;

/* DOT伤害常数 */
const PctFloat_t JX3_DOT_DAMAGE_CONST_PARAM = 12;



/* 角色等级 */
const int JX3_PLAYER_LEVEL = JX3_LEVEL_VERSION;

#if JX3_LEVEL_VERSION == 120
/* 等级系数 */
const int JX3_LEVEL_PARAM = 450;

/* 等级常数 */
const int JX3_LEVEL_CONST = 45750;

/* 全局系数 */
const double JX3_CRITICAL_STRIKE_PARAM       = 9.530;
const double JX3_CRITICAL_STRIKE_POWER_PARAM = 3.335;
const double JX3_HIT_VALUE_PARAM             = 6.931;
const double JX3_INSIGHT_PARAM               = 9.189;
const double JX3_OVERCOME_PARAM              = 9.530;
const double JX3_HASTE_RATE                  = 11.695;
const double JX3_PHYSICS_SHIELD_PARAM        = 5.091;
const double JX3_MAGIC_SHIELD_PARAM          = 5.091;
const double JX3_SURPLUS_PARAM               = 13.192;
#elif JX3_LEVEL_VERSION == 110
/* 等级系数 */
const int JX3_LEVEL_PARAM = 205;

/* 等级常数 */
const int JX3_LEVEL_CONST = 18800;

/* 全局系数 */
const double JX3_CRITICAL_STRIKE_PARAM       = 9.530;
const double JX3_CRITICAL_STRIKE_POWER_PARAM = 3.335;
const double JX3_HIT_VALUE_PARAM             = 6.931;
const double JX3_INSIGHT_PARAM               = 9.189;
const double JX3_OVERCOME_PARAM              = 9.530;
const double JX3_HASTE_RATE                  = 11.695;
const double JX3_PHYSICS_SHIELD_PARAM        = 5.091;
const double JX3_MAGIC_SHIELD_PARAM          = 5.091;
const double JX3_SURPLUS_PARAM               = 2.557;
#elif JX3_LEVEL_VERSION == 100
/* 等级系数 */
const int JX3_LEVEL_PARAM = 185;

/* 等级常数 */
const int JX3_LEVEL_CONST = 16800;

/* 全局系数 */
const double JX3_CRITICAL_STRIKE_PARAM       = 9.026;
const double JX3_CRITICAL_STRIKE_POWER_PARAM = 3.159;
const double JX3_HIT_VALUE_PARAM             = 8.205;
const double JX3_INSIGHT_PARAM               = 5.128;
const double JX3_OVERCOME_PARAM              = 9.026;
const double JX3_HASTE_RATE                  = 11.077;
const double JX3_PHYSICS_SHIELD_PARAM        = 4.084;
const double JX3_MAGIC_SHIELD_PARAM          = 4.084;
#endif // JX3_LEVEL_VERSION

/* 主属性转换系数 */
const double JX3_AGILITY_TO_CRITICAL_STRIKE = 0.64;
const double JX3_STRENGTH_TO_ATTACK_BASE    = 0.15;
const double JX3_STRENGTH_TO_OVERCOME_BASE  = 0.3;
const double JX3_SPIRIT_TO_CRITICAL_STRIKE  = 0.64;
const double JX3_SPUNK_TO_ATTACK_BASE       = 0.18;
const double JX3_SPUNK_TO_OVERCOME_BASE     = 0.3;

const double JX3_AGILITY_TO_ATTACK_TAI_XU_JIAN_YI          = 1.45;
const double JX3_AGILITY_TO_CRITICAL_STRIKE_TAI_XU_JIAN_YI = 0.58;

/* 公共冷却 */
const Frame_t JX3_GLOBAL_COOLDOWN = 16 + 8;

/* 角色基础命中 */
const PctFloat_t JX3_PLAYER_HIT_VALUE_PERCENT_BASE = 0.9;

/* 角色基础会心效果 */
const PctFloat_t JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE = 1.75;

/* 熔嵌 - 主属性 */
const Value_t JX3_ENCHANTS_VALUE_PRIMARY[9] = { 0, 1, 3, 5, 7, 9, 10, 14, 18 };

/* 熔嵌 - 攻击 */
const Value_t JX3_ENCHANTS_VALUE_ATTACK[9] = { 0, 2, 5, 7, 10, 12, 15, 20, 26 };

/* 熔嵌 - 副属性 */
const Value_t JX3_ENCHANTS_VALUE_SUB[9] = { 0, 5, 11, 17, 22, 28, 34, 45, 58 };

/* 强化 - 系数 */
const double JX3_STRENGTHEN_LEVEL[9] = { 0.0, 0.005, 0.013, 0.024, 0.038, 0.055, 0.075, 0.098, 0.124 };

/* 五彩石 - 主属性 */
const double JX3_STONE_PRIMARY = 5.175;

/* 五彩石 - 攻击 */
const double JX3_STONE_ATTACK = 10.35;

/* 五彩石 - 副属性 */
const double JX3_STONE_SUB = 23.125;

/* 五彩石 - 武器伤害 */
const double JX3_STONE_WEAPON_ATTACK = 15.525;

} // namespace JX3DPS

#endif // JX3PARAMS_H