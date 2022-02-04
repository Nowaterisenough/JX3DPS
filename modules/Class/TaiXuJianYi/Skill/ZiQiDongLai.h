#ifndef ZIQIDONGLAI_H
#define ZIQIDONGLAI_H

#include "Core/Skill.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class ZiQiDongLai : public Skill
{
public:
    ZiQiDongLai(Player &player);
    ZiQiDongLai(const ZiQiDongLai &skill);
    ~ZiQiDongLai();
    ZiQiDongLai *Clone();
    ZiQiDongLai& operator=(const ZiQiDongLai &skill);

    /* 执行 */
    void Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType);

private:
    /* 初始化基本信息 */
    void InitBaseParams();

    /* 初始化伤害系数 */
    void InitDamageParams();

    /* 加成效果 */
    void SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings, TableRes tableRes);

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

};

}

}

#endif // ZIQIDONGLAI_H
