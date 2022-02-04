#include "TaiXuJianYi.h"

#include "Class/TaiXuJianYi/Skill/BaHuangGuiYuan.h"
//#include "Class/TaiXuJianYi/Skill/RenJianHeYi.h"
//#include "Class/TaiXuJianYi/Skill/WuWoWuJian.h"
//#include "Class/TaiXuJianYi/Skill/SanChaiJianFa.h"
//#include "Class/TaiXuJianYi/Skill/SanHuanTaoYue.h"
//#include "Class/TaiXuJianYi/Skill/SuiXingChen.h"
//#include "Class/TaiXuJianYi/Skill/ShengTaiJi.h"
//#include "Class/TaiXuJianYi/Skill/TunRiYue.h"
//#include "Class/TaiXuJianYi/Skill/ZiQiDongLai.h"
//#include "Class/TaiXuJianYi/Buff/ChiYing.h"
//#include "Class/TaiXuJianYi/Buff/DieRen.h"
//#include "Class/TaiXuJianYi/Buff/FengShi.h"
//#include "Class/TaiXuJianYi/Buff/JianMing.h"
//#include "Class/TaiXuJianYi/Buff/QiSheng.h"
//#include "Class/TaiXuJianYi/Buff/RenJianHeYiTBuf.h"
//#include "Class/TaiXuJianYi/Buff/RongJin.h"
//#include "Class/TaiXuJianYi/Buff/ShengTaiJiStd.h"
//#include "Class/TaiXuJianYi/Buff/SuiXingChenBuf.h"
//#include "Class/TaiXuJianYi/Buff/SuiXingChenStd.h"
//#include "Class/TaiXuJianYi/Buff/SuiXingChenStd2.h"
//#include "Class/TaiXuJianYi/Buff/TunRiYueStd.h"
//#include "Class/TaiXuJianYi/Buff/XuanMen.h"
//#include "Class/TaiXuJianYi/Buff/YunZhongJianC.h"
//#include "Class/TaiXuJianYi/Buff/YunZhongJianJ.h"
//#include "Class/TaiXuJianYi/Buff/YunZhongJianY.h"
//#include "Class/TaiXuJianYi/Buff/ZiQiDongLaiBuf.h"
//#include "Class/TaiXuJianYi/Buff/TaiXuJianYiTeamPoint.h"
//#include "Class/TaiXuJianYi/Buff/TaiXuJianYiCW.h"
#include "kkk.h"

namespace JX3DPS {

namespace TaiXuJianYi {

int Player::s_publicCooldown = 24;

TaiXuJianYi::TaiXuJianYi()
{
    InitClass();
}

TaiXuJianYi::~TaiXuJianYi()
{

}

int TaiXuJianYi::UpdateQidian(int qidian)
{
    m_qidianBefore = m_qidian;
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
    m_talentCuoRui = false;
    m_talentXinGu = false;
    m_talentTongGen = false;
    m_talentShenMai = false;
    m_talentKunWu = false;
    m_talentBaiHong = false;
    m_talentHuaSanQing = false;
    m_talentYunZhongJian = false;
    m_talentWuYi = false;
    m_talentFengShi = false;
    m_talentKuangGe = false;
    m_talentDieRen = false;
    m_talentQieYu = false;
    m_talentChangSheng = false;
    m_talentFuYin = false;
    m_talentHeGuang = false;
    m_talentSuiWu = false;
    m_talentQiSheng = false;
    m_talentWuYu = false;
    m_talentXuanMen = false;


    m_skillMap[SKI_WU_WO_WU_JIAN] = new WuWoWuJian;
    m_skillMap[SKI_BA_HUANG_GUI_YUAN] = new BaHuangGuiYuan;
    m_skillMap[SKI_SAN_HUAN_TAO_YUE] = new SanHuanTaoYue;
    m_skillMap[SKI_REN_JIAN_HE_YI] = new RenJianHeYi;
    m_skillMap[SKI_SUI_XING_CHEN] = new SuiXingChen;
    m_skillMap[SKI_SHENG_TAI_JI] = new ShengTaiJi;
    m_skillMap[SKI_TUN_RI_YUE] = new TunRiYue;
    m_skillMap[SKI_ZI_QI_DONG_LAI] = new ZiQiDongLai;
    m_skillMap[SKI_SAN_CHAI_JIAN_FA] = new SanChaiJianFa;

    m_buffMap[BUF_CLASS_EFFECT] = new RongJin;
    m_buffMap[BUF_CLASS_TEAM_POINT] = new TaiXuJianYiTeamPoint;
    m_buffMap[BUF_CLASS_CW] = new TaiXuJianYiCW;
    m_buffMap[BUF_CLASS_ATTACK] = new JianMing;

    m_buffMap[BUF_ZI_QI_DONG_LAI] = new ZiQiDongLaiBuf;
    m_buffMap[BUF_XUAN_MEN] = new XuanMen;
    m_buffMap[BUF_SUI_XING_CHEN] = new SuiXingChenBuf;
    m_buffMap[BUF_SUI_XING_CHEN_STD] = new SuiXingChenStd;
    m_buffMap[BUF_SUI_XING_CHEN_STD2] = new SuiXingChenStd2;
    m_buffMap[BUF_SHENG_TAI_JI_STD] = new ShengTaiJiStd;
    m_buffMap[BUF_TUN_RI_YUE_STD] = new TunRiYueStd;
    m_buffMap[BUF_YUN_ZHONG_JIAN_C] = new YunZhongJianC;
    m_buffMap[BUF_YUN_ZHONG_JIAN_J] = new YunZhongJianJ;
    m_buffMap[BUF_YUN_ZHONG_JIAN_Y] = new YunZhongJianY;
    m_buffMap[BUF_CHI_YING] = new ChiYing;
    m_buffMap[BUF_QI_SHENG] = new QiSheng;
    m_buffMap[BUF_FENG_SHI] = new FengShi;
}

void TaiXuJianYi::UpdateAgility()
{
    AddPhysicsCriticalStrike(static_cast<int>(-m_agility * (0.64 + 0.58)));
    AddPhysicsAttack(static_cast<int>(-m_agility * 1.45));
    m_agility = m_agilityBase * (1024 + m_agilityBaseBinPercent) >> 10;
    AddPhysicsCriticalStrike(static_cast<int>(m_agility * (0.64 + 0.58)));
    AddPhysicsAttack(static_cast<int>(m_agility * 1.45));
}

SkillMap InitSkillTaiXuJianYi()
{
    SkillMap skillMap;
    //skillMap[SKI_WU_WO_WU_JIAN] = new WuWoWuJian(this);
    skillMap[SKI_BA_HUANG_GUI_YUAN] = new BaHuangGuiYuan(this);
    skillMap[SKI_SAN_HUAN_TAO_YUE] = new SanHuanTaoYue;
    skillMap[SKI_REN_JIAN_HE_YI] = new RenJianHeYi;
    skillMap[SKI_SUI_XING_CHEN] = new SuiXingChen;
    skillMap[SKI_SHENG_TAI_JI] = new ShengTaiJi;
    skillMap[SKI_TUN_RI_YUE] = new TunRiYue;
    skillMap[SKI_ZI_QI_DONG_LAI] = new ZiQiDongLai;
    skillMap[SKI_SAN_CHAI_JIAN_FA] = new SanChaiJianFa;
    return skillMap;
}

}

}
