#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"
#include <cstdlib>
#include <iostream>
#include <new>
#include <random>

// Compile the unchanged old arithmetic in a separate namespace.
#define JX3DPS LegacyReference
#include "legacy_reference/Damage.hpp"
#undef JX3DPS
#include "legacy_reference/skill_parameters.inc"

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;
#define CHECK(x) do { if (!(x)) { std::cerr << __LINE__ << ": " #x "\n"; std::exit(1); } } while (false)
template <typename F> void Rejects(F f) {
    try { f(); } catch (const std::invalid_argument &) { return; }
    CHECK(false);
}
static bool count_allocations = false;
static std::size_t allocations = 0;
void *operator new(std::size_t size) {
    if (count_allocations) ++allocations;
    if (auto *p = std::malloc(size ? size : 1)) return p;
    throw std::bad_alloc();
}
void *operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { std::free(p); }
void operator delete[](void *p, std::size_t) noexcept { std::free(p); }

CompiledMacro Compile(std::string_view text) {
    auto [macro, errors] = MacroCompiler::Compile(text, tx::MacroOptions());
    for (const auto &error : errors) std::cerr << error.message << '\n';
    CHECK(errors.empty());
    return macro;
}
struct HitsOnly {
    void Reset(std::uint64_t) {}
    std::uint64_t Next() { return ~std::uint64_t{0}; }
};
struct MixedRolls {
    unsigned index = 0;
    void Reset(std::uint64_t) { index = 0; }
    std::uint64_t Next() { return (index++ % 2) ? ~std::uint64_t{0} : 0; }
};
struct ThresholdRolls {
    void Reset(std::uint64_t) {}
    std::uint64_t Next() { return static_cast<std::uint64_t>(0.275 * 0x1.0p53) << 11; }
};
struct FirstCritThenThreshold {
    unsigned index = 0;
    void Reset(std::uint64_t) { index = 0; }
    std::uint64_t Next() {
        if (index++ == 0) return 0;
        return static_cast<std::uint64_t>(0.26 * 0x1.0p53) << 11;
    }
};
struct AlwaysProc {
    void Reset(std::uint64_t) {}
    std::uint64_t Next() { return 0; }
};
struct FixedQidianPhase {
    tick_t frame = 15;
    tick_t operator()(std::uint64_t) const { return frame; }
};
struct Result {
    State state;
    std::vector<DamageIntent> hits;
    std::vector<StateMutation> mutations;
    value_t damage;
    std::array<tx::FieldInstance, 3> fields;
};
template <typename Rng = HitsOnly>
Result Run(std::string_view text, tx::Config config = {}, tick_t duration = 480, std::uint64_t seed = 0) {
    auto macro = Compile(text);
    using Rules = tx::BasicRules<Rng, FixedQidianPhase>;
    Simulation<Rules> sim(macro.program, Rules(tx::Prepare(config)));
    sim.Start(duration, seed); sim.Run();
    return {sim.GetState(), sim.Log().Intents(), sim.Log().Mutations(), sim.TotalDamage(), sim.GetRules().Fields()};
}

// Inputs and modifiers are assembled independently of Prepared::Input/Chance.
value_t ReferenceDamage(const tx::Config &c, unsigned effect, unsigned profile, bool crit) {
    using namespace LegacyReference;
    if (effect >= tx::ZeroEffect && effect < tx::ChiYingEffect) return 0;
    LegacyParam param(0, 0, 0);
    unsigned recipe = 0;
    int skill_damage = 0, skill_power = 0, count = 1, dot_count = 8;
    bool dot = false, surplus = false;
    if (effect < 20) {
        unsigned q = effect % 10;
        param = legacy_wuwu[q]; recipe = c.recipes[0];
        if (effect >= 10) skill_damage += 307;
        if (c.talents & tx::TongGen) skill_damage += 102;
        if (q >= 5 && (c.talents & tx::WuYi)) skill_power += 307;
    } else if (effect == 20) { param = legacy_bahuang[0]; recipe = c.recipes[1]; }
    else if (effect < 31) { param = legacy_bahuang_extra[effect-21]; recipe = c.recipes[1]; }
    else if (effect == 31) {
        param = legacy_sanhuan[0]; recipe = c.recipes[2];
        if (c.talents & tx::XinGu) skill_power += 102;
    } else if (effect == 32) { param = legacy_sanchai[0]; skill_damage += 205; }
    else if (effect < 36) { param = legacy_pozhao[effect-33]; surplus = true; }
    else if (effect < tx::ZeroEffect) {
        param = legacy_dieren[(effect-36)/9]; count = (effect-36)%9; dot = true;
        if (c.talents & tx::XuJi) param.channel = static_cast<int>(param.channel*1.2);
    } else if (effect == tx::ChiYingEffect) param = legacy_chiying[0];
    else if (effect == tx::WanJianEffect) {
        param = legacy_wanjian[0];
        if (c.talents & tx::TongGen) skill_damage += 102;
        if (c.talents & tx::XuJi) skill_power += 1000+102;
    } else if (effect == tx::RenJianDotEffect) {
        param = legacy_renjian_dot[0]; dot = true; dot_count = 4;
    } else if (effect == tx::JianRuEffect) {
        param = legacy_jianru[0];
    } else if (effect == tx::JianRuActiveEffect) {
        param = legacy_jianru_active[0];
    } else if (effect >= tx::YunZhongSuiXingEffect && effect <= tx::YunZhongTunRiEffect) {
        param = effect == tx::YunZhongSuiXingEffect ? legacy_yunzhong_suixing[0] :
            effect == tx::YunZhongShengTaiEffect ? legacy_yunzhong_shengtaiji[0] : legacy_yunzhong_tunriyue[0];
    } else if (effect == tx::JingHuaEffect) {
        param = legacy_jinghua[0];
    } else if (effect == tx::JingHuaSuiEffect) {
        param = legacy_jinghua_sui[0];
    } else if (effect == tx::BaHuangCwEffect) {
        param = legacy_bahuang_extra[9]; recipe = c.recipes[1];
    } else if (effect == tx::WeaponCwExtraEffect) {
        param = LegacyParam((20+20+2)/2, 0, 65); recipe = c.recipes[1];
    } else if (effect >= tx::WanXiangEffect && effect < tx::WanXiangEffect + 3) {
        const auto stack = static_cast<int>(effect - tx::WanXiangEffect) + 1;
        param = LegacyParam(10, 0, static_cast<int>(205*stack*1.8));
        dot = true; dot_count = 10;
    } else {
        param = effect == tx::RenJianEffect ? legacy_renjian[0] : legacy_renjian_suixing[0];
        if (c.renjian_recipes & tx::Damage40) skill_damage += 409;
        if (c.renjian_recipes & tx::Damage60) skill_damage += 614;
    }
    skill_damage += ((recipe&tx::Damage3) ? 31 : 0) + ((recipe&tx::Damage4) ? 41 : 0) + ((recipe&tx::Damage5) ? 51 : 0);
    if ((c.equipment & tx::WeaponCW) && (effect < 31 || effect == tx::BaHuangCwEffect || effect == tx::WeaponCwExtraEffect))
        skill_damage += 51;
    const auto &a = c.attributes;
    const bool purple = profile & 1, suixing = profile & 2, qisheng = profile & 4, guchang = profile & 8;
    const bool set_attribute = profile & tx::SetAttributeProfile;
    const auto attack = a.attack_base*(1024+a.attack_percent+(purple ? 256 : 0)+(qisheng ? 102 : 0))/1024 + a.attack_by_class;
    const int suixing_power = suixing ? ((c.talents & tx::FuYin) ? 205 : 102) : 0;
    const int lieyun_power = (profile & 16) ? 154 : 0;
    const auto overcome = a.overcome_base < 0 ? a.overcome :
        a.overcome_base*(1024+a.overcome_percent+204*((profile & tx::XuanMenProfileMask)/tx::XuanMenProfileUnit))/1024 + a.overcome_by_class;
    const auto haste = 1024.0 / (static_cast<int>(a.haste / (11.695*8250) * 1024) + a.haste_percent + 1024);
    const auto wanxiang_interval = static_cast<int>(48 * haste);
    value_t raw;
    if (surplus) raw = EffectDamage(SurplusDamage(a.surplus, param.channel, 120), a.damage_percent);
    else raw = count * EffectDamageAll(attack, dot ? PhysicsDotDamageCoefficient(param.channel, dot_count,
        effect >= tx::WanXiangEffect && effect < tx::WanXiangEffect + 3 ? wanxiang_interval : 48) : PhysicsDamageCoefficient(param.channel, 0),
        a.weapon, param.weapon, param.fixed, a.damage_percent+skill_damage);
    return FinalPhysicsDamage(120, c.target.level, raw, c.target.shield, 0, a.ignore_shield+(guchang ? 614 : 0), 0,
        overcome, crit ? 1 : 0, a.crit_power, a.crit_power_percent+skill_power+(purple ? 256 : 0)+suixing_power+lieyun_power+(set_attribute ? 41 : 0),
        a.strain, a.strain_percent, a.pve_percent, c.target.vulnerable);
}

