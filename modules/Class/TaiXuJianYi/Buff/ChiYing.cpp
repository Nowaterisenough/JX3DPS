#include "ChiYing.h"

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
    m_stackNum = 0;
    m_effectNum = 0;
}

void ChiYing::Refresh()
{
    m_lastFrames = static_cast<int>(m_lastFrames * m_player->GetAttr().GetHastePercent());
    m_effectNum = 1;
    m_stackNum++;
    m_stackNum = GET_MIN_INT(s_maxStackNum, m_stackNum);
}

void ChiYing::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    if (param != FORCE) {
        for (auto it : targetsMap[NORMAL]) {
            if (m_stackNum > 0) {
                m_stackNum--;
                TableRes tableRes = GetPhysicsRollResult(m_player->GetAttr(), *it);
                UpdatePhysicsStats(m_player->GetAttr(), *it, stats, settings, tableRes, m_subNames[0], 0);
            }
        }
        m_lastFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_stackNum, INVALID_FRAMES_SET, m_lastFrames);
        m_effectNum = IF_1ST_0_TO_0_ELSE_2ND(m_stackNum, m_effectNum);
    } else {
        m_lastFrames = INVALID_FRAMES_SET;
        m_effectNum = 0;
        m_stackNum = 0;
    }
}

void ChiYing::Clean(Target &target, Stats &stats, Settings &settings, int param)
{
    TableRes tableRes = GetPhysicsRollResult(m_player->GetAttr(), target);
    UpdatePhysicsStats(m_player->GetAttr(), target, stats, settings, tableRes, m_subNames[0], 0);
    m_stackNum--;
    m_lastFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_stackNum, INVALID_FRAMES_SET, m_lastFrames);
    m_effectNum = IF_1ST_0_TO_0_ELSE_2ND(m_stackNum, m_effectNum);
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
