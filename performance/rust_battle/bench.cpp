#include <windows.h>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"

#include "bridge.hpp"
#include "observation.hpp"

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace tx = tai_xu;
namespace mw = mo_wen;

struct API
{
    decltype(&rust_new)    create;
    decltype(&rust_delete) destroy;
    Arm                    arm;
    Cancel                 cancel;
    decltype(&rust_run)    run;
};

constexpr API rust{ rust_new, rust_delete, rust_arm, rust_cancel, rust_run };
constexpr API control{ cpp_new, cpp_delete, cpp_arm, cpp_cancel, cpp_run };

struct TimerPort
{
    const API               &api;
    void                    *driver = nullptr;
    std::vector<TimerHandle> handles;
    std::vector<uint16_t>    priorities;

    uint32_t Create(TimerHandle h, uint16_t priority = 0)
    {
        handles.push_back(h);
        priorities.push_back(priority);
        return static_cast<uint32_t>(handles.size() - 1);
    }

    void ArmAt(uint32_t id, int32_t at) { api.arm(driver, id, at); }

    void Cancel(uint32_t id) { api.cancel(driver, id); }
};

template <class Rules>
class SharedRulesEngine
{
    const MacroProgram        &program;
    Rules                      rules;
    TimerPort                  port;
    State                      state, initial;
    DamageIntentLog            log{ 4096, 8192 };
    std::vector<LineView>      lines;
    std::vector<ConditionView> conditions;

    static SharedRulesEngine &self(void *p) { return *static_cast<SharedRulesEngine *>(p); }

    static constexpr Hooks hooks{
        [](void *p, uint64_t seed) {
            auto &e = self(p);
            e.state.Reset();
            e.log.Clear();
            e.rules.Reset(e, seed);
            e.initial = e.state;
            e.log.Clear();
        },
        [](void *p, int32_t frame) { self(p).state.now = frame; },
        [](void *p, uint32_t id) {
            auto &e = self(p);
            e.rules.OnTimer(e, e.port.handles.at(id), id);
        },
        [](void *p) {
            const auto &s = self(p).state;
            return s.preparing_skill != INVALID_SLOT || s.casting_skill != INVALID_SLOT;
        },
        [](void *p, uint32_t i) {
            auto       &e    = self(p);
            const auto &line = e.program.lines[i];
            if constexpr (requires { e.rules.BeforeMacroLine(e, line); }) {
                e.rules.BeforeMacroLine(e, line);
            }
            return e.rules.CanAttempt(e.state, line);
        },
        [](void *p, uint32_t i) {
            auto &e = self(p);
            return ReadValue(e.state, e.program.conditions[i]);
        },
        [](void *p, uint32_t i) {
            auto &e = self(p);
            return e.rules.Cast(e, e.program.lines[i]);
        },
        [](void *p) {
            auto &e = self(p);
            return NextMacroBoundary(e.program, e.state).value_or(INT32_MAX);
        }
    };

public:
    static constexpr bool debug_enabled = false;

    SharedRulesEngine(const MacroProgram &prog, Rules input, const API &api) :
        program(prog), rules(std::move(input)), port{ api }
    {
        state.Resize(prog.skill_count, prog.buff_count);
        initial = state;
        rules.Setup(*this);
        for (const auto &l : prog.lines) {
            lines.push_back({ l.condition_begin, l.condition_count, static_cast<uint8_t>(IsSequential(l.command)) });
        }
        for (const auto &c : prog.conditions) {
            conditions.push_back({ c.value, c.on_true, c.on_false, static_cast<uint8_t>(c.comparator) });
        }
        const bool        BUSY       = std::any_of(prog.lines.begin(), prog.lines.end(), Rules::MayActWhileBusy);
        const bool        BOUNDARIES = std::any_of(prog.conditions.begin(), prog.conditions.end(), [](auto &c) {
            return c.kind == ValueKind::SKILL_COOLDOWN || c.kind == ValueKind::BUFF_DURATION;
        });
        const ProgramView VIEW{ lines.data(), conditions.data(), lines.size(), conditions.size(), BUSY, BOUNDARIES };
        port.driver = api.create(port.priorities.data(), port.priorities.size(), &VIEW);
    }

    ~SharedRulesEngine() { port.api.destroy(port.driver); }

    SharedRulesEngine(const SharedRulesEngine &) = delete;

    State &MutableState() { return state; }

    const State &GetState() const { return state; }

    DamageIntentLog &Log() { return log; }

    TimerPort &Timers() { return port; }

    const Rules &GetRules() const { return rules; }

    void Fight(uint64_t seed) { port.api.run(port.driver, this, &hooks, seed, 4800); }

    template <class F>
    value_t TotalDamage(F observe)
    {
        value_t total = 0;
        for (const auto &hit : log.Intents()) {
            const auto DAMAGE = rules.Reduce(hit);
            observe(hit, DAMAGE);
            total += DAMAGE;
        }
        return total;
    }
};

using namespace battle_bench;

template <class Rules>
struct Traced : Rules
{
    Audit           *audit;
    const MacroLine *first;

    template <class Engine>
    bool Cast(Engine &e, const MacroLine &line)
    {
        if (!Rules::Cast(e, line)) {
            return false;
        }
        audit->write('C', e.GetState().now, uint32_t(&line - first), line.action_slot, line.command);
        return true;
    }

    template <class Engine>
    void OnTimer(Engine &e, TimerHandle h, uint32_t id)
    {
        audit->write('T', e.GetState().now, id, h.kind, h.slot, h.target);
        Rules::OnTimer(e, h, id);
    }
};

template <bool Trace, class Rules>
void execute(const std::string &mode, const MacroProgram &prog, Rules rules, uint32_t n, unsigned threads, Audit &audit)
{
    std::vector<Result> results(threads);
    for (auto &result : results) {
        result.skills.resize(prog.skill_count);
    }
    const auto WORKER = [&](unsigned w) {
        auto      &r    = results[w];
        const auto LOOP = [&](auto &sim) {
            for (uint64_t i = uint64_t(n) * w / threads; i < uint64_t(n) * (w + 1) / threads; ++i) {
                auto seed = IterationSeed(42, i);
                if constexpr (Trace) {
                    audit.write('F', i);
                }
                if constexpr (requires { sim.Fight(seed); }) {
                    sim.Fight(seed);
                } else {
                    sim.Start(4800, seed);
                    sim.Run();
                }
                const auto DAMAGE =
                    sim.TotalDamage([&](const DamageIntent &h, value_t d) { r.skills.at(h.skill).add(d, h.outcome); });
                r.batch.Add(DAMAGE, i);
                if constexpr (Trace) {
                    dump(sim, audit);
                }
            }
        };
        if (mode == "native") {
            Simulation<Rules> sim(prog, rules);
            LOOP(sim);
        } else {
            SharedRulesEngine<Rules> sim(prog, rules, mode == "rust" ? rust : control);
            LOOP(sim);
        }
    };
    const auto start = std::chrono::steady_clock::now();
    BatchStats batch;
    if (!Trace && mode == "native") {
        BatchOptions options;
        options.iterations = n;
        options.seed       = 42;
        options.duration   = 4800;
        options.workers    = threads;
        BatchControl control;
        const auto   REDUCE = [&](auto &sim, unsigned w) {
            return sim.TotalDamage([&](const DamageIntent &h, value_t d) {
                results[w].skills.at(h.skill).add(d, h.outcome);
            });
        };
        batch = detail::RunBatchImpl<true, false, detail::BatchAssignment::SHARED_BLOCKS>(prog, options, rules, &control, REDUCE);
    } else {
        std::vector<std::jthread> pool;
        for (unsigned w = 1; w < threads; ++w) {
            pool.emplace_back(WORKER, w);
        }
        WORKER(0);
    }
    std::vector<Skill> merged(prog.skill_count);
    for (auto &r : results) {
        batch.Merge(r.batch);
        for (size_t i = 0; i < merged.size(); ++i) {
            merged[i].damage += r.skills[i].damage;
            for (size_t j = 0; j < 4; ++j) {
                merged[i].outcomes[j] += r.skills[i].outcomes[j];
            }
        }
    }
    const double SEC = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    std::cout << SEC << ' ' << batch.count << ' ' << batch.checksum;
    for (auto &s : merged) {
        std::cout << ' ' << s.damage;
        for (const auto K : s.outcomes) {
            std::cout << ',' << K;
        }
    }
    std::cout << '\n';
}

template <class Config>
void scene(const Config &cfg, const CompileOptions &options, std::string macro, const std::string &mode, uint32_t n, unsigned threads, const char *trace)
{
    auto [compiled, errors] = MacroCompiler::Compile(macro, options);
    if (!errors.empty()) {
        throw std::runtime_error(errors.front().message);
    }
    const auto RULES = LoadSpecialization(cfg);
    Audit      audit;
    if (trace) {
        audit.stream.exceptions(std::ios::badbit | std::ios::failbit);
        audit.stream.open(trace, std::ios::binary);
        const Traced<decltype(RULES)> TRACED{ RULES, &audit, compiled.program.lines.data() };
        execute<true>(mode, compiled.program, TRACED, n, threads, audit);
    } else {
        execute<false>(mode, compiled.program, RULES, n, threads, audit);
    }
}

int main(int argc, char *const *argv)
{
    if (argc < 5 || argc > 7) {
        return 2;
    }
    const std::string MODE    = argv[1];
    const std::string WHICH   = argv[2];
    unsigned          n       = std::stoul(argv[3]);
    unsigned          workers = std::stoul(argv[4]);
    if ((MODE != "native" && MODE != "rust" && MODE != "control") || (workers == 0u) || workers > n || (argc >= 6 && workers != 1))
    {
        return 2;
    }
    const char *trace = argc >= 6 ? argv[5] : nullptr;
    if (workers == 1 && (SetThreadAffinityMask(GetCurrentThread(), 4) == 0u)) {
        return 3;
    }
    std::string custom;
    if (argc == 7) {
        std::ifstream f(argv[6]);
        if (!f) {
            return 2;
        }
        custom.assign(std::istreambuf_iterator<char>(f), {});
    }
    if (WHICH == "tx" || WHICH == "tx_buffs") {
        tx::Config t;
        if (WHICH == "tx_buffs") {
            t.talents   = tx::DIE_REN_TALENT | tx::WU_YI | tx::SHEN_MAI | tx::XUAN_MEN | tx::LIE_YUN | tx::GU_CHANG;
            t.equipment = tx::WEAPON_CW | tx::SET_ATTRIBUTE;
            t.attributes.overcome_base     = 20000;
            t.attributes.overcome_by_class = 5000;
            t.team_buffs                   = {
                { tx::team::JI_LEI,           0,  96,  1  },
                { tx::team::NONG_MEI,         48, 96,  1  },
                { tx::team::HAO_LING_SAN_JUN, 16, 0,   48 },
                { tx::team::PO_FENG,          32, 160, 1  }
            };
        }
        scene(t,
              tx::MacroOptions(false),
              argc == 7 ? custom : "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [qidian>=8] wuwu\n/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu",
              MODE,
              n,
              workers,
              trace);
    } else if (WHICH == "mw" || WHICH == "mw_equipped") {
        mw::Config m;
        if (WHICH == "mw_equipped") {
            m.talents = mw::HAO_ZHONG | mw::FEI_FAN | mw::SHI_XIANG | mw::ZHENG_LV_HE_MING | mw::LIU_ZHAO_TALENT | mw::KE_MENG;
            m.equipment = mw::ENCHANT_SHOES | mw::ENCHANT_WRIST | mw::ENCHANT_BELT | mw::SET_ATTRIBUTE | mw::SET_SKILL |
                          mw::WEAPON_CW | mw::WEAPON_WATER;
            m.team_effects = mw::YOU_REN | mw::JING_MIAO;
        }
        scene(m,
              mw::MacroOptions(false),
              argc == 7 ? custom : "/cast [tnobuff:shang] shang\n/cast [tnobuff:jue] jue\n/cast shuying\n/cast gaoshan\n/cast [buff:qufeng=4] yu\n/cast bianzhi\n/cast biangong",
              MODE,
              n,
              workers,
              trace);
    } else {
        return 2;
    }
}