void ArithmeticOracle() {
    std::mt19937 rng(49271);
    for (unsigned i = 0; i < 120; ++i) {
        tx::Config c;
        c.talents = tx::SupportedTalents & ((i % 2) ? ~tx::FuYin : ~0u);
        if (i % 3) c.talents &= ~tx::XuJi;
        if (i % 2) c.equipment = tx::WeaponCW;
        c.renjian_recipes = tx::Damage40|tx::Damage60|tx::Dot|tx::Cooldown;
        c.recipes = {tx::Damage3|tx::Damage4|tx::Damage5|tx::Qidian,
                     tx::Damage3|tx::Damage5|tx::Cooldown, tx::Damage4|tx::Damage5|tx::Crit4};
        auto &a = c.attributes;
        a.attack_base = rng()%60000; a.attack_by_class = rng()%15000; a.attack_percent = rng()%2048;
        a.weapon = rng()%10000; a.crit_power = rng()%50000; a.overcome = rng()%100000;
        a.overcome_base = rng()%100000; a.overcome_by_class = rng()%20000; a.overcome_percent = rng()%2048;
        a.strain = rng()%60000; a.surplus = rng()%12000;
        a.damage_percent = rng()%2048; a.crit_power_percent = rng()%1024;
        a.strain_percent = rng()%1024; a.pve_percent = rng()%1024; a.ignore_shield = rng()%1025;
        c.target.shield = rng()%200000; c.target.vulnerable = rng()%1024; c.target.level = 110+rng()%21;
        auto p = tx::Prepare(c);
        for (unsigned snapshot = 0; snapshot < tx::ProfileCount; ++snapshot)
            for (unsigned effect = 0; effect < tx::EffectCount; ++effect)
                for (unsigned critical = 0; critical < 2; ++critical)
                    CHECK(p->damage[snapshot][effect][critical] == ReferenceDamage(c, effect, snapshot, critical));
    }
    tx::Config c;
    c.talents = tx::WuYi;
    const auto p = tx::Prepare(c);
    CHECK(p->critical_chance[0][tx::WuWoEffect+5] > p->critical_chance[0][tx::PoZhaoEffect] + .09);
    CHECK(p->critical_chance[0][tx::DieRenEffect+1] == p->critical_chance[0][tx::PoZhaoEffect]);
    CHECK(std::abs(p->critical_chance[tx::SuiXingProfile][tx::PoZhaoEffect] - p->critical_chance[0][tx::PoZhaoEffect] - .05) < 1e-12);
}

void TimingsAndResources() {
    auto r = Run("/scast wuwu\n/scast sanchai", {}, 1);
    CHECK(r.hits.size() == 3 && r.hits[0].level == 9 && r.hits[1].skill == tx::PoZhao);
    CHECK(r.state.last_skill == tx::WuWo && r.state.qidian == 0);
    CHECK(r.state.SkillReadyAt(tx::BaHuang) == 24 && r.state.SkillReadyAt(tx::SanChai) == 22);
    r = Run("/cast sanchai", {}, 67);
    CHECK(r.hits.size() == 4 && r.hits[3].frame == 66 && r.state.last_skill == INVALID_SLOT);
    r = Run("/cast sanhuan", {}, 65);
    CHECK(r.hits.size() == 9 && r.hits[3].frame == 32 && r.hits[6].frame == 64);
    r = Run<MixedRolls>("/scast sanhuan", {}, 1);
    CHECK(r.hits.size() == 3 && r.hits[0].outcome == RollResult::DOUBLE && r.hits[1].outcome == RollResult::HIT && r.hits[2].outcome == RollResult::DOUBLE);
    tx::Config c;
    c.initial_qidian = 0; c.talents = tx::ShenMai;
    r = Run("/scast sanchai\n/scast sanhuan", c, 1);
    CHECK(r.state.qidian == 7); // Initial ClassFeature point plus unconditional ShenMai procs.
    c = {}; c.attributes.haste = 10000; c.recipes[tx::SanHuan] = tx::Cooldown;
    auto p = tx::Prepare(c);
    r = Run("/cast sanhuan", c, 100);
    CHECK(r.hits[3].frame == p->gcd && p->sanhuan_cd == p->gcd);
    r = Run("/scast bahuang\n/scast [skill_cd:wuwu=0] wuwu", {}, 25);
    CHECK(r.hits[2].frame == 24);
    r = Run("/scast sanhuan\n/scast [skill_cd:wuwu<1] ziqi", {}, 10);
    CHECK(r.hits.back().skill == tx::ZiQi && r.hits.back().frame == 9);
    c = {}; c.initial_qidian = 0;
    r = Run("/cast wuwu", c, 100);
    CHECK(r.hits.size() == 5 && r.hits.front().frame == 0 && r.hits.front().level == 0);
    c.target.distance = 5;
    r = Run("/cast sanchai", c, 100);
    CHECK(r.hits.empty());
}

void NaturalQidianRegeneration() {
    using Rules = tx::BasicRules<tx::SplitMixRolls, FixedQidianPhase>;
    auto idle = Compile("/cast [qidian>10] sanchai");
    auto threshold = Compile("/scast [qidian>=3] wuwu");
    tx::Config c; c.initial_qidian = 0;
    const auto prepared = tx::Prepare(c);
    std::array<bool, 16> phases_seen{};
    for (unsigned seed = 0; seed < 1024; ++seed) {
        const auto phase = tx::NaturalQidianPhase{}(seed);
        CHECK(phase >= 0 && phase < 16 && phase == tx::NaturalQidianPhase{}(seed));
        phases_seen[phase] = true;
    }
    CHECK(std::all_of(phases_seen.begin(), phases_seen.end(), [](bool seen) { return seen; }));
    for (tick_t phase = 0; phase < 16; ++phase) {
        Simulation<Rules> fast(idle.program, Rules(prepared, {}, {phase}));
        for (const tick_t end : {0, 1, 15, 16, 17, 31, 32, 160, 4800}) {
            fast.Start(end, 7);
            CHECK(fast.GetState().qidian == 1 && fast.Log().Mutations().empty());
            fast.Run();
            const auto ticks = end > phase ? 1 + (end - 1 - phase) / 16 : 0;
            CHECK(fast.GetState().qidian == std::min(10, 1 + ticks));
            CHECK(fast.Timers().Pending() == 1);
            CHECK(fast.Log().Mutations().size() == static_cast<std::size_t>(std::min(9, ticks)));
            for (unsigned i = 0; i < fast.Log().Mutations().size(); ++i) {
                const auto &event = fast.Log().Mutations()[i];
                CHECK(event.kind == MutationKind::Resources);
                CHECK(event.frame == phase + static_cast<tick_t>(i)*16 && event.a == 2 + static_cast<int>(i));
            }
        }
        Simulation<Rules> wake(threshold.program, Rules(prepared, {}, {phase}));
        Simulation<Rules, true> debug(threshold.program, Rules(prepared, {}, {phase}));
        wake.Start(80, 7); wake.Run();
        debug.Start(80, 7);
        while (debug.Continue()) {}
        CHECK(wake.Log().Size() == 1 && wake.Log().Intents()[0].frame == phase + 16);
        CHECK(wake.Log().Intents()[0].level == 2);
        CHECK(wake.Log().Mutations() == debug.Log().Mutations());
        CHECK(wake.Log().Intents() == debug.Log().Intents());
        CHECK(wake.TotalDamage() == debug.TotalDamage());
        // The restoring resource mutation precedes the macro's same-frame hit.
        const auto &hit = wake.Log().Intents()[0];
        CHECK(std::any_of(wake.Log().Mutations().begin(), wake.Log().Mutations().end(), [&](const auto &m) {
            return m.frame == hit.frame && m.kind == MutationKind::Resources && m.a == 3 && m.sequence < hit.sequence;
        }));
    }
    auto attacks = Compile("/cast sanchai");
    Simulation<Rules> zero(attacks.program, Rules(tx::Prepare(), {}, {0}));
    Simulation<Rules> last(attacks.program, Rules(tx::Prepare(), {}, {15}));
    zero.Start(400, 8); zero.Run(); last.Start(400, 8); last.Run();
    CHECK(zero.Log().Intents() == last.Log().Intents()); // Phase sampling does not consume critical rolls.
    Simulation<Rules> invalid(idle.program, Rules(prepared, {}, {16}));
    Rejects([&] { invalid.Start(10, 0); });
}

