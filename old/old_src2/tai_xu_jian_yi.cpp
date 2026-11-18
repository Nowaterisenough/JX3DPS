#include "tai_xu_jian_yi.h"

using namespace JX3DPS;

太虚剑意::Player::Player()
{
    BindGlobalCoolDown(24, 无我无剑, 八荒归元, 三环套月, 万剑归宗, 人剑合一, 生太极, 碎星辰, 吞日月);

    BindGlobalCoolDown(16, 三柴剑法);

    BindGlobalCoolDown(48, 无我无剑, 三环套月, 万剑归宗);

    BindGlobalCoolDown(32, 人剑合一, 生太极, 碎星辰, 吞日月);

    Register(POST_ROLL, 人剑合一, 无我无剑, 八荒归元, 三环套月, [this]() {
        if (self.roll == DOUBLE) {
            AddBuff(云中剑·碎星辰);
        }
    });
}