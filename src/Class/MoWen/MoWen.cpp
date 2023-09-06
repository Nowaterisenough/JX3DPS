/**
 * Project: JX3DPS
 * File: MoWen.cpp
 * Description:
 * Created Date: 2023-07-31 16:03:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-06 17:57:13
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	----- ----------------------------------------------------------
 */

#include "MoWen.h"

#include "Buff3rd.h"
#include "MoWenBuff.h"
#include "MoWenSkill.h"
#include "Target.hpp"

namespace JX3DPS {

namespace MoWen {

Player::Player()
{
    attribute.SetClassType(ClassType::MO_WEN);
}

void Player::Init()
{
    skills.emplace(SKILL_PO_ZHAO, new Skill::PoZhao(this, nullptr));
    skills.emplace(SKILL_GONG, new Skill::Gong(this, nullptr));
    skills.emplace(SKILL_BIAN_GONG, new Skill::BianGong(this, nullptr));
    skills.emplace(SKILL_SHANG, new Skill::Shang(this, nullptr));
    skills.emplace(SKILL_JUE, new Skill::Jue(this, nullptr));
    skills.emplace(SKILL_ZHI, new Skill::Zhi(this, nullptr));
    skills.emplace(SKILL_BIAN_ZHI, new Skill::BianZhi(this, nullptr));
    skills.emplace(SKILL_YU, new Skill::Yu(this, nullptr));
    skills.emplace(SKILL_GAO_SHAN_LIU_SHUI, new Skill::GaoShanLiuShui(this, nullptr));
    skills.emplace(SKILL_YANG_CHUN_BAI_XUE, new Skill::YangChunBaiXue(this, nullptr));
    skills.emplace(SKILL_SHU_YING_HENG_XIE, new Skill::ShuYingHengXie(this, nullptr));
    skills.emplace(SKILL_GU_YING_HUA_SHUANG, new Skill::GuYingHuaShuang(this, nullptr));
    skills.emplace(SKILL_YI_XING_HUAN_YING, new Skill::YiXingHuanYing(this, nullptr));
    skills.emplace(SKILL_WU_YIN_LIU_LV, new Skill::WuYinLiuLv(this, nullptr));

    buffs.emplace(BUFF_QU_FENG, new Buff::QuFeng(this, nullptr));
    buffs.emplace(BUFF_SHANG, new Buff::Shang(this, nullptr));
    buffs.emplace(BUFF_JUE, new Buff::Jue(this, nullptr));
    buffs.emplace(BUFF_GAO_SHAN_LIU_SHUI, new Buff::GaoShanLiuShui(this, nullptr));
    buffs.emplace(BUFF_YANG_CHUN_BAI_XUE, new Buff::YangChunBaiXue(this, nullptr));
    buffs.emplace(BUFF_YING_ZI, new Buff::YingZi(this, nullptr));
    buffs.emplace(BUFF_GU_YING_HUA_SHUANG, new Buff::GuYingHuaShuang(this, nullptr));

    if (talents[TALENT_XIAN_FENG]) {
        buffs.emplace(BUFF_XIAN_FENG, new Buff::XianFeng(this, nullptr));
        buffs.emplace(BUFF_XIAN_FENG_BIAO_JI, new Buff::XianFengBiaoJi(this, nullptr));

        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                             std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                                  std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                              std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                            std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                            std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                                 std::bind(&TriggerXianFeng, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                           std::bind(&TriggerXianFeng, std::placeholders::_1));

        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG_DAMAGE,
                                            std::bind(&TriggerXianFengDamage, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG_DAMAGE,
                                                 std::bind(&TriggerXianFengDamage, std::placeholders::_1));

        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG_CLEAR,
                                            std::bind(&TriggerXianFengClear, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG_CLEAR,
                                                 std::bind(&TriggerXianFengClear, std::placeholders::_1));

        skills[SKILL_GAO_SHAN_LIU_SHUI]->AddTriggerEffect(
            TRIGGER_XIAN_FENG_BIAO_JI,
            std::bind(&TriggerXianFengBiaoJi, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_XIAN_FENG_BIAO_JI,
            std::bind(&TriggerXianFengBiaoJi, std::placeholders::_1));
        skills[SKILL_YI_XING_HUAN_YING]->AddTriggerEffect(
            TRIGGER_XIAN_FENG_BIAO_JI,
            std::bind(&TriggerXianFengBiaoJi, std::placeholders::_1));
    } else {
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                             std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_XIAN_FENG,
                                           std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG_DAMAGE,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG_DAMAGE,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG_CLEAR,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_XIAN_FENG_CLEAR,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_GAO_SHAN_LIU_SHUI]->AddTriggerEffect(TRIGGER_XIAN_FENG_BIAO_JI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_XIAN_FENG_BIAO_JI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YI_XING_HUAN_YING]->AddTriggerEffect(TRIGGER_XIAN_FENG_BIAO_JI,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_HAO_QING]) {
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_HAO_QING_ZHI,
                                            std::bind(&TriggerHaoQingZhi, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_HAO_QING_BIAN_ZHI,
                                                 std::bind(&TriggerHaoQingBianZhi, std::placeholders::_1));
    } else {
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_HAO_QING_ZHI,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_HAO_QING_BIAN_ZHI,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_YUN_HAN]) {
        buffs.emplace(BUFF_YUN_HAN, new Buff::YunHan(this, nullptr));

        buffs[BUFF_YING_ZI]->AddTriggerEffect(TRIGGER_YUN_HAN,
                                              std::bind(&TriggerYunHan, std::placeholders::_1));
    } else {
        buffs[BUFF_YING_ZI]->AddTriggerEffect(TRIGGER_YUN_HAN,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_CAN_LIAN]) {
        buffs.emplace(BUFF_CAN_LIAN, new Buff::CanLian(this, nullptr));

        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_CAN_LIAN_ADD,
            std::bind(&TriggerCanLianAdd, std::placeholders::_1));
        skills[SKILL_GAO_SHAN_LIU_SHUI]->AddTriggerEffect(
            TRIGGER_CAN_LIAN_CLEAR,
            std::bind(&TriggerCanLianClear, std::placeholders::_1));
    } else {
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_CAN_LIAN_ADD,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_GAO_SHAN_LIU_SHUI]->AddTriggerEffect(TRIGGER_CAN_LIAN_CLEAR,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_SHU_LI]) {
        buffs.emplace(BUFF_SHU_LI, new Buff::ShuLi(this, nullptr));

        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SHU_LI, std::bind(&TriggerShuLi, std::placeholders::_1));
    } else {
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SHU_LI, std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_SHI_XIANG]) {
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SHI_XIANG,
                                           std::bind(&TriggerShiXiang, std::placeholders::_1));
    } else {
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SHI_XIANG,
                                           std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_ZHI_ZHI]) {
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ZHI_ZHI,
                                                  std::bind(&TriggerZhiZhi, std::placeholders::_1));
    } else {
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ZHI_ZHI,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_LIU_ZHAO]) {
        buffs.emplace(BUFF_LIU_ZHAO, new Buff::LiuZhao(this, nullptr));

        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_LIU_ZHAO_DAMAGE,
                                            std::bind(&TriggerLiuZhaoDamage, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_LIU_ZHAO_DAMAGE,
                                                 std::bind(&TriggerLiuZhaoDamage, std::placeholders::_1));
        skills[SKILL_PO_ZHAO]->AddTriggerEffect(TRIGGER_LIU_ZHAO_SURPLUS_DAMAGE,
                                                std::bind(&TriggerLiuZhaoSurplusDamage, std::placeholders::_1));
    } else {
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_LIU_ZHAO_DAMAGE,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_LIU_ZHAO_DAMAGE,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_PO_ZHAO]->AddTriggerEffect(TRIGGER_LIU_ZHAO_SURPLUS_DAMAGE,
                                                std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (talents[TALENT_ZHENG_LV_HE_MING]) {
        skills.emplace(SKILL_ZHENG_LV_HE_MING_1, new Skill::ZhengLvHeMing1(this, nullptr));
        skills.emplace(SKILL_ZHENG_LV_HE_MING_2, new Skill::ZhengLvHeMing2(this, nullptr));
        skills.emplace(SKILL_ZHENG_LV_HE_MING_3, new Skill::ZhengLvHeMing3(this, nullptr));

        buffs.emplace(BUFF_ZHI_YIN_MIAO_YI, new Buff::ZhiYinMiaoYi(this, nullptr));
        buffs.emplace(BUFF_ZHENG_LV_HE_MING, new Buff::ZhengLvHeMing(this, nullptr));
        buffs.emplace(BUFF_ZHI_YIN_HE_MING, new Buff::ZhiYinHeMing(this, nullptr));
    }

    if (equipEffects[EQUIP_EFFECT_ENCHANT_SHOES]) {
        buffs.emplace(BUFF_ENCHANT_SHOES, new Buff3rd::EnchantShoesMagic(this, nullptr));

        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                             std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                  std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                              std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                 std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                           std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_SHOES,
            std::bind(&TriggerEnchantShoes, std::placeholders::_1));
        skills[SKILL_WU_YIN_LIU_LV]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                      std::bind(&TriggerEnchantShoes, std::placeholders::_1));
    } else {
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                             std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_YIN_LIU_LV]->AddTriggerEffect(TRIGGER_ENCHANT_SHOES,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (equipEffects[EQUIP_EFFECT_ENCHANT_BELT]) {
        buffs.emplace(BUFF_ENCHANT_BELT, new Buff3rd::EnchantBelt(this, nullptr));

        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                             std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                  std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                              std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                 std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                           std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_BELT,
            std::bind(&TriggerEnchantBelt, std::placeholders::_1));
        skills[SKILL_WU_YIN_LIU_LV]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                      std::bind(&TriggerEnchantBelt, std::placeholders::_1));

    } else {
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                             std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_YIN_LIU_LV]->AddTriggerEffect(TRIGGER_ENCHANT_BELT,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (equipEffects[EQUIP_EFFECT_ENCHANT_WRIST]) {
        buffs.emplace(BUFF_ENCHANT_WRIST, new Buff3rd::EnchantWristMagic(this, nullptr));

        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                             std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                  std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                              std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                 std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                           std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));

        skills[SKILL_WU_YIN_LIU_LV]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                      std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_SHU_YING_HENG_XIE]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_ZHENG_LV_HE_MING_1]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_ZHENG_LV_HE_MING_2]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_ZHENG_LV_HE_MING_3]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_GU_YING_HUA_SHUANG]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));
        skills[SKILL_GAO_SHAN_LIU_SHUI]->AddTriggerEffect(
            TRIGGER_ENCHANT_WRIST,
            std::bind(&TriggerEnchantWrist, std::placeholders::_1));

    } else {
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                             std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_YIN_LIU_LV]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_SHU_YING_HENG_XIE]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHENG_LV_HE_MING_1]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHENG_LV_HE_MING_2]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHENG_LV_HE_MING_3]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_GU_YING_HUA_SHUANG]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_GAO_SHAN_LIU_SHUI]->AddTriggerEffect(TRIGGER_ENCHANT_WRIST,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (equipEffects[EQUIP_EFFECT_SET_ATTRIBUTE]) {
        buffs.emplace(BUFF_SET_ATTRIBUTE,
                      static_cast<JX3DPS::Buff *>(new Buff::SetAttribute(this, nullptr)));

        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                             std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                  std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                              std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                            std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                            std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                 std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                           std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_SET_ATTRIBUTE,
            std::bind(&TriggerSetAttribute, std::placeholders::_1));
        buffs[BUFF_LIU_ZHAO]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                               std::bind(&TriggerSetAttribute, std::placeholders::_1));
        buffs[BUFF_XIAN_FENG]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                std::bind(&TriggerSetAttribute, std::placeholders::_1));
        skills[SKILL_WU_YIN_LIU_LV]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                      std::bind(&TriggerSetAttribute, std::placeholders::_1));

    } else {
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                             std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        buffs[BUFF_LIU_ZHAO]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                               std::bind(&TriggerVoid, std::placeholders::_1));
        buffs[BUFF_XIAN_FENG]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_WU_YIN_LIU_LV]->AddTriggerEffect(TRIGGER_SET_ATTRIBUTE,
                                                      std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (equipEffects[EQUIP_EFFECT_WEAPON_CW]) {
        buffs.emplace(BUFF_WEAPON_EFFECT_CW,
                      static_cast<JX3DPS::Buff *>(new Buff::WeaponEffectCW(this, nullptr)));
        buffs.emplace(BUFF_SHEN_BING_GONG,
                      static_cast<JX3DPS::Buff *>(new Buff::ShenBingGong(this, nullptr)));

        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                             std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                  std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                              std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                            std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                            std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                 std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                           std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                          std::bind(&TriggerWeaponCW, std::placeholders::_1));
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_WEAPON_CW_DOT,
                                             std::bind(&TriggerWeaponCWDot, std::placeholders::_1));

        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_WEAPON_CW_DOT,
                                                  std::bind(&TriggerWeaponCWDot, std::placeholders::_1));

        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_WEAPON_CW_DAMAGE,
                                           std::bind(&TriggerWeaponCWDamage, std::placeholders::_1));
    } else {
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                             std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_WEAPON_CW,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_WEAPON_CW_DOT,
                                             std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_WEAPON_CW_DOT,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));

        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_WEAPON_CW_DAMAGE,
                                           std::bind(&TriggerVoid, std::placeholders::_1));
    }

    if (equipEffects[EQUIP_EFFECT_WEAPON_WATER]) {
        buffs.emplace(BUFF_WEAPON_EFFECT_WATER,
                      static_cast<JX3DPS::Buff *>(new Buff3rd::WeaponEffectWater(this, nullptr)));

        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                             std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                  std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                              std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                            std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                            std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                 std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                           std::bind(&TriggerWeaponWater, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(
            TRIGGER_WEAPON_WATER,
            std::bind(&TriggerWeaponWater, std::placeholders::_1));
    } else {
        skills[SKILL_GONG]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                             std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_GONG]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                  std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_SHANG]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                              std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_JUE]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_ZHI]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                            std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_BIAN_ZHI]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                 std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YU]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                           std::bind(&TriggerVoid, std::placeholders::_1));
        skills[SKILL_YANG_CHUN_BAI_XUE]->AddTriggerEffect(TRIGGER_WEAPON_WATER,
                                                          std::bind(&TriggerVoid, std::placeholders::_1));
    }
}