void ChargesAndSnapshots() {
    auto r = Run("/cast ziqi", {}, 1201);
    CHECK(r.hits.size() == 3 && r.hits[0].frame == 0 && r.hits[1].frame == 0 && r.hits[2].frame == 1200);
    CHECK(r.state.skill_energy[tx::ZiQi] == 0 && r.state.skill_ready_at[tx::ZiQi] == 2400);
    tx::Config c; c.talents = tx::RuoShui;
    r = Run("/scast ziqi\n/scast ziqi\n/scast ziqi", c, 480);
    CHECK(r.hits.size() == 3 && r.state.self_buff_expires_at[tx::Purple] == 480);
    r = Run("/scast ziqi\n/scast [bufftime:ziqi<1] wuwu", {}, 161);
    CHECK(r.hits[1].frame == 145 && r.state.qidian == 3);
    CHECK(r.state.self_buff_stacks[tx::Purple] == 0); // Last resource tick precedes clear.
    c = {}; c.talents = tx::DieRenTalent;
    r = Run("/scast ziqi\n/scast wuwu", c, 385);
    CHECK(r.hits.size() == 11 && r.state.target_buff_stacks[tx::DieRen] == 0);
    for (std::size_t i = 3; i < r.hits.size(); ++i) {
        CHECK(r.hits[i].frame == static_cast<int>(i-2)*48);
        CHECK(r.hits[i].snapshot == 1); // Purple expired at 160; DOT remains snapshotted.
    }
    r = Run("/scast wuwu\n/scast ziqi\n/scast wuwu", c, 410);
    CHECK(r.hits[5].frame == 48 && r.hits[5].level == 1 && r.hits[5].snapshot == 1);
    CHECK(r.hits.back().frame == 384 && r.hits.size() == 13);
    c.attributes.haste = 10000;
    r = Run("/scast wuwu", c, 400);
    CHECK(r.hits.size() == 10); // Eight ticks with legacy fractional-duration rounding.
}

void TalentOrder() {
    tx::Config c; c.talents = tx::FengShiTalent;
    auto r = Run("/scast wuwu", c, 1);
    CHECK(r.state.target_buff_stacks[tx::DieRen] == 1);
    r = Run("/scast bahuang\n/scast wuwu", c, 25);
    CHECK(r.hits[2].effect == tx::FengWuWoEffect+9 && r.state.self_buff_stacks[tx::FengShi] == 0);
    c.talents = tx::DieRenTalent|tx::WuYu;
    r = Run("/scast wuwu\n/scast bahuang\n/scast ziqi\n/scast wuwu", c, 49);
    CHECK(r.state.skill_ready_at[tx::BaHuang] == 216-88);
    c.talents = tx::DieRenTalent|tx::QieYu; c.target.life = .4;
    r = Run("/scast wuwu\n/scast bahuang", c, 25);
    CHECK(r.hits.size() == 5 && r.hits.back().sub == 1);
    CHECK(tx::Formulas[r.hits.back().effect].count == 7 && r.state.target_buff_stacks[tx::DieRen] == 0);
    c = {}; c.talents = tx::DieRenTalent|tx::HuanYue;
    r = Run("/scast wuwu\n/scast sanhuan", c, 385);
    CHECK(r.hits.size() == 16 && r.hits[8].skill == tx::DieRenDamage);
    CHECK(r.hits[5].skill == tx::PoZhao && r.hits[5].sub == 1);
    CHECK(r.hits[8].frame == 24 && r.hits.back().frame == 240);
    CHECK(r.state.qidian == 10);
}

void PreparedFields() {
    tx::Config c; c.initial_qidian = 0;
    constexpr auto script = "/scast suixing\n/sfcast ziqi\n/scast sanchai";
    auto r = Run(script, c, 16);
    CHECK(r.hits.empty() && r.state.preparing_skill == tx::SuiXing);
    CHECK(r.state.skill_prepare_at[tx::SuiXing] == 16 && r.state.last_skill == INVALID_SLOT);
    CHECK(r.state.qidian == 2 && r.state.self_buff_stacks[tx::SuiXingBuff] == 0);
    r = Run(script, c, 17);
    CHECK(r.hits.size() == 3 && r.hits[0].skill == tx::SuiXing && r.hits[0].frame == 16);
    CHECK(r.hits[1].skill == tx::ZiQi && r.hits[2].skill == tx::SanChai);
    CHECK(r.state.preparing_skill == INVALID_SLOT && r.state.last_skill == tx::ZiQi);
    CHECK(r.state.skill_ready_at[tx::SuiXing] == 176 && r.state.gcd_ready_at[0] == 24);
    CHECK(r.state.self_buff_expires_at[tx::SuiXingBuff] == 80);
    r = Run("/scast suixing\n/scast [last_skill:suixing] wuwu", c, 25);
    CHECK(r.hits.size() == 2 && r.hits[1].frame == 24 && r.hits[1].level == 3);
    CHECK(r.hits[1].snapshot == tx::SuiXingProfile);
    c.shengtaiji_recipes = tx::Prepare1|tx::Prepare2|tx::Prepare3|tx::Cooldown;
    r = Run("/cast shengtaiji", c, 149);
    CHECK(r.hits.size() == 2 && r.hits[0].frame == 10 && r.hits[1].frame == 148);
    CHECK(r.state.skill_ready_at[tx::ShengTai] == 276);
    c.attributes.haste = 10000;
    auto p = tx::Prepare(c);
    r = Run("/scast shengtaiji", c, 100);
    CHECK(r.hits[0].frame == p->shengtaiji_prepare && r.state.skill_ready_at[tx::ShengTai] == p->shengtaiji_prepare+128);
    c = {}; c.target.distance = 100;
    r = Run("/scast tunriyue", c, 17);
    CHECK(r.hits.size() == 1 && r.state.target_buff_stacks[tx::TunRiBuff] == 1);
    c = {}; c.talents = tx::FuYin; c.attributes.haste_percent = 10000;
    r = Run("/scast suixing\n/scast shengtaiji\n/scast tunriyue", c, 6);
    CHECK(r.hits.size() == 3 && r.hits[0].frame == 1 && r.hits[1].frame == 3);
    c.shengtaiji_recipes = tx::Prepare1|tx::Prepare2|tx::Prepare3;
    r = Run("/scast shengtaiji\n/scast sanchai", c, 1);
    CHECK(r.hits.size() == 2 && r.hits[0].frame == 0 && r.state.preparing_skill == INVALID_SLOT);
}

