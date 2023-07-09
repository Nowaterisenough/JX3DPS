/**
 * Project: JX3DPS
 * File: TaiXuJianYi.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-10 06:02:14
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "TaiXuJianYi.h"

#include <Utils/Utils.h>

#include "Optimization.h"
#include "Player.h"

JX3DPS::TaiXuJianYi::Player::Player() : JX3DPS::Player()
{
    classType = Class::TAI_XU_JIAN_YI;
    attr      = std::make_unique<Attr>(Class::TAI_XU_JIAN_YI);
}

JX3DPS::TaiXuJianYi::Player::~Player() { }

void JX3DPS::TaiXuJianYi::Player::Init()
{
    damageAddPercentInt = 184;

    for (Id_t i = Talent::BEGIN; i < Talent::END; i++) {
        if (talents.find(i) == talents.end()) {
            talents[i] = false;
        }
    }

    skills.emplace(JX3DPS::Skill::WU_WO_WU_JIAN,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::WuWoWuJian>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::BA_HUANG_GUI_YUAN,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::SAN_HUAN_TAO_YUE,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::SanHuanTaoYue>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::WAN_JIAN_GUI_ZONG,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::WanJianGuiZong>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::REN_JIAN_HE_YI,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::RenJianHeYi>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::SAN_CHAI_JIAN_FA,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::SanChaiJianFa>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::SUI_XING_CHEN,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::SuiXingChen>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::SHENG_TAI_JI,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::ShengTaiJi>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::TUN_RI_YUE,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::TunRiYue>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::ZI_QI_DONG_LAI,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::ZiQiDongLai>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::JING_HUA_YING,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::JingHuaYing>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::REN_JIAN_HE_YI_SUI_XING_CHEN,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::RenJianHeYiSuiXingChen>(this, nullptr)));
    skills.emplace(JX3DPS::Skill::REN_JIAN_HE_YI_TUN_RI_YUE,
                   std::move(std::make_unique<JX3DPS::TaiXuJianYi::Skill::RenJianHeYiTunRiYue>(this, nullptr)));

    buffs.emplace(JX3DPS::Buff::DIE_REN,
                  std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::DieRen>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::ZI_QI_DONG_LAI,
                  std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::ZiQiDongLai>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::FIELD_SUI_XING_CHEN,
                  std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChen>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::FIELD_SHENG_TAI_JI,
                  std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::FieldShengTaiJi>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::FIELD_TUN_RI_YUE,
                  std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::FieldTunRiYue>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::SUI_XING_CHEN,
                  std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::SuiXingChen>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::TUN_RI_YUE,
                  std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::TunRiYue>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::CLASS_FEATURE,
                  std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::ClassFeatureRongJin>(this, nullptr)));

    if (talents[Talent::XUAN_MEN]) {
        buffs.emplace(JX3DPS::Buff::XUAN_MEN,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::XuanMen>(this, nullptr)));
    }

    if (talents[Talent::QI_SHENG]) {
        buffs.emplace(JX3DPS::Buff::FIELD_SUI_XING_CHEN_QI_SHENG,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChenQiSheng>(this, nullptr)));
        buffs.emplace(JX3DPS::Buff::QI_SHENG,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::QiSheng>(this, nullptr)));
    }

    if (talents[Talent::FENG_SHI]) {
        buffs.emplace(JX3DPS::Buff::FENG_SHI,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::FengShi>(this, nullptr)));
    }

    if (talents[Talent::LIE_YUN]) {
        buffs.emplace(JX3DPS::Buff::FIELD_LIE_YUN,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::FieldLieYun>(this, nullptr)));
        buffs.emplace(JX3DPS::Buff::LIE_YUN,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::LieYun>(this, nullptr)));
    }

    if (talents[Talent::JING_HUA_YING]) {
        buffs.emplace(JX3DPS::Buff::JING_HUA_YING,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::JingHuaYing>(this, nullptr)));
    }

    if (this->weaponCW) {
        buffs.emplace(JX3DPS::Buff::WEAPON_EFFECT_CW_1,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::WeaponEffectCW1>(this, nullptr)));
        buffs.emplace(JX3DPS::Buff::WAN_XIANG_GUI_YUAN,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::WanXiangGuiYuan>(this, nullptr)));
    }

    if (this->classSetBuff) {
        buffs.emplace(JX3DPS::Buff::CLASS_SET_BUFF,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::ClassSetBuffJianMing>(this, nullptr)));
    }

    if (talents[Talent::CHANG_SHENG]) {
        buffs.emplace(JX3DPS::Buff::CHI_YING,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::ChiYing>(this, nullptr)));
    }

    if (talents[Talent::YUN_ZHONG_JIAN]) {
        buffs.emplace(JX3DPS::Buff::YUN_ZHONG_JIAN_SHENG_TAI_JI,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::YunZhongJianShengTaiJi>(this, nullptr)));
    }

    if (secrets[JX3DPS::Skill::REN_JIAN_HE_YI][6]) {
        buffs.emplace(JX3DPS::Buff::REN_JIAN_HE_YI,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::RenJianHeYi>(this, nullptr)));
    }

    // 人剑合一 秘籍 气剑
    if (secrets[JX3DPS::Skill::REN_JIAN_HE_YI][7]) {
        buffs.emplace(JX3DPS::Buff::YUN_ZHONG_JIAN_SUI_XING_CHEN,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::YunZhongJianSuiXingChen>(this, nullptr)));
        buffs.emplace(JX3DPS::Buff::YUN_ZHONG_JIAN_TUN_RI_YUE,
                      std::move(std::make_unique<JX3DPS::TaiXuJianYi::Buff::YunZhongJianTunRiYue>(this, nullptr)));
    }

    if (enchantShoes) {
        buffs.emplace(JX3DPS::Buff::ENCHANT_SHOES,
                      std::move(std::make_unique<JX3DPS::Buff3rd::EnchantShoesPhysics>(this, nullptr)));
    }

    if (enchantWrist) {
        buffs.emplace(JX3DPS::Buff::ENCHANT_WRIST,
                      std::move(std::make_unique<JX3DPS::Buff3rd::EnchantWristPhysics>(this, nullptr)));
    }

    if (enchantBelt) {
        buffs.emplace(JX3DPS::Buff::ENCHANT_BELT,
                      std::move(std::make_unique<JX3DPS::Buff3rd::EnchantBelt>(this, nullptr)));
    }

    buffs.emplace(JX3DPS::Buff::THIRD_XIU_QI, std::move(std::make_unique<JX3DPS::Buff3rd::XiuQi>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_HAN_RU_LEI,
                  std::move(std::make_unique<JX3DPS::Buff3rd::HanRuLei>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_PO_FENG,
                  std::move(std::make_unique<JX3DPS::Buff3rd::PoFeng>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_JING_FENG,
                  std::move(std::make_unique<JX3DPS::Buff3rd::JingFeng>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_JIE_HUO,
                  std::move(std::make_unique<JX3DPS::Buff3rd::JieHuo>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_HAO_LING_SAN_JUN,
                  std::move(std::make_unique<JX3DPS::Buff3rd::HaoLingSanJun>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_SHE_SHEN_HONG_FA,
                  std::move(std::make_unique<JX3DPS::Buff3rd::SheShenHongFa>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_CHAO_SHENG,
                  std::move(std::make_unique<JX3DPS::Buff3rd::ChaoSheng>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_SHENG_YU_MING_XIN,
                  std::move(std::make_unique<JX3DPS::Buff3rd::ShengYuMingXin>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_ZHEN_FEN,
                  std::move(std::make_unique<JX3DPS::Buff3rd::ZhenFen>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_HAN_XIAO_QIAN_JUN,
                  std::move(std::make_unique<JX3DPS::Buff3rd::HanXiaoQianJun>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_SHU_KUANG,
                  std::move(std::make_unique<JX3DPS::Buff3rd::ShuKuang>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_HAN_CHANG_LIN_LI,
                  std::move(std::make_unique<JX3DPS::Buff3rd::HanChangLinLi>(this, nullptr)));
    buffs.emplace(JX3DPS::Buff::THIRD_JI_LEI, std::move(std::make_unique<JX3DPS::Buff3rd::JiLei>(this, nullptr)));

    if (enchantJacket) {
        attr->AddPhysicsAttackBase(450);
    }

    if (enchantHat) {
        attr->AddPhysicsOvercomeBase(999);
    }

    if (hasBuff3rd.hanRuLei) {
        attr->AddPhysicsAttackBasePercentInt(51);
    }

    if (hasBuff3rd.xiuQi) {
        attr->AddAgilityBase(244);
        attr->AddStrengthBase(244);
    }
}

/* ------------------------------------------------ Skill ----------------------------------------------- */

JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::WuWoWuJian(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::WU_WO_WU_JIAN;
    m_name  = "无我无剑";
    m_range = 4;

    m_subNames.push_back("");
    m_subNames.push_back("白虹");

    m_levelNames.push_back("1气点");
    m_levelNames.push_back("2气点");
    m_levelNames.push_back("3气点");
    m_levelNames.push_back("4气点");
    m_levelNames.push_back("5气点");
    m_levelNames.push_back("6气点");
    m_levelNames.push_back("7气点");
    m_levelNames.push_back("8气点");
    m_levelNames.push_back("9气点");
    m_levelNames.push_back("10气点");

    m_damageParams[0].emplace_back((2696 * 1 / 10 / 12 + (2696 + 286) * 1 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 1,
                                   0.0);
    m_damageParams[0].emplace_back((2696 * 2 / 10 / 12 + (2696 + 286) * 2 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 2,
                                   0.0);
    m_damageParams[0].emplace_back((2696 * 3 / 10 / 12 + (2696 + 286) * 3 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 3,
                                   0.0);
    m_damageParams[0].emplace_back((2696 * 4 / 10 / 12 + (2696 + 286) * 4 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 4,
                                   0.0);
    m_damageParams[0].emplace_back((2696 * 5 / 10 / 12 + (2696 + 286) * 5 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 5,
                                   0.0);
    m_damageParams[0].emplace_back((2696 * 6 / 10 / 12 + (2696 + 286) * 6 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 6,
                                   0.065);
    m_damageParams[0].emplace_back((2696 * 7 / 10 / 12 + (2696 + 286) * 7 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 7,
                                   0.065);
    m_damageParams[0].emplace_back((2696 * 8 / 10 / 12 + (2696 + 286) * 8 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 8,
                                   0.13);
    m_damageParams[0].emplace_back((2696 * 9 / 10 / 12 + (2696 + 286) * 9 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 9,
                                   0.13);
    m_damageParams[0].emplace_back((2696 * 10 / 10 / 12 + (2696 + 286) * 10 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 10,
                                   0.23);

    m_damageParams[1].emplace_back((2696 * 1 / 10 / 12 + (2696 + 286) * 1 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 1,
                                   0.0);
    m_damageParams[1].emplace_back((2696 * 2 / 10 / 12 + (2696 + 286) * 2 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 2,
                                   0.0);
    m_damageParams[1].emplace_back((2696 * 3 / 10 / 12 + (2696 + 286) * 3 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 3,
                                   0.0);
    m_damageParams[1].emplace_back((2696 * 4 / 10 / 12 + (2696 + 286) * 4 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 4,
                                   0.0);
    m_damageParams[1].emplace_back((2696 * 5 / 10 / 12 + (2696 + 286) * 5 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 5,
                                   0.0);
    m_damageParams[1].emplace_back((2696 * 6 / 10 / 12 + (2696 + 286) * 6 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 6,
                                   0.065);
    m_damageParams[1].emplace_back((2696 * 7 / 10 / 12 + (2696 + 286) * 7 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 7,
                                   0.065);
    m_damageParams[1].emplace_back((2696 * 8 / 10 / 12 + (2696 + 286) * 8 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 8,
                                   0.13);
    m_damageParams[1].emplace_back((2696 * 9 / 10 / 12 + (2696 + 286) * 9 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 9,
                                   0.13);
    m_damageParams[1].emplace_back((2696 * 10 / 10 / 12 + (2696 + 286) * 10 / 10 / 12) / 2,
                                   2048,
                                   19 * 1.1 * 1.1 * 1.1 * 1.1 * 1.05 * 10,
                                   0.23);

    if (m_player->talents[Talent::TONG_GEN]) {
        m_skillDamageAddPercentInt += 102;
    }

    if (m_player->secrets[Skill::WU_WO_WU_JIAN][0]) {
        m_skillCriticalStrikeAddPercent += 0.02;
    }

    if (m_player->secrets[Skill::WU_WO_WU_JIAN][1]) {
        m_skillCriticalStrikeAddPercent += 0.03;
    }

    if (m_player->secrets[Skill::WU_WO_WU_JIAN][2]) {
        m_skillCriticalStrikeAddPercent += 0.04;
    }

    if (m_player->secrets[Skill::WU_WO_WU_JIAN][3]) {
        m_skillDamageAddPercentInt += 30;
    }

    if (m_player->secrets[Skill::WU_WO_WU_JIAN][4]) {
        m_skillDamageAddPercentInt += 40;
    }

    if (m_player->secrets[Skill::WU_WO_WU_JIAN][5]) {
        m_skillDamageAddPercentInt += 51;
    }

    if (m_player->secrets[Skill::WU_WO_WU_JIAN][6]) {
        m_skillQidianAdd += 1;
    }

    if (m_player->classSetSkill) {
        m_skillDamageAddPercentInt += 102;
    }
}

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::Cast()
{
    m_player->lastCastSkill = m_id;

    *m_globalCooldown = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::TriggerFengShiAdd()
{
    m_skillDamageAddPercentInt += 307;
}

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::TriggerFengShiClear()
{
    m_skillDamageAddPercentInt -= 307;
}

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::SubEffect()
{
    int qidian = m_player->qidian;

    m_player->qidian = 0;

    // 续气秘籍
    m_player->AddQidian(m_skillQidianAdd);

    int level = qidian - 1;

    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->targetId, rollResult, 0, level);
    Record(m_player->targetId, rollResult, damage, 0, level);

    // 无意 3格气以上
    if (m_player->talents.at(Talent::WU_YI) && qidian > 6) {
        m_skillCriticalStrikeAddPercent      += 0.1;
        m_skillCriticalStrikePowerAddPercent += 0.3;
    }

    // 白虹 目标外额外5个目标
    if (m_player->talents.at(Talent::BAI_HONG)) {
        int count = 0;
        for (auto &target : (*m_targets)) {
            if (target.second->GetDistance() > 6 || target.first == m_player->targetId) {
                continue;
            }
            count++;
            SubEffectBaiHong(target.first, level);
            if (count == 5) {
                break;
            }
        }
    }

    // 无欲 目标有叠刃
    if (m_player->talents.at(Talent::WU_YU) &&
        m_player->buffs[JX3DPS::Buff::DIE_REN]->GetTimeLeft(m_player->targetId) > 0)
    {
        m_player->skills[Skill::BA_HUANG_GUI_YUAN]->UpdateKeyFrame(qidian * 8);
    }

    // 长生 持盈
    if (m_player->talents.at(Talent::CHANG_SHENG) && m_player->buffs[JX3DPS::Buff::CHI_YING]->GetTimeLeft() > 0)
    {
        static_cast<Buff::ChiYing *>(m_player->buffs[JX3DPS::Buff::CHI_YING].get())->TriggerDamage();
    }

    // 大附魔 腕
    if (m_player->enchantWrist) [[likely]] {
        static_cast<Buff3rd::EnchantWristPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_WRIST].get())
            ->TriggerDamage();
    }

    // 大附魔 腰
    if (m_player->enchantBelt) [[likely]] {
        static_cast<Buff3rd::EnchantBelt *>(m_player->buffs[JX3DPS::Buff::ENCHANT_BELT].get())->TriggerAdd();
    }

    // 大附魔 鞋
    if (m_player->enchantShoes && rollResult == RollResult::DOUBLE) {
        static_cast<Buff3rd::EnchantShoesPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_SHOES].get())
            ->TriggerDamage();
    }

    // 叠刃
    if (m_player->talents.at(Talent::DIE_REN) && rollResult == RollResult::DOUBLE) {
        m_player->buffs[JX3DPS::Buff::DIE_REN]->Add(m_player->targetId, 2);
    } else if (m_player->talents.at(Talent::DIE_REN)) {
        m_player->buffs[JX3DPS::Buff::DIE_REN]->Add(m_player->targetId, 1);
    }

    // 深埋 会心
    if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
        m_player->AddQidian(2);
    }

    // 无意 结束去掉状态
    if (m_player->talents.at(Talent::WU_YI) && qidian > 6) {
        m_skillCriticalStrikeAddPercent      -= 0.1;
        m_skillCriticalStrikePowerAddPercent -= 0.3;
    }

    // 风逝 结束去掉状态
    if (m_player->talents.at(Talent::FENG_SHI) && m_player->buffs[JX3DPS::Buff::FENG_SHI]->GetTimeLeft() > 0)
    {
        m_player->buffs[JX3DPS::Buff::FENG_SHI]->Clear();
        m_player->buffs[JX3DPS::Buff::DIE_REN]->Add(m_player->targetId, 1);
    }

    // 橙武特效1 八荒归元无调息 持续伤害
    if (m_player->weaponCW && m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1]->GetTimeLeft() > 0) {
        static_cast<Buff::WeaponEffectCW1 *>(m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1].get())
            ->TriggerAdd();
    }

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    if (m_player->classSetBuff && m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF]->GetTimeLeft() > 0) {
        static_cast<Buff::ClassSetBuffJianMing *>(m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF].get())
            ->TriggerAdd();
    }
}

void JX3DPS::TaiXuJianYi::Skill::WuWoWuJian::SubEffectBaiHong(Id_t targetId, int qidian)
{
    int level = qidian - 1;

    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(targetId, rollResult, 1, level);
    Record(targetId, rollResult, damage, 1, level);

    // 风逝
    if (m_player->talents.at(Talent::FENG_SHI) && m_player->buffs[JX3DPS::Buff::FENG_SHI]->GetTimeLeft() > 0)
    {
        m_player->buffs[JX3DPS::Buff::DIE_REN]->Add(targetId, 1);
    }

    // 叠刃
    if (m_player->talents.at(Talent::DIE_REN) && rollResult == RollResult::DOUBLE) {
        m_player->buffs[JX3DPS::Buff::DIE_REN]->Add(targetId, 2);
    } else if (m_player->talents.at(Talent::DIE_REN)) {
        m_player->buffs[JX3DPS::Buff::DIE_REN]->Add(targetId, 1);
    }

    // 深埋 会心
    if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
        m_player->AddQidian(2);
    }
}

JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::BaHuangGuiYuan(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::BA_HUANG_GUI_YUAN;
    m_name  = "八荒归元";
    m_range = 12;

    m_subNames.push_back("一段");
    m_subNames.push_back("二段");
    m_subNames.push_back("橙武");

    m_levelNames.push_back("100%-90%");
    m_levelNames.push_back("90%-80%");
    m_levelNames.push_back("80%-70%");
    m_levelNames.push_back("70%-60%");
    m_levelNames.push_back("60%-50%");
    m_levelNames.push_back("50%-40%");
    m_levelNames.push_back("40%-30%");
    m_levelNames.push_back("30%-20%");
    m_levelNames.push_back("20%-10%");
    m_levelNames.push_back("10%-0%");

    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);
    m_damageParams[0].emplace_back((1801 / 15 * 1.1 * 1.05 + (1801 + 130) / 15 * 1.1 * 1.05) / 2, 0, 0, 0.0);

    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 1) * 1.1 * 1.1 * 1.05, 0.0);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 2) * 1.1 * 1.1 * 1.05, 0.0);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 3) * 1.1 * 1.1 * 1.05, 0.0);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 4) * 1.1 * 1.1 * 1.05, 0.0);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 5) * 1.1 * 1.1 * 1.05, 0.0);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 6) * 1.1 * 1.1 * 1.05, 0.0);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 7) * 1.1 * 1.1 * 1.05, 0.0);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 8) * 1.1 * 1.1 * 1.05, 0.0);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 9) * 1.1 * 1.1 * 1.05, 0.0);
    m_damageParams[1].emplace_back(0, 2048, (128 + 16 * 10) * 1.1 * 1.1 * 1.05, 0.0);

    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);
    m_damageParams[2].emplace_back((20 + 2) / 2, 0, 205, 0.0);

    m_cooldownFixed = 16 * 12;

    if (m_player->talents[Talent::FENG_SHI]) {
        m_range += 2.0;
    }

    if (m_player->secrets[Skill::BA_HUANG_GUI_YUAN][0]) {
        m_skillDamageAddPercentInt += 30;
    }

    if (m_player->secrets[Skill::BA_HUANG_GUI_YUAN][1]) {
        m_skillDamageAddPercentInt += 40;
    }

    if (m_player->secrets[Skill::BA_HUANG_GUI_YUAN][2]) {
        m_skillDamageAddPercentInt += 51;
    }

    if (m_player->secrets[Skill::BA_HUANG_GUI_YUAN][3]) {
        m_skillQidianAdd += 1;
    }

    if (m_player->secrets[Skill::BA_HUANG_GUI_YUAN][4]) {
        m_cooldownFixed -= 16 * 1;
    }
}

void JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::Cast()
{
    m_player->lastCastSkill = m_id;
    m_cooldown              = m_cooldownFixed;
    *m_globalCooldown       = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::SubEffect()
{
    m_player->AddQidian(2);

    // 续气秘籍
    m_player->AddQidian(m_skillQidianAdd);

    int level = 10 - ((*m_targets)[m_player->targetId]->GetLifePercent() - 0.01) * 10;

    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->targetId, rollResult, 0, level);
    Record(m_player->targetId, rollResult, damage, 0, level);

    // 深埋 会心
    if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
        m_player->AddQidian(2);
    }

    // 大附魔 腕
    if (m_player->enchantWrist) [[likely]] {
        static_cast<Buff3rd::EnchantWristPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_WRIST].get())
            ->TriggerDamage();
    }

    // 大附魔 腰
    if (m_player->enchantBelt) [[likely]] {
        static_cast<Buff3rd::EnchantBelt *>(m_player->buffs[JX3DPS::Buff::ENCHANT_BELT].get())->TriggerAdd();
    }

    // 大附魔 鞋
    if (m_player->enchantShoes && rollResult == RollResult::DOUBLE) {
        static_cast<Buff3rd::EnchantShoesPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_SHOES].get())
            ->TriggerDamage();
    }

    rollResult = GetPhysicsRollResult();
    damage     = CalcPhysicsDamage(m_player->targetId, rollResult, 1, level);
    Record(m_player->targetId, rollResult, damage, 1, level);

    // 风逝
    if (m_player->talents.at(Talent::FENG_SHI)) {
        m_player->buffs[JX3DPS::Buff::FENG_SHI]->Add();
    }

    // 切玉
    if (m_player->talents.at(Talent::QIE_YU) &&
        ((*m_targets)[m_player->targetId]->GetLifePercent() <= 0.4 ||
         m_player->buffs[JX3DPS::Buff::ZI_QI_DONG_LAI]->GetTimeLeft() > 0 &&
             (*m_targets)[m_player->targetId]->GetLifePercent() <= 0.6))
    {
        static_cast<TaiXuJianYi::Buff::DieRen *>(m_player->buffs[JX3DPS::Buff::DIE_REN].get())
            ->TriggerQieYu(m_player->targetId);
    }

    // 长生 持盈
    if (m_player->talents.at(Talent::CHANG_SHENG) && m_player->buffs[JX3DPS::Buff::CHI_YING]->GetTimeLeft() > 0)
    {
        static_cast<Buff::ChiYing *>(m_player->buffs[JX3DPS::Buff::CHI_YING].get())->TriggerDamage();
    }

    // 橙武特效1 八荒归元无调息 持续伤害
    if (m_player->weaponCW && m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1]->GetTimeLeft() > 0) {
        static_cast<Buff::WeaponEffectCW1 *>(m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1].get())
            ->TriggerAdd();
    }

    // 橙武特效1 持续伤害
    if (m_player->weaponCW && m_cooldownFixed == 0) {
        m_player->buffs[JX3DPS::Buff::WAN_XIANG_GUI_YUAN]->Add(m_player->targetId, 1);
    }

    // 橙武特效2 八荒归元 额外一次伤害
    if (m_player->weaponCW && RandomUniform(1, 100) <= 10) {
        RollResult  rollResult = GetPhysicsRollResult();
        GainsDamage damage     = CalcPhysicsDamage(m_player->targetId, rollResult, 2, level);
        Record(m_player->targetId, rollResult, damage, 2, level);
    }

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    if (m_player->classSetBuff && m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF]->GetTimeLeft() > 0) {
        static_cast<Buff::ClassSetBuffJianMing *>(m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF].get())
            ->TriggerAdd();
    }
}

void JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::ResetCooldown()
{
    m_cooldownFixed = 16 * 12;

    if (m_player->secrets[Skill::BA_HUANG_GUI_YUAN][4]) {
        m_cooldownFixed -= 16 * 1;
    }
}

void JX3DPS::TaiXuJianYi::Skill::BaHuangGuiYuan::ClearCooldown()
{
    m_cooldownFixed = 0;
}

