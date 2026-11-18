#include "common.hpp"
#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"
#include <bit>
using namespace JX3DPS::runtime;
namespace mw = JX3DPS::runtime::mo_wen;
#ifdef RUNTIME_DRAW_REPLAY
struct ReplayRolls {
    std::shared_ptr<const std::vector<double>> samples;
    std::shared_ptr<const std::vector<int>> kinds;
    std::shared_ptr<std::size_t> cursor = std::make_shared<std::size_t>(0);
    void Reset(std::uint64_t) { *cursor = 0; }
    std::uint64_t Next(int kind = 0) {
        if (*cursor >= samples->size()) throw std::runtime_error("legacy draw tape exhausted at " + std::to_string(*cursor));
        if ((*kinds)[*cursor] != kind) throw std::runtime_error("legacy draw kind mismatch at " + std::to_string(*cursor));
        return std::bit_cast<std::uint64_t>((*samples)[(*cursor)++]);
    }
    int Integer(int low, int high) {
        const auto sample=UnitSample(Next(low==1 && high==1024 ? 1024 : -1-low*1024-high));
        return low==1 && high==1024 ? static_cast<int>(sample*1024)+1 : static_cast<int>(sample);
    }
    static double UnitSample(std::uint64_t key) { return std::bit_cast<double>(key); }
};
#endif

int main(int argc, char **argv) try {
    Arguments args(argc, argv);
    mw::Config config;
    config.attributes.crit = args.crit;
    config.attributes.pve_percent = 61;
    if (args.weapon_cw) config.equipment |= mw::WeaponCW;
    const std::map<std::string, unsigned> talents = {
        {"haozhong", mw::HaoZhong},
        {"feifan", mw::FeiFan},
        {"xianfeng", mw::XianFengTalent},
        {"liuzhao", mw::LiuZhaoTalent},
        {"haoqing", mw::HaoQing},
        {"shixiang", mw::ShiXiang},
        {"zhizhi", mw::ZhiZhi},
        {"kemeng", mw::KeMeng},
        {"zhengming", mw::ZhengMing},
        {"mingjin", mw::MingJinTalent},
        {"yunhan", mw::YunHanTalent},
        {"canlian", mw::CanLianTalent},
        {"zhenglv", mw::ZhengLvHeMing}
    };
    for (const auto &name : args.talents) {
        if (!talents.contains(name)) throw std::invalid_argument("unsupported comparison talent: " + name);
        config.talents |= talents.at(name);
    }
    auto [macro, errors] = MacroCompiler::Compile(args.Macro(), mw::MacroOptions(false));
    if (!errors.empty()) throw std::runtime_error(errors.front().message);
#ifdef RUNTIME_DRAW_REPLAY
    std::ifstream input(args.draws_path);
    if (!input) throw std::runtime_error("cannot read legacy draw tape");
    const auto tape = Json::parse(input);
    if (tape.at("format") != 1) throw std::runtime_error("unsupported draw tape format");
    ReplayRolls rng{std::make_shared<const std::vector<double>>(tape.at("samples").get<std::vector<double>>()),
        std::make_shared<const std::vector<int>>(tape.at("kinds").get<std::vector<int>>())};
    if (rng.samples->size() != rng.kinds->size()) throw std::runtime_error("invalid draw tape kinds");
    for (std::size_t i=0; i<rng.samples->size(); ++i)
        if ((*rng.kinds)[i]>=0 && !((*rng.samples)[i]>=0 && (*rng.samples)[i]<1)) throw std::runtime_error("invalid unit draw");
    using Rules = mw::BasicRules<ReplayRolls>;
#else
    mw::SplitMixRolls rng;
    using Rules = mw::BasicRules<mw::SplitMixRolls>;
#endif
    Simulation<Rules> sim(macro.program, Rules(mw::Prepare(config), rng), args.seconds*16*16+64, args.seconds*16*40+128);
    for (int i = 0; i < args.warmup; ++i) { sim.Start(args.seconds*16, IterationSeed(0, i)); sim.Run(); (void)sim.TotalDamage(); }
    Totals totals;
    const auto start = Clock::now();
    std::exception_ptr failure;
    try {
        for (int i = 0; i < args.iterations; ++i) {
            sim.Start(args.seconds*16, IterationSeed(0, i)); sim.Run(); totals.Add(sim.TotalDamage());
        }
    } catch (...) { failure=std::current_exception(); }
    const double elapsed = std::chrono::duration<double>(Clock::now() - start).count();
    Json sample = Json::object();
    std::ofstream trace;
    if (!args.trace_path.empty()) {
        trace.open(args.trace_path);
        if (!trace) throw std::runtime_error("cannot open trace output");
        trace << "frame,name,damage,outcome,type\n";
    }
    for (const auto &hit : sim.Log().Intents()) {
        const auto name = std::string(mw::Skills.entries[hit.skill].name);
        auto &info = sample[name];
        if (info.is_null()) info = {{"hits", 0LL}, {"damage", 0LL}};
        const auto damage = sim.GetRules().Reduce(hit);
        info["hits"] = info["hits"].get<long long>() + 1;
        info["damage"] = info["damage"].get<long long>() + damage;
        if (trace.is_open()) trace << hit.frame << ',' << name << ',' << damage << ',' << int(hit.outcome) << ",3\n";
    }
    if (trace.is_open() && !trace) throw std::runtime_error("trace write failed");
    if (failure) std::rethrow_exception(failure);
#ifdef RUNTIME_DRAW_REPLAY
    auto result = totals.Result("mo-wen-runtime-draw-replay", elapsed, args, sample);
    result["draws"] = *rng.cursor;
    result["unused_draws"] = rng.samples->size() - *rng.cursor;
    std::cout << result.dump() << '\n';
    if (*rng.cursor != rng.samples->size()) throw std::runtime_error("unused legacy draws: " + std::to_string(rng.samples->size() - *rng.cursor));
#else
    std::cout << totals.Result("mo-wen-runtime", elapsed, args, sample).dump() << '\n';
#endif
} catch (const std::exception &error) { std::cerr << error.what() << '\n'; return 1; }