void FieldOwnership() {
    auto r = Run("/scast suixing", {}, 401);
    CHECK(r.state.self_buff_stacks[tx::FieldSuiXing] == 0);
    CHECK(r.state.self_buff_stacks[tx::SuiXingBuff] == 1 && r.state.self_buff_expires_at[tx::SuiXingBuff] == 464);
    r = Run("/scast suixing", {}, 465);
    CHECK(r.state.self_buff_stacks[tx::SuiXingBuff] == 0);
    r = Run("/scast tunriyue", {}, 401);
    CHECK(r.state.self_buff_stacks[tx::FieldTunRi] == 0 && r.state.target_buff_expires_at[tx::TunRiBuff] == 416);
    r = Run("/scast tunriyue", {}, 417);
    CHECK(r.state.target_buff_stacks[tx::TunRiBuff] == 0);
    tx::Config c; c.talents = tx::FuYin;
    r = Run("/scast suixing", c, 593);
    CHECK(r.state.self_buff_stacks[tx::FieldSuiXing] == 0 && r.state.self_buff_expires_at[tx::SuiXingBuff] == 656);
    r = Run("/scast suixing\n/scast suixing\n/scast shengtaiji\n/scast tunriyue", {}, 241);
    CHECK(r.hits.size() == 4 && r.hits[1].frame == 192 && r.hits[3].frame == 240);
    CHECK(r.state.self_buff_stacks[tx::FieldSuiXing] == 1 && r.state.self_buff_stacks[tx::FieldShengTai] == 1 && r.state.self_buff_stacks[tx::FieldTunRi] == 1);
    for (const auto &field : r.fields) CHECK(field.order != 0 && field.skill != INVALID_SLOT);
    CHECK(r.state.self_buff_expires_at[tx::FieldSuiXing] == 576); // The oldest global instance was evicted.
    r = Run("/scast suixing\n/scast suixing\n/scast shengtaiji\n/scast tunriyue", {}, 401);
    CHECK(r.state.self_buff_stacks[tx::FieldTunRi] == 1 && r.state.self_buff_expires_at[tx::FieldTunRi] == 624);
    c = {}; c.talents = tx::QiSheng;
    r = Run("/scast shengtaiji", c, 401);
    CHECK(r.state.self_buff_expires_at[tx::QiShengBuff] == 448);
    r = Run("/scast shengtaiji", c, 449);
    CHECK(r.state.self_buff_stacks[tx::QiShengBuff] == 0);
}

void FieldDamageInputs() {
    tx::Config c; c.talents = tx::DieRenTalent|tx::GuChang;
    auto r = Run("/scast wuwu\n/scast suixing", c, 49);
    CHECK(r.hits.back().skill == tx::DieRenDamage && r.hits.back().frame == 48);
    CHECK(r.hits.back().snapshot == tx::LiveGuChangProfile); // Live ignore, old crit/AP snapshot.
    r = Run("/scast suixing\n/scast [bufftime:suixing<1] wuwu", c, 498);
    CHECK(r.hits[1].frame == 449 && r.hits[1].snapshot == (tx::SuiXingProfile|tx::LiveGuChangProfile));
    CHECK(r.hits.back().frame == 497 && r.hits.back().snapshot == tx::SuiXingProfile);
    c.talents |= tx::QieYu; c.target.life = .5;
    r = Run("/scast suixing\n/scast wuwu\n/scast bahuang", c, 49);
    CHECK(r.state.target_buff_stacks[tx::DieRen] != 0); // SuiXing must not satisfy Purple's execute threshold.
    c = {}; c.talents = tx::ChangSheng|tx::FuYin|tx::QiSheng|tx::GuChang;
    r = Run("/scast suixing\n/scast shengtaiji\n/scast tunriyue\n/scast wuwu\n/scast bahuang\n/scast sanhuan", c, 121);
    CHECK(r.hits.size() == 13 && r.state.self_buff_stacks[tx::ChiYingBuff] == 0);
    CHECK(r.hits[5].skill == tx::ChiYingDamage && r.hits[8].skill == tx::ChiYingDamage && r.hits[12].skill == tx::ChiYingDamage);
    CHECK(r.hits[5].frame == 72 && r.hits[8].frame == 96 && r.hits[12].frame == 120);
    CHECK(r.hits[3].snapshot == (tx::SuiXingProfile|tx::QiShengProfile|tx::LiveGuChangProfile));
    r = Run("/scast suixing\n/scast sanchai", c, 17);
    CHECK(r.state.self_buff_stacks[tx::ChiYingBuff] == 1); // Autoattack does not consume ChiYing.
    r = Run("/scast suixing", c, 257);
    CHECK(r.state.self_buff_stacks[tx::ChiYingBuff] == 0);
}

constexpr std::string_view FieldRotation =
    "/cast [nobuff:suixing] suixing\n/cast [nobuff:qisheng] shengtaiji\n"
    "/cast [tnobuff:tunriyue] tunriyue\n/cast [nobuff:ziqi&qidian<6] ziqi\n"
    "/cast [qidian>=8] wuwu\n/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";

void FieldExplosions() {
    tx::Config c; c.initial_qidian = 0; c.talents = tx::ShenMai|tx::XuJi;
    auto r = Run("/scast wanjian", c, 1);
    CHECK(r.hits.size() == 1 && r.hits[0].skill == tx::WanJian && r.state.qidian == 4);
    CHECK(r.state.skill_ready_at[tx::WanJian] == 160 && r.state.gcd_ready_at[0] == 24);
    CHECK(r.state.target_buff_stacks[tx::DieRen] == 1);
    auto p = tx::Prepare(c);
    CHECK(p->critical_chance[0][tx::WanJianEffect] == p->critical_chance[0][tx::PoZhaoEffect]);
    CHECK(p->Input(tx::WanJianEffect, 0).crit_power_percent == 1102);
    c = {}; c.target.distance = 8;
    r = Run("/scast wanjian", c, 1); CHECK(r.hits.size() == 1 && r.state.skill_ready_at[tx::WanJian] == 192);
    c.target.distance = 8.01;
    r = Run("/scast wanjian", c, 1); CHECK(r.hits.empty());

    c = {}; c.talents = tx::XuanMen|tx::ChangSheng|tx::ShenMai;
    c.attributes.overcome_base = 20000; c.attributes.overcome_by_class = 5000;
    c.renjian_recipes = tx::Cooldown|tx::Dot|tx::Damage40|tx::Damage60;
    constexpr auto three = "/scast suixing\n/scast shengtaiji\n/scast tunriyue\n/scast renjian";
    r = Run(three, c, 73);
    CHECK(r.hits.size() == 7 && r.hits[3].frame == 72 && r.hits[6].skill == tx::ChiYingDamage);
    CHECK(r.state.skill_ready_at[tx::RenJian] == 312 && r.state.skill_ready_at[tx::RenJianSuiXing] == 312);
    CHECK(r.state.self_buff_stacks[tx::XuanMenBuff] == 3 && r.state.self_buff_expires_at[tx::XuanMenBuff] == 712);
    CHECK(r.state.self_buff_stacks[tx::ChiYingBuff] == 2);
    for (unsigned i = 3; i < 7; ++i) CHECK(!(r.hits[i].snapshot & tx::XuanMenProfileMask));
    for (const auto &field : r.fields) CHECK(field.skill == INVALID_SLOT);
    CHECK(r.state.target_buff_tick_at[tx::RenJianDot] == 120 && r.state.target_buff_expires_at[tx::RenJianDot] == 264);
    r = Run(three, c, 265);
    CHECK(r.hits.size() == 11 && r.state.target_buff_stacks[tx::RenJianDot] == 0);
    for (unsigned i = 7; i < 11; ++i) {
        CHECK(r.hits[i].frame == 120+static_cast<int>(i-7)*48 && r.hits[i].skill == tx::RenJianDamage);
        CHECK((r.hits[i].snapshot & tx::XuanMenProfileMask) == 3*tx::XuanMenProfileUnit);
    }
    c.shengtaiji_recipes = tx::Cooldown;
    CHECK(tx::Prepare(c)->shengtaiji_cd == 368);
    c.shengtaiji_recipes = 0;
    CHECK(tx::Prepare(c)->shengtaiji_cd == 400);
    r = Run("/scast renjian\n/scast renjian_suixing", c, 241);
    CHECK(r.hits.empty() && r.state.last_skill == tx::RenJianSuiXing);
    CHECK(r.state.skill_ready_at[tx::RenJian] == 480 && r.state.self_buff_stacks[tx::XuanMenBuff] == 0);
    CHECK(r.state.self_buff_expires_at[tx::XuanMenBuff] == 880);
    r = Run("/scast renjian\n/scast [buff:xuanmen] sanchai\n/scast [buff:xuanmen>0] sanchai", c, 1);
    CHECK(r.hits.empty() && r.state.self_buff_expires_at[tx::XuanMenBuff] == 640);
    const auto existence = Compile("/cast [buff:xuanmen] sanchai\n/cast [buff:xuanmen>0] sanchai\n/cast [nobuff:xuanmen] sanchai");
    for (int stacks = 0; stacks <= 3; ++stacks) {
        r.state.self_buff_stacks[tx::XuanMenBuff] = stacks;
        CHECK(EvaluateCondition(r.state, existence.program.conditions[0]) == (stacks > 0));
        CHECK(EvaluateCondition(r.state, existence.program.conditions[1]) == (stacks > 0));
        CHECK(EvaluateCondition(r.state, existence.program.conditions[2]) == (stacks == 0));
    }
    r = Run("/scast suixing\n/scast renjian\n/scast renjian_suixing", c, 265);
    CHECK(r.state.self_buff_stacks[tx::XuanMenBuff] == 1 && r.state.self_buff_expires_at[tx::XuanMenBuff] == 904);

    c = {};
    constexpr auto prefix = "/scast suixing\n/scast suixing\n/scast shengtaiji\n/scast renjian_suixing";
    r = Run(prefix, c, 225);
    CHECK(r.hits.size() == 4 && r.hits.back().skill == tx::RenJianSuiXing && r.hits.back().frame == 224);
    CHECK(r.state.self_buff_stacks[tx::FieldSuiXing] == 1 && r.state.self_buff_stacks[tx::FieldShengTai] == 1);
    CHECK(r.state.self_buff_expires_at[tx::FieldSuiXing] == 576);
    r = Run("/scast suixing\n/scast shengtaiji\n/scast suixing\n/scast renjian_suixing", c, 201);
    CHECK(r.hits.size() == 5 && r.state.self_buff_stacks[tx::FieldShengTai] == 0 && r.state.self_buff_stacks[tx::FieldSuiXing] == 1);
    c.target.distance = 8; c.renjian_recipes = tx::Range1|tx::Range2;
    r = Run("/scast suixing\n/scast renjian", c, 25); CHECK(r.hits.size() == 2);
    c.renjian_recipes = tx::Range1;
    r = Run("/scast suixing\n/scast renjian", c, 25); CHECK(r.hits.size() == 1 && r.state.skill_ready_at[tx::RenJian] == 0);
    c = {}; c.renjian_recipes = tx::Dot; c.attributes.haste = 10000;
    p = tx::Prepare(c);
    r = Run("/scast suixing\n/scast renjian", c, 250);
    CHECK(r.hits.size() == 6 && r.hits[2].frame == p->gcd+p->dot_interval);
    CHECK(r.hits.back().frame == p->gcd+4*p->dot_interval && r.state.target_buff_stacks[tx::RenJianDot] == 0);
}

