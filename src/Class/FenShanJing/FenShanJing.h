/**
 * Project: 
 * File: FenShanJing.h
 * Description: 
 * Created Date: 2023-08-25 20:44:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-30 11:58:35
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */


#ifndef __JX3DPS_CLASS_FEN_SHAN_JING_H__
#define __JX3DPS_CLASS_FEN_SHAN_JING_H__

#include "Player.h"

namespace JX3DPS {

namespace FenShanJing {

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

    Frame_t cooldownSanChaiJianFaCurrent = 0;

    static void TriggerWuYi(const Params &params);

};

} // namespace FenShanJing

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_FEN_SHAN_JING_H__