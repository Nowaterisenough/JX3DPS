#include "RenJianHeYiTBuf.h"

#include "Core/Target.h"
#include "Core/Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t RenJianHeYiTBuf::s_lastFrames = 8 * 16;
Frame_t RenJianHeYiTBuf::s_intervalFrames = 1 * 16;
int RenJianHeYiTBuf::s_maxEffectNum = 6;

RenJianHeYiTBuf::RenJianHeYiTBuf(Player &player, Target &target) :
    Buff(player)
{
    InitBaseParams();
    InitDamageParams();
    m_target = &target;
}

RenJianHeYiTBuf::RenJianHeYiTBuf(const RenJianHeYiTBuf &buff) : Buff(buff)
{

}

RenJianHeYiTBuf::~RenJianHeYiTBuf()
{

}

RenJianHeYiTBuf *RenJianHeYiTBuf::Clone()
{
    return new RenJianHeYiTBuf(*this);
}

RenJianHeYiTBuf &RenJianHeYiTBuf::operator=(const RenJianHeYiTBuf &buff)
{
    Buff::operator=(buff);
    return *this;
}

void RenJianHeYiTBuf::Cast(TargetsMap &targetsMap, Stats &stats)
{
    TableRes tableRes = GetPhysicsRollResult(*m_snapAttr, *m_target, RollType::DOT);
    UpdatePhysicsStats(*m_snapAttr, *m_target, stats, settings, tableRes, m_subNames[0], 0);
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_snapAttr->GetHastePercent());
    m_effectCount--;
    m_lastFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_effectCount, INVALID_FRAMES_SET, m_lastFrames);
    m_intervalFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_effectCount, INVALID_FRAMES_SET, m_intervalFrames);
}

void RenJianHeYiTBuf::Refresh()
{
    *m_snapAttr = m_player->Attr();
    m_lastFrames = static_cast<int>(s_lastFrames * m_snapAttr->GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_snapAttr->GetHastePercent());
    m_effectCount = s_maxEffectNum;
}

void RenJianHeYiTBuf::Clean(TargetsMap &targetsMap, Stats &stats, int param)
{
    m_lastFrames = INVALID_FRAMES_SET;
    m_intervalFrames = INVALID_FRAMES_SET;
    m_effectCount = 0;
}

void RenJianHeYiTBuf::InitBaseParams()
{
    m_id = TBUF_REN_JIAN_HE_YI;
    m_name = "人剑合一Dot";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_isDamage = true;
}

void RenJianHeYiTBuf::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0].fixedDamage = 0;
    m_damageParams[m_subNames[0]][0].weaponDamageBinPercent = 0;
    m_damageParams[m_subNames[0]][0].attackDamagePercent = 0.02725;
}

}

}
