#include "common.hpp"
#include <memory>
#include <sstream>
#include <spdlog/spdlog.h>
#include "Class/TaiXuJianYi/TaiXuJianYi.h"
#include "Class/TaiXuJianYi/TaiXuJianYiBuff.h"
#include "Buff.h"
#include "Skill.h"
#include "Regex.h"
#include "TimeLine.hpp"

namespace JX3DPS {
Stats Simulate(Player &, ExprSkillsHash &, ExprEvents &, Options &);
long long Sum(const Stats &);
}
using namespace JX3DPS;

// Validation-only phase override; the default benchmark uses the original Buff.
class FixedPhaseFeature : public TaiXuJianYi::Buff::ClassFeature {
public:
    FixedPhaseFeature(JX3DPS::Player *player, int phase) : ClassFeature(player, nullptr), phase_(phase) {}
    FixedPhaseFeature *Clone() const override { return new FixedPhaseFeature(*this); }
    void Add(Id_t target, int stacks, Frame_t min, Frame_t max) override {
        ClassFeature::Add(target, stacks, min, max);
        m_snapshots[PLAYER_ID].interval = phase_;
    }
private:
    int phase_;
};

// The original Buff::Record updates stats but emits no TimeLine entry. Observe
// a DOT tick's stats delta only in traced validation fights, outside benchmarks.
class TraceDieRen : public TaiXuJianYi::Buff::DieRen {
public:
    explicit TraceDieRen(const TaiXuJianYi::Buff::DieRen &source) : DieRen(source) {}
    TraceDieRen *Clone() const override { return new TraceDieRen(*this); }
    void Trigger() override {
        auto before = GetStats();
        DieRen::Trigger();
        const auto &gains = GetStats().gainStats;
        const auto current = gains.find(Attribute::Type::DEFAULT);
        if (current == gains.end()) return;
        for (const auto &[target, effects] : current->second)
            for (const auto &[effect, subs] : effects) for (const auto &[sub, levels] : subs)
                for (const auto &[level, rolls] : levels) for (const auto &[roll, value] : rolls) {
                    const auto &old = before.gainStats[Attribute::Type::DEFAULT][target][effect][sub][level][roll];
                    if (value.first == old.first) continue;
                    if (value.first != old.first + 1) throw std::runtime_error("ambiguous DOT trace delta");
                    TimeLine::RecordSkill(std::string(JX3DPS_NAME.at(static_cast<int>(effect))), roll,
                        static_cast<int>(value.second.SumDamage() - old.second.SumDamage()), NodeType::SKILL_EFFECT);
                }
    }
};

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
    const auto preparation_start = Clock::now();
    spdlog::set_level(spdlog::level::off);
    TimeLine::SetMode(Options::DEFAULT);
    TaiXuJianYi::Player player;
    auto &a = player.attribute;
    a.SetAttributeInitial(Attribute::Type::PHYSICS_ATTACK_POWER_BASE, 10000);
    a.SetPhysicsAttackPowerByClass(2500);
    a.SetAttributeInitial(Attribute::Type::WEAPON_DAMAGE_BASE, 2000);
    a.SetAttributeInitial(Attribute::Type::WEAPON_DAMAGE_RAND, 0);
    a.SetAttributeInitial(Attribute::Type::PHYSICS_CRITICAL_STRIKE, args.crit);
    a.SetAttributeInitial(Attribute::Type::PHYSICS_CRITICAL_STRIKE_POWER, 15000);
    a.SetAttributeInitial(Attribute::Type::PHYSICS_OVERCOME_BASE, 25000);
    a.SetPhysicsOvercomeByClass(0);
    a.SetAttributeInitial(Attribute::Type::STRAIN_BASE, 10000);
    a.SetAttributeInitial(Attribute::Type::SURPLUS_VALUE_BASE, 3000);
    a.SetAttributeInitial(Attribute::Type::HASTE_BASE, 0);
    if (args.dot) { player.talents[TALENT_DIE_REN] = true; player.talents[TALENT_SHEN_MAI] = true; }
    if (args.weapon_cw) player.equipEffects[EQUIP_EFFECT_WEAPON_CW] = true;
    const std::map<std::string, Id_t> talents = {
        {"wuyi", TALENT_WU_YI}, {"ruoshui", TALENT_RUO_SHUI}, {"fengshi", TALENT_FENG_SHI},
        {"wuyu", TALENT_WU_YU}, {"xingu", TALENT_XIN_GU}, {"tonggen", TALENT_TONG_GEN},
        {"huanyue", TALENT_HUAN_YUE}, {"lieyun", TALENT_LIE_YUN}, {"xuji", TALENT_XU_JI}
    };
    for (const auto &name : args.talents) {
        if (!talents.contains(name)) throw std::invalid_argument("unsupported comparison talent: " + name);
        player.talents[talents.at(name)] = true;
    }
    player.Init();
    const std::map<std::string, Id_t> team_ids{
        {"han_ru_lei", BUFF_3RD_HAN_RU_LEI}, {"po_feng", BUFF_3RD_PO_FENG}, {"jing_feng", BUFF_3RD_JING_FENG},
        {"jie_huo", BUFF_3RD_JIE_HUO}, {"chao_sheng", BUFF_3RD_CHAO_SHENG}, {"sheng_yu_ming_xin", BUFF_3RD_SHENG_YU_MING_XIN},
        {"zhen_fen", BUFF_3RD_ZHEN_FEN}, {"han_xiao_qian_jun", BUFF_3RD_HAN_XIAO_QIAN_JUN},
        {"shu_kuang", BUFF_3RD_SHU_KUANG}, {"han_chang_lin_li", BUFF_3RD_HAN_CHANG_LIN_LI},
        {"ji_lei", BUFF_3RD_JI_LEI}, {"nong_mei", BUFF_3RD_NONG_MEI},
        {"she_shen_hong_fa", BUFF_3RD_SHE_SHEN_HONG_FA}, {"hao_ling_san_jun", BUFF_3RD_HAO_LING_SAN_JUN}
    };
    struct TeamEvent { Id_t id; int frame, duration, stacks; };
    std::vector<TeamEvent> team_events;
    std::unordered_set<Id_t> enabled_team;
    std::istringstream schedule(args.team_buffs);
    for (std::string row; std::getline(schedule, row, ';'); ) {
        std::istringstream fields(row);
        std::string name, frame, duration, stacks;
        if (!std::getline(fields, name, ':') || !std::getline(fields, frame, ':') ||
            !std::getline(fields, duration, ':') || !std::getline(fields, stacks) || !team_ids.contains(name))
            throw std::invalid_argument("invalid legacy team schedule");
        const auto id = team_ids.at(name);
        team_events.push_back({id, std::stoi(frame), std::stoi(duration), std::stoi(stacks)});
        enabled_team.insert(id);
    }
    player.AddBuff3rds(enabled_team);
    player.SetQidian(args.initial_qidian);
    if (args.phase >= 0) {
        delete player.buffs.at(BUFF_CLASS_FEATURE);
        player.buffs[BUFF_CLASS_FEATURE] = new FixedPhaseFeature(&player, args.phase);
    }
