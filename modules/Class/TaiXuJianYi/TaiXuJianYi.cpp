#include "TaiXuJianYi.h"

#include "Core/Skill.h"
#include "Class/TaiXuJianYi/Skill/BaHuangGuiYuan.h"
#include "Class/TaiXuJianYi/Skill/RenJianHeYi.h"
#include "Class/TaiXuJianYi/Skill/WuWoWuJian.h"
#include "Class/TaiXuJianYi/Skill/SanChaiJianFa.h"
#include "Class/TaiXuJianYi/Skill/SanHuanTaoYue.h"
#include "Class/TaiXuJianYi/Skill/SuiXingChen.h"
#include "Class/TaiXuJianYi/Skill/ShengTaiJi.h"
#include "Class/TaiXuJianYi/Skill/TunRiYue.h"
#include "Class/TaiXuJianYi/Skill/ZiQiDongLai.h"
#include "Class/TaiXuJianYi/Buff/ChiYing.h"
#include "Class/TaiXuJianYi/Buff/DieRen.h"
#include "Class/TaiXuJianYi/Buff/FengShi.h"
#include "Class/TaiXuJianYi/Buff/JianMing.h"
#include "Class/TaiXuJianYi/Buff/QiSheng.h"
#include "Class/TaiXuJianYi/Buff/RenJianHeYiTBuf.h"
#include "Class/TaiXuJianYi/Buff/RongJin.h"
#include "Class/TaiXuJianYi/Buff/ShengTaiJiStd.h"
#include "Class/TaiXuJianYi/Buff/SuiXingChenBuf.h"
#include "Class/TaiXuJianYi/Buff/SuiXingChenStd.h"
#include "Class/TaiXuJianYi/Buff/SuiXingChenStd2.h"
#include "Class/TaiXuJianYi/Buff/TunRiYueStd.h"
#include "Class/TaiXuJianYi/Buff/XuanMen.h"
#include "Class/TaiXuJianYi/Buff/YunZhongJianC.h"
#include "Class/TaiXuJianYi/Buff/YunZhongJianJ.h"
#include "Class/TaiXuJianYi/Buff/YunZhongJianY.h"
#include "Class/TaiXuJianYi/Buff/ZiQiDongLaiBuf.h"
#include "Class/TaiXuJianYi/Buff/TaiXuJianYiTeamPoint.h"
#include "Class/TaiXuJianYi/Buff/TaiXuJianYiCW.h"

