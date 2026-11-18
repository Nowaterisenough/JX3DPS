/**
 * @file jx3dps.cpp
 * @brief JX3DPS动态库完整实现
 *
 * 实现 jx3dps.h 中定义的所有API接口
 */

#include "jx3dps.h"
#include "version.h"
#include "core/simulator.hpp"
#include "core/context.h"
#include "class/tai_xu_jian_yi/tai_xu_jian_yi.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <cstdarg>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <vector>

// 内部实现命名空间
namespace {

using json = nlohmann::json;

// ============================================================================
// 全局状态管理
// ============================================================================

struct GlobalState {
    std::string last_result;      // 最后一次模拟结果
    std::string last_error;        // 最后一次错误信息
    std::atomic<double> progress;  // 模拟进度 (0.0 - 1.0)
    std::mutex result_mutex;       // 结果保护锁

    // 调试器状态
    struct {
        std::string debug_info;        // 调试信息
        std::vector<std::string> breakpoints;  // 断点列表
        bool is_debugging = false;     // 是否处于调试状态
        std::mutex debug_mutex;        // 调试锁
    } debugger;

    GlobalState() : progress(0.0) {}
};

// 全局单例
GlobalState& GetGlobalState() {
    static GlobalState state;
    return state;
}

// ============================================================================
// 进度回调类型
// ============================================================================

using ProgressCallback = void(*)(double progress);

// ============================================================================
// 模拟器实现类
// ============================================================================

class SimulatorImpl {
public:
    SimulatorImpl(const std::string& input_json)
        : input_(input_json)
        , progress_(0.0)
    {
        try {
            config_ = json::parse(input_json);
            NormalizeLegacyConfig();
        } catch (const json::exception& e) {
            spdlog::error("JSON解析失败: {}", e.what());
            throw std::runtime_error(std::string("JSON解析失败: ") + e.what());
        }
    }

    // Convert the desktop application's historical schema once at the API
    // boundary so the hot simulation path only handles the compact schema.
    void NormalizeLegacyConfig() {
        if (!config_.contains("player")) {
            json player = json::object();
            player["name"] = "太虚剑意模拟角色";
            player["class"] = config_.value("ClassType", "太虚剑意");
            player["level"] = config_.value("GameLevel", JX3DPS::JX3_PLAYER_LEVEL);
            if (config_.contains("Attribute")) player["attributes"] = config_["Attribute"];
            if (config_.contains("Talents")) player["talents"] = config_["Talents"];
            if (config_.contains("Recipes")) player["recipes"] = config_["Recipes"];
            json equipment = json::object();
            if (config_.contains("equipment")) equipment = config_["equipment"];
            else if (config_.contains("Equipment")) equipment = config_["Equipment"];
            if (config_.contains("EquipEffects")) equipment["equip_effects"] = config_["EquipEffects"];
            if (config_.contains("Consumables")) equipment["consumables"] = config_["Consumables"];
            if (config_.contains("Permanents")) equipment["permanents"] = config_["Permanents"];
            if (!equipment.empty()) player["equipment"] = std::move(equipment);
            config_["player"] = std::move(player);
        }

        if (!config_.contains("target")) {
            config_["target"] = {
                {"name", "木桩"},
                {"level", config_.value("GameLevel", JX3DPS::JX3_PLAYER_LEVEL)}
            };
        }

        if (!config_.contains("simulation")) {
            int duration = 300;
            if (config_.contains("EventsExpression") && config_["EventsExpression"].is_array()) {
                for (const auto &event : config_["EventsExpression"]) {
                    if (!event.is_string() || event.get<std::string>().size() < 6) continue;
                    const auto value = event.get<std::string>();
                    if (value[2] == ':' && value[5] == ' ') {
                        const int minutes = std::stoi(value.substr(0, 2));
                        const int seconds = std::stoi(value.substr(3, 2));
                        duration = std::max(duration, minutes * 60 + seconds);
                    }
                }
            }
            int iterations = 1;
            if (config_.contains("Options")) {
                iterations = config_["Options"].value("SimIterations", iterations);
            }
            config_["simulation"] = {
                {"duration", duration},
                {"iterations", std::max(iterations, 1)},
                {"frame_rate", 16}
            };
        }

        if (!config_.contains("macro") && config_.contains("SkillsExpression") &&
            config_["SkillsExpression"].is_object()) {
            const auto &macros = config_["SkillsExpression"];
            if (!macros.empty()) {
                const auto &first = macros.begin().value();
                std::string macro;
                if (first.is_array()) {
                    for (const auto &line : first) {
                        if (!line.is_string()) continue;
                        if (!macro.empty()) macro.push_back('\n');
                        macro += line.get<std::string>();
                    }
                } else if (first.is_string()) {
                    macro = first.get<std::string>();
                }
                if (!macro.empty()) config_["macro"] = std::move(macro);
            }
        }
    }

