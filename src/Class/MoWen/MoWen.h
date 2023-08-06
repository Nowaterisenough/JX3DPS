/**
 * Project: JX3DPS
 * File: MoWen.h
 * Description:
 * Created Date: 2023-07-31 16:03:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-06 04:11:47
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __JX3DPS_CLASS_MO_WEN_H__
#define __JX3DPS_CLASS_MO_WEN_H__

#include "Player.h"

namespace JX3DPS {

namespace MoWen {

class Player : public JX3DPS::Player
{
public:
    Player();

    Player(const Player &other) : JX3DPS::Player(other) { }

    Player &operator=(const JX3DPS::Player &other) override
    {
        if (this == &other) {
            return *this;
        }

        JX3DPS::Player::operator=(other);
        return *this;
    }

    Player *Clone() const override { return new Player(*this); }

    void Init() override;

    inline void UpdateGlobalCooldown(Frame_t next) override
    {
        globalCooldownCurrent -= next;
        globalCooldownCurrent = std::max(globalCooldownCurrent, 0);

        cooldownStyleCurrent -= next;
        cooldownStyleCurrent = std::max(cooldownStyleCurrent, 0);

        cooldownGaoShanLiuShuiCurrent -= next;
        cooldownGaoShanLiuShuiCurrent = std::max(cooldownGaoShanLiuShuiCurrent, 0);
    }

    enum class Style
    {
        YANG_CHUN_BAI_XUE = 0,
        GAO_SHAN_LIU_SHUI
    };

    Style style;

    int styleCount = 0;

    inline void AddStyleCount(int count)
    {
        styleCount += (count + static_cast<int>(style));
    }

    Frame_t cooldownStyleCurrent = 0;

    Frame_t cooldownGaoShanLiuShuiCurrent = 0;

    static void TriggerXianFeng(const Params &params);

    static void TriggerXianFengBiaoJi(const Params &params);

    static void TriggerXianFengDamage(const Params &params);

    static void TriggerXianFengClear(const Params &params);

    static void TriggerYunHan(const Params &params);

    static void TriggerCanLianAdd(const Params &params);

    static void TriggerCanLianClear(const Params &params);

    static void TriggerShuLi(const Params &params);

    static void TriggerShiXiang(const Params &params);

    static void TriggerZhiZhi(const Params &params);

    static void TriggerHaoQingZhi(const Params &params);

    static void TriggerHaoQingBianZhi(const Params &params);

    static void TriggerLiuZhaoDamage(const Params &params);

    static void TriggerLiuZhaoSurplusDamage(const Params &params);
};

} // namespace MoWen

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_MO_WEN_H__