void Player::TriggerXianFeng(const Params &params)
{
    static_cast<Buff::XianFeng *>(params.player->buffs[BUFF_XIAN_FENG])->TriggerAdd();
}

void Player::TriggerXianFengBiaoJi(const Params &params)
{
    if (params.player->buffs[BUFF_XIAN_FENG]->GetDurationCurrent() > 0) {
        static_cast<Buff::XianFengBiaoJi *>(params.player->buffs[BUFF_XIAN_FENG_BIAO_JI])->TriggerAdd();
    }
}

void Player::TriggerXianFengDamage(const Params &params)
{
    if (params.player->buffs[BUFF_XIAN_FENG_BIAO_JI]->GetDurationCurrent() > 0) {
        static_cast<Buff::XianFeng *>(params.player->buffs[BUFF_XIAN_FENG])
            ->TriggerDamage(params.targetId, params.stackNum);
    }
}

void Player::TriggerXianFengClear(const Params &params)
{
    if (params.player->buffs[BUFF_XIAN_FENG]->GetDurationCurrent() > 0 &&
        params.player->buffs[BUFF_XIAN_FENG_BIAO_JI]->GetDurationCurrent() > 0)
    {
        static_cast<Buff::XianFeng *>(params.player->buffs[BUFF_XIAN_FENG])->TriggerClear();
    }
}

