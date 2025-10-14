/**
 * @file context.cpp
 * @brief SimContext的线程局部存储定义
 */

#include "context.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "src/macro/macro_interpreter.h"
#include "src/class/tai_xu_jian_yi/tai_xu_jian_yi.h"

namespace JX3DPS {

// 定义线程局部上下文
thread_local Context context;

/**
 * @brief 从心法名称创建玩家实例
 */
static void *CreatePlayerByKungfu(const std::string &kungfu_name)
{
    // TODO: 实现心法名称到玩家实例的映射
    // 这里暂时只支持太虚剑意
    if (kungfu_name == "太虚剑意" || kungfu_name == "TaiXuJianYi") {
        return new 太虚剑意::Player();
    }

    spdlog::error("不支持的心法: {}", kungfu_name);
    return nullptr;
}

/**
 * @brief 从宏中提取时间条件阈值
 */
static void ExtractMacroTriggers(const std::string &macro_str, vector_t<MacroTrigger> &triggers)
{
    // 简单的正则匹配提取时间条件
    // 支持的格式: bufftime:BUFF名>N, skill_cd:技能名<N, tbufftime:BUFF名>N

    // TODO: 这里需要解析宏的 AST 来提取时间条件
    // 暂时使用简单的字符串查找
    size_t pos = 0;

    // 查找 bufftime:
    while ((pos = macro_str.find("bufftime:", pos)) != std::string::npos) {
        pos += 9; // 跳过 "bufftime:"
        size_t end_pos = macro_str.find_first_of("><=&| ]", pos);
        if (end_pos != std::string::npos) {
            std::string buff_name = macro_str.substr(pos, end_pos - pos);
            // 提取阈值
            char op = macro_str[end_pos];
            if (op == '>' || op == '<' || op == '=') {
                size_t num_start = end_pos + 1;
                size_t num_end   = macro_str.find_first_not_of("0123456789", num_start);
                if (num_end != std::string::npos && num_end > num_start) {
                    int threshold_seconds = std::stoi(macro_str.substr(num_start, num_end - num_start));
                    tick_t threshold      = threshold_seconds * JX3_FRAMES_PER_SECOND;

                    // TODO: 获取 BUFF ID 并转换为 cache_index
                    // 这里需要实际的 BUFF ID 映射
                    spdlog::debug("发现宏时间条件: bufftime:{} {} {}", buff_name, op, threshold_seconds);

                    // MacroTrigger trigger;
                    // trigger.type        = MacroTrigger::Type::BUFF_DURATION;
                    // trigger.cache_index = GetBuffCacheIndex(buff_name);
                    // trigger.threshold   = threshold;
                    // triggers.push_back(trigger);
                }
            }
        }
    }

    // 查找 skill_cd:
    pos = 0;
    while ((pos = macro_str.find("skill_cd:", pos)) != std::string::npos) {
        pos += 9; // 跳过 "skill_cd:"
        size_t end_pos = macro_str.find_first_of("><=&| ]", pos);
        if (end_pos != std::string::npos) {
            std::string skill_name = macro_str.substr(pos, end_pos - pos);
            char        op         = macro_str[end_pos];
            if (op == '>' || op == '<' || op == '=') {
                size_t num_start = end_pos + 1;
                size_t num_end   = macro_str.find_first_not_of("0123456789", num_start);
                if (num_end != std::string::npos && num_end > num_start) {
                    int threshold_seconds = std::stoi(macro_str.substr(num_start, num_end - num_start));
                    tick_t threshold      = threshold_seconds * JX3_FRAMES_PER_SECOND;

                    spdlog::debug("发现宏时间条件: skill_cd:{} {} {}", skill_name, op, threshold_seconds);

                    // TODO: 实现技能 CD 触发器
                }
            }
        }
    }

    // 查找 tbufftime:
    pos = 0;
    while ((pos = macro_str.find("tbufftime:", pos)) != std::string::npos) {
        pos += 10; // 跳过 "tbufftime:"
        size_t end_pos = macro_str.find_first_of("><=&| ]", pos);
        if (end_pos != std::string::npos) {
            std::string buff_name = macro_str.substr(pos, end_pos - pos);
            char        op        = macro_str[end_pos];
            if (op == '>' || op == '<' || op == '=') {
                size_t num_start = end_pos + 1;
                size_t num_end   = macro_str.find_first_not_of("0123456789", num_start);
                if (num_end != std::string::npos && num_end > num_start) {
                    int threshold_seconds = std::stoi(macro_str.substr(num_start, num_end - num_start));
                    tick_t threshold      = threshold_seconds * JX3_FRAMES_PER_SECOND;

                    spdlog::debug("发现宏时间条件: tbufftime:{} {} {}", buff_name, op, threshold_seconds);

                    // TODO: 实现目标 BUFF 触发器
                }
            }
        }
    }
}

bool Context::Init(const std::string &config_json)
{
    try {
        // 解析 JSON
        auto config = nlohmann::json::parse(config_json);

        // 1. 解析玩家心法
        if (config.contains("player")) {
            const auto &player_config = config["player"];

            // 优先使用 "class" 字段，兼容 "kungfu" 字段
            std::string kungfu_name;
            if (player_config.contains("class")) {
                kungfu_name = player_config["class"];
            } else if (player_config.contains("kungfu")) {
                kungfu_name = player_config["kungfu"];
            } else {
                spdlog::error("JSON 配置缺少 player.class 或 player.kungfu 字段");
                return false;
            }

            player = CreatePlayerByKungfu(kungfu_name);

            if (player == nullptr) {
                spdlog::error("创建玩家失败: 不支持的心法 {}", kungfu_name);
                return false;
            }

            spdlog::info("成功创建玩家: {}", kungfu_name);
        } else {
            spdlog::error("JSON 配置缺少 player 字段");
            return false;
        }

        // 2. 解析宏列表
        if (config.contains("macros") && config["macros"].is_array()) {
            macro_triggers.clear();

            for (const auto &macro_item : config["macros"]) {
                std::string macro_str = macro_item.get<std::string>();
                spdlog::debug("解析宏: {}", macro_str);

                // 提取宏中的时间条件
                ExtractMacroTriggers(macro_str, macro_triggers);
            }

            spdlog::info("成功解析 {} 个宏，提取 {} 个时间触发器",
                         config["macros"].size(),
                         macro_triggers.size());
        }

        // 3. 重置上下文状态
        cache.Reset();
        current_tick = 0;

        return true;

    } catch (const nlohmann::json::exception &e) {
        spdlog::error("JSON 解析错误: {}", e.what());
        return false;
    } catch (const std::exception &e) {
        spdlog::error("初始化错误: {}", e.what());
        return false;
    }
}

} // namespace JX3DPS
