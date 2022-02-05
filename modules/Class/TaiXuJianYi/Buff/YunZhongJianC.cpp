#include "YunZhongJianC.h"

#include "Core/Target.h"
#include "Core/Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t YunZhongJianC::s_lastFrames = 8 * 16;
Frame_t YunZhongJianC::s_intervalFrames = 16;
int YunZhongJianC::s_maxEffectNum = 8;

YunZhongJianC::YunZhongJianC(Player &player) :
    Buff(player)
{
    InitBaseParams();
    InitDamageParams();
}

YunZhongJianC::YunZhongJianC(const YunZhongJianC &buff) : Buff(buff)
{

}

YunZhongJianC::~YunZhongJianC()
{

}

YunZhongJianC *YunZhongJianC::Clone()
{
    return new YunZhongJianC(*this);
}

YunZhongJianC &YunZhongJianC::operator=(const YunZhongJianC &buff)
{
    Buff::operator=(buff);
    return *this;
}

void YunZhongJianC::Cast(TargetsMap &targetsMap, Stats &stats)
{
    for (auto it : targetsMap[NORMAL]) {
        TableRes tableRes = GetPhysicsRollResult(m_player->Attr(), *it);
        UpdatePhysicsStats(m_player->Attr(), *it, stats, settings, tableRes, m_subNames[0], 0);
        SubEffect(targetsMap, stats, settings);
    }
    m_intervalFrames = s_intervalFrames;
    m_effectCount--;
    m_lastFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_effectCount, INVALID_FRAMES_SET, m_lastFrames);
    m_intervalFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_effectCount, INVALID_FRAMES_SET, m_intervalFrames);
}

void YunZhongJianC::Refresh()
{
    m_lastFrames = static_cast<int>(s_lastFrames * m_player->Attr().GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->Attr().GetHastePercent());
    m_effectCount = s_maxEffectNum;
}

void YunZhongJianC::Clean(TargetsMap &targetsMap, Stats &stats, int param)
{
    m_effectCount = 0;
    m_lastFrames = INVALID_FRAMES_SET;
    m_intervalFrames = INVALID_FRAMES_SET;
}

void YunZhongJianC::InitBaseParams()
{
    m_id = BUF_YUN_ZHONG_JIAN_C;
    m_name = "云中剑_碎星辰";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_isDamage = true;
}

void YunZhongJianC::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0].fixedDamage = (35 + 55) / 2;
    m_damageParams[m_subNames[0]][0].weaponDamageBinPercent = 0;
    m_damageParams[m_subNames[0]][0].attackDamagePercent = 0.48125;
}

void YunZhongJianC::SubEffect(TargetsMap &targetsMap, Stats &stats)
{
    if (m_player->GetGearSetAttack() && Random(0.0, 1.0) < 0.1) {
        m_player->buffs[BUF_CLASS_ATTACK]->Refresh();
    }
}

}

}