JX3DPS::TaiXuJianYi::Skill::SanHuanTaoYue::SanHuanTaoYue(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::SAN_HUAN_TAO_YUE;
    m_name  = "三环套月";
    m_range = 4;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back((1233 / 10 + (1233 + 137) / 10) / 2, 1024, 120 * 1.1 * 1.05 * 1.1, 0.0);

    m_cooldownFixed = 16 * 2;

    if (m_player->talents[Talent::XIN_GU]) {
        m_skillCriticalStrikeAddPercent      += 0.1;
        m_skillCriticalStrikePowerAddPercent += 0.1;
    }
    if (m_player->talents[Talent::HUAN_YUE]) {
        m_cooldownFixed                           += 16 * 4;
        m_damageParams[0][0].surplusDamagePercent += 0.065;
    }
    if (m_player->talents[Talent::FENG_SHI]) {
        m_range += 2.0;
    }

    if (m_player->secrets[Skill::SAN_HUAN_TAO_YUE][0]) {
        m_cooldownFixed -= 16 / 2;
    }

    if (m_player->secrets[Skill::SAN_HUAN_TAO_YUE][1]) {
        m_skillCriticalStrikeAddPercent += 0.02;
    }

    if (m_player->secrets[Skill::SAN_HUAN_TAO_YUE][2]) {
        m_skillCriticalStrikeAddPercent += 0.03;
    }

    if (m_player->secrets[Skill::SAN_HUAN_TAO_YUE][3]) {
        m_skillCriticalStrikeAddPercent += 0.04;
    }

    if (m_player->secrets[Skill::SAN_HUAN_TAO_YUE][4]) {
        m_skillDamageAddPercentInt += 30;
    }

    if (m_player->secrets[Skill::SAN_HUAN_TAO_YUE][5]) {
        m_skillDamageAddPercentInt += 40;
    }

    if (m_player->secrets[Skill::SAN_HUAN_TAO_YUE][6]) {
        m_skillDamageAddPercentInt += 51;
    }
}

void JX3DPS::TaiXuJianYi::Skill::SanHuanTaoYue::Cast()
{
    m_player->lastCastSkill = m_id;
    m_cooldown              = m_cooldownFixed;
    *m_globalCooldown       = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::SanHuanTaoYue::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::SanHuanTaoYue::SubEffect()
{
    // 续气3格
    m_player->AddQidian(6);

    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->targetId, rollResult, 0, 0);
    Record(m_player->targetId, rollResult, damage, 0, 0);

    // 深埋 会心
    if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
        m_player->AddQidian(2);
    }

    // 大附魔 腕
    if (m_player->enchantWrist) [[likely]] {
        static_cast<Buff3rd::EnchantWristPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_WRIST].get())
            ->TriggerDamage();
    }

    // 大附魔 腰
    if (m_player->enchantBelt) [[likely]] {
        static_cast<Buff3rd::EnchantBelt *>(m_player->buffs[JX3DPS::Buff::ENCHANT_BELT].get())->TriggerAdd();
    }

    // 大附魔 鞋
    if (m_player->enchantShoes && rollResult == RollResult::DOUBLE) {
        static_cast<Buff3rd::EnchantShoesPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_SHOES].get())
            ->TriggerDamage();
    }

    damage = CalcPhysicsDamage(m_player->targetId, rollResult, 0, 0);
    Record(m_player->targetId, rollResult, damage, 0, 0);

    damage = CalcPhysicsDamage(m_player->targetId, rollResult, 0, 0);
    Record(m_player->targetId, rollResult, damage, 0, 0);

    // 风逝
    if (m_player->talents.at(Talent::FENG_SHI)) {
        m_player->buffs[JX3DPS::Buff::FENG_SHI]->Add();
    }

    // 环月
    if (m_player->talents.at(Talent::HUAN_YUE)) {
        m_player->AddQidian(6);
        if (m_player->buffs[JX3DPS::Buff::DIE_REN]->GetTimeLeft(m_player->targetId) > 0) {
            static_cast<TaiXuJianYi::Buff::DieRen *>(m_player->buffs[JX3DPS::Buff::DIE_REN].get())
                ->TriggerHuanYue(m_player->targetId);
            static_cast<TaiXuJianYi::Buff::DieRen *>(m_player->buffs[JX3DPS::Buff::DIE_REN].get())
                ->TriggerHuanYue(m_player->targetId);
            static_cast<TaiXuJianYi::Buff::DieRen *>(m_player->buffs[JX3DPS::Buff::DIE_REN].get())
                ->TriggerHuanYue(m_player->targetId);
        }
    }

    // 长生 持盈
    if (m_player->talents.at(Talent::CHANG_SHENG) && m_player->buffs[JX3DPS::Buff::CHI_YING]->GetTimeLeft() > 0)
    {
        static_cast<Buff::ChiYing *>(m_player->buffs[JX3DPS::Buff::CHI_YING].get())->TriggerDamage();
    }

    // 橙武特效1 八荒归元无调息 持续伤害
    if (m_player->weaponCW && m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1]->GetTimeLeft() > 0) {
        static_cast<Buff::WeaponEffectCW1 *>(m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1].get())
            ->TriggerAdd();
    }

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    if (m_player->classSetBuff && m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF]->GetTimeLeft() > 0) {
        static_cast<Buff::ClassSetBuffJianMing *>(m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF].get())
            ->TriggerAdd();
    }
}

JX3DPS::TaiXuJianYi::Skill::WanJianGuiZong::WanJianGuiZong(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::WAN_JIAN_GUI_ZONG;
    m_name  = "万剑归宗";
    m_range = 8;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back((1132 / 10 * 1.5 + (1132 + 113) / 10 * 1.5) / 2, 0, 100 * 1.1 * 1.25 * 1.05 * 1.5 * 1.1, 0.0);

    m_cooldownFixed    = 16 * 12;
    m_targetCountFixed = 6;

    if (m_player->talents[Talent::TONG_GEN]) {
        m_skillDamageAddPercentInt += 102;
        m_targetCountFixed         += 2;
    }

    if (m_player->talents[Talent::XU_JI]) {
        m_skillCriticalStrikeAddPercent      += 0.1;
        m_skillCriticalStrikePowerAddPercent += 0.1;
        m_cooldownFixed                      -= 16 * 2;
    }
}

void JX3DPS::TaiXuJianYi::Skill::WanJianGuiZong::Cast()
{
    m_player->lastCastSkill = m_id;
    m_cooldown              = m_cooldownFixed;
    *m_globalCooldown       = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::WanJianGuiZong::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::WanJianGuiZong::SubEffect()
{
    int count = 0;
    for (auto &target : *m_targets) {
        if (target.second->GetDistance() > m_range) { // 超出范围
            continue;
        }
        if (count == m_targetCountFixed) {
            break;
        }
        count++;

        // 命中一个目标 续气半格
        m_player->AddQidian(1);

        RollResult rollResult = GetPhysicsRollResult();

        // 深埋 会心
        if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
            m_player->AddQidian(2);
        }

        // 大附魔 腕
        if (m_player->enchantWrist) [[likely]] {
            static_cast<Buff3rd::EnchantWristPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_WRIST].get())
                ->TriggerDamage();
        }

        // 大附魔 腰
        if (m_player->enchantBelt) [[likely]] {
            static_cast<Buff3rd::EnchantBelt *>(m_player->buffs[JX3DPS::Buff::ENCHANT_BELT].get())->TriggerAdd();
        }

        // 大附魔 鞋
        if (m_player->enchantShoes && rollResult == RollResult::DOUBLE) {
            static_cast<Buff3rd::EnchantShoesPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_SHOES].get())
                ->TriggerDamage();
        }

        // 虚极
        if (m_player->talents[Talent::XU_JI]) {
            m_player->buffs[JX3DPS::Buff::DIE_REN]->Add(target.first);
        }

        // 镜花影
        if (m_player->talents[Talent::JING_HUA_YING] &&
            m_player->buffs[JX3DPS::Buff::TUN_RI_YUE]->GetTimeLeft(m_player->targetId) > 0)
        {
            m_player->buffs[JX3DPS::Buff::JING_HUA_YING]->Add();
        }

        // 橙武特效1 八荒归元无调息 持续伤害
        if (m_player->weaponCW && m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1]->GetTimeLeft() > 0)
        {
            static_cast<Buff::WeaponEffectCW1 *>(m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1].get())
                ->TriggerAdd();
        }

        GainsDamage damage = CalcPhysicsDamage(target.first, rollResult, 0, 0);
        Record(target.first, rollResult, damage, 0, 0);
    }

    // 长生 持盈 不会因为多目标触发
    if (m_player->talents.at(Talent::CHANG_SHENG) && m_player->buffs[JX3DPS::Buff::CHI_YING]->GetTimeLeft() > 0)
    {
        static_cast<Buff::ChiYing *>(m_player->buffs[JX3DPS::Buff::CHI_YING].get())->TriggerDamage();
    }

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    if (m_player->classSetBuff && m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF]->GetTimeLeft() > 0) {
        static_cast<Buff::ClassSetBuffJianMing *>(m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF].get())
            ->TriggerAdd();
    }
}

JX3DPS::TaiXuJianYi::Skill::RenJianHeYi::RenJianHeYi(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::REN_JIAN_HE_YI;
    m_name  = "人剑合一";
    m_range = 6;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(1273 / 20, 0, 40, 0.0);

    m_cooldownFixed = 16 * 20;

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][0]) {
        m_cooldownFixed -= 16 * 5;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][1]) {
        m_range += 1;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][2]) {
        m_range += 1;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][3]) {
        m_range += 1;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][4]) {
        m_skillDamageAddPercentInt += 409;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][5]) {
        m_skillDamageAddPercentInt += 614;
    }
}

void JX3DPS::TaiXuJianYi::Skill::RenJianHeYi::Cast()
{
    m_player->lastCastSkill = m_id;
    m_cooldown              = m_cooldownFixed;
    *m_globalCooldown       = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::RenJianHeYi::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::RenJianHeYi::SubEffect()
{
    m_player->skills[Skill::REN_JIAN_HE_YI_TUN_RI_YUE]->SetCooldown(m_cooldownFixed);
    m_player->skills[Skill::REN_JIAN_HE_YI_SUI_XING_CHEN]->SetCooldown(m_cooldownFixed);
    int count = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size();
    for (auto &target : *m_targets) {
        if (target.second->GetDistance() > m_range) [[unlikely]] { // 超出范围
            continue;
        }
        for (int i = 0; i < count; ++i) {

            RollResult rollResult = GetPhysicsRollResult();

            // 深埋 会心
            if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
                m_player->AddQidian(2);
            }

            // 大附魔 腕
            if (m_player->enchantWrist) [[likely]] {
                static_cast<Buff3rd::EnchantWristPhysics *>(
                    m_player->buffs[JX3DPS::Buff::ENCHANT_WRIST].get())
                    ->TriggerDamage();
            }

            // 大附魔 腰
            if (m_player->enchantBelt) [[likely]] {
                static_cast<Buff3rd::EnchantBelt *>(m_player->buffs[JX3DPS::Buff::ENCHANT_BELT].get())
                    ->TriggerAdd();
            }

            // 大附魔 鞋
            if (m_player->enchantShoes && rollResult == RollResult::DOUBLE) {
                static_cast<Buff3rd::EnchantShoesPhysics *>(
                    m_player->buffs[JX3DPS::Buff::ENCHANT_SHOES].get())
                    ->TriggerDamage();
            }

            // 橙武特效1 八荒归元无调息 持续伤害
            if (m_player->weaponCW && m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1]->GetTimeLeft() > 0)
            {
                static_cast<Buff::WeaponEffectCW1 *>(
                    m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1].get())
                    ->TriggerAdd();
            }

            if (m_player->secrets[Skill::REN_JIAN_HE_YI][6]) {
                m_player->buffs[JX3DPS::Buff::REN_JIAN_HE_YI]->Add(target.first);
            }

            GainsDamage damage = CalcPhysicsDamage(target.first, rollResult, 0, 0);
            Record(target.first, rollResult, damage, 0, 0);
        }
    }

    // 长生 持盈
    if (m_player->talents.at(Talent::CHANG_SHENG) && m_player->buffs[JX3DPS::Buff::CHI_YING]->GetTimeLeft() > 0)
    {
        static_cast<Buff::ChiYing *>(m_player->buffs[JX3DPS::Buff::CHI_YING].get())->TriggerDamage();
    }

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    if (m_player->classSetBuff && m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF]->GetTimeLeft() > 0) {
        static_cast<Buff::ClassSetBuffJianMing *>(m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF].get())
            ->TriggerAdd();
    }

    // 玄门
    if (m_player->talents[Talent::XUAN_MEN]) {
        m_player->buffs[JX3DPS::Buff::XUAN_MEN]->Add(JX3DPS_PLAYER, count);
    }

    // 清除气场 云中剑
    for (int i = 0; i < count; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();

        if (m_player->talents[Talent::YUN_ZHONG_JIAN] && id == JX3DPS::Buff::FIELD_SHENG_TAI_JI) {
            m_player->buffs[JX3DPS::Buff::YUN_ZHONG_JIAN_SHENG_TAI_JI]->Add();
        } else if (m_player->secrets[JX3DPS::Skill::REN_JIAN_HE_YI][7] &&
                   (id == JX3DPS::Buff::FIELD_SUI_XING_CHEN || id == JX3DPS::Buff::FIELD_TUN_RI_YUE))
        {
            m_player->buffs[id + 1]->Add();
        }
    }
}

