/**
 * Project: JX3DPS
 * File: TaiXuJianYi.h
 * Description:
 * Created Date: 2023-07-20 02:40:46
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-19 19:15:35
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

    std::list<Id_t> fields;

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

    static void TriggerFieldQiSheng(const Params &params);

    static void TriggerRenJianHeYiDot(const Params &params);

    static void TriggerShengTaiJiEffectCooldown(const Params &params);

    static void TriggerYunZhongJianSuiXingChen(const Params &params);

    static void TriggerYunZhongJianTunRiYue(const Params &params);

    static void TriggerYunZhongJianShengTaiJi(const Params &params);

    static void TriggerEnchantShoes(const Params &params);

    static void TriggerEnchantBelt(const Params &params);

    static void TriggerEnchantWrist(const Params &params);

    static void TriggerWeaponEffectCW(const Params &params);

    static void TriggerWeaponEffectCWDot(const Params &params);

    static void TriggerWeaponEffectCWDamage(const Params &params);

    static void TriggerSetAttribute(const Params &params);

    static void TriggerTeamCoreTaiXuJianYiYouRen(const Params &params);
};

} // namespace TaiXuJianYi

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_TAI_XU_JIAN_YI_H__