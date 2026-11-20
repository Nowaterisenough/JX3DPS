#include "config_json.hpp"
#include "default_macro.hpp"
#include "jx3dps.h"
#ifndef __EMSCRIPTEN__
#    include "src/core/runtime/attribute_gains.hpp"

#    include "profile.hpp"
#endif
#include <atomic>
#include <charconv>
#include <memory>
#include <numeric>

#include <nlohmann/json.hpp>

#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/batch_statistics.hpp"
#include "src/core/runtime/simulation.hpp"
#include "src/core/runtime/skill_totals.hpp"
#include "src/engine/scenarios.hpp"

namespace {
using namespace JX3DPS;
namespace rt             = runtime;
using Json               = nlohmann::json;
constexpr int MaxSeconds = 1800, MaxFights = 100000;

int bounded(const Json &j, const char *name, int lo, int hi)
{
    const auto &v = j.at(name);
    if (!v.is_number_integer() || v < lo || v > hi) {
        throw std::invalid_argument(std::string(name) + " 超出范围");
    }
    return v.get<int>();
}

uint64_t seedOf(const Json &j)
{
    const auto &s = j.at("seed").get_ref<const std::string &>();
    uint64_t    value{};
    auto [end, err] = std::from_chars(s.data(), s.data() + s.size(), value);
    if (s.empty() || err != std::errc{} || end != s.data() + s.size()) {
        throw std::invalid_argument("种子必须是 64 位无符号整数");
    }
    return value;
}

struct Session
{
    virtual ~Session()                            = default;
    virtual Json advance()                        = 0;
    virtual Json debug(const std::string &action) = 0;
};

template <class Config>
class Battle final : public Session
{
    using Rules = typename Config::RuntimeModule::Rules;
    rt::CompiledMacro                            macro;
    Rules                                        rules;
    int                                          seconds, fights;
    uint64_t                                     seed;
    rt::SkillCatalog                             catalog;
    rt::BatchStats                               stats;
    std::vector<rt::SkillTotals>                 skills;
    std::vector<int64_t>                         damage;
    std::unique_ptr<rt::Simulation<Rules>>       batch;
    std::unique_ptr<rt::Simulation<Rules, true>> debugger;

public:
    Battle(const Json &j, const Config &cfg, rt::SkillCatalog cat, bool debug) :
        rules(rt::LoadSpecialization(cfg)), seconds(bounded(j, "seconds", 1, MaxSeconds)),
        fights(bounded(j, "iterations", 1, MaxFights)), seed(seedOf(j)), catalog(cat), skills(cat.skills.size())
    {
        const auto &source = j.at("macro").get_ref<const std::string &>();
        if (source.size() > 64 * 1024) {
            throw std::invalid_argument("宏文本超过 64 KiB");
        }
        auto [compiled, errors] = rt::MacroCompiler::Compile(source, rt::CatalogMacroOptions(cat, debug));
        if (!errors.empty()) {
            throw std::invalid_argument("第 " + std::to_string(errors[0].line) + " 行 · " + errors[0].message);
        }
        if (compiled.program.lines.empty()) {
            throw std::invalid_argument("请至少输入一行施放指令");
        }
        macro            = std::move(compiled);
        const int frames = 16 * seconds, hitCapacity = frames * 8 + 32, mutationCapacity = frames * 24 + 64;
        if (debug) {
            debugger = std::make_unique<rt::Simulation<Rules, true>>(macro.program, rules, hitCapacity, mutationCapacity);
            for (int line : j.at("breakpoints")) {
                for (rt::Pc i = 0; i < macro.source.size(); ++i) {
                    if (macro.source[i].line == static_cast<unsigned>(line)) {
                        debugger->SetBreakpoint(i);
                    }
                }
            }
            debugger->Start(frames, rt::IterationSeed(seed, 0));
        } else {
            batch = std::make_unique<rt::Simulation<Rules>>(macro.program, rules, hitCapacity, mutationCapacity);
            damage.reserve(fights);
        }
    }

