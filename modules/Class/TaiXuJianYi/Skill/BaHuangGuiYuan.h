#ifndef BAHUANGGUIYUAN_H
#define BAHUANGGUIYUAN_H

#include "Core/Skill.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class BaHuangGuiYuan : public Skill
{
public:
    BaHuangGuiYuan();
    BaHuangGuiYuan(Player *player);
    ~BaHuangGuiYuan();

    /* 执行 */
    void Cast(Player &player,
              TargetList &targetList,
              Stats::ThreadStats &threadStats,
              Stats::SIM_MODE &simMode);

    /* 减CD */
    void UpdateCooldown(int num);

    /* 气点加成 */
    void UpdateSkillQidian(int num);

private:
    /* 初始化基本信息 */
    void InitBaseParams();

    /* 初始化伤害系数 */
    void InitDamageParams();

    /* 判定 */
    Stats::TableResult GetRollResult(Player &player, Target &target);

    /* 伤害计算 */
    Stats::DamageStats GetDamage(Player &player, Target &target, Stats::TableResult tableResult);

    /* 伤害统计 */
    void RecordStats(Player &player,
                     Target &target,
                     Stats::ThreadStats &threadStats,
                     Stats::SIM_MODE &simMode,
                     Stats::TableResult tableResult);

    /* 附加效果 */
    void SubEffect(Player &player,
                   TargetList &targetList,
                   Stats::ThreadStats &threadStats,
                   Stats::SIM_MODE &simMode,
                   Stats::TableResult tableResult);

private_customize_func:
    /* 判定 - 2段 */
    Stats::TableResult GetRollResult2(Player &player, Target &target);

    /* 伤害计算 - 2段 */
    Stats::DamageStats GetDamage2(Player &player, Target &target, Stats::TableResult tableResult);

    /* 伤害统计 - 2段 */
    void RecordStats2(Player &player,
                      Target &target,
                      Stats::ThreadStats &threadStats,
                      Stats::SIM_MODE &simMode,
                      Stats::TableResult tableResult);

    /* 判定 - 和光 */
    Stats::TableResult GetRollResultHeGuang(Player &player, Target &target);

    /* 伤害计算 - 和光 */
    Stats::DamageStats GetDamageHeGuang(Player &player, Target &target, Stats::TableResult tableResult);

    /* 伤害统计 - 和光 */
    void RecordStatsHeGuang(Player &player,
                            Target &target,
                            Stats::ThreadStats &threadStats,
                            Stats::SIM_MODE &simMode,
                            Stats::TableResult tableResult);

private_var:
    /* CD */
    static int s_cooldown;

    /* 吟唱时间 */
    // static int s_prepareFrames;

    /* 作用间隔 */
    // static int s_intervalFrames;

    /* 最大作用次数 */
    // static int s_maxEffectNum;

    /* 最大充能数 */
    // static int s_maxEnergyNum;

    /* 伤害参数 */
    // Stats::DamageParam m_damageParam;
    // std::vector<Stats::DamageParam> m_damageParamVec;
    std::map<std::string, std::vector<Stats::DamageParam>> m_damageParamVecMap;

private_customize_var:
    /* 气点加成 */
    int m_skillQidianAdd;
};

}

}

#endif // BAHUANGGUIYUAN_H