void SplitSnapshots() {
    tx::Config c; c.talents = tx::XuanMen|tx::DieRenTalent;
    c.attributes.overcome_base = 17003; c.attributes.overcome_by_class = 4321; c.attributes.overcome_percent = 157;
    auto p = tx::Prepare(c);
    CHECK(p->Input(tx::PoZhaoEffect, tx::XuanMenProfileUnit).overcome == 17003*(1024+157+204)/1024+4321);
    auto r = Run<ThresholdRolls>("/scast wuwu\n/scast suixing\n/scast renjian", c, 97);
    bool saw_live = false;
    for (const auto &hit : r.hits) if (hit.skill == tx::DieRenDamage && hit.frame == 96) {
        CHECK(hit.snapshot == tx::XuanMenProfileUnit && hit.outcome == RollResult::HIT);
        CHECK(p->damage[hit.snapshot][hit.effect][0] > p->damage[0][hit.effect][0]);
        saw_live = true;
    }
    CHECK(saw_live);
    r = Run<ThresholdRolls>("/scast suixing\n/scast renjian\n/scast [bufftime:xuanmen<1] wuwu", c, 698);
    CHECK(r.hits[2].frame == 649 && r.hits[2].outcome == RollResult::DOUBLE);
    CHECK(r.hits.back().skill == tx::DieRenDamage && r.hits.back().frame == 697);
    CHECK(r.hits.back().snapshot == 0 && r.hits.back().outcome == RollResult::DOUBLE);
    CHECK(r.state.self_buff_stacks[tx::XuanMenBuff] == 0);
    c.renjian_recipes = tx::Dot;
    r = Run<ThresholdRolls>("/scast shengtaiji\n/scast renjian", c, 73);
    CHECK(r.hits.back().skill == tx::RenJianDamage && r.hits.back().snapshot == tx::XuanMenProfileUnit);
    CHECK(r.hits.back().outcome == RollResult::HIT); // XuanMen's crit bonus was not snapshotted.
}

void LieYunLifetime() {
    tx::Config c; c.talents = tx::DieRenTalent|tx::FengShiTalent|tx::LieYun;
    constexpr auto four = "/scast wuwu\n/scast ziqi\n/scast wuwu";
    auto r = Run(four, c, 25);
    CHECK(r.state.target_buff_stacks[tx::DieRen] == 4 && r.state.self_buff_stacks[tx::LieYunBuff] == 1);
    CHECK(r.state.target_buff_expires_at[tx::HiddenLieYun] == 408 && r.state.target_buff_tick_at[tx::HiddenLieYun] == 32);
    r = Run(four, c, 49);
    CHECK(r.hits.back().skill == tx::DieRenDamage && r.hits.back().snapshot == (tx::PurpleProfile|tx::LieYunProfile));
    r = Run(four, c, 385);
    CHECK(r.state.target_buff_stacks[tx::DieRen] == 0 && r.state.target_buff_expires_at[tx::HiddenLieYun] == 768);
    CHECK(r.state.self_buff_expires_at[tx::LieYunBuff] == 400);
    r = Run(four, c, 769);
    CHECK(r.state.target_buff_stacks[tx::HiddenLieYun] == 0 && r.state.self_buff_expires_at[tx::LieYunBuff] == 784);
    r = Run(four, c, 785); CHECK(r.state.self_buff_stacks[tx::LieYunBuff] == 0);

    c.talents |= tx::QieYu; c.target.life = .4;
    r = Run(std::string(four)+"\n/scast bahuang", c, 49);
    CHECK(r.state.target_buff_stacks[tx::DieRen] == 0 && r.state.target_buff_expires_at[tx::HiddenLieYun] == 432);
    r = Run(std::string(four)+"\n/scast bahuang\n/scast wuwu", c, 73);
    CHECK(r.state.target_buff_stacks[tx::HiddenLieYun] == 0 && r.state.self_buff_expires_at[tx::LieYunBuff] == 88);
    CHECK(r.state.target_buff_stacks[tx::DieRen] == 2);
    c.talents = tx::DieRenTalent|tx::FengShiTalent|tx::LieYun|tx::HuanYue;
    r = Run(std::string(four)+"\n/scast sanhuan\n/scast sanhuan", c, 145);
    CHECK(r.state.target_buff_stacks[tx::DieRen] == 0 && r.state.target_buff_stacks[tx::HiddenLieYun] == 0);
    CHECK(r.state.self_buff_expires_at[tx::LieYunBuff] == 160);
    r = Run(std::string(four)+"\n/scast sanhuan\n/scast sanhuan", c, 161);
    CHECK(r.state.self_buff_stacks[tx::LieYunBuff] == 0);
    c.talents |= tx::ShenMai;
    r = Run(std::string(four)+"\n/scast wuwu\n/scast wuwu", c, 73);
    CHECK(r.state.target_buff_stacks[tx::DieRen] == 7);
    c.talents &= ~tx::LieYun;
    r = Run(std::string(four)+"\n/scast wuwu\n/scast wuwu", c, 73);
    CHECK(r.state.target_buff_stacks[tx::DieRen] == 5 && r.state.target_buff_stacks[tx::HiddenLieYun] == 0);
}

void JianRuLifetime() {
    tx::Config c; c.talents = tx::JianRu|tx::ShenMai; c.initial_qidian = 0;
    auto r = Run("/scast ziqi\n/scast bahuang\n/scast wanjian", c, 123);
    CHECK(r.hits.size() == 8);
    CHECK(r.hits[0].skill == tx::ZiQi && r.hits[1].skill == tx::BaHuang);
    CHECK(r.hits[3].skill == tx::WanJian && r.hits[3].frame == 24);
    CHECK(r.hits[4].skill == tx::JianRuDamage && r.hits[4].effect == tx::JianRuEffect && r.hits[4].frame == 40);
    CHECK(r.hits[5].frame == 56 && r.hits[6].frame == 72 && r.hits[7].frame == 88);
    CHECK(r.state.self_buff_stacks[tx::JianRuBuff] == 0);
    CHECK(r.hits[5].snapshot == tx::PurpleProfile);

    c = {}; c.talents = tx::JianRu; c.initial_qidian = 0;
    r = Run("/scast bahuang", c, 99);
    CHECK(r.hits.size() == 2 && r.state.self_buff_stacks[tx::JianRuBuff] == 0);
    r = Run("/scast ziqi\n/scast bahuang", c, 97);
    CHECK(r.hits.size() == 3 && r.state.self_buff_stacks[tx::JianRuBuff] == 1);
    CHECK(r.state.self_buff_expires_at[tx::JianRuBuff] == 98);
}

