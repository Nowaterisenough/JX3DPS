/**
 * Project: JX3DPS
 * File: Player.cpp
 * Description:
 * Created Date: 2023-07-20 02:39:38
 * Author: 难为水
 * -----
 * Last Modified: 2023-10-16 22:25:08
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "Player.h"

#include <Buff3rd.h>

#include "Class/MoWen/MoWen.h"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"

#include "Buff.h"
#include "Buff3rd.h"
#include "Skill.h"

namespace JX3DPS {

JX3DPS::Player::Player() { }

Player *Player::PlayerFactoryGenerate(ClassType classType)
{
    switch (classType) {
        case ClassType::TAI_XU_JIAN_YI: return new TaiXuJianYi::Player();
        case ClassType::ZI_XIA_GONG: return nullptr;
        case ClassType::FEN_SHAN_JING: return nullptr;
        case ClassType::MO_WEN: return new MoWen::Player();
        default: return nullptr;
    }
    return nullptr;
}

Player::~Player()
{
    for (auto &[id, skill] : this->skills) {
        delete skill;
    }

    for (auto &[id, buff] : this->buffs) {
        delete buff;
    }
}

JX3DPS::Player::Player(const Player &other)
{
    m_qidian      = other.m_qidian;
    m_qidianLimit = other.m_qidianLimit;
    m_rage        = other.m_rage;
    m_rageLimit   = other.m_rageLimit;
    m_energy      = other.m_energy;
    m_energyLimit = other.m_energyLimit;
    m_sun         = other.m_sun;
    m_sunLimit    = other.m_sunLimit;
    m_moon        = other.m_moon;
    m_moonLimit   = other.m_moonLimit;

    m_lifePercent = other.m_lifePercent;
    m_manaPercent = other.m_manaPercent;

    m_cast   = other.m_cast;
    m_reCast = other.m_reCast;
    m_stop   = other.m_stop;

    m_delayMax = other.m_delayMax;
    m_delayMin = other.m_delayMin;

    m_targetId      = other.m_targetId;
    m_lastCastSkill = other.m_lastCastSkill;

    globalCooldown        = other.globalCooldown;
    globalCooldownCurrent = other.globalCooldownCurrent;

    attribute = other.attribute;

    for (auto &[id, buff] : other.buffs) {
        Buff *b = buff->Clone();
        b->SetPlayer(this);
        buffs.emplace(id, b);
    }
    for (auto &[id, skill] : other.skills) {
        Skill *s = skill->Clone();
        s->SetPlayer(this);
        skills.emplace(id, s);
    }
    for (auto &[id, talent] : other.talents) {
        talents.emplace(id, talent);
    }
    for (auto &[id, secret] : other.recipes) {
        recipes.emplace(id, secret);
    }
    for (auto &[id, equipEffect] : other.equipEffects) {
        recipes.emplace(id, equipEffect);
    }
}

JX3DPS::Player &JX3DPS::Player::operator=(const Player &other)
{
    if (this == &other) {
        return *this;
    }

    m_qidian      = other.m_qidian;
    m_qidianLimit = other.m_qidianLimit;
    m_rage        = other.m_rage;
    m_rageLimit   = other.m_rageLimit;
    m_energy      = other.m_energy;
    m_energyLimit = other.m_energyLimit;
    m_sun         = other.m_sun;
    m_sunLimit    = other.m_sunLimit;
    m_moon        = other.m_moon;
    m_moonLimit   = other.m_moonLimit;

    m_lifePercent = other.m_lifePercent;
    m_manaPercent = other.m_manaPercent;

    m_cast   = other.m_cast;
    m_reCast = other.m_reCast;
    m_stop   = other.m_stop;

    m_delayMax = other.m_delayMax;
    m_delayMin = other.m_delayMin;

    m_targetId      = other.m_targetId;
    m_lastCastSkill = other.m_lastCastSkill;

    globalCooldown        = other.globalCooldown;
    globalCooldownCurrent = other.globalCooldownCurrent;

    attribute = other.attribute;

    for (auto &[id, buff] : other.buffs) {
        Buff *b = buff->Clone();
        b->SetPlayer(this);
        buffs.emplace(id, b);
    }
    for (auto &[id, skill] : other.skills) {
        Skill *s = skill->Clone();
        s->SetPlayer(this);
        skills.emplace(id, s);
    }
    for (auto &[id, talent] : other.talents) {
        talents.emplace(id, talent);
    }
    for (auto &[id, secret] : other.recipes) {
        recipes.emplace(id, secret);
    }
    for (auto &[id, equipEffect] : other.equipEffects) {
        recipes.emplace(id, equipEffect);
    }

    return *this;
}

void JX3DPS::Player::SetTargets(Targets *targets)
{
    m_targets = targets;
    for (auto &[id, skill] : skills) {
        skill->SetTargets(targets);
    }
    for (auto &[id, buff] : buffs) {
        buff->SetTargets(targets);
    }
}

void Player::AddBuff3rds(const std::unordered_set<Id_t> &buff3rds)
{
    if (buff3rds.find(BUFF_3RD_HAN_RU_LEI) != buff3rds.end()) {
        buffs[BUFF_3RD_HAN_RU_LEI] = new Buff3rd::HanRuLei(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_XIU_QI) != buff3rds.end()) {
        buffs[BUFF_3RD_XIU_QI] = new Buff3rd::XiuQi(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_PO_FENG) != buff3rds.end()) {
        buffs[BUFF_3RD_PO_FENG] = new Buff3rd::PoFeng(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_JING_FENG) != buff3rds.end()) {
        buffs[BUFF_3RD_JING_FENG] = new Buff3rd::JingFeng(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_JI_LEI) != buff3rds.end()) {
        buffs[BUFF_3RD_JI_LEI] = new Buff3rd::JiLei(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_JIE_HUO) != buff3rds.end()) {
        buffs[BUFF_3RD_JIE_HUO] = new Buff3rd::JieHuo(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_HAN_CHANG_LIN_LI) != buff3rds.end()) {
        buffs[BUFF_3RD_HAN_CHANG_LIN_LI] = new Buff3rd::HanChangLinLi(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_SHU_KUANG) != buff3rds.end()) {
        buffs[BUFF_3RD_SHU_KUANG] = new Buff3rd::ShuKuang(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_HAN_XIAO_QIAN_JUN) != buff3rds.end()) {
        buffs[BUFF_3RD_HAN_XIAO_QIAN_JUN] = new Buff3rd::HanXiaoQianJun(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_ZHEN_FEN) != buff3rds.end()) {
        buffs[BUFF_3RD_ZHEN_FEN] = new Buff3rd::ZhenFen(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_SHENG_YU_MING_XIN) != buff3rds.end()) {
        buffs[BUFF_3RD_SHENG_YU_MING_XIN] = new Buff3rd::ShengYuMingXin(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_CHAO_SHENG) != buff3rds.end()) {
        buffs[BUFF_3RD_CHAO_SHENG] = new Buff3rd::ChaoSheng(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_SHE_SHEN_HONG_FA) != buff3rds.end()) {
        buffs[BUFF_3RD_SHE_SHEN_HONG_FA] = new Buff3rd::SheShenHongFa(this, nullptr);
    }
    if (buff3rds.find(BUFF_3RD_HAO_LING_SAN_JUN) != buff3rds.end()) {
        buffs[BUFF_3RD_HAO_LING_SAN_JUN] = new Buff3rd::HaoLingSanJun(this, nullptr);
    }
    // if (buff3rds.find(BUFF_3RD_FIELD_SUI_XING_CHEN) != buff3rds.end()) {
    //     buffs[BUFF_3RD_FIELD_SUI_XING_CHEN] = new Buff3rd::FieldSuiXingChen(this, nullptr);
    // }
    // if (buff3rds.find(BUFF_3RD_FIELD_PO_CANG_QIONG) != buff3rds.end()) {
    //     buffs[BUFF_3RD_FIELD_PO_CANG_QIONG] = new Buff3rd::FieldPoCangQiong(this, nullptr);
    // }
    // if (buff3rds.find(BUFF_3RD_SUI_XING_CHEN) != buff3rds.end()) {
    //     buffs[BUFF_3RD_SUI_XING_CHEN] = new Buff3rd::SuiXingChen(this, nullptr);
    // }
    // if (buff3rds.find(BUFF_3RD_PO_CANG_QIONG) != buff3rds.end()) {
    //     buffs[BUFF_3RD_PO_CANG_QIONG] = new Buff3rd::PoCangQiong(this, nullptr);
    // }
}

bool Player::StopReCastSkill()
{
        if (m_reCast) {
            m_reCast = false;
            skills[m_reCastSkill]->Stop();
        }
        return true;
    }

void Player::TriggerWeaponWater(const Params &params)
{
    static_cast<Buff3rd::WeaponEffectWater *>(params.player->buffs[BUFF_WEAPON_EFFECT_WATER])
        ->TriggerAdd();
}

} // namespace JX3DPS