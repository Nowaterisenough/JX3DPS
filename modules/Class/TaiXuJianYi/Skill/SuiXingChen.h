#ifndef SUIXINGCHEN_H
#define SUIXINGCHEN_H

#include "Core/Skill.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class SuiXingChen : public Skill
{
public:
    SuiXingChen(Player &player);
    SuiXingChen(const SuiXingChen &skill);
    ~SuiXingChen();
    SuiXingChen *Clone();
    SuiXingChen& operator=(const SuiXingChen &skill);

    /* 执行 */
    void Cast(TargetsMap &targetsMap, Stats &stats, CastType castType);

    /* 气点加成 */
    void UpdateSkillQidian(int num);

private:
    /* 初始化基本信息 */
    void InitBaseParams();

    /* 初始化伤害系数 */
    void InitDamageParams();

    /* 加成效果 */
    void SubEffect(TargetsMap &targetsMap, Stats &stats, TableRes tableRes);


    /* CD */
    static Frame_t s_cooldown;

    /* 吟唱时间 */
    static Frame_t s_prepareFrames;

    /* 作用间隔 */
    // static int s_intervalFrames;

    /* 最大作用次数 */
    // static int s_maxEffectNum;

    /* 最大充能数 */
    // static int s_maxEnergyNum;

    /* 气点加成 */
    int m_skillQidianAdd;
};

}

}

#endif // SUIXINGCHEN_H
