#ifndef WUWOWUJIAN_H
#define WUWOWUJIAN_H

#include "Core/Skill.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class WuWoWuJian : public Skill
{
public:
    WuWoWuJian(Player &player);
    WuWoWuJian(const WuWoWuJian &skill);
    ~WuWoWuJian();
    WuWoWuJian *Clone();
    WuWoWuJian& operator=(const WuWoWuJian &skill);

    /* 执行 */
    void Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType);

    /* 气点加成 */
    void UpdateSkillQidian(int num);

private:
    /* 初始化基本信息 */
    void InitBaseParams();

    /* 初始化伤害系数 */
    void InitDamageParams();

    /* 外功判定 */
    TableRes GetPhysicsRollResultBaiHong(Target &target, RollType rollType = RollType::COMMON);

    /* 外功伤害 */
    Damage GetPhysicsDamageBaiHong(Target &target,
                                        TableRes tableRes,
                                        std::string &subName,
                                        int level);

    /* 外功统计 */
    void UpdatePhysicsStatsBaiHong(Target &target,
                                   Stats &stats,
                                   Settings &settings,
                                   TableRes tableRes,
                                   std::string &subName,
                                   int level);

    /* 加成效果 */
    void SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings, TableRes tableRes);

    /* 加成效果 */
    void SubEffectBaiHong(TargetsMap &targetsMap, Stats &stats, Settings &settings, TableRes tableRes);


    /* CD */
    static Frame_t s_cooldown;

    /* 吟唱时间 */
    // static int s_prepareFrames;

    /* 作用间隔 */
    // static int s_intervalFrames;

    /* 最大作用次数 */
    // static int s_maxEffectNum;

    /* 最大充能数 */
    // static int s_maxEnergyNum;

    /* 气点加成 */
    int m_skillQidianAdd;

    /* 气点 */
    int m_qidian;
};

}

}

#endif // WUWOWUJIAN_H
