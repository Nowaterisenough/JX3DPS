#ifndef JX3DPS_PLAYER_ATTRIBUTE_HPP
#define JX3DPS_PLAYER_ATTRIBUTE_HPP

#include "src/global/jx3.h"
#include "src/global/types.h"
#include <algorithm>

namespace JX3DPS {

/**
 * @brief 属性系统
 *
 * 统一管理角色所有属性，包括基础属性和加成属性
 */
class Attribute {
public:
    enum class Type {
        DEFAULT = 0,

        // 主属性
        AGILITY_BASE,                            // 身法
        AGILITY_BASE_ADDITIONAL_PERCENT_INT,     // 身法加成
        STRENGTH_BASE,                           // 力道
        STRENGTH_BASE_ADDITIONAL_PERCENT_INT,    // 力道加成
        SPIRIT_BASE,                             // 根骨
        SPIRIT_BASE_ADDITIONAL_PERCENT_INT,      // 根骨加成
        SPUNK_BASE,                              // 元气
        SPUNK_BASE_ADDITIONAL_PERCENT_INT,       // 元气加成

        // 武器伤害
        WEAPON_DAMAGE_BASE,                          // 基础武器伤害
        WEAPON_DAMAGE_RAND,                          // 浮动武器伤害
        WEAPON_DAMAGE_BASE_ADDITIONAL_PERCENT_INT,   // 武器伤害加成

        // 攻击力
        ATTACK_POWER_BASE,                                // 攻击力（通用）
        PHYSICS_ATTACK_POWER_BASE,                        // 外功攻击力
        MAGIC_ATTACK_POWER_BASE,                          // 内功攻击力（通用）
        NEUTRAL_ATTACK_POWER_BASE,                        // 混元性内功攻击力
        SOLAR_ATTACK_POWER_BASE,                          // 阳性内功攻击力
        LUNAR_ATTACK_POWER_BASE,                          // 阴性内功攻击力
        POISON_ATTACK_POWER_BASE,                         // 毒性内功攻击力
        ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT,         // 攻击力加成
        PHYSICS_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT, // 外功攻击力加成
        MAGIC_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT,   // 内功攻击力加成
        NEUTRAL_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT, // 混元性内功攻击力加成
        SOLAR_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT,   // 阳性内功攻击力加成
        LUNAR_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT,   // 阴性内功攻击力加成
        POISON_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT,  // 毒性内功攻击力加成

        // 会心
        CRITICAL_STRIKE,                                    // 会心等级（通用）
        PHYSICS_CRITICAL_STRIKE,                            // 外功会心等级
        MAGIC_CRITICAL_STRIKE,                              // 内功会心等级（通用）
        NEUTRAL_CRITICAL_STRIKE,                            // 混元性内功会心等级
        SOLAR_CRITICAL_STRIKE,                              // 阳性内功会心等级
        LUNAR_CRITICAL_STRIKE,                              // 阴性内功会心等级
        POISON_CRITICAL_STRIKE,                             // 毒性内功会心等级
        CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT,         // 会心加成
        PHYSICS_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT, // 外功会心加成
        MAGIC_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT,   // 内功会心加成
        NEUTRAL_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT, // 混元性内功会心加成
        SOLAR_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT,   // 阳性内功会心加成
        LUNAR_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT,   // 阴性内功会心加成
        POISON_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT,  // 毒性内功会心加成

        // 会心效果
        CRITICAL_STRIKE_POWER,                                    // 会心效果（通用）
        PHYSICS_CRITICAL_STRIKE_POWER,                            // 外功会心效果
        MAGIC_CRITICAL_STRIKE_POWER,                              // 内功会心效果（通用）
        NEUTRAL_CRITICAL_STRIKE_POWER,                            // 混元性内功会心效果
        SOLAR_CRITICAL_STRIKE_POWER,                              // 阳性内功会心效果
        LUNAR_CRITICAL_STRIKE_POWER,                              // 阴性内功会心效果
        POISON_CRITICAL_STRIKE_POWER,                             // 毒性内功会心效果
        CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,             // 会效加成
        PHYSICS_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,     // 外功会效加成
        MAGIC_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,       // 内功会效加成
        NEUTRAL_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,     // 混元性内功会效加成
        SOLAR_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,       // 阳性内功会效加成
        LUNAR_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,       // 阴性内功会效加成
        POISON_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT,      // 毒性内功会效加成

