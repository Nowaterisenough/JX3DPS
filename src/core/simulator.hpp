#ifndef JX3DPS_CORE_SIMULATOR_HPP
#define JX3DPS_CORE_SIMULATOR_HPP

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "src/global/types.h"
#include "src/macro/macro_interpreter.h"
#include "src/player/player.hpp"

#include "context.h"
#include "keyframe.hpp"

namespace JX3DPS {

using json = nlohmann::json;

/**
 * @brief 模拟器配置
 */
struct SimulatorConfig
{
    int         duration_seconds = 300; // 模拟时长（秒）
    int         iterations       = 1;   // 模拟次数
    int         frame_rate       = 16;  // 帧率 (16帧/秒)
    std::string macro;                  // 宏字符串
    json        player_config;          // 玩家配置
    json        target_config;          // 目标配置

    // 从JSON加载配置
    static SimulatorConfig FromJson(const json &config)
    {
        SimulatorConfig result;

        if (config.contains("simulation")) {
            const auto &sim         = config["simulation"];
            result.duration_seconds = sim.value("duration", 300);
            result.iterations       = sim.value("iterations", 1);
            result.frame_rate       = sim.value("frame_rate", 16);
        }

        if (config.contains("macro")) {
            result.macro = config["macro"].get<std::string>();
        }

        if (config.contains("player")) {
            result.player_config = config["player"];
        }

        if (config.contains("target")) {
            result.target_config = config["target"];
        }

        return result;
    }
};

/**
 * @brief 模拟结果统计
 */
struct SimulationStats
{
    double    average_dps   = 0.0;
    double    max_dps       = 0.0;
    double    min_dps       = 0.0;
    double    std_deviation = 0.0;
    long long total_damage  = 0;
    int       total_frames  = 0;
    double    total_time    = 0.0;

    // 技能统计
    struct SkillStat
    {
        std::string name;
        int         cast_count     = 0;
        long long   total_damage   = 0;
        int         hit_count      = 0;
        int         critical_count = 0;
        double      average_damage = 0.0;
        double      critical_rate  = 0.0;
        double      dps            = 0.0;
    };

    hash_t<jx3id_t, SkillStat> skill_stats;

    // BUFF统计
    struct BuffStat
    {
        std::string name;
        double      total_uptime   = 0.0;
        double      uptime_percent = 0.0;
        double      average_stacks = 0.0;
        int         tick_count     = 0;
        long long   total_damage   = 0;
    };

    hash_t<jx3id_t, BuffStat> buff_stats;

    // 时间轴事件
    struct TimelineEvent
    {
        double      time  = 0.0;
        int         frame = 0;
        std::string event_type; // "cast", "damage", "buff_add", "buff_remove"
        std::string skill_name;
        std::string buff_name;
        long long   damage      = 0;
        bool        is_critical = false;
    };

    std::vector<TimelineEvent> timeline;

    // 转换为JSON
    json ToJson() const
    {
        json result;

        result["average_dps"]   = average_dps;
        result["max_dps"]       = max_dps;
        result["min_dps"]       = min_dps;
        result["std_deviation"] = std_deviation;
        result["total_damage"]  = total_damage;
        result["total_frames"]  = total_frames;
        result["total_time"]    = total_time;

        // 技能统计
        json skills = json::object();
        for (const auto &[id, stat] : skill_stats) {
            skills[stat.name] = {
                { "cast_count",     stat.cast_count     },
                { "total_damage",   stat.total_damage   },
                { "hit_count",      stat.hit_count      },
                { "critical_count", stat.critical_count },
                { "average_damage", stat.average_damage },
                { "critical_rate",  stat.critical_rate  },
                { "dps",            stat.dps            }
            };
        }
        result["skill_stats"] = skills;

        // BUFF统计
        json buffs = json::object();
        for (const auto &[id, stat] : buff_stats) {
            buffs[stat.name] = {
                { "total_uptime",   stat.total_uptime   },
                { "uptime_percent", stat.uptime_percent },
                { "average_stacks", stat.average_stacks },
                { "tick_count",     stat.tick_count     },
                { "total_damage",   stat.total_damage   }
            };
        }
        result["buff_stats"] = buffs;

        // 时间轴
        json timeline_json = json::array();
        for (const auto &event : timeline) {
            timeline_json.push_back({
                { "time",        event.time        },
                { "frame",       event.frame       },
                { "event_type",  event.event_type  },
                { "skill_name",  event.skill_name  },
                { "buff_name",   event.buff_name   },
                { "damage",      event.damage      },
                { "is_critical", event.is_critical }
            });
        }
        result["timeline"] = timeline_json;

        return result;
    }
};

/**
 * @brief 核心模拟器类
 *
 * 负责：
 * 1. 初始化玩家和目标
 * 2. 解析宏语句
 * 3. 帧循环驱动
 * 4. 统计数据收集
 */
template <typename PlayerClass>
class Simulator
{
public:
    using ProgressCallback = std::function<void(double)>;

