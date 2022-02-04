#ifndef CHIYING_H
#define CHIYING_H

#include "Core/Buff.h"

class ChiYing : public Buff
{
public:
    ChiYing();
    ~ChiYing();

    /* 执行 */
    void Cast(Player &player,
              TargetList &targetList,
              Stats::ThreadStats &threadStats,
              Stats::SIM_MODE &simMode);

    /* 添加或刷新 */
    void Refresh(Player &player);

    /* 结算 */
    void Clean(Player &player,
               Target &target,
               Stats::ThreadStats &threadStats,
               Stats::SIM_MODE &simMode);

    void Clean(Player &player,
               TargetList &targetList,
               Stats::ThreadStats &threadStats,
               Stats::SIM_MODE &simMode);

private:
    /* 初始化基本信息 */
    void InitBaseParams();

    /* 初始化伤害系数 */
    void InitDamageParams();

    /* 判定 */
    Stats::TableResult GetRollResult(Player &player, Target &target);

    /* 伤害计算 */
    Stats::DamageStats GetDamage(Player &player, Target &target, Stats::TableResult tableResult);

    /* 技能统计 */
    void RecordStats(Player &player,
                     Target &target,
                     Stats::ThreadStats &threadStats,
                     Stats::SIM_MODE &simMode,
                     Stats::TableResult tableResult);

    /* 附加效果 */
    // void SubEffect(Player &player, TargetList &targetList, Stats::ThreadStats threadStats, Stats::SIM_MODE simMode, Stats::TableResult tableResult);

private_var:
    /* 第三方冷却 */
    // static int s_3rdCooldown;

    /* 内置CD */
    // static int s_cooldown;

    /* 持续时间 */
    static int s_lastFrames;

    /* 作用间隔 */
    // static int s_intervalFrames;

    /* 最大作用次数 */
    // static int s_maxEffectNum;

    /* 最大可叠加层数 */
    static int s_maxStackNum;

    /* 伤害参数 */
    Stats::DamageParam m_damageParam;
    // std::vector<Stats::DamageParam> m_damageParamVec;
    // std::map<std::string, std::vector<Stats::DamageParam>> m_damageParamVecMap;

    /* 快照属性 */
    // SnapshotAttribute m_snapshotAttribute;

    /* 目标 */
    // Target *m_target;
};

#endif // CHIYING_H