void Player::TriggerYunHan(const Params &params)
{
    static_cast<Buff::YunHan *>(params.player->buffs[BUFF_YUN_HAN])
        ->TriggerAdd(params.stackNum * static_cast<int>(params.type));
}

void Player::TriggerCanLianAdd(const Params &params)
{
    static_cast<Buff::CanLian *>(params.player->buffs[BUFF_CAN_LIAN])->TriggerAdd();
}

void Player::TriggerCanLianClear(const Params &params)
{
    static_cast<Buff::CanLian *>(params.player->buffs[BUFF_CAN_LIAN])->TriggerClear();
}

void Player::TriggerShuLi(const Params &params)
{
    static_cast<Buff::ShuLi *>(params.player->buffs[BUFF_SHU_LI])->TriggerAdd();
}

void Player::TriggerShiXiang(const Params &params)
{
    static_cast<Skill::Zhi *>(params.player->skills[SKILL_ZHI])->TriggerShiXiang();
    static_cast<Skill::BianZhi *>(params.player->skills[SKILL_BIAN_ZHI])->TriggerShiXiang();
    static_cast<Skill::Yu *>(params.player->skills[SKILL_YU])->TriggerShiXiang();
}

void Player::TriggerZhiZhi(const Params &params)
{
    if (params.player->buffs[BUFF_SHANG]->GetDurationCurrent(params.targetId) > 0) {
        static_cast<Buff::Shang *>(params.player->buffs[BUFF_SHANG])->TriggerAdd(params.targetId);
    }
    if (params.player->buffs[BUFF_JUE]->GetDurationCurrent(params.targetId) > 0) {
        static_cast<Buff::Jue *>(params.player->buffs[BUFF_JUE])->TriggerAdd(params.targetId);
    }

    static_cast<MoWen::Player *>(params.player)->cooldownStyleCurrent = 0;
}

