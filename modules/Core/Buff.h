/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-06 13:50:25
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 15:05:33
 * @FilePath    : \JX3DPS\modules\Core\Buff.h
 */
/**
 * @Description : Buff基类
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-06 13:53:14
 * @FilePath    : \JX3DPS\modules\Core\Buff.h
 */

#ifndef BUFF_H
#define BUFF_H

#include "Common/ConstVal.h"

class Attr;

namespace JX3DPS {

class Buff
{
public:
    Buff();
    virtual ~Buff();

    Buff(const Buff &buff);
    Buff &operator=(const Buff &buff);

    /* 虚复制构造函数 */
    virtual Buff *Clone() = 0;

    /**
     * @brief Buff执行函数
     * @param 1_targetsMap 目标全体.
     * @param 2_stats 统计.
     */
    virtual void Cast(Targets &targets, Stats &stats) = 0;

    /* Buff刷新函数 */
    virtual void Refresh() = 0;

    /**
     * @brief Buff清除函数
     * @param 1_targetsMap 目标全体.
     * @param 2_stats 统计.
     * @param 3_castType 执行类型(主动: 如切玉; 被动: 强制去除, 效果及伤害不生效).

     */
    virtual void Clean(Targets &targets, Stats &stats, CastType castType = EFFECT) = 0;

    /**
     * @brief 设置所属角色函数
     * @param player 角色.
     */
    inline void SetPlayer(Player &player);

    /**
     * @brief 设置所属目标函数
     * @param target 目标.
     */
    inline void SetTarget(Target &target);

    /* 获取下次作用时间 */
    virtual Frame_t GetNextTime() const;

    /**
     * @brief 更新下次作用时间
     * @param frames 更新帧数
     */
    inline void UpdateTime(const Frame_t frames);

    /* 判断Buff是否存在 */
    inline bool IsExist() const;

    /* 伤害性 */
    inline bool IsDamage() const;

    /* 获取ID */
    inline Id_t GetId() const;

    /* 获取名称 */
    inline const std::string &GetName() const;

    /**
     * @brief 获取词缀
     * @param subIndex 词缀序数
     */
    inline const std::string &GetSubName(int subIndex) const;

    /**
     * @brief 获取强度名称
     * @param levelIndex 强度
     */
    inline const std::string &GetLevelName(int levelIndex) const;

    /* 获取词缀数 */
    inline int GetSubCount() const;

    /* 获取强度层数 */
    inline int GetLevelCount() const;

    /* 获取层数 */
    inline int GetStackCount() const;

    /* 获取剩余作用次数 */
    inline int GetEffectCount() const;

    /* 获取内置CD */
    inline Frame_t GetCooldown() const;

    /* 获取剩余持续时间 */
    inline Frame_t GetRestTime() const;

protected:
#if JX3DPS_MAJOR_VERSION >= 110
    /**
     * @brief Roll点函数
     * @param playerCriticalStrikePercent 角色会心百分比.
     * @return 判定结果, 包括: 命中; 会心.
     */
    Table Roll(PCT_t playerCriticalStrikePercent) const;
#else
    Table Buff::Roll(PCT_t    playerHitPercent,
                        PCT_t    playerCriticalStrikePercent,
                        PCT_t    playerStrainPercent,
                        PCT_t    targetMissPercent,
                        PCT_t    targetSightPercent,
                        const RollType rollType = RollType::COMMON);
#endif // JX3DPS_MAJOR_VERSION >= 110

    /**
     * @brief 外功判定函数
     * @param attr 角色属性(有时为快照属性).
     * @return 判定结果, 包括: 命中; 会心.
     */
    inline Table GetPhysicsRollResult(const Attr &attr) const;

    /**
     * @brief 外功伤害函数
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @return 伤害结果.
     */
    Damage GetPhysicsDamage(const Attr        &attr,
                            const Target      &target,
                            Table     table,
                            const std::string &subName,
                            int          level) const;

    /**
     * @brief 外功统计函数
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计
     */
    void UpdatePhysicsStats(const Attr        &attr,
                            const Target      &target,
                            Table     table,
                            const std::string &subName,
                            int          level,
                            Stats             &stats) const;