    // 运行模拟
    int Run(ProgressCallback callback = nullptr) {
        try {
            spdlog::info("开始DPS模拟");

            // 初始化
            if (!Initialize()) {
                return -1;
            }
            UpdateProgress(0.1, callback);

            // 解析配置
            if (!ParseConfiguration()) {
                return -2;
            }
            UpdateProgress(0.2, callback);

            // 执行模拟
            if (!ExecuteSimulation(callback)) {
                return -3;
            }

            // 生成结果
            if (!GenerateResult()) {
                return -4;
            }
            UpdateProgress(1.0, callback);

            spdlog::info("DPS模拟完成");
            return 0;

        } catch (const std::exception& e) {
            spdlog::error("模拟失败: {}", e.what());
            GetGlobalState().last_error = e.what();
            return -100;
        }
    }

    const std::string& GetResult() const {
        return result_;
    }

private:
    bool Initialize() {
        spdlog::debug("初始化模拟器");

        // 检查必要字段
        if (!config_.contains("player")) {
            GetGlobalState().last_error = "缺少 player 配置";
            return false;
        }

        if (!config_.contains("target")) {
            GetGlobalState().last_error = "缺少 target 配置";
            return false;
        }

        return true;
    }

    bool ParseConfiguration() {
        spdlog::debug("解析配置");

        try {
            // 解析玩家配置（用于日志）
            if (config_.contains("player")) {
                const auto& player = config_["player"];
                spdlog::info("玩家: {}", player.value("name", "未知"));
                spdlog::info("心法: {}", player.value("class", "未知"));
            }

            // 解析目标配置
            if (config_.contains("target")) {
                const auto& target = config_["target"];
                spdlog::info("目标: {}", target.value("name", "木桩"));
                spdlog::info("等级: {}", target.value("level", JX3DPS::JX3_PLAYER_LEVEL));
            }

            // 解析模拟配置
            if (config_.contains("simulation")) {
                const auto& sim = config_["simulation"];
                duration_ = sim.value("duration", 300);  // 默认5分钟
                iterations_ = sim.value("iterations", 1);  // 默认1次

                spdlog::info("模拟时长: {} 秒", duration_);
                spdlog::info("模拟次数: {}", iterations_);
            }

            return true;

        } catch (const json::exception& e) {
            GetGlobalState().last_error = std::string("配置解析失败: ") + e.what();
            return false;
        }
    }

    bool ExecuteSimulation(ProgressCallback callback) {
        spdlog::debug("执行模拟");

        const double base_progress = 0.2;
        const double sim_progress_range = 0.7;  // 0.2 -> 0.9

        std::vector<double> dps_results;
        dps_results.reserve(iterations_);

        // Reuse one simulator for the full batch. Macro ASTs and compact ID
        // bindings are compiled once, while each iteration resets only hot
        // runtime state.
        if (config_["player"].value("class", "太虚剑意") == "太虚剑意") {
            using namespace JX3DPS;
            SimulatorConfig sim_config = SimulatorConfig::FromJson(config_);
            Simulator<太虚剑意::Player> simulator(sim_config);
            auto progress = [&](double value) {
                UpdateProgress(base_progress + sim_progress_range * value, callback);
            };
            const auto stats = simulator.RunMultiple(progress);
            for (const auto &item : stats) dps_results.push_back(item.average_dps);
            if (!stats.empty()) detailed_stats_ = stats.front();
            CalculateStatistics(dps_results);
            return true;
        }

        for (int i = 0; i < iterations_; ++i) {
            // 单次模拟
            double dps = SimulateSingleIteration(i);
            dps_results.push_back(dps);

            // 更新进度
            double iter_progress = base_progress +
                sim_progress_range * (i + 1) / iterations_;
            UpdateProgress(iter_progress, callback);

            spdlog::debug("迭代 {}/{}: DPS = {:.2f}",
                i + 1, iterations_, dps);
        }

        // 计算统计数据
        CalculateStatistics(dps_results);

        return true;
    }

