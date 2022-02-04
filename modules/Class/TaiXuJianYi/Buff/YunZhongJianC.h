#ifndef YUNZHONGJIANC_H
#define YUNZHONGJIANC_H

#include "Core/Buff.h"

namespace JX3DPS {

namespace TaiXuJianYi {

class YunZhongJianC : public Buff
{
public:
    YunZhongJianC(Player &player);
    YunZhongJianC(const YunZhongJianC &buff);
    ~YunZhongJianC();
    YunZhongJianC *Clone();
    YunZhongJianC& operator=(const YunZhongJianC &buff);

    /* 执行 */
    void Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings);

    /* 添加或刷新 */
    void Refresh();

    /* 结算 */
    void Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param);

private:
    /* 初始化基本信息 */
    void InitBaseParams();

    /* 初始化伤害系数 */
    void InitDamageParams();

    /* 加成效果 */
    void SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings);

private_var:
    /* 第三方冷却 */
    // static int s_3rdCooldown;

    /* 内置CD */
    // static int s_cooldown;

    /* 持续时间 */
    static Frame_t s_lastFrames;

    /* 作用间隔 */
    static Frame_t s_intervalFrames;

    /* 最大作用次数 */
    static int s_maxEffectNum;

    /* 最大可叠加层数 */
    // static int s_maxStackNum;

};

}

}

#endif // YUNZHONGJIANC_H
