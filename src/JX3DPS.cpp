﻿/**
 * Project: JX3DPS
 * File: JX3DPS.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-13 04:35:53
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "JX3DPS.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <ThreadPool/ThreadPool.h>

#include "Attribute/Attribute.hpp"
#include "Class/TaiXuJianYi/TaiXuJianYi.h"
#include "Global/Defs.h"
#include "Utils/Utils.hpp"

#include "Buff.h"
#include "JsonParse.h"
#include "KeyFrame.h"
#include "Regex.h"
#include "Skill.h"
#include "Target.hpp"
#include "TimeLine.hpp"

namespace JX3DPS {

void SummarizeStats(Player &player, Stats &stats)
{
    for (auto &[id, skill] : player.skills) {
        stats += skill->GetStats();
    }
    for (auto &[id, buff] : player.buffs) {
        stats += buff->GetStats();
    }
}

long long Sum(const Stats &stats)
{
    long long sum = 0;
    if (stats.gainStats.size() == 0) {
        return 0;
    }
    for (auto &[targetId, targetDamage] : stats.gainStats.at(Attribute::Type::DEFAULT)) {
        for (auto &[effectId, effectDamage] : targetDamage) {
            for (auto &[sub, subDamage] : effectDamage) {
                for (auto &[level, levelDamage] : subDamage) {
                    for (auto &[roll, info] : levelDamage) {
                        sum += info.second.SumDamage();
                    }
                }
            }
        }
    }
    return sum;
}

Stats Simulate(Player &p, ExprSkillsHash &exprSkillsHash, ExprEvents &exprEvents, Options &op)
{
    Options  options = op;
    Targets *targets = new Targets;
    Player  *player  = p.Clone();
    player->SetTargets(targets);

    ExprEvents events;
    for (const auto &exprEvent : exprEvents) {
        events.emplace_back(exprEvent);
    }

    ExprSkillsHash skillsHash;
    for (const auto &[id, exprSkills] : exprSkillsHash) {
        ExprSkills skills;
        for (const auto &exprSkill : exprSkills) {
            skills.emplace_back(exprSkill);
        }
        skillsHash.emplace(id, skills);
    }

    KeyFrame::KeyFrameSequence keyFrameSequence;
    KeyFrame::GenerateKeyFrameSequence(keyFrameSequence, player, events, skillsHash);
    KeyFrame::KeyFrameAdvance(keyFrameSequence, player, targets, events, skillsHash, options);

    Stats stats;
    SummarizeStats(*player, stats);

    delete player;
    for (auto &[id, target] : *targets) {
        delete target;
    }
    delete targets;

    return stats;
}

void SimulatePool(ExprSkillsHash &exprSkillsHash,
                  ExprEvents     &exprEvents,
                  Player         &player,
                  Options        &options,
                  Stats          &stats,
                  void           *obj,
                  void (*progress)(void *, double, const char *))
{
    StatsInit(player.GetClassType(), stats);

    int count = 1;
    for (auto &[type, sc] : options.gainSwitch) {
        if (sc) {
            count++;
        }
    }

    int step = options.simIterations * count / 200;
    if (step == 0) {
        step = 1;
    }

    // 默认模拟
    {
        std::list<std::future<Stats>> results;
        for (int i = 0; i < options.simIterations; i++) {
            auto res = ThreadPool::Instance()->Enqueue(Simulate,
                                                       std::ref(player),
                                                       std::ref(exprSkillsHash),
                                                       std::ref(exprEvents),
                                                       std::ref(options));
            results.emplace_back(std::move(res));
        }

        for (int i = 0; i < options.simIterations; i++) {
            Stats t  = results.front().get();
            stats   += t;
            stats.damageList.push_back(Sum(t));
            results.pop_front();
            if (i % step == 0 && obj != nullptr) {
                progress(obj, i * 1.0 / (count * options.simIterations), "模拟中...");
            }
        }
    }

    // 收益模拟
    int index = 1;
    for (auto &[type, sc] : options.gainSwitch) {
        if (!sc) {
            continue;
        }

        std::string progressText =
            std::string(Attribute::ATTRIBUTE_NAME.at(static_cast<int>(type))).append("收益模拟中...");

        Stats temp;
        StatsInit(player.GetClassType(), temp);

        player.attribute.SetGainSwitch(type, true);

        std::list<std::future<Stats>> results;
        for (int i = 0; i < options.simIterations; i++) {
            auto res = ThreadPool::Instance()->Enqueue(Simulate,
                                                       std::ref(player),
                                                       std::ref(exprSkillsHash),
                                                       std::ref(exprEvents),
                                                       std::ref(options));
            results.emplace_back(std::move(res));
        }

        for (int i = 0; i < options.simIterations; i++) {
            Stats t  = results.front().get();
            temp    += t;
            results.pop_front();
            if (i % step == 0 && obj != nullptr) {
                progress(obj,
                         (i + index * options.simIterations) * 1.0 / (count * options.simIterations),
                         progressText.c_str());
            }
        }
        index++;
        player.attribute.SetGainSwitch(type, false);

        stats.gainStats[type] = temp.gainStats[Attribute::Type::DEFAULT];
    }

    if (obj != nullptr) {
        progress(obj, 1.0, "模拟完成");
    }
}

Error_t InitPlayer(const nlohmann::ordered_json &json, Player **player)
{
    ClassType classType;
    ParseJsonToClassType(json, classType);
    *player = Player::PlayerFactoryGenerate(classType);

    Error_t err = ParseJsonToTalents(json, (*player)->talents);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = ParseJsonToRecipes(json, (*player)->recipes);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = ParseJsonToEquipEffects(json, (*player)->equipEffects);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = ParseJsonToAttribute(json, (*player)->attribute);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = ParseJsonToPermanents(json, (*player)->attribute);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = ParseJsonToTeamCore(json, (*player)->attribute, (*player)->teamCore);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    std::unordered_set<Id_t> buff3rds;
    err = ParseJsonToBuff3rds(json, buff3rds);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    (*player)->Init();
    (*player)->AddBuff3rds(buff3rds);

    return JX3DPS_SUCCESS;
}

Error_t InitParams(const nlohmann::ordered_json &json,
                   ExprSkillsHash               &exprSkillsHash,
                   ExprEvents                   &exprEvents,
                   Player                      **player,
                   Options                      &options)
{
    // 解析模拟选项
    Error_t err = ParseJsonToOptions(json, options);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }
    TimeLine::SetMode(options.mode);

    // 解析技能语句
    std::list<std::pair<std::string, std::list<std::string>>> skills;
    err = ParseJsonToSkills(json, skills);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = Regex::ParseToExprSkillsHash(skills, exprSkillsHash);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    // 解析事件语句
    std::list<std::string> events;
    err = ParseJsonToEvents(json, events);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    err = Regex::ParseToExprEvents(events, exprEvents);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    return InitPlayer(json, player);
}

Error_t Start(const nlohmann::ordered_json &in,
              nlohmann::ordered_json       &out,
              void                         *obj,
              void (*progress)(void *, double, const char *))
{
    ExprSkillsHash exprSkillsHash;
    ExprEvents     exprEvents;
    Player        *player = nullptr;
    Options        options;

    Error_t err = InitParams(in, exprSkillsHash, exprEvents, &player, options);
    if (err != JX3DPS_SUCCESS) {
        return err;
    }

    Stats stats;
    SimulatePool(exprSkillsHash, exprEvents, *player, options, stats, obj, progress);
    delete player;

    out["SimIterations"] = options.simIterations;
    out["Frames"]        = options.totalFrames;
    out["ClassType"]     = in["ClassType"].get<std::string>().c_str();

    if (options.mode == Options::Mode::DEBUG) {
        TimeLineToJson(TimeLine::Instance().GetInfosList(), out["TimeLine"]);
    }
    return StatsToJson(stats, out);
}

} // namespace JX3DPS

int JX3DPSSimulate(const char *const in, char *out, void *obj, void (*progress)(void *, double, const char *))
{
    spdlog::info("{}  Version: {}", JX3DPS::NAME, JX3DPS::VERSION);
    spdlog::info("Start simulation.");
    auto start = std::chrono::steady_clock::now();

    spdlog::debug("Input: {}", in);

    nlohmann::ordered_json jsonIn;
    try {
        jsonIn = nlohmann::ordered_json::parse(in);
    } catch (nlohmann::ordered_json::parse_error &e) {
        spdlog::error("parse json error: {}", e.what());
        return JX3DPS::JX3DPS_ERROR_INVALID_JSON;
    }

    nlohmann::ordered_json jsonOut;
    JX3DPS::Error_t        err = JX3DPS::Start(jsonIn, jsonOut, obj, progress);

    if (err != JX3DPS::JX3DPS_SUCCESS) {
        spdlog::error("Simulate error: {}", err);
        return err;
    }

    strcpy(out, jsonOut.dump().data());

    spdlog::debug("Output: {}", out);

    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    spdlog::info("Simulation finished.");
    spdlog::info("Total time: {:.8f} s", elapsed.count());

    return JX3DPS::JX3DPS_SUCCESS;
}

const char *const JX3DPSVersion()
{
    return JX3DPS::VERSION;
}