void YunZhongLifetime() {
    tx::Config c; c.renjian_recipes = tx::YunZhongSuiXing;
    auto r = Run("/scast suixing\n/scast renjian", c, 170);
    CHECK(r.hits.size() == 10);
    CHECK(r.hits[0].skill == tx::SuiXing && r.hits[0].frame == 16);
    CHECK(r.hits[1].skill == tx::RenJian && r.hits[1].frame == 24);
    for (unsigned i = 2; i < 10; ++i) {
        CHECK(r.hits[i].skill == tx::YunZhongDamage);
        CHECK(r.hits[i].effect == tx::YunZhongSuiXingEffect);
        CHECK(r.hits[i].frame == 40 + static_cast<int>(i-2)*16);
    }
    CHECK(r.state.self_buff_stacks[tx::YunZhongSuiXingBuff] == 0);

    // Keep RenJian in range while checking YunZhongJian's stricter six-chi range.
    c.renjian_recipes |= tx::Range1;
    c.target.distance = 6.1;
    r = Run("/scast suixing\n/scast renjian", c, 80);
    CHECK(r.hits.size() == 2 && r.hits[1].skill == tx::RenJian);
    CHECK(r.state.self_buff_stacks[tx::YunZhongSuiXingBuff] == 1);
}

void JingHuaLifetime() {
    tx::Config c; c.talents = tx::JingHuaTalent;
    auto r = Run("/scast tunriyue\n/scast wanjian\n/scast jinghua", c, 160);
    unsigned immediate = 0, fragment = 0;
    for (const auto &hit : r.hits) {
        if (hit.effect == tx::JingHuaEffect) {
            ++immediate;
            CHECK(hit.frame == 48 && hit.sub == 0);
        } else if (hit.effect == tx::JingHuaSuiEffect) {
            ++fragment;
            CHECK(hit.frame == 96 && hit.sub == 1);
        }
    }
    CHECK(immediate == 1 && fragment == 1);
    CHECK(r.state.self_buff_stacks[tx::JingHuaBuff] == 0);
    CHECK(r.state.skill_ready_at[tx::JingHua] == std::numeric_limits<tick_t>::max());

    c = {};
    r = Run("/scast jinghua", c, 80);
    CHECK(r.hits.empty());
}

void WeaponCwLifetime() {
    struct SixteenthProc {
        unsigned count = 0;
        void Reset(std::uint64_t) { count = 0; }
        std::uint64_t Next() { return 0; }
        bool Proc(int numerator, int denominator) {
            CHECK(numerator == 31 && denominator == 1024);
            return ++count == 16;
        }
    };
    tx::Config c; c.equipment = tx::WeaponCW;
    auto r = Run<SixteenthProc>("/cast sanhuan", c, 480);
    CHECK(r.state.self_buff_stacks[tx::WeaponCwBuff] == 0);
    r = Run<SixteenthProc>("/cast sanhuan", c, 481);
    CHECK(r.state.self_buff_stacks[tx::WeaponCwBuff] == 1);
    CHECK(r.state.self_buff_expires_at[tx::WeaponCwBuff] == 576);
    r = Run<HitsOnly>("/cast bahuang", c, 1000);
    CHECK(r.state.target_buff_stacks[tx::WanXiangBuff] == 0);
    CHECK(std::none_of(r.hits.begin(), r.hits.end(), [](const auto &hit) { return hit.skill == tx::WanXiangDamage; }));

    const auto macro = "/scast bahuang\n/scast bahuang\n/scast bahuang\n/scast bahuang\n/scast bahuang";
    r = Run<AlwaysProc>(macro, c, 1057);
    CHECK(r.hits.size() == 25);
    const std::array<tick_t, 5> frames{0, 192, 384, 576, 600};
    for (unsigned cast = 0; cast < frames.size(); ++cast) {
        CHECK(r.hits[cast*3].effect == tx::BaHuangEffect && r.hits[cast*3].frame == frames[cast]);
        CHECK(r.hits[cast*3+1].effect == (cast == 4 ? tx::BaHuangCwEffect : tx::BaHuangExtraEffect));
        CHECK(r.hits[cast*3+2].effect == tx::WeaponCwExtraEffect);
    }
    // Refresh at 600 keeps the tick at 624 and the tenth/final tick at 1056.
    for (unsigned i = 15; i < r.hits.size(); ++i) {
        CHECK(r.hits[i].effect == tx::WanXiangEffect + 1);
        CHECK(r.hits[i].frame == 624 + static_cast<int>(i-15)*48);
        CHECK(r.hits[i].sub == 2);
    }
    CHECK(r.state.self_buff_stacks[tx::WeaponCwBuff] == 0);
    CHECK(r.state.target_buff_stacks[tx::WanXiangBuff] == 0);
    CHECK(r.state.skill_ready_at[tx::BaHuang] == 672);
    r = Run<AlwaysProc>(macro, c, 1056);
    CHECK(r.hits.size() == 24 && r.state.target_buff_stacks[tx::WanXiangBuff] == 2);
}

void SetAttributeLifetime() {
    tx::Config c; c.equipment = tx::SetAttribute;
    auto r = Run("/scast wanjian\n/scast wuwu", c, 119);
    CHECK(r.hits.size() == 3 && r.hits[0].frame == 0 && r.hits[1].frame == 24 && r.hits[2].frame == 24);
    CHECK((r.hits[0].snapshot & tx::SetAttributeProfile) == 0);
    CHECK((r.hits[1].snapshot & tx::SetAttributeProfile) != 0);
    CHECK(r.state.self_buff_stacks[tx::SetAttributeBuff] == 1);
    r = Run("/scast wanjian\n/scast wuwu", c, 121);
    CHECK(r.state.self_buff_stacks[tx::SetAttributeBuff] == 0);
}

constexpr std::string_view ExplosionRotation =
    "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [buff:field_suixing&buff:field_shengtaiji&buff:field_tunriyue] renjian\n"
    "/cast [nobuff:field_suixing] suixing\n/cast [nobuff:field_shengtaiji] shengtaiji\n"
    "/cast [nobuff:field_tunriyue] tunriyue\n/cast wanjian\n/cast jinghua\n/cast [qidian>=8] wuwu\n"
    "/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";