        // 破防
        OVERCOME_BASE,                                // 破防等级（通用）
        PHYSICS_OVERCOME_BASE,                        // 外功破防等级
        MAGIC_OVERCOME_BASE,                          // 内功破防等级（通用）
        NEUTRAL_OVERCOME_BASE,                        // 混元性内功破防等级
        SOLAR_OVERCOME_BASE,                          // 阳性内功破防等级
        LUNAR_OVERCOME_BASE,                          // 阴性内功破防等级
        POISON_OVERCOME_BASE,                         // 毒性内功破防等级
        OVERCOME_BASE_ADDITIONAL_PERCENT_INT,         // 破防加成
        PHYSICS_OVERCOME_BASE_ADDITIONAL_PERCENT_INT, // 外功破防加成
        MAGIC_OVERCOME_BASE_ADDITIONAL_PERCENT_INT,   // 内功破防加成
        NEUTRAL_OVERCOME_BASE_ADDITIONAL_PERCENT_INT, // 混元性内功破防加成
        SOLAR_OVERCOME_BASE_ADDITIONAL_PERCENT_INT,   // 阳性内功破防加成
        LUNAR_OVERCOME_BASE_ADDITIONAL_PERCENT_INT,   // 阴性内功破防加成
        POISON_OVERCOME_BASE_ADDITIONAL_PERCENT_INT,  // 毒性内功破防加成

        // 无双
        STRAIN_BASE,                          // 无双等级
        STRAIN_BASE_ADDITIONAL_PERCENT_INT,   // 无双加成

        // 破招
        SURPLUS_VALUE_BASE, // 破招值

        // 加速
        HASTE_BASE,                        // 加速等级
        HASTE_BASE_ADDITIONAL_PERCENT_INT, // 加速加成

        // 招架
        PARRY_BASE,                              // 招架等级
        PARRY_BASE_ADDITIONAL_PERCENT_INT,       // 招架加成
        PARRY_VALUE_BASE,                        // 招架值
        PARRY_VALUE_BASE_ADDITIONAL_PERCENT_INT, // 招架值加成

        // 无视防御
        SHIELD_IGNORE_PERCENT_INT,         // 无视防御
        PHYSICS_SHIELD_IGNORE_PERCENT_INT, // 外功无视防御
        MAGIC_SHIELD_IGNORE_PERCENT_INT,   // 内功无视防御

        // 伤害加成
        DAMAGE_ADDITIONAL_PERCENT_INT,     // 伤害加成
        PVE_DAMAGE_ADDITIONAL_PERCENT_INT, // PVE伤害加成

        COUNT
    };

    // 属性名称
    static constexpr const char *const ATTRIBUTE_NAMES[] = {
        "默认",
        "身法", "身法加成",
        "力道", "力道加成",
        "根骨", "根骨加成",
        "元气", "元气加成",
        "基础武器伤害", "浮动武器伤害", "武器伤害加成",
        "攻击力", "外功攻击力", "内功攻击力",
        "混元性内功攻击力", "阳性内功攻击力", "阴性内功攻击力", "毒性内功攻击力",
        "攻击力加成", "外功攻击力加成", "内功攻击力加成",
        "混元性内功攻击力加成", "阳性内功攻击力加成", "阴性内功攻击力加成", "毒性内功攻击力加成",
        "会心等级", "外功会心等级", "内功会心等级",
        "混元性内功会心等级", "阳性内功会心等级", "阴性内功会心等级", "毒性内功会心等级",
        "会心加成", "外功会心加成", "内功会心加成",
        "混元性内功会心加成", "阳性内功会心加成", "阴性内功会心加成", "毒性内功会心加成",
        "会心效果", "外功会心效果", "内功会心效果",
        "混元性内功会心效果", "阳性内功会心效果", "阴性内功会心效果", "毒性内功会心效果",
        "会效加成", "外功会效加成", "内功会效加成",
        "混元性内功会效加成", "阳性内功会效加成", "阴性内功会效加成", "毒性内功会效加成",
        "破防等级", "外功破防等级", "内功破防等级",
        "混元性内功破防等级", "阳性内功破防等级", "阴性内功破防等级", "毒性内功破防等级",
        "破防加成", "外功破防加成", "内功破防加成",
        "混元性内功破防加成", "阳性内功破防加成", "阴性内功破防加成", "毒性内功破防加成",
        "无双等级", "无双加成",
        "破招值",
        "加速等级", "加速加成",
        "招架等级", "招架加成", "招架值", "招架值加成",
        "无视防御", "外功无视防御", "内功无视防御",
        "伤害加成", "PVE伤害加成",
    };