    double SimulateSingleIteration(int iteration) {
        using namespace JX3DPS;

        // 根据心法类型创建对应的模拟器
        std::string class_name = config_["player"].value("class", "太虚剑意");

        if (class_name == "太虚剑意") {
            // 创建太虚剑意模拟器
            SimulatorConfig sim_config = SimulatorConfig::FromJson(config_);

            Simulator<太虚剑意::Player> simulator(sim_config);

            // 运行单次模拟
            SimulationStats stats = simulator.RunSingle();

            // 存储详细统计数据（第一次迭代）
            if (iteration == 0) {
                detailed_stats_ = stats;
            }

            return stats.average_dps;
        } else {
            // 不支持的心法，返回示例值
            spdlog::warn("不支持的心法: {}", class_name);

            double base_dps = 50000.0;
            std::srand(static_cast<unsigned>(iteration + std::time(nullptr)));
            double variance = 0.95 + (std::rand() % 100) / 1000.0;
            return base_dps * variance;
        }
    }

    void CalculateStatistics(const std::vector<double>& results) {
        if (results.empty()) {
            return;
        }

        // 平均值
        double sum = 0.0;
        for (double dps : results) {
            sum += dps;
        }
        avg_dps_ = sum / results.size();

        // 最大/最小
        max_dps_ = results[0];
        min_dps_ = results[0];
        for (double dps : results) {
            if (dps > max_dps_) max_dps_ = dps;
            if (dps < min_dps_) min_dps_ = dps;
        }

        // 标准差
        double variance = 0.0;
        for (double dps : results) {
            double diff = dps - avg_dps_;
            variance += diff * diff;
        }
        std_dps_ = std::sqrt(variance / results.size());

        spdlog::info("平均DPS: {:.2f}", avg_dps_);
        spdlog::info("最大DPS: {:.2f}", max_dps_);
        spdlog::info("最小DPS: {:.2f}", min_dps_);
        spdlog::info("标准差: {:.2f}", std_dps_);
    }

    bool GenerateResult() {
        spdlog::debug("生成结果");

        try {
            json result;

            // 基础信息
            result["version"] = VERSION;
            result["jx3_version"] = JX3DPS::JX3_VERSIONS[static_cast<std::size_t>(JX3_VERSION)];
            result["timestamp"] = std::time(nullptr);

            // 统计数据
            result["statistics"] = {
                {"average_dps", avg_dps_},
                {"max_dps", max_dps_},
                {"min_dps", min_dps_},
                {"std_deviation", std_dps_},
                {"iterations", iterations_},
                {"duration", duration_}
            };

            // 玩家信息
            if (config_.contains("player")) {
                result["player"] = config_["player"];
            }

            // 目标信息
            if (config_.contains("target")) {
                result["target"] = config_["target"];
            }

            // 详细数据 (使用详细统计信息)
            if (detailed_stats_.total_frames > 0) {
                result["details"] = detailed_stats_.ToJson();
            } else {
                result["details"] = {
                    {"timeline", json::array()},
                    {"skill_stats", json::object()},
                    {"buff_stats", json::object()}
                };
            }

            result_ = result.dump(2);  // 2空格缩进

            return true;

        } catch (const json::exception& e) {
            GetGlobalState().last_error = std::string("结果生成失败: ") + e.what();
            return false;
        }
    }

