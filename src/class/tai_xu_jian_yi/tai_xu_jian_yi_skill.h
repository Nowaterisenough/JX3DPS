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
    static constexpr Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT;
    static constexpr int LEVEL = 3; // 0气, 1气, 2气

    PROPERTY tick_t cooldown = 0;
    PROPERTY tick_t *current_cooldown = nullptr;
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
    static constexpr Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS;
    static constexpr int LEVEL = 10;

    PROPERTY tick_t cooldown = 0;
    PROPERTY tick_t *current_cooldown = nullptr;
    PROPERTY int range = 4;

    // JX3BOX 2026-09-16: B=269*n/120, R=28*n/120, coefficient=0.053517*n.
    SKILL_DATA int base_damage[LEVEL] = {2, 4, 6, 8, 11, 13, 15, 17, 20, 22};
    SKILL_DATA int rand_damage[LEVEL] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2};
    SKILL_DATA cof_t physics_ap[LEVEL] = {
        0.053517, 0.107034, 0.160551, 0.214068, 0.267585,
        0.321102, 0.374619, 0.428136, 0.481653, 0.535170,
    };
    SKILL_DATA pctn_t weapon_coefficient[LEVEL] = {
        PCT_100 * 2, PCT_100 * 2, PCT_100 * 2, PCT_100 * 2, PCT_100 * 2,
        PCT_100 * 2, PCT_100 * 2, PCT_100 * 2, PCT_100 * 2, PCT_100 * 2
    };

    bool CheckResource() const;

    无我无剑();
};

/**
 * @brief 八荒归元 - AOE技能
 */
class 八荒归元 : public SkillImpl<八荒归元> {
public:
    static constexpr Tag TAG = Tag::AOE | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t cooldown = 192;
    PROPERTY tick_t *current_cooldown = nullptr;
    PROPERTY int range = 8;
    PROPERTY int limit = 5;

    PROPERTY int base_damage = 1034;
    PROPERTY int rand_damage = 0;
    PROPERTY cof_t physics_ap = 0.718652;
    PROPERTY pctn_t weapon_coefficient = PCT_100 * 2;
};

/**
 * @brief 三环套月 - 连环技能
 */
class 三环套月 : public SkillImpl<三环套月> {
public:
    static constexpr Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t cooldown = 32;
    PROPERTY tick_t *current_cooldown = nullptr;
    PROPERTY int range = 5;

    PROPERTY int base_damage = 442;
    PROPERTY int rand_damage = 0;
    PROPERTY cof_t physics_ap = 0.307199;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    三环套月();
};

/**
 * @brief 万剑归宗 - 爆发技能
 */
class 万剑归宗 : public SkillImpl<万剑归宗> {
public:
    static constexpr Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t cooldown = 192;
    PROPERTY tick_t *current_cooldown = nullptr;
    PROPERTY int range = 20;

    PROPERTY int base_damage = 177;
    PROPERTY int rand_damage = 8;
    PROPERTY cof_t physics_ap = 1.35;
    PROPERTY pctn_t weapon_coefficient = PCT_100 * 2;
};

// ========== 剑气场技能 ==========

/**
 * @brief 人剑合一 - 特殊DOT技能
 */
class 人剑合一 : public SkillImpl<人剑合一> {
public:
    static constexpr Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::INSTANT | Tag::PHYSICS | Tag::DOT;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t cooldown = 320;
    PROPERTY tick_t *current_cooldown = nullptr;
    PROPERTY int range = 20;

    PROPERTY int base_damage = 63;
    PROPERTY int rand_damage = 0;
    PROPERTY cof_t physics_ap = 0.027801;
    PROPERTY pctn_t weapon_coefficient = 0;

    人剑合一();
};

/**
 * @brief 碎星辰 - 读条技能, 生成剑气场
 */
class 碎星辰 : public SkillImpl<碎星辰> {
public:
    static constexpr Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t casting = 16;
    PROPERTY tick_t *current_casting = nullptr;
    PROPERTY tick_t cooldown = 160;
    PROPERTY tick_t *current_cooldown = nullptr;
    PROPERTY int range = 20;

    PROPERTY int base_damage = 0;
    PROPERTY int rand_damage = 0;
    PROPERTY cof_t physics_ap = 0.0;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    碎星辰();
};

/**
 * @brief 生太极 - 读条技能, 生成剑气场
 */
class 生太极 : public SkillImpl<生太极> {
public:
    static constexpr Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t casting = 16;
    PROPERTY tick_t *current_casting = nullptr;
    PROPERTY tick_t cooldown = 160;
    PROPERTY tick_t *current_cooldown = nullptr;
    PROPERTY int range = 20;

    PROPERTY int base_damage = 0;
    PROPERTY int rand_damage = 0;
    PROPERTY cof_t physics_ap = 0.0;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    生太极();
};

/**
 * @brief 吞日月 - 读条技能, 生成剑气场
 */
class 吞日月 : public SkillImpl<吞日月> {
public:
    static constexpr Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t casting = 16;
    PROPERTY tick_t *current_casting = nullptr;
    PROPERTY tick_t cooldown = 160;
    PROPERTY tick_t *current_cooldown = nullptr;
    PROPERTY int range = 20;

    PROPERTY int base_damage = 0;
    PROPERTY int rand_damage = 0;
    PROPERTY cof_t physics_ap = 0.0;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    吞日月();
};

// ========== 特殊技能 ==========

/**
 * @brief 三柴剑法 - 蓄力技能
 */
class 三柴剑法 : public SkillImpl<三柴剑法> {
public:
    static constexpr Tag TAG = Tag::TARGET | Tag::DAMAGE | Tag::MELEE | Tag::PHYSICS;
    static constexpr int LEVEL = 1;

    PROPERTY tick_t prepare = 32;
    PROPERTY tick_t *current_prepare = nullptr;
    PROPERTY tick_t cooldown = 0;
    PROPERTY tick_t *current_cooldown = nullptr;
    PROPERTY int range = 4;

    PROPERTY int base_damage = 21;
    PROPERTY int rand_damage = 0;
    PROPERTY cof_t physics_ap = 0.0;
    PROPERTY pctn_t weapon_coefficient = PCT_100;

    三柴剑法();
};

/**
 * @brief 紫气东来 - 充能技能, 恢复气点
 */
class 紫气东来 : public SkillImpl<紫气东来> {
public:
    static constexpr Tag TAG = Tag::SELF | Tag::INSTANT | Tag::NO_GCD;
    static constexpr int LEVEL = 1;

    PROPERTY int energy_count = 2;
    PROPERTY int energy_max = 2;
    PROPERTY tick_t energy_cooldown = 1200;
    PROPERTY int range = 0;

    紫气东来();
};

} // namespace 太虚剑意
} // namespace JX3DPS

#endif // JX3DPS_CLASS_TAI_XU_JIAN_YI_SKILLS_H