JX3DPS::TaiXuJianYi::Skill::RenJianHeYiSuiXingChen::RenJianHeYiSuiXingChen(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::REN_JIAN_HE_YI_SUI_XING_CHEN;
    m_name  = "人剑合一·空爆·碎星辰";
    m_range = 6;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(1273 / 20, 0, 40, 0.0);

    m_cooldownFixed = 16 * 20;

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][0]) {
        m_cooldownFixed -= 16 * 5;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][1]) {
        m_range += 1;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][2]) {
        m_range += 1;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][3]) {
        m_range += 1;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][4]) {
        m_skillDamageAddPercentInt += 409;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][5]) {
        m_skillDamageAddPercentInt += 614;
    }
}

void JX3DPS::TaiXuJianYi::Skill::RenJianHeYiSuiXingChen::Cast()
{
    m_player->lastCastSkill = m_id;
    m_cooldown              = m_cooldownFixed;
    *m_globalCooldown       = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::RenJianHeYiSuiXingChen::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::RenJianHeYiSuiXingChen::SubEffect()
{
    m_player->skills[Skill::REN_JIAN_HE_YI]->SetCooldown(m_cooldownFixed);
    m_player->skills[Skill::REN_JIAN_HE_YI_TUN_RI_YUE]->SetCooldown(m_cooldownFixed);
    int count = 0;

    for (auto &id : static_cast<TaiXuJianYi::Player *>(m_player)->fields) {
        if (id == JX3DPS::Buff::FIELD_SUI_XING_CHEN) {
            continue;
        }
        count++;
        for (auto &target : *m_targets) {
            if (target.second->GetDistance() > m_range) [[unlikely]] { // 超出范围
                continue;
            }

            RollResult rollResult = GetPhysicsRollResult();

            // 深埋 会心
            if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
                m_player->AddQidian(2);
            }

            // 大附魔 腕
            if (m_player->enchantWrist) [[likely]] {
                static_cast<Buff3rd::EnchantWristPhysics *>(
                    m_player->buffs[JX3DPS::Buff::ENCHANT_WRIST].get())
                    ->TriggerDamage();
            }

            // 大附魔 腰
            if (m_player->enchantBelt) [[likely]] {
                static_cast<Buff3rd::EnchantBelt *>(m_player->buffs[JX3DPS::Buff::ENCHANT_BELT].get())
                    ->TriggerAdd();
            }

            // 大附魔 鞋
            if (m_player->enchantShoes && rollResult == RollResult::DOUBLE) {
                static_cast<Buff3rd::EnchantShoesPhysics *>(
                    m_player->buffs[JX3DPS::Buff::ENCHANT_SHOES].get())
                    ->TriggerDamage();
            }

            // 橙武特效1 八荒归元无调息 持续伤害
            if (m_player->weaponCW && m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1]->GetTimeLeft() > 0)
            {
                static_cast<Buff::WeaponEffectCW1 *>(
                    m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1].get())
                    ->TriggerAdd();
            }

            if (m_player->secrets[Skill::REN_JIAN_HE_YI][6]) {
                m_player->buffs[JX3DPS::Buff::REN_JIAN_HE_YI]->Add(target.first);
            }

            GainsDamage damage = CalcPhysicsDamage(target.first, rollResult, 0, 0);
            Record(target.first, rollResult, damage, 0, 0);
        }
    }

    // 长生 持盈
    if (m_player->talents.at(Talent::CHANG_SHENG) && m_player->buffs[JX3DPS::Buff::CHI_YING]->GetTimeLeft() > 0)
    {
        static_cast<Buff::ChiYing *>(m_player->buffs[JX3DPS::Buff::CHI_YING].get())->TriggerDamage();
    }

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    if (m_player->classSetBuff && m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF]->GetTimeLeft() > 0) {
        static_cast<Buff::ClassSetBuffJianMing *>(m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF].get())
            ->TriggerAdd();
    }

    // 玄门
    if (m_player->talents[Talent::XUAN_MEN]) {
        m_player->buffs[JX3DPS::Buff::XUAN_MEN]->Add(JX3DPS_PLAYER, count);
    }

    // 清除气场 云中剑
    for (int i = 0; i < count; ++i) {
        for (auto &id : static_cast<TaiXuJianYi::Player *>(m_player)->fields) {
            if (id == JX3DPS::Buff::FIELD_SUI_XING_CHEN) {
                continue;
            }
            m_player->buffs[id]->Clear();
            if (m_player->talents[Talent::YUN_ZHONG_JIAN] && id == JX3DPS::Buff::FIELD_SHENG_TAI_JI) {
                m_player->buffs[JX3DPS::Buff::YUN_ZHONG_JIAN_SHENG_TAI_JI]->Add();
            } else if (m_player->secrets[JX3DPS::Skill::REN_JIAN_HE_YI][7] && id == JX3DPS::Buff::FIELD_TUN_RI_YUE)
            {
                m_player->buffs[id + 1]->Add();
            }
            break;
        }
    }
}

JX3DPS::TaiXuJianYi::Skill::RenJianHeYiTunRiYue::RenJianHeYiTunRiYue(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::REN_JIAN_HE_YI_TUN_RI_YUE;
    m_name  = "人剑合一·空爆·吞日月";
    m_range = 6;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(1273 / 20, 0, 40, 0.0);

    m_cooldownFixed = 16 * 20;

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][0]) {
        m_cooldownFixed -= 16 * 5;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][1]) {
        m_range += 1;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][2]) {
        m_range += 1;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][3]) {
        m_range += 1;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][4]) {
        m_skillDamageAddPercentInt += 409;
    }

    if (m_player->secrets[Skill::REN_JIAN_HE_YI][5]) {
        m_skillDamageAddPercentInt += 614;
    }
}

void JX3DPS::TaiXuJianYi::Skill::RenJianHeYiTunRiYue::Cast()
{
    m_player->lastCastSkill = m_id;
    m_cooldown              = m_cooldownFixed;
    *m_globalCooldown       = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::RenJianHeYiTunRiYue::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::RenJianHeYiTunRiYue::SubEffect()
{
    m_player->skills[Skill::REN_JIAN_HE_YI]->SetCooldown(m_cooldownFixed);
    m_player->skills[Skill::REN_JIAN_HE_YI_SUI_XING_CHEN]->SetCooldown(m_cooldownFixed);
    int count = 0;

    for (auto &id : static_cast<TaiXuJianYi::Player *>(m_player)->fields) {
        if (id == JX3DPS::Buff::FIELD_TUN_RI_YUE) {
            continue;
        }
        count++;
        for (auto &target : *m_targets) {
            if (target.second->GetDistance() > m_range) [[unlikely]] { // 超出范围
                continue;
            }

            RollResult rollResult = GetPhysicsRollResult();

            // 深埋 会心
            if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
                m_player->AddQidian(2);
            }

            // 大附魔 腕
            if (m_player->enchantWrist) [[likely]] {
                static_cast<Buff3rd::EnchantWristPhysics *>(
                    m_player->buffs[JX3DPS::Buff::ENCHANT_WRIST].get())
                    ->TriggerDamage();
            }

            // 大附魔 腰
            if (m_player->enchantBelt) [[likely]] {
                static_cast<Buff3rd::EnchantBelt *>(m_player->buffs[JX3DPS::Buff::ENCHANT_BELT].get())
                    ->TriggerAdd();
            }

            // 大附魔 鞋
            if (m_player->enchantShoes && rollResult == RollResult::DOUBLE) {
                static_cast<Buff3rd::EnchantShoesPhysics *>(
                    m_player->buffs[JX3DPS::Buff::ENCHANT_SHOES].get())
                    ->TriggerDamage();
            }

            // 橙武特效1 八荒归元无调息 持续伤害
            if (m_player->weaponCW && m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1]->GetTimeLeft() > 0)
            {
                static_cast<Buff::WeaponEffectCW1 *>(
                    m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1].get())
                    ->TriggerAdd();
            }

            if (m_player->secrets[Skill::REN_JIAN_HE_YI][6]) {
                m_player->buffs[JX3DPS::Buff::REN_JIAN_HE_YI]->Add(target.first);
            }

            GainsDamage damage = CalcPhysicsDamage(target.first, rollResult, 0, 0);
            Record(target.first, rollResult, damage, 0, 0);
        }
    }

    // 长生 持盈
    if (m_player->talents.at(Talent::CHANG_SHENG) && m_player->buffs[JX3DPS::Buff::CHI_YING]->GetTimeLeft() > 0)
    {
        static_cast<Buff::ChiYing *>(m_player->buffs[JX3DPS::Buff::CHI_YING].get())->TriggerDamage();
    }

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    if (m_player->classSetBuff && m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF]->GetTimeLeft() > 0) {
        static_cast<Buff::ClassSetBuffJianMing *>(m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF].get())
            ->TriggerAdd();
    }

    // 玄门
    if (m_player->talents[Talent::XUAN_MEN]) {
        m_player->buffs[JX3DPS::Buff::XUAN_MEN]->Add(JX3DPS_PLAYER, count);
    }

    // 清除气场 云中剑
    for (int i = 0; i < count; ++i) {
        for (auto &id : static_cast<TaiXuJianYi::Player *>(m_player)->fields) {
            if (id == JX3DPS::Buff::FIELD_TUN_RI_YUE) {
                continue;
            }
            m_player->buffs[id]->Clear();
            if (m_player->talents[Talent::YUN_ZHONG_JIAN] && id == JX3DPS::Buff::FIELD_SHENG_TAI_JI) {
                m_player->buffs[JX3DPS::Buff::YUN_ZHONG_JIAN_SHENG_TAI_JI]->Add();
            } else if (m_player->secrets[JX3DPS::Skill::REN_JIAN_HE_YI][7] && id == JX3DPS::Buff::FIELD_SUI_XING_CHEN)
            {
                m_player->buffs[id + 1]->Add();
            }
            break;
        }
    }
}

JX3DPS::TaiXuJianYi::Skill::SanChaiJianFa::SanChaiJianFa(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id             = Skill::SAN_CHAI_JIAN_FA;
    m_name           = "三柴剑法";
    m_range          = 4;
    m_globalCooldown = &m_cooldown;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_cooldownFixed = 22;

    // TODO
    m_damageParams[0].emplace_back(0, 1024, 0, 0.0);
}

void JX3DPS::TaiXuJianYi::Skill::SanChaiJianFa::Cast()
{
    m_cooldown = m_cooldownFixed * m_player->attr->GetHastePercent();

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::SanChaiJianFa::Trigger() { }

void JX3DPS::TaiXuJianYi::Skill::SanChaiJianFa::SubEffect()
{

    RollResult rollResult = GetPhysicsRollResult();

    // 深埋 会心
    if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
        m_player->AddQidian(2);
    }

    // // 大附魔 腕
    // if (m_player->enchantWrist) [[likely]] {
    //     static_cast<Buff3rd::EnchantWristPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_WRIST].get())
    //         ->TriggerDamage();
    // }

    // // 大附魔 腰
    // if (m_player->enchantBelt) [[likely]] {
    //     static_cast<Buff3rd::EnchantBelt *>(m_player->buffs[JX3DPS::Buff::ENCHANT_BELT].get())->TriggerAdd();
    // }

    // // 大附魔 鞋
    // if (m_player->enchantShoes && rollResult == RollResult::DOUBLE) {
    //     static_cast<Buff3rd::EnchantShoesPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_SHOES].get())
    //         ->TriggerDamage();
    // }

    // 橙武特效1 八荒归元无调息 持续伤害
    if (m_player->weaponCW && m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1]->GetTimeLeft() > 0) {
        static_cast<Buff::WeaponEffectCW1 *>(m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1].get())
            ->TriggerAdd();
    }

    GainsDamage damage = CalcPhysicsDamage(m_player->targetId, rollResult, 0, 0);
    Record(m_player->targetId, rollResult, damage, 0, 0);

    // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
    if (m_player->classSetBuff && m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF]->GetTimeLeft() > 0) {
        static_cast<Buff::ClassSetBuffJianMing *>(m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF].get())
            ->TriggerAdd();
    }
}

JX3DPS::TaiXuJianYi::Skill::SuiXingChen::SuiXingChen(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::SUI_XING_CHEN;
    m_name  = "碎星辰";
    m_range = JX3DPS_UNLIMITED_RANGE;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_cooldownFixed      = 16 * 10;
    m_prepareFramesFixed = 16 * 1;
}

void JX3DPS::TaiXuJianYi::Skill::SuiXingChen::Cast()
{
    m_prepareFrames   = m_prepareFramesFixed * m_player->attr->GetHastePercent();
    m_cooldown        = JX3DPS_INVALID_FRAMES_SET;
    *m_globalCooldown = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;
    m_player->isCast   = true;
    m_player->isReCast = true;
}

