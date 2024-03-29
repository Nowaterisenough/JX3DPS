﻿/**
 * Project: JX3DPS
 * File: TaiXuJianYi.h
 * Description:
 * Created Date: 2023-07-20 02:40:46
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-12 09:43:34
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#ifndef __JX3DPS_CLASS_TAI_XU_JIAN_YI_H__
#define __JX3DPS_CLASS_TAI_XU_JIAN_YI_H__

#include "Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

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
        if (cooldownSanChaiJianFaCurrent > 0) {
            frame = std::min(frame, cooldownSanChaiJianFaCurrent);
        }
        return frame;
    }

    inline void UpdateGlobalCooldown(Frame_t next) override
    {
        globalCooldownCurrent -= next;
        globalCooldownCurrent  = std::max(globalCooldownCurrent, 0);

        cooldownSanChaiJianFaCurrent -= next;
        cooldownSanChaiJianFaCurrent  = std::max(cooldownSanChaiJianFaCurrent, 0);
    }

    std::list<Id_t> fields;
    
    Id_t fieldId = TARGET_PLACE_HOLDERS_END;

    void RemoveField(Id_t fieldId, int stackNum = 1)
    {
        int stack = 0;
        for (auto it = fields.begin(); it != fields.end();) {
            if (*it == fieldId) {
                it = fields.erase(it);
                ++stack;
                if (stack == stackNum) {
                    break;
                }
            } else {
                ++it;
            }
        }
    }

    Frame_t cooldownSanChaiJianFaCurrent = 0;

    static void TriggerWuYi(const Params &params);

    static void TriggerFengShiAdd(const Params &params);

    static void TriggerFengShiClear(const Params &params);

    static void TriggerShenMai(const Params &params);

    static void TriggerXuJi(const Params &params);

    static void TriggerXuanMen(const Params &params);

    static void TriggerChangSheng(const Params &params);

    static void TriggerChiYing(const Params &params);

    static void TriggerWuYu(const Params &params);

    static void TriggerDieRen(const Params &params);

    static void TriggerQieYu(const Params &params);

    static void TriggerHuanYue(const Params &params);

    static void TriggerJingHuaYing(const Params &params);

    static void TriggerGuChang(const Params &params);

    static void TriggerLieYun(const Params &params);

    static void TriggerQiSheng(const Params &params);

    static void TriggerJianRuAdd(const Params &params);

    static void TriggerJianRu(const Params &params);

    static void TriggerFieldQiSheng(const Params &params);

    static void TriggerRenJianHeYiDot(const Params &params);

    static void TriggerShengTaiJiEffectCooldown(const Params &params);

    static void TriggerYunZhongJianSuiXingChen(const Params &params);

    static void TriggerYunZhongJianTunRiYue(const Params &params);

    static void TriggerYunZhongJianShengTaiJi(const Params &params);

    static void TriggerEnchantShoes(const Params &params);

    static void TriggerEnchantBelt(const Params &params);

    static void TriggerEnchantWrist(const Params &params);

    static void TriggerWeaponCW(const Params &params);

    static void TriggerWeaponCWDot(const Params &params);

    static void TriggerWeaponCWDamage(const Params &params);

    static void TriggerSetAttribute(const Params &params);

    static void TriggerTeamCoreTaiXuJianYiYouRen(const Params &params);
};

} // namespace TaiXuJianYi

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_TAI_XU_JIAN_YI_H__