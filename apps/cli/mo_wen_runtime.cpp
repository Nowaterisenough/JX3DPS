#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"
#include <charconv>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace mw = JX3DPS::runtime::mo_wen;

namespace {
constexpr std::string_view ExampleMacro =
    "/cast [tnobuff:shang] shang\n/cast [tnobuff:jue] jue\n"
    "/cast shuying\n/cast gaoshan\n/cast [buff:qufeng=4] yu\n"
    "/cast bianzhi\n/cast biangong";
constexpr std::array<std::pair<std::string_view, unsigned>, 15> Talents{{
    {"haozhong", mw::HaoZhong}, {"feifan", mw::FeiFan}, {"changqing", mw::ChangQing},
    {"xianfeng", mw::XianFengTalent}, {"liuzhao", mw::LiuZhaoTalent}, {"haoqing", mw::HaoQing},
    {"shixiang", mw::ShiXiang}, {"zhizhi", mw::ZhiZhi}, {"kemeng", mw::KeMeng},
    {"zhengming", mw::ZhengMing}, {"mingjin", mw::MingJinTalent}, {"xiuqi", mw::XiuQi},
    {"yunhan", mw::YunHanTalent}, {"canlian", mw::CanLianTalent}, {"zhenglv", mw::ZhengLvHeMing}
}};
constexpr std::array<std::pair<std::string_view, unsigned>, 7> Equipment{{
    {"weapon_cw", mw::WeaponCW}, {"set_attribute", mw::SetAttribute}, {"set_skill", mw::SetSkill},
    {"weapon_water", mw::WeaponWater}, {"shoes", mw::EnchantShoes}, {"wrist", mw::EnchantWrist}, {"belt", mw::EnchantBelt}
}};
constexpr std::array<std::pair<std::string_view, unsigned>, 2> TeamEffects{{
    {"you_ren", mw::YouRen}, {"jingmiao", mw::JingMiao}
}};
constexpr std::array<std::pair<std::string_view, unsigned>, 8> GongRecipes{{
    {"crit2", mw::GongCrit2}, {"crit3", mw::GongCrit3}, {"crit4", mw::GongCrit4},
    {"damage3", mw::GongDamage3}, {"damage4", mw::GongDamage4},
    {"prepare1", mw::GongPrepare1}, {"prepare2", mw::GongPrepare2}, {"range", mw::GongRange}
}};
constexpr std::array<std::pair<std::string_view, unsigned>, 6> ShangRecipes{{
    {"crit2", mw::ShangCrit2}, {"crit3", mw::ShangCrit3}, {"crit4", mw::ShangCrit4},
    {"damage3", mw::ShangDamage3}, {"damage4", mw::ShangDamage4}, {"damage5", mw::ShangDamage5}
}};
constexpr std::array<std::pair<std::string_view, unsigned>, 6> ZhiRecipes{{
    {"crit2", mw::ZhiCrit2}, {"crit3", mw::ZhiCrit3}, {"crit4", mw::ZhiCrit4},
    {"damage3", mw::ZhiDamage3}, {"damage4", mw::ZhiDamage4}, {"range", mw::ZhiRange}
}};
constexpr std::array<std::pair<std::string_view, unsigned>, 4> YuRecipes{{
    {"crit3", mw::YuCrit3}, {"crit4", mw::YuCrit4},
    {"damage3", mw::YuDamage3}, {"damage4", mw::YuDamage4}
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
void Attribute(mw::Attributes &a, std::string_view text) {
    const auto equals = text.find('=');
    if (equals == text.npos) throw std::invalid_argument("attribute syntax is name=integer");
    const auto key = text.substr(0, equals);
    const auto value = Integer<value_t>(text.substr(equals+1));
    constexpr std::array<std::pair<std::string_view, value_t mw::Attributes::*>, 9> values{{
        {"attack_base", &mw::Attributes::attack_base}, {"attack_by_class", &mw::Attributes::attack_by_class},
        {"weapon", &mw::Attributes::weapon}, {"crit", &mw::Attributes::crit}, {"crit_power", &mw::Attributes::crit_power},
        {"overcome", &mw::Attributes::overcome}, {"strain", &mw::Attributes::strain},
        {"surplus", &mw::Attributes::surplus}, {"haste", &mw::Attributes::haste}
    }};
    for (auto [name, field] : values) if (name == key && field) { a.*field = value; return; }
    constexpr std::array<std::pair<std::string_view, int mw::Attributes::*>, 9> percents{{
        {"attack_percent", &mw::Attributes::attack_percent}, {"haste_percent", &mw::Attributes::haste_percent},
        {"crit_basis_points", &mw::Attributes::crit_basis_points}, {"crit_power_percent", &mw::Attributes::crit_power_percent},
        {"damage_percent", &mw::Attributes::damage_percent}, {"strain_percent", &mw::Attributes::strain_percent},
        {"pve_percent", &mw::Attributes::pve_percent}, {"ignore_shield", &mw::Attributes::ignore_shield},
        {"overcome_percent", &mw::Attributes::overcome_percent}
    }};
    for (auto [name, field] : percents) if (name == key && field) {
        if (value < 0 || value > 10000) throw std::invalid_argument("attribute bonus outside supported range");
        a.*field = static_cast<int>(value); return;
    }
    throw std::invalid_argument("unknown resolved attribute: " + std::string(key));
}
void Help() {
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
template <typename Engine> void Trace(Engine &sim, const std::string &path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("cannot open trace output");
    out << "frame,sequence,report_id,skill_name,effect,sub,level,outcome,snapshot,extra_crit_power,damage,random_key\n";
    for (const auto &hit : sim.Log().Intents()) {
        const auto &skill = mw::Skills.entries[hit.skill];
        out << hit.frame << ',' << hit.sequence << ',' << skill.id << ',' << skill.name << ',' << hit.effect << ',' << hit.sub << ',' << hit.level
            << ',' << static_cast<int>(hit.outcome) << ',' << hit.snapshot << ',' << hit.base << ','
            << sim.GetRules().Reduce(hit) << ',' << hit.random_key << '\n';
    }
    if (!out) throw std::runtime_error("could not write complete trace");
}
template <typename Engine> void PrintState(const Engine &sim, const CompiledMacro &macro) {
    const auto &s = sim.GetState();
    const auto &cursor = sim.MacroDebugState();
    constexpr std::array phases{"line", "condition", "action", "finished"};
    std::cout << "frame=" << s.now
              << " style=" << (sim.GetRules().CurrentStyle()==mw::Style::GaoShanLiuShui ? "gaoshan" : "yangchun")
              << " qufeng=" << s.self_buff_stacks[mw::QuFeng]
              << " shadows=" << sim.GetRules().ShadowCount()
              << " last_skill=" << (s.last_skill == INVALID_SLOT ? "none" : mw::SkillAliases[s.last_skill])
              << " phase=" << phases[static_cast<unsigned>(cursor.phase)] << '\n';
    std::cout << "ready_in_frames:";
    for (Slot slot : mw::CastableSkills)
        std::cout << ' ' << mw::SkillAliases[slot] << '=' << std::max(0, s.SkillReadyAt(slot)-s.now);
    std::cout << "\ncharges: yu=" << s.skill_energy[mw::Yu] << " shuying=" << s.skill_energy[mw::ShuYing]
              << " zhi=" << s.skill_energy[mw::Zhi] << '\n';
    if (s.preparing_skill != INVALID_SLOT)
        std::cout << "preparing=" << mw::SkillAliases[s.preparing_skill]
                  << " remaining_frames=" << s.skill_prepare_at[s.preparing_skill]-s.now << '\n';
    if (s.casting_skill != INVALID_SLOT)
        std::cout << "channeling=" << mw::SkillAliases[s.casting_skill]
                  << " channel_complete_in=" << s.skill_casting_at[s.casting_skill]-s.now << '\n';
    for (Slot buff=0; buff<mw::BuffCount; ++buff) {
        const bool target=mw::IsDot(buff);
        const auto stacks=target?s.target_buff_stacks[buff]:s.self_buff_stacks[buff];
        if (!stacks) continue;
        const auto expiry=target?s.target_buff_expires_at[buff]:s.self_buff_expires_at[buff];
        std::cout << (target?"target_":"self_") << mw::BuffAliases[buff] << '=' << stacks
                  << " remaining=" << std::max(0,expiry-s.now);
        if (target) std::cout << " next_tick_in=" << std::max(0,s.target_buff_tick_at[buff]-s.now);
        std::cout << '\n';
    }
    for (const auto &shadow : sim.GetRules().Shadows()) if (shadow.expires_at)
        std::cout << "shadow #" << shadow.order << " expires_in=" << shadow.expires_at-s.now
                  << " next_tick_in=" << shadow.tick_at-s.now << " snapshot=" << shadow.snapshot.profile << '\n';
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
bool Debug(Simulation<mw::Rules, true> &sim, const CompiledMacro &macro) {
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
    mw::Config config;
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
        else if (flag == "--recipes-gong") config.recipes[0] = Flags(value, GongRecipes);
        else if (flag == "--recipes-shang") config.recipes[1] = Flags(value, ShangRecipes);
        else if (flag == "--recipes-zhi") config.recipes[2] = Flags(value, ZhiRecipes);
        else if (flag == "--recipes-yu") config.recipes[3] = Flags(value, YuRecipes);
        else if (flag == "--style") {
            if (value=="gaoshan") config.initial_style=mw::Style::GaoShanLiuShui;
            else if (value=="yangchun") config.initial_style=mw::Style::YangChunBaiXue;
            else throw std::invalid_argument("unknown style: " + std::string(value));
        }
        else if (flag == "--attribute") Attribute(config.attributes, value);
        else if (flag == "--target-level") config.target.level = Integer<int>(value);
        else if (flag == "--shield") config.target.shield = Integer<value_t>(value);
        else if (flag == "--life") config.target.life = Decimal(value);
        else if (flag == "--distance") config.target.distance = Decimal(value);
        else if (flag == "--shadows") config.initial_yingzi = Integer<int>(value);
        else if (flag == "--vulnerable") config.target.vulnerable = Integer<int>(value);
        else if (flag == "--delay") config.delay = Integer<tick_t>(value);
        else throw std::invalid_argument("unsupported option: " + std::string(flag));
    }
    if (!options.iterations || !options.workers) throw std::invalid_argument("iterations and workers must be positive");
    if ((debug || !trace_path.empty() || replay) && (options.iterations != 1 || options.workers != 1))
        throw std::invalid_argument("debug, trace and replay require one iteration and one worker");
    options.damage_capacity = static_cast<std::size_t>(options.duration)*16 + 64;
    options.mutation_capacity = static_cast<std::size_t>(options.duration)*40 + 128;
    const auto data = mw::Prepare(config);
    auto [macro, errors] = MacroCompiler::Compile(macro_text, mw::MacroOptions(debug));
    for (const auto &error : errors) std::cerr << "line " << error.line << ": " << error.message << '\n';
    if (!errors.empty()) return 2;
    std::cout << "ruleset=mo-wen-ba960f7 fps=16 immutable_damage_cache_bytes=" << data->CacheBytes()
              << " seed_base=" << options.seed << '\n';
    const auto start = std::chrono::steady_clock::now();
    BatchStats stats;
    if (debug || !trace_path.empty() || replay) {
        const auto index = replay.value_or(0);
        const auto seed = IterationSeed(options.seed, index);
        std::cout << "iteration=" << index << " fight_seed=" << seed << '\n';
        if (debug) {
            Simulation<mw::Rules, true> sim(macro.program, mw::Rules(data), options.damage_capacity, options.mutation_capacity);
            sim.Start(options.duration, seed);
            if (!Debug(sim, macro)) { std::cout << "Debug session ended before fight completion.\n"; return 0; }
            if (!trace_path.empty()) Trace(sim, trace_path);
            stats.Add(sim.TotalDamage(), index);
        } else {
            Simulation<mw::Rules> sim(macro.program, mw::Rules(data), options.damage_capacity, options.mutation_capacity);
            sim.Start(options.duration, seed); sim.Run();
            if (!trace_path.empty()) Trace(sim, trace_path);
            stats.Add(sim.TotalDamage(), index);
        }
    } else stats = RunBatch(macro.program, options, mw::Rules(data));
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
