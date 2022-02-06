/**
 * @Description : Skill基类
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 15:05:25
 * @FilePath    : \JX3DPS\modules\Core\Skill.h
 */

#ifndef SKILL_H
#define SKILL_H

#include "Common/ConstVal.h"

namespace JX3DPS {

class Skill
{
public:
    Skill();
    virtual ~Skill();

    Skill(const Skill &skill);
    Skill &operator=(const Skill &skill);

    /* 虚复制构造函数 */
    virtual Skill *Clone() = 0;

    /**
     * @brief Skill执行函数
     * @param 1_targetsMap 目标全体.
     * @param 2_stats 统计.
     * @param 3_castType 执行类型, 包括预读条、效果、结算、强制.
     */
    virtual void Cast(Targets &targets, Stats &stats, CastType castType = EFFECT) = 0;

    /**
     * @brief Skill中断函数
     * @param 1_targetsMap 目标全体.
     * @param 2_stats 统计.
     */
    virtual void Interrupt(Targets &targets, Stats &stats);

    /**
     * @brief 设置所属角色函数
     * @param player 角色.
     */
    inline void SetPlayer(Player &player);

    /* 获取下次作用时间 */
    inline Frame_t GetNextTime() const;

    /**
     * @brief 获取可执行时间, 主要考虑充能技能
     */
    inline Frame_t GetEnableTime() const;

    /* 刷新时间 */
    inline void UpdateTime(Frame_t frames);

    /* ID */
    inline Id_t GetId() const;

    /* 名称 */
    inline const std::string &GetName() const;

    /* 后缀字段 */
    inline const std::string &GetSubName(int subIndex) const;

    /* 强度字段 */
    inline const std::string &GetLevelName(int levelIndex) const;

    /* 词缀数 */
    inline int GetSubCount() const;

    /* 强度层数 */
    inline int GetLevelCount() const;

    /* 充能数 */
    inline int GetEnergyCount() const;

    /* 冷却时间 */
    inline Frame_t GetCooldown() const;

    /* 公共冷却技能 */
    inline bool IsPublicSkill() const;

    /* 冷却加成 */
    inline void UpdateSkillCooldown(const Frame_t frames);

    /* 会心加成 */
    inline void UpdateSkillCriticalStrikePercent(PCT_t percent);

    /* 会心效果加成 */
    inline void UpdateSkillCriticalStrikePowerPercent(PCT_t percent);

    /* 伤害加成 */
    inline void UpdateSkillDamageBinPercent(BinPCT_t binPercent);

    /* 设置延迟 */
    inline static void SetDelay(int delayMin, int delayMax);

protected:
    /* 判定 */
    inline Table Roll(PCT_t playerCriticalStrikePercent) const;

    /* 外功判定 */
    Table GetPhysicsRollResult() const;

    /* 外功伤害 */
    inline Damage GetPhysicsDamage(const Target      &target,
                                   Table              table,
                                   const std::string &subName,
                                   int                level) const;

    /* 外功统计 */
    void UpdatePhysicsStats(const Target      &target,
                            Table              table,
                            const std::string &subName,
                            int                level,
                            Stats             &stats) const;

    /* 内功判定 */
    Table GetMagicRollResult() const;

    /* 内功伤害 */
    inline Damage
    GetMagicDamage(const Target &target, Table table, const std::string &subName, int level) const;

    /* 内功统计 */
    void UpdateMagicStats(const Target      &target,
                          Table              table,
                          const std::string &subName,
                          int                level,
                          Stats             &stats) const;

    /* 重置公共冷却 */
    void ResetPublicCooldown();

    /**
     * @brief 详细统计空函数: 计算循环属性如会心、加速的收益时通过函数指针调用, 不进行详细统计和收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_damage 伤害.
     * @param 6_stats 统计.
     */
    inline void StatsNoneDetail(const Target      &target,
                                Table              table,
                                const std::string &subName,
                                int                level,
                                const Damage      &damage,
                                Stats             &stats) const;

    /**
     * @brief 详细统计函数: 计算默认属性时通过函数指针调用, 进行详细统计和收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_damage 伤害.
     * @param 6_stats 统计.
     */
    void StatsDetail(const Target      &target,
                     Table              table,
                     const std::string &subName,
                     int                level,
                     const Damage      &damage,
                     Stats             &stats) const;

