
#ifndef __JX3DPS_JX3_CONST_H__
#define __JX3DPS_JX3_CONST_H__

#include <array>
#include <string_view>

#include "type.h"
#include "version.h"

namespace JX3DPS {

/*----------------------- 游戏常数 -----------------------*/

/* 整型百分数基数100% */
constexpr pctn_t JX3_PERCENT_INT_BASE = 1024;

/* 浮点型百分数基数100% */
constexpr pctf_t JX3_PERCENT_FLOAT_BASE = 1.0;

/* 会心百分数基数100% */
constexpr bpn_t JX3_BASIS_POINT_INT_BASE = 10000;

/* 每秒帧数 */
constexpr frame_t JX3_FRAMES_PER_SECOND = 16;

/* 分钟秒数 */
constexpr frame_t JX3_SECONDS_PER_MINUTE = 60;

/* 公共冷却 */
constexpr frame_t JX3_GLOBAL_COOLDOWN = 24;

/* 角色基础命中 */
constexpr pctf_t JX3_PLAYER_HIT_VALUE_PERCENT_BASE = 0.9;

/* 角色基础会心效果 */
constexpr pctf_t JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE = 1.75;

/* 高等级等级差减伤比例 */
constexpr pctf_t JX3_LEVEL_DAMAGE_REDUCTION = 0.05;

/* 低等级等级差增伤比例 */
constexpr pctf_t JX3_LEVEL_DAMAGE_INCREASE = 0.15;

/* 最大等级差 */
constexpr int JX3_LEVEL_DAMAGE_MAX_LEVEL_DIFF = 10;

/* 外功伤害系数 */
constexpr cof_t JX3_PHYSICS_DAMAGE_PARAM = 10.0;

/* 内功伤害系数 */
constexpr cof_t JX3_MAGIC_DAMAGE_PARAM = 12.0;

/* 伤害常数 */
constexpr cof_t JX3_DAMAGE_CONST_PARAM = 16.0;

/* DOT伤害常数 */
constexpr cof_t JX3_DOT_DAMAGE_CONST_PARAM = 12.0;

/* 主属性转换系数 */
constexpr pctn_t JX3_AGILITY_TO_CRITICAL_STRIKE    = 655;
constexpr pctn_t JX3_STRENGTH_TO_ATTACK_POWER_BASE = 153;
constexpr pctn_t JX3_STRENGTH_TO_OVERCOME_BASE     = 307;
constexpr pctn_t JX3_SPIRIT_TO_CRITICAL_STRIKE     = 655;
constexpr pctn_t JX3_SPUNK_TO_ATTACK_POWER_BASE    = 184;
constexpr pctn_t JX3_SPUNK_TO_OVERCOME_BASE        = 307;

/* 角色等级 */
constexpr int JX3_PLAYER_LEVEL = JX3_LEVEL_VERSION;

#if JX3_LEVEL_VERSION == 120
/* 等级系数 */
constexpr int JX3_LEVEL_PARAM = 450;

/* 等级常数 */
constexpr int JX3_LEVEL_CONST = 45750;

/* 全局系数 */
constexpr cof_t JX3_CRITICAL_STRIKE_PARAM       = 9.530;
constexpr cof_t JX3_CRITICAL_STRIKE_POWER_PARAM = 3.335;
constexpr cof_t JX3_HIT_VALUE_PARAM             = 6.931;
constexpr cof_t JX3_INSIGHT_PARAM               = 9.189;
constexpr cof_t JX3_OVERCOME_PARAM              = 9.530;
constexpr cof_t JX3_HASTE_RATE                  = 11.695;
constexpr cof_t JX3_PHYSICS_SHIELD_PARAM        = 5.091;
constexpr cof_t JX3_MAGIC_SHIELD_PARAM          = 5.091;
constexpr cof_t JX3_SURPLUS_PARAM               = 13.192;
constexpr cof_t JX3_PARRY_PARAM                 = 4.345;
#elif JX3_LEVEL_VERSION == 110
/* 等级系数 */
constexpr int JX3_LEVEL_PARAM = 205;

/* 等级常数 */
constexpr int JX3_LEVEL_CONST = 18800;

/* 全局系数 */
constexpr cof_t JX3_CRITICAL_STRIKE_PARAM       = 9.530;
constexpr cof_t JX3_CRITICAL_STRIKE_POWER_PARAM = 3.335;
constexpr cof_t JX3_HIT_VALUE_PARAM             = 6.931;
constexpr cof_t JX3_INSIGHT_PARAM               = 9.189;
constexpr cof_t JX3_OVERCOME_PARAM              = 9.530;
constexpr cof_t JX3_HASTE_RATE                  = 11.695;
constexpr cof_t JX3_PHYSICS_SHIELD_PARAM        = 5.091;
constexpr cof_t JX3_MAGIC_SHIELD_PARAM          = 5.091;
constexpr cof_t JX3_SURPLUS_PARAM               = 2.557;
#elif JX3_LEVEL_VERSION == 100
/* 等级系数 */
constexpr int JX3_LEVEL_PARAM = 185;

/* 等级常数 */
constexpr int JX3_LEVEL_CONST = 16800;

/* 全局系数 */
constexpr cof_t JX3_CRITICAL_STRIKE_PARAM       = 9.026;
constexpr cof_t JX3_CRITICAL_STRIKE_POWER_PARAM = 3.159;
constexpr cof_t JX3_HIT_VALUE_PARAM             = 8.205;
constexpr cof_t JX3_INSIGHT_PARAM               = 5.128;
constexpr cof_t JX3_OVERCOME_PARAM              = 9.026;
constexpr cof_t JX3_HASTE_RATE                  = 11.077;
constexpr cof_t JX3_PHYSICS_SHIELD_PARAM        = 4.084;
constexpr cof_t JX3_MAGIC_SHIELD_PARAM          = 4.084;
#endif // JX3_LEVEL_VERSION

/* 熔嵌 - 主属性 */
constexpr value_t JX3_ENCHANTS_VALUE_PRIMARY[9] = { 0, 1, 3, 5, 7, 9, 10, 14, 18 };

/* 熔嵌 - 攻击 */
constexpr value_t JX3_ENCHANTS_VALUE_ATTACK[9] = { 0, 2, 5, 7, 10, 12, 15, 20, 26 };

/* 熔嵌 - 副属性 */
constexpr value_t JX3_ENCHANTS_VALUE_SUB[9] = { 0, 5, 11, 17, 22, 28, 34, 45, 58 };

/* 强化 - 系数 */
constexpr cof_t JX3_STRENGTHEN_LEVEL[9] = { 0.0,   0.005, 0.013, 0.024, 0.038,
                                            0.055, 0.075, 0.098, 0.124 };

/* 五彩石 - 主属性 */
constexpr cof_t JX3_STONE_PRIMARY = 5.175;

/* 五彩石 - 攻击 */
constexpr cof_t JX3_STONE_ATTACK = 10.35;

/* 五彩石 - 副属性 */
constexpr cof_t JX3_STONE_SUB = 23.125;

/* 五彩石 - 武器伤害 */
constexpr cof_t JX3_STONE_WEAPON_ATTACK = 15.525;

} // namespace JX3DPS

#endif // __JX3DPS_JX3_CONST_H__