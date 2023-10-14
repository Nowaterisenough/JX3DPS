/**
 * Project: JX3DPS
 * File: MoWen.h
 * Description:
 * Created Date: 2023-07-31 16:03:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-14 20:36:22
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

    Player(const Player &other);

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

    Frame_t GetNextGlobalCooldown() const override
    {
        Frame_t frame = JX3DPS_INVALID_FRAMES_SET;
        if (globalCooldownCurrent > 0) {
            frame = globalCooldownCurrent;
        }
        if (cooldownStyleCurrent > 0) {
            frame = std::min(frame, cooldownStyleCurrent);
        }
        if (cooldownGaoShanLiuShuiCurrent > 0) {
            frame = std::min(frame, cooldownGaoShanLiuShuiCurrent);
        }
        if (cooldownYangChunBaiXueCurrent > 0) {
            frame = std::min(frame, cooldownYangChunBaiXueCurrent);
        }
        if (cooldownShuYingHengXieCurrent > 0) {
            frame = std::min(frame, cooldownShuYingHengXieCurrent);
        }
        if (cooldownGuYingHuaShuangCurrent > 0) {
            frame = std::min(frame, cooldownGuYingHuaShuangCurrent);
        }
        if (cooldownWuYinLiuLvCurrent > 0) {
            frame = std::min(frame, cooldownWuYinLiuLvCurrent);
        }
        return frame;
    }

    inline void UpdateGlobalCooldown(Frame_t next) override
    {
        globalCooldownCurrent -= next;
        globalCooldownCurrent  = std::max(globalCooldownCurrent, 0);

        cooldownStyleCurrent -= next;
        cooldownStyleCurrent  = std::max(cooldownStyleCurrent, 0);

        cooldownGaoShanLiuShuiCurrent -= next;
        cooldownGaoShanLiuShuiCurrent  = std::max(cooldownGaoShanLiuShuiCurrent, 0);

        cooldownYangChunBaiXueCurrent -= next;
        cooldownYangChunBaiXueCurrent  = std::max(cooldownYangChunBaiXueCurrent, 0);

        cooldownShuYingHengXieCurrent -= next;
        cooldownShuYingHengXieCurrent  = std::max(cooldownShuYingHengXieCurrent, 0);

        cooldownGuYingHuaShuangCurrent -= next;
        cooldownGuYingHuaShuangCurrent  = std::max(cooldownGuYingHuaShuangCurrent, 0);

        cooldownWuYinLiuLvCurrent -= next;
        cooldownWuYinLiuLvCurrent  = std::max(cooldownWuYinLiuLvCurrent, 0);
    }

    enum class Style
    {
        YANG_CHUN_BAI_XUE = 0,
        GAO_SHAN_LIU_SHUI
    };

    Style style = Style::GAO_SHAN_LIU_SHUI;

    Frame_t cooldownStyleCurrent = 0;

    Frame_t cooldownGaoShanLiuShuiCurrent = 0;

    Frame_t cooldownYangChunBaiXueCurrent = 0;

    Frame_t cooldownShuYingHengXieCurrent = 0;

    Frame_t cooldownGuYingHuaShuangCurrent = 0;

    Frame_t cooldownWuYinLiuLvCurrent = 0;

    static void TriggerXianFeng(const Params &params);

    static void TriggerXianFengBiaoJi(const Params &params);

    static void TriggerXianFengDamage(const Params &params);

    static void TriggerXianFengClear(const Params &params);

    static void TriggerYunHan(const Params &params);

    static void TriggerCanLianAdd(const Params &params);

    static void TriggerCanLianClear(const Params &params);

    static void TriggerMingJin(const Params &params);

    static void TriggerShiXiang(const Params &params);

    static void TriggerZhiZhi(const Params &params);

    static void TriggerZhengMingSurplusDamage(const Params &params);

    static void TriggerHaoQingZhi(const Params &params);

    static void TriggerHaoQingBianZhi(const Params &params);

    static void TriggerLiuZhaoDamage(const Params &params);

    static void TriggerLiuZhaoSurplusDamage(const Params &params);

    static void TriggerEnchantShoes(const Params &params);

    static void TriggerEnchantBelt(const Params &params);

    static void TriggerEnchantWrist(const Params &params);

    static void TriggerSetAttribute(const Params &params);

    static void TriggerWeaponCW(const Params &params);

    static void TriggerWeaponCWDot(const Params &params);

    static void TriggerWeaponCWDamage(const Params &params);

    static void TriggerTeamCoreYouRen(const Params &params);
};

} // namespace MoWen

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_MO_WEN_H__