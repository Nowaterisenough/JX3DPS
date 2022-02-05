/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-04 19:47:00
 * @Update      : NoWats
 * @LastTime    : 2022-02-05 15:48:43
 * @FilePath    : \JX3DPS\modules\Class\TaiXuJianYi\Skill\ZiQiDongLai.cpp
 */
#include "ZiQiDongLai.h"

#include "Core/Buff.h"
#include "Core/Target.h"
#include "Core/Player.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

namespace JX3DPS {

namespace TaiXuJianYi {

int ZiQiDongLai::s_cooldown = 100 * 16;

ZiQiDongLai::ZiQiDongLai(Player &player) : Skill(player)
{
    InitBaseParams();
}

ZiQiDongLai::ZiQiDongLai(const ZiQiDongLai &skill) : Skill(skill) {}

ZiQiDongLai::~ZiQiDongLai() {}

ZiQiDongLai *ZiQiDongLai::Clone()
{
    return new ZiQiDongLai(*this);
}

ZiQiDongLai &ZiQiDongLai::operator=(const ZiQiDongLai &skill)
{
    Skill::operator=(skill);
    return *this;
}

void ZiQiDongLai::Cast(TargetsMap &targetsMap, Stats &stats, Settings &settings, CastType castType)
{
    UpdatePhysicsStats(*targetsMap[NORMAL].front(), stats, settings, TableRes::ALL, m_subNames[0], 0);
    SubEffect(targetsMap, stats, settings, TableRes::ALL);
    m_cooldown = s_cooldown;
}

void ZiQiDongLai::InitBaseParams()
{
    m_id   = SKI_ZI_QI_DONG_LAI;
    m_name = "紫气东来";
    m_subNames.push_back("");
    m_levelNames.push_back("");
    m_publicCooldown = &m_player->m_publicCooldown;
}

void ZiQiDongLai::InitDamageParams()
{
    m_damageParams[m_subNames[0]][0] = DamageParam(0, 0, 0.0);
}

void ZiQiDongLai::SubEffect(TargetsMap &targetsMap, Stats &stats, Settings &settings, TableRes tableRes)
{
    static_cast<TaiXuJianYi *>(m_player)->UpdateQidian(10);
    m_player->buffs[BUF_ZI_QI_DONG_LAI]->Refresh();
}

} // namespace TaiXuJianYi

} // namespace JX3DPS