#ifndef LEGACY_DRAW_RECORD
    if (args.dot && !args.trace_path.empty()) {
        auto traced = std::make_unique<TraceDieRen>(*static_cast<TaiXuJianYi::Buff::DieRen *>(player.buffs.at(BUFF_DIE_REN)));
        delete player.buffs.at(BUFF_DIE_REN);
        player.buffs[BUFF_DIE_REN] = traced.release();
    }
#endif
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
    for (const auto app : team_events) {
        const bool target_buff = app.id == BUFF_3RD_PO_FENG || app.id == BUFF_3RD_JING_FENG || app.id == BUFF_3RD_JIE_HUO;
        events.emplace_back(app.frame, [app, target, target_buff](auto *p, auto *t) {
            const auto id = target_buff ? target : PLAYER_ID;
            if (!app.stacks) {
                if (p->buffs.at(app.id)->GetDurationCurrent(id) <= 0) return JX3DPS_SUCCESS;
                return Expression::ClearBuff(p, t, id, app.id, std::max(1, p->buffs.at(app.id)->GetStackNumCurrent(id)));
            }
            const auto duration = app.duration ? app.duration : JX3DPS_DEFAULT_DURATION_FRAMES;
            return Expression::AddBuff(p, t, id, app.id, app.stacks, duration, duration);
        });
    }
    events.emplace_back(args.seconds * 16, Expression::SetEnd);
    events.sort([](const auto &a, const auto &b) { return a.first < b.first; });
    Options options;
    options.totalFrames = args.seconds * 16;
    options.framePrecision = args.precision;
    const double preparation_s = std::chrono::duration<double>(Clock::now()-preparation_start).count();
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
    const char *engine = "legacy-draw-record-validation";
    std::ofstream draws(args.draws_path);
    draws << Json{{"format", 1}, {"samples", legacy_draws::samples}, {"kinds", legacy_draws::kinds},
        {"integer_procs", legacy_draws::integer_procs}}.dump() << '\n';
    if (!draws) throw std::runtime_error("draw record write failed");
#elif defined(LEGACY_DPS_ONLY)
    const char *engine = "legacy-dps-only-diagnostic";
#else
    const char *engine = "legacy-ba960f7";
#endif
    auto result = totals.Result(engine, elapsed, args, Describe(last));
    result["preparation_s"] = preparation_s;
#ifdef LEGACY_DRAW_RECORD
    result["draws"] = legacy_draws::samples.size();
#endif
    result["attributes"] = {{"attack_base", a.GetPhysicsAttackPowerBase()}, {"attack_by_class", a.GetPhysicsAttackPowerByClass()},
        {"weapon", a.GetWeaponDamage()}, {"crit", a.GetPhysicsCriticalStrike()}, {"crit_basis_points", a.GetPhysicsCriticalStrikeAdditionalBasisPointInt()},
        {"crit_power", a.GetPhysicsCriticalStrikePower()}, {"crit_power_percent", a.GetPhysicsCriticalStrikePowerAdditionalPercentInt()},
        {"overcome", a.GetPhysicsOvercome()}, {"strain", a.GetStrainBase()}, {"surplus", a.GetSurplusValueBase()}, {"haste", a.GetHastePercent()},
        {"attack", a.GetPhysicsAttackPower()}, {"pve_percent", a.GetPVEDamageAdditionalPercentInt()}, {"damage_percent", a.GetDamageAdditionalPercentInt()}};
    if (!args.trace_path.empty()) {
        std::ofstream out(args.trace_path);
        out << "frame,name,damage,outcome,type\n";
        for (const auto &[frame, entries] : TimeLine::Instance().GetInfosList())
            for (const auto &entry : entries) out << frame << ',' << entry.name << ',' << entry.damage << ',' << int(entry.rollResult) << ',' << int(entry.type) << '\n';
        if (!out) throw std::runtime_error("trace write failed");
    }
    std::cout << result.dump() << '\n';
} catch (const std::exception &error) { std::cerr << error.what() << '\n'; return 1; }
