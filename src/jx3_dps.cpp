/**
 * Project: JX3DPS
 * File: jx3_dps.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2025-10-06
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include "jx3dps.h"

#include <cstdarg>
#include <cstring>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <ThreadPool/ThreadPool.h>

#include "attribute/attribute.hpp"
#include "class/tai_xu_jian_yi/tai_xu_jian_yi.h"
#include "global/defs.h"
#include "utils/utils.hpp"

#include "buff.h"
#include "json_parse.h"
#include "key_frame.h"
#include "regex.h"
#include "skill.h"
#include "target.hpp"
#include "time_line.hpp"

namespace JX3DPS {

// 全局状态
static std::string g_result;
static double      g_progress = 0.0;

// 全局调试器状态
struct DebuggerState
{
    Player                      *player          = nullptr;
    Targets                     *targets         = nullptr;
    ExprSkillsHash               exprSkillsHash;
    ExprEvents                   exprEvents;
    KeyFrame::KeyFrameSequence   keyFrameSequence;
    Options                      options;

    // CastSkills 需要的状态
    Id_t                         exprSkillsId    = EXPRESSION_SKILL_PLACE_HOLDERS_1;
    Id_t                         lastExprSkillsId = EXPRESSION_SKILL_PLACE_HOLDERS_1;
    ExprSkills                   exprSkills;

    Frame_t                      currentFrame    = 0;
    bool                         finished        = false;
    std::string                  lastSkill;
    std::string                  currentMacro;
    std::string                  debugInfo;

    ~DebuggerState()
    {
        if (player) {
            delete player;
            player = nullptr;
        }
        if (targets) {
            for (auto &[id, target] : *targets) {
                delete target;
            }
            delete targets;
            targets = nullptr;
        }
    }
};

static std::unique_ptr<DebuggerState> g_debugger;

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
            if (i % step == 0) {
                double prog = i * 1.0 / (count * options.simIterations);
                g_progress  = prog;
                if (obj != nullptr && progress != nullptr) {
                    progress(obj, prog, "模拟中...");
                }
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
            if (i % step == 0) {
                double prog = (i + index * options.simIterations) * 1.0 / (count * options.simIterations);
                g_progress  = prog;
                if (obj != nullptr && progress != nullptr) {
                    progress(obj, prog, progressText.c_str());
                }
            }
        }
        index++;
        player.attribute.SetGainSwitch(type, false);

        stats.gainStats[type] = temp.gainStats[Attribute::Type::DEFAULT];
    }

    g_progress = 1.0;
    if (obj != nullptr && progress != nullptr) {
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

// C API 实现

const char *jx3dps_version()
{
    return JX3DPS::VERSION;
}

const char *jx3dps_jx3_version()
{
    return JX3DPS::JX3_VERSION_STR;
}

int jx3dps_simulate(const char *in, ...)
{
    spdlog::info("{}  Version: {}", JX3DPS::NAME, JX3DPS::VERSION);
    spdlog::info("Start simulation.");
    auto start = std::chrono::steady_clock::now();

    spdlog::debug("Input: {}", in);

    // 解析可变参数
    va_list args;
    va_start(args, in);
    char *out                                                   = va_arg(args, char *);
    void *obj                                                   = va_arg(args, void *);
    void (*progress)(void *, double, const char *) = va_arg(args, void (*)(void *, double, const char *));
    va_end(args);

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
        return err;
    }

    JX3DPS::g_result = jsonOut.dump();
    if (out != nullptr) {
        strcpy(out, JX3DPS::g_result.c_str());
    }

    spdlog::debug("Output: {}", JX3DPS::g_result);

    auto                              end     = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    spdlog::info("Simulation finished.");
    spdlog::info("Total time: {:.8f} s", elapsed.count());

    return JX3DPS::JX3DPS_SUCCESS;
}

const char *jx3dps_get_result()
{
    return JX3DPS::g_result.c_str();
}

double jx3dps_get_progress()
{
    return JX3DPS::g_progress;
}

int jx3dps_debug(const char *in, ...)
{
    spdlog::info("Start debug.");
    spdlog::debug("Input: {}", in);

    // 解析可变参数
    va_list args;
    va_start(args, in);
    char *out                                                   = va_arg(args, char *);
    void *obj                                                   = va_arg(args, void *);
    void (*progress)(void *, double, const char *) = va_arg(args, void (*)(void *, double, const char *));
    va_end(args);

    // 清理旧的调试器状态
    JX3DPS::g_debugger = std::make_unique<JX3DPS::DebuggerState>();

    // 解析 JSON 输入
    nlohmann::ordered_json jsonIn;
    try {
        jsonIn = nlohmann::ordered_json::parse(in);
    } catch (nlohmann::ordered_json::parse_error &e) {
        spdlog::error("parse json error: {}", e.what());
        return JX3DPS::JX3DPS_ERROR_INVALID_JSON;
    }

    // 初始化参数
    JX3DPS::Error_t err = JX3DPS::InitParams(jsonIn,
                                             JX3DPS::g_debugger->exprSkillsHash,
                                             JX3DPS::g_debugger->exprEvents,
                                             &JX3DPS::g_debugger->player,
                                             JX3DPS::g_debugger->options);
    if (err != JX3DPS::JX3DPS_SUCCESS) {
        return err;
    }

    // 创建 Targets
    JX3DPS::g_debugger->targets = new JX3DPS::Targets();
    JX3DPS::g_debugger->player->SetTargets(JX3DPS::g_debugger->targets);

    // 生成 KeyFrameSequence
    JX3DPS::KeyFrame::GenerateKeyFrameSequence(JX3DPS::g_debugger->keyFrameSequence,
                                               JX3DPS::g_debugger->player,
                                               JX3DPS::g_debugger->exprEvents,
                                               JX3DPS::g_debugger->exprSkillsHash);

    // 初始化 exprSkills
    if (JX3DPS::g_debugger->exprSkillsHash.find(JX3DPS::g_debugger->exprSkillsId) !=
        JX3DPS::g_debugger->exprSkillsHash.end()) {
        JX3DPS::g_debugger->exprSkills = JX3DPS::g_debugger->exprSkillsHash.at(JX3DPS::g_debugger->exprSkillsId);
    }

    spdlog::info("Debug session initialized.");
    return JX3DPS::JX3DPS_SUCCESS;
}

const char *jx3dps_debugger_step_in()
{
    // step_in 和 step_over 在宏调试中通常相同
    return jx3dps_debugger_step_over();
}

const char *jx3dps_debugger_step_over()
{
    if (!JX3DPS::g_debugger || !JX3DPS::g_debugger->player) {
        static std::string error = R"({"error":"debugger_not_initialized"})";
        return error.c_str();
    }

    if (JX3DPS::g_debugger->keyFrameSequence.empty()) {
        JX3DPS::g_debugger->finished = true;
        static std::string finished = R"({"status":"finished"})";
        return finished.c_str();
    }

    // 1. 获取下一帧并更新时间
    JX3DPS::Frame_t next = JX3DPS::g_debugger->keyFrameSequence.front().first;
    JX3DPS::g_debugger->currentFrame += next;

    // 2. 更新关键帧序列（所有技能/buff的冷却等）
    JX3DPS::KeyFrame::UpdateKeyFrameSequence(JX3DPS::g_debugger->keyFrameSequence,
                                             JX3DPS::g_debugger->player,
                                             next);

    // 3. 执行当前帧的所有事件
    for (auto &[type, id] : JX3DPS::g_debugger->keyFrameSequence.front().second) {
        if (type == JX3DPS::KeyFrame::KeyFrameType::SKILL) {
            JX3DPS::g_debugger->player->skills[id]->Trigger();
            JX3DPS::g_debugger->lastSkill = "技能ID:" + std::to_string(static_cast<int>(id));
        } else if (type == JX3DPS::KeyFrame::KeyFrameType::BUFF) {
            JX3DPS::g_debugger->player->buffs[id]->Trigger();
        } else if (type == JX3DPS::KeyFrame::KeyFrameType::EVENT) {
            // 事件处理
        }
    }

    // 4. 移除已处理的帧
    JX3DPS::g_debugger->keyFrameSequence.pop_front();

    // 5. 调用 CastSkills 执行宏逻辑
    if (!JX3DPS::g_debugger->player->IsStop()) {
        try {
            JX3DPS::Id_t skillId = JX3DPS::KeyFrame::CastSkills(
                JX3DPS::g_debugger->player,
                JX3DPS::g_debugger->targets,
                JX3DPS::g_debugger->exprSkillsHash,
                JX3DPS::g_debugger->exprSkills,
                JX3DPS::g_debugger->currentFrame,
                JX3DPS::g_debugger->exprSkillsId,
                JX3DPS::g_debugger->lastExprSkillsId,
                0);
        } catch (const std::exception &e) {
            spdlog::error("CastSkills exception: {}", e.what());
        }
    }

    // 6. 生成调试信息 JSON
    nlohmann::ordered_json debugJson;
    debugJson["status"]         = "running";
    debugJson["currentFrame"]   = JX3DPS::g_debugger->currentFrame;
    debugJson["currentSeconds"] = JX3DPS::g_debugger->currentFrame / 16.0;
    debugJson["lastSkill"]      = JX3DPS::g_debugger->lastSkill;
    debugJson["lifePercent"]    = JX3DPS::g_debugger->player->GetLifePercent();
    debugJson["manaPercent"]    = JX3DPS::g_debugger->player->GetManaPercent();
    debugJson["qidian"]         = JX3DPS::g_debugger->player->GetQidian();

    JX3DPS::g_debugger->debugInfo = debugJson.dump();
    return JX3DPS::g_debugger->debugInfo.c_str();
}

const char *jx3dps_debugger_continue()
{
    if (!JX3DPS::g_debugger || !JX3DPS::g_debugger->player) {
        static std::string error = R"({"error":"debugger_not_initialized"})";
        return error.c_str();
    }

    // 持续执行直到结束（未来可以添加断点支持）
    while (!JX3DPS::g_debugger->keyFrameSequence.empty()) {
        jx3dps_debugger_step_over();
    }

    // 返回最终状态
    nlohmann::ordered_json debugJson;
    debugJson["status"]         = "finished";
    debugJson["currentFrame"]   = JX3DPS::g_debugger->currentFrame;
    debugJson["currentSeconds"] = JX3DPS::g_debugger->currentFrame / 16.0;

    JX3DPS::g_debugger->debugInfo = debugJson.dump();
    return JX3DPS::g_debugger->debugInfo.c_str();
}

void jx3dps_debugger_set_breakpoint(const char *in)
{
    // 解析断点 JSON (未来实现)
    // 格式: {"line": 5} 或 {"frame": 100}
    spdlog::debug("Set breakpoint: {}", in);

    // TODO: 将断点信息保存到 g_debugger 中
    // 在 step_over 或 continue 中检查是否到达断点
}