namespace JX3DPS {

namespace TaiXuJianYi {

//int Player::s_publicCooldown = 24;

TaiXuJianYi::TaiXuJianYi()
{
    InitClass();
    m_attr = new Attr(TAI_XU_JIAN_YI);
}

TaiXuJianYi::TaiXuJianYi(const TaiXuJianYi &player) : Player(player)
{
    m_qidian = player.m_qidian;
    m_attr = new Attr(*player.m_attr);
    for (auto it : player.skills) {
        skills[it.first] = it.second->Clone();
        skills[it.first]->SetPlayer(*this);
    }
    for (auto it : player.buffs) {
        buffs[it.first] = it.second->Clone();
        buffs[it.first]->SetPlayer(*this);
    }
    for (auto it : player.talents) {
        talents[it.first] = it.second;
    }
}

TaiXuJianYi::~TaiXuJianYi()
{
    for (auto it : buffs) {
        delete it.second;
    }
    for (auto it : skills) {
        delete it.second;
    }
}

TaiXuJianYi *TaiXuJianYi::Clone()
{
    return new TaiXuJianYi(*this);
}

TaiXuJianYi &TaiXuJianYi::operator=(const TaiXuJianYi &player)
{
    Player::operator=(player);
    m_qidian = player.m_qidian;
    m_attr = new Attr(*player.m_attr);
    for (auto it : player.skills) {
        skills[it.first] = it.second->Clone();
        skills[it.first]->SetPlayer(*this);
    }
    for (auto it : player.buffs) {
        buffs[it.first] = it.second->Clone();
        buffs[it.first]->SetPlayer(*this);
    }
    for (auto it : player.talents) {
        talents[it.first] = it.second;
    }
    return *this;
}

bool TaiXuJianYi::SetGearSetCW(bool gearSetCW)
{
    return false;
}

bool TaiXuJianYi::SetGearSetClass(bool gearSetClass)
{
    return false;
}

bool TaiXuJianYi::SetGearSetAttack(bool gearSetAttack)
{
    return false;
}

int TaiXuJianYi::UpdateQidian(int qidian)
{
    m_qidian += qidian;
    m_qidian = BETWEEN(0, m_qidian, 10);
    return m_qidian;
}

int TaiXuJianYi::GetQidian()
{
    return m_qidian;
}

void TaiXuJianYi::InitClass()
{
    talents[CUO_RUI] = false;
    talents[XIN_GU] = false;
    talents[TONG_GEN] = false;
    talents[SHEN_MAI] = false;
    talents[KUN_WU] = false;
    talents[BAI_HONG] = false;
    talents[HUA_SAN_QING] = false;
    talents[YUN_ZHONG_JIAN] = false;
    talents[WU_YI] = false;
    talents[FENG_SHI] = false;
    talents[KUANG_GE] = false;
    talents[DIE_REN] = false;
    talents[QIE_YU] = false;
    talents[CHANG_SHENG] = false;
    talents[FU_YIN] = false;
    talents[HE_GUANG] = false;
    talents[SUI_WU] = false;
    talents[QI_SHENG] = false;
    talents[WU_YU] = false;
    talents[XUAN_MEN] = false;


    skills[SKI_WU_WO_WU_JIAN] = new WuWoWuJian(*this);
    skills[SKI_BA_HUANG_GUI_YUAN] = new BaHuangGuiYuan(*this);
    skills[SKI_SAN_HUAN_TAO_YUE] = new SanHuanTaoYue(*this);
    skills[SKI_REN_JIAN_HE_YI] = new RenJianHeYi(*this);
    skills[SKI_SUI_XING_CHEN] = new SuiXingChen(*this);
    skills[SKI_SHENG_TAI_JI] = new ShengTaiJi  (*this);
    skills[SKI_TUN_RI_YUE] = new TunRiYue(*this);
    skills[SKI_ZI_QI_DONG_LAI] = new ZiQiDongLai(*this);
    skills[SKI_SAN_CHAI_JIAN_FA] = new SanChaiJianFa(*this);

    buffs[BUF_CLASS_EFFECT] = new RongJin(*this);
    buffs[BUF_CLASS_TEAM_POINT] = new TaiXuJianYiTeamPoint(*this);
    buffs[BUF_CLASS_CW] = new TaiXuJianYiCW(*this);
    buffs[BUF_CLASS_ATTACK] = new JianMing(*this);

    buffs[BUF_ZI_QI_DONG_LAI] = new ZiQiDongLaiBuf(*this);
    buffs[BUF_XUAN_MEN] = new XuanMen(*this);
    buffs[BUF_SUI_XING_CHEN] = new SuiXingChenBuf(*this);
    buffs[BUF_SUI_XING_CHEN_STD] = new SuiXingChenStd(*this);
    buffs[BUF_SUI_XING_CHEN_STD2] = new SuiXingChenStd2(*this);
    buffs[BUF_SHENG_TAI_JI_STD] = new ShengTaiJiStd(*this);
    buffs[BUF_TUN_RI_YUE_STD] = new TunRiYueStd(*this);
    buffs[BUF_YUN_ZHONG_JIAN_C] = new YunZhongJianC(*this);
    buffs[BUF_YUN_ZHONG_JIAN_J] = new YunZhongJianJ(*this);
    buffs[BUF_YUN_ZHONG_JIAN_Y] = new YunZhongJianY(*this);
    buffs[BUF_CHI_YING] = new ChiYing(*this);
    buffs[BUF_QI_SHENG] = new QiSheng(*this);
    buffs[BUF_FENG_SHI] = new FengShi(*this);
}

}

}
