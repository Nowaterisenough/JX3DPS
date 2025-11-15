#ifndef JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILLS_H
#define JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILLS_H

#include "src/core/context.h"
#include "src/skill/skill_impl.hpp"
#include "tai_xu_jian_yi_ids.h"

namespace JX3DPS {
namespace 太虚剑意 {

// 前向声明
class Player;

// ========== 基础技能 ==========

/**
 * @brief 破招 - 破招伤害技能
 */
class 破招 : public SkillImpl<破招> {
public:
    CONSTANT Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT;
    static constexpr int LEVEL = 3; // 0气, 1气, 2气

    PROPERTY tick_t cooldown = 0;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[0];
    PROPERTY int range = 4;

    SKILL_DATA int physics_surplus_cof[LEVEL] = {
        static_cast<int>(PCT_100 * PCT_100 * 0.065),
        static_cast<int>(PCT_100 * PCT_100 * 0.13),
        static_cast<int>(PCT_100 * PCT_100 * 0.23),
    };
};

/**
 * @brief 无我无剑 - 核心输出技能
 */
class 无我无剑 : public SkillImpl<无我无剑> {
public:
    CONSTANT Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS;
    static constexpr int LEVEL = 10;

    PROPERTY tick_t cooldown = 24;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[1];
    PROPERTY int range = 4;

    SKILL_DATA int base_damage[LEVEL] = {224, 449, 673, 898, 1122, 1347, 1571, 1795, 2020, 2244};
    SKILL_DATA int rand_damage[LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    SKILL_DATA cof_t physics_ap[LEVEL] = {
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 1,
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 2,
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 3,
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 4,
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 5,
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 6,
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 7,
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 8,
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 9,
        19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 10,
    };
    SKILL_DATA pctn_t weapon_coefficient[LEVEL] = {
        PCT_100, PCT_100, PCT_100, PCT_100, PCT_100,
        PCT_100, PCT_100, PCT_100, PCT_100, PCT_100
    };

    无我无剑();
};

/**
 * @brief 八荒归元 - AOE技能
 */
class 八荒归元 : public SkillImpl<八荒归元> {
public:
    CONSTANT Tag TAG = Tag::AOE | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t cooldown = 480;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[2];
    PROPERTY int range = 8;
    PROPERTY int limit = 5;

    PROPERTY int base_damage = 1080;
    PROPERTY int rand_damage = 180;
    PROPERTY cof_t physics_ap = 234.3;
    PROPERTY pctn_t weapon_coefficient = PCT_100;
};

/**
 * @brief 三环套月 - 连环技能
 */
class 三环套月 : public SkillImpl<三环套月> {
public:
    CONSTANT Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t cooldown = 160;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[3];
    PROPERTY int range = 5;

    PROPERTY int base_damage = 360;
    PROPERTY int rand_damage = 60;
    PROPERTY cof_t physics_ap = 78.1;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    三环套月();
};

/**
 * @brief 万剑归宗 - 爆发技能
 */
class 万剑归宗 : public SkillImpl<万剑归宗> {
public:
    CONSTANT Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t cooldown = 1920;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[4];
    PROPERTY int range = 20;

    PROPERTY int base_damage = 2160;
    PROPERTY int rand_damage = 360;
    PROPERTY cof_t physics_ap = 468.6;
    PROPERTY pctn_t weapon_coefficient = PCT_100 * 2;
};

// ========== 剑气场技能 ==========

/**
 * @brief 人剑合一 - 特殊DOT技能
 */
class 人剑合一 : public SkillImpl<人剑合一> {
public:
    CONSTANT Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS | Tag::DOT;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t cooldown = 0;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[5];
    PROPERTY int range = 20;

    PROPERTY int base_damage = 216;
    PROPERTY int rand_damage = 36;
    PROPERTY cof_t physics_ap = 46.9;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    人剑合一();
};

/**
 * @brief 碎星辰 - 读条技能, 生成剑气场
 */
class 碎星辰 : public SkillImpl<碎星辰> {
public:
    CONSTANT Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t casting = 32;
    PROPERTY tick_t *current_casting = &context.cache.skill_casting[7];
    PROPERTY tick_t cooldown = 112;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[7];
    PROPERTY int range = 20;

    PROPERTY int base_damage = 1080;
    PROPERTY int rand_damage = 180;
    PROPERTY cof_t physics_ap = 234.3;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    碎星辰();
};

/**
 * @brief 生太极 - 读条技能, 生成剑气场
 */
class 生太极 : public SkillImpl<生太极> {
public:
    CONSTANT Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t casting = 32;
    PROPERTY tick_t *current_casting = &context.cache.skill_casting[8];
    PROPERTY tick_t cooldown = 112;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[8];
    PROPERTY int range = 20;

    PROPERTY int base_damage = 1080;
    PROPERTY int rand_damage = 180;
    PROPERTY cof_t physics_ap = 234.3;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    生太极();
};

/**
 * @brief 吞日月 - 读条技能, 生成剑气场
 */
class 吞日月 : public SkillImpl<吞日月> {
public:
    CONSTANT Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t casting = 32;
    PROPERTY tick_t *current_casting = &context.cache.skill_casting[9];
    PROPERTY tick_t cooldown = 112;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[9];
    PROPERTY int range = 20;

    PROPERTY int base_damage = 1080;
    PROPERTY int rand_damage = 180;
    PROPERTY cof_t physics_ap = 234.3;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    吞日月();
};

// ========== 特殊技能 ==========

/**
 * @brief 三柴剑法 - 蓄力技能
 */
class 三柴剑法 : public SkillImpl<三柴剑法> {
public:
    CONSTANT Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t prepare = 32;
    PROPERTY tick_t *current_prepare = &context.cache.skill_prepare[6];
    PROPERTY tick_t cooldown = 0;
    PROPERTY tick_t *current_cooldown = &context.cache.skill_cooldown[6];
    PROPERTY int range = 4;

    PROPERTY int base_damage = 648;
    PROPERTY int rand_damage = 108;
    PROPERTY cof_t physics_ap = 140.6;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    三柴剑法();
};

/**
 * @brief 紫气东来 - 充能技能, 恢复气点
 */
class 紫气东来 : public SkillImpl<紫气东来> {
public:
    CONSTANT Tag TAG = Tag::SELF | Tag::INSTANT | Tag::NO_GCD;
    static constexpr int LEVEL = 1;

    PROPERTY int energy_count = 2;
    PROPERTY int energy_max = 2;
    PROPERTY tick_t energy_cooldown = 480;
    PROPERTY int range = 0;

    紫气东来();
};

} // namespace 太虚剑意
} // namespace JX3DPS

#endif // JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILLS_H
