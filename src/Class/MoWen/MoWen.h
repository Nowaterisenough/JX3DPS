/**
 * Project: JX3DPS
 * File: MoWen.h
 * Description:
 * Created Date: 2023-07-31 16:03:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-02 01:11:56
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

    static void TriggerWuYi(const Params &params);

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

    static void TriggerXianFeng(const Params &params);

    static void TriggerXianFengBiaoJi(const Params &params);
};

} // namespace MoWen

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_MO_WEN_H__