/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-04 12:08:10
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 13:27:31
 * @FilePath    : \JX3DPS\modules\Class\TaiXuJianYi\Skill\ZiQiDongLai.h
 */

#ifndef ZIQIDONGLAI_H
#define ZIQIDONGLAI_H

#include "Core/Skill.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class ZiQiDongLai : public Skill
{
public:
    ZiQiDongLai();
    ~ZiQiDongLai();

    /* 执行 */
    void Cast(Player             &player,
              TargetList         &targetList,
              Stats::ThreadStats &threadStats,
              Stats::SIM_MODE    &simMode);

private:
    /* 初始化基本信息 */
    void InitBaseParams();

    /* 初始化伤害系数 */
    // void InitDamageParams();

    /* 判定 */
    // Stats::TableResult GetRollResult(Player &player, Target &target);

    /* 伤害计算 */
    // Stats::DamageStats GetDamage(Player &player, Target &target, Stats::TableResult tableResult);

    /* 伤害统计 */
    void RecordStats(Player             &player,
                     Target             &target,
                     Stats::ThreadStats &threadStats,
                     Stats::SIM_MODE    &simMode,
                     Stats::TableResult  tableResult);

    /* 附加效果 */
    void SubEffect(Player             &player,
                   TargetList         &targetList,
                   Stats::ThreadStats &threadStats,
                   Stats::SIM_MODE    &simMode,
                   Stats::TableResult  tableResult);

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
    // std::map<std::string, std::vector<Stats::DamageParam>> m_damageParamVecMap;
};

#endif // ZIQIDONGLAI_H