    Json advance() override
    {
        if (!batch || stats.count >= static_cast<unsigned>(fights)) {
            throw std::logic_error("没有待执行的批次");
        }
        const auto end = std::min<uint64_t>(stats.count + 128, fights);
        while (stats.count < end) {
            const auto i = stats.count;
            batch->Start(seconds * 16, rt::IterationSeed(seed, i));
            batch->Run();
            auto d = batch->TotalDamage([&](const rt::DamageIntent &hit, value_t amount) {
                skills.at(hit.skill).Add(amount, hit.outcome);
            });
            damage.push_back(d);
            stats.Add(d, i);
        }
        if (stats.count < static_cast<unsigned>(fights)) {
            return {
                { "completed", stats.count }
            };
        }
        Json rows = Json::array();
        for (size_t i = 0; i < skills.size(); ++i) {
            const auto &s = skills[i];
            if (s.count) {
                rows.push_back({
                    { "name",     catalog.skills[i].name   },
                    { "damage",   std::to_string(s.damage) },
                    { "count",    s.count                  },
                    { "critical", s.critical               }
                });
            }
        }
        std::array<uint32_t, 24> bins{};
        for (auto d : damage) {
            const auto range = stats.max_damage - stats.min_damage;
            const auto idx = range ? std::min<size_t>(23, static_cast<size_t>(double(d - stats.min_damage) / range * 24)) : 0;
            ++bins[idx];
        }
        return {
            { "completed", stats.count                           },
            { "meanDps",   stats.mean_damage / seconds           },
            { "minDamage", std::to_string(stats.min_damage)      },
            { "maxDamage", std::to_string(stats.max_damage)      },
            { "sdDps",     stats.PopulationDeviation() / seconds },
            { "checksum",  std::to_string(stats.checksum)        },
            { "skills",    rows                                  },
            { "bins",      bins                                  }
        };
    }

    Json debug(const std::string &action) override
    {
        if (!debugger) {
            throw std::logic_error("没有调试会话");
        }
        if (action == "into") {
            debugger->StepInto();
        } else if (action == "over") {
            debugger->StepOver();
        } else if (action == "continue") {
            debugger->Continue(100000);
        } else if (action != "start") {
            throw std::invalid_argument("未知调试指令");
        }
        const auto &state  = debugger->GetState();
        const auto &cursor = debugger->MacroDebugState();
        const auto  line   = cursor.instruction < macro.source.size() ? macro.source[cursor.instruction].line : 0;
        return {
            { "frame",      state.now                                                           },
            { "line",       line                                                                },
            { "finished",   debugger->Finished()                                                },
            { "breakpoint", debugger->DebugControl().paused                                     },
            { "hits",       debugger->Log().Size()                                              },
            { "qidian",     state.qidian                                                        },
            { "damage",     debugger->Finished() ? std::to_string(debugger->TotalDamage()) : "" }
        };
    }
};
} // namespace

struct jx3dps_engine
{
    std::unique_ptr<Session> active;
    std::string              response;
#ifndef __EMSCRIPTEN__
    JX3DPS::runtime::BatchControl control;
#else
    bool     cancelled = false;
    uint64_t completed = 0;
#endif
};

