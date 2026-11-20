#include <bit>
#include <cstdint>

#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"

#include "common.hpp"
using namespace JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;

struct BenchmarkPhase
{
    int fixed = -1;

    JX3DPS::tick_t operator()(std::uint64_t seed) const { return fixed < 0 ? tx::NaturalQidianPhase{}(seed) : fixed; }
};
#ifdef RUNTIME_DRAW_REPLAY
struct ReplayRolls
{
    std::shared_ptr<const std::vector<double>> samples;
    std::shared_ptr<const std::vector<int>>    kinds;
    std::shared_ptr<std::size_t>               cursor = std::make_shared<std::size_t>(0);

    void Reset(std::uint64_t) { *cursor = 0; }

    std::uint64_t Next(int kind = 0)
    {
        if (*cursor >= samples->size()) {
            throw std::runtime_error("legacy draw tape exhausted at " + std::to_string(*cursor));
        }
        if ((*kinds)[*cursor] != kind) {
            throw std::runtime_error("legacy draw kind mismatch at " + std::to_string(*cursor));
        }
        return std::bit_cast<std::uint64_t>((*samples)[(*cursor)++]);
    }

    bool Proc(int numerator, int denominator)
    {
        return UnitSample(Next(denominator)) < double(numerator) / denominator;
    }

    static double UnitSample(std::uint64_t key) { return std::bit_cast<double>(key); }
};
#endif

int main(int argc, char **argv)
try {
    const Arguments ARGS(argc, argv);
    const auto      preparation_start = Clock::now();
    tx::Config      config;
    config.attributes.crit          = ARGS.crit;
    config.attributes.overcome_base = 25000;
    config.team_buffs               = tx::team::Parse(ARGS.team_buffs);
    config.initial_qidian           = ARGS.initial_qidian;
    // Legacy Tai Xu's intrinsic PvE multiplier is part of its resolved stats.
    config.attributes.pve_percent   = 184;
    if (ARGS.dot) {
        config.talents = tx::DIE_REN_TALENT | tx::SHEN_MAI;
    }
    if (ARGS.weapon_cw) {
        config.equipment |= tx::WEAPON_CW;
    }
    const std::map<std::string, unsigned> TALENTS = {
        { "wuyi",    tx::WU_YI           },
        { "ruoshui", tx::RUO_SHUI        },
        { "fengshi", tx::FENG_SHI_TALENT },
        { "wuyu",    tx::WU_YU           },
        { "xingu",   tx::XIN_GU          },
        { "tonggen", tx::TONG_GEN        },
        { "huanyue", tx::HUAN_YUE        },
        { "lieyun",  tx::LIE_YUN         },
        { "xuji",    tx::XU_JI           }
    };
    for (const auto &name : ARGS.talents) {
        if (!TALENTS.contains(name)) {
            throw std::invalid_argument("unsupported comparison talent: " + name);
        }
        config.talents |= TALENTS.at(name);
    }
    auto [macro, errors] = MacroCompiler::Compile(ARGS.Macro(), tx::MacroOptions(false));
    if (!errors.empty()) {
        throw std::runtime_error(errors.front().message);
    }
#ifdef RUNTIME_DRAW_REPLAY
    std::ifstream input(ARGS.draws_path);
    if (!input) {
        throw std::runtime_error("cannot read legacy draw tape");
    }
    const auto tape = Json::parse(input);
    if (tape.at("format") != 1) {
        throw std::runtime_error("unsupported draw tape format");
    }
    const ReplayRolls RNG{ std::make_shared<const std::vector<double>>(tape.at("samples").get<std::vector<double>>()),
                           std::make_shared<const std::vector<int>>(tape.at("kinds").get<std::vector<int>>()) };
    if (RNG.samples->size() != RNG.kinds->size()) {
        throw std::runtime_error("invalid draw tape kinds");
    }
    for (double sample : *RNG.samples) {
        if (!(sample >= 0 && sample < 1)) {
            throw std::runtime_error("invalid unit draw");
        }
    }
    using Rules = tx::BasicRules<ReplayRolls, BenchmarkPhase>;
#else
    const tx::SplitMixRolls RNG;
    using Rules = tx::BasicRules<tx::SplitMixRolls, BenchmarkPhase>;
#endif
    Simulation<Rules> sim(macro.program,
                          Rules(tx::Prepare(config), RNG, { ARGS.phase }),
                          (ARGS.seconds * 16 * 8) + 64,
                          (ARGS.seconds * 16 * 24) + 128);
    const double      PREPARATION_S = std::chrono::duration<double>(Clock::now() - preparation_start).count();
    for (int i = 0; i < ARGS.warmup; ++i) {
        sim.Start(ARGS.seconds * 16, IterationSeed(0, i));
        sim.Run();
        (void)sim.TotalDamage();
    }
    Totals     totals;
    const auto start = Clock::now();
    for (int i = 0; i < ARGS.iterations; ++i) {
        sim.Start(ARGS.seconds * 16, IterationSeed(0, i));
        sim.Run();
        totals.Add(sim.TotalDamage());
    }
    const double  ELAPSED = std::chrono::duration<double>(Clock::now() - start).count();
    Json          sample  = Json::object();
    std::ofstream trace;
    if (!ARGS.trace_path.empty()) {
        trace.open(ARGS.trace_path);
        if (!trace) {
            throw std::runtime_error("cannot open trace output");
        }
        trace << "frame,name,damage,outcome,type\n";
    }
    for (const auto &hit : sim.Log().Intents()) {
        const auto NAME = std::string(tx::Skills.entries[hit.skill].name);
        auto      &info = sample[NAME];
        if (info.is_null()) {
            info = {
                { "hits",   0LL },
                { "damage", 0LL }
            };
        }
        const auto DAMAGE = sim.GetRules().Reduce(hit);
        info["hits"]      = info["hits"].get<long long>() + 1;
        info["damage"]    = info["damage"].get<long long>() + DAMAGE;
        if (trace.is_open()) {
            trace << hit.frame << ',' << NAME << ',' << DAMAGE << ',' << int(hit.outcome) << ",3\n";
        }
    }
    if (trace.is_open() && !trace) {
        throw std::runtime_error("trace write failed");
    }
#ifdef RUNTIME_DRAW_REPLAY
    auto result            = totals.Result("new-runtime-draw-replay-validation", ELAPSED, ARGS, sample);
    result["draws"]        = *RNG.cursor;
    result["unused_draws"] = RNG.samples->size() - *RNG.cursor;
    std::cout << result.dump() << '\n';
    if (*RNG.cursor != RNG.samples->size()) {
        throw std::runtime_error("unused legacy draws: " + std::to_string(RNG.samples->size() - *RNG.cursor));
    }
#else
    auto result                       = totals.Result("new-runtime", ELAPSED, ARGS, sample);
    result["preparation_s"]           = PREPARATION_S;
    result["attribute_versions"]      = sim.GetRules().Versions().Size();
    result["attribute_version_bytes"] = sizeof(tx::AttributeVersion);
    result["prepared_bytes"]          = sizeof(tx::Prepared);
    result["damage_intent_bytes"]     = sizeof(DamageIntent);
    std::cout << result.dump() << '\n';
#endif
} catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 1;
}