void ExplosionModeParity() {
    auto macro = Compile(ExplosionRotation);
    tx::Config c; c.talents = tx::SupportedTalents; c.attributes.overcome_base = 25000;
    c.equipment = tx::WeaponCW;
    c.renjian_recipes = tx::Cooldown|tx::Dot|tx::Damage40|tx::Damage60;
    const auto data = tx::Prepare(c);
    Simulation<tx::Rules> fast(macro.program, tx::Rules(data));
    Simulation<tx::Rules, true> debug(macro.program, tx::Rules(data));
    bool saw_renjian = false, saw_wanjian = false, saw_lieyun = false;
    for (unsigned seed = 0; seed < 100; ++seed) {
        fast.Start(4800, seed); debug.Start(4800, seed);
        debug.SetBreakpoint(1);
        for (unsigned stop = 0; stop < 12 && debug.Continue(); ++stop) debug.StepOver();
        debug.SetBreakpoint(1, false);
        while (debug.Continue()) {}
        fast.Run();
        CHECK(fast.Log().Intents() == debug.Log().Intents() && fast.Log().Mutations() == debug.Log().Mutations());
        CHECK(fast.GetRules().Fields() == debug.GetRules().Fields());
        value_t reference = 0;
        for (const auto &hit : fast.Log().Intents()) {
            reference += ReferenceDamage(c, hit.effect, hit.snapshot, hit.outcome == RollResult::DOUBLE);
            saw_renjian |= hit.skill == tx::RenJian;
            saw_wanjian |= hit.skill == tx::WanJian;
            saw_lieyun |= (hit.snapshot & tx::LieYunProfile) != 0;
        }
        CHECK(fast.TotalDamage() == reference && debug.TotalDamage() == reference);
    }
    CHECK(saw_renjian && saw_wanjian && saw_lieyun);
    count_allocations = true;
    for (unsigned seed = 0; seed < 100; ++seed) { fast.Start(4800, seed); fast.Run(); (void)fast.TotalDamage(); }
    count_allocations = false;
    CHECK(allocations == 0);
    BatchOptions options; options.iterations = 1000;
    const auto a = RunBatch(macro.program, options, tx::Rules(data));
    options.workers = 4;
    const auto b = RunBatch(macro.program, options, tx::Rules(data));
    CHECK(a.checksum == b.checksum && a.min_damage == b.min_damage && a.max_damage == b.max_damage);
    fast.Start(1, 0); fast.Run();
    CHECK(fast.GetState().self_buff_stacks[tx::XuanMenBuff] == 0 && fast.GetState().self_buff_stacks[tx::LieYunBuff] == 0);
    CHECK(fast.GetState().target_buff_stacks[tx::HiddenLieYun] == 0 && fast.GetState().target_buff_stacks[tx::RenJianDot] == 0);
}

void FieldModeParity() {
    auto macro = Compile(FieldRotation);
    tx::Config c; c.talents = tx::SupportedTalents; c.attributes.overcome_base = 25000;
    auto data = tx::Prepare(c);
    Simulation<tx::Rules> fast(macro.program, tx::Rules(data));
    Simulation<tx::Rules, true> debug(macro.program, tx::Rules(data));
    for (unsigned seed = 0; seed < 100; ++seed) {
        fast.Start(4800, seed); debug.Start(4800, seed);
        debug.SetBreakpoint(1);
        CHECK(debug.Continue() && debug.GetState().preparing_skill == tx::SuiXing);
        CHECK(debug.GetState().now == 0 && debug.Log().Intents().empty());
        debug.SetBreakpoint(1, false);
        while (debug.Continue()) {}
        fast.Run();
        CHECK(fast.Log().Intents() == debug.Log().Intents() && fast.Log().Mutations() == debug.Log().Mutations());
        CHECK(fast.GetRules().Fields() == debug.GetRules().Fields());
        value_t reference = 0;
        for (const auto &hit : fast.Log().Intents()) reference += ReferenceDamage(c, hit.effect, hit.snapshot, hit.outcome == RollResult::DOUBLE);
        CHECK(fast.TotalDamage() == reference && debug.TotalDamage() == reference);
    }
    count_allocations = true;
    for (unsigned seed = 0; seed < 100; ++seed) { fast.Start(4800, seed); fast.Run(); (void)fast.TotalDamage(); }
    count_allocations = false;
    CHECK(allocations == 0);
    BatchOptions options; options.iterations = 1000;
    const auto a = RunBatch(macro.program, options, tx::Rules(data));
    options.workers = 4;
    const auto b = RunBatch(macro.program, options, tx::Rules(data));
    CHECK(a.checksum == b.checksum);
    auto preparing_macro = Compile("/scast suixing");
    Simulation<tx::Rules> restart(preparing_macro.program, tx::Rules(data));
    restart.Start(1, 0); restart.Run();
    CHECK(restart.GetState().preparing_skill == tx::SuiXing);
    restart.Start(17, 0); restart.Run();
    CHECK(restart.Log().Size() == 1 && restart.Log().Intents()[0].frame == 16);
    restart.Start(1, 0); restart.Run();
    CHECK(restart.GetState().self_buff_stacks[tx::SuiXingBuff] == 0);
    for (const auto &field : restart.GetRules().Fields()) CHECK(field.skill == INVALID_SLOT);
}

constexpr std::string_view Rotation =
    "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [qidian>=8] wuwu\n"
    "/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";
void ModeParity() {
    auto macro = Compile(Rotation);
    tx::Config c; c.talents = tx::DieRenTalent|tx::WuYi|tx::ShenMai|tx::FengShiTalent|tx::WuYu|tx::XinGu;
    auto data = tx::Prepare(c);
    Simulation<tx::Rules> fast(macro.program, tx::Rules(data));
    Simulation<tx::Rules, true> debug(macro.program, tx::Rules(data));
    static_assert(sizeof(fast) < sizeof(debug));
    for (unsigned seed = 0; seed < 100; ++seed) {
        fast.Start(4800, seed); debug.Start(4800, seed);
        debug.SetBreakpoint(1);
        CHECK(debug.Continue() && debug.DebugControl().paused);
        debug.StepInto(); debug.StepOver();
        debug.SetBreakpoint(1, false);
        while (debug.Continue()) {}
        fast.Run();
        CHECK(fast.Log().Intents() == debug.Log().Intents());
        CHECK(fast.Log().Mutations() == debug.Log().Mutations());
        CHECK(fast.TotalDamage() == debug.TotalDamage());
        CHECK(fast.GetState().qidian == debug.GetState().qidian);
        value_t reference = 0;
        for (const auto &hit : fast.Log().Intents())
            reference += ReferenceDamage(c, hit.effect, hit.snapshot, hit.outcome == RollResult::DOUBLE);
        CHECK(fast.TotalDamage() == reference);
    }
    count_allocations = true;
    for (unsigned i = 0; i < 100; ++i) { fast.Start(4800, i); fast.Run(); (void)fast.TotalDamage(); }
    count_allocations = false;
    CHECK(allocations == 0);
    BatchOptions options; options.iterations = 1000;
    auto a = RunBatch(macro.program, options, tx::Rules(data));
    options.workers = 4;
    auto b = RunBatch(macro.program, options, tx::Rules(data));
    CHECK(a.checksum == b.checksum && a.min_damage == b.min_damage && a.max_damage == b.max_damage);
}

void YouRenLifetime() {
    tx::Config c;
    c.team_effects = tx::YouRen;
    auto r = Run<AlwaysProc>("/scast wuwu", c, 321);
    CHECK(r.state.self_buff_stacks[tx::YouRenBuff] == 0);
    CHECK(r.state.self_buff_expires_at[tx::YouRenBuff] == 0);

    r = Run<FirstCritThenThreshold>("/scast wuwu\n/scast sanhuan\n/scast wuwu", c, 49);
    unsigned wuwu_hits = 0;
    for (const auto &hit : r.hits) if (hit.skill == tx::WuWo) {
        ++wuwu_hits;
        if (wuwu_hits == 1 || wuwu_hits == 2) CHECK(hit.outcome == RollResult::DOUBLE);
    }
    CHECK(wuwu_hits == 2);
    CHECK(r.state.self_buff_stacks[tx::YouRenBuff] == 5);
    CHECK(r.state.self_buff_expires_at[tx::YouRenBuff] == 368);

    c.team_effects = 0;
    r = Run<FirstCritThenThreshold>("/scast wuwu\n/scast sanhuan\n/scast wuwu", c, 49);
    wuwu_hits = 0;
    for (const auto &hit : r.hits) if (hit.skill == tx::WuWo) {
        ++wuwu_hits;
        if (wuwu_hits == 2) CHECK(hit.outcome == RollResult::HIT);
    }
    CHECK(wuwu_hits == 2);
}

