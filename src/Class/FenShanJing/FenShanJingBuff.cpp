/**
 * Project: JX3DPS
 * File: FenShanJingBuff.cpp
 * Description:
 * Created Date: 2023-08-25 20:44:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-26 09:48:22
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "FenShanJingBuff.h"

#include "FenShanJingSkill.h"
#include "Target.hpp"

namespace JX3DPS {

namespace FenShanJing {

namespace Buff {

DieRen::DieRen(JX3DPS::Player *player, Targets *targets) : JX3DPS::Buff(player, targets)
{
    m_id          = BUFF_DIE_REN;
    m_name        = "叠刃";
    m_interval    = 16 * 3;
    m_stackNum    = 5;
    m_effectCount = 8;

    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 1);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 2);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 3);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 4);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 5);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 6);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 7);

    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 1);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 2);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 3);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 4);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 5);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 6);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 7);

    if (m_player->talents[TALENT_LIE_YUN]) {
        m_stackNum = 7;
    }

    if (m_player->talents[TALENT_XU_JI]) {
        m_effectDamageAdditionalPercentInt += 102 * 2;
    }
}

} // namespace Buff

} // namespace FenShanJing

} // namespace JX3DPS
