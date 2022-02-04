#ifndef WUWOWUJIAN_H
#define WUWOWUJIAN_H

#include "Core/Skill.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class WuWoWuJian : public Skill
{
public:
    WuWoWuJian();
    ~WuWoWuJian();

    /* 执行 */
    void Cast(Player &player,
              TargetList &targetList,
              Stats::ThreadStats &threadStats,
              Stats::SIM_MODE &simMode);

public_customize_func:
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
    /* 判定 - 白虹 */
    Stats::TableResult GetRollResultBaiHong(Player &player, Target &target);

    /* 伤害计算 - 白虹 */
    Stats::DamageStats GetDamageBaiHong(Player &player, Target &target, Stats::TableResult tableResult);

    /* 伤害统计 - 白虹 */
    void RecordStatsBaiHong(Player &player,
                      Target &target,
                      Stats::ThreadStats &threadStats,
                      Stats::SIM_MODE &simMode,
                      Stats::TableResult tableResult);

    /* 附加效果 - 白虹 */
    void SubEffectBaiHong(Player &player,
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
    std::vector<Stats::DamageParam> m_damageParamVec;
    // std::map<std::string, std::vector<Stats::DamageParam>> m_damageParamVecMap;

private_customize_var:
    /* 气点加成 */
    int m_skillQidianAdd;

    /* 气点 */
    int m_qidian;
};

#endif // WUWOWUJIAN_H