    /**
     * @brief 内功判定函数
     * @param attr 角色属性(有时为快照属性).
     * @return 判定结果, 包括: 命中; 会心; 无双.
     */
    inline Table GetMagicRollResult(const Attr &attr) const;

    /**
     * @brief 内功伤害函数
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @return 伤害结果.
     */
    Damage GetMagicDamage(const Attr        &attr,
                          const Target      &target,
                          Table     table,
                          const std::string &subName,
                          int          level) const;

    /**
     * @brief 内功统计函数
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void UpdateMagicStats(const Attr        &attr,
                          const Target      &target,
                          Table     table,
                          const std::string &subName,
                          int          level,
                          Stats             &stats) const;

    /**
     * @brief 详细统计空函数: 计算循环属性如会心、加速的收益时通过函数指针调用, 不进行详细统计和收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_damage 伤害.
     * @param 7_stats 统计.
     */
    inline void StatsNoneDetail(const Attr        &attr,
                                const Target      &target,
                                Table     table,
                                const std::string &subName,
                                int          level,
                                const Damage      &damage,
                                Stats             &stats) const;

    /**
     * @brief 详细统计函数: 计算默认属性时通过函数指针调用, 进行详细统计和收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_damage 伤害.
     * @param 7_stats 统计.
     */
    void StatsDetail(const Attr        &attr,
                     const Target      &target,
                     Table     table,
                     const std::string &subName,
                     int          level,
                     const Damage      &damage,
                     Stats             &stats) const;

    /**
     * @brief 收益统计空函数: 不计算某属性收益时通过函数指针调用, 不进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    inline void StatsNoneBonus(const Attr        &attr,
                               const Target      &target,
                               Table     table,
                               const std::string &subName,
                               int          level,
                               Stats             &stats) const;

    /**
     * @brief 武器伤害外功收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsPhysicsWeaponAttackBonus(const Attr        &attr,
                                       const Target      &target,
                                       Table     table,
                                       const std::string &subName,
                                       int          level,
                                       Stats             &stats) const;

    /**
     * @brief 武器伤害内功收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsMagicWeaponAttackBonus(const Attr        &attr,
                                     const Target      &target,
                                     Table     table,
                                     const std::string &subName,
                                     int          level,
                                     Stats             &stats) const;

    /**
     * @brief 外功基础攻击收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsPhysicsAttackBaseBonus(const Attr        &attr,
                                     const Target      &target,
                                     Table     table,
                                     const std::string &subName,
                                     int          level,
                                     Stats             &stats) const;

    /**
     * @brief 内功基础攻击收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsMagicAttackBaseBonus(const Attr        &attr,
                                   const Target      &target,
                                   Table     table,
                                   const std::string &subName,
                                   int          level,
                                   Stats             &stats) const;

    /**
     * @brief 外功会心效果收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsPhysicsCriticalStrikePowerBonus(const Attr        &attr,
                                              const Target      &target,
                                              Table     table,
                                              const std::string &subName,
                                              int          level,
                                              Stats             &stats) const;

    /**
     * @brief 内功会心效果收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsMagicCriticalStrikePowerBonus(const Attr        &attr,
                                            const Target      &target,
                                            Table     table,
                                            const std::string &subName,
                                            int          level,
                                            Stats             &stats) const;

    /**
     * @brief 外功基础破防收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsPhysicsOvercomeBaseBonus(const Attr        &attr,
                                       const Target      &target,
                                       Table     table,
                                       const std::string &subName,
                                       int          level,
                                       Stats             &stats) const;

    /**
     * @brief 内功基础破防收益统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsMagicOvercomeBaseBonus(const Attr        &attr,
                                     const Target      &target,
                                     Table     table,
                                     const std::string &subName,
                                     int          level,
                                     Stats             &stats) const;

    /**
     * @brief 无双收益外功统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsPhysicsStrainBonus(const Attr        &attr,
                                 const Target      &target,
                                 Table     table,
                                 const std::string &subName,
                                 int          level,
                                 Stats             &stats) const;

    /**
     * @brief 无双收益内功统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsMagicStrainBonus(const Attr        &attr,
                               const Target      &target,
                               Table     table,
                               const std::string &subName,
                               int          level,
                               Stats             &stats) const;

    /**
     * @brief 破招收益外功统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsPhysicsSurplusBonus(const Attr        &attr,
                                  const Target      &target,
                                  Table     table,
                                  const std::string &subName,
                                  int          level,
                                  Stats             &stats) const;

    /**
     * @brief 破招收益内功统计函数: 通过函数指针调用进行该属性的收益计算统计.
     * @param 1_attr 角色属性(有时为快照属性).
     * @param 2_target 目标.
     * @param 3_tableRes 判定结果, 包括: 命中; 会心.
     * @param 4_subName 词缀.
     * @param 5_level 强度.
     * @param 6_stats 统计.
     */
    void StatsMagicSurplusBonus(const Attr        &attr,
                                const Target      &target,
                                Table     table,
                                const std::string &subName,
                                int          level,
                                Stats             &stats) const;