    void UpdateProgress(double progress, ProgressCallback callback) {
        progress_ = progress;
        GetGlobalState().progress.store(progress);

        if (callback) {
            callback(progress);
        }
    }

private:
    std::string input_;
    json config_;
    std::string result_;

    double progress_;
    int duration_ = 300;
    int iterations_ = 1;

    double avg_dps_ = 0.0;
    double max_dps_ = 0.0;
    double min_dps_ = 0.0;
    double std_dps_ = 0.0;

    JX3DPS::SimulationStats detailed_stats_;  // 详细统计数据
};

// ============================================================================
// 调试器实现类
// ============================================================================

class DebuggerImpl {
public:
    DebuggerImpl(const std::string& input_json)
        : input_(input_json)
    {
        try {
            config_ = json::parse(input_json);
            if (config_.contains("simulation")) {
                const int duration = config_["simulation"].value("duration", 300);
                max_frames_ = std::max(1, duration) * 16;
            }
        } catch (const json::exception& e) {
            spdlog::error("JSON解析失败: {}", e.what());
            throw std::runtime_error(std::string("JSON解析失败: ") + e.what());
        }
    }

    int Run(ProgressCallback callback = nullptr) {
        std::lock_guard<std::mutex> lock(GetGlobalState().debugger.debug_mutex);

        GetGlobalState().debugger.is_debugging = true;
        current_frame_ = 0;

        spdlog::info("调试器已启动");

        // 生成初始调试信息
        UpdateDebugInfo();

        return 0;
    }

    std::string StepIn() {
        std::lock_guard<std::mutex> lock(GetGlobalState().debugger.debug_mutex);

        if (!GetGlobalState().debugger.is_debugging) {
            return R"({"error": "调试器未启动"})";
        }

        current_frame_++;
        spdlog::debug("单步进入: 帧 {}", current_frame_);

        UpdateDebugInfo();
        return GetGlobalState().debugger.debug_info;
    }

    std::string StepOver() {
        std::lock_guard<std::mutex> lock(GetGlobalState().debugger.debug_mutex);

        if (!GetGlobalState().debugger.is_debugging) {
            return R"({"error": "调试器未启动"})";
        }

        current_frame_ += 16;  // 跳过1帧 (假设16帧/秒)
        spdlog::debug("单步越过: 帧 {}", current_frame_);

        UpdateDebugInfo();
        return GetGlobalState().debugger.debug_info;
    }

    std::string Continue() {
        std::lock_guard<std::mutex> lock(GetGlobalState().debugger.debug_mutex);

        if (!GetGlobalState().debugger.is_debugging) {
            return R"({"error": "调试器未启动"})";
        }

        // 执行到下一个断点或结束
        bool hit_breakpoint = false;

        while (current_frame_ < max_frames_ && !hit_breakpoint) {
            current_frame_++;

            // 支持 frame:N 和纯数字两种断点写法；未知格式保留给宏断点扩展。
            for (const auto& bp : GetGlobalState().debugger.breakpoints) {
                int breakpoint_frame = -1;
                try {
                    if (bp.rfind("frame:", 0) == 0) {
                        breakpoint_frame = std::stoi(bp.substr(6));
                    } else if (!bp.empty() && std::isdigit(static_cast<unsigned char>(bp.front()))) {
                        breakpoint_frame = std::stoi(bp);
                    }
                } catch (const std::exception&) {
                    breakpoint_frame = -1;
                }
                if (breakpoint_frame >= 0 && current_frame_ == breakpoint_frame) {
                    hit_breakpoint = true;
                    break;
                }
            }
        }

        if (current_frame_ >= max_frames_) {
            GetGlobalState().debugger.is_debugging = false;
            spdlog::info("调试完成");
        }

        UpdateDebugInfo();
        return GetGlobalState().debugger.debug_info;
    }

private:
    void UpdateDebugInfo() {
        json info;

        info["frame"] = current_frame_;
        info["time"] = static_cast<double>(current_frame_) / 16.0;  // 秒
        info["is_debugging"] = GetGlobalState().debugger.is_debugging;

        // TODO: 添加更多调试信息
        // - 当前技能
        // - 当前BUFF
        // - 玩家状态
        // - 目标状态

        info["player"] = {
            {"hp", 100000},
            {"mp", 10000},
            {"qidian", 5}
        };

        info["target"] = {
            {"hp", 50000000},
            {"buffs", json::array()}
        };

        GetGlobalState().debugger.debug_info = info.dump(2);
    }

private:
    std::string input_;
    json config_;
    int current_frame_ = 0;
    int max_frames_ = 16 * 300;  // 5分钟 @ 16帧/秒
};

// Keep the debugger object alive between the C API start and step calls.
thread_local std::unique_ptr<DebuggerImpl> active_debugger;

} // anonymous namespace

