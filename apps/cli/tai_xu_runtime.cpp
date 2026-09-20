#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"
#include <charconv>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;

namespace {
constexpr std::string_view ExampleMacro =
    "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [qidian>=8] wuwu\n"
    "/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";
constexpr std::array<std::pair<std::string_view, unsigned>, 19> Talents{{
    {"dieren", tx::DieRenTalent}, {"wuyi", tx::WuYi}, {"shenmai", tx::ShenMai}, {"ruoshui", tx::RuoShui},
    {"fengshi", tx::FengShiTalent}, {"wuyu", tx::WuYu}, {"huanyue", tx::HuanYue}, {"qieyu", tx::QieYu},
    {"xingu", tx::XinGu}, {"tonggen", tx::TongGen}, {"fuyin", tx::FuYin},
    {"guchang", tx::GuChang}, {"qisheng", tx::QiSheng}, {"changsheng", tx::ChangSheng},
    {"xuanmen", tx::XuanMen}, {"lieyun", tx::LieYun}, {"xuji", tx::XuJi}, {"jianru", tx::JianRu}, {"jinghua", tx::JingHuaTalent}
}};
constexpr std::array<std::pair<std::string_view, unsigned>, 2> Equipment{{
    {"weapon_cw", tx::WeaponCW}, {"set_attribute", tx::SetAttribute}
}};
constexpr std::array<std::pair<std::string_view, unsigned>, 1> TeamEffects{{
    {"you_ren", tx::YouRen}
}};
constexpr std::array<std::pair<std::string_view, unsigned>, 20> Recipes{{
    {"crit2", tx::Crit2}, {"crit3", tx::Crit3}, {"crit4", tx::Crit4},
    {"damage3", tx::Damage3}, {"damage4", tx::Damage4}, {"damage5", tx::Damage5},
    {"qidian", tx::Qidian}, {"cooldown", tx::Cooldown},
    {"prepare1", tx::Prepare1}, {"prepare2", tx::Prepare2}, {"prepare3", tx::Prepare3},
    {"range1", tx::Range1}, {"range2", tx::Range2}, {"range3", tx::Range3},
    {"damage40", tx::Damage40}, {"damage60", tx::Damage60}, {"dot", tx::Dot},
    {"yunzhong_suixing", tx::YunZhongSuiXing}, {"yunzhong_shengtaiji", tx::YunZhongShengTai},
    {"yunzhong_tunriyue", tx::YunZhongTunRi}
}};
template <typename T> T Integer(std::string_view text) {
    T value{};
    auto [end, error] = std::from_chars(text.data(), text.data()+text.size(), value);
    if (error != std::errc{} || end != text.data()+text.size()) throw std::invalid_argument("invalid integer: " + std::string(text));
    return value;
}
double Decimal(std::string_view text) {
    double value{};
    auto [end, error] = std::from_chars(text.data(), text.data()+text.size(), value);
    if (error != std::errc{} || end != text.data()+text.size() || !std::isfinite(value))
        throw std::invalid_argument("invalid decimal: " + std::string(text));
    return value;
}
template <typename Table> unsigned Flags(std::string_view text, const Table &table) {
    unsigned result = 0;
    if (text == "none") return result;
    do {
        const auto comma = text.find(',');
        auto name = text.substr(0, comma);
        unsigned flag = 0;
        for (auto [key, value] : table) if (key == name) flag = value;
        if (!flag) throw std::invalid_argument("unmigrated or unknown option: " + std::string(name));
        if (result & flag) throw std::invalid_argument("duplicate option: " + std::string(name));
        result |= flag;
        if (comma == text.npos) return result;
        text.remove_prefix(comma+1);
    } while (true);
}
void Attribute(tx::Attributes &a, std::string_view text) {
    const auto equals = text.find('=');
    if (equals == text.npos) throw std::invalid_argument("attribute syntax is name=integer");
    const auto key = text.substr(0, equals);
    const auto value = Integer<value_t>(text.substr(equals+1));
    constexpr std::array<std::pair<std::string_view, value_t tx::Attributes::*>, 11> values{{
        {"attack_base", &tx::Attributes::attack_base}, {"attack_by_class", &tx::Attributes::attack_by_class},
        {"weapon", &tx::Attributes::weapon}, {"crit", &tx::Attributes::crit}, {"crit_power", &tx::Attributes::crit_power},
        {"overcome", &tx::Attributes::overcome}, {"strain", &tx::Attributes::strain},
        {"surplus", &tx::Attributes::surplus}, {"haste", &tx::Attributes::haste},
        {"overcome_base", &tx::Attributes::overcome_base}, {"overcome_by_class", &tx::Attributes::overcome_by_class}
    }};
    for (auto [name, field] : values) if (name == key && field) { a.*field = value; return; }
    constexpr std::array<std::pair<std::string_view, int tx::Attributes::*>, 9> percents{{
        {"attack_percent", &tx::Attributes::attack_percent}, {"haste_percent", &tx::Attributes::haste_percent},
        {"crit_basis_points", &tx::Attributes::crit_basis_points}, {"crit_power_percent", &tx::Attributes::crit_power_percent},
        {"damage_percent", &tx::Attributes::damage_percent}, {"strain_percent", &tx::Attributes::strain_percent},
        {"pve_percent", &tx::Attributes::pve_percent}, {"ignore_shield", &tx::Attributes::ignore_shield},
        {"overcome_percent", &tx::Attributes::overcome_percent}
    }};
    for (auto [name, field] : percents) if (name == key && field) {
        if (value < 0 || value > 10000) throw std::invalid_argument("attribute bonus outside supported range");
        a.*field = static_cast<int>(value); return;
    }
    throw std::invalid_argument("unknown resolved attribute: " + std::string(key));
}
void Help() {
    std::cout << "Tai Xu core migration (ba960f7, level 120, single stationary target)\n"
        "Defaults are example resolved attributes, no talents/recipes, 300 seconds.\n"
        "Not a complete equipment/talent/game configuration importer.\n\n"
        "--iterations N --workers N --seconds N --seed N\n"
        "--macro UTF8_FILE       Otherwise use the included six-line example macro.\n"
        "--talents LIST          Comma-separated: dieren,wuyi,shenmai,ruoshui,fengshi,\n"
        "                        wuyu,huanyue,qieyu,xingu,tonggen,fuyin,guchang,\n"
        "                        qisheng,changsheng,xuanmen,lieyun,xuji,jianru,jinghua (or none).\n"
        "--equipment LIST       weapon_cw,set_attribute (or none).\n"
        "--team-effects LIST    you_ren (or none).\n"
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
template <typename Engine> void Trace(Engine &sim, const std::string &path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("cannot open trace output");
    out << "frame,sequence,skill_id,skill_name,sub,level,outcome,snapshot,damage,random_key\n";
    for (const auto &hit : sim.Log().Intents()) {
        const auto &skill = tx::Skills.entries[hit.skill];
        out << hit.frame << ',' << hit.sequence << ',' << skill.id << ',' << skill.name << ',' << hit.sub << ',' << hit.level
            << ',' << static_cast<int>(hit.outcome) << ',' << hit.snapshot << ','
            << sim.GetRules().Reduce(hit, sim.GetState()) << ',' << hit.random_key << '\n';
    }
    if (!out) throw std::runtime_error("could not write complete trace");
}
template <typename Engine> void PrintState(const Engine &sim, const CompiledMacro &macro) {
    const auto &s = sim.GetState();
    const auto &cursor = sim.MacroDebugState();
    constexpr std::array phases{"line", "condition", "action", "finished"};
    std::cout << "frame=" << s.now << " qidian=" << s.qidian << " charges=" << s.skill_energy[tx::ZiQi]
              << " dieren=" << s.target_buff_stacks[tx::DieRen] << " purple=" << s.self_buff_stacks[tx::Purple]
              << " last_skill=" << (s.last_skill == INVALID_SLOT ? "none" : tx::SkillAliases[s.last_skill])
              << " phase=" << phases[static_cast<unsigned>(cursor.phase)] << '\n';
    std::cout << "ready_in_frames:";
    for (Slot slot : tx::CastableSkills) {
        std::cout << ' ' << tx::SkillAliases[slot] << '=';
        if (s.preparing_skill == slot) std::cout << "preparing";
        else std::cout << std::max(0, s.SkillReadyAt(slot)-s.now);
    }
    std::cout << '\n';
    if (s.preparing_skill != INVALID_SLOT)
        std::cout << "preparing=" << tx::SkillAliases[s.preparing_skill]
                  << " remaining_frames=" << s.skill_prepare_at[s.preparing_skill]-s.now << '\n';
    for (Slot buff : {tx::SuiXingBuff, tx::QiShengBuff, tx::ChiYingBuff, tx::XuanMenBuff, tx::LieYunBuff, tx::JingHuaBuff, tx::WeaponCwBuff, tx::SetAttributeBuff})
        std::cout << tx::BuffAliases[buff] << '=' << s.self_buff_stacks[buff]
                  << " remaining=" << std::max(0, s.self_buff_expires_at[buff]-s.now) << ' ';
    std::cout << "target_tunriyue=" << s.target_buff_stacks[tx::TunRiBuff] << '\n';
    for (Slot buff : {tx::HiddenLieYun, tx::RenJianDot, tx::WanXiangBuff})
        std::cout << "target_" << tx::BuffAliases[buff] << '=' << s.target_buff_stacks[buff]
                  << " remaining=" << std::max(0, s.target_buff_expires_at[buff]-s.now)
                  << " next_tick_in=" << std::max(0, s.target_buff_tick_at[buff]-s.now) << '\n';
    for (const auto &field : sim.GetRules().Fields()) if (field.skill != INVALID_SLOT)
        std::cout << "field #" << field.order << ' ' << tx::SkillAliases[field.skill]
                  << " expires_in=" << field.expires_at-s.now << " next_tick_in=" << field.tick_at-s.now << '\n';
    if (!sim.Finished() && cursor.instruction < macro.source.size()) {
        std::cout << "line " << macro.source[cursor.instruction].line << ": " << macro.source[cursor.instruction].text << '\n';
        const auto &line = macro.program.lines[cursor.instruction];
        if (cursor.phase == DebugPhase::Condition && cursor.condition < line.condition_count) {
            constexpr std::array comparisons{"<", "<=", "=", "!=", ">", ">="};
            const auto &condition = macro.program.conditions[line.condition_begin+cursor.condition];
            std::cout << "condition " << cursor.condition+1 << ": " << ReadValue(s, condition) << ' '
                << comparisons[static_cast<unsigned>(condition.comparator)] << ' ' << condition.value
                << " => " << (EvaluateCondition(s, condition) ? "true" : "false") << " (time values in frames)\n";
        }
    }
}
bool Debug(Simulation<tx::Rules, true> &sim, const CompiledMacro &macro) {
    PrintState(sim, macro);
    std::string input;
    while (!sim.Finished() && std::cout << "debug> " && std::getline(std::cin, input)) {
        std::istringstream words(input);
        std::string command; words >> command;
        if (command == "quit") return false;
        if (command == "continue") sim.Continue();
        else if (command == "step") sim.StepInto();
        else if (command == "next") sim.StepOver();
        else if (command == "break" || command == "clear") {
            unsigned line = 0; words >> line;
            bool found = false;
            for (Pc pc = 0; pc < macro.source.size(); ++pc)
                if (macro.source[pc].line == line) { sim.SetBreakpoint(pc, command == "break"); found = true; }
            if (!found) std::cout << "No macro instruction at that source line.\n";
        } else if (command != "state") std::cout << "Commands: continue, step, next, break LINE, clear LINE, state, quit\n";
        PrintState(sim, macro);
        if (sim.DebugControl().budget_exhausted) std::cout << "Step budget reached; continue to resume.\n";
    }
    return sim.Finished();
}
} // namespace

int main(int argc, char **argv) try {
    tx::Config config;
    BatchOptions options;
    std::string macro_text(ExampleMacro), trace_path;
    bool debug = false;
    std::optional<std::uint64_t> replay;
    for (int i = 1; i < argc; ++i) {
        std::string_view flag(argv[i]);
        if (flag == "--help") { Help(); return 0; }
        if (flag == "--debug") { debug = true; continue; }
        if (++i == argc) throw std::invalid_argument("missing value for " + std::string(flag));
        std::string_view value(argv[i]);
        if (flag == "--iterations") options.iterations = Integer<std::uint64_t>(value);
        else if (flag == "--workers") options.workers = Integer<unsigned>(value);
        else if (flag == "--seed") options.seed = Integer<std::uint64_t>(value);
        else if (flag == "--replay") replay = Integer<std::uint64_t>(value);
        else if (flag == "--seconds") {
            const auto seconds = Decimal(value);
            if (seconds <= 0 || seconds > 86400 || std::floor(seconds*16) != seconds*16)
                throw std::invalid_argument("duration must be 1..1382400 whole frames (16 FPS)");
            options.duration = static_cast<tick_t>(seconds*16);
        } else if (flag == "--macro") {
            std::ifstream input(std::string(value), std::ios::binary);
            if (!input) throw std::invalid_argument("cannot read macro file");
            macro_text.assign(std::istreambuf_iterator<char>(input), {});
            if (macro_text.starts_with("\xef\xbb\xbf")) macro_text.erase(0, 3);
        } else if (flag == "--trace") trace_path = value;
        else if (flag == "--talents") config.talents = Flags(value, Talents);
        else if (flag == "--equipment") config.equipment = Flags(value, Equipment);
        else if (flag == "--team-effects") config.team_effects = Flags(value, TeamEffects);
        else if (flag == "--recipes-wuwu") config.recipes[tx::WuWo] = Flags(value, Recipes);
        else if (flag == "--recipes-bahuang") config.recipes[tx::BaHuang] = Flags(value, Recipes);
        else if (flag == "--recipes-sanhuan") config.recipes[tx::SanHuan] = Flags(value, Recipes);
        else if (flag == "--recipes-shengtaiji") config.shengtaiji_recipes = Flags(value, Recipes);
        else if (flag == "--recipes-renjian") config.renjian_recipes = Flags(value, Recipes);
        else if (flag == "--attribute") Attribute(config.attributes, value);
        else if (flag == "--target-level") config.target.level = Integer<int>(value);
        else if (flag == "--shield") config.target.shield = Integer<value_t>(value);
        else if (flag == "--life") config.target.life = Decimal(value);
        else if (flag == "--distance") config.target.distance = Decimal(value);
        else if (flag == "--qidian") config.initial_qidian = Integer<int>(value);
        else if (flag == "--delay") config.delay = Integer<tick_t>(value);
        else throw std::invalid_argument("unsupported option: " + std::string(flag));
    }
    if (!options.iterations || !options.workers) throw std::invalid_argument("iterations and workers must be positive");
    if ((debug || !trace_path.empty() || replay) && (options.iterations != 1 || options.workers != 1))
        throw std::invalid_argument("debug, trace and replay require one iteration and one worker");
    options.damage_capacity = static_cast<std::size_t>(options.duration)*8 + 32;
    options.mutation_capacity = static_cast<std::size_t>(options.duration)*24 + 64;
    const auto data = tx::Prepare(config);
    auto [macro, errors] = MacroCompiler::Compile(macro_text, tx::MacroOptions(debug));
    for (const auto &error : errors) std::cerr << "line " << error.line << ": " << error.message << '\n';
    if (!errors.empty()) return 2;
    std::cout << "ruleset=ba960f7-core-subset fps=16 immutable_damage_cache_bytes=" << sizeof(data->damage)
              << " seed_base=" << options.seed << '\n';
    const auto start = std::chrono::steady_clock::now();
    BatchStats stats;
    if (debug || !trace_path.empty() || replay) {
        const auto index = replay.value_or(0);
        const auto seed = IterationSeed(options.seed, index);
        std::cout << "iteration=" << index << " fight_seed=" << seed << '\n';
        if (debug) {
            Simulation<tx::Rules, true> sim(macro.program, tx::Rules(data), options.damage_capacity, options.mutation_capacity);
            sim.Start(options.duration, seed);
            if (!Debug(sim, macro)) { std::cout << "Debug session ended before fight completion.\n"; return 0; }
            if (!trace_path.empty()) Trace(sim, trace_path);
            stats.Add(sim.TotalDamage(), index);
        } else {
            Simulation<tx::Rules> sim(macro.program, tx::Rules(data), options.damage_capacity, options.mutation_capacity);
            sim.Start(options.duration, seed); sim.Run();
            if (!trace_path.empty()) Trace(sim, trace_path);
            stats.Add(sim.TotalDamage(), index);
        }
    } else stats = RunBatch(macro.program, options, tx::Rules(data));
    const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
    const double seconds = options.duration/16.0;
    std::cout << std::fixed << std::setprecision(3) << "fights=" << stats.count << " workers=" << options.workers
              << " mean_dps=" << stats.mean_damage/seconds << " dps_sd=" << stats.PopulationDeviation()/seconds
              << " min_damage=" << stats.min_damage << " max_damage=" << stats.max_damage
              << " checksum=" << stats.checksum << " elapsed_s=" << elapsed << '\n';
    return 0;
} catch (const std::exception &error) {
    std::cerr << "error: " << error.what() << '\n';
    return 1;
}