void ActionDiagnostics() {
    auto observe = [](std::string_view text, tx::Config config, auto setup) {
        auto macro = Compile(text);
        Simulation<tx::Rules, true> sim(macro.program, tx::Rules(tx::Prepare(config)));
        MacroObservation observed;
        sim.SetDebugObserver([](void *context, const MacroObservation &step) {
            if (step.phase == DebugPhase::Action) *static_cast<MacroObservation *>(context) = step;
        }, &observed);
        sim.Start(100, 1);
        setup(sim.MutableState());
        sim.StepOver();
        CHECK(observed.phase == DebugPhase::Action);
        return observed;
    };
    const auto unchanged = [](State &) {};
    auto config = tx::Config{};
    config.initial_qidian = 0;
    auto step = observe("/cast wuwu", config, [](State &s) { s.qidian = 0; });
    CHECK(!step.result && step.action.reason == ActionBlockReason::Qidian && step.action.actual == 0 && step.action.required == 1);
    config = {}; config.target.distance = 5;
    step = observe("/cast wuwu", config, unchanged);
    CHECK(step.action.reason == ActionBlockReason::Distance && step.action.actual == 5 && step.action.required == 4);
    step = observe("/cast wuwu", {}, [](State &s) { s.gcd_ready_at[0] = 16; });
    CHECK(step.action.reason == ActionBlockReason::GlobalCooldown && step.action.actual == 16);
    step = observe("/cast bahuang", {}, [](State &s) { s.skill_ready_at[tx::BaHuang] = 32; s.gcd_ready_at[0] = 16; });
    CHECK(step.action.reason == ActionBlockReason::SkillCooldown && step.action.actual == 32);
    step = observe("/cast bahuang", {}, [](State &s) {
        s.self_buff_stacks[tx::WeaponCwBuff] = 1; s.skill_ready_at[tx::BaHuang] = 32; s.gcd_ready_at[0] = 16;
    });
    CHECK(step.action.reason == ActionBlockReason::GlobalCooldown && step.action.actual == 16);
    step = observe("/cast ziqi", {}, [](State &s) { s.skill_energy[tx::ZiQi] = 0; });
    CHECK(step.action.reason == ActionBlockReason::Charges);
    step = observe("/fcast wuwu", {}, [](State &s) { s.preparing_skill = tx::SuiXing; s.skill_prepare_at[tx::SuiXing] = 16; });
    CHECK(step.action.reason == ActionBlockReason::Preparing && step.action.subject == tx::SuiXing && step.action.actual == 16);
    step = observe("/cast wuwu", {}, [](State &s) { s.casting_skill = tx::WanJian; s.skill_casting_at[tx::WanJian] = 24; });
    CHECK(step.action.reason == ActionBlockReason::Channeling && step.action.subject == tx::WanJian);
    step = observe("/cast jinghua", {}, unchanged);
    CHECK(step.action.reason == ActionBlockReason::MissingTalent);
    config = {}; config.talents = tx::JingHuaTalent;
    step = observe("/cast jinghua", config, unchanged);
    CHECK(step.action.reason == ActionBlockReason::MissingBuff);

    // This migrated skill subset has no channel starter; seed an active owner
    // to exercise the real force-cast rejection path and its state mutations.
    const auto channel = [](State &s) {
        s.casting_skill = tx::WanJian;
        s.skill_casting_at[tx::WanJian] = 24;
        s.gcd_ready_at[0] = 16;
    };
    auto macro = Compile("/fcast wuwu\n/cast sanhuan");
    Simulation<tx::Rules, true> debug(macro.program, tx::Rules{});
    std::vector<MacroObservation> observations;
    debug.SetDebugObserver([](void *context, const MacroObservation &event) {
        static_cast<std::vector<MacroObservation> *>(context)->push_back(event);
    }, &observations);
    debug.Start(100, 7); channel(debug.MutableState()); debug.Continue();
    const auto interruption = std::find_if(observations.begin(), observations.end(), [](const auto &event) {
        return event.action.interrupted == tx::WanJian;
    });
    CHECK(interruption != observations.end() && !interruption->result);
    CHECK(interruption->action.reason == ActionBlockReason::GlobalCooldown);
    CHECK(std::count_if(observations.begin(), observations.end(), [](const auto &event) {
        return event.action.interrupted != INVALID_SLOT;
    }) == 1); // The interruption must not leak into later observations.
    Simulation<tx::Rules> fast(macro.program, tx::Rules{});
    fast.Start(100, 7); channel(fast.MutableState()); fast.Run();
    CHECK(fast.Log().Mutations() == debug.Log().Mutations());
    CHECK(fast.Log().Intents() == debug.Log().Intents());
    CHECK(fast.TotalDamage() == debug.TotalDamage());
}

void Validation() {
    auto c = tx::Config{};
    c.talents = 1 << 19; Rejects([&] { tx::Prepare(c); });
    c = {}; c.recipes[tx::BaHuang] = tx::Crit2; Rejects([&] { tx::Prepare(c); });
    c = {}; c.shengtaiji_recipes = tx::Damage3; Rejects([&] { tx::Prepare(c); });
    c = {}; c.renjian_recipes = tx::Prepare1; Rejects([&] { tx::Prepare(c); });
    c = {}; c.renjian_recipes = tx::Range1|tx::Range2|tx::Range3|tx::Dot|tx::Cooldown; Rejects([&] { tx::Prepare(c); });
    c = {}; c.talents = tx::XuanMen; Rejects([&] { tx::Prepare(c); });
    c = {}; c.team_effects = 2; Rejects([&] { tx::Prepare(c); });
    c = {}; c.attributes.overcome_percent = 102; Rejects([&] { tx::Prepare(c); });
    c = {}; c.target.life = 0; Rejects([&] { tx::Prepare(c); });
    c = {}; c.target.distance = std::numeric_limits<double>::quiet_NaN(); Rejects([&] { tx::Prepare(c); });
    CHECK(!MacroCompiler::Compile("/cast unmigrated", tx::MacroOptions()).second.empty());
    CHECK(!MacroCompiler::Compile("/cast pozhao", tx::MacroOptions()).second.empty());
    auto chinese = MacroCompiler::Compile(std::string("/cast ")+std::string(tx::Skills.entries[tx::WuWo].name), tx::MacroOptions());
    CHECK(chinese.second.empty() && chinese.first.program.lines[0].action_slot == tx::WuWo);
    c = {};
    c.attributes.attack_base = c.attributes.attack_by_class = c.attributes.weapon = 1000000;
    c.attributes.crit_power = c.attributes.overcome = c.attributes.strain = c.attributes.surplus = 1000000;
    c.attributes.attack_percent = c.attributes.crit_power_percent = c.attributes.damage_percent = 10000;
    c.attributes.strain_percent = c.attributes.pve_percent = c.target.vulnerable = 10000;
    c.target.level = 110; c.target.shield = 0; c.talents = tx::SupportedTalents;
    c.attributes.overcome_base = c.attributes.overcome_by_class = 1000000;
    c.attributes.overcome_percent = 10000;
    auto extremes = tx::Prepare(c);
    for (unsigned effect = 0; effect < tx::EffectCount; ++effect)
        CHECK(extremes->damage[1][effect][1] == ReferenceDamage(c, effect, true, true));
    auto macro = Compile("/cast wuwu");
    Rejects([&] { Simulation<tx::Rules> too_large(macro.program, tx::Rules(extremes), 100000, 1); });
    State initial; initial.Resize(tx::SkillCount, tx::BuffCount);
    initial.skill_gcd_group[tx::WuWo] = 0;
    auto state = initial, scratch = initial;
    DamageIntentLog log;
    SetGcd(state, log, 0, 24); SetSkillEnergy(state, log, tx::ZiQi, 1);
    SetPrepare(state, log, tx::SuiXing, 16, true);
    log.Record(0, tx::WuWo, 0, 0, 0, 0, 0);
    (void)log.Reduce(initial, scratch, [](const auto &, const State &s) -> value_t {
        CHECK(s.SkillReadyAt(tx::WuWo) == 24 && s.skill_energy[tx::ZiQi] == 1);
        CHECK(s.preparing_skill == tx::SuiXing && s.skill_prepare_at[tx::SuiXing] == 16);
        return 0;
    });
}

int main() {
    ArithmeticOracle(); TimingsAndResources(); NaturalQidianRegeneration(); ChargesAndSnapshots(); TalentOrder(); ModeParity(); Validation(); ActionDiagnostics();
    PreparedFields(); FieldOwnership(); FieldDamageInputs(); FieldModeParity();
    FieldExplosions(); SplitSnapshots(); LieYunLifetime(); JianRuLifetime(); YunZhongLifetime(); JingHuaLifetime(); WeaponCwLifetime(); SetAttributeLifetime(); YouRenLifetime(); ExplosionModeParity();
    std::cout << "Tai Xu migration: " << 120*tx::ProfileCount*tx::EffectCount*2+tx::EffectCount
              << " legacy damage comparisons, field/explosion/JianRu/YunZhong/JingHua/YouRen/live-snapshot checks, "
              << "300 mode comparisons, zero hot allocations, batch parity passed\n";
}
