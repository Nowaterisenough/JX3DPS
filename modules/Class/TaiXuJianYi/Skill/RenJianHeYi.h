#ifndef RENJIANHEYI_H
#define RENJIANHEYI_H

#include "Core/Skill.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class RenJianHeYi : public Skill
{
public:
    RenJianHeYi(Player &player);
    RenJianHeYi(const RenJianHeYi &skill);
    ~RenJianHeYi();
    RenJianHeYi *Clone();
    RenJianHeYi& operator=(const RenJianHeYi &skill);

    /* 执行 */
    void Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType);

    /* 气点加成 */
    void UpdateSkillQidian(int num);

private:
    /* 初始化基本信息 */
    void InitBaseParams();

    /* 初始化伤害系数 */
    void InitDamageParams();

    /* 加成效果 */
    void SubEffect(Target &target, Stats &stats, Settings &settings, TableRes tableRes);

private_var:
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
};

}

}

#endif // RENJIANHEYI_H