    Simulator(const SimulatorConfig &config) :
        config_(config), total_frames_(config.duration_seconds * config.frame_rate)
    {
        spdlog::info("初始化模拟器: {}秒, {}帧/秒, 共{}帧", config_.duration_seconds, config_.frame_rate, total_frames_);
    }

    /**
     * @brief 运行单次模拟（使用 TickCache 跳帧）
     */
    SimulationStats RunSingle(ProgressCallback progress_callback = nullptr)
    {
        spdlog::debug("开始单次模拟（TickCache 跳帧模式）");

        // 1. 初始化
        Initialize();

        // 2. 解析宏
        if (!ParseMacro()) {
            spdlog::error("宏解析失败");
            return {};
        }

        // 3. 主循环：从 TickCache 获取最小 tick 并跳跃
        tick_t current_time = 0;
        while (current_time < total_frames_) {
            // 从 Context 获取下一个关键帧时刻（包含宏阈值）
            tick_t next_tick = context.GetNextTick();

            // 如果没有待处理的事件，说明模拟结束
            if (next_tick == 0) {
                spdlog::debug("TickCache 中无待处理事件，模拟结束");
                break;
            }

            // 跳转到下一个事件时间点
            if (current_time + next_tick >= total_frames_) {
                next_tick = total_frames_ - current_time;
            }

            current_time += next_tick;

            // 更新进度
            if (progress_callback && current_time % 100 == 0) {
                double progress = static_cast<double>(current_time) / total_frames_;
                progress_callback(progress);
            }

            // 更新 TickCache 和 GCD（减去跳过的时间）
            context.Update(next_tick);

            // 当 GCD 为 0 时，尝试执行宏中的技能
            if (context.GetGlobalCooldown() == 0 && macro_ast_) {
                bool should_cast = macro_ast_->Evaluate();
                if (should_cast) {
                    spdlog::trace("帧 {}: 宏条件满足，尝试施放技能", current_time);
                    // TODO: 执行宏技能
                }
            }

            // 触发所有在此时间点到期的技能/BUFF
            TriggerExpiredEvents();

            current_frame_ = current_time;
        }

        // 4. 生成统计
        SimulationStats stats = GenerateStats();

        spdlog::debug("单次模拟完成: DPS = {:.2f}", stats.average_dps);

        return stats;
    }

    /**
     * @brief 运行多次模拟
     */
    std::vector<SimulationStats> RunMultiple(ProgressCallback progress_callback = nullptr)
    {
        spdlog::info("开始{}次模拟", config_.iterations);

        std::vector<SimulationStats> results;
        results.reserve(config_.iterations);

        for (int i = 0; i < config_.iterations; ++i) {
            auto callback = [&, i](double iter_progress) {
                if (progress_callback) {
                    double total_progress = (i + iter_progress) / config_.iterations;
                    progress_callback(total_progress);
                }
            };

            SimulationStats stats = RunSingle(callback);
            results.push_back(stats);

            spdlog::info("迭代 {}/{}: DPS = {:.2f}", i + 1, config_.iterations, stats.average_dps);

            // 重置状态
            Reset();
        }

        return results;
    }

