#include "common.hpp"
#include <memory>
#include <sstream>
#include <spdlog/spdlog.h>
#include "Class/MoWen/MoWen.h"
#include "Class/MoWen/MoWenBuff.h"
#include "Buff.h"
#include "Skill.h"
#include "Regex.h"
#include "TimeLine.hpp"

namespace JX3DPS {
Stats Simulate(Player &, ExprSkillsHash &, ExprEvents &, Options &);
long long Sum(const Stats &);
}
using namespace JX3DPS;

Json Describe(const Stats &stats) {
    Json result = Json::object();
    for (const auto &[target, effects] : stats.gainStats.at(Attribute::Type::DEFAULT))
        for (const auto &[effect, subs] : effects) {
            long long damage = 0, hits = 0;
            for (const auto &[sub, levels] : subs) for (const auto &[level, rolls] : levels)
                for (const auto &[roll, value] : rolls) { hits += value.first; damage += value.second.SumDamage(); }
            result[std::string(JX3DPS_NAME.at(static_cast<int>(effect)))] = {{"hits", hits}, {"damage", damage}};
        }
    return result;
}

int main(int argc, char **argv) try {
    Arguments args(argc, argv);
    spdlog::set_level(spdlog::level::off);
    TimeLine::SetMode(Options::DEFAULT);
    MoWen::Player player;
    auto &a = player.attribute;
    a.SetAttributeInitial(Attribute::Type::MAGIC_ATTACK_POWER_BASE, 10000);
    a.SetMagicAttackPowerByClass(2500);
    a.SetAttributeInitial(Attribute::Type::WEAPON_DAMAGE_BASE, 2000);
    a.SetAttributeInitial(Attribute::Type::WEAPON_DAMAGE_RAND, 0);
    a.SetAttributeInitial(Attribute::Type::MAGIC_CRITICAL_STRIKE, args.crit);
    a.SetAttributeInitial(Attribute::Type::MAGIC_CRITICAL_STRIKE_POWER, 15000);
    a.SetAttributeInitial(Attribute::Type::MAGIC_OVERCOME_BASE, 25000);
    a.SetMagicOvercomeByClass(0);
    a.SetAttributeInitial(Attribute::Type::STRAIN_BASE, 10000);
    a.SetAttributeInitial(Attribute::Type::SURPLUS_VALUE_BASE, 3000);
    a.SetAttributeInitial(Attribute::Type::HASTE_BASE, 0);
    if (args.weapon_cw) player.equipEffects[EQUIP_EFFECT_WEAPON_CW] = true;
    const std::map<std::string, Id_t> talents = {
        {"haozhong", TALENT_HAO_ZHONG},
        {"feifan", TALENT_FEI_FAN},
        {"xianfeng", TALENT_XIAN_FENG},
        {"liuzhao", TALENT_LIU_ZHAO},
        {"haoqing", TALENT_HAO_QING},
        {"shixiang", TALENT_SHI_XIANG},
        {"zhizhi", TALENT_ZHI_ZHI},
        {"kemeng", TALENT_KE_MENG},
        {"zhengming", TALENT_ZHENG_MING},
        {"mingjin", TALENT_MING_JIN},
        {"yunhan", TALENT_YUN_HAN},
        {"canlian", TALENT_CAN_LIAN},
        {"zhenglv", TALENT_ZHENG_LV_HE_MING}
    };
    for (const auto &name : args.talents) {
        if (!talents.contains(name)) throw std::invalid_argument("unsupported comparison talent: " + name);
        player.talents[talents.at(name)] = true;
    }
    player.Init();
    ExprSkillsHash macros;
    std::istringstream text(args.Macro());
    std::string line;
    ExprSkills lines;
    while (std::getline(text, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line.starts_with('#')) continue;
        ExprSkill instruction;
        if (Regex::ParseToExprSkill(line, instruction) != JX3DPS_SUCCESS) throw std::runtime_error("legacy compiler rejected: " + line);
        lines.push_back(std::move(instruction));
    }
    macros.emplace(EXPRESSION_SKILL_PLACE_HOLDERS_1, std::move(lines));
    ExprEvents events;
    const auto target = TARGET_PLACE_HOLDERS_1;
    events.emplace_back(0, [target](auto *p, auto *t) { return Expression::AddTarget(p, t, target, 124, 30000, 4.0); });
    events.emplace_back(0, [target](auto *p, auto *t) { return Expression::ChangeTarget(p, t, target); });
    events.emplace_back(args.seconds * 16, Expression::SetEnd);
    Options options;
    options.totalFrames = args.seconds * 16;
    options.framePrecision = args.precision;
    for (int i = 0; i < args.warmup; ++i) (void)Simulate(player, macros, events, options);
    if (!args.trace_path.empty()) TimeLine::SetMode(Options::DEBUG);
    Totals totals;
    Stats last;
#ifdef LEGACY_DRAW_RECORD
    legacy_draws::Start();
#endif
    const auto start = Clock::now();
    for (int i = 0; i < args.iterations; ++i) {
        auto stats = Simulate(player, macros, events, options);
        totals.Add(Sum(stats));
        if (i + 1 == args.iterations) last = std::move(stats);
    }
    const double elapsed = std::chrono::duration<double>(Clock::now() - start).count();
#ifdef LEGACY_DRAW_RECORD
    legacy_draws::active = false;
    const char *engine = "mo-wen-legacy-draw-record";
    std::ofstream draws(args.draws_path);
    draws << Json{{"format", 1}, {"samples", legacy_draws::samples}, {"kinds", legacy_draws::kinds},
        {"integer_procs", legacy_draws::integer_procs}}.dump() << '\n';
    if (!draws) throw std::runtime_error("draw record write failed");
#elif defined(LEGACY_DPS_ONLY)
    const char *engine = "legacy-dps-only-diagnostic";
#else
    const char *engine = "mo-wen-legacy-ba960f7";
#endif
    auto result = totals.Result(engine, elapsed, args, Describe(last));
#ifdef LEGACY_DRAW_RECORD
    result["draws"] = legacy_draws::samples.size();
#endif
    result["attributes"] = {{"attack_base", a.GetMagicAttackPowerBase()}, {"attack_by_class", a.GetMagicAttackPowerByClass()},
        {"weapon", a.GetWeaponDamage()}, {"crit", a.GetMagicCriticalStrike()}, {"crit_basis_points", a.GetMagicCriticalStrikeAdditionalBasisPointInt()},
        {"crit_power", a.GetMagicCriticalStrikePower()}, {"crit_power_percent", a.GetMagicCriticalStrikePowerAdditionalPercentInt()},
        {"overcome", a.GetMagicOvercome()}, {"strain", a.GetStrainBase()}, {"surplus", a.GetSurplusValueBase()}, {"haste", a.GetHastePercent()},
        {"attack", a.GetMagicAttackPower()}, {"pve_percent", a.GetPVEDamageAdditionalPercentInt()}, {"damage_percent", a.GetDamageAdditionalPercentInt()}};
    if (!args.trace_path.empty()) {
        std::ofstream out(args.trace_path);
        out << "frame,name,damage,outcome,type\n";
        for (const auto &[frame, entries] : TimeLine::Instance().GetInfosList())
            for (const auto &entry : entries) out << frame << ',' << entry.name << ',' << entry.damage << ',' << int(entry.rollResult) << ',' << int(entry.type) << '\n';
        if (!out) throw std::runtime_error("trace write failed");
    }
    std::cout << result.dump() << '\n';
} catch (const std::exception &error) { std::cerr << error.what() << '\n'; return 1; }
