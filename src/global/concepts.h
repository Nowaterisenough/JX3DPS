#ifndef JX3DPS_GLOBAL_CONCEPTS_H
#define JX3DPS_GLOBAL_CONCEPTS_H

#include "types.h"
#include <concepts>
#include <type_traits>

namespace JX3DPS {

// ============= 技能字段检测 Concepts =============

/**
 * @brief 检测技能是否有前摇
 */
template <typename T>
concept HasPrepare = requires(T t) {
    { T::prepare } -> std::convertible_to<tick_t>;
    { T::current_prepare } -> std::convertible_to<tick_t *>;
};

/**
 * @brief 检测技能是否有读条
 */
template <typename T>
concept HasCasting = requires(T t) {
    { T::casting } -> std::convertible_to<tick_t>;
    { T::current_casting } -> std::convertible_to<tick_t *>;
};

/**
 * @brief 检测技能是否有物理攻击力系数
 */
template <typename T>
concept HasPhysicsAP = requires(T t) {
    { T::physics_ap };
};

/**
 * @brief 检测技能是否有混元性内功攻击力系数
 */
template <typename T>
concept HasNeutralAP = requires(T t) {
    { T::neutral_ap };
};

/**
 * @brief 检测技能是否有阳性内功攻击力系数
 */
template <typename T>
concept HasSolarAP = requires(T t) {
    { T::solar_ap };
};

/**
 * @brief 检测技能是否有阴性内功攻击力系数
 */
template <typename T>
concept HasLunarAP = requires(T t) {
    { T::lunar_ap };
};

/**
 * @brief 检测技能是否有毒性内功攻击力系数
 */
template <typename T>
concept HasPoisonAP = requires(T t) {
    { T::poison_ap };
};

/**
 * @brief 检测技能是否有魔法攻击力系数（任意一种内功类型）
 */
template <typename T>
concept HasMagicAP = HasNeutralAP<T> || HasSolarAP<T> || HasLunarAP<T> || HasPoisonAP<T>;

/**
 * @brief 检测技能是否有武器伤害系数
 */
template <typename T>
concept HasWeaponDamage = requires(T t) {
    { T::weapon_coefficient };
};

/**
 * @brief 检测技能是否有充能
 */
template <typename T>
concept HasEnergy = requires(T t) {
    { T::energy_count } -> std::convertible_to<int>;
    { T::energy_max } -> std::convertible_to<int>;
};

/**
 * @brief 检测技能是否有多段伤害
 */
template <typename T>
concept HasMultiSub = requires(T t) {
    { T::SUB } -> std::convertible_to<int>;
} && (T::SUB > 1);

/**
 * @brief 检测技能是否有冷却
 */
template <typename T>
concept HasCooldown = requires(T t) {
    { T::cooldown } -> std::convertible_to<tick_t>;
    { T::current_cooldown } -> std::convertible_to<tick_t *>;
};

/**
 * @brief 检测技能是否无GCD
 */
template <typename T>
concept HasNoGCD = requires(T t) {
    { T::TAG } -> std::convertible_to<Tag>;
} && ((T::TAG & Tag::NO_GCD) == Tag::NO_GCD);

/**
 * @brief 检测技能是否有增伤
 */
template <typename T>
concept HasDamageAdd = requires(T t) {
    { T::damage_add } -> std::convertible_to<pctn_t>;
};

/**
 * @brief 检测技能是否有资源消耗检查
 */
template <typename T>
concept HasResourceCheck = requires(T t) {
    { t.CheckResource() } -> std::convertible_to<bool>;
};

// ============= BUFF字段检测 Concepts =============

/**
 * @brief 检测BUFF是否有间隔触发
 */
template <typename T>
concept HasInterval = requires(T t) {
    { T::interval } -> std::convertible_to<tick_t>;
    { T::current_interval } -> std::convertible_to<tick_t *>;
};

/**
 * @brief 检测是否为DOT类型BUFF
 */
template <typename T>
concept IsDebuff = requires(T t) {
    { T::TAG } -> std::convertible_to<Tag>;
} && ((T::TAG & Tag::DOT) == Tag::DOT);

/**
 * @brief 检测是否为HOT类型BUFF
 */
template <typename T>
concept IsBuff = requires(T t) {
    { T::TAG } -> std::convertible_to<Tag>;
} && ((T::TAG & Tag::HOT) == Tag::HOT);

/**
 * @brief 检测BUFF是否受加速影响
 */
template <typename T>
concept IsHasted = requires(T t) {
    { T::TAG } -> std::convertible_to<Tag>;
} && ((T::TAG & Tag::HASTED) == Tag::HASTED);

/**
 * @brief 检测BUFF是否有层数管理
 */
template <typename T>
concept HasStackNum = requires(T t) {
    { T::stack_num } -> std::convertible_to<int>;
};

/**
 * @brief 检测BUFF是否有伤害
 */
template <typename T>
concept HasBuffDamage = requires(T t) {
    { T::base_damage };
} || requires(T t) {
    { T::TAG } -> std::convertible_to<Tag>;
} && ((T::TAG & Tag::DAMAGE) == Tag::DAMAGE);

// ============= 玩家检测 Concepts =============

/**
 * @brief 检测是否为Player类型
 *
 * 注意：Player是模板类，这里检测T是否有标准Player接口
 */
template <typename T>
concept IsPlayer = requires(T t) {
    { t.GetQidian() } -> std::convertible_to<int>;
    { t.SetQidian(0) } -> std::same_as<void>;
};

/**
 * @brief 检测是否为技能类型
 */
template <typename T>
concept IsSkill = requires(T t) {
    typename T::Self;
    { t.Cast() };
    { t.Tick() };
};

/**
 * @brief 检测是否为BUFF类型
 */
template <typename T>
concept IsBuffType = requires(T t) {
    typename T::Self;
    typename T::Snapshot;
    { t.Add(jx3id_t{}, int{}, tick_t{}, tick_t{}) };
    { t.Tick() };
};

// ============= 工具 Concepts =============

/**
 * @brief 检测是否为数组类型
 */
template <typename T>
concept IsArray = std::is_array_v<T>;

/**
 * @brief 检测是否为整数类型
 */
template <typename T>
concept IsIntegral = std::is_integral_v<T>;

/**
 * @brief 检测是否为浮点类型
 */
template <typename T>
concept IsFloatingPoint = std::is_floating_point_v<T>;

} // namespace JX3DPS

#endif // JX3DPS_GLOBAL_CONCEPTS_H
