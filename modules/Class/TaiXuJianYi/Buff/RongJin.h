#ifndef RONGJIN_H
#define RONGJIN_H

#include "Core/Buff.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class RongJin : public Buff
{
public:
    RongJin(Player &player);
    RongJin(const RongJin &buff);
    ~RongJin();
    RongJin *Clone();
    RongJin& operator=(const RongJin &buff);

    /* 执行 */
    void Cast(TargetsMap &targetsMap, Stats &stats);

    /* 添加或刷新 */
    void Refresh();

    /* 结算 */
    void Clean(TargetsMap &targetsMap, Stats &stats, int param);

private:
    /* 初始化基本信息 */
    void InitBaseParams();

    /* 初始化伤害系数 */
    // void InitDamageParams();


    /* 第三方冷却 */
    // static int s_3rdCooldown;

    /* 内置CD */
    // static int s_cooldown;

    /* 持续时间 */
    // static int s_lastFrames;

    /* 作用间隔 */
    static Frame_t s_intervalFrames;

    /* 最大作用次数 */
    // static int s_maxEffectNum;

    /* 最大可叠加层数 */
    // static int s_maxStackNum;

};

}

}

#endif // RONGJIN_H