    /* 角色 */
    Player *m_player = nullptr;

    /* 目标 */
    Target *m_target = nullptr;

    /* 快照属性 */
    Attr *m_attrSnap = nullptr;

    /* ID */
    Id_t m_id = 0;

    /* 名称 */
    std::string m_name = nullptr;

    /* 词缀字段 */
    std::vector<std::string> m_subNames;

    /* 强度字段 */
    std::vector<std::string> m_levelNames;

    /* 第三方冷却 */
    Frame_t m_3rdCooldown = INVALID_FRAMES_SET;

    /* 内置CD */
    Frame_t m_cooldown = INVALID_FRAMES_SET;

    /* 持续时间 */
    Frame_t m_lastFrames = INVALID_FRAMES_SET;

    /* 作用间隔 */
    Frame_t m_intervalFrames = INVALID_FRAMES_SET;

    /* 作用次数 */
    int m_effectCount = 0;

    /* 叠加层数 */
    int m_stackCount = 0;

    /* 伤害参数 */
    DamageParams m_damageParams;

    /* 伤害性 */
    bool m_isDamage = false;

    /* 详细统计 */
    void (Buff::*m_statsDetail)(const Attr &,
                                const Target &,
                                Table,
                                const std::string &,
                                int,
                                const Damage &,
                                Stats &) const = &Buff::StatsNoneDetail;

    /* 收益统计 - 武器伤害 */
    void (Buff::*m_statsWeaponAttackBonus)(const Attr &,
                                           const Target &,
                                           Table,
                                           const std::string &,
                                           int,
                                           Stats &) const = &Buff::StatsNoneBonus;

    /* 收益统计 - 基础攻击 */
    void (Buff::*m_statsAttackBaseBonus)(const Attr &,
                                         const Target &,
                                         Table,
                                         const std::string &,
                                         int,
                                         Stats &) const = &Buff::StatsNoneBonus;

    /* 收益统计 - 会心效果 */
    void (Buff::*m_statsCriticalStrikePowerBonus)(const Attr &,
                                                  const Target &,
                                                  Table,
                                                  const std::string &,
                                                  int,
                                                  Stats &) const = &Buff::StatsNoneBonus;

    /* 收益统计 - 基础破防 */
    void (Buff::*m_statsOvercomeBaseBonus)(const Attr &,
                                           const Target &,
                                           Table,
                                           const std::string &,
                                           int,
                                           Stats &) const = &Buff::StatsNoneBonus;

    /* 收益统计 - 无双 */
    void (Buff::*m_statsStrainBonus)(const Attr &,
                                     const Target &,
                                     Table,
                                     const std::string &,
                                     int,
                                     Stats &) const = &Buff::StatsNoneBonus;

    /* 收益统计 - 破招 */
    void (Buff::*m_statsSurplusBonus)(const Attr &,
                                      const Target &,
                                      Table,
                                      const std::string &,
                                      int,
                                      Stats &) const = &Buff::StatsNoneBonus;
};

} // namespace JX3DPS

#endif // BUFF_H