void Player::TriggerHaoQingZhi(const Params &params)
{
    static_cast<Skill::Zhi *>(params.player->skills[SKILL_ZHI])->SubEffect();
}

void Player::TriggerHaoQingBianZhi(const Params &params)
{
    static_cast<Skill::BianZhi *>(params.player->skills[SKILL_BIAN_ZHI])->SubEffect();
}

void Player::TriggerLiuZhaoDamage(const Params &params)
{
    int stackNum = 0;
    if (params.player->buffs[BUFF_SHANG]->GetDurationCurrent(params.player->GetTargetId()) > 0)
    {
        stackNum++;
    }
    if (params.player->buffs[BUFF_JUE]->GetDurationCurrent(params.player->GetTargetId()) > 0)
    {
        stackNum++;
    }
    if (stackNum == 0) {
        return;
    }
    static_cast<Buff::LiuZhao *>(params.player->buffs[BUFF_LIU_ZHAO])->TriggerDamage(stackNum);
    static_cast<Buff::LiuZhao *>(params.player->buffs[BUFF_LIU_ZHAO])->TriggerAdd(stackNum);
}

void Player::TriggerLiuZhaoSurplusDamage(const Params &params)
{
    if (params.player->buffs[BUFF_LIU_ZHAO]->GetDurationCurrent() > 0) {
        static_cast<Buff::LiuZhao *>(params.player->buffs[BUFF_LIU_ZHAO])->TriggerSurplusDamage();
    }
}

