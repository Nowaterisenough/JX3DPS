/**
 * Project: JX3DPS
 * File: TaiXuJianYi.h
 * Description:
 * Created Date: 2023-07-20 02:40:46
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-28 16:36:28
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

    void TriggerShenMai(const Params &params);
};

} // namespace TaiXuJianYi

} // namespace JX3DPS

#endif // __JX3DPS_CLASS_TAI_XU_JIAN_YI_H__