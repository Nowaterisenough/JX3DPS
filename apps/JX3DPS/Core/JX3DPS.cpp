/**
 * Project: JX3DPS
 * File: JX3DPS.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-08 06:17:54
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "JX3DPS.h"

#include <spdlog/spdlog.h>

#include <ThreadPool/ThreadPool.h>
#include <Utils/Utils.h>

#include "KeyFrame.h"
#include "TaiXuJianYi.h"

namespace JX3DPS {

void SummarizeStats(Player &player, Stats &stats)
{
    for (auto &skill : player.skills) {
        stats += skill.second->GetStats();
    }
    for (auto &buff : player.buffs) {
        stats += buff.second->GetStats();
    }
}

void EvaluateGains(Player &player, const Stats &stats, std::vector<float> &gains, int time, int simCount)
{
    gains.resize(17);
    long long attackBaseDamage = 0;
    long long weaponDamage     = 0;

    for (auto &targetStats : stats.damageStats) {       // TargetStats
        for (auto &effectStats : targetStats.second) {  // EffectStats
            for (auto &subStats : effectStats.second) { // SubStats
                for (auto &levelStats : subStats.second) {
                    for (auto &rollStats : levelStats.second) {
                        weaponDamage     += rollStats.second.second.weaponDamage;
                        attackBaseDamage += rollStats.second.second.attackBaseDamage;
                    }
                }
            }
        }
    }
    gains[static_cast<int>(AttributeType::NONE)] = stats.normalDamage / time / simCount;
    gains[static_cast<int>(AttributeType::PHYSICS_ATTACK)] =
        (float)attackBaseDamage * ATTRIBUTE_GAIN_BY_BASE.at(static_cast<int>(AttributeType::PHYSICS_ATTACK)) /
        player.attr->GetPhysicsAttackFromBase() / stats.normalDamage;
    gains[static_cast<int>(AttributeType::WEAPON_ATTACK)] =
        (float)weaponDamage * ATTRIBUTE_GAIN_BY_BASE.at(static_cast<int>(AttributeType::WEAPON_ATTACK)) /
        player.attr->GetWeaponAttack() / stats.normalDamage;
    gains[static_cast<int>(AttributeType::PHYSICS_CRITICAL_STRIKE_POWER)] =
        stats.criticalStrikePowerDamage * 1.0 / stats.normalDamage - 1.0;
    gains[static_cast<int>(AttributeType::PHYSICS_OVERCOME)] =
        stats.overcomeDamage * 1.0 / stats.normalDamage - 1.0;
    gains[static_cast<int>(AttributeType::STRAIN)] = stats.strainDamage * 1.0 / stats.normalDamage - 1.0;
    gains[static_cast<int>(AttributeType::SURPLUS)] = stats.surplusDamage * 1.0 / stats.normalDamage - 1.0;
}

Stats Simulate(Player &p, ExprEvents &exprEvents, ExprSkills &exprSkills)
{

    Targets *targets = new Targets;
    Player  *player  = p.Clone();
    player->SetTargets(targets);

    ExprEvents events;
    for (auto exprEvent : exprEvents) {
        events.push_back(exprEvent);
    }
    ExprSkills skills;
    for (auto exprSkill : exprSkills) {
        skills.push_back(exprSkill);
    }
    KeyFrame::KeyFrameSequence keyFrameSequence;
    KeyFrame::GenerateKeyFrameSequence(keyFrameSequence, player, events);
    KeyFrame::KeyFrameAdvance(keyFrameSequence, player, targets, events, skills);

    Stats stats;
    SummarizeStats(*player, stats);

    delete player;
    delete targets;

    return stats;
}

Error_t SimulatePool(ExprEvents &exprEvents,
                     ExprSkills &exprSkills,
                     Player     &player,
                     int         simIterations,
                     Stats      &stats,
                     void       *obj,
                     void (*memberFunction)(void *, double))
{
    std::list<std::future<Stats>> results;
    for (int i = 0; i < simIterations; i++) {
        auto res =
            ThreadPool::Instance()->Enqueue(Simulate, std::ref(player), std::ref(exprEvents), std::ref(exprSkills));
        results.emplace_back(std::move(res));
    }

    int step = simIterations / 100;
    if (step == 0) {
        step = 1;
    }
    for (int i = 0; i < simIterations; i++) {
        if (i % step == 0) {
            memberFunction(obj, (double)i / simIterations);
        }
        stats += results.front().get();
        results.pop_front();
    }
    memberFunction(obj, 1.0);

    return JX3DPS::JX3DPS_SUCCESS;
}

Error_t ParseExpr(const std::list<std::string> &events, const std::list<std::string> &skills, ExprEvents &exprEvents, ExprSkills &exprSkills)
{
    ExprEvent exprEvent;

    ExprSkills exprEventSkill;
    for (auto &event : events) {
        ExprSkill exprSkill;
        Error_t   err = ParseMacro(event, exprEvent, exprSkill);
        if (err == JX3DPS_SUCCESS_EVENT) {
            exprEvents.push_back(exprEvent);
        } else if (err == JX3DPS_SUCCESS_EVENT_SKILL) {
            exprEvents.push_back(exprEvent);
            exprEventSkill.push_back(exprSkill);
        } else {
            return err;
        }
    }
    for (auto &skill : skills) {
        ExprSkill exprSkill;
        Error_t   err = ParseMacro(skill, exprEvent, exprSkill);
        if (err != JX3DPS_SUCCESS_SKILL) {
            return err;
        }
        exprSkills.push_back(exprSkill);
    }
    exprSkills.splice(exprSkills.end(),
                      exprEventSkill); // 需要将事件里的技能插入到技能序列中，以保持随时更新帧数，但实际上并不执行
    return JX3DPS::JX3DPS_SUCCESS;
}

Error_t InitPlayer(const nlohmann::json &json, std::shared_ptr<Player> &player)
{
    Class classType = Class::TAI_XU_JIAN_YI;
    player          = Player::PlayerFactoryGenerate(classType);

    ParseJson2Talents(json, player->talents);
    ParseJson2Secrets(json, player->secrets);

    ParseJson2Attr(json, *(player->attr.get()));

    player->enchantWrist  = json["set_effects"]["EnchantWrist"];
    player->enchantShoes  = json["set_effects"]["EnchantShoes"];
    player->enchantBelt   = json["set_effects"]["EnchantBelt"];
    player->enchantJacket = json["set_effects"]["EnchantJacket"];
    player->enchantHat    = json["set_effects"]["EnchantHat"];
    player->weaponCW      = json["set_effects"]["WeaponCW"];
    player->classSetBuff  = json["set_effects"]["ClassSetBuff"];
    player->classSetSkill = json["set_effects"]["ClassSetSkill"];

    player->delayMin = json["delay_min"].get<int>();
    player->delayMax = json["delay_max"].get<int>();

    player->Init();

    return JX3DPS::JX3DPS_SUCCESS;
}

Error_t InitParams(const nlohmann::json    &json,
                   ExprSkills              &exprSkills,
                   ExprEvents              &exprEvents,
                   std::shared_ptr<Player> &player,
                   int                     &simIterations)
{
    std::list<std::string> skills;
    std::list<std::string> events;

    Error_t err = ParseJson2Skills(json, skills);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = ParseJson2Events(json, events);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = ParseExpr(events, skills, exprEvents, exprSkills);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = ParseJson2SimIterations(json, simIterations);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    return InitPlayer(json, player);
}

} // namespace JX3DPS

int JX3DPSSimulate(const char *const in, char *out, void *obj, void (*memberFunction)(void *, double))
{

    spdlog::info("{}  Version: {}", JX3DPS::NAME, JX3DPS::VERSION);

    auto start = std::chrono::steady_clock::now();

    std::string    str(in);
    nlohmann::json json;
    try {
        json = nlohmann::json::parse(str);
    } catch (nlohmann::json::parse_error &e) {
        spdlog::error("parse json error: {}", e.what());
        return JX3DPS::JX3DPS_ERROR_INVALID_JSON;
    }

    JX3DPS::ExprSkills exprSkills;
    JX3DPS::ExprEvents exprEvents;
    int                simIterations = 0;

    std::shared_ptr<JX3DPS::Player> player = nullptr;

    JX3DPS::Error_t err = InitParams(json, exprSkills, exprEvents, player, simIterations);
    if (err != JX3DPS::JX3DPS_SUCCESS) {
        return err;
    }

    JX3DPS::Stats stats;

    err = SimulatePool(exprEvents, exprSkills, *player.get(), simIterations, stats, obj, memberFunction);

    if (err != JX3DPS::JX3DPS_SUCCESS) {
        return err;
    }

    int time = exprEvents.back().first / 16;

    nlohmann::json jsonObj;

    std::vector<float> gains;
    JX3DPS::EvaluateGains(*player.get(), stats, gains, time, simIterations);
    JX3DPS::Gains2Json(gains, jsonObj["gains"]);

    JX3DPS::Stats2Json(stats.damageStats, jsonObj["stats"]);
    jsonObj["sim_iterations"] = simIterations;
    strcpy(out, jsonObj.dump().data());

    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;

    spdlog::info("time: {:.8f} s", elapsed_seconds.count());

    return JX3DPS::JX3DPS_SUCCESS;
}

const char *const JX3DPSVersion()
{
    return JX3DPS::VERSION;
}