void JX3DPS::TaiXuJianYi::Skill::SuiXingChen::Trigger()
{
    // 读条结束
    m_prepareFrames         = JX3DPS_INVALID_FRAMES_SET;
    m_cooldown              = m_cooldownFixed;
    m_player->lastCastSkill = m_id;
    m_player->isCast        = false;
    m_player->isReCast      = false;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::SuiXingChen::SubEffect()
{
    m_player->buffs[JX3DPS::Buff::FIELD_SUI_XING_CHEN]->Add();

    if (m_player->talents[Talent::CHANG_SHENG]) {
        m_player->buffs[JX3DPS::Buff::CHI_YING]->Add();
    }

    Record(JX3DPS_PLAYER, RollResult::HIT, GainsDamage(), 0, 0);
}

JX3DPS::TaiXuJianYi::Skill::ShengTaiJi::ShengTaiJi(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::SHENG_TAI_JI;
    m_name  = "生太极";
    m_range = JX3DPS_UNLIMITED_RANGE;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_cooldownFixed      = 16 * 10;
    m_prepareFramesFixed = 16 * 1;

    if (m_player->talents[Talent::XUAN_MEN]) {
        m_cooldownFixed += 16 * 15;
    }

    if (m_player->secrets[Skill::SHENG_TAI_JI][0]) {
        m_prepareFramesFixed -= 2;
    }

    if (m_player->secrets[Skill::SHENG_TAI_JI][1]) {
        m_prepareFramesFixed -= 2;
    }

    if (m_player->secrets[Skill::SHENG_TAI_JI][2]) {
        m_prepareFramesFixed -= 2;
    }
}

void JX3DPS::TaiXuJianYi::Skill::ShengTaiJi::Cast()
{
    m_prepareFrames   = m_prepareFramesFixed * m_player->attr->GetHastePercent();
    m_cooldown        = JX3DPS_INVALID_FRAMES_SET;
    *m_globalCooldown = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;
    m_player->isCast   = true;
    m_player->isReCast = true;
}

void JX3DPS::TaiXuJianYi::Skill::ShengTaiJi::Trigger()
{
    // 读条结束
    m_prepareFrames = JX3DPS_INVALID_FRAMES_SET;

    if (!m_player->secrets[Skill::SHENG_TAI_JI][6]) [[likely]] {
        m_cooldown = m_cooldownFixed;
    } else [[unlikely]] {
        for (auto &id : static_cast<TaiXuJianYi::Player *>(m_player)->fields) {
            if (id == JX3DPS::Buff::FIELD_SHENG_TAI_JI) {
                m_cooldown = m_cooldownFixed - 16 * 2;
            }
        }
    }

    m_player->lastCastSkill = m_id;
    m_player->isCast        = false;
    m_player->isReCast      = false;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::ShengTaiJi::SubEffect()
{
    m_player->buffs[JX3DPS::Buff::FIELD_SHENG_TAI_JI]->Add();

    if (m_player->talents[Talent::CHANG_SHENG]) {
        m_player->buffs[JX3DPS::Buff::CHI_YING]->Add();
    }

    Record(JX3DPS_PLAYER, RollResult::HIT, GainsDamage(), 0, 0);
}

JX3DPS::TaiXuJianYi::Skill::TunRiYue::TunRiYue(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::TUN_RI_YUE;
    m_name  = "吞日月";
    m_range = JX3DPS_UNLIMITED_RANGE;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_cooldownFixed      = 16 * 10;
    m_prepareFramesFixed = 16 * 1;
}

void JX3DPS::TaiXuJianYi::Skill::TunRiYue::Cast()
{
    m_prepareFrames   = m_prepareFramesFixed * m_player->attr->GetHastePercent();
    m_cooldown        = JX3DPS_INVALID_FRAMES_SET;
    *m_globalCooldown = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;
    m_player->isCast   = true;
    m_player->isReCast = true;
}

void JX3DPS::TaiXuJianYi::Skill::TunRiYue::Trigger()
{
    // 读条结束
    m_prepareFrames         = JX3DPS_INVALID_FRAMES_SET;
    m_cooldown              = m_cooldownFixed;
    m_player->lastCastSkill = m_id;
    m_player->isCast        = false;
    m_player->isReCast      = false;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::TunRiYue::SubEffect()
{
    m_player->buffs[JX3DPS::Buff::FIELD_TUN_RI_YUE]->Add();

    if (m_player->talents[Talent::CHANG_SHENG]) {
        m_player->buffs[JX3DPS::Buff::CHI_YING]->Add();
    }

    Record(JX3DPS_PLAYER, RollResult::HIT, GainsDamage(), 0, 0);
}

JX3DPS::TaiXuJianYi::Skill::ZiQiDongLai::ZiQiDongLai(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id                 = Skill::ZI_QI_DONG_LAI;
    m_name               = "紫气东来";
    m_range              = JX3DPS_UNLIMITED_RANGE;
    m_globalCooldown     = &m_cooldown;
    m_energyCountFixed   = 2;
    m_energyCount        = m_energyCountFixed;
    m_cooldownFixed      = 16 * 75;
    m_prepareFramesFixed = 16 * 3;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    if (m_player->talents[Talent::RUO_SHUI]) {
        m_cooldownFixed = 16 * 60;
        m_energyCount = m_energyCountFixed = 3;
    }
}

void JX3DPS::TaiXuJianYi::Skill::ZiQiDongLai::Cast()
{
    m_player->lastCastSkill = m_id;
    m_cooldown              = m_cooldownFixed;
    m_energyCount--;
    m_player->qidian = 10;

    SubEffect();
}

void JX3DPS::TaiXuJianYi::Skill::ZiQiDongLai::Trigger()
{
    // 冷却结束，充能
    m_energyCount++;
    if (m_energyCount >= m_energyCountFixed) {
        m_energyCount = m_energyCountFixed;
        m_cooldown    = JX3DPS_INVALID_FRAMES_SET;
    } else {
        m_cooldown = m_cooldownFixed;
    }
}

void JX3DPS::TaiXuJianYi::Skill::ZiQiDongLai::SubEffect()
{
    m_player->buffs[JX3DPS::Buff::ZI_QI_DONG_LAI]->Add();
    Record(JX3DPS_PLAYER, RollResult::HIT, GainsDamage(), 0, 0);
}

JX3DPS::TaiXuJianYi::Skill::JingHuaYing::JingHuaYing(JX3DPS::Player *player, Targets *targets) :
    Skill(player, targets)
{
    m_id    = Skill::JING_HUA_YING;
    m_name  = "镜花影";
    m_range = 8;

    m_subNames.push_back("");
    m_subNames.push_back("碎");

    m_levelNames.push_back("");

    // TODO ： 武器伤害 lua脚本没有，描述有
    m_damageParams[0].emplace_back((113 + 113 + 67) / 2, 0, 620 * 1.1 * 1.1, 0.0);

    m_damageParams[1].emplace_back(200, 0, 200 * 1.1 * 1.1, 0.0);

    m_prepareFramesFixed = 16 * 3;

    m_prepareFrames = JX3DPS_INVALID_FRAMES_SET;
    m_cooldown      = JX3DPS_INVALID_FRAMES_SET;
}

void JX3DPS::TaiXuJianYi::Skill::JingHuaYing::Cast()
{
    m_player->lastCastSkill = m_id;
    m_cooldown              = JX3DPS_INVALID_FRAMES_SET;
    *m_globalCooldown       = m_player->globalCooldownFixed * m_player->attr->GetHastePercent() +
                        RandomNormal(m_player->delayMin, m_player->delayMax) / JX3DPS_DELAY;
    m_prepareFrames = m_prepareFramesFixed;

    SubEffect(0);
}

void JX3DPS::TaiXuJianYi::Skill::JingHuaYing::Trigger()
{
    // 触发 镜花影·碎
    m_prepareFrames = JX3DPS_INVALID_FRAMES_SET;

    SubEffect(1);
}

void JX3DPS::TaiXuJianYi::Skill::JingHuaYing::TriggerAddJingHuaYing()
{
    m_cooldown = 0;
}

void JX3DPS::TaiXuJianYi::Skill::JingHuaYing::TriggerClearJingHuaYing()
{
    m_cooldown = JX3DPS_INVALID_FRAMES_SET;
}

void JX3DPS::TaiXuJianYi::Skill::JingHuaYing::SubEffect(int sub)
{
    if (sub == 1 && m_targets->find(m_player->targetId) == m_targets->end()) {
        return;
    }
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->targetId, rollResult, sub, 0);
    Record(m_player->targetId, rollResult, damage, sub, 0);

    if (sub == 0) {
        // 长生 持盈
        if (m_player->talents.at(Talent::CHANG_SHENG) &&
            m_player->buffs[JX3DPS::Buff::CHI_YING]->GetTimeLeft() > 0)
        {
            static_cast<Buff::ChiYing *>(m_player->buffs[JX3DPS::Buff::CHI_YING].get())->TriggerDamage();
        }

        // 深埋 会心
        if (m_player->talents.at(Talent::SHEN_MAI) && rollResult == RollResult::DOUBLE) {
            m_player->AddQidian(2);
        }

        // 大附魔 腕
        if (m_player->enchantWrist) [[likely]] {
            static_cast<Buff3rd::EnchantWristPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_WRIST].get())
                ->TriggerDamage();
        }

        // 大附魔 腰
        if (m_player->enchantBelt) [[likely]] {
            static_cast<Buff3rd::EnchantBelt *>(m_player->buffs[JX3DPS::Buff::ENCHANT_BELT].get())->TriggerAdd();
        }

        // 大附魔 鞋
        if (m_player->enchantShoes && rollResult == RollResult::DOUBLE) {
            static_cast<Buff3rd::EnchantShoesPhysics *>(m_player->buffs[JX3DPS::Buff::ENCHANT_SHOES].get())
                ->TriggerDamage();
        }

        // 橙武特效1 八荒归元无调息 持续伤害
        if (m_player->weaponCW && m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1]->GetTimeLeft() > 0)
        {
            static_cast<Buff::WeaponEffectCW1 *>(m_player->buffs[JX3DPS::Buff::WEAPON_EFFECT_CW_1].get())
                ->TriggerAdd();
        }

        // 门派套装效果 剑鸣 影响属性，需要在计算伤害之后
        if (m_player->classSetBuff && m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF]->GetTimeLeft() > 0)
        {
            static_cast<Buff::ClassSetBuffJianMing *>(m_player->buffs[JX3DPS::Buff::CLASS_SET_BUFF].get())
                ->TriggerAdd();
        }
    }
}

/* ------------------------------------------------ Buff ----------------------------------------------- */

JX3DPS::TaiXuJianYi::Buff::DieRen::DieRen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id    = Buff::DIE_REN;
    m_name  = "叠刃";
    m_tbuff = true;

    m_subNames.push_back("");
    m_subNames.push_back("切玉");

    m_levelNames.push_back("1层");
    m_levelNames.push_back("2层");
    m_levelNames.push_back("3层");
    m_levelNames.push_back("4层");
    m_levelNames.push_back("5层");
    m_levelNames.push_back("6层");
    m_levelNames.push_back("7层");

    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 1, 0.0);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 2, 0.0);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 3, 0.0);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 4, 0.0);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 5, 0.0);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 6, 0.0);
    m_damageParams[0].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 7, 0.0);

    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 1, 0.0);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 2, 0.0);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 3, 0.0);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 4, 0.0);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 5, 0.0);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 6, 0.0);
    m_damageParams[1].emplace_back(0, 0, static_cast<int>(58 * 1.15 * 1.1 * 1.1) * 7, 0.0);

    m_intervalFixed    = 16 * 3;
    m_stackNumFixed    = 5;
    m_effectCountFixed = 8;

    if (m_player->talents[Talent::LIE_YUN]) {
        m_stackNumFixed = 7;
    }

    if (m_player->talents[Talent::XU_JI]) {
        m_skillDamageAddPercentInt += 102 * 2;
    }
}

