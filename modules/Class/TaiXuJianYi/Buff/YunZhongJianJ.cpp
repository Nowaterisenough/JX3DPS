#include "YunZhongJianJ.h"

#include "Core/Target.h"
#include "Core/Player.h"

namespace JX3DPS {

namespace TaiXuJianYi {

Frame_t YunZhongJianJ::s_lastFrames = 8 * 16;
Frame_t YunZhongJianJ::s_intervalFrames = 16;
int YunZhongJianJ::s_maxEffectNum = 8;

YunZhongJianJ::YunZhongJianJ(Player &player) :
    Buff(player)
{
    InitBaseParams();
    InitDamageParams();
}

YunZhongJianJ::YunZhongJianJ(const YunZhongJianJ &buff) : Buff(buff)
{

}

YunZhongJianJ::~YunZhongJianJ()
{

}

YunZhongJianJ *YunZhongJianJ::Clone()
{
    return new YunZhongJianJ(*this);
}

YunZhongJianJ &YunZhongJianJ::operator=(const YunZhongJianJ &buff)
{
    Buff::operator=(buff);
    return *this;
}

void YunZhongJianJ::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    for (auto it : targetsMap[NORMAL]) {
        TableRes tableRes = GetPhysicsRollResult(m_player->GetAttr(), *it);
        UpdatePhysicsStats(m_player->GetAttr(), *it, stats, settings, tableRes, m_subNames[0], 0);
        SubEffect(targetsMap, stats, settings);
    }
    m_intervalFrames = s_intervalFrames;
    m_effectNum--;
    m_lastFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_effectNum, INVALID_FRAMES_SET, m_lastFrames);
    m_intervalFrames = IF_1ST_0_TO_2ND_ELSE_3RD(m_effectNum, INVALID_FRAMES_SET, m_intervalFrames);
}

void YunZhongJianJ::Refresh()
{
    m_lastFrames = static_cast<int>(s_lastFrames * m_player->GetAttr().GetHastePercent());
    m_intervalFrames = static_cast<int>(s_intervalFrames * m_player->GetAttr().GetHastePercent());
    m_effectNum = s_maxEffectNum;
}

void YunZhongJianJ::Clean(TargetsMap &targetsMap, Stats &stats, Settings &settings, int param)
{
    m_effectNum = 0;
    m_lastFrames = INVALID_FRAMES_SET;
    m_intervalFrames = INVALID_FRAMES_SET;
}

void YunZhongJianJ::InitBaseParams()
{
    m_id = BUF_YUN_ZHONG_JIAN_J;
    m_name = "云中剑_生太极";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_isDamage = true;
}

void YunZhongJianJ::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0].fixedDamage = (35 + 55) / 2;
    m_damageParams[m_subNames[0]][0].weaponDamageBinPercent = 0;
    m_damageParams[m_subNames[0]][0].attackDamagePercent = 0.48125;
}

void YunZhongJianJ::SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings)
{
    if (m_player->GetGearSetAttack() && RandBetween(0.0, 1.0) < 0.1) {
        m_player->buffs[BUF_CLASS_ATTACK]->Refresh();
    }
}

}

}
