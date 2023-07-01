/**
 * Project: JX3DPS
 * File: JX3DPS.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-01 02:44:34
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

void SummarizeStats(Player &player, DamageStats &damageStats)
{
    for (auto &skill : player.skills) {
        damageStats += skill.second->GetDamageStats();
    }
    for (auto &buff : player.buffs) {
        damageStats += buff.second->GetDamageStats();
    }
}

DamageStats Simulate(Player &p, ExprEvents &exprEvents, ExprSkills &exprSkills)
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

    DamageStats damageStats;
    SummarizeStats(*player, damageStats);

    delete player;
    delete targets;

    return damageStats;
}

Error_t SimulatePool(ExprEvents  &exprEvents,
                     ExprSkills  &exprSkills,
                     Player      &player,
                     int          simIterations,
                     DamageStats &damageStats,
                     void        *obj,
                     void (*memberFunction)(void *, double))
{
    std::list<std::future<DamageStats>> results;
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
        damageStats += results.front().get();
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

    JX3DPS::DamageStats damageStats;

    err = SimulatePool(exprEvents, exprSkills, *player.get(), simIterations, damageStats, obj, memberFunction);

    if (err != JX3DPS::JX3DPS_SUCCESS) {
        return err;
    }

    long long sum = 0;
    for (auto &damageStat : damageStats) {
        for (auto &targetStats : damageStat.second) {
            int num = 0;
            for (auto &effectStats : targetStats.second) {
                for (auto &subStats : effectStats.second) {
                    for (auto &levelStats : subStats.second) {

                        num += levelStats.second.first;

                        sum += levelStats.second.second.SumDamage();
                    }
                }
            }
            std::string  name;
            JX3DPS::Id_t id = targetStats.first;
            if ((id / 100) % 10) {
                name = JX3DPS::Buff::BUFF_NAME.at(id);
            } else {
                name = JX3DPS::Skill::SKILL_NAME.at(id);
            }
            spdlog::debug("target: {}\t skill: {}\t num: {}", damageStat.first, name, num);
        }
    }
    sum /= 60;
    sum /= 15;
    sum /= simIterations;

    spdlog::debug("avg : {}", sum);
    nlohmann::json jsonObj;
    jsonObj["avg"] = sum;
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