void JX3DPS::TaiXuJianYi::Buff::DieRen::Trigger()
{
    for (auto iter = m_targetSnapshots.begin(); iter != m_targetSnapshots.end();) {
        if (iter->second.interval == 0) {                      // 叠刃生效一次
            int level             = iter->second.stackNum - 1; // 层数
            iter->second.interval = m_intervalFixed * iter->second.hastePercent;
            SubEffect(iter->first, level);
        }
        if (iter->second.duration == 0) { // 叠刃消失
            SubEffectClear(iter->first, 0);
            iter = m_targetSnapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void JX3DPS::TaiXuJianYi::Buff::DieRen::TriggerQieYu(Id_t targetId)
{
    // 切玉
    int effectCount = m_targetSnapshots[targetId].duration / m_intervalFixed *
                      m_targetSnapshots[targetId].hastePercent; // 剩余效果次数
    int level = m_targetSnapshots[targetId].stackNum - 1;       // 层数

    SubEffectQieYun(targetId, level, effectCount);
    SubEffectClear(targetId, 0);
    m_targetSnapshots.erase(targetId); // 结算完删除，避免快照清空
}

void JX3DPS::TaiXuJianYi::Buff::DieRen::TriggerHuanYue(Id_t targetId)
{
    // 环月
    if (m_targetSnapshots.empty()) {
        return;
    }
    int level = m_targetSnapshots[targetId].stackNum - 1; // 层数
    m_targetSnapshots[targetId].duration -=
        static_cast<Frame_t>(m_intervalFixed * m_targetSnapshots[targetId].hastePercent);
    if (m_targetSnapshots[targetId].duration < 0) {
        m_targetSnapshots.erase(targetId);
        SubEffectClear(targetId, 0);
    } else if (m_targetSnapshots[targetId].duration == 0) {
        SubEffect(targetId, level);
        m_targetSnapshots.erase(targetId);
        SubEffectClear(targetId, 0);
    } else {
        SubEffect(targetId, level);
    }
}

void JX3DPS::TaiXuJianYi::Buff::DieRen::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(targetId) == m_targetSnapshots.end()) { // 不存在叠刃
        m_targetSnapshots[targetId].interval = m_intervalFixed * m_player->attr->GetHastePercent();
    }
    m_targetSnapshots[targetId].stackNum += stackNum;
    m_targetSnapshots[targetId].stackNum = std::min(m_targetSnapshots[targetId].stackNum, m_stackNumFixed);

    SubEffectAdd(targetId, m_targetSnapshots[targetId].stackNum);

    // 快照属性
    m_targetSnapshots[targetId].SnapShot(*m_player->attr.get(),
                                         m_skillCriticalStrikeAddPercent,
                                         m_skillCriticalStrikePowerAddPercent,
                                         m_skillDamageAddPercentInt);

    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[targetId].duration =
            m_intervalFixed * m_player->attr->GetHastePercent() * (m_effectCountFixed - 1) +
            m_targetSnapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
    } else [[unlikely]] {
        m_targetSnapshots[targetId].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::DieRen::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots[targetId].stackNum -= stackNum;
    int stack                             = m_targetSnapshots[targetId].stackNum;
    if (stack <= 0) [[likely]] { // 叠刃消失
        m_targetSnapshots.erase(targetId);
    }
    SubEffectClear(targetId, stack);
}

void JX3DPS::TaiXuJianYi::Buff::DieRen::SubEffect(Id_t targetId, int stackNum)
{
    RollResult  rollResult = GetPhysicsRollResultDot(targetId);
    GainsDamage damage     = CalcPhysicsDamageDot(targetId, rollResult, 0, stackNum, 1);
    Record(targetId, rollResult, damage, 0, stackNum);
}

void JX3DPS::TaiXuJianYi::Buff::DieRen::SubEffectQieYun(Id_t targetId, int stackNum, int effectCount)
{
    RollResult  rollResult = GetPhysicsRollResultDot(targetId);
    GainsDamage damage     = CalcPhysicsDamageDot(targetId, rollResult, 1, stackNum, effectCount);
    Record(targetId, rollResult, damage, 1, stackNum);
}

void JX3DPS::TaiXuJianYi::Buff::DieRen::SubEffectAdd(Id_t targetId, int stackNum)
{
    if (m_player->talents[Talent::LIE_YUN] && stackNum >= 4) {
        m_player->buffs[JX3DPS::Buff::FIELD_LIE_YUN]->Add(targetId);
    }
}

void JX3DPS::TaiXuJianYi::Buff::DieRen::SubEffectClear(Id_t targetId, int stackNum)
{
    if (m_player->talents[Talent::LIE_YUN] && stackNum < 4) {
        m_player->buffs[JX3DPS::Buff::FIELD_LIE_YUN]->Clear(targetId);
    }
}

JX3DPS::TaiXuJianYi::Buff::WanXiangGuiYuan::WanXiangGuiYuan(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id    = Buff::WAN_XIANG_GUI_YUAN;
    m_name  = "万象归元";
    m_tbuff = true;

    m_subNames.push_back("");

    m_levelNames.push_back("1层");
    m_levelNames.push_back("2层");
    m_levelNames.push_back("3层");

    m_damageParams[0].emplace_back(0, 0, 10 * 1, 0.0);
    m_damageParams[0].emplace_back(0, 0, 10 * 2, 0.0);
    m_damageParams[0].emplace_back(0, 0, 10 * 3, 0.0);

    m_intervalFixed    = 16 * 3;
    m_stackNumFixed    = 3;
    m_effectCountFixed = 10;
}

void JX3DPS::TaiXuJianYi::Buff::WanXiangGuiYuan::Trigger()
{
    for (auto iter = m_targetSnapshots.begin(); iter != m_targetSnapshots.end();) {
        if (iter->second.interval == 0) {                      // 叠刃生效一次
            int level             = iter->second.stackNum - 1; // 层数
            iter->second.interval = m_intervalFixed * iter->second.hastePercent;
            SubEffect(iter->first, level);
        }
        if (iter->second.duration == 0) { // 叠刃消失
            iter = m_targetSnapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void JX3DPS::TaiXuJianYi::Buff::WanXiangGuiYuan::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.find(targetId) == m_targetSnapshots.end()) { // 不存在叠刃
        m_targetSnapshots[targetId].interval = m_intervalFixed * m_player->attr->GetHastePercent();
    }
    m_targetSnapshots[targetId].stackNum += stackNum;
    m_targetSnapshots[targetId].stackNum = std::min(m_targetSnapshots[targetId].stackNum, m_stackNumFixed);

    // 快照属性
    m_targetSnapshots[targetId].SnapShot(*m_player->attr.get(),
                                         m_skillCriticalStrikeAddPercent,
                                         m_skillCriticalStrikePowerAddPercent,
                                         m_skillDamageAddPercentInt);

    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[targetId].duration =
            m_intervalFixed * m_player->attr->GetHastePercent() * (m_effectCountFixed - 1) +
            m_targetSnapshots[targetId].interval; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
    } else [[unlikely]] {
        m_targetSnapshots[targetId].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::WanXiangGuiYuan::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots[targetId].stackNum -= stackNum;
    int stack                             = m_targetSnapshots[targetId].stackNum;
    if (stack <= 0) [[likely]] { // 叠刃消失
        m_targetSnapshots.erase(targetId);
    }
}

void JX3DPS::TaiXuJianYi::Buff::WanXiangGuiYuan::SubEffect(Id_t targetId, int stackNum)
{
    RollResult  rollResult = GetPhysicsRollResultDot(targetId);
    GainsDamage damage     = CalcPhysicsDamageDot(targetId, rollResult, 0, stackNum, 1);
    Record(targetId, rollResult, damage, 0, stackNum);
}

JX3DPS::TaiXuJianYi::Buff::RenJianHeYi::RenJianHeYi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id    = Buff::REN_JIAN_HE_YI;
    m_name  = "人剑合一";
    m_tbuff = true;

    m_subNames.push_back("");
    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 16, 0.0);

    m_intervalFixed    = 16 * 3;
    m_effectCountFixed = 4;
}

void JX3DPS::TaiXuJianYi::Buff::RenJianHeYi::Trigger()
{
    for (auto iter = m_targetSnapshots.begin(); iter != m_targetSnapshots.end();) {
        if (iter->second.interval == 0) { // 生效一次
            iter->second.interval = m_intervalFixed * iter->second.hastePercent;
            SubEffect(iter->first);
        }
        if (iter->second.duration == 0) { // 消失
            iter = m_targetSnapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void JX3DPS::TaiXuJianYi::Buff::RenJianHeYi::Add(Id_t targetId, int stackNum, Frame_t duration)
{

    m_targetSnapshots[targetId].interval = m_intervalFixed * m_player->attr->GetHastePercent();

    // 快照属性
    m_targetSnapshots[targetId].SnapShot(*m_player->attr.get(),
                                         m_skillCriticalStrikeAddPercent,
                                         m_skillCriticalStrikePowerAddPercent,
                                         m_skillDamageAddPercentInt);

    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[targetId].duration =
            m_intervalFixed * m_player->attr->GetHastePercent() * m_effectCountFixed; // 对齐叠刃保证每次刷新都是8跳，且消失时最后一跳
    } else [[unlikely]] {
        m_targetSnapshots[targetId].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::RenJianHeYi::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(targetId);
}

void JX3DPS::TaiXuJianYi::Buff::RenJianHeYi::SubEffect(Id_t targetId)
{
    RollResult  rollResult = GetPhysicsRollResultDot(targetId);
    GainsDamage damage     = CalcPhysicsDamageDot(targetId, rollResult, 0, 0, 1);
    Record(targetId, rollResult, damage, 0, 0);
}

JX3DPS::TaiXuJianYi::Buff::ZiQiDongLai::ZiQiDongLai(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::ZI_QI_DONG_LAI;
    m_name = "紫气东来";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 10;
    m_intervalFixed = 16 * 1;
}

void JX3DPS::TaiXuJianYi::Buff::ZiQiDongLai::Trigger()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].interval == 0) [[likely]] {
        m_player->AddQidian(2);
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
    }
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == 0) {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
        SubEffectClear();
    }
}

void JX3DPS::TaiXuJianYi::Buff::ZiQiDongLai::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.empty()) { // 不存在紫气东来
        m_targetSnapshots[JX3DPS_PLAYER].stackNum++;
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
        if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
        } else [[unlikely]] {
            m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
        }
        SubEffectAdd();
    } else {
        if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_targetSnapshots[JX3DPS_PLAYER].duration += m_durationFixed;
        } else [[unlikely]] {
            m_targetSnapshots[JX3DPS_PLAYER].duration += duration;
        }
    }
}

void JX3DPS::TaiXuJianYi::Buff::ZiQiDongLai::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::ZiQiDongLai::SubEffectAdd()
{
    m_player->attr->AddPhysicsAttackBasePercentInt(256);
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(0.25);
    m_player->attr->AddPhysicsCriticalStrikePowerPercentFromCustom(0.25);
}

void JX3DPS::TaiXuJianYi::Buff::ZiQiDongLai::SubEffectClear()
{
    m_player->attr->AddPhysicsAttackBasePercentInt(-256);
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(-0.25);
    m_player->attr->AddPhysicsCriticalStrikePowerPercentFromCustom(-0.25);
}

JX3DPS::TaiXuJianYi::Buff::XuanMen::XuanMen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::XUAN_MEN;
    m_name = "玄门";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 40;

    m_stackNumFixed = 3;
}

void JX3DPS::TaiXuJianYi::Buff::XuanMen::Trigger()
{
    int stackNum = m_targetSnapshots[JX3DPS_PLAYER].stackNum;
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear(stackNum);
}

