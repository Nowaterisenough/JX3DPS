#include "ChiYing.h"

#include "Common/Utils.h"
#include "Core/Target.h"
#include "Core/Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t ChiYing::s_lastFrames = 15 * 16;
int ChiYing::s_maxStackNum = 3;

ChiYing::ChiYing(Player &player) :
    Buff(player)
{
    InitBaseParams();
    InitDamageParams();
}

ChiYing::ChiYing(const ChiYing &buff) : Buff(buff)
{

}

ChiYing::~ChiYing()
{

}

ChiYing *ChiYing::Clone()
{
    return new ChiYing(*this);
}

ChiYing &ChiYing::operator=(const ChiYing &buff)
{
    Buff::operator=(buff);
    return *this;
}

void ChiYing::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_stackCount = 0;
    m_effectCount = 0;
}

void ChiYing::Refresh()
{
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->Attr().GetHastePercent());
    m_effectCount = 1;
    m_stackCount++;
    m_stackCount = GET_MIN_INT(s_maxStackNum, m_stackCount);
}

void ChiYing::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    if (param != FORCE) {
        for (auto it : targetsMap[NORMAL]) {
            if (m_stackCount > 0) {
                m_stackCount--;
                TableRes tableRes = GetPhysicsRollResult(m_player->Attr());
                UpdatePhysicsStats(m_player->Attr(), *it, tableRes, m_subNames[0], 0, stats);
            }
        }
        m_lastFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_stackCount, INVALID_FRAMES_SET, m_lastFrames);
        m_effectCount = IF_1ST_0_TO_0_ELSE_2ND(m_stackCount, m_effectCount);
    } else {
        m_lastFrames = INVALID_FRAMES_SET;
        m_effectCount = 0;
        m_stackCount = 0;
    }
}

void ChiYing::Clean(Target &target, Stats &stats, Settings &settings, int param)
{
    TableRes tableRes = GetPhysicsRollResult(m_player->Attr());
    UpdatePhysicsStats(m_player->Attr(), target, tableRes, m_subNames[0], 0, stats);
    m_stackCount--;
    m_lastFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_stackCount, INVALID_FRAMES_SET, m_lastFrames);
    m_effectCount = IF_1ST_0_TO_0_ELSE_2ND(m_stackCount, m_effectCount);
}

void ChiYing::InitBaseParams()
{
    m_id = BUF_CHI_YING;
    m_name = "持盈";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_isDamage = true;
}

void ChiYing::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0].fixedDamage = (35 + 55) / 2;
    m_damageParams[m_subNames[0]][0].weaponDamageBinPercent = 0;
    m_damageParams[m_subNames[0]][0].attackDamagePercent = 0.795;
}

}

}
