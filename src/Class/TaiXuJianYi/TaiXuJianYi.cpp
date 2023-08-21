/**
 * Project: JX3DPS
 * File: TaiXuJianYi.h
 * Description:
 * Created Date: 2023-07-20 02:40:46
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-21 11:52:19
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "TaiXuJianYi.h"

#include "Buff3rd.h"
#include "TaiXuJianYiBuff.h"
#include "TaiXuJianYiSkill.h"
#include "Target.hpp"

namespace JX3DPS {

namespace TaiXuJianYi {

Player::Player() : JX3DPS::Player()
{
    attribute.SetClassType(ClassType::TAI_XU_JIAN_YI);
}

Player::Player(const Player &other) : JX3DPS::Player(other)
{
    this->buffs[BUFF_CLASS_FEATURE]->Add();
}

void Player::Init()
{
    skills.emplace(SKILL_PO_ZHAO, static_cast<JX3DPS::Skill *>(new Skill::PoZhao(this, nullptr)));
    skills.emplace(SKILL_WU_WO_WU_JIAN,
                   static_cast<JX3DPS::Skill *>(new Skill::WuWoWuJian(this, nullptr)));
    skills.emplace(SKILL_BA_HUANG_GUI_YUAN,
                   static_cast<JX3DPS::Skill *>(new Skill::BaHuangGuiYuan(this, nullptr)));
    skills.emplace(SKILL_SAN_HUAN_TAO_YUE,
                   static_cast<JX3DPS::Skill *>(new Skill::SanHuanTaoYue(this, nullptr)));
    skills.emplace(SKILL_WAN_JIAN_GUI_ZONG,
                   static_cast<JX3DPS::Skill *>(new Skill::WanJianGuiZong(this, nullptr)));
    skills.emplace(SKILL_REN_JIAN_HE_YI,
                   static_cast<JX3DPS::Skill *>(new Skill::RenJianHeYi(this, nullptr)));
    skills.emplace(SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN,
                   static_cast<JX3DPS::Skill *>(new Skill::RenJianHeYiSuiXingChen(this, nullptr)));
    skills.emplace(SKILL_REN_JIAN_HE_YI_TUN_RI_YUE,
                   static_cast<JX3DPS::Skill *>(new Skill::RenJianHeYiTunRiYue(this, nullptr)));
    skills.emplace(SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI,
                   static_cast<JX3DPS::Skill *>(new Skill::RenJianHeYiShengTaiJi(this, nullptr)));
    skills.emplace(SKILL_SAN_CHAI_JIAN_FA,
                   static_cast<JX3DPS::Skill *>(new Skill::SanChaiJianFa(this, nullptr)));
    skills.emplace(SKILL_SUI_XING_CHEN,
                   static_cast<JX3DPS::Skill *>(new Skill::SuiXingChen(this, nullptr)));
    skills.emplace(SKILL_SHENG_TAI_JI,
                   static_cast<JX3DPS::Skill *>(new Skill::ShengTaiJi(this, nullptr)));
    skills.emplace(SKILL_TUN_RI_YUE, static_cast<JX3DPS::Skill *>(new Skill::TunRiYue(this, nullptr)));
    skills.emplace(SKILL_ZI_QI_DONG_LAI,
                   static_cast<JX3DPS::Skill *>(new Skill::ZiQiDongLai(this, nullptr)));
    skills.emplace(SKILL_JING_HUA_YING,
                   static_cast<JX3DPS::Skill *>(new Skill::JingHuaYing(this, nullptr)));

    buffs.emplace(BUFF_DIE_REN,
                  static_cast<JX3DPS::Buff *>(new TaiXuJianYi::Buff::DieRen(this, nullptr)));
    buffs.emplace(BUFF_ZI_QI_DONG_LAI,
                  static_cast<JX3DPS::Buff *>(new Buff::ZiQiDongLai(this, nullptr)));
    buffs.emplace(BUFF_FIELD_SUI_XING_CHEN,
                  static_cast<JX3DPS::Buff *>(new Buff::FieldSuiXingChen(this, nullptr)));
    buffs.emplace(BUFF_FIELD_SHENG_TAI_JI,
                  static_cast<JX3DPS::Buff *>(new Buff::FieldShengTaiJi(this, nullptr)));
    buffs.emplace(BUFF_FIELD_TUN_RI_YUE,
                  static_cast<JX3DPS::Buff *>(new Buff::FieldTunRiYue(this, nullptr)));
    buffs.emplace(BUFF_SUI_XING_CHEN, static_cast<JX3DPS::Buff *>(new Buff::SuiXingChen(this, nullptr)));
    buffs.emplace(BUFF_TUN_RI_YUE, static_cast<JX3DPS::Buff *>(new Buff::TunRiYue(this, nullptr)));
    buffs.emplace(BUFF_CLASS_FEATURE,
                  static_cast<JX3DPS::Buff *>(new Buff::ClassFeature(this, nullptr)));

    if (talents[TALENT_SHEN_MAI]) {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                          std::bind(&TriggerShenMai, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                      std::bind(&TriggerShenMai, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                         std::bind(&TriggerShenMai, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                          std::bind(&TriggerShenMai, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                       std::bind(&TriggerShenMai, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                         std::bind(&TriggerShenMai, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                          std::bind(&TriggerShenMai, std::placeholders::_1));
        }
    } else {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_SHEN_MAI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        }
    }

    if (talents[TALENT_HUAN_YUE]) {
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_HUAN_YUE,
                                                         std::bind(&TriggerHuanYue, std::placeholders::_1));
    } else {
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_HUAN_YUE,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_BAI_HONG]) {
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_BAI_HONG,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
    } else {
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_BAI_HONG,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_WU_YI]) {
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_WU_YI,
                                                      std::bind(&TriggerWuYi, std::placeholders::_1));
    } else {
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_WU_YI,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_YUN_ZHONG_JIAN]) {
        buffs.emplace(BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI,
                      static_cast<JX3DPS::Buff *>(new Buff::YunZhongJianShengTaiJi(this, nullptr)));

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SHENG_TAI_JI,
            std::bind(&TriggerYunZhongJianShengTaiJi, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SHENG_TAI_JI,
            std::bind(&TriggerYunZhongJianShengTaiJi, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SHENG_TAI_JI,
            std::bind(&TriggerYunZhongJianShengTaiJi, std::placeholders::_1));
    } else {
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_YUN_ZHONG_JIAN_SHENG_TAI_JI,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SHENG_TAI_JI,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SHENG_TAI_JI,
            std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_JING_HUA_YING]) {
        skills.emplace(SKILL_JING_HUA_YING,
                       static_cast<JX3DPS::Skill *>(new Skill::JingHuaYing(this, nullptr)));
        buffs.emplace(BUFF_JING_HUA_YING,
                      static_cast<JX3DPS::Buff *>(new Buff::JingHuaYing(this, nullptr)));

        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(
            TRIGGER_JING_HUA_YING,
            std::bind(&TriggerJingHuaYing, std::placeholders::_1));
    } else {
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_JING_HUA_YING,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_FENG_SHI]) {
        buffs.emplace(BUFF_FENG_SHI, static_cast<JX3DPS::Buff *>(new Buff::FengShi(this, nullptr)));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(
            TRIGGER_FENG_SHI_ADD,
            std::bind(&TriggerFengShiAdd, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_FENG_SHI_ADD,
                                                         std::bind(&TriggerFengShiAdd, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_FENG_SHI_CLEAR,
                                                      std::bind(&TriggerFengShiClear, std::placeholders::_1));
    } else {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_FENG_SHI_ADD,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_FENG_SHI_ADD,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_FENG_SHI_CLEAR,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_DIE_REN]) {
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_DIE_REN,
                                                      std::bind(&TriggerDieRen, std::placeholders::_1));
    } else {
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_DIE_REN,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_QIE_YU]) {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_QIE_YU,
                                                          std::bind(&TriggerQieYu, std::placeholders::_1));
    } else {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_QIE_YU,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_CHANG_SHENG]) {
        buffs.emplace(BUFF_CHI_YING, static_cast<JX3DPS::Buff *>(new Buff::ChiYing(this, nullptr)));

        skills[SKILL_SUI_XING_CHEN]->AddTriggerEffect(TRIGGER_CHANG_SHENG,
                                                      std::bind(&TriggerChangSheng, std::placeholders::_1));
        skills[SKILL_SHENG_TAI_JI]->AddTriggerEffect(TRIGGER_CHANG_SHENG,
                                                     std::bind(&TriggerChangSheng, std::placeholders::_1));
        skills[SKILL_TUN_RI_YUE]->AddTriggerEffect(TRIGGER_CHANG_SHENG,
                                                   std::bind(&TriggerChangSheng, std::placeholders::_1));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                          std::bind(&TriggerChiYing, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                      std::bind(&TriggerChiYing, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                         std::bind(&TriggerChiYing, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                          std::bind(&TriggerChiYing, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                       std::bind(&TriggerChiYing, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                          std::bind(&TriggerChiYing, std::placeholders::_1));
        }
    } else {
        skills[SKILL_SUI_XING_CHEN]->AddTriggerEffect(TRIGGER_CHANG_SHENG,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHENG_TAI_JI]->AddTriggerEffect(TRIGGER_CHANG_SHENG,
                                                     std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_TUN_RI_YUE]->AddTriggerEffect(TRIGGER_CHANG_SHENG,
                                                   std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_CHI_YING,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        }
    }

    if (talents[TALENT_LIE_YUN]) {
        buffs.emplace(BUFF_HIDDEN_LIE_YUN,
                      static_cast<JX3DPS::Buff *>(new Buff::FieldLieYun(this, nullptr)));
        buffs.emplace(BUFF_LIE_YUN, static_cast<JX3DPS::Buff *>(new Buff::LieYun(this, nullptr)));

        buffs[BUFF_DIE_REN]->AddTriggerEffect(TRIGGER_LIE_YUN,
                                              std::bind(&TriggerLieYun, std::placeholders::_1));
    } else {
        buffs[BUFF_DIE_REN]->AddTriggerEffect(TRIGGER_LIE_YUN,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_GU_CHANG]) {
        buffs[BUFF_SUI_XING_CHEN]->AddTriggerEffect(TRIGGER_GU_CHANG,
                                                    std::bind(&TriggerGuChang, std::placeholders::_1));
    } else {
        buffs[BUFF_SUI_XING_CHEN]->AddTriggerEffect(TRIGGER_GU_CHANG,
                                                    std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_WU_YU]) {
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_WU_YU,
                                                      std::bind(&TriggerWuYu, std::placeholders::_1));
    } else {
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_WU_YU,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_QI_SHENG]) {
        buffs.emplace(BUFF_QI_SHENG, static_cast<JX3DPS::Buff *>(new Buff::QiSheng(this, nullptr)));
        buffs.emplace(BUFF_FIELD_SUI_XING_CHEN_QI_SHENG,
                      static_cast<JX3DPS::Buff *>(new Buff::FieldSuiXingChenQiSheng(this, nullptr)));

        buffs[BUFF_FIELD_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_FIELD_QI_SHENG,
            std::bind(&TriggerFieldQiSheng, std::placeholders::_1));
        buffs[BUFF_FIELD_SHENG_TAI_JI]->AddTriggerEffect(TRIGGER_QI_SHENG,
                                                         std::bind(&TriggerQiSheng, std::placeholders::_1));
    } else {
        buffs[BUFF_FIELD_SUI_XING_CHEN]->AddTriggerEffect(TRIGGER_FIELD_QI_SHENG,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        buffs[BUFF_FIELD_SHENG_TAI_JI]->AddTriggerEffect(TRIGGER_QI_SHENG,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_XU_JI]) {
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_XU_JI,
                                                          std::bind(&TriggerXuJi, std::placeholders::_1));
    } else {
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_XU_JI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_XUAN_MEN]) {
        buffs.emplace(BUFF_XUAN_MEN, static_cast<JX3DPS::Buff *>(new Buff::XuanMen(this, nullptr)));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_XUAN_MEN,
                                                       std::bind(&TriggerXuanMen, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_XUAN_MEN,
            std::bind(&TriggerXuanMen, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_XUAN_MEN,
            std::bind(&TriggerXuanMen, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_XUAN_MEN,
            std::bind(&TriggerXuanMen, std::placeholders::_1));
    } else {
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_XUAN_MEN,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]
            ->AddTriggerEffect(TRIGGER_XUAN_MEN, std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]
            ->AddTriggerEffect(TRIGGER_XUAN_MEN, std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]
            ->AddTriggerEffect(TRIGGER_XUAN_MEN, std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (recipes[RECIPE_REN_JIAN_HE_YI_EFFECT_YUN_ZHONG_JIAN]) {
        buffs.emplace(BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN,
                      static_cast<JX3DPS::Buff *>(new Buff::YunZhongJianSuiXingChen(this, nullptr)));
        buffs.emplace(BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE,
                      static_cast<JX3DPS::Buff *>(new Buff::YunZhongJianTunRiYue(this, nullptr)));

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SUI_XING_CHEN,
            std::bind(&TriggerYunZhongJianSuiXingChen, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SUI_XING_CHEN,
            std::bind(&TriggerYunZhongJianSuiXingChen, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SUI_XING_CHEN,
            std::bind(&TriggerYunZhongJianSuiXingChen, std::placeholders::_1));

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_TUN_RI_YUE,
            std::bind(&TriggerYunZhongJianTunRiYue, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_TUN_RI_YUE,
            std::bind(&TriggerYunZhongJianTunRiYue, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_TUN_RI_YUE,
            std::bind(&TriggerYunZhongJianTunRiYue, std::placeholders::_1));
    } else {
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_YUN_ZHONG_JIAN_SUI_XING_CHEN,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SUI_XING_CHEN,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_SUI_XING_CHEN,
            std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_YUN_ZHONG_JIAN_TUN_RI_YUE,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_TUN_RI_YUE,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_YUN_ZHONG_JIAN_TUN_RI_YUE,
            std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (recipes[RECIPE_REN_JIAN_HE_YI_EFFECT_DOT]) {
        buffs.emplace(BUFF_REN_JIAN_HE_YI,
                      static_cast<JX3DPS::Buff *>(new Buff::RenJianHeYi(this, nullptr)));

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(
            TRIGGER_REN_JIAN_HE_YI_DOT,
            std::bind(&TriggerRenJianHeYiDot, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_REN_JIAN_HE_YI_DOT,
            std::bind(&TriggerRenJianHeYiDot, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_REN_JIAN_HE_YI_DOT,
            std::bind(&TriggerRenJianHeYiDot, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_REN_JIAN_HE_YI_DOT,
            std::bind(&TriggerRenJianHeYiDot, std::placeholders::_1));
    } else {
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_REN_JIAN_HE_YI_DOT,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_REN_JIAN_HE_YI_DOT,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_REN_JIAN_HE_YI_DOT,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_REN_JIAN_HE_YI_DOT,
            std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (equipEffects[EQUIP_EFFECT_WEAPON_CW]) {
        buffs.emplace(BUFF_WEAPON_EFFECT_CW,
                      static_cast<JX3DPS::Buff *>(new Buff::WeaponEffectCW(this, nullptr)));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                          std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                      std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                         std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                          std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                       std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                         std::bind(&TriggerWeaponCW, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                          std::bind(&TriggerWeaponCW, std::placeholders::_1));
        }

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                       std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_WEAPON_CW,
            std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_WEAPON_CW,
            std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_WEAPON_CW,
            std::bind(&TriggerWeaponCW, std::placeholders::_1));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(
            TRIGGER_WEAPON_CW_DOT,
            std::bind(&TriggerWeaponCWDot, std::placeholders::_1));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(
            TRIGGER_WEAPON_CW_DAMAGE,
            std::bind(&TriggerWeaponCWDamage, std::placeholders::_1));
    } else {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        }

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]
            ->AddTriggerEffect(TRIGGER_WEAPON_CW, std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]
            ->AddTriggerEffect(TRIGGER_WEAPON_CW, std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]
            ->AddTriggerEffect(TRIGGER_WEAPON_CW, std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_WEAPON_CW_DOT,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_WEAPON_CW_DAMAGE,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (equipEffects[EQUIP_EFFECT_WEAPON_WATER]) {
        buffs.emplace(BUFF_WEAPON_EFFECT_WATER,
                      static_cast<JX3DPS::Buff *>(new Buff3rd::WeaponEffectWater(this, nullptr)));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                      std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                       std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerWeaponWater, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(
                TRIGGER_WEAPON_WATER,
                std::bind(&TriggerWeaponWater, std::placeholders::_1));
        }

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                       std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerWeaponWater, std::placeholders::_1));
    } else {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        }

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (equipEffects[EQUIP_EFFECT_SET_ATTRIBUTE]) {
        buffs.emplace(BUFF_SET_ATTRIBUTE,
                      static_cast<JX3DPS::Buff *>(new Buff::SetAttribute(this, nullptr)));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                      std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                       std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerSetAttribute, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(
                TRIGGER_SET_ATTRIBUTE,
                std::bind(&TriggerSetAttribute, std::placeholders::_1));
        }

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                       std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerSetAttribute, std::placeholders::_1));
    } else {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        }

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (equipEffects[EQUIP_EFFECT_ENCHANT_SHOES]) {
        buffs.emplace(BUFF_ENCHANT_SHOES,
                      static_cast<JX3DPS::Buff *>(new Buff3rd::EnchantShoesPhysics(this, nullptr)));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                      std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                       std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(
                TRIGGER_ENCHANT_SHOES,
                std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        }

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                       std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        if (talents[TALENT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI]->AddTriggerEffect(
                TRIGGER_ENCHANT_SHOES,
                std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        }
        if (recipes[RECIPE_REN_JIAN_HE_YI_EFFECT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN]->AddTriggerEffect(
                TRIGGER_ENCHANT_SHOES,
                std::bind(&TriggerEnchantShoes, std::placeholders::_1));
            buffs[BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE]->AddTriggerEffect(
                TRIGGER_ENCHANT_SHOES,
                std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        }
    } else {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        }

        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI]->AddTriggerEffect(
                TRIGGER_ENCHANT_SHOES,
                std::bind(&TriggerVoid, std::placeholders::_1));
        }
        if (recipes[RECIPE_REN_JIAN_HE_YI_EFFECT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN]->AddTriggerEffect(
                TRIGGER_ENCHANT_SHOES,
                std::bind(&TriggerVoid, std::placeholders::_1));
            buffs[BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE]->AddTriggerEffect(
                TRIGGER_ENCHANT_SHOES,
                std::bind(&TriggerVoid, std::placeholders::_1));
        }
    }

    if (equipEffects[EQUIP_EFFECT_ENCHANT_BELT]) {
        buffs.emplace(BUFF_ENCHANT_BELT,
                      static_cast<JX3DPS::Buff *>(new Buff3rd::EnchantBelt(this, nullptr)));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                      std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                       std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(
                TRIGGER_ENCHANT_BELT,
                std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        }
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                       std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        if (talents[TALENT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI]->AddTriggerEffect(
                TRIGGER_ENCHANT_BELT,
                std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        }

        if (recipes[RECIPE_REN_JIAN_HE_YI_EFFECT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN]->AddTriggerEffect(
                TRIGGER_ENCHANT_BELT,
                std::bind(&TriggerEnchantBelt, std::placeholders::_1));
            buffs[BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE]->AddTriggerEffect(
                TRIGGER_ENCHANT_BELT,
                std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        }
    } else {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        }
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI]->AddTriggerEffect(
                TRIGGER_ENCHANT_BELT,
                std::bind(&TriggerVoid, std::placeholders::_1));
        }
        if (recipes[RECIPE_REN_JIAN_HE_YI_EFFECT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN]->AddTriggerEffect(
                TRIGGER_ENCHANT_BELT,
                std::bind(&TriggerVoid, std::placeholders::_1));
            buffs[BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE]->AddTriggerEffect(
                TRIGGER_ENCHANT_BELT,
                std::bind(&TriggerVoid, std::placeholders::_1));
        }
    }

    if (equipEffects[EQUIP_EFFECT_ENCHANT_WRIST]) {
        buffs.emplace(BUFF_ENCHANT_WRIST,
                      static_cast<JX3DPS::Buff *>(new Buff3rd::EnchantWristPhysics(this, nullptr)));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                      std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                       std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(
                TRIGGER_ENCHANT_WRIST,
                std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        }
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                       std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_SHENG_TAI_JI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                     std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_SUI_XING_CHEN]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                      std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_TUN_RI_YUE]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                   std::bind(&TriggerEnchantWrist, std::placeholders::_1));

        if (talents[TALENT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI]->AddTriggerEffect(
                TRIGGER_ENCHANT_WRIST,
                std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        }
        if (recipes[RECIPE_REN_JIAN_HE_YI_EFFECT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN]->AddTriggerEffect(
                TRIGGER_ENCHANT_WRIST,
                std::bind(&TriggerEnchantWrist, std::placeholders::_1));
            buffs[BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE]->AddTriggerEffect(
                TRIGGER_ENCHANT_WRIST,
                std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        }
    }

    else
    {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        }
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SHENG_TAI_JI]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_SUI_XING_CHEN]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI_TUN_RI_YUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHENG_TAI_JI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                     std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SUI_XING_CHEN]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_TUN_RI_YUE]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                   std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI]->AddTriggerEffect(
                TRIGGER_ENCHANT_WRIST,
                std::bind(&TriggerVoid, std::placeholders::_1));
        }
        if (recipes[RECIPE_REN_JIAN_HE_YI_EFFECT_YUN_ZHONG_JIAN]) {
            buffs[BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN]->AddTriggerEffect(
                TRIGGER_ENCHANT_WRIST,
                std::bind(&TriggerVoid, std::placeholders::_1));
            buffs[BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE]->AddTriggerEffect(
                TRIGGER_ENCHANT_WRIST,
                std::bind(&TriggerVoid, std::placeholders::_1));
        }
    }

    if (teamCore == ClassType::TAI_XU_JIAN_YI) {
        buffs.emplace(BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
                      static_cast<JX3DPS::Buff *>(new Buff::TeamCoreTaiXuJianYiYouRen(this, nullptr)));
        buffs.emplace(BUFF_TEAM_CORE_TAI_XU_JIAN_YI_JING_MIAO,
                      static_cast<JX3DPS::Buff *>(new Buff::TeamCoreTaiXuJianYiJingMiao(this, nullptr)));

        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(
            TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
            std::bind(&TriggerTeamCoreTaiXuJianYiYouRen, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(
            TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
            std::bind(&TriggerTeamCoreTaiXuJianYiYouRen, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(
            TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
            std::bind(&TriggerTeamCoreTaiXuJianYiYouRen, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(
            TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
            std::bind(&TriggerTeamCoreTaiXuJianYiYouRen, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(
            TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
            std::bind(&TriggerTeamCoreTaiXuJianYiYouRen, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(
            TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
            std::bind(&TriggerTeamCoreTaiXuJianYiYouRen, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(
                TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
                std::bind(&TriggerTeamCoreTaiXuJianYiYouRen, std::placeholders::_1));
        }
    } else {
        skills[SKILL_BA_HUANG_GUI_YUAN]->AddTriggerEffect(TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_WO_WU_JIAN]->AddTriggerEffect(TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_HUAN_TAO_YUE]->AddTriggerEffect(TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WAN_JIAN_GUI_ZONG]->AddTriggerEffect(TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_REN_JIAN_HE_YI]->AddTriggerEffect(TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
                                                       std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SAN_CHAI_JIAN_FA]->AddTriggerEffect(TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
                                                         std::bind(&TriggerVoid, std::placeholders::_1));
        if (talents[TALENT_JING_HUA_YING]) {
            skills[SKILL_JING_HUA_YING]->AddTriggerEffect(TRIGGER_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        }
    }
}

void Player::TriggerWuYi(const Params &params)
{
    if (params.level > 6) {
        params.player->skills[SKILL_WU_WO_WU_JIAN]->AddCriticalStrikeAdditionalBasisPointInt(
            1000 * static_cast<int>(params.type));
        params.player->skills[SKILL_WU_WO_WU_JIAN]->AddCriticalStrikePowerAdditionalPercentInt(
            307 * static_cast<int>(params.type));
    }
}

void Player::TriggerFengShiAdd(const Params &params)
{
    static_cast<Buff::FengShi *>(params.player->buffs[BUFF_FENG_SHI])->TriggerAdd();
}

void Player::TriggerFengShiClear(const Params &params)
{
    static_cast<Buff::FengShi *>(params.player->buffs[BUFF_FENG_SHI])->TriggerClear();
    static_cast<Buff::DieRen *>(params.player->buffs[BUFF_DIE_REN])
        ->TriggerAdd(params.player->GetTargetId(), 1);
}

void Player::TriggerShenMai(const Params &params)
{
    params.player->AddQidian(2);
}

void Player::TriggerXuJi(const Params &params)
{
    static_cast<Buff::DieRen *>(params.player->buffs[BUFF_DIE_REN])->TriggerAdd(params.targetId, 1);
}

void Player::TriggerXuanMen(const Params &params)
{
    static_cast<Buff::XuanMen *>(params.player->buffs[BUFF_XUAN_MEN])->TriggerAdd(params.stackNum);
}

void Player::TriggerChangSheng(const Params &params)
{
    static_cast<Buff::ChiYing *>(params.player->buffs[BUFF_CHI_YING])->TriggerAdd(1);
}

void Player::TriggerChiYing(const Params &params)
{
    if (params.player->buffs[BUFF_CHI_YING]->GetDurationCurrent() > 0) {
        static_cast<Buff::ChiYing *>(params.player->buffs[BUFF_CHI_YING])->TriggerDamage();
    }
}

void Player::TriggerWuYu(const Params &params)
{
    if (params.player->buffs[BUFF_DIE_REN]->GetDurationCurrent(params.player->GetTargetId()) > 0)
    {
        params.player->skills[SKILL_BA_HUANG_GUI_YUAN]->UpdateKeyFrame((params.level + 1) * 8);
    }
}

void Player::TriggerDieRen(const Params &params)
{
    static_cast<Buff::DieRen *>(params.player->buffs[BUFF_DIE_REN])
        ->TriggerAdd(params.targetId, 1 + static_cast<int>(params.rollResult));
}

void Player::TriggerQieYu(const Params &params)
{
    if ((*params.player->GetTargets())[params.player->GetTargetId()]->GetLifePercent() <= 0.4 ||
        (params.player->buffs[BUFF_ZI_QI_DONG_LAI]->GetDurationCurrent() > 0 &&
         (*params.player->GetTargets())[params.player->GetTargetId()]->GetLifePercent() <= 0.6))
    {
        static_cast<TaiXuJianYi::Buff::DieRen *>(params.player->buffs[BUFF_DIE_REN])
            ->TriggerQieYu(params.player->GetTargetId());
    }
}

void Player::TriggerHuanYue(const Params &params)
{
    params.player->AddQidian(4);
    static_cast<TaiXuJianYi::Skill::PoZhao *>(params.player->skills[SKILL_PO_ZHAO])
        ->TriggerDamage(params.player->GetTargetId(), 1, 0);
    static_cast<TaiXuJianYi::Skill::PoZhao *>(params.player->skills[SKILL_PO_ZHAO])
        ->TriggerDamage(params.player->GetTargetId(), 1, 0);
    static_cast<TaiXuJianYi::Skill::PoZhao *>(params.player->skills[SKILL_PO_ZHAO])
        ->TriggerDamage(params.player->GetTargetId(), 1, 0);
    if (params.player->buffs[BUFF_DIE_REN]->GetDurationCurrent(params.player->GetTargetId()) > 0)
    {
        static_cast<TaiXuJianYi::Buff::DieRen *>(params.player->buffs[BUFF_DIE_REN])
            ->TriggerHuanYue(params.player->GetTargetId());
        static_cast<TaiXuJianYi::Buff::DieRen *>(params.player->buffs[BUFF_DIE_REN])
            ->TriggerHuanYue(params.player->GetTargetId());
        static_cast<TaiXuJianYi::Buff::DieRen *>(params.player->buffs[BUFF_DIE_REN])
            ->TriggerHuanYue(params.player->GetTargetId());
    }
}

void Player::TriggerJingHuaYing(const Params &params)
{
    if (params.player->buffs[BUFF_TUN_RI_YUE]->GetDurationCurrent(params.player->GetTargetId()) > 0)
    {
        static_cast<Buff::JingHuaYing *>(params.player->buffs[BUFF_JING_HUA_YING])->TriggerAdd();
    }
}

void Player::TriggerLieYun(const Params &params)
{
    static_cast<Buff::FieldLieYun *>(params.player->buffs[BUFF_HIDDEN_LIE_YUN])
        ->TriggerAdd(params.targetId);
}

void Player::TriggerGuChang(const Params &params)
{
    params.player->attribute.AddShieldIgnorePercentInt(614 * static_cast<int>(params.type));
}

void Player::TriggerQiSheng(const Params &params)
{
    static_cast<Buff::QiSheng *>(params.player->buffs[BUFF_QI_SHENG])->TriggerAdd();
}

void Player::TriggerFieldQiSheng(const Params &params)
{
    static_cast<Buff::FieldSuiXingChenQiSheng *>(params.player->buffs[BUFF_FIELD_SUI_XING_CHEN_QI_SHENG])
        ->TriggerAdd(params.stackNum * static_cast<int>(params.type));
}

void Player::TriggerRenJianHeYiDot(const Params &params)
{
    static_cast<Buff::RenJianHeYi *>(params.player->buffs[BUFF_REN_JIAN_HE_YI])
        ->TriggerAdd(params.targetId);
}

void Player::TriggerShengTaiJiEffectCooldown(const Params &params)
{
    for (auto &id : static_cast<TaiXuJianYi::Player *>(params.player)->fields) {
        if (id == BUFF_FIELD_SHENG_TAI_JI) {
            static_cast<Skill::ShengTaiJi *>(params.player->skills[SKILL_SHENG_TAI_JI])
                ->SetEnergyCooldownCurrent(16 * 8);
            break;
        }
    }
}

void Player::TriggerYunZhongJianSuiXingChen(const Params &params)
{
    static_cast<Buff::YunZhongJianSuiXingChen *>(params.player->buffs[BUFF_YUN_ZHONG_JIAN_SUI_XING_CHEN])
        ->TriggerAdd();
}

void Player::TriggerYunZhongJianTunRiYue(const Params &params)
{
    static_cast<Buff::YunZhongJianTunRiYue *>(params.player->buffs[BUFF_YUN_ZHONG_JIAN_TUN_RI_YUE])
        ->TriggerAdd();
}

void Player::TriggerYunZhongJianShengTaiJi(const Params &params)
{
    static_cast<Buff::YunZhongJianShengTaiJi *>(params.player->buffs[BUFF_YUN_ZHONG_JIAN_SHENG_TAI_JI])
        ->TriggerAdd();
}

void Player::TriggerEnchantShoes(const Params &params)
{
    if (params.rollResult == RollResult::DOUBLE) {
        static_cast<Buff3rd::EnchantShoesPhysics *>(params.player->buffs[BUFF_ENCHANT_SHOES])
            ->TriggerDamage();
    }
}

void Player::TriggerEnchantBelt(const Params &params)
{
    static_cast<Buff3rd::EnchantBelt *>(params.player->buffs[BUFF_ENCHANT_BELT])->TriggerAdd();
}

void Player::TriggerEnchantWrist(const Params &params)
{
    static_cast<Buff3rd::EnchantWristPhysics *>(params.player->buffs[BUFF_ENCHANT_WRIST])->TriggerDamage();
}

void Player::TriggerWeaponCW(const Params &params)
{
    static_cast<TaiXuJianYi::Buff::WeaponEffectCW *>(params.player->buffs[BUFF_WEAPON_EFFECT_CW])
        ->TriggerAdd();
}

void Player::TriggerWeaponCWDot(const Params &params)
{
    static_cast<TaiXuJianYi::Buff::WanXiangGuiYuan *>(params.player->buffs[BUFF_WAN_XIANG_GUI_YUAN])
        ->TriggerAdd(params.player->GetTargetId(), 1);
}

void Player::TriggerWeaponCWDamage(const Params &params)
{
    static_cast<TaiXuJianYi::Skill::BaHuangGuiYuan *>(params.player->skills[SKILL_BA_HUANG_GUI_YUAN])
        ->TriggerDamage();
}

void Player::TriggerSetAttribute(const Params &params)
{
    static_cast<TaiXuJianYi::Buff::SetAttribute *>(params.player->buffs[BUFF_SET_ATTRIBUTE])
        ->TriggerAdd();
}

void Player::TriggerTeamCoreTaiXuJianYiYouRen(const Params &params)
{
    if (params.rollResult == RollResult::DOUBLE) {
        static_cast<TaiXuJianYi::Buff::TeamCoreTaiXuJianYiYouRen *>(params.player->buffs[BUFF_TEAM_CORE_TAI_XU_JIAN_YI_YOU_REN])
            ->TriggerAdd();
    }
}

} // namespace TaiXuJianYi

} // namespace JX3DPS