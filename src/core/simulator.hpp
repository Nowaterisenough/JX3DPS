#ifndef JX3DPS_CORE_SIMULATOR_HPP
#define JX3DPS_CORE_SIMULATOR_HPP

#include <functional>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "src/global/types.h"
#include "src/macro/macro_interpreter.h"
#include "src/player/player.hpp"
#include "src/class/tai_xu_jian_yi/tai_xu_jian_yi_ids.h"
#include "src/class/tai_xu_jian_yi/tai_xu_jian_yi_skill.h"

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
    std::vector<std::string> macros;    // 按优先级排列的宏语句
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

        if (config.contains("macro") && config["macro"].is_string()) {
            result.macro = config["macro"].get<std::string>();
            std::istringstream lines(result.macro);
            for (std::string line; std::getline(lines, line);) {
                if (!line.empty()) result.macros.push_back(line);
            }
        } else if (config.contains("macros") && config["macros"].is_array()) {
            for (const auto &line : config["macros"]) {
                if (line.is_string()) result.macros.push_back(line.get<std::string>());
            }
            if (!result.macros.empty()) result.macro = result.macros.front();
        }

        if (config.contains("player")) {
            result.player_config = config["player"];
        }

        // New profiles keep gear under player, while legacy profiles may
        // store it at the document root.
        if (config.contains("equipment") || config.contains("Equipment")) {
            if (result.player_config.is_null()) result.player_config = json::object();
            result.player_config["equipment"] = config.contains("equipment")
                ? config["equipment"] : config["Equipment"];
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
        if (!macro_compiled_ && !ParseMacro()) {
            spdlog::error("宏解析失败");
            return {};
        }

        // 3. 主循环：从 TickCache 获取最小 tick 并跳跃
        tick_t current_time = 0;
        while (current_time < total_frames_) {
            // 宏只在运行期执行；AST 和条件 ID 已在解析阶段完成缓存绑定。
            if (context.GetGlobalCooldown() == 0) {
                // Macro lines are ordered by priority. Try the next line when
                // a satisfied condition points at a skill that is not ready.
                for (const auto &macro_ast : macro_asts_) {
                    if (macro_ast && macro_ast->Evaluate() &&
                        ExecuteMacroAction(macro_ast->GetAction())) {
                        break;
                    }
                }
            }

            // 从 Context 获取下一个关键帧时刻（包含宏阈值）
            tick_t next_tick = context.GetNextTick();

            // 宏和持续时间都可能暂时没有事件，使用一帧心跳保证循环能继续。
            if (next_tick == 0) next_tick = 1;

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

        // The first run builds ID/trigger bindings. Subsequent iterations
        // retain those compile-time bindings and only clear hot state.
        if (macro_compiled_) context.ResetRuntime();
        else context.Reset();

        // 创建玩家
        player_        = std::make_unique<PlayerClass>();
        context.player = player_.get();

        // 从配置初始化玩家属性
        InitializePlayerFromConfig();
        context.physics_attack_power = player_->attribute.GetPhysicsAttackPower();
        context.weapon_damage = player_->attribute.GetWeaponDamage();
        context.physics_critical_strike = player_->attribute.GetPhysicsCriticalStrike();
        context.physics_overcome = player_->attribute.GetPhysicsOvercome();
        context.haste = player_->attribute.GetHastePercent();

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
            using AttributeType = Attribute::Type;
            const auto set = [&](const char *name, AttributeType type) {
                if (attr.contains(name) && attr[name].is_number()) {
                    player_->attribute.Set(type, attr[name].get<value_t>());
                }
            };
            set("身法", AttributeType::AGILITY_BASE);
            set("力道", AttributeType::STRENGTH_BASE);
            set("根骨", AttributeType::SPIRIT_BASE);
            set("元气", AttributeType::SPUNK_BASE);
            set("基础武器伤害", AttributeType::WEAPON_DAMAGE_BASE);
            set("浮动武器伤害", AttributeType::WEAPON_DAMAGE_RAND);
            set("外功基础攻击", AttributeType::PHYSICS_ATTACK_POWER_BASE);
            set("内功基础攻击", AttributeType::MAGIC_ATTACK_POWER_BASE);
            set("外功会心等级", AttributeType::PHYSICS_CRITICAL_STRIKE);
            set("内功会心等级", AttributeType::MAGIC_CRITICAL_STRIKE);
            set("外功会效等级", AttributeType::PHYSICS_CRITICAL_STRIKE_POWER);
            set("内功会效等级", AttributeType::MAGIC_CRITICAL_STRIKE_POWER);
            set("外功基础破防等级", AttributeType::PHYSICS_OVERCOME_BASE);
            set("内功基础破防等级", AttributeType::MAGIC_OVERCOME_BASE);
            set("无双", AttributeType::STRAIN_BASE);
            set("破招值", AttributeType::SURPLUS_VALUE_BASE);
            set("加速等级", AttributeType::HASTE_BASE);
            spdlog::debug("加载玩家属性: {}项", attr.size());
        }

        // Equipment, gems, enchants, food and set bonuses are flattened once
        // here. Skill damage only reads the resulting Context cache.
        player_->equipment = EquipmentLoadout::FromJson(cfg);
        player_->equipment.ApplyTo(player_->attribute);
        context.equipment_effect_mask = player_->equipment.runtime.triggered_effect_mask;

        // Compile names into numeric IDs before the simulation loop.
        if (cfg.contains("talents") && cfg["talents"].is_array()) {
            for (const auto &entry : cfg["talents"]) {
                if (!entry.is_string()) continue;
                const auto id = 太虚剑意::TalentIdFromName(entry.get<std::string>());
                if (id != 0) player_->active_talents[id] = true;
            }
        }
        if (cfg.contains("recipes") && cfg["recipes"].is_object()) {
            for (auto it = cfg["recipes"].begin(); it != cfg["recipes"].end(); ++it) {
                if (!it.value().is_array()) continue;
                for (const auto &entry : it.value()) {
                    if (!entry.is_string()) continue;
                    const auto id = 太虚剑意::RecipeIdFromName(entry.get<std::string>());
                    if (id != 0) player_->active_recipes[id] = true;
                }
            }
        }

        spdlog::debug("加载{}个奇穴、{}组秘籍、{}个装备部位",
                      player_->active_talents.size(),
                      cfg.contains("recipes") && cfg["recipes"].is_object() ? cfg["recipes"].size() : 0,
                      std::count(player_->equipment.equipped.begin(), player_->equipment.equipped.end(), true));
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

            // 使用当前心法的真实 ID，缓存层再将稀疏 ID 压缩到固定槽位。
            interpreter.SetSkillIdGetter([](const std::string &name) -> jx3id_t {
                const auto id = 太虚剑意::SkillIdFromName(name);
                if (id != 0) return id;
                return static_cast<jx3id_t>(std::hash<std::string>{}(name) & 0x7fffffff);
            });

            interpreter.SetBuffIdGetter([](const std::string &name) -> jx3id_t {
                const auto id = 太虚剑意::BuffIdFromName(name);
                if (id != 0) return id;
                return static_cast<jx3id_t>(std::hash<std::string>{}(name) & 0x7fffffff);
            });

            // 解析宏
            macro_asts_.clear();
            const auto macro_lines = config_.macros.empty()
                ? std::vector<std::string>{config_.macro}
                : config_.macros;
            for (const auto &line : macro_lines) {
                if (line.empty()) continue;
                auto [ast, error] = interpreter.ParseMacro(line);
                if (error != ParserError::SUCCESS) {
                    // Keep valid priority lines usable when an old profile
                    // contains a stale or truncated macro entry.
                    spdlog::warn("跳过无效宏语句: {}", line);
                    continue;
                }
                macro_asts_.push_back(std::move(ast));
            }
            if (macro_asts_.empty()) return false;
            macro_compiled_ = true;
            spdlog::debug("宏解析成功: {} 条", macro_asts_.size());

            return true;

        } catch (const std::exception &e) {
            spdlog::error("宏解析异常: {}", e.what());
            return false;
        }
    }

    bool ExecuteMacroAction(const std::string &action)
    {
        if constexpr (std::is_same_v<PlayerClass, 太虚剑意::Player>) {
            if (action == "无我无剑") {
                太虚剑意::无我无剑 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else if (action == "八荒归元") {
                太虚剑意::八荒归元 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else if (action == "三环套月") {
                太虚剑意::三环套月 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else if (action == "万剑归宗") {
                太虚剑意::万剑归宗 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else if (action == "人剑合一") {
                太虚剑意::人剑合一 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else if (action == "碎星辰") {
                太虚剑意::碎星辰 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else if (action == "生太极") {
                太虚剑意::生太极 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else if (action == "吞日月") {
                太虚剑意::吞日月 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else if (action == "三柴剑法") {
                太虚剑意::三柴剑法 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else if (action == "紫气东来") {
                太虚剑意::紫气东来 skill;
                if (!skill.IsReady()) return false;
                skill.Cast();
            } else {
                return false;
            }
            return true;
        }
        (void)action;
        return false;
    }

    /**
     * @brief 执行一帧
     */
    void ExecuteFrame()
    {
        current_frame_++;

        // 1. 评估宏条件并执行技能
        for (const auto &macro_ast : macro_asts_) {
            if (macro_ast && macro_ast->Evaluate()) {
                spdlog::trace("帧 {}: 宏条件满足，动作 {}", current_frame_, macro_ast->GetAction());
                break;
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

        stats.total_damage = total_damage_ + context.damage_total;
        stats.total_frames = current_frame_;
        stats.total_time   = static_cast<double>(current_frame_) / config_.frame_rate;

        if (stats.total_time > 0) {
            // SkillImpl writes directly into Context::damage_total, while
            // frame_damage_ is reserved for external timeline collectors.
            // Use the finalized total so the public DPS value matches the
            // reported damage value.
            stats.average_dps = static_cast<double>(stats.total_damage) / stats.total_time;
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
    std::vector<std::unique_ptr<AstNode>> macro_asts_;
    bool macro_compiled_ = false;

    // 统计数据
    long long                                   total_damage_ = 0;
    long long                                   frame_damage_ = 0;
    hash_t<jx3id_t, SimulationStats::SkillStat> skill_stats_;
    hash_t<jx3id_t, SimulationStats::BuffStat>  buff_stats_;
    std::vector<SimulationStats::TimelineEvent> timeline_;
};

} // namespace JX3DPS

#endif // JX3DPS_CORE_SIMULATOR_HPP