namespace {
#ifndef __EMSCRIPTEN__
Json statsJson(const rt::BatchStats &s)
{
    return {
        { "count",       s.count                    },
        { "mean_damage", s.mean_damage              },
        { "m2_damage",   s.m2_damage                },
        { "min_damage",  s.min_damage               },
        { "max_damage",  s.max_damage               },
        { "checksum",    std::to_string(s.checksum) }
    };
}

template <class C>
Json nativeBatch(jx3dps_engine &engine, const Json &j, const C &cfg, rt::SkillCatalog catalog)
{
    rt::BatchOptions opts;
    opts.seed     = seedOf(j);
    opts.duration = bounded(j, "frames", 1, std::numeric_limits<tick_t>::max());
    opts.workers  = bounded(j, "workers", 1, 4096);
    if (!j.at("iterations").is_number_unsigned()) {
        throw std::invalid_argument("invalid iterations");
    }
    opts.iterations = j.at("iterations").get<uint64_t>();
    if (!opts.iterations) {
        throw std::invalid_argument("invalid batch dimensions");
    }
    auto capacity = [&](const char *key, size_t fallback) {
        if (!j.contains(key)) {
            return fallback;
        }
        const auto &value = j.at(key);
        if (!value.is_number_unsigned() || value > std::numeric_limits<size_t>::max()) {
            throw std::invalid_argument(std::string("invalid ") + key);
        }
        return value.get<size_t>();
    };
    opts.damage_capacity   = capacity("damageCapacity", opts.damage_capacity);
    opts.mutation_capacity = capacity("mutationCapacity", opts.mutation_capacity);
    auto [macro, errors] = rt::MacroCompiler::Compile(j.at("macro").get<std::string>(), rt::CatalogMacroOptions(catalog, false));
    if (!errors.empty()) {
        throw std::invalid_argument("第 " + std::to_string(errors[0].line) + " 行 · " + errors[0].message);
    }

    const bool                                collect = j.value("collectSkills", true);
    std::vector<std::vector<rt::SkillTotals>> totals(std::min<uint64_t>(opts.workers, opts.iterations),
                                                     std::vector<rt::SkillTotals>(collect ? catalog.skills.size() : 0));
    auto                                      reducer = [&](auto &sim, unsigned worker) {
        if (!collect) {
            return sim.TotalDamage();
        }
        return sim.TotalDamage([&](const rt::DamageIntent &h, value_t amount) {
            totals[worker].at(h.skill).Add(amount, h.outcome);
        });
    };
    engine.control.completed.store(0, std::memory_order_relaxed);
    Json result;
    auto execute = [&]<bool Measure>(rt::BatchProfile *profile) {
        if (j.contains("increments")) {
            auto gains = rt::RunAttributeGainsControlled<C, decltype(reducer), Measure>(
                macro.program,
                opts,
                engine.control,
                cfg,
                reducer,
                j.at("increments").get<rt::AttributeGainIncrements>(),
                profile);
            result["stats"] = statsJson(gains.primary);
            result["gains"] = Json::array();
            for (const auto &g : gains.gains) {
                result["gains"].push_back(g ? statsJson(*g) : Json(nullptr));
            }
        } else {
            auto stats = rt::detail::RunBatchImpl<true, Measure, rt::detail::BatchAssignment::SharedBlocks>(
                macro.program,
                opts,
                rt::LoadSpecialization(cfg),
                &engine.control,
                reducer,
                profile);
            result["stats"] = statsJson(stats);
        }
    };
    if (j.value("hotspots", false)) {
        rt::BatchProfile profile(static_cast<unsigned>(totals.size()));
        execute.template operator()<true>(&profile);
        const auto       summary = JX3DPS::engine::Summarize(profile);
        Json             report  = {
            { "fights",   summary.fights       },
            { "samples",  summary.samplePoints },
            { "workerMs", summary.workerMs     },
            { "phaseUs",  summary.phaseUs      },
            { "workers",  Json::array()        },
            { "calls",    Json::array()        }
        };
        for (const auto &w : summary.workers) {
            report["workers"].push_back({ w.fights, w.samplePoints });
        }
        for (const auto &c : summary.calls) {
            report["calls"].push_back({ c.function, c.parent, c.inclusive, c.self });
        }
        result["profile"] = std::move(report);
    } else {
        execute.template operator()<false>(nullptr);
    }
    result["skills"] = Json::array();
    for (size_t i = 0; collect && i < catalog.skills.size(); ++i) {
        rt::SkillTotals sum;
        for (const auto &worker : totals) {
            sum.Merge(worker[i]);
        }
        if (sum.count) {
            result["skills"].push_back({
                { "skill",    i                          },
                { "damage",   std::to_string(sum.damage) },
                { "count",    sum.count                  },
                { "hit",      sum.hit                    },
                { "critical", sum.critical               },
                { "insight",  sum.insight                },
                { "miss",     sum.miss                   }
            });
        }
    }
    return result;
}
#endif
Json handle(jx3dps_engine &session, const Json &j)
{
    auto      &active = session.active;
    const auto op     = j.at("op").get<std::string>();
#ifndef __EMSCRIPTEN__
    if (op != "batch" && session.control.cancel_requested.load(std::memory_order_relaxed)) {
        throw std::runtime_error("已取消");
    }
#endif
    if (op == "defaults") {
        return {
            { "tx", engine::DefaultMacro(false) },
            { "mw", engine::DefaultMacro(true)  }
        };
    }
    if (op == "next") {
        if (!active) {
            throw std::logic_error("批次已结束");
        }
        return active->advance();
    }
    if (op == "debug") {
        if (!active) {
            throw std::logic_error("调试已结束");
        }
        return active->debug(j.at("action"));
    }
    if (op != "start" && op != "batch") {
        throw std::invalid_argument("未知请求");
    }
    active.reset();
    auto       config = engine::Scenario(j.at("scenario").get<std::string>());
    const bool debug  = j.value("debug", false);
    return std::visit(
        [&](auto &cfg) {
            if (j.contains("config")) {
                j.at("config").get_to(cfg);
            }
            const auto attrs = j.value("attributes", Json::object());
            auto      &a     = cfg.attributes;
            if (attrs.contains("attack")) {
                a.attack_base = bounded(attrs, "attack", 0, 1000000);
            }
            if (attrs.contains("crit")) {
                a.crit = bounded(attrs, "crit", 0, 1000000);
            }
            if (attrs.contains("haste")) {
                a.haste = bounded(attrs, "haste", 0, 1000000);
            }
            using C  = std::decay_t<decltype(cfg)>;
            auto cat = std::is_same_v<C, rt::tai_xu::Config> ? rt::tai_xu::Catalog : rt::mo_wen::Catalog;
#ifndef __EMSCRIPTEN__
            if (op == "batch") return nativeBatch(session, j, cfg, cat);
#else
            if (op == "batch") throw std::invalid_argument("use start/next for cooperative batches");
#endif
            active = std::make_unique<Battle<C>>(j, cfg, cat, debug);
            return debug ? active->debug("start")
                         : Json{
                               { "completed", 0 }
            };
        },
        config);
}
} // namespace