// ============================================================================
// C API 实现
// ============================================================================

extern "C" {

JX3DPS_API const char* jx3dps_version() {
    return VERSION;
}

JX3DPS_API const char* jx3dps_jx3_version() {
    return JX3DPS::JX3_VERSIONS[static_cast<std::size_t>(JX3_VERSION)];
}

JX3DPS_API int jx3dps_simulate(const char* const in, ...) {
    if (!in) {
        spdlog::error("输入为空");
        GetGlobalState().last_error = "输入为空";
        return -1;
    }

    try {
        // The historical ABI is variadic, but callers may provide no extra
        // arguments (the web build does this). Never read an absent vararg;
        // use the result/progress accessors for a stable cross-language API.
        ProgressCallback callback = nullptr;

        // 创建模拟器并运行
        SimulatorImpl simulator(in);
        int result = simulator.Run(callback);

        if (result == 0) {
            std::lock_guard<std::mutex> lock(GetGlobalState().result_mutex);
            GetGlobalState().last_result = simulator.GetResult();
        }

        return result;

    } catch (const std::exception& e) {
        spdlog::error("模拟异常: {}", e.what());
        GetGlobalState().last_error = e.what();
        return -100;
    }
}

JX3DPS_API const char* jx3dps_get_result() {
    std::lock_guard<std::mutex> lock(GetGlobalState().result_mutex);
    return GetGlobalState().last_result.c_str();
}

JX3DPS_API double jx3dps_get_progress() {
    return GetGlobalState().progress.load();
}

JX3DPS_API int jx3dps_debug(const char* const in, ...) {
    if (!in) {
        spdlog::error("输入为空");
        GetGlobalState().last_error = "输入为空";
        return -1;
    }

    try {
        ProgressCallback callback = nullptr;

        // 创建调试器并运行；后续单步接口复用同一个实例。
        auto debugger = std::make_unique<DebuggerImpl>(in);
        const int result = debugger->Run(callback);
        if (result == 0) active_debugger = std::move(debugger);
        return result;

    } catch (const std::exception& e) {
        spdlog::error("调试异常: {}", e.what());
        GetGlobalState().last_error = e.what();
        return -100;
    }
}

JX3DPS_API const char* jx3dps_debugger_step_in() {
    if (!active_debugger) {
        return R"({"error": "调试器未初始化，请先调用 jx3dps_debug"})";
    }

    std::string result = active_debugger->StepIn();
    return GetGlobalState().debugger.debug_info.c_str();
}

JX3DPS_API const char* jx3dps_debugger_step_over() {
    if (!active_debugger) {
        return R"({"error": "调试器未初始化，请先调用 jx3dps_debug"})";
    }

    std::string result = active_debugger->StepOver();
    return GetGlobalState().debugger.debug_info.c_str();
}

JX3DPS_API const char* jx3dps_debugger_continue() {
    if (!active_debugger) {
        return R"({"error": "调试器未初始化，请先调用 jx3dps_debug"})";
    }

    std::string result = active_debugger->Continue();
    return GetGlobalState().debugger.debug_info.c_str();
}

JX3DPS_API void jx3dps_debugger_set_breakpoint(const char* const in) {
    if (!in) {
        spdlog::warn("断点输入为空");
        return;
    }

    std::lock_guard<std::mutex> lock(GetGlobalState().debugger.debug_mutex);
    GetGlobalState().debugger.breakpoints.emplace_back(in);

    spdlog::info("设置断点: {}", in);
}

} // extern "C"