    /**
     * @brief 计算聚合统计
     */
    static SimulationStats AggregateStats(const std::vector<SimulationStats> &all_stats)
    {
        if (all_stats.empty()) {
            return {};
        }

        SimulationStats result;

        // 计算DPS统计
        double sum     = 0.0;
        result.max_dps = all_stats[0].average_dps;
        result.min_dps = all_stats[0].average_dps;

        for (const auto &stats : all_stats) {
            double dps  = stats.average_dps;
            sum        += dps;

            if (dps > result.max_dps) {
                result.max_dps = dps;
            }
            if (dps < result.min_dps) {
                result.min_dps = dps;
            }
        }

        result.average_dps = sum / all_stats.size();

        // 计算标准差
        double variance = 0.0;
        for (const auto &stats : all_stats) {
            double diff  = stats.average_dps - result.average_dps;
            variance    += diff * diff;
        }
        result.std_deviation = std::sqrt(variance / all_stats.size());

        // 使用第一次迭代的详细数据
        result.skill_stats  = all_stats[0].skill_stats;
        result.buff_stats   = all_stats[0].buff_stats;
        result.timeline     = all_stats[0].timeline;
        result.total_damage = all_stats[0].total_damage;
        result.total_frames = all_stats[0].total_frames;
        result.total_time   = all_stats[0].total_time;

        return result;
    }

private:
    /**
     * @brief 初始化模拟
     */
    void Initialize()
    {
        spdlog::debug("初始化模拟环境");

        // 重置上下文
        context.Reset();

        // 创建玩家
        player_        = std::make_unique<PlayerClass>();
        context.player = player_.get();

        // 从配置初始化玩家属性
        InitializePlayerFromConfig();

        // 创建目标
        target_                      = std::make_unique<Target>();
        target_->id                  = 1;
        target_->level               = config_.target_config.value("level", JX3_PLAYER_LEVEL);
        context.targets[target_->id] = target_.get();

        spdlog::debug("玩家和目标已初始化");
    }

    /**
     * @brief 从配置初始化玩家属性
     */
    void InitializePlayerFromConfig()
    {
        if (config_.player_config.empty()) {
            spdlog::warn("玩家配置为空，使用默认值");
            return;
        }

        const auto &cfg = config_.player_config;

        // 设置属性
        if (cfg.contains("attributes")) {
            const auto &attr = cfg["attributes"];

            // TODO: 根据PlayerClass的实际属性接口设置
            // 这里需要根据实际的Player类实现来调整
            spdlog::debug("加载玩家属性");
        }

        // 设置奇穴
        if (cfg.contains("talents")) {
            const auto &talents = cfg["talents"];
            spdlog::debug("加载{}个奇穴", talents.size());
        }

        // 设置秘籍
        if (cfg.contains("recipes")) {
            const auto &recipes = cfg["recipes"];
            spdlog::debug("加载{}个秘籍", recipes.size());
        }
    }

    /**
     * @brief 解析宏
     */
    bool ParseMacro()
    {
        if (config_.macro.empty()) {
            spdlog::warn("宏为空");
            return false;
        }

        spdlog::debug("解析宏: {}", config_.macro);

        try {
            MacroInterpreter interpreter;

            // 设置ID查询函数（TODO: 实现实际的ID映射）
            interpreter.SetSkillIdGetter([](const std::string &name) -> jx3id_t {
                // 简化实现：返回名称的哈希值
                return std::hash<std::string>{}(name);
            });

            interpreter.SetBuffIdGetter([](const std::string &name) -> jx3id_t { return std::hash<std::string>{}(name); });

            // 解析宏
            auto [ast, error] = interpreter.ParseMacro(config_.macro);

            if (error != ParserError::SUCCESS) {
                spdlog::error("宏解析错误");
                return false;
            }

            macro_ast_ = std::move(ast);
            spdlog::debug("宏解析成功");

            return true;

        } catch (const std::exception &e) {
            spdlog::error("宏解析异常: {}", e.what());
            return false;
        }
    }

    /**
     * @brief 执行一帧
     */
    void ExecuteFrame()
    {
        current_frame_++;

        // 1. 评估宏条件并执行技能
        if (macro_ast_) {
            bool should_cast = macro_ast_->Evaluate();

            if (should_cast) {
                // TODO: 获取宏中的技能并施放
                // 这需要扩展AstNode以携带技能信息
                spdlog::trace("帧 {}: 宏条件满足", current_frame_);
            }
        }

        // 2. 更新BUFF（DOT跳）
        UpdateBuffs();

        // 3. 更新技能CD/读条/前摇
        // （这由SimContext::Update自动完成）

        // 4. 收集统计数据
        CollectFrameStats();
    }

    /**
     * @brief 更新BUFF
     */
    void UpdateBuffs()
    {
        // TODO: 遍历所有活跃BUFF并触发tick事件
        // 这需要维护一个BUFF列表
    }