void JX3DPS::TaiXuJianYi::Buff::XuanMen::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    int stack = std::min(stackNum, m_stackNumFixed - m_targetSnapshots[JX3DPS_PLAYER].stackNum);
    if (stack > 0) [[unlikely]] { // 玄门不满层 or 没有玄门
        m_targetSnapshots[JX3DPS_PLAYER].stackNum += stack;
        SubEffectAdd(stack);
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::XuanMen::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots[JX3DPS_PLAYER].stackNum -= stackNum;
    int stack = std::max(m_targetSnapshots[JX3DPS_PLAYER].stackNum, 0);
    if (stack == 0) { // 清空玄门
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
    SubEffectClear(stack);
}

void JX3DPS::TaiXuJianYi::Buff::XuanMen::SubEffectAdd(int stackNum)
{
    m_player->attr->AddPhysicsOvercomeBasePercentInt(204 * stackNum);
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(0.03 * stackNum);
}

void JX3DPS::TaiXuJianYi::Buff::XuanMen::SubEffectClear(int stackNum)
{
    m_player->attr->AddPhysicsOvercomeBasePercentInt(-204 * stackNum);
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(-0.03 * stackNum);
}

JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChen::FieldSuiXingChen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::FIELD_SUI_XING_CHEN;
    m_name = "气场·碎星辰";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 24;
    m_intervalFixed = 16 * 3;
    m_stackNumFixed = 3;
    if (m_player->talents[Talent::FU_YIN]) {
        m_durationFixed += 16 * 12;
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChen::Trigger()
{
    // 遍历每一个碎星辰气场
    for (auto iter = m_targetSnapshots.begin(); iter != m_targetSnapshots.end();) {
        if (iter->second.interval == 0) [[likely]] { // 碎星辰生效一次
            iter->second.interval = m_intervalFixed;
            SubEffect();
        }
        if (iter->second.duration == 0) [[unlikely]] { // 气场消失
            iter = m_targetSnapshots.erase(iter);
            SubEffectClear();                          // 删除同步的碎星辰 期声气场

            // 删除player记录的气场信息
            static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id);
        } else [[likely]] {
            ++iter;
        }
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChen::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    // 最多三个气场
    stackNum = std::min(stackNum, m_stackNumFixed);
    // 计算要清除的气场数量
    int stack = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size() + stackNum - m_stackNumFixed;
    // 清除排序靠前的气场
    for (int i = 0; i < stack; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();
    }

    int size = m_targetSnapshots.size();
    for (int i = size; i < size + stackNum; ++i) {
        m_targetSnapshots[i].interval = m_intervalFixed;
        if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_targetSnapshots[i].duration = m_durationFixed;
        } else [[unlikely]] {
            m_targetSnapshots[i].duration = duration;
        }
        // player记录气场信息
        static_cast<TaiXuJianYi::Player *>(m_player)->fields.emplace_back(m_id);
    }
    SubEffectAdd(stackNum);
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChen::Clear(Id_t targetId, int stackNum)
{
    // 计算要清除的气场数量
    stackNum = std::min(stackNum, std::min(m_stackNumFixed, static_cast<int>(m_targetSnapshots.size())));
    for (int i = 0; i < stackNum; ++i) {
        auto iter = m_targetSnapshots.begin();
        iter      = m_targetSnapshots.erase(iter);
    }
    // 清除player记录的气场信息
    static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id, stackNum);
    SubEffectClear(stackNum); // 删除同步的碎星辰 期声气场
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChen::SubEffect()
{
    m_player->buffs[SUI_XING_CHEN]->Add();
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChen::SubEffectAdd(int stackNum)
{
    m_player->buffs[SUI_XING_CHEN]->Add();
    if (m_player->talents[Talent::QI_SHENG]) {
        m_player->buffs[FIELD_SUI_XING_CHEN_QI_SHENG]->Add(JX3DPS_PLAYER, stackNum);
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChen::SubEffectClear(int stackNum)
{
    if (m_player->talents[Talent::QI_SHENG]) {
        m_player->buffs[FIELD_SUI_XING_CHEN_QI_SHENG]->Clear(JX3DPS_PLAYER, stackNum);
    }
}

JX3DPS::TaiXuJianYi::Buff::FieldShengTaiJi::FieldShengTaiJi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::FIELD_SHENG_TAI_JI;
    m_name = "气场·生太极";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 24;
    m_intervalFixed = 16 * 3;
    m_stackNumFixed = 3;

    if (m_player->secrets[JX3DPS::Skill::SHENG_TAI_JI][3]) {
        m_range += 1;
    }

    if (m_player->secrets[JX3DPS::Skill::SHENG_TAI_JI][4]) {
        m_range -= 2;
    }

    if (m_player->secrets[JX3DPS::Skill::SHENG_TAI_JI][5]) {
        m_range -= 2;
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldShengTaiJi::Trigger()
{
    // 遍历每一个气场
    for (auto iter = m_targetSnapshots.begin(); iter != m_targetSnapshots.end();) {
        if (iter->second.interval == 0) [[likely]] { // 期声生效一次
            iter->second.interval = m_intervalFixed;
            SubEffect();
        }
        if (iter->second.duration == 0) [[unlikely]] { // 气场消失
            iter = m_targetSnapshots.erase(iter);

            // 删除player记录的气场信息
            static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id);
        } else [[likely]] {
            ++iter;
        }
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldShengTaiJi::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    // 最多三个气场
    stackNum = std::min(stackNum, m_stackNumFixed);
    // 计算要清除的气场数量
    int stack = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size() + stackNum - m_stackNumFixed;

    // 清除排序靠前的气场
    for (int i = 0; i < stack; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();
    }

    int size = m_targetSnapshots.size();
    for (int i = size; i < size + stackNum; ++i) {
        m_targetSnapshots[i].interval = m_intervalFixed;
        if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_targetSnapshots[i].duration = m_durationFixed;
        } else [[unlikely]] {
            m_targetSnapshots[i].duration = duration;
        }
        // player记录气场信息
        static_cast<TaiXuJianYi::Player *>(m_player)->fields.emplace_back(m_id);
    }

    SubEffectAdd(stackNum);
}

void JX3DPS::TaiXuJianYi::Buff::FieldShengTaiJi::Clear(Id_t targetId, int stackNum)
{
    // 计算要清除的气场数量
    stackNum = std::min(stackNum, std::min(m_stackNumFixed, static_cast<int>(m_targetSnapshots.size())));

    for (int i = 0; i < stackNum; ++i) {
        auto iter = m_targetSnapshots.begin();
        iter      = m_targetSnapshots.erase(iter);
    }
    // 清除player记录的气场信息
    static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id, stackNum);
}

void JX3DPS::TaiXuJianYi::Buff::FieldShengTaiJi::SubEffect()
{
    if (m_player->talents[Talent::QI_SHENG]) {
        m_player->buffs[QI_SHENG]->Add();
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldShengTaiJi::SubEffectAdd(int stackNum)
{
    if (m_player->talents[Talent::QI_SHENG]) {
        m_player->buffs[QI_SHENG]->Add();
    }
}

JX3DPS::TaiXuJianYi::Buff::FieldTunRiYue::FieldTunRiYue(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::FIELD_TUN_RI_YUE;
    m_name = "气场·吞日月";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 24;
    m_intervalFixed = 20;
    m_stackNumFixed = 3;
}

void JX3DPS::TaiXuJianYi::Buff::FieldTunRiYue::Trigger()
{
    // 遍历每一个气场
    for (auto iter = m_targetSnapshots.begin(); iter != m_targetSnapshots.end();) {
        if (iter->second.interval == 0) [[likely]] { // debuff生效一次
            iter->second.interval = m_intervalFixed;
            SubEffect();
        }
        if (iter->second.duration == 0) [[unlikely]] { // 气场消失
            iter = m_targetSnapshots.erase(iter);

            // 删除player记录的气场信息
            static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id);
        } else [[likely]] {
            ++iter;
        }
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldTunRiYue::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    // 最多三个气场
    stackNum = std::min(stackNum, m_stackNumFixed);
    // 计算要清除的气场数量
    int stack = static_cast<TaiXuJianYi::Player *>(m_player)->fields.size() + stackNum - m_stackNumFixed;
    // 清除排序靠前的气场
    for (int i = 0; i < stack; ++i) {
        Id_t id = static_cast<TaiXuJianYi::Player *>(m_player)->fields.front();
        m_player->buffs[id]->Clear();
    }

    int size = m_targetSnapshots.size();
    for (int i = size; i < size + stackNum; ++i) {
        m_targetSnapshots[i].interval = m_intervalFixed;
        if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
            m_targetSnapshots[i].duration = m_durationFixed;
        } else [[unlikely]] {
            m_targetSnapshots[i].duration = duration;
        }
        // player记录气场信息
        static_cast<TaiXuJianYi::Player *>(m_player)->fields.emplace_back(m_id);
    }
    SubEffectAdd(stackNum);
}

void JX3DPS::TaiXuJianYi::Buff::FieldTunRiYue::Clear(Id_t targetId, int stackNum)
{
    // 计算要清除的气场数量
    stackNum = std::min(stackNum, std::min(m_stackNumFixed, static_cast<int>(m_targetSnapshots.size())));
    for (int i = 0; i < stackNum; ++i) {
        auto iter = m_targetSnapshots.begin();
        iter      = m_targetSnapshots.erase(iter);
    }
    // 清除player记录的气场信息
    static_cast<TaiXuJianYi::Player *>(m_player)->RemoveField(m_id, stackNum);
}

void JX3DPS::TaiXuJianYi::Buff::FieldTunRiYue::SubEffect()
{
    // TODO : 没考虑距离
    for (auto &target : *m_targets) {
        m_player->buffs[TUN_RI_YUE]->Add(target.first);
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldTunRiYue::SubEffectAdd(int stackNum)
{
    // TODO : 没考虑距离
    for (auto &target : *m_targets) {
        m_player->buffs[TUN_RI_YUE]->Add(target.first);
    }
}

JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChenQiSheng::FieldSuiXingChenQiSheng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::FIELD_SUI_XING_CHEN_QI_SHENG;
    m_name = "气场·碎星辰·期声";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 24;
    m_intervalFixed = 16 * 3;
    m_stackNumFixed = 3;
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChenQiSheng::Trigger()
{
    // 遍历每一个气场
    for (auto iter = m_targetSnapshots.begin(); iter != m_targetSnapshots.end(); ++iter) {
        if (iter->second.interval == 0) [[likely]] { // 期声生效一次
            iter->second.interval = m_intervalFixed;
        }
    }
    SubEffect();
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChenQiSheng::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    int size = m_targetSnapshots.size();
    for (int i = size; i < stackNum + size; ++i) {
        m_targetSnapshots[i].interval = m_intervalFixed;
        m_targetSnapshots[i].duration = JX3DPS_INVALID_FRAMES_SET;
    }
    SubEffect();
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChenQiSheng::Clear(Id_t targetId, int stackNum)
{
    for (int i = 0; i < stackNum; ++i) {
        auto iter = m_targetSnapshots.begin();
        iter      = m_targetSnapshots.erase(iter);
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldSuiXingChenQiSheng::SubEffect()
{
    m_player->buffs[QI_SHENG]->Add();
}

JX3DPS::TaiXuJianYi::Buff::SuiXingChen::SuiXingChen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::SUI_XING_CHEN;
    m_name = "碎星辰";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_skillCriticalStrikeAddPercent      = 0.05;
    m_skillCriticalStrikePowerAddPercent = 0.1;

    m_durationFixed = 16 * 4;

    if (m_player->talents[Talent::FU_YIN]) {
        m_skillCriticalStrikeAddPercent      = 0.1;
        m_skillCriticalStrikePowerAddPercent = 0.2;
    }
}

void JX3DPS::TaiXuJianYi::Buff::SuiXingChen::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::SuiXingChen::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.empty()) [[unlikely]] { // buff不存在时，添加buff
        SubEffectAdd();
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::SuiXingChen::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::SuiXingChen::SubEffectAdd()
{
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(m_skillCriticalStrikeAddPercent);
    m_player->attr->AddPhysicsCriticalStrikePowerPercentFromCustom(m_skillCriticalStrikePowerAddPercent);
    if (m_player->talents[Talent::GU_CHANG]) {
        m_player->attr->AddShieldIgnorePercentInt(m_targets, 614);
    }
}

void JX3DPS::TaiXuJianYi::Buff::SuiXingChen::SubEffectClear()
{
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(-m_skillCriticalStrikeAddPercent);
    m_player->attr->AddPhysicsCriticalStrikePowerPercentFromCustom(-m_skillCriticalStrikePowerAddPercent);
    if (m_player->talents[Talent::GU_CHANG]) {
        m_player->attr->AddShieldIgnorePercentInt(m_targets, -614);
    }
}

JX3DPS::TaiXuJianYi::Buff::QiSheng::QiSheng(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::QI_SHENG;
    m_name = "期声";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 20;
}

void JX3DPS::TaiXuJianYi::Buff::QiSheng::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::QiSheng::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.empty()) { // buff不存在时，添加buff
        SubEffectAdd();
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::QiSheng::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::QiSheng::SubEffectAdd()
{
    m_player->attr->AddPhysicsAttackBasePercentInt(102);
}

void JX3DPS::TaiXuJianYi::Buff::QiSheng::SubEffectClear()
{
    m_player->attr->AddPhysicsAttackBasePercentInt(-102);
}

JX3DPS::TaiXuJianYi::Buff::FengShi::FengShi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::FENG_SHI;
    m_name = "风逝";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 5;
}

void JX3DPS::TaiXuJianYi::Buff::FengShi::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::FengShi::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.empty()) [[likely]] { // buff不存在时，添加buff
        SubEffectAdd();
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::FengShi::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::FengShi::SubEffectAdd()
{
    static_cast<Skill::WuWoWuJian *>(m_player->skills[JX3DPS::Skill::WU_WO_WU_JIAN].get())->TriggerFengShiAdd();
}

void JX3DPS::TaiXuJianYi::Buff::FengShi::SubEffectClear()
{
    static_cast<Skill::WuWoWuJian *>(m_player->skills[JX3DPS::Skill::WU_WO_WU_JIAN].get())->TriggerFengShiClear();
}

JX3DPS::TaiXuJianYi::Buff::TunRiYue::TunRiYue(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id    = Buff::TUN_RI_YUE;
    m_name  = "吞日月";
    m_tbuff = true;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 20;
}

void JX3DPS::TaiXuJianYi::Buff::TunRiYue::Trigger()
{
    for (auto iter = m_targetSnapshots.begin(); iter != m_targetSnapshots.end();) {
        if (iter->second.duration == 0) {
            iter = m_targetSnapshots.erase(iter);
        } else {
            ++iter;
        }
    }
}

void JX3DPS::TaiXuJianYi::Buff::TunRiYue::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[targetId].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[targetId].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::TunRiYue::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(targetId);
}

JX3DPS::TaiXuJianYi::Buff::JingHuaYing::JingHuaYing(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::JING_HUA_YING;
    m_name = "镜花影";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 8;
}

void JX3DPS::TaiXuJianYi::Buff::JingHuaYing::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::JingHuaYing::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[targetId].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[targetId].duration = duration;
    }
    SubEffectAdd();
}

void JX3DPS::TaiXuJianYi::Buff::JingHuaYing::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(targetId);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::JingHuaYing::SubEffectAdd()
{
    static_cast<TaiXuJianYi::Skill::JingHuaYing *>(m_player->skills[JX3DPS::Skill::JING_HUA_YING].get())
        ->TriggerAddJingHuaYing();
}

void JX3DPS::TaiXuJianYi::Buff::JingHuaYing::SubEffectClear()
{
    static_cast<TaiXuJianYi::Skill::JingHuaYing *>(m_player->skills[JX3DPS::Skill::JING_HUA_YING].get())
        ->TriggerClearJingHuaYing();
}

JX3DPS::TaiXuJianYi::Buff::FieldLieYun::FieldLieYun(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id    = Buff::FIELD_LIE_YUN;
    m_name  = "隐藏·裂云";
    m_tbuff = true;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 24;
    m_intervalFixed = 8;
}

void JX3DPS::TaiXuJianYi::Buff::FieldLieYun::Trigger()
{
    for (auto iter = m_targetSnapshots.begin(); iter != m_targetSnapshots.end();) {
        if (iter->second.interval == 0) [[likely]] {
            iter->second.interval = m_intervalFixed;
            SubEffectAdd(iter->first);
            ++iter;
        }
        if (iter->second.duration == 0) [[unlikely]] {
            iter = m_targetSnapshots.erase(iter);
        } else [[likely]] {
            ++iter;
        }
    }
}

void JX3DPS::TaiXuJianYi::Buff::FieldLieYun::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    m_targetSnapshots[targetId].duration = m_durationFixed;
    SubEffectAdd(targetId);
}

void JX3DPS::TaiXuJianYi::Buff::FieldLieYun::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(targetId);
}

void JX3DPS::TaiXuJianYi::Buff::FieldLieYun::SubEffectAdd(Id_t targetId)
{
    if ((*m_targets)[targetId]->GetDistance() <= 15) {
        m_player->buffs[LIE_YUN]->Add();
    }
}

JX3DPS::TaiXuJianYi::Buff::LieYun::LieYun(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::LIE_YUN;
    m_name = "裂云";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 1;
}

void JX3DPS::TaiXuJianYi::Buff::LieYun::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::LieYun::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.empty()) {
        SubEffectAdd();
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::LieYun::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::LieYun::SubEffectAdd()
{
    m_player->attr->AddPhysicsCriticalStrikePowerPercentFromCustom(0.15);
}

void JX3DPS::TaiXuJianYi::Buff::LieYun::SubEffectClear()
{
    m_player->attr->AddPhysicsCriticalStrikePowerPercentFromCustom(-0.15);
}

JX3DPS::TaiXuJianYi::Buff::ChiYing::ChiYing(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::CHI_YING;
    m_name = "持盈";

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 127, 0.0);

    m_durationFixed = 16 * 15;
    m_stackNumFixed = 3;
}

void JX3DPS::TaiXuJianYi::Buff::ChiYing::Trigger()
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
}

void JX3DPS::TaiXuJianYi::Buff::ChiYing::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    m_targetSnapshots[JX3DPS_PLAYER].stackNum += stackNum;
    m_targetSnapshots[JX3DPS_PLAYER].stackNum =
        std::min(m_targetSnapshots[JX3DPS_PLAYER].stackNum, m_stackNumFixed);
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::ChiYing::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots[JX3DPS_PLAYER].stackNum -= stackNum;
    if (m_targetSnapshots[JX3DPS_PLAYER].stackNum <= 0) [[likely]] {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
}

void JX3DPS::TaiXuJianYi::Buff::ChiYing::TriggerDamage()
{
    RollResult  rollResult = GetPhysicsRollResult();
    GainsDamage damage     = CalcPhysicsDamage(m_player->targetId, rollResult, 0, 0);
    Record(m_player->targetId, rollResult, damage, 0, 0);
    m_targetSnapshots[JX3DPS_PLAYER].stackNum--;
    if (m_targetSnapshots[JX3DPS_PLAYER].stackNum == 0) {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
}

JX3DPS::TaiXuJianYi::Buff::YunZhongJianShengTaiJi::YunZhongJianShengTaiJi(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id    = Buff::YUN_ZHONG_JIAN_SHENG_TAI_JI;
    m_name  = "云中剑·生太极";
    m_range = 6.0;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 70 * 1.1, 0.0);

    m_durationFixed = 16 * 8;
    m_intervalFixed = 16 * 1;
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianShengTaiJi::Trigger()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].interval == 0) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
        SubEffect();
    }
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == 0) [[unlikely]] {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianShengTaiJi::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.empty()) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianShengTaiJi::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianShengTaiJi::SubEffect()
{
    int count = 0;
    for (auto &target : *m_targets) {
        if (target.second->GetDistance() <= m_range) {
            count++;
            if (count == 4) {
                break;
            }
            RollResult  rollResult = GetPhysicsRollResult();
            GainsDamage damage     = CalcPhysicsDamage(target.first, rollResult, 0, 0);
            Record(target.first, rollResult, damage, 0, 0);
        }
    }
}

