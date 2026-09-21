#include "common.hpp"
#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"
#include <bit>
using namespace JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;
struct BenchmarkPhase {
    int fixed = -1;
    JX3DPS::tick_t operator()(std::uint64_t seed) const { return fixed < 0 ? tx::NaturalQidianPhase{}(seed) : fixed; }
};
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
    bool Proc(int numerator, int denominator) { return UnitSample(Next(denominator)) < double(numerator) / denominator; }
    static double UnitSample(std::uint64_t key) { return std::bit_cast<double>(key); }
};
#endif

int main(int argc, char **argv) try {
    Arguments args(argc, argv);
    const auto preparation_start = Clock::now();
    tx::Config config;
    config.attributes.crit = args.crit;
    config.attributes.overcome_base = 25000;
    config.team_buffs = tx::team::Parse(args.team_buffs);
    config.initial_qidian = args.initial_qidian;
    // Legacy Tai Xu's intrinsic PvE multiplier is part of its resolved stats.
    config.attributes.pve_percent = 184;
    if (args.dot) config.talents = tx::DieRenTalent | tx::ShenMai;
    if (args.weapon_cw) config.equipment |= tx::WeaponCW;
    const std::map<std::string, unsigned> talents = {
        {"wuyi", tx::WuYi}, {"ruoshui", tx::RuoShui}, {"fengshi", tx::FengShiTalent},
        {"wuyu", tx::WuYu}, {"xingu", tx::XinGu}, {"tonggen", tx::TongGen},
        {"huanyue", tx::HuanYue}, {"lieyun", tx::LieYun}, {"xuji", tx::XuJi}
    };
    for (const auto &name : args.talents) {
        if (!talents.contains(name)) throw std::invalid_argument("unsupported comparison talent: " + name);
        config.talents |= talents.at(name);
    }
    auto [macro, errors] = MacroCompiler::Compile(args.Macro(), tx::MacroOptions(false));
    if (!errors.empty()) throw std::runtime_error(errors.front().message);
#ifdef RUNTIME_DRAW_REPLAY
    std::ifstream input(args.draws_path);
    if (!input) throw std::runtime_error("cannot read legacy draw tape");
    const auto tape = Json::parse(input);
    if (tape.at("format") != 1) throw std::runtime_error("unsupported draw tape format");
    ReplayRolls rng{std::make_shared<const std::vector<double>>(tape.at("samples").get<std::vector<double>>()),
        std::make_shared<const std::vector<int>>(tape.at("kinds").get<std::vector<int>>())};
    if (rng.samples->size() != rng.kinds->size()) throw std::runtime_error("invalid draw tape kinds");
    for (double sample : *rng.samples)
        if (!(sample >= 0 && sample < 1)) throw std::runtime_error("invalid unit draw");
    using Rules = tx::BasicRules<ReplayRolls, BenchmarkPhase>;
#else
    tx::SplitMixRolls rng;
    using Rules = tx::BasicRules<tx::SplitMixRolls, BenchmarkPhase>;
#endif
    Simulation<Rules> sim(macro.program, Rules(tx::Prepare(config), rng, {args.phase}), args.seconds*16*8+64, args.seconds*16*24+128);
    const double preparation_s = std::chrono::duration<double>(Clock::now()-preparation_start).count();
    for (int i = 0; i < args.warmup; ++i) { sim.Start(args.seconds*16, IterationSeed(0, i)); sim.Run(); (void)sim.TotalDamage(); }
    Totals totals;
    const auto start = Clock::now();
    for (int i = 0; i < args.iterations; ++i) {
        sim.Start(args.seconds*16, IterationSeed(0, i)); sim.Run(); totals.Add(sim.TotalDamage());
    }
    const double elapsed = std::chrono::duration<double>(Clock::now() - start).count();
    Json sample = Json::object();
    std::ofstream trace;
    if (!args.trace_path.empty()) {
        trace.open(args.trace_path);
        if (!trace) throw std::runtime_error("cannot open trace output");
        trace << "frame,name,damage,outcome,type\n";
    }
    for (const auto &hit : sim.Log().Intents()) {
        const auto name = std::string(tx::Skills.entries[hit.skill].name);
        auto &info = sample[name];
        if (info.is_null()) info = {{"hits", 0LL}, {"damage", 0LL}};
        const auto damage = sim.GetRules().Reduce(hit);
        info["hits"] = info["hits"].get<long long>() + 1;
        info["damage"] = info["damage"].get<long long>() + damage;
        if (trace.is_open()) trace << hit.frame << ',' << name << ',' << damage << ',' << int(hit.outcome) << ",3\n";
    }
    if (trace.is_open() && !trace) throw std::runtime_error("trace write failed");
#ifdef RUNTIME_DRAW_REPLAY
    auto result = totals.Result("new-runtime-draw-replay-validation", elapsed, args, sample);
    result["draws"] = *rng.cursor;
    result["unused_draws"] = rng.samples->size() - *rng.cursor;
    std::cout << result.dump() << '\n';
    if (*rng.cursor != rng.samples->size()) throw std::runtime_error("unused legacy draws: " + std::to_string(rng.samples->size() - *rng.cursor));
#else
    auto result = totals.Result("new-runtime", elapsed, args, sample);
    result["preparation_s"] = preparation_s;
    result["attribute_versions"] = sim.GetRules().Versions().Size();
    result["attribute_version_bytes"] = sizeof(tx::AttributeVersion);
    result["prepared_bytes"] = sizeof(tx::Prepared);
    result["damage_intent_bytes"] = sizeof(DamageIntent);
    std::cout << result.dump() << '\n';
#endif
} catch (const std::exception &error) { std::cerr << error.what() << '\n'; return 1; }
