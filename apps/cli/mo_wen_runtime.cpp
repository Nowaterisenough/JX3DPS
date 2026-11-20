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

#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"
#include "src/engine/client.hpp"

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace mw = JX3DPS::runtime::mo_wen;

namespace {
constexpr std::string_view ExampleMacro =
    "/cast [tnobuff:shang] shang\n/cast [tnobuff:jue] jue\n"
    "/cast shuying\n/cast gaoshan\n/cast [buff:qufeng=4] yu\n"
    "/cast bianzhi\n/cast biangong";
constexpr std::array<std::pair<std::string_view, unsigned>, 15> Talents{
    { { "haozhong", mw::HAO_ZHONG },
     { "feifan", mw::FEI_FAN },
     { "changqing", mw::CHANG_QING },
     { "xianfeng", mw::XIAN_FENG_TALENT },
     { "liuzhao", mw::LIU_ZHAO_TALENT },
     { "haoqing", mw::HAO_QING },
     { "shixiang", mw::SHI_XIANG },
     { "zhizhi", mw::ZHI_ZHI },
     { "kemeng", mw::KE_MENG },
     { "zhengming", mw::ZHENG_MING },
     { "mingjin", mw::MING_JIN_TALENT },
     { "xiuqi", mw::XIU_QI },
     { "yunhan", mw::YUN_HAN_TALENT },
     { "canlian", mw::CAN_LIAN_TALENT },
     { "zhenglv", mw::ZHENG_LV_HE_MING } }
};
constexpr std::array<std::pair<std::string_view, unsigned>, 7> Equipment{
    { { "weapon_cw", mw::WEAPON_CW },
     { "set_attribute", mw::SET_ATTRIBUTE },
     { "set_skill", mw::SET_SKILL },
     { "weapon_water", mw::WEAPON_WATER },
     { "shoes", mw::ENCHANT_SHOES },
     { "wrist", mw::ENCHANT_WRIST },
     { "belt", mw::ENCHANT_BELT } }
};
constexpr std::array<std::pair<std::string_view, unsigned>, 2> TeamEffects{
    { { "you_ren", mw::YOU_REN }, { "jingmiao", mw::JING_MIAO } }
};
constexpr std::array<std::pair<std::string_view, unsigned>, 8> GongRecipes{
    { { "crit2", mw::GONG_CRIT2 },
     { "crit3", mw::GONG_CRIT3 },
     { "crit4", mw::GONG_CRIT4 },
     { "damage3", mw::GONG_DAMAGE3 },
     { "damage4", mw::GONG_DAMAGE4 },
     { "prepare1", mw::GONG_PREPARE1 },
     { "prepare2", mw::GONG_PREPARE2 },
     { "range", mw::GONG_RANGE } }
};
constexpr std::array<std::pair<std::string_view, unsigned>, 6> ShangRecipes{
    { { "crit2", mw::SHANG_CRIT2 },
     { "crit3", mw::SHANG_CRIT3 },
     { "crit4", mw::SHANG_CRIT4 },
     { "damage3", mw::SHANG_DAMAGE3 },
     { "damage4", mw::SHANG_DAMAGE4 },
     { "damage5", mw::SHANG_DAMAGE5 } }
};
constexpr std::array<std::pair<std::string_view, unsigned>, 6> ZhiRecipes{
    { { "crit2", mw::ZHI_CRIT2 },
     { "crit3", mw::ZHI_CRIT3 },
     { "crit4", mw::ZHI_CRIT4 },
     { "damage3", mw::ZHI_DAMAGE3 },
     { "damage4", mw::ZHI_DAMAGE4 },
     { "range", mw::ZHI_RANGE } }
};
constexpr std::array<std::pair<std::string_view, unsigned>, 4> YuRecipes{
    { { "crit3", mw::YU_CRIT3 }, { "crit4", mw::YU_CRIT4 }, { "damage3", mw::YU_DAMAGE3 }, { "damage4", mw::YU_DAMAGE4 } }
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

void Attribute(mw::Attributes &a, std::string_view text)
{
    const auto EQUALS = text.find('=');
    if (EQUALS == std::string_view::npos) {
        throw std::invalid_argument("attribute syntax is name=integer");
    }
    const auto KEY   = text.substr(0, EQUALS);
    const auto value = Integer<value_t>(text.substr(EQUALS + 1));
    constexpr std::array<std::pair<std::string_view, value_t mw::Attributes::*>, 9> VALUES{
        { { "attack_base", &mw::Attributes::attack_base },
         { "attack_by_class", &mw::Attributes::attack_by_class },
         { "weapon", &mw::Attributes::weapon },
         { "crit", &mw::Attributes::crit },
         { "crit_power", &mw::Attributes::crit_power },
         { "overcome", &mw::Attributes::overcome },
         { "strain", &mw::Attributes::strain },
         { "surplus", &mw::Attributes::surplus },
         { "haste", &mw::Attributes::haste } }
    };
    for (auto [name, field] : VALUES) {
        if (name == KEY && (field != nullptr)) {
            a.*field = value;
            return;
        }
    }
    constexpr std::array<std::pair<std::string_view, int mw::Attributes::*>, 9> PERCENTS{
        { { "attack_percent", &mw::Attributes::attack_percent },
         { "haste_percent", &mw::Attributes::haste_percent },
         { "crit_basis_points", &mw::Attributes::crit_basis_points },
         { "crit_power_percent", &mw::Attributes::crit_power_percent },
         { "damage_percent", &mw::Attributes::damage_percent },
         { "strain_percent", &mw::Attributes::strain_percent },
         { "pve_percent", &mw::Attributes::pve_percent },
         { "ignore_shield", &mw::Attributes::ignore_shield },
         { "overcome_percent", &mw::Attributes::overcome_percent } }
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
    std::cout << "Mo Wen runtime (ba960f7, level 120, single stationary target)\n"
                 "Defaults: example resolved attributes, Gao Shan style, no talents/recipes, 300 seconds.\n\n"
                 "--iterations N --workers N --seconds N --seed N\n"
                 "--macro UTF8_FILE       Otherwise use the included seven-line Gao Shan rotation.\n"
                 "--style gaoshan|yangchun --shadows N --delay FRAMES\n"
                 "--talents LIST          haozhong,feifan,changqing,xianfeng,liuzhao,haoqing,\n"
                 "                        shixiang,zhizhi,kemeng,zhengming,mingjin,xiuqi,\n"
                 "                        yunhan,canlian,zhenglv (or none).\n"
                 "                        changqing/xiuqi have no behavior in the pinned legacy code.\n"
                 "--equipment LIST       weapon_cw,set_attribute,set_skill,weapon_water,shoes,wrist,belt (or none).\n"
                 "--team-effects LIST    you_ren,jingmiao (or none).\n"
                 "--recipes-gong LIST    crit2,crit3,crit4,damage3,damage4,prepare1,prepare2,range\n"
                 "--recipes-shang LIST   crit2,crit3,crit4,damage3,damage4,damage5\n"
                 "--recipes-zhi LIST     crit2,crit3,crit4,damage3,damage4,range\n"
                 "--recipes-yu LIST      crit3,crit4,damage3,damage4\n"
                 "                       At most four recipes per skill; Chinese macro names accepted.\n"
                 "--attribute NAME=INT   Resolved attributes; see docs/mo_wen_runtime_migration.md.\n"
                 "--target-level N --shield N --life DECIMAL --distance DECIMAL --vulnerable N\n"
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
    out << "frame,sequence,report_id,skill_name,effect,sub,level,outcome,snapshot,extra_crit_power,damage,random_key\n";
    for (const auto &hit : sim.Log().Intents()) {
        const auto &skill = mw::Skills.entries[hit.skill];
        out << hit.frame << ',' << hit.sequence << ',' << skill.id << ',' << skill.name << ',' << hit.effect << ','
            << hit.sub << ',' << hit.level << ',' << static_cast<int>(hit.outcome) << ',' << hit.snapshot << ','
            << hit.base << ',' << sim.GetRules().Reduce(hit) << ',' << hit.random_key << '\n';
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
    std::cout << "frame=" << s.now
              << " style=" << (sim.GetRules().CurrentStyle() == mw::Style::GAO_SHAN_LIU_SHUI ? "gaoshan" : "yangchun")
              << " qufeng=" << s.self_buff_stacks[mw::QU_FENG] << " shadows=" << sim.GetRules().ShadowCount()
              << " last_skill=" << (s.last_skill == INVALID_SLOT ? "none" : mw::SkillAliases[s.last_skill])
              << " phase=" << PHASES[static_cast<unsigned>(cursor.phase)] << '\n';
    std::cout << "ready_in_frames:";
    for (const Slot slot : mw::CastableSkills) {
        std::cout << ' ' << mw::SkillAliases[slot] << '=' << std::max(0, s.SkillReadyAt(slot) - s.now);
    }
    std::cout << "\ncharges: yu=" << s.skill_energy[mw::YU] << " shuying=" << s.skill_energy[mw::SHU_YING]
              << " zhi=" << s.skill_energy[mw::ZHI] << '\n';
    if (s.preparing_skill != INVALID_SLOT) {
        std::cout << "preparing=" << mw::SkillAliases[s.preparing_skill]
                  << " remaining_frames=" << s.skill_prepare_at[s.preparing_skill] - s.now << '\n';
    }
    if (s.casting_skill != INVALID_SLOT) {
        std::cout << "channeling=" << mw::SkillAliases[s.casting_skill]
                  << " channel_complete_in=" << s.skill_casting_at[s.casting_skill] - s.now << '\n';
    }
    for (Slot buff = 0; buff < mw::BUFF_COUNT; ++buff) {
        const bool target = mw::IsDot(buff);
        const auto STACKS = target ? s.target_buff_stacks[buff] : s.self_buff_stacks[buff];
        if (!STACKS) {
            continue;
        }
        const auto EXPIRY = target ? s.target_buff_expires_at[buff] : s.self_buff_expires_at[buff];
        std::cout << (target ? "target_" : "self_") << mw::BuffAliases[buff] << '=' << STACKS
                  << " remaining=" << std::max(0, EXPIRY - s.now);
        if (target) {
            std::cout << " next_tick_in=" << std::max(0, s.target_buff_tick_at[buff] - s.now);
        }
        std::cout << '\n';
    }
    for (const auto &shadow : sim.GetRules().Shadows()) {
        if (shadow.expires_at) {
            std::cout << "shadow #" << shadow.order << " expires_in=" << shadow.expires_at - s.now
                      << " next_tick_in=" << shadow.tick_at - s.now << " snapshot=" << shadow.snapshot.profile << '\n';
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

bool Debug(Simulation<mw::Rules, true> &sim, const CompiledMacro &macro)
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
    mw::Config                   config;
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
        } else if (FLAG == "--recipes-gong") {
            config.recipes[0] = Flags(value, GongRecipes);
        } else if (FLAG == "--recipes-shang") {
            config.recipes[1] = Flags(value, ShangRecipes);
        } else if (FLAG == "--recipes-zhi") {
            config.recipes[2] = Flags(value, ZhiRecipes);
        } else if (FLAG == "--recipes-yu") {
            config.recipes[3] = Flags(value, YuRecipes);
        } else if (FLAG == "--style") {
            if (value == "gaoshan") {
                config.initial_style = mw::Style::GAO_SHAN_LIU_SHUI;
            } else if (value == "yangchun") {
                config.initial_style = mw::Style::YANG_CHUN_BAI_XUE;
            } else {
                throw std::invalid_argument("unknown style: " + std::string(value));
            }
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
        } else if (FLAG == "--shadows") {
            config.initial_yingzi = Integer<int>(value);
        } else if (FLAG == "--vulnerable") {
            config.target.vulnerable = Integer<int>(value);
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
    options.damage_capacity   = (static_cast<std::size_t>(options.duration) * 16) + 64;
    options.mutation_capacity = (static_cast<std::size_t>(options.duration) * 40) + 128;
    const auto  RULES         = LoadSpecialization(config);
    const auto &data          = RULES.Data();
    auto [macro, errors]      = MacroCompiler::Compile(macro_text, mw::MacroOptions(debug));
    for (const auto &error : errors) {
        std::cerr << "line " << error.line << ": " << error.message << '\n';
    }
    if (!errors.empty()) {
        return 2;
    }
    std::cout << "ruleset=mo-wen-ba960f7 fps=16 immutable_damage_cache_bytes=" << data.CacheBytes()
              << " seed_base=" << options.seed << '\n';
    const auto start = std::chrono::steady_clock::now();
    BatchStats stats;
    if (debug || !trace_path.empty() || replay) {
        const auto INDEX = replay.value_or(0);
        const auto SEED  = IterationSeed(options.seed, INDEX);
        std::cout << "iteration=" << INDEX << " fight_seed=" << SEED << '\n';
        if (debug) {
            Simulation<mw::Rules, true> sim(macro.program, RULES, options.damage_capacity, options.mutation_capacity);
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
            Simulation<mw::Rules> sim(macro.program, RULES, options.damage_capacity, options.mutation_capacity);
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