JX3DPS::TaiXuJianYi::Buff::YunZhongJianSuiXingChen::YunZhongJianSuiXingChen(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id    = Buff::YUN_ZHONG_JIAN_SUI_XING_CHEN;
    m_name  = "云中剑·碎星辰";
    m_range = 6.0;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 70 * 1.1, 0.0);

    m_durationFixed = 16 * 8;
    m_intervalFixed = 16 * 1;
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianSuiXingChen::Trigger()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].interval == 0) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
        SubEffect();
    }
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == 0) [[unlikely]] {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianSuiXingChen::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.empty()) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianSuiXingChen::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianSuiXingChen::SubEffect()
{
    int count = 0;
    for (auto &target : *m_targets) {
        if (target.second->GetDistance() <= m_range) {
            count++;
            if (count == 4) {
                break;
            }
            RollResult  rollResult = GetPhysicsRollResult();
            GainsDamage damage     = CalcPhysicsDamage(target.first, rollResult, 0, 0);
            Record(target.first, rollResult, damage, 0, 0);
        }
    }
}

JX3DPS::TaiXuJianYi::Buff::YunZhongJianTunRiYue::YunZhongJianTunRiYue(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id    = Buff::YUN_ZHONG_JIAN_TUN_RI_YUE;
    m_name  = "云中剑·吞日月";
    m_range = 6.0;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back((40 + 40 + 17) / 2, 0, 70 * 1.1, 0.0);

    m_durationFixed = 16 * 8;
    m_intervalFixed = 16 * 1;
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianTunRiYue::Trigger()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].interval == 0) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
        SubEffect();
    }
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == 0) [[unlikely]] {
        m_targetSnapshots.erase(JX3DPS_PLAYER);
    }
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianTunRiYue::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots.empty()) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianTunRiYue::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
}

void JX3DPS::TaiXuJianYi::Buff::YunZhongJianTunRiYue::SubEffect()
{
    int count = 0;
    for (auto &target : *m_targets) {
        if (target.second->GetDistance() <= m_range) {
            count++;
            if (count == 4) {
                break;
            }
            RollResult  rollResult = GetPhysicsRollResult();
            GainsDamage damage     = CalcPhysicsDamage(target.first, rollResult, 0, 0);
            Record(target.first, rollResult, damage, 0, 0);
        }
    }
}

JX3DPS::TaiXuJianYi::Buff::ClassFeatureRongJin::ClassFeatureRongJin(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::CLASS_FEATURE;
    m_name = "太虚剑意·融金";

    m_targetSnapshots[JX3DPS_PLAYER].duration = JX3DPS_INVALID_FRAMES_SET;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_intervalFixed = 16 * 1;
}

void JX3DPS::TaiXuJianYi::Buff::ClassFeatureRongJin::Trigger()
{
    m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
    SubEffectAdd();
}

void JX3DPS::TaiXuJianYi::Buff::ClassFeatureRongJin::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    m_targetSnapshots[JX3DPS_PLAYER].interval = RandomUniform(0, m_intervalFixed - 1);
    m_targetSnapshots[JX3DPS_PLAYER].duration = JX3DPS_INVALID_FRAMES_SET;
    SubEffectAdd();
}

void JX3DPS::TaiXuJianYi::Buff::ClassFeatureRongJin::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
}

void JX3DPS::TaiXuJianYi::Buff::ClassFeatureRongJin::SubEffectAdd()
{
    m_player->AddQidian(1);
}

JX3DPS::TaiXuJianYi::Buff::WeaponEffectCW1::WeaponEffectCW1(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::WEAPON_EFFECT_CW_1;
    m_name = "武器·橙武特效1";

    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_intervalFixed = 16 * 30;
    m_durationFixed = 16 * 6;
}

void JX3DPS::TaiXuJianYi::Buff::WeaponEffectCW1::Trigger()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == 0) { // buff时间结束
        m_targetSnapshots[JX3DPS_PLAYER].duration = JX3DPS_INVALID_FRAMES_SET;
        SubEffectClear();
    } else { // 内置CD时间结束
        m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;
    }
}

void JX3DPS::TaiXuJianYi::Buff::WeaponEffectCW1::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
    SubEffectAdd();
}

void JX3DPS::TaiXuJianYi::Buff::WeaponEffectCW1::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::WeaponEffectCW1::TriggerAdd()
{
    if (m_targetSnapshots[JX3DPS_PLAYER].interval == JX3DPS_INVALID_FRAMES_SET && RandomUniform(1, 100) <= 3)
    {
        m_targetSnapshots[JX3DPS_PLAYER].interval = m_intervalFixed;
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
        SubEffectAdd();
    }
}

void JX3DPS::TaiXuJianYi::Buff::WeaponEffectCW1::SubEffectAdd()
{
    static_cast<TaiXuJianYi::Skill::BaHuangGuiYuan *>(
        m_player->skills[JX3DPS::Skill::BA_HUANG_GUI_YUAN].get())
        ->ClearCooldown();
}

void JX3DPS::TaiXuJianYi::Buff::WeaponEffectCW1::SubEffectClear()
{
    static_cast<TaiXuJianYi::Skill::BaHuangGuiYuan *>(
        m_player->skills[JX3DPS::Skill::BA_HUANG_GUI_YUAN].get())
        ->ResetCooldown();
}

JX3DPS::TaiXuJianYi::Buff::ClassSetBuffJianMing::ClassSetBuffJianMing(JX3DPS::Player *player, Targets *targets) :
    JX3DPS::Buff(player, targets)
{
    m_id   = Buff::CLASS_SET_BUFF;
    m_name = "套装·剑鸣";

    m_targetSnapshots[JX3DPS_PLAYER].interval = JX3DPS_INVALID_FRAMES_SET;

    m_subNames.push_back("");

    m_levelNames.push_back("");

    m_damageParams[0].emplace_back(0, 0, 0, 0.0);

    m_durationFixed = 16 * 6;
}

void JX3DPS::TaiXuJianYi::Buff::ClassSetBuffJianMing::Trigger()
{
    // buff结束，不存在自判定，设置为无效帧避免频繁判定
    m_targetSnapshots[JX3DPS_PLAYER].duration = JX3DPS_INVALID_FRAMES_SET;
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::ClassSetBuffJianMing::Add(Id_t targetId, int stackNum, Frame_t duration)
{
    if (m_targetSnapshots[JX3DPS_PLAYER].duration == JX3DPS_INVALID_FRAMES_SET) {
        SubEffectAdd();
    }
    if (duration == JX3DPS_DEFAULT_DURATION_FRAMES) [[likely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    } else [[unlikely]] {
        m_targetSnapshots[JX3DPS_PLAYER].duration = duration;
    }
}

void JX3DPS::TaiXuJianYi::Buff::ClassSetBuffJianMing::Clear(Id_t targetId, int stackNum)
{
    m_targetSnapshots.erase(JX3DPS_PLAYER);
    SubEffectClear();
}

void JX3DPS::TaiXuJianYi::Buff::ClassSetBuffJianMing::TriggerAdd()
{
    if (RandomUniform(1, 100) <= 10) {
        if (m_targetSnapshots[JX3DPS_PLAYER].duration == JX3DPS_INVALID_FRAMES_SET) {
            SubEffectAdd();
        }
        m_targetSnapshots[JX3DPS_PLAYER].duration = m_durationFixed;
    }
}

void JX3DPS::TaiXuJianYi::Buff::ClassSetBuffJianMing::SubEffectAdd()
{
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(0.04);
    m_player->attr->AddMagicCriticalStrikePowerPercentFromCustom(0.04);
}

void JX3DPS::TaiXuJianYi::Buff::ClassSetBuffJianMing::SubEffectClear()
{
    m_player->attr->AddPhysicsCriticalStrikePercentFromCustom(-0.04);
    m_player->attr->AddMagicCriticalStrikePowerPercentFromCustom(-0.04);
}