    /**
     * @brief 触发所有在当前时间点到期的技能/BUFF事件
     *
     * 检查 TickCache 中所有值为 0 的项，触发对应的事件
     */
    void TriggerExpiredEvents()
    {
        // 检查技能冷却到期
        for (size_t i = 0; i < TickCache::MAX_SKILLS; ++i) {
            if (context.cache.skill_cooldown[i] == 0) {
                // 技能CD结束，可以触发回调或标记
                spdlog::trace("技能 {} CD结束", i);
            }
            if (context.cache.skill_prepare[i] == 0) {
                // 技能前摇结束
                spdlog::trace("技能 {} 前摇结束", i);
            }
            if (context.cache.skill_casting[i] == 0) {
                // 技能读条结束
                spdlog::trace("技能 {} 读条结束", i);
            }
        }

        // 检查BUFF事件
        for (size_t u = 0; u < TickCache::MAX_UNITS; ++u) {
            for (size_t b = 0; b < TickCache::MAX_BUFFS; ++b) {
                if (context.cache.buff_duration[u][b] == 0) {
                    // BUFF持续时间结束
                    spdlog::trace("单位 {} BUFF {} 持续时间结束", u, b);
                }
                if (context.cache.buff_interval[u][b] == 0) {
                    // BUFF tick间隔到期
                    spdlog::trace("单位 {} BUFF {} tick触发", u, b);
                    // TODO: 触发BUFF的tick效果，然后重置interval
                }
            }
        }
    }

    /**
     * @brief 收集帧统计数据
     */
    void CollectFrameStats()
    {
        // 记录当前帧的状态
        // 累积伤害、技能统计等
        total_damage_ += frame_damage_;
        frame_damage_  = 0;
    }

    /**
     * @brief 生成统计结果
     */
    SimulationStats GenerateStats()
    {
        SimulationStats stats;

        stats.total_damage = total_damage_;
        stats.total_frames = current_frame_;
        stats.total_time   = static_cast<double>(current_frame_) / config_.frame_rate;

        if (stats.total_time > 0) {
            stats.average_dps = total_damage_ / stats.total_time;
        }

        // 复制详细统计
        stats.skill_stats = skill_stats_;
        stats.buff_stats  = buff_stats_;
        stats.timeline    = timeline_;

        return stats;
    }

    /**
     * @brief 重置模拟器状态
     */
    void Reset()
    {
        current_frame_ = 0;
        total_damage_  = 0;
        frame_damage_  = 0;
        skill_stats_.clear();
        buff_stats_.clear();
        timeline_.clear();
        player_.reset();
        target_.reset();
    }

public:
    /**
     * @brief 记录伤害事件（供技能/BUFF调用）
     */
    void RecordDamage(jx3id_t skill_id, const std::string &skill_name, long long damage, bool is_critical = false)
    {
        frame_damage_ += damage;

        // 更新技能统计
        auto &skill_stat         = skill_stats_[skill_id];
        skill_stat.name          = skill_name;
        skill_stat.total_damage += damage;
        skill_stat.hit_count++;
        if (is_critical) {
            skill_stat.critical_count++;
        }

        // 记录时间轴事件
        SimulationStats::TimelineEvent event;
        event.time        = static_cast<double>(current_frame_) / config_.frame_rate;
        event.frame       = current_frame_;
        event.event_type  = "damage";
        event.skill_name  = skill_name;
        event.damage      = damage;
        event.is_critical = is_critical;
        timeline_.push_back(event);
    }

    /**
     * @brief 记录技能施放（供技能调用）
     */
    void RecordSkillCast(jx3id_t skill_id, const std::string &skill_name)
    {
        auto &skill_stat = skill_stats_[skill_id];
        skill_stat.name  = skill_name;
        skill_stat.cast_count++;

        // 记录时间轴事件
        SimulationStats::TimelineEvent event;
        event.time       = static_cast<double>(current_frame_) / config_.frame_rate;
        event.frame      = current_frame_;
        event.event_type = "cast";
        event.skill_name = skill_name;
        timeline_.push_back(event);
    }

private:
    SimulatorConfig config_;
    int             total_frames_;
    int             current_frame_ = 0;

    std::unique_ptr<PlayerClass> player_;
    std::unique_ptr<Target>      target_;
    std::unique_ptr<AstNode>     macro_ast_;

    // 统计数据
    long long                                   total_damage_ = 0;
    long long                                   frame_damage_ = 0;
    hash_t<jx3id_t, SimulationStats::SkillStat> skill_stats_;
    hash_t<jx3id_t, SimulationStats::BuffStat>  buff_stats_;
    std::vector<SimulationStats::TimelineEvent> timeline_;
};

} // namespace JX3DPS

#endif // JX3DPS_CORE_SIMULATOR_HPP