    /**
     * @brief 收益统计空函数: 不计算某属性收益时通过函数指针调用, 不进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    inline void StatsNoneBonus(const Target      &target,
                               Table              table,
                               const std::string &subName,
                               int                level,
                               Stats             &stats) const;

    /**
     * @brief 武器伤害外功收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsPhysicsWeaponAttackBonus(const Target      &target,
                                       Table              table,
                                       const std::string &subName,
                                       int                level,
                                       Stats             &stats) const;

    /**
     * @brief 武器伤害内功收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsMagicWeaponAttackBonus(const Target      &target,
                                     Table              table,
                                     const std::string &subName,
                                     int                level,
                                     Stats             &stats) const;

    /**
     * @brief 外功基础攻击收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsPhysicsAttackBaseBonus(const Target      &target,
                                     Table              table,
                                     const std::string &subName,
                                     int                level,
                                     Stats             &stats) const;

    /**
     * @brief 内功基础攻击收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsMagicAttackBaseBonus(const Target      &target,
                                   Table              table,
                                   const std::string &subName,
                                   int                level,
                                   Stats             &stats) const;

    /**
     * @brief 外功会心效果收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsPhysicsCriticalStrikePowerBonus(const Target      &target,
                                              Table              table,
                                              const std::string &subName,
                                              int                level,
                                              Stats             &stats) const;

    /**
     * @brief 内功会心效果收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsMagicCriticalStrikePowerBonus(const Target      &target,
                                            Table              table,
                                            const std::string &subName,
                                            int                level,
                                            Stats             &stats) const;

    /**
     * @brief 外功基础破防收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsPhysicsOvercomeBaseBonus(const Target      &target,
                                       Table              table,
                                       const std::string &subName,
                                       int                level,
                                       Stats             &stats) const;

    /**
     * @brief 内功基础破防收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsMagicOvercomeBaseBonus(const Target      &target,
                                     Table              table,
                                     const std::string &subName,
                                     int                level,
                                     Stats             &stats) const;

    /**
     * @brief 无双收益外功统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsPhysicsStrainBonus(const Target      &target,
                                 Table              table,
                                 const std::string &subName,
                                 int                level,
                                 Stats             &stats) const;

    /**
     * @brief 无双收益内功统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsMagicStrainBonus(const Target      &target,
                               Table              table,
                               const std::string &subName,
                               int                level,
                               Stats             &stats) const;

    /**
     * @brief 破招收益外功统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsPhysicsSurplusBonus(const Target      &target,
                                  Table              table,
                                  const std::string &subName,
                                  int                level,
                                  Stats             &stats) const;

    /**
     * @brief 破招收益内功统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_target 目标.
     * @param 2_tableRes 判定结果, 包括: 命中; 会心.
     * @param 3_subName 词缀.
     * @param 4_level 强度.
     * @param 5_stats 统计.
     */
    void StatsMagicSurplusBonus(const Target      &target,
                                Table              table,
                                const std::string &subName,
                                int                level,
                                Stats             &stats) const;

protected:
    /* 角色 */
    Player *m_player = nullptr;

    /* ping范围 */
    static int s_delayMin;
    static int s_delayMax;

    /* ID */
    Id_t m_id = 0;

    /* 名称 */
    std::string m_name = nullptr;

    /* 词缀字段 */
    std::vector<std::string> m_subNames;

    /* 强度字段 */
    std::vector<std::string> m_levelNames;

    /* 公共冷却 */
    static Frame_t s_publicCooldown;
    Frame_t       *publicCooldown     = nullptr;
    Frame_t        m_isPublicCooldown = NOT_PUBLIC_SKILL;

    /* 技能CD */
    Frame_t m_cooldown = 0;

    /* 吟唱时间 */
    Frame_t m_prepareFrames = INVALID_FRAMES_SET;

    /* 作用间隔 */
    Frame_t m_intervalFrames = INVALID_FRAMES_SET;

    /* 作用次数 */
    int m_effectCount = 0;

    /* 充能数 */
    int m_energyCount = 0;

    /* 冷却加成 */
    Frame_t m_skillCooldownAdd = 0;

    /* 技能会心加成 */
    PCT_t m_skillCriticalStrikePercentAdd = 0;

    /* 技能会心效果加成 */
    PCT_t m_skillCriticalStrikePowerPercentAdd = 0;

    /* 技能伤害加成 */
    PCT_t m_skillDamageBinPercentAdd = 0;

    /* 技能命中加成 */
    PCT_t m_skillHitValuePercentAdd = 0;

    /* 伤害参数 */
    DamageParams m_damageParams;

    /* 详细统计 */
    void (Skill::*m_statsDetail)(const Target &,
                                 Table,
                                 const std::string &,
                                 int,
                                 const Damage &,
                                 Stats &) const = &Skill::StatsNoneDetail;

    /* 收益统计 - 武器伤害 */
    void (Skill::*m_statsWeaponAttackBonus)(const Target &,
                                            Table,
                                            const std::string &,
                                            int,
                                            Stats &) const = &Skill::StatsNoneBonus;

    /* 收益统计 - 基础攻击 */
    void (Skill::*m_statsAttackBaseBonus)(const Target &,
                                          Table,
                                          const std::string &,
                                          int,
                                          Stats &) const = &Skill::StatsNoneBonus;

    /* 收益统计 - 会心效果 */
    void (Skill::*m_statsCriticalStrikePowerBonus)(const Target &,
                                                   Table,
                                                   const std::string &,
                                                   int,
                                                   Stats &) const = &Skill::StatsNoneBonus;

    /* 收益统计 - 基础破防 */
    void (Skill::*m_statsOvercomeBaseBonus)(const Target &,
                                            Table,
                                            const std::string &,
                                            int,
                                            Stats &) const = &Skill::StatsNoneBonus;

    /* 收益统计 - 无双 */
    void (Skill::*m_statsStrainBonus)(const Target &,
                                      Table,
                                      const std::string &,
                                      int,
                                      Stats &) const = &Skill::StatsNoneBonus;

    /* 收益统计 - 破招 */
    void (Skill::*m_statsSurplusBonus)(const Target &,
                                       Table,
                                       const std::string &,
                                       int,
                                       Stats &) const = &Skill::StatsNoneBonus;
};

} // namespace JX3DPS

#endif // SKILL_H