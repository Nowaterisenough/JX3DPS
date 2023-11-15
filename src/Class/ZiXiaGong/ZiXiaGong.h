#ifndef __JX3DPS_CLASS_ZI_XIA_GONG_H__
#define __JX3DPS_CLASS_ZI_XIA_GONG_H__

#include "Player.h"

namespace JX3DPS {

namespace ZiXiaGong {

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

    static void TriggerFieldQiSheng(const Params &params);

    static void TriggerEnchantShoes(const Params &params);

    static void TriggerEnchantBelt(const Params &params);

    static void TriggerEnchantWrist(const Params &params);

    static void TriggerWeaponCW(const Params &params);

    static void TriggerWeaponCWDot(const Params &params);

    static void TriggerWeaponCWDamage(const Params &params);

    static void TriggerSetAttribute(const Params &params);

};

} // namespace ZiXiaGong

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_ZI_XIA_GONG_H__