void Player::TriggerEnchantShoes(const Params &params)
{
    if (params.rollResult == RollResult::DOUBLE) {
        static_cast<Buff3rd::EnchantShoesMagic *>(params.player->buffs[BUFF_ENCHANT_SHOES])
            ->TriggerDamage();
    }
}

void Player::TriggerEnchantBelt(const Params &params)
{
    static_cast<Buff3rd::EnchantBelt *>(params.player->buffs[BUFF_ENCHANT_BELT])->TriggerAdd();
}

void Player::TriggerEnchantWrist(const Params &params)
{
    static_cast<Buff3rd::EnchantWristMagic *>(params.player->buffs[BUFF_ENCHANT_WRIST])->TriggerDamage();
}

void Player::TriggerSetAttribute(const Params &params)
{
    static_cast<Buff::SetAttribute *>(params.player->buffs[BUFF_SET_ATTRIBUTE])->TriggerAdd();
}

void Player::TriggerWeaponCW(const Params &params)
{
    if (RandomUniform(1, 1024) <= 25) {
        static_cast<Buff::WeaponEffectCW *>(params.player->buffs[BUFF_WEAPON_EFFECT_CW])->TriggerAdd();
    }
}

void Player::TriggerWeaponCWDot(const Params &params)
{
    if (params.player->buffs[BUFF_WEAPON_EFFECT_CW]->GetDurationCurrent() > 0) {
        static_cast<Buff::ShenBingGong *>(params.player->buffs[BUFF_SHEN_BING_GONG])
            ->TriggerAdd(params.player->GetTargetId(), 1);
    }
}

void Player::TriggerWeaponCWDamage(const Params &params)
{
    if (RandomUniform(1, 1024) <= 307) {
        static_cast<Skill::Yu *>(params.player->skills[SKILL_YU])->TriggerDamage();
    }
}

} // namespace MoWen

} // namespace JX3DPS