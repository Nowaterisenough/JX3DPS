#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>

#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"
#include "src/engine/client.hpp"

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;

namespace {
constexpr std::string_view ExampleMacro =
    "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [qidian>=8] wuwu\n"
    "/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";
constexpr std::array<std::pair<std::string_view, unsigned>, 19> Talents{
    { { "dieren", tx::DIE_REN_TALENT },
     { "wuyi", tx::WU_YI },
     { "shenmai", tx::SHEN_MAI },
     { "ruoshui", tx::RUO_SHUI },
     { "fengshi", tx::FENG_SHI_TALENT },
     { "wuyu", tx::WU_YU },
     { "huanyue", tx::HUAN_YUE },
     { "qieyu", tx::QIE_YU },
     { "xingu", tx::XIN_GU },
     { "tonggen", tx::TONG_GEN },
     { "fuyin", tx::FU_YIN },
     { "guchang", tx::GU_CHANG },
     { "qisheng", tx::QI_SHENG },
     { "changsheng", tx::CHANG_SHENG },
     { "xuanmen", tx::XUAN_MEN },
     { "lieyun", tx::LIE_YUN },
     { "xuji", tx::XU_JI },
     { "jianru", tx::JIAN_RU },
     { "jinghua", tx::JING_HUA_TALENT } }
};
constexpr std::array<std::pair<std::string_view, unsigned>, 2> Equipment{
    { { "weapon_cw", tx::WEAPON_CW }, { "set_attribute", tx::SET_ATTRIBUTE } }
};
constexpr std::array<std::pair<std::string_view, unsigned>, 1>  TeamEffects{ { { "you_ren", tx::YOU_REN } } };
constexpr std::array<std::pair<std::string_view, unsigned>, 20> Recipes{
    { { "crit2", tx::CRIT2 },
     { "crit3", tx::CRIT3 },
     { "crit4", tx::CRIT4 },
     { "damage3", tx::DAMAGE3 },
     { "damage4", tx::DAMAGE4 },
     { "damage5", tx::DAMAGE5 },
     { "qidian", tx::QIDIAN },
     { "cooldown", tx::COOLDOWN },
     { "prepare1", tx::PREPARE1 },
     { "prepare2", tx::PREPARE2 },
     { "prepare3", tx::PREPARE3 },
     { "range1", tx::RANGE1 },
     { "range2", tx::RANGE2 },
     { "range3", tx::RANGE3 },
     { "damage40", tx::DAMAGE40 },
     { "damage60", tx::DAMAGE60 },
     { "dot", tx::DOT },
     { "yunzhong_suixing", tx::YUN_ZHONG_SUI_XING },
     { "yunzhong_shengtaiji", tx::YUN_ZHONG_SHENG_TAI },
     { "yunzhong_tunriyue", tx::YUN_ZHONG_TUN_RI } }
};

template <typename T>
T Integer(std::string_view text)
{
    T value{};
    auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size()) {
        throw std::invalid_argument("invalid integer: " + std::string(text));
    }
    return value;
}

double Decimal(std::string_view text)
{
    double value{};
    auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size() || !std::isfinite(value)) {
        throw std::invalid_argument("invalid decimal: " + std::string(text));
    }
    return value;
}

template <typename Table>
unsigned Flags(std::string_view text, const Table &table)
{
    unsigned result = 0;
    if (text == "none") {
        return result;
    }
    do {
        const auto COMMA = text.find(',');
        const auto NAME  = text.substr(0, COMMA);
        unsigned   flag  = 0;
        for (auto [key, value] : table) {
            if (key == NAME) {
                flag = value;
            }
        }
        if (!flag) {
            throw std::invalid_argument("unmigrated or unknown option: " + std::string(NAME));
        }
        if (result & flag) {
            throw std::invalid_argument("duplicate option: " + std::string(NAME));
        }
        result |= flag;
        if (COMMA == std::string_view::npos) {
            return result;
        }
        text.remove_prefix(COMMA + 1);
    } while (true);
}

void Attribute(tx::Attributes &a, std::string_view text)
{
    const auto EQUALS = text.find('=');
    if (EQUALS == std::string_view::npos) {
        throw std::invalid_argument("attribute syntax is name=integer");
    }
    const auto KEY   = text.substr(0, EQUALS);
    const auto value = Integer<value_t>(text.substr(EQUALS + 1));
    constexpr std::array<std::pair<std::string_view, value_t tx::Attributes::*>, 11> VALUES{
        { { "attack_base", &tx::Attributes::attack_base },
         { "attack_by_class", &tx::Attributes::attack_by_class },
         { "weapon", &tx::Attributes::weapon },
         { "crit", &tx::Attributes::crit },
         { "crit_power", &tx::Attributes::crit_power },
         { "overcome", &tx::Attributes::overcome },
         { "strain", &tx::Attributes::strain },
         { "surplus", &tx::Attributes::surplus },
         { "haste", &tx::Attributes::haste },
         { "overcome_base", &tx::Attributes::overcome_base },
         { "overcome_by_class", &tx::Attributes::overcome_by_class } }
    };
    for (auto [name, field] : VALUES) {
        if (name == KEY && (field != nullptr)) {
            a.*field = value;
            return;
        }
    }
    constexpr std::array<std::pair<std::string_view, int tx::Attributes::*>, 9> PERCENTS{
        { { "attack_percent", &tx::Attributes::attack_percent },
         { "haste_percent", &tx::Attributes::haste_percent },
         { "crit_basis_points", &tx::Attributes::crit_basis_points },
         { "crit_power_percent", &tx::Attributes::crit_power_percent },
         { "damage_percent", &tx::Attributes::damage_percent },
         { "strain_percent", &tx::Attributes::strain_percent },
         { "pve_percent", &tx::Attributes::pve_percent },
         { "ignore_shield", &tx::Attributes::ignore_shield },
         { "overcome_percent", &tx::Attributes::overcome_percent } }
    };
    for (auto [name, field] : PERCENTS) {
        if (name == KEY && (field != nullptr)) {
            if (value < 0 || value > 10000) {
                throw std::invalid_argument("attribute bonus outside supported range");
            }
            a.*field = static_cast<int>(value);
            return;
        }
    }
    throw std::invalid_argument("unknown resolved attribute: " + std::string(KEY));
}

void Help()
{
    std::cout
        << "Tai Xu core migration (ba960f7, level 120, single stationary target)\n"
           "Defaults are example resolved attributes, no talents/recipes, 300 seconds.\n"
           "Not a complete equipment/talent/game configuration importer.\n\n"
           "--iterations N --workers N --seconds N --seed N\n"
           "--macro UTF8_FILE       Otherwise use the included six-line example macro.\n"
           "--talents LIST          Comma-separated: dieren,wuyi,shenmai,ruoshui,fengshi,\n"
           "                        wuyu,huanyue,qieyu,xingu,tonggen,fuyin,guchang,\n"
           "                        qisheng,changsheng,xuanmen,lieyun,xuji,jianru,jinghua (or none).\n"
           "--equipment LIST       weapon_cw,set_attribute (or none).\n"
           "--team-effects LIST    you_ren (or none).\n"
           "--team-buffs SCHEDULE  name:frame:duration:stacks separated by semicolons; repeatable.\n"
           "--version-capacity N   Maximum observed attribute versions per fight (default 4096).\n"
           "--recipes-wuwu LIST --recipes-bahuang LIST --recipes-sanhuan LIST\n"
           "                        crit2,crit3,crit4,damage3,damage4,damage5,qidian,cooldown\n"
           "--recipes-shengtaiji LIST  prepare1,prepare2,prepare3,cooldown\n"
           "--recipes-renjian LIST  cooldown,range1,range2,range3,damage40,damage60,dot,\n"
           "                         yunzhong_suixing,yunzhong_shengtaiji,yunzhong_tunriyue\n"
           "Field skills: suixing, shengtaiji, tunriyue (Chinese names also accepted).\n"
           "Field attacks: wanjian, renjian, renjian_suixing.\n"
           "--attribute NAME=INT   Repeat for resolved attributes; see migration docs.\n"
           "XuanMen requires overcome_base; optional overcome_by_class/overcome_percent.\n"
           "--target-level N --shield N --life DECIMAL --distance DECIMAL\n"
           "--qidian N --delay FRAMES\n"
           "--trace CSV_FILE       One fight, event-level damage output.\n"
           "--debug                One fight; interactive macro stepping/breakpoints.\n"
           "Debug commands: continue, step, next, break LINE, clear LINE, state, quit.\n"
           "Batch iteration i is reproduced with --seed BASE --replay i.\n";
}

template <typename Engine>
void Trace(Engine &sim, const std::string &path)
{
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        throw std::runtime_error("cannot open trace output");
    }
    out << "frame,sequence,skill_id,skill_name,sub,level,outcome,snapshot,damage,random_key,snapshot_version,live_"
           "version\n";
    for (const auto &hit : sim.Log().Intents()) {
        const auto &skill = tx::Skills.entries[hit.skill];
        out << hit.frame << ',' << hit.sequence << ',' << skill.id << ',' << skill.name << ',' << hit.sub << ',' << hit.level << ','
            << static_cast<int>(hit.outcome) << ',' << hit.snapshot << ',' << sim.GetRules().Reduce(hit, sim.GetState())
            << ',' << hit.random_key << ',' << hit.snapshot_version << ',' << hit.live_version << '\n';
    }
    if (!out) {
        throw std::runtime_error("could not write complete trace");
    }
}

template <typename Engine>
void PrintState(const Engine &sim, const CompiledMacro &macro)
{
    const auto          &s      = sim.GetState();
    const auto          &cursor = sim.MacroDebugState();
    constexpr std::array PHASES{ "line", "condition", "action", "finished" };
    std::cout << "frame=" << s.now << " qidian=" << s.qidian << " charges=" << s.skill_energy[tx::ZI_QI]
              << " dieren=" << s.target_buff_stacks[tx::DIE_REN] << " purple=" << s.self_buff_stacks[tx::PURPLE]
              << " last_skill=" << (s.last_skill == INVALID_SLOT ? "none" : tx::SkillAliases[s.last_skill])
              << " phase=" << PHASES[static_cast<unsigned>(cursor.phase)] << '\n';
    std::cout << "ready_in_frames:";
    for (const Slot slot : tx::CastableSkills) {
        std::cout << ' ' << tx::SkillAliases[slot] << '=';
        if (s.preparing_skill == slot) {
            std::cout << "preparing";
        } else {
            std::cout << std::max(0, s.SkillReadyAt(slot) - s.now);
        }
    }
    std::cout << '\n';
    if (s.preparing_skill != INVALID_SLOT) {
        std::cout << "preparing=" << tx::SkillAliases[s.preparing_skill]
                  << " remaining_frames=" << s.skill_prepare_at[s.preparing_skill] - s.now << '\n';
    }
    for (const Slot BUFF :
         { tx::SUI_XING_BUFF, tx::QI_SHENG_BUFF, tx::CHI_YING_BUFF, tx::XUAN_MEN_BUFF, tx::LIE_YUN_BUFF, tx::JING_HUA_BUFF, tx::WEAPON_CW_BUFF, tx::SET_ATTRIBUTE_BUFF })
    {
        std::cout << tx::BuffAliases[BUFF] << '=' << s.self_buff_stacks[BUFF]
                  << " remaining=" << std::max(0, s.self_buff_expires_at[BUFF] - s.now) << ' ';
    }
    std::cout << "target_tunriyue=" << s.target_buff_stacks[tx::TUN_RI_BUFF] << '\n';
    for (const Slot BUFF : { tx::HIDDEN_LIE_YUN, tx::REN_JIAN_DOT, tx::WAN_XIANG_BUFF }) {
        std::cout << "target_" << tx::BuffAliases[BUFF] << '=' << s.target_buff_stacks[BUFF]
                  << " remaining=" << std::max(0, s.target_buff_expires_at[BUFF] - s.now)
                  << " next_tick_in=" << std::max(0, s.target_buff_tick_at[BUFF] - s.now) << '\n';
    }
    for (const auto &field : sim.GetRules().Fields()) {
        if (field.skill != INVALID_SLOT) {
            std::cout << "field #" << field.order << ' ' << tx::SkillAliases[field.skill]
                      << " expires_in=" << field.expires_at - s.now << " next_tick_in=" << field.tick_at - s.now << '\n';
        }
    }
    if (!sim.Finished() && cursor.instruction < macro.source.size()) {
        std::cout << "line " << macro.source[cursor.instruction].line << ": " << macro.source[cursor.instruction].text << '\n';
        const auto &line = macro.program.lines[cursor.instruction];
        if (cursor.phase == DebugPhase::CONDITION && cursor.condition < line.condition_count) {
            constexpr std::array COMPARISONS{ "<", "<=", "=", "!=", ">", ">=" };
            const auto          &condition = macro.program.conditions[line.condition_begin + cursor.condition];
            std::cout << "condition " << cursor.condition + 1 << ": " << ReadValue(s, condition) << ' '
                      << COMPARISONS[static_cast<unsigned>(condition.comparator)] << ' ' << condition.value << " => "
                      << (EvaluateCondition(s, condition) ? "true" : "false") << " (time values in frames)\n";
        }
    }
}

bool Debug(Simulation<tx::Rules, true> &sim, const CompiledMacro &macro)
{
    PrintState(sim, macro);
    std::string input;
    while (!sim.Finished() && std::cout << "debug> " && std::getline(std::cin, input)) {
        std::istringstream words(input);
        std::string        command;
        words >> command;
        if (command == "quit") {
            return false;
        }
        if (command == "continue") {
            sim.Continue();
        } else if (command == "step") {
            sim.StepInto();
        } else if (command == "next") {
            sim.StepOver();
        } else if (command == "break" || command == "clear") {
            unsigned line = 0;
            words >> line;
            bool found = false;
            for (Pc pc = 0; pc < macro.source.size(); ++pc) {
                if (macro.source[pc].line == line) {
                    sim.SetBreakpoint(pc, command == "break");
                    found = true;
                }
            }
            if (!found) {
                std::cout << "No macro instruction at that source line.\n";
            }
        } else if (command != "state") {
            std::cout << "Commands: continue, step, next, break LINE, clear LINE, state, quit\n";
        }
        PrintState(sim, macro);
        if (sim.DebugControl().budget_exhausted) {
            std::cout << "Step budget reached; continue to resume.\n";
        }
    }
    return sim.Finished();
}
} // namespace

int main(int argc, char *const *argv)
try {
    tx::Config                   config;
    BatchOptions                 options;
    std::string                  macro_text(ExampleMacro);
    std::string                  trace_path;
    bool                         debug = false;
    std::optional<std::uint64_t> replay;
    for (int i = 1; i < argc; ++i) {
        const std::string_view FLAG(argv[i]);
        if (FLAG == "--help") {
            Help();
            return 0;
        }
        if (FLAG == "--debug") {
            debug = true;
            continue;
        }
        if (++i == argc) {
            throw std::invalid_argument("missing value for " + std::string(FLAG));
        }
        const std::string_view value(argv[i]);
        if (FLAG == "--iterations") {
            options.iterations = Integer<std::uint64_t>(value);
        } else if (FLAG == "--workers") {
            options.workers = Integer<unsigned>(value);
        } else if (FLAG == "--seed") {
            options.seed = Integer<std::uint64_t>(value);
        } else if (FLAG == "--replay") {
            replay = Integer<std::uint64_t>(value);
        } else if (FLAG == "--seconds") {
            const auto SECONDS = Decimal(value);
            if (SECONDS <= 0 || SECONDS > 86400 || std::floor(SECONDS * JX3DPS::FRAMES_PER_SECOND) != SECONDS * JX3DPS::FRAMES_PER_SECOND)
            {
                throw std::invalid_argument("duration must be 1..1382400 whole frames (16 FPS)");
            }
            options.duration = static_cast<tick_t>(SECONDS * JX3DPS::FRAMES_PER_SECOND);
        } else if (FLAG == "--macro") {
            std::ifstream input(std::string(value), std::ios::binary);
            if (!input) {
                throw std::invalid_argument("cannot read macro file");
            }
            macro_text.assign(std::istreambuf_iterator<char>(input), {});
            if (macro_text.starts_with("\xef\xbb\xbf")) {
                macro_text.erase(0, 3);
            }
        } else if (FLAG == "--trace") {
            trace_path = value;
        } else if (FLAG == "--talents") {
            config.talents = Flags(value, Talents);
        } else if (FLAG == "--equipment") {
            config.equipment = Flags(value, Equipment);
        } else if (FLAG == "--team-effects") {
            config.team_effects = Flags(value, TeamEffects);
        } else if (FLAG == "--team-buffs") {
            const auto SCHEDULE = tx::team::Parse(value);
            config.team_buffs.insert(config.team_buffs.end(), SCHEDULE.begin(), SCHEDULE.end());
        } else if (FLAG == "--version-capacity") {
            config.attribute_version_capacity = Integer<std::size_t>(value);
        } else if (FLAG == "--recipes-wuwu") {
            config.recipes[tx::WU_WO] = Flags(value, Recipes);
        } else if (FLAG == "--recipes-bahuang") {
            config.recipes[tx::BA_HUANG] = Flags(value, Recipes);
        } else if (FLAG == "--recipes-sanhuan") {
            config.recipes[tx::SAN_HUAN] = Flags(value, Recipes);
        } else if (FLAG == "--recipes-shengtaiji") {
            config.shengtaiji_recipes = Flags(value, Recipes);
        } else if (FLAG == "--recipes-renjian") {
            config.renjian_recipes = Flags(value, Recipes);
        } else if (FLAG == "--attribute") {
            Attribute(config.attributes, value);
        } else if (FLAG == "--target-level") {
            config.target.level = Integer<int>(value);
        } else if (FLAG == "--shield") {
            config.target.shield = Integer<value_t>(value);
        } else if (FLAG == "--life") {
            config.target.life = Decimal(value);
        } else if (FLAG == "--distance") {
            config.target.distance = Decimal(value);
        } else if (FLAG == "--qidian") {
            config.initial_qidian = Integer<int>(value);
        } else if (FLAG == "--delay") {
            config.delay = Integer<tick_t>(value);
        } else {
            throw std::invalid_argument("unsupported option: " + std::string(FLAG));
        }
    }
    if ((options.iterations == 0u) || (options.workers == 0u)) {
        throw std::invalid_argument("iterations and workers must be positive");
    }
    if ((debug || !trace_path.empty() || replay) && (options.iterations != 1 || options.workers != 1)) {
        throw std::invalid_argument("debug, trace and replay require one iteration and one worker");
    }
    options.damage_capacity   = (static_cast<std::size_t>(options.duration) * 8) + 32;
    options.mutation_capacity = (static_cast<std::size_t>(options.duration) * 24) + 64;
    const auto  RULES         = LoadSpecialization(config);
    const auto &data          = RULES.Data();
    auto [macro, errors]      = MacroCompiler::Compile(macro_text, tx::MacroOptions(debug));
    for (const auto &error : errors) {
        std::cerr << "line " << error.line << ": " << error.message << '\n';
    }
    if (!errors.empty()) {
        return 2;
    }
    std::cout << "ruleset=ba960f7-core-subset fps=16 prepared_bytes=" << sizeof(data)
              << " attribute_version_bytes=" << sizeof(tx::AttributeVersion) << " seed_base=" << options.seed << '\n';
    const auto start = std::chrono::steady_clock::now();
    BatchStats stats;
    if (debug || !trace_path.empty() || replay) {
        const auto INDEX = replay.value_or(0);
        const auto SEED  = IterationSeed(options.seed, INDEX);
        std::cout << "iteration=" << INDEX << " fight_seed=" << SEED << '\n';
        if (debug) {
            Simulation<tx::Rules, true> sim(macro.program, RULES, options.damage_capacity, options.mutation_capacity);
            sim.Start(options.duration, SEED);
            if (!Debug(sim, macro)) {
                std::cout << "Debug session ended before fight completion.\n";
                return 0;
            }
            if (!trace_path.empty()) {
                Trace(sim, trace_path);
            }
            stats.Add(sim.TotalDamage(), INDEX);
        } else {
            Simulation<tx::Rules> sim(macro.program, RULES, options.damage_capacity, options.mutation_capacity);
            sim.Start(options.duration, SEED);
            sim.Run();
            if (!trace_path.empty()) {
                Trace(sim, trace_path);
            }
            stats.Add(sim.TotalDamage(), INDEX);
        }
    } else {
        const auto ENGINE = JX3DPS::engine::Create();
        stats             = JX3DPS::engine::Batch(ENGINE.get(), config, macro_text, options).primary;
    }
    const auto   ELAPSED = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    const double SECONDS = JX3DPS::FramesToSeconds(options.duration);
    std::cout << std::fixed << std::setprecision(3) << "fights=" << stats.count << " workers=" << options.workers
              << " mean_dps=" << stats.mean_damage / SECONDS << " dps_sd=" << stats.PopulationDeviation() / SECONDS
              << " min_damage=" << stats.min_damage << " max_damage=" << stats.max_damage
              << " checksum=" << stats.checksum << " elapsed_s=" << ELAPSED << '\n';
    return 0;
} catch (const std::exception &error) {
    std::cerr << "error: " << error.what() << '\n';
    return 1;
}