    // 构造函数
    Attribute() { std::fill(std::begin(m_values), std::end(m_values), 0); }

    // 属性访问
    value_t Get(Type type) const { return m_values[static_cast<size_t>(type)]; }

    void Set(Type type, value_t value) { m_values[static_cast<size_t>(type)] = value; }

    void Add(Type type, value_t value) { m_values[static_cast<size_t>(type)] += value; }

    // ===== 计算属性 (外功) =====

    value_t GetPhysicsAttackPower() const {
        value_t base = Get(Type::PHYSICS_ATTACK_POWER_BASE);
        base += Get(Type::ATTACK_POWER_BASE);

        // 主属性转换
        base += Get(Type::STRENGTH_BASE) * JX3_STRENGTH_TO_ATTACK_POWER_BASE / JX3_PERCENT_INT_BASE;

        // 加成
        pctn_t percent = Get(Type::PHYSICS_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetPhysicsCriticalStrike() const {
        value_t base = Get(Type::PHYSICS_CRITICAL_STRIKE);
        base += Get(Type::CRITICAL_STRIKE);

        // 主属性转换
        base += Get(Type::AGILITY_BASE) * JX3_AGILITY_TO_CRITICAL_STRIKE / JX3_PERCENT_INT_BASE;

        return base;
    }

    pctf_t GetPhysicsCriticalStrikePercent() const {
        value_t cs    = GetPhysicsCriticalStrike();
        bpn_t bpAdd   = Get(Type::PHYSICS_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);

        double percent = static_cast<double>(cs) / (JX3_CRITICAL_STRIKE_PARAM * (JX3_PLAYER_LEVEL + JX3_LEVEL_PARAM));
        percent += static_cast<double>(bpAdd) / JX3_BASIS_POINT_INT_BASE;

        return percent;
    }

    value_t GetPhysicsCriticalStrikePower() const {
        value_t base = Get(Type::PHYSICS_CRITICAL_STRIKE_POWER);
        base += Get(Type::CRITICAL_STRIKE_POWER);

        pctn_t percent = Get(Type::PHYSICS_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetPhysicsOvercome() const {
        value_t base = Get(Type::PHYSICS_OVERCOME_BASE);
        base += Get(Type::OVERCOME_BASE);

        // 主属性转换
        base += Get(Type::STRENGTH_BASE) * JX3_STRENGTH_TO_OVERCOME_BASE / JX3_PERCENT_INT_BASE;

        pctn_t percent = Get(Type::PHYSICS_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::OVERCOME_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    // ===== 计算属性 (内功 - 通用) =====

    value_t GetMagicAttackPower() const {
        value_t base = Get(Type::MAGIC_ATTACK_POWER_BASE);
        base += Get(Type::ATTACK_POWER_BASE);

        // 主属性转换
        base += Get(Type::SPUNK_BASE) * JX3_SPUNK_TO_ATTACK_POWER_BASE / JX3_PERCENT_INT_BASE;

        pctn_t percent = Get(Type::MAGIC_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetMagicCriticalStrike() const {
        value_t base = Get(Type::MAGIC_CRITICAL_STRIKE);
        base += Get(Type::CRITICAL_STRIKE);

        // 主属性转换
        base += Get(Type::SPIRIT_BASE) * JX3_SPIRIT_TO_CRITICAL_STRIKE / JX3_PERCENT_INT_BASE;

        return base;
    }

    pctf_t GetMagicCriticalStrikePercent() const {
        value_t cs  = GetMagicCriticalStrike();
        bpn_t bpAdd = Get(Type::MAGIC_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);

        double percent = static_cast<double>(cs) / (JX3_CRITICAL_STRIKE_PARAM * (JX3_PLAYER_LEVEL + JX3_LEVEL_PARAM));
        percent += static_cast<double>(bpAdd) / JX3_BASIS_POINT_INT_BASE;

        return percent;
    }

    value_t GetMagicCriticalStrikePower() const {
        value_t base = Get(Type::MAGIC_CRITICAL_STRIKE_POWER);
        base += Get(Type::CRITICAL_STRIKE_POWER);

        pctn_t percent = Get(Type::MAGIC_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetMagicOvercome() const {
        value_t base = Get(Type::MAGIC_OVERCOME_BASE);
        base += Get(Type::OVERCOME_BASE);

        // 主属性转换
        base += Get(Type::SPUNK_BASE) * JX3_SPUNK_TO_OVERCOME_BASE / JX3_PERCENT_INT_BASE;

        pctn_t percent = Get(Type::MAGIC_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::OVERCOME_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    // ===== 计算属性 (内功 - 分类型) =====

    value_t GetNeutralAttackPower() const {
        value_t base = Get(Type::NEUTRAL_ATTACK_POWER_BASE);
        base += GetMagicAttackPower();

        pctn_t percent = Get(Type::NEUTRAL_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetSolarAttackPower() const {
        value_t base = Get(Type::SOLAR_ATTACK_POWER_BASE);
        base += GetMagicAttackPower();

        pctn_t percent = Get(Type::SOLAR_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetLunarAttackPower() const {
        value_t base = Get(Type::LUNAR_ATTACK_POWER_BASE);
        base += GetMagicAttackPower();

        pctn_t percent = Get(Type::LUNAR_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetPoisonAttackPower() const {
        value_t base = Get(Type::POISON_ATTACK_POWER_BASE);
        base += GetMagicAttackPower();

        pctn_t percent = Get(Type::POISON_ATTACK_POWER_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    // 混元性会心
    value_t GetNeutralCriticalStrike() const {
        value_t base = Get(Type::NEUTRAL_CRITICAL_STRIKE);
        base += GetMagicCriticalStrike();
        return base;
    }

    pctf_t GetNeutralCriticalStrikePercent() const {
        value_t cs  = GetNeutralCriticalStrike();
        bpn_t bpAdd = Get(Type::NEUTRAL_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::MAGIC_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);

        double percent = static_cast<double>(cs) / (JX3_CRITICAL_STRIKE_PARAM * (JX3_PLAYER_LEVEL + JX3_LEVEL_PARAM));
        percent += static_cast<double>(bpAdd) / JX3_BASIS_POINT_INT_BASE;

        return percent;
    }

    value_t GetNeutralCriticalStrikePower() const {
        value_t base = Get(Type::NEUTRAL_CRITICAL_STRIKE_POWER);
        base += GetMagicCriticalStrikePower();

        pctn_t percent = Get(Type::NEUTRAL_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::MAGIC_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetNeutralOvercome() const {
        value_t base = Get(Type::NEUTRAL_OVERCOME_BASE);
        base += GetMagicOvercome();

        pctn_t percent = Get(Type::NEUTRAL_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::MAGIC_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::OVERCOME_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    // 阳性会心
    value_t GetSolarCriticalStrike() const {
        value_t base = Get(Type::SOLAR_CRITICAL_STRIKE);
        base += GetMagicCriticalStrike();
        return base;
    }

    pctf_t GetSolarCriticalStrikePercent() const {
        value_t cs  = GetSolarCriticalStrike();
        bpn_t bpAdd = Get(Type::SOLAR_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::MAGIC_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);

        double percent = static_cast<double>(cs) / (JX3_CRITICAL_STRIKE_PARAM * (JX3_PLAYER_LEVEL + JX3_LEVEL_PARAM));
        percent += static_cast<double>(bpAdd) / JX3_BASIS_POINT_INT_BASE;

        return percent;
    }

    value_t GetSolarCriticalStrikePower() const {
        value_t base = Get(Type::SOLAR_CRITICAL_STRIKE_POWER);
        base += GetMagicCriticalStrikePower();

        pctn_t percent = Get(Type::SOLAR_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::MAGIC_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetSolarOvercome() const {
        value_t base = Get(Type::SOLAR_OVERCOME_BASE);
        base += GetMagicOvercome();

        pctn_t percent = Get(Type::SOLAR_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::MAGIC_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::OVERCOME_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    // 阴性会心
    value_t GetLunarCriticalStrike() const {
        value_t base = Get(Type::LUNAR_CRITICAL_STRIKE);
        base += GetMagicCriticalStrike();
        return base;
    }

    pctf_t GetLunarCriticalStrikePercent() const {
        value_t cs  = GetLunarCriticalStrike();
        bpn_t bpAdd = Get(Type::LUNAR_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::MAGIC_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);

        double percent = static_cast<double>(cs) / (JX3_CRITICAL_STRIKE_PARAM * (JX3_PLAYER_LEVEL + JX3_LEVEL_PARAM));
        percent += static_cast<double>(bpAdd) / JX3_BASIS_POINT_INT_BASE;

        return percent;
    }

    value_t GetLunarCriticalStrikePower() const {
        value_t base = Get(Type::LUNAR_CRITICAL_STRIKE_POWER);
        base += GetMagicCriticalStrikePower();

        pctn_t percent = Get(Type::LUNAR_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::MAGIC_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetLunarOvercome() const {
        value_t base = Get(Type::LUNAR_OVERCOME_BASE);
        base += GetMagicOvercome();

        pctn_t percent = Get(Type::LUNAR_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::MAGIC_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::OVERCOME_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    // 毒性会心
    value_t GetPoisonCriticalStrike() const {
        value_t base = Get(Type::POISON_CRITICAL_STRIKE);
        base += GetMagicCriticalStrike();
        return base;
    }

    pctf_t GetPoisonCriticalStrikePercent() const {
        value_t cs  = GetPoisonCriticalStrike();
        bpn_t bpAdd = Get(Type::POISON_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::MAGIC_CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);
        bpAdd += Get(Type::CRITICAL_STRIKE_ADDITIONAL_BASIS_POINT_INT);

        double percent = static_cast<double>(cs) / (JX3_CRITICAL_STRIKE_PARAM * (JX3_PLAYER_LEVEL + JX3_LEVEL_PARAM));
        percent += static_cast<double>(bpAdd) / JX3_BASIS_POINT_INT_BASE;

        return percent;
    }

    value_t GetPoisonCriticalStrikePower() const {
        value_t base = Get(Type::POISON_CRITICAL_STRIKE_POWER);
        base += GetMagicCriticalStrikePower();

        pctn_t percent = Get(Type::POISON_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::MAGIC_CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::CRITICAL_STRIKE_POWER_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    value_t GetPoisonOvercome() const {
        value_t base = Get(Type::POISON_OVERCOME_BASE);
        base += GetMagicOvercome();

        pctn_t percent = Get(Type::POISON_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::MAGIC_OVERCOME_BASE_ADDITIONAL_PERCENT_INT);
        percent += Get(Type::OVERCOME_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    // ===== 通用属性 =====

    value_t GetStrainBase() const {
        value_t base   = Get(Type::STRAIN_BASE);
        pctn_t percent = Get(Type::STRAIN_BASE_ADDITIONAL_PERCENT_INT);

        return base * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

    pctf_t GetHastePercent() const {
        value_t haste = Get(Type::HASTE_BASE);

        double percent = static_cast<double>(haste) / (JX3_HASTE_RATE * (JX3_PLAYER_LEVEL + JX3_LEVEL_PARAM));

        return 1.0 / (1.0 + percent);
    }

    value_t GetWeaponDamage() const {
        value_t base = Get(Type::WEAPON_DAMAGE_BASE);
        value_t rand = Get(Type::WEAPON_DAMAGE_RAND);

        pctn_t percent = Get(Type::WEAPON_DAMAGE_BASE_ADDITIONAL_PERCENT_INT);

        return (base + rand / 2) * (JX3_PERCENT_INT_BASE + percent) / JX3_PERCENT_INT_BASE;
    }

private:
    value_t m_values[static_cast<size_t>(Type::COUNT)] = {};
};

} // namespace JX3DPS

#endif // JX3DPS_PLAYER_ATTRIBUTE_HPP