extern "C" uint32_t jx3dps_abi_version(void)
{
    return 1;
}

extern "C" jx3dps_engine *jx3dps_create(void)
{
    try {
        return new jx3dps_engine;
    } catch (...) {
        return nullptr;
    }
}

extern "C" void jx3dps_destroy(jx3dps_engine *session)
{
    delete session;
}

extern "C" void jx3dps_cancel(jx3dps_engine *session)
{
    if (!session) {
        return;
    }
#ifndef __EMSCRIPTEN__
    session->control.cancel_requested.store(true, std::memory_order_relaxed);
#else
    session->cancelled = true;
#endif
}

extern "C" uint64_t jx3dps_completed(const jx3dps_engine *session)
{
    if (!session) {
        return 0;
    }
#ifndef __EMSCRIPTEN__
    return session->control.completed.load(std::memory_order_relaxed);
#else
    return session->completed;
#endif
}

extern "C" const char *jx3dps_call(jx3dps_engine *session, const char *request)
{
    if (!session || !request) {
        return R"({"error":"invalid engine or request"})";
    }
    try {
        try {
            auto j = Json::parse(request);
#ifdef __EMSCRIPTEN__
            if (session->cancelled) {
                throw std::runtime_error("已取消");
            }
#endif
            auto result = handle(*session, j);
            if (result.contains("completed")) {
#ifdef __EMSCRIPTEN__
                session->completed = result.at("completed").get<uint64_t>();
#else
                session->control.completed.store(result.at("completed").get<uint64_t>(), std::memory_order_relaxed);
#endif
            }
            session->response = result.dump();
        } catch (const std::exception &e) {
            session->active.reset();
            session->response = Json{
                { "error", e.what() }
            }.dump(-1, ' ', false, Json::error_handler_t::replace);
        }
        return session->response.c_str();
    } catch (...) {
        return R"({"error":"engine failure"})";
    }
}
