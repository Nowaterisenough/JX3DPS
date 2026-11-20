#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <random>
#include <stdexcept>

#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"

#include "check.hpp"

// Compile the unchanged old arithmetic in a separate namespace.
#define JX3DPS LegacyReference
#include "legacy_reference/Damage.hpp"
#undef JX3DPS
#include "legacy_reference/skill_parameters.inc"

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace tx = JX3DPS::runtime::tai_xu;

template <typename F>
void Rejects(F f)
{
    try {
        f();
    } catch (const std::invalid_argument &) {
        return;
    }
    CHECK(false);
}

static bool        count_allocations = false;
static std::size_t allocations       = 0;

void *operator new(std::size_t size)
{
    if (count_allocations) {
        ++allocations;
    }
    if (auto *p = std::malloc((size != 0u) ? size : 1)) {
        return p;
    }
    throw std::bad_alloc();
}

void *operator new[](std::size_t n)
{
    return ::operator new(n);
}

void operator delete(void *p) noexcept
{
    std::free(p);
}

void operator delete[](void *p) noexcept
{
    std::free(p);
}

void operator delete(void *p, std::size_t) noexcept
{
    std::free(p);
}

void operator delete[](void *p, std::size_t) noexcept
{
    std::free(p);
}

CompiledMacro Compile(std::string_view text)
{
    auto [macro, errors] = MacroCompiler::Compile(text, tx::MacroOptions());
    for (const auto &error : errors) {
        std::cerr << error.message << '\n';
    }
    CHECK(errors.empty());
    return macro;
}

struct HitsOnly
{
    void Reset(std::uint64_t) { }

    std::uint64_t Next() { return ~std::uint64_t{ 0 }; }
};

struct MixedRolls
{
    unsigned index = 0;

    void Reset(std::uint64_t) { index = 0; }

    std::uint64_t Next() { return ((index++ % 2) != 0u) ? ~std::uint64_t{ 0 } : 0; }
};

struct ThresholdRolls
{
    void Reset(std::uint64_t) { }

    std::uint64_t Next() { return static_cast<std::uint64_t>(0.275 * 0x1.0p53) << 11; }
};

struct FirstCritThenThreshold
{
    unsigned index = 0;

    void Reset(std::uint64_t) { index = 0; }

    std::uint64_t Next()
    {
        if (index++ == 0) {
            return 0;
        }
        return static_cast<std::uint64_t>(0.26 * 0x1.0p53) << 11;
    }
};

struct AlwaysProc
{
    void Reset(std::uint64_t) { }

    std::uint64_t Next() { return 0; }
};

struct FixedQidianPhase
{
    tick_t frame = 15;

    tick_t operator()(std::uint64_t) const { return frame; }
};

struct Result
{
    State                            state;
    std::vector<DamageIntent>        hits;
    std::vector<StateMutation>       mutations;
    value_t                          damage;
    std::array<tx::FieldInstance, 3> fields;
};

template <typename Rng = HitsOnly>
Result Run(std::string_view text, tx::Config config = {}, tick_t duration = 480, std::uint64_t seed = 0)
{
    const auto MACRO = Compile(text);
    using Rules      = tx::BasicRules<Rng, FixedQidianPhase>;
    Simulation<Rules> sim(MACRO.program, Rules(tx::Prepare(config)));
    sim.Start(duration, seed);
    sim.Run();
    return { sim.GetState(), sim.Log().Intents(), sim.Log().Mutations(), sim.TotalDamage(), sim.GetRules().Fields() };
}

// Inputs and modifiers are assembled independently of Prepared::Input/Chance.
value_t ReferenceDamage(const tx::Config &c, unsigned effect, unsigned profile, bool crit)
{
    using namespace LegacyReference;
    if (effect >= tx::ZeroEffect && effect < tx::ChiYingEffect) {
        return 0;
    }
    LegacyParam param(0, 0, 0);
    unsigned    recipe       = 0;
    int         skill_damage = 0;
    int         skill_power  = 0;
    int         count        = 1;
    int         dot_count    = 8;
    bool        dot          = false;
    bool        surplus      = false;
    if (effect < 20) {
        const unsigned Q = effect % 10;
        param            = legacy_wuwu[Q];
        recipe           = c.recipes[0];
        if (effect >= 10) {
            skill_damage += 307;
        }
        if ((c.talents & tx::TONG_GEN) != 0u) {
            skill_damage += 102;
        }
        if (Q >= 5 && ((c.talents & tx::WU_YI) != 0u)) {
            skill_power += 307;
        }
    } else if (effect == 20) {
        param  = legacy_bahuang[0];
        recipe = c.recipes[1];
    } else if (effect < 31) {
        param  = legacy_bahuang_extra[effect - 21];
        recipe = c.recipes[1];
    } else if (effect == 31) {
        param  = legacy_sanhuan[0];
        recipe = c.recipes[2];
        if ((c.talents & tx::XIN_GU) != 0u) {
            skill_power += 102;
        }
    } else if (effect == 32) {
        param         = legacy_sanchai[0];
        skill_damage += 205;
    } else if (effect < 36) {
        param   = legacy_pozhao[effect - 33];
        surplus = true;
    } else if (effect < tx::ZeroEffect) {
        param = legacy_dieren[(effect - 36) / 9];
        count = (effect - 36) % 9;
        dot   = true;
        if ((c.talents & tx::XU_JI) != 0u) {
            param.channel = static_cast<int>(param.channel * 1.2);
        }
    } else if (effect == tx::ChiYingEffect) {
        param = legacy_chiying[0];
    } else if (effect == tx::WanJianEffect) {
        param = legacy_wanjian[0];
        if ((c.talents & tx::TONG_GEN) != 0u) {
            skill_damage += 102;
        }
        if ((c.talents & tx::XU_JI) != 0u) {
            skill_power += 1000 + 102;
        }
    } else if (effect == tx::RenJianDotEffect) {
        param     = legacy_renjian_dot[0];
        dot       = true;
        dot_count = 4;
    } else if (effect == tx::JianRuEffect) {
        param = legacy_jianru[0];
    } else if (effect == tx::JianRuActiveEffect) {
        param = legacy_jianru_active[0];
    } else if (effect >= tx::YunZhongSuiXingEffect && effect <= tx::YunZhongTunRiEffect) {
        param = effect == tx::YunZhongSuiXingEffect    ? legacy_yunzhong_suixing[0]
                : effect == tx::YunZhongShengTaiEffect ? legacy_yunzhong_shengtaiji[0]
                                                       : legacy_yunzhong_tunriyue[0];
    } else if (effect == tx::JingHuaEffect) {
        param = legacy_jinghua[0];
    } else if (effect == tx::JingHuaSuiEffect) {
        param = legacy_jinghua_sui[0];
    } else if (effect == tx::BaHuangCwEffect) {
        param  = legacy_bahuang_extra[9];
        recipe = c.recipes[1];
    } else if (effect == tx::WeaponCwExtraEffect) {
        param  = LegacyParam((20 + 20 + 2) / 2, 0, 65);
        recipe = c.recipes[1];
    } else if (effect >= tx::WanXiangEffect && effect < tx::WanXiangEffect + 3) {
        const auto STACK = static_cast<int>(effect - tx::WanXiangEffect) + 1;
        param            = LegacyParam(10, 0, static_cast<int>(205 * STACK * 1.8));
        dot              = true;
        dot_count        = 10;
    } else {
        param = effect == tx::RenJianEffect ? legacy_renjian[0] : legacy_renjian_suixing[0];
        if ((c.renjian_recipes & tx::DAMAGE40) != 0u) {
            skill_damage += 409;
        }
        if ((c.renjian_recipes & tx::DAMAGE60) != 0u) {
            skill_damage += 614;
        }
    }
    skill_damage += (((recipe & tx::DAMAGE3) != 0u) ? 31 : 0) + (((recipe & tx::DAMAGE4) != 0u) ? 41 : 0) +
                    (((recipe & tx::DAMAGE5) != 0u) ? 51 : 0);
    if (((c.equipment & tx::WEAPON_CW) != 0u) && (effect < 31 || effect == tx::BaHuangCwEffect || effect == tx::WeaponCwExtraEffect))
    {
        skill_damage += 51;
    }
    const auto &a             = c.attributes;
    const bool  PURPLE        = profile & 1;
    const bool  SUIXING       = profile & 2;
    const bool  QISHENG       = profile & 4;
    const bool  GUCHANG       = profile & 8;
    const bool  SET_ATTRIBUTE = (profile & tx::SET_ATTRIBUTE_PROFILE) != 0u;
    const auto ATTACK = (a.attack_base * (1024 + a.attack_percent + (PURPLE ? 256 : 0) + (QISHENG ? 102 : 0)) / 1024) + a.attack_by_class;
    const int  SUIXING_POWER = SUIXING ? (((c.talents & tx::FU_YIN) != 0u) ? 205 : 102) : 0;
    const int  LIEYUN_POWER  = ((profile & 16) != 0u) ? 154 : 0;
    const auto OVERCOME =
        a.overcome_base < 0
            ? a.overcome
            : (a.overcome_base *
               (1024 + a.overcome_percent + (204 * ((profile & tx::XUAN_MEN_PROFILE_MASK) / tx::XUAN_MEN_PROFILE_UNIT))) / 1024) +
                  a.overcome_by_class;
    const auto HASTE = 1024.0 / (static_cast<int>(a.haste / (11.695 * 8250) * 1024) + a.haste_percent + 1024);
    const auto WANXIANG_INTERVAL = static_cast<int>(48 * HASTE);
    value_t    raw               = 0;
    if (surplus) {
        raw = EffectDamage(SurplusDamage(a.surplus, param.channel, 120), a.damage_percent);
    } else {
        raw = count *
              EffectDamageAll(
                  ATTACK,
                  dot ? PhysicsDotDamageCoefficient(param.channel, dot_count, effect >= tx::WanXiangEffect && effect < tx::WanXiangEffect + 3 ? WANXIANG_INTERVAL : 48)
                      : PhysicsDamageCoefficient(param.channel, 0),
                  a.weapon,
                  param.weapon,
                  param.fixed,
                  a.damage_percent + skill_damage);
    }
    return FinalPhysicsDamage(
        120,
        c.target.level,
        raw,
        c.target.shield,
        0,
        a.ignore_shield + (GUCHANG ? 614 : 0),
        0,
        OVERCOME,
        crit ? 1 : 0,
        a.crit_power,
        a.crit_power_percent + skill_power + (PURPLE ? 256 : 0) + SUIXING_POWER + LIEYUN_POWER + (SET_ATTRIBUTE ? 41 : 0),
        a.strain,
        a.strain_percent,
        a.pve_percent,
        c.target.vulnerable);
}

void ArithmeticOracle()
{
    std::mt19937 rng(49271);
    for (unsigned i = 0; i < 120; ++i) {
        tx::Config c;
        c.talents = tx::SupportedTalents & (((i % 2) != 0u) ? ~tx::FU_YIN : ~0u);
        if ((i % 3) != 0u) {
            c.talents &= ~tx::XU_JI;
        }
        if ((i % 2) != 0u) {
            c.equipment = tx::WEAPON_CW;
        }
        c.renjian_recipes    = tx::DAMAGE40 | tx::DAMAGE60 | tx::DOT | tx::COOLDOWN;
        c.recipes            = { tx::DAMAGE3 | tx::DAMAGE4 | tx::DAMAGE5 | tx::QIDIAN,
                                 tx::DAMAGE3 | tx::DAMAGE5 | tx::COOLDOWN,
                                 tx::DAMAGE4 | tx::DAMAGE5 | tx::CRIT4 };
        auto &a              = c.attributes;
        a.attack_base        = rng() % 60000;
        a.attack_by_class    = rng() % 15000;
        a.attack_percent     = rng() % 2048;
        a.weapon             = rng() % 10000;
        a.crit_power         = rng() % 50000;
        a.overcome           = rng() % 100000;
        a.overcome_base      = rng() % 100000;
        a.overcome_by_class  = rng() % 20000;
        a.overcome_percent   = rng() % 2048;
        a.strain             = rng() % 60000;
        a.surplus            = rng() % 12000;
        a.damage_percent     = rng() % 2048;
        a.crit_power_percent = rng() % 1024;
        a.strain_percent     = rng() % 1024;
        a.pve_percent        = rng() % 1024;
        a.ignore_shield      = rng() % 1025;
        c.target.shield      = rng() % 200000;
        c.target.vulnerable  = rng() % 1024;
        c.target.level       = 110 + (rng() % 21);
        const auto P         = tx::Prepare(c);
        for (unsigned snapshot = 0; snapshot < tx::ProfileCount; ++snapshot) {
            for (unsigned effect = 0; effect < tx::EffectCount; ++effect) {
                for (unsigned critical = 0; critical < 2; ++critical) {
                    CHECK(P->Damage(snapshot, effect, critical) == ReferenceDamage(c, effect, snapshot, critical));
                }
            }
        }
    }
    tx::Config c;
    c.talents    = tx::WU_YI;
    const auto P = tx::Prepare(c);
    CHECK(P->Chance(tx::WuWoEffect + 5, 0) > P->Chance(tx::PoZhaoEffect, 0) + .09);
    CHECK(P->Chance(tx::DieRenEffect + 1, 0) == P->Chance(tx::PoZhaoEffect, 0));
    CHECK(std::abs(P->Chance(tx::PoZhaoEffect, tx::SUI_XING_PROFILE) - P->Chance(tx::PoZhaoEffect, 0) - .05) < 1e-12);
}

void TimingsAndResources()
{
    auto r = Run("/scast wuwu\n/scast sanchai", {}, 1);
    CHECK(r.hits.size() == 3 && r.hits[0].level == 9 && r.hits[1].skill == tx::PO_ZHAO);
    CHECK(r.state.last_skill == tx::WU_WO && r.state.qidian == 0);
    CHECK(r.state.SkillReadyAt(tx::BA_HUANG) == 24 && r.state.SkillReadyAt(tx::SAN_CHAI) == 22);
    r = Run("/cast sanchai", {}, 67);
    CHECK(r.hits.size() == 4 && r.hits[3].frame == 66 && r.state.last_skill == INVALID_SLOT);
    r = Run("/cast sanhuan", {}, 65);
    CHECK(r.hits.size() == 9 && r.hits[3].frame == 32 && r.hits[6].frame == 64);
    r = Run<MixedRolls>("/scast sanhuan", {}, 1);
    CHECK(r.hits.size() == 3 && r.hits[0].outcome == RollResult::DOUBLE && r.hits[1].outcome == RollResult::HIT &&
          r.hits[2].outcome == RollResult::DOUBLE);
    tx::Config c;
    c.recipes[tx::WU_WO] = tx::QIDIAN;
    CHECK(Run("/scast wuwu", c, 1).state.qidian == 1);
    c.recipes[tx::WU_WO] = tx::DAMAGE3;
    CHECK(Run("/scast wuwu", c, 1).state.qidian == 0);
    c                = {};
    c.initial_qidian = 0;
    c.talents        = tx::SHEN_MAI;
    r                = Run("/scast sanchai\n/scast sanhuan", c, 1);
    CHECK(r.state.qidian == 7); // Initial ClassFeature point plus unconditional ShenMai procs.
    c                       = {};
    c.attributes.haste      = 10000;
    c.recipes[tx::SAN_HUAN] = tx::COOLDOWN;
    const auto P            = tx::Prepare(c);
    r                       = Run("/cast sanhuan", c, 100);
    CHECK(r.hits[3].frame == P->gcd && P->sanhuan_cd == P->gcd);
    r = Run("/scast bahuang\n/scast [skill_cd:wuwu=0] wuwu", {}, 25);
    CHECK(r.hits[2].frame == 24);
    r = Run("/scast sanhuan\n/scast [skill_cd:wuwu<1] ziqi", {}, 10);
    CHECK(r.hits.back().skill == tx::ZI_QI && r.hits.back().frame == 9);
    c                = {};
    c.initial_qidian = 0;
    r                = Run("/cast wuwu", c, 100);
    CHECK(r.hits.size() == 5 && r.hits.front().frame == 0 && r.hits.front().level == 0);
    c.target.distance = 5;
    r                 = Run("/cast sanchai", c, 100);
    CHECK(r.hits.empty());
}

void NaturalQidianRegeneration()
{
    using Rules          = tx::BasicRules<tx::SplitMixRolls, FixedQidianPhase>;
    const auto IDLE      = Compile("/cast [qidian>10] sanchai");
    const auto THRESHOLD = Compile("/scast [qidian>=3] wuwu");
    tx::Config c;
    c.initial_qidian              = 0;
    const auto           PREPARED = tx::Prepare(c);
    std::array<bool, 16> phases_seen{};
    for (unsigned seed = 0; seed < 1024; ++seed) {
        const auto PHASE = tx::NaturalQidianPhase{}(seed);
        CHECK(PHASE >= 0 && PHASE < 16 && PHASE == tx::NaturalQidianPhase{}(seed));
        phases_seen[PHASE] = true;
    }
    CHECK(std::all_of(phases_seen.begin(), phases_seen.end(), [](bool seen) { return seen; }));
    for (tick_t phase = 0; phase < 16; ++phase) {
        Simulation<Rules> fast(IDLE.program, Rules(PREPARED, {}, { phase }));
        for (const tick_t END : { 0, 1, 15, 16, 17, 31, 32, 160, 4800 }) {
            fast.Start(END, 7);
            CHECK(fast.GetState().qidian == 1 && fast.Log().Mutations().empty());
            fast.Run();
            const auto TICKS = END > phase ? 1 + ((END - 1 - phase) / 16) : 0;
            CHECK(fast.GetState().qidian == std::min(10, 1 + TICKS));
            CHECK(fast.Timers().Pending() == 1);
            CHECK(fast.Log().Mutations().size() == static_cast<std::size_t>(std::min(9, TICKS)));
            for (unsigned i = 0; i < fast.Log().Mutations().size(); ++i) {
                const auto &event = fast.Log().Mutations()[i];
                CHECK(event.kind == MutationKind::RESOURCES);
                CHECK(event.frame == phase + static_cast<tick_t>(i) * 16 && event.a == 2 + static_cast<int>(i));
            }
        }
        Simulation<Rules>       wake(THRESHOLD.program, Rules(PREPARED, {}, { phase }));
        Simulation<Rules, true> debug(THRESHOLD.program, Rules(PREPARED, {}, { phase }));
        wake.Start(80, 7);
        wake.Run();
        debug.Start(80, 7);
        while (debug.Continue()) {
        }
        CHECK(wake.Log().Size() == 1 && wake.Log().Intents()[0].frame == phase + 16);
        CHECK(wake.Log().Intents()[0].level == 2);
        CHECK(wake.Log().Mutations() == debug.Log().Mutations());
        CHECK(wake.Log().Intents() == debug.Log().Intents());
        CHECK(wake.TotalDamage() == debug.TotalDamage());
        // The restoring resource mutation precedes the macro's same-frame hit.
        const auto &hit = wake.Log().Intents()[0];
        CHECK(std::any_of(wake.Log().Mutations().begin(), wake.Log().Mutations().end(), [&](const auto &m) {
            return m.frame == hit.frame && m.kind == MutationKind::RESOURCES && m.a == 3 && m.sequence < hit.sequence;
        }));
    }
    const auto        ATTACKS = Compile("/cast sanchai");
    Simulation<Rules> zero(ATTACKS.program, Rules(tx::Prepare(), {}, { 0 }));
    Simulation<Rules> last(ATTACKS.program, Rules(tx::Prepare(), {}, { 15 }));
    zero.Start(400, 8);
    zero.Run();
    last.Start(400, 8);
    last.Run();
    CHECK(zero.Log().Intents() == last.Log().Intents()); // Phase sampling does not consume critical rolls.
    Simulation<Rules> invalid(IDLE.program, Rules(PREPARED, {}, { 16 }));
    Rejects([&] { invalid.Start(10, 0); });
}

void ChargesAndSnapshots()
{
    auto r = Run("/cast ziqi", {}, 1201);
    CHECK(r.hits.size() == 3 && r.hits[0].frame == 0 && r.hits[1].frame == 0 && r.hits[2].frame == 1200);
    CHECK(r.state.skill_energy[tx::ZI_QI] == 0 && r.state.skill_ready_at[tx::ZI_QI] == 2400);
    tx::Config c;
    c.talents = tx::RUO_SHUI;
    r         = Run("/scast ziqi\n/scast ziqi\n/scast ziqi", c, 480);
    CHECK(r.hits.size() == 3 && r.state.self_buff_expires_at[tx::PURPLE] == 480);
    r = Run("/scast ziqi\n/scast [bufftime:ziqi<1] wuwu", {}, 161);
    CHECK(r.hits[1].frame == 145 && r.state.qidian == 3);
    CHECK(r.state.self_buff_stacks[tx::PURPLE] == 0); // Last resource tick precedes clear.
    c         = {};
    c.talents = tx::DIE_REN_TALENT;
    r         = Run("/scast ziqi\n/scast wuwu", c, 385);
    CHECK(r.hits.size() == 11 && r.state.target_buff_stacks[tx::DIE_REN] == 0);
    for (std::size_t i = 3; i < r.hits.size(); ++i) {
        CHECK(r.hits[i].frame == static_cast<int>(i - 2) * 48);
        CHECK(r.hits[i].snapshot == 1); // Purple expired at 160; DOT remains snapshotted.
    }
    r = Run("/scast wuwu\n/scast ziqi\n/scast wuwu", c, 410);
    CHECK(r.hits[5].frame == 48 && r.hits[5].level == 1 && r.hits[5].snapshot == 1);
    CHECK(r.hits.back().frame == 384 && r.hits.size() == 13);
    c.attributes.haste = 10000;
    r                  = Run("/scast wuwu", c, 400);
    CHECK(r.hits.size() == 10); // Eight ticks with legacy fractional-duration rounding.
}

void TalentOrder()
{
    tx::Config c;
    c.talents = tx::FENG_SHI_TALENT;
    auto r    = Run("/scast wuwu", c, 1);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] == 1);
    r = Run("/scast bahuang\n/scast wuwu", c, 25);
    CHECK(r.hits[2].effect == tx::FengWuWoEffect + 9 && r.state.self_buff_stacks[tx::FENG_SHI] == 0);
    c.talents = tx::DIE_REN_TALENT | tx::WU_YU;
    r         = Run("/scast wuwu\n/scast bahuang\n/scast ziqi\n/scast wuwu", c, 49);
    CHECK(r.state.skill_ready_at[tx::BA_HUANG] == 216 - 88);
    c.talents     = tx::DIE_REN_TALENT | tx::QIE_YU;
    c.target.life = .4;
    r             = Run("/scast wuwu\n/scast bahuang", c, 25);
    CHECK(r.hits.size() == 5 && r.hits.back().sub == 1);
    CHECK(tx::Formulas[r.hits.back().effect].count == 7 && r.state.target_buff_stacks[tx::DIE_REN] == 0);
    c         = {};
    c.talents = tx::DIE_REN_TALENT | tx::HUAN_YUE;
    r         = Run("/scast wuwu\n/scast sanhuan", c, 385);
    CHECK(r.hits.size() == 16 && r.hits[8].skill == tx::DIE_REN_DAMAGE);
    CHECK(r.hits[5].skill == tx::PO_ZHAO && r.hits[5].sub == 1);
    CHECK(r.hits[8].frame == 24 && r.hits.back().frame == 240);
    CHECK(r.state.qidian == 10);
}

void OffGcdDuringCooldown()
{
    const auto result = Run("/cast bahuang\n/cast [nobuff:ziqi] ziqi\n/cast sanchai", {}, 24);
    CHECK(result.state.gcd_ready_at[0] == 24);
    std::vector<std::pair<Slot, tick_t>> events;
    for (const auto &hit : result.hits) {
        if (hit.effect == tx::BaHuangEffect || hit.effect == tx::ZeroEffect || hit.effect == tx::SanChaiEffect) {
            events.emplace_back(hit.skill, hit.frame);
        }
    }
    CHECK((events == std::vector<std::pair<Slot, tick_t>>{
                         { tx::BA_HUANG, 0  },
                         { tx::ZI_QI,    0  },
                         { tx::SAN_CHAI, 0  },
                         { tx::SAN_CHAI, 22 }
    }));

    // A changed group must take effect even if the original macro has only
    // primary-GCD actions and group zero is still locked.
    const auto            MACRO = Compile("/cast bahuang");
    Simulation<tx::Rules> sim(MACRO.program);
    sim.Start(1, 42);
    sim.MutableState().gcd_ready_at[0]               = 24;
    sim.MutableState().skill_gcd_group[tx::BA_HUANG] = 1;
    sim.Run();
    CHECK(!sim.Log().Intents().empty());
    CHECK(sim.Log().Intents().front().skill == tx::BA_HUANG && sim.Log().Intents().front().frame == 0);
}

void PreparedFields()
{
    tx::Config c;
    c.initial_qidian      = 0;
    constexpr auto SCRIPT = "/scast suixing\n/sfcast ziqi\n/scast sanchai";
    auto           r      = Run(SCRIPT, c, 16);
    CHECK(r.hits.empty() && r.state.preparing_skill == tx::SUI_XING);
    CHECK(r.state.skill_prepare_at[tx::SUI_XING] == 16 && r.state.last_skill == INVALID_SLOT);
    CHECK(r.state.qidian == 2 && r.state.self_buff_stacks[tx::SUI_XING_BUFF] == 0);
    r = Run(SCRIPT, c, 17);
    CHECK(r.hits.size() == 3 && r.hits[0].skill == tx::SUI_XING && r.hits[0].frame == 16);
    CHECK(r.hits[1].skill == tx::ZI_QI && r.hits[2].skill == tx::SAN_CHAI);
    CHECK(r.state.preparing_skill == INVALID_SLOT && r.state.last_skill == tx::ZI_QI);
    CHECK(r.state.skill_ready_at[tx::SUI_XING] == 176 && r.state.gcd_ready_at[0] == 24);
    CHECK(r.state.self_buff_expires_at[tx::SUI_XING_BUFF] == 80);
    r = Run("/scast suixing\n/scast [last_skill:suixing] wuwu", c, 25);
    CHECK(r.hits.size() == 2 && r.hits[1].frame == 24 && r.hits[1].level == 3);
    CHECK(r.hits[1].snapshot == tx::SUI_XING_PROFILE);
    c.shengtaiji_recipes = tx::PREPARE1 | tx::PREPARE2 | tx::PREPARE3 | tx::COOLDOWN;
    r                    = Run("/cast shengtaiji", c, 149);
    CHECK(r.hits.size() == 2 && r.hits[0].frame == 10 && r.hits[1].frame == 148);
    CHECK(r.state.skill_ready_at[tx::SHENG_TAI] == 276);
    c.attributes.haste = 10000;
    const auto P       = tx::Prepare(c);
    r                  = Run("/scast shengtaiji", c, 100);
    CHECK(r.hits[0].frame == P->shengtaiji_prepare && r.state.skill_ready_at[tx::SHENG_TAI] == P->shengtaiji_prepare + 128);
    c                 = {};
    c.target.distance = 100;
    r                 = Run("/scast tunriyue", c, 17);
    CHECK(r.hits.size() == 1 && r.state.target_buff_stacks[tx::TUN_RI_BUFF] == 1);
    c                          = {};
    c.talents                  = tx::FU_YIN;
    c.attributes.haste_percent = 10000;
    r                          = Run("/scast suixing\n/scast shengtaiji\n/scast tunriyue", c, 6);
    CHECK(r.hits.size() == 3 && r.hits[0].frame == 1 && r.hits[1].frame == 3);
    c.shengtaiji_recipes = tx::PREPARE1 | tx::PREPARE2 | tx::PREPARE3;
    r                    = Run("/scast shengtaiji\n/scast sanchai", c, 1);
    CHECK(r.hits.size() == 2 && r.hits[0].frame == 0 && r.state.preparing_skill == INVALID_SLOT);
}

void FieldOwnership()
{
    auto r = Run("/scast suixing", {}, 401);
    CHECK(r.state.self_buff_stacks[tx::FIELD_SUI_XING] == 0);
    CHECK(r.state.self_buff_stacks[tx::SUI_XING_BUFF] == 1 && r.state.self_buff_expires_at[tx::SUI_XING_BUFF] == 464);
    r = Run("/scast suixing", {}, 465);
    CHECK(r.state.self_buff_stacks[tx::SUI_XING_BUFF] == 0);
    r = Run("/scast tunriyue", {}, 401);
    CHECK(r.state.self_buff_stacks[tx::FIELD_TUN_RI] == 0 && r.state.target_buff_expires_at[tx::TUN_RI_BUFF] == 416);
    r = Run("/scast tunriyue", {}, 417);
    CHECK(r.state.target_buff_stacks[tx::TUN_RI_BUFF] == 0);
    tx::Config c;
    c.talents = tx::FU_YIN;
    r         = Run("/scast suixing", c, 593);
    CHECK(r.state.self_buff_stacks[tx::FIELD_SUI_XING] == 0 && r.state.self_buff_expires_at[tx::SUI_XING_BUFF] == 656);
    r = Run("/scast suixing\n/scast suixing\n/scast shengtaiji\n/scast tunriyue", {}, 241);
    CHECK(r.hits.size() == 4 && r.hits[1].frame == 192 && r.hits[3].frame == 240);
    CHECK(r.state.self_buff_stacks[tx::FIELD_SUI_XING] == 1 && r.state.self_buff_stacks[tx::FIELD_SHENG_TAI] == 1 &&
          r.state.self_buff_stacks[tx::FIELD_TUN_RI] == 1);
    for (const auto &field : r.fields) {
        CHECK(field.order != 0 && field.skill != INVALID_SLOT);
    }
    CHECK(r.state.self_buff_expires_at[tx::FIELD_SUI_XING] == 576); // The oldest global instance was evicted.
    r = Run("/scast suixing\n/scast suixing\n/scast shengtaiji\n/scast tunriyue", {}, 401);
    CHECK(r.state.self_buff_stacks[tx::FIELD_TUN_RI] == 1 && r.state.self_buff_expires_at[tx::FIELD_TUN_RI] == 624);
    c         = {};
    c.talents = tx::QI_SHENG;
    r         = Run("/scast shengtaiji", c, 401);
    CHECK(r.state.self_buff_expires_at[tx::QI_SHENG_BUFF] == 448);
    r = Run("/scast shengtaiji", c, 449);
    CHECK(r.state.self_buff_stacks[tx::QI_SHENG_BUFF] == 0);
}

void FieldDamageInputs()
{
    tx::Config c;
    c.talents = tx::DIE_REN_TALENT | tx::GU_CHANG;
    auto r    = Run("/scast wuwu\n/scast suixing", c, 49);
    CHECK(r.hits.back().skill == tx::DIE_REN_DAMAGE && r.hits.back().frame == 48);
    CHECK(r.hits.back().snapshot == tx::LIVE_GU_CHANG_PROFILE); // Live ignore, old crit/AP snapshot.
    r = Run("/scast suixing\n/scast [bufftime:suixing<1] wuwu", c, 498);
    CHECK(r.hits[1].frame == 449 && r.hits[1].snapshot == (tx::SUI_XING_PROFILE | tx::LIVE_GU_CHANG_PROFILE));
    CHECK(r.hits.back().frame == 497 && r.hits.back().snapshot == tx::SUI_XING_PROFILE);
    c.talents     |= tx::QIE_YU;
    c.target.life  = .5;
    r              = Run("/scast suixing\n/scast wuwu\n/scast bahuang", c, 49);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] != 0); // SuiXing must not satisfy Purple's execute threshold.
    c         = {};
    c.talents = tx::CHANG_SHENG | tx::FU_YIN | tx::QI_SHENG | tx::GU_CHANG;
    r = Run("/scast suixing\n/scast shengtaiji\n/scast tunriyue\n/scast wuwu\n/scast bahuang\n/scast sanhuan", c, 121);
    CHECK(r.hits.size() == 13 && r.state.self_buff_stacks[tx::CHI_YING_BUFF] == 0);
    CHECK(r.hits[5].skill == tx::CHI_YING_DAMAGE && r.hits[8].skill == tx::CHI_YING_DAMAGE && r.hits[12].skill == tx::CHI_YING_DAMAGE);
    CHECK(r.hits[5].frame == 72 && r.hits[8].frame == 96 && r.hits[12].frame == 120);
    CHECK(r.hits[3].snapshot == (tx::SUI_XING_PROFILE | tx::QI_SHENG_PROFILE | tx::LIVE_GU_CHANG_PROFILE));
    r = Run("/scast suixing\n/scast sanchai", c, 17);
    CHECK(r.state.self_buff_stacks[tx::CHI_YING_BUFF] == 1); // Autoattack does not consume ChiYing.
    r = Run("/scast suixing", c, 257);
    CHECK(r.state.self_buff_stacks[tx::CHI_YING_BUFF] == 0);
}

constexpr std::string_view FieldRotation =
    "/cast [nobuff:suixing] suixing\n/cast [nobuff:qisheng] shengtaiji\n"
    "/cast [tnobuff:tunriyue] tunriyue\n/cast [nobuff:ziqi&qidian<6] ziqi\n"
    "/cast [qidian>=8] wuwu\n/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";

void FieldExplosions()
{
    tx::Config c;
    c.initial_qidian = 0;
    c.talents        = tx::SHEN_MAI | tx::XU_JI;
    auto r           = Run("/scast wanjian", c, 1);
    CHECK(r.hits.size() == 1 && r.hits[0].skill == tx::WAN_JIAN && r.state.qidian == 4);
    CHECK(r.state.skill_ready_at[tx::WAN_JIAN] == 160 && r.state.gcd_ready_at[0] == 24);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] == 1);
    auto p = tx::Prepare(c);
    CHECK(p->Chance(tx::WanJianEffect, 0) == p->Chance(tx::PoZhaoEffect, 0));
    CHECK(p->Input(tx::WanJianEffect, 0).crit_power_percent == 1102);
    c                 = {};
    c.target.distance = 8;
    r                 = Run("/scast wanjian", c, 1);
    CHECK(r.hits.size() == 1 && r.state.skill_ready_at[tx::WAN_JIAN] == 192);
    c.target.distance = 8.01;
    r                 = Run("/scast wanjian", c, 1);
    CHECK(r.hits.empty());

    c                              = {};
    c.talents                      = tx::XUAN_MEN | tx::CHANG_SHENG | tx::SHEN_MAI;
    c.attributes.overcome_base     = 20000;
    c.attributes.overcome_by_class = 5000;
    c.renjian_recipes              = tx::COOLDOWN | tx::DOT | tx::DAMAGE40 | tx::DAMAGE60;
    constexpr auto THREE           = "/scast suixing\n/scast shengtaiji\n/scast tunriyue\n/scast renjian";
    r                              = Run(THREE, c, 73);
    CHECK(r.hits.size() == 7 && r.hits[3].frame == 72 && r.hits[6].skill == tx::CHI_YING_DAMAGE);
    CHECK(r.state.skill_ready_at[tx::REN_JIAN] == 312 && r.state.skill_ready_at[tx::REN_JIAN_SUI_XING] == 312);
    CHECK(r.state.self_buff_stacks[tx::XUAN_MEN_BUFF] == 3 && r.state.self_buff_expires_at[tx::XUAN_MEN_BUFF] == 712);
    CHECK(r.state.self_buff_stacks[tx::CHI_YING_BUFF] == 2);
    for (unsigned i = 3; i < 7; ++i) {
        CHECK(!(r.hits[i].snapshot & tx::XUAN_MEN_PROFILE_MASK));
    }
    for (const auto &field : r.fields) {
        CHECK(field.skill == INVALID_SLOT);
    }
    CHECK(r.state.target_buff_tick_at[tx::REN_JIAN_DOT] == 120 && r.state.target_buff_expires_at[tx::REN_JIAN_DOT] == 264);
    r = Run(THREE, c, 265);
    CHECK(r.hits.size() == 11 && r.state.target_buff_stacks[tx::REN_JIAN_DOT] == 0);
    for (unsigned i = 7; i < 11; ++i) {
        CHECK(r.hits[i].frame == 120 + static_cast<int>(i - 7) * 48 && r.hits[i].skill == tx::REN_JIAN_DAMAGE);
        CHECK((r.hits[i].snapshot & tx::XUAN_MEN_PROFILE_MASK) == 3 * tx::XUAN_MEN_PROFILE_UNIT);
    }
    c.shengtaiji_recipes = tx::COOLDOWN;
    CHECK(tx::Prepare(c)->shengtaiji_cd == 368);
    c.shengtaiji_recipes = 0;
    CHECK(tx::Prepare(c)->shengtaiji_cd == 400);
    r = Run("/scast renjian\n/scast renjian_suixing", c, 241);
    CHECK(r.hits.empty() && r.state.last_skill == tx::REN_JIAN_SUI_XING);
    CHECK(r.state.skill_ready_at[tx::REN_JIAN] == 480 && r.state.self_buff_stacks[tx::XUAN_MEN_BUFF] == 0);
    CHECK(r.state.self_buff_expires_at[tx::XUAN_MEN_BUFF] == 880);
    r = Run("/scast renjian\n/scast [buff:xuanmen] sanchai\n/scast [buff:xuanmen>0] sanchai", c, 1);
    CHECK(r.hits.empty() && r.state.self_buff_expires_at[tx::XUAN_MEN_BUFF] == 640);
    const auto EXISTENCE =
        Compile("/cast [buff:xuanmen] sanchai\n/cast [buff:xuanmen>0] sanchai\n/cast [nobuff:xuanmen] sanchai");
    for (int stacks = 0; stacks <= 3; ++stacks) {
        r.state.self_buff_stacks[tx::XUAN_MEN_BUFF] = stacks;
        CHECK(EvaluateCondition(r.state, EXISTENCE.program.conditions[0]) == (stacks > 0));
        CHECK(EvaluateCondition(r.state, EXISTENCE.program.conditions[1]) == (stacks > 0));
        CHECK(EvaluateCondition(r.state, EXISTENCE.program.conditions[2]) == (stacks == 0));
    }
    r = Run("/scast suixing\n/scast renjian\n/scast renjian_suixing", c, 265);
    CHECK(r.state.self_buff_stacks[tx::XUAN_MEN_BUFF] == 1 && r.state.self_buff_expires_at[tx::XUAN_MEN_BUFF] == 904);

    c                     = {};
    constexpr auto PREFIX = "/scast suixing\n/scast suixing\n/scast shengtaiji\n/scast renjian_suixing";
    r                     = Run(PREFIX, c, 225);
    CHECK(r.hits.size() == 4 && r.hits.back().skill == tx::REN_JIAN_SUI_XING && r.hits.back().frame == 224);
    CHECK(r.state.self_buff_stacks[tx::FIELD_SUI_XING] == 1 && r.state.self_buff_stacks[tx::FIELD_SHENG_TAI] == 1);
    CHECK(r.state.self_buff_expires_at[tx::FIELD_SUI_XING] == 576);
    r = Run("/scast suixing\n/scast shengtaiji\n/scast suixing\n/scast renjian_suixing", c, 201);
    CHECK(r.hits.size() == 5 && r.state.self_buff_stacks[tx::FIELD_SHENG_TAI] == 0 &&
          r.state.self_buff_stacks[tx::FIELD_SUI_XING] == 1);
    c.target.distance = 8;
    c.renjian_recipes = tx::RANGE1 | tx::RANGE2;
    r                 = Run("/scast suixing\n/scast renjian", c, 25);
    CHECK(r.hits.size() == 2);
    c.renjian_recipes = tx::RANGE1;
    r                 = Run("/scast suixing\n/scast renjian", c, 25);
    CHECK(r.hits.size() == 1 && r.state.skill_ready_at[tx::REN_JIAN] == 0);
    c                  = {};
    c.renjian_recipes  = tx::DOT;
    c.attributes.haste = 10000;
    p                  = tx::Prepare(c);
    r                  = Run("/scast suixing\n/scast renjian", c, 250);
    CHECK(r.hits.size() == 6 && r.hits[2].frame == p->gcd + p->dot_interval);
    CHECK(r.hits.back().frame == p->gcd + 4 * p->dot_interval && r.state.target_buff_stacks[tx::REN_JIAN_DOT] == 0);
}

void SplitSnapshots()
{
    tx::Config c;
    c.talents                      = tx::XUAN_MEN | tx::DIE_REN_TALENT;
    c.attributes.overcome_base     = 17003;
    c.attributes.overcome_by_class = 4321;
    c.attributes.overcome_percent  = 157;
    const auto P                   = tx::Prepare(c);
    CHECK(P->Input(tx::PoZhaoEffect, tx::XUAN_MEN_PROFILE_UNIT).overcome == 17003 * (1024 + 157 + 204) / 1024 + 4321);
    auto r        = Run<ThresholdRolls>("/scast wuwu\n/scast suixing\n/scast renjian", c, 97);
    bool saw_live = false;
    for (const auto &hit : r.hits) {
        if (hit.skill == tx::DIE_REN_DAMAGE && hit.frame == 96) {
            CHECK(hit.snapshot == tx::XUAN_MEN_PROFILE_UNIT && hit.outcome == RollResult::HIT);
            CHECK(P->Damage(hit.snapshot, hit.effect, 0) > P->Damage(0, hit.effect, 0));
            saw_live = true;
        }
    }
    CHECK(saw_live);
    r = Run<ThresholdRolls>("/scast suixing\n/scast renjian\n/scast [bufftime:xuanmen<1] wuwu", c, 698);
    CHECK(r.hits[2].frame == 649 && r.hits[2].outcome == RollResult::DOUBLE);
    CHECK(r.hits.back().skill == tx::DIE_REN_DAMAGE && r.hits.back().frame == 697);
    CHECK(r.hits.back().snapshot == 0 && r.hits.back().outcome == RollResult::DOUBLE);
    CHECK(r.state.self_buff_stacks[tx::XUAN_MEN_BUFF] == 0);
    c.renjian_recipes = tx::DOT;
    r                 = Run<ThresholdRolls>("/scast shengtaiji\n/scast renjian", c, 73);
    CHECK(r.hits.back().skill == tx::REN_JIAN_DAMAGE && r.hits.back().snapshot == tx::XUAN_MEN_PROFILE_UNIT);
    CHECK(r.hits.back().outcome == RollResult::HIT); // XuanMen's crit bonus was not snapshotted.
}

void LieYunLifetime()
{
    tx::Config c;
    c.talents           = tx::DIE_REN_TALENT | tx::FENG_SHI_TALENT | tx::LIE_YUN;
    constexpr auto FOUR = "/scast wuwu\n/scast ziqi\n/scast wuwu";
    auto           r    = Run(FOUR, c, 25);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] == 4 && r.state.self_buff_stacks[tx::LIE_YUN_BUFF] == 1);
    CHECK(r.state.target_buff_expires_at[tx::HIDDEN_LIE_YUN] == 408 && r.state.target_buff_tick_at[tx::HIDDEN_LIE_YUN] == 32);
    r = Run(FOUR, c, 49);
    CHECK(r.hits.back().skill == tx::DIE_REN_DAMAGE && r.hits.back().snapshot == (tx::PURPLE_PROFILE | tx::LIE_YUN_PROFILE));
    r = Run(FOUR, c, 385);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] == 0 && r.state.target_buff_expires_at[tx::HIDDEN_LIE_YUN] == 768);
    CHECK(r.state.self_buff_expires_at[tx::LIE_YUN_BUFF] == 400);
    r = Run(FOUR, c, 769);
    CHECK(r.state.target_buff_stacks[tx::HIDDEN_LIE_YUN] == 0 && r.state.self_buff_expires_at[tx::LIE_YUN_BUFF] == 784);
    r = Run(FOUR, c, 785);
    CHECK(r.state.self_buff_stacks[tx::LIE_YUN_BUFF] == 0);

    c.talents     |= tx::QIE_YU;
    c.target.life  = .4;
    r              = Run(std::string(FOUR) + "\n/scast bahuang", c, 49);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] == 0 && r.state.target_buff_expires_at[tx::HIDDEN_LIE_YUN] == 432);
    r = Run(std::string(FOUR) + "\n/scast bahuang\n/scast wuwu", c, 73);
    CHECK(r.state.target_buff_stacks[tx::HIDDEN_LIE_YUN] == 0 && r.state.self_buff_expires_at[tx::LIE_YUN_BUFF] == 88);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] == 2);
    c.talents = tx::DIE_REN_TALENT | tx::FENG_SHI_TALENT | tx::LIE_YUN | tx::HUAN_YUE;
    r         = Run(std::string(FOUR) + "\n/scast sanhuan\n/scast sanhuan", c, 145);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] == 0 && r.state.target_buff_stacks[tx::HIDDEN_LIE_YUN] == 0);
    CHECK(r.state.self_buff_expires_at[tx::LIE_YUN_BUFF] == 160);
    r = Run(std::string(FOUR) + "\n/scast sanhuan\n/scast sanhuan", c, 161);
    CHECK(r.state.self_buff_stacks[tx::LIE_YUN_BUFF] == 0);
    c.talents |= tx::SHEN_MAI;
    r          = Run(std::string(FOUR) + "\n/scast wuwu\n/scast wuwu", c, 73);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] == 7);
    c.talents &= ~tx::LIE_YUN;
    r          = Run(std::string(FOUR) + "\n/scast wuwu\n/scast wuwu", c, 73);
    CHECK(r.state.target_buff_stacks[tx::DIE_REN] == 5 && r.state.target_buff_stacks[tx::HIDDEN_LIE_YUN] == 0);
}

void JianRuLifetime()
{
    tx::Config c;
    c.talents        = tx::JIAN_RU | tx::SHEN_MAI;
    c.initial_qidian = 0;
    auto r           = Run("/scast ziqi\n/scast bahuang\n/scast wanjian", c, 123);
    CHECK(r.hits.size() == 8);
    CHECK(r.hits[0].skill == tx::ZI_QI && r.hits[1].skill == tx::BA_HUANG);
    CHECK(r.hits[3].skill == tx::WAN_JIAN && r.hits[3].frame == 24);
    CHECK(r.hits[4].skill == tx::JIAN_RU_DAMAGE && r.hits[4].effect == tx::JianRuEffect && r.hits[4].frame == 40);
    CHECK(r.hits[5].frame == 56 && r.hits[6].frame == 72 && r.hits[7].frame == 88);
    CHECK(r.state.self_buff_stacks[tx::JIAN_RU_BUFF] == 0);
    CHECK(r.hits[5].snapshot == tx::PURPLE_PROFILE);

    c                = {};
    c.talents        = tx::JIAN_RU;
    c.initial_qidian = 0;
    r                = Run("/scast bahuang", c, 99);
    CHECK(r.hits.size() == 2 && r.state.self_buff_stacks[tx::JIAN_RU_BUFF] == 0);
    r = Run("/scast ziqi\n/scast bahuang", c, 97);
    CHECK(r.hits.size() == 3 && r.state.self_buff_stacks[tx::JIAN_RU_BUFF] == 1);
    CHECK(r.state.self_buff_expires_at[tx::JIAN_RU_BUFF] == 98);
}

void YunZhongLifetime()
{
    tx::Config c;
    c.renjian_recipes = tx::YUN_ZHONG_SUI_XING;
    auto r            = Run("/scast suixing\n/scast renjian", c, 170);
    CHECK(r.hits.size() == 10);
    CHECK(r.hits[0].skill == tx::SUI_XING && r.hits[0].frame == 16);
    CHECK(r.hits[1].skill == tx::REN_JIAN && r.hits[1].frame == 24);
    for (unsigned i = 2; i < 10; ++i) {
        CHECK(r.hits[i].skill == tx::YUN_ZHONG_DAMAGE);
        CHECK(r.hits[i].effect == tx::YunZhongSuiXingEffect);
        CHECK(r.hits[i].frame == 40 + static_cast<int>(i - 2) * 16);
    }
    CHECK(r.state.self_buff_stacks[tx::YUN_ZHONG_SUI_XING_BUFF] == 0);

    // Keep RenJian in range while checking YunZhongJian's stricter six-chi range.
    c.renjian_recipes |= tx::RANGE1;
    c.target.distance  = 6.1;
    r                  = Run("/scast suixing\n/scast renjian", c, 80);
    CHECK(r.hits.size() == 2 && r.hits[1].skill == tx::REN_JIAN);
    CHECK(r.state.self_buff_stacks[tx::YUN_ZHONG_SUI_XING_BUFF] == 1);
}

void JingHuaLifetime()
{
    tx::Config c;
    c.talents          = tx::JING_HUA_TALENT;
    auto     r         = Run("/scast tunriyue\n/scast wanjian\n/scast jinghua", c, 160);
    unsigned immediate = 0;
    unsigned fragment  = 0;
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
    CHECK(r.state.self_buff_stacks[tx::JING_HUA_BUFF] == 0);
    CHECK(r.state.skill_ready_at[tx::JING_HUA] == std::numeric_limits<tick_t>::max());

    c = {};
    r = Run("/scast jinghua", c, 80);
    CHECK(r.hits.empty());
}

void WeaponCwLifetime()
{
    struct SixteenthProc
    {
        unsigned count = 0;

        void Reset(std::uint64_t) { count = 0; }

        std::uint64_t Next() { return 0; }

        bool Proc(int numerator, int denominator)
        {
            CHECK(numerator == 31 && denominator == 1024);
            return ++count == 16;
        }
    };

    tx::Config c;
    c.equipment = tx::WEAPON_CW;
    auto r      = Run<SixteenthProc>("/cast sanhuan", c, 480);
    CHECK(r.state.self_buff_stacks[tx::WEAPON_CW_BUFF] == 0);
    r = Run<SixteenthProc>("/cast sanhuan", c, 481);
    CHECK(r.state.self_buff_stacks[tx::WEAPON_CW_BUFF] == 1);
    CHECK(r.state.self_buff_expires_at[tx::WEAPON_CW_BUFF] == 576);
    r = Run<HitsOnly>("/cast bahuang", c, 1000);
    CHECK(r.state.target_buff_stacks[tx::WAN_XIANG_BUFF] == 0);
    CHECK(std::none_of(r.hits.begin(), r.hits.end(), [](const auto &hit) { return hit.skill == tx::WAN_XIANG_DAMAGE; }));

    const auto *const MACRO = "/scast bahuang\n/scast bahuang\n/scast bahuang\n/scast bahuang\n/scast bahuang";
    r                       = Run<AlwaysProc>(MACRO, c, 1057);
    CHECK(r.hits.size() == 25);
    const std::array<tick_t, 5> FRAMES{ 0, 192, 384, 576, 600 };
    for (unsigned cast = 0; cast < FRAMES.size(); ++cast) {
        CHECK(r.hits[cast * 3].effect == tx::BaHuangEffect && r.hits[cast * 3].frame == FRAMES[cast]);
        CHECK(r.hits[cast * 3 + 1].effect == (cast == 4 ? tx::BaHuangCwEffect : tx::BaHuangExtraEffect));
        CHECK(r.hits[cast * 3 + 2].effect == tx::WeaponCwExtraEffect);
    }
    // Refresh at 600 keeps the tick at 624 and the tenth/final tick at 1056.
    for (unsigned i = 15; i < r.hits.size(); ++i) {
        CHECK(r.hits[i].effect == tx::WanXiangEffect + 1);
        CHECK(r.hits[i].frame == 624 + static_cast<int>(i - 15) * 48);
        CHECK(r.hits[i].sub == 2);
    }
    CHECK(r.state.self_buff_stacks[tx::WEAPON_CW_BUFF] == 0);
    CHECK(r.state.target_buff_stacks[tx::WAN_XIANG_BUFF] == 0);
    CHECK(r.state.skill_ready_at[tx::BA_HUANG] == 672);
    r = Run<AlwaysProc>(MACRO, c, 1056);
    CHECK(r.hits.size() == 24 && r.state.target_buff_stacks[tx::WAN_XIANG_BUFF] == 2);
}

void SetAttributeLifetime()
{
    tx::Config c;
    c.equipment = tx::SET_ATTRIBUTE;
    auto r      = Run("/scast wanjian\n/scast wuwu", c, 119);
    CHECK(r.hits.size() == 3 && r.hits[0].frame == 0 && r.hits[1].frame == 24 && r.hits[2].frame == 24);
    CHECK((r.hits[0].snapshot & tx::SET_ATTRIBUTE_PROFILE) == 0);
    CHECK((r.hits[1].snapshot & tx::SET_ATTRIBUTE_PROFILE) != 0);
    CHECK(r.state.self_buff_stacks[tx::SET_ATTRIBUTE_BUFF] == 1);
    r = Run("/scast wanjian\n/scast wuwu", c, 121);
    CHECK(r.state.self_buff_stacks[tx::SET_ATTRIBUTE_BUFF] == 0);
}

constexpr std::string_view ExplosionRotation =
    "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [buff:field_suixing&buff:field_shengtaiji&buff:field_tunriyue] renjian\n"
    "/cast [nobuff:field_suixing] suixing\n/cast [nobuff:field_shengtaiji] shengtaiji\n"
    "/cast [nobuff:field_tunriyue] tunriyue\n/cast wanjian\n/cast jinghua\n/cast [qidian>=8] wuwu\n"
    "/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";

void ExplosionModeParity()
{
    const auto MACRO = Compile(ExplosionRotation);
    tx::Config c;
    c.talents                        = tx::SupportedTalents;
    c.attributes.overcome_base       = 25000;
    c.equipment                      = tx::WEAPON_CW;
    c.renjian_recipes                = tx::COOLDOWN | tx::DOT | tx::DAMAGE40 | tx::DAMAGE60;
    const auto                  DATA = tx::Prepare(c);
    Simulation<tx::Rules>       fast(MACRO.program, tx::Rules(DATA));
    Simulation<tx::Rules, true> debug(MACRO.program, tx::Rules(DATA));
    bool                        saw_renjian = false;
    bool                        saw_wanjian = false;
    bool                        saw_lieyun  = false;
    for (unsigned seed = 0; seed < 100; ++seed) {
        fast.Start(4800, seed);
        debug.Start(4800, seed);
        debug.SetBreakpoint(1);
        for (unsigned stop = 0; stop < 12 && debug.Continue(); ++stop) {
            debug.StepOver();
        }
        debug.SetBreakpoint(1, false);
        while (debug.Continue()) {
        }
        fast.Run();
        CHECK(fast.Log().Intents() == debug.Log().Intents() && fast.Log().Mutations() == debug.Log().Mutations());
        CHECK(fast.GetRules().Fields() == debug.GetRules().Fields());
        value_t reference = 0;
        for (const auto &hit : fast.Log().Intents()) {
            reference   += ReferenceDamage(c, hit.effect, hit.snapshot, hit.outcome == RollResult::DOUBLE);
            saw_renjian |= hit.skill == tx::REN_JIAN;
            saw_wanjian |= hit.skill == tx::WAN_JIAN;
            saw_lieyun  |= (hit.snapshot & tx::LIE_YUN_PROFILE) != 0;
        }
        CHECK(fast.TotalDamage() == reference && debug.TotalDamage() == reference);
    }
    CHECK(saw_renjian && saw_wanjian && saw_lieyun);
    count_allocations = true;
    for (unsigned seed = 0; seed < 100; ++seed) {
        fast.Start(4800, seed);
        fast.Run();
        (void)fast.TotalDamage();
    }
    count_allocations = false;
    CHECK(allocations == 0);
    BatchOptions options;
    options.iterations = 1000;
    const auto A       = RunBatch(MACRO.program, options, tx::Rules(DATA));
    options.workers    = 4;
    const auto B       = RunBatch(MACRO.program, options, tx::Rules(DATA));
    CHECK(A.checksum == B.checksum && A.min_damage == B.min_damage && A.max_damage == B.max_damage);
    fast.Start(1, 0);
    fast.Run();
    CHECK(fast.GetState().self_buff_stacks[tx::XUAN_MEN_BUFF] == 0 && fast.GetState().self_buff_stacks[tx::LIE_YUN_BUFF] == 0);
    CHECK(fast.GetState().target_buff_stacks[tx::HIDDEN_LIE_YUN] == 0 && fast.GetState().target_buff_stacks[tx::REN_JIAN_DOT] == 0);
}

void FieldModeParity()
{
    const auto MACRO = Compile(FieldRotation);
    tx::Config c;
    c.talents                        = tx::SupportedTalents;
    c.attributes.overcome_base       = 25000;
    const auto                  DATA = tx::Prepare(c);
    Simulation<tx::Rules>       fast(MACRO.program, tx::Rules(DATA));
    Simulation<tx::Rules, true> debug(MACRO.program, tx::Rules(DATA));
    for (unsigned seed = 0; seed < 100; ++seed) {
        fast.Start(4800, seed);
        debug.Start(4800, seed);
        debug.SetBreakpoint(1);
        CHECK(debug.Continue() && debug.GetState().preparing_skill == tx::SUI_XING);
        CHECK(debug.GetState().now == 0 && debug.Log().Intents().empty());
        debug.SetBreakpoint(1, false);
        while (debug.Continue()) {
        }
        fast.Run();
        CHECK(fast.Log().Intents() == debug.Log().Intents() && fast.Log().Mutations() == debug.Log().Mutations());
        CHECK(fast.GetRules().Fields() == debug.GetRules().Fields());
        value_t reference = 0;
        for (const auto &hit : fast.Log().Intents()) {
            reference += ReferenceDamage(c, hit.effect, hit.snapshot, hit.outcome == RollResult::DOUBLE);
        }
        CHECK(fast.TotalDamage() == reference && debug.TotalDamage() == reference);
    }
    count_allocations = true;
    for (unsigned seed = 0; seed < 100; ++seed) {
        fast.Start(4800, seed);
        fast.Run();
        (void)fast.TotalDamage();
    }
    count_allocations = false;
    CHECK(allocations == 0);
    BatchOptions options;
    options.iterations = 1000;
    const auto A       = RunBatch(MACRO.program, options, tx::Rules(DATA));
    options.workers    = 4;
    const auto B       = RunBatch(MACRO.program, options, tx::Rules(DATA));
    CHECK(A.checksum == B.checksum);
    const auto            PREPARING_MACRO = Compile("/scast suixing");
    Simulation<tx::Rules> restart(PREPARING_MACRO.program, tx::Rules(DATA));
    restart.Start(1, 0);
    restart.Run();
    CHECK(restart.GetState().preparing_skill == tx::SUI_XING);
    restart.Start(17, 0);
    restart.Run();
    CHECK(restart.Log().Size() == 1 && restart.Log().Intents()[0].frame == 16);
    restart.Start(1, 0);
    restart.Run();
    CHECK(restart.GetState().self_buff_stacks[tx::SUI_XING_BUFF] == 0);
    for (const auto &field : restart.GetRules().Fields()) {
        CHECK(field.skill == INVALID_SLOT);
    }
}

constexpr std::string_view Rotation =
    "/cast [nobuff:ziqi&qidian<6] ziqi\n/cast [qidian>=8] wuwu\n"
    "/cast bahuang\n/cast sanhuan\n/cast sanchai\n/cast [qidian>=1] wuwu";

void ModeParity()
{
    const auto MACRO = Compile(Rotation);
    tx::Config c;
    c.talents = tx::DIE_REN_TALENT | tx::WU_YI | tx::SHEN_MAI | tx::FENG_SHI_TALENT | tx::WU_YU | tx::XIN_GU;
    const auto                  DATA = tx::Prepare(c);
    Simulation<tx::Rules>       fast(MACRO.program, tx::Rules(DATA));
    Simulation<tx::Rules, true> debug(MACRO.program, tx::Rules(DATA));
    static_assert(sizeof(fast) < sizeof(debug));
    for (unsigned seed = 0; seed < 100; ++seed) {
        fast.Start(4800, seed);
        debug.Start(4800, seed);
        debug.SetBreakpoint(1);
        CHECK(debug.Continue() && debug.DebugControl().paused);
        debug.StepInto();
        debug.StepOver();
        debug.SetBreakpoint(1, false);
        while (debug.Continue()) {
        }
        fast.Run();
        CHECK(fast.Log().Intents() == debug.Log().Intents());
        CHECK(fast.Log().Mutations() == debug.Log().Mutations());
        CHECK(fast.TotalDamage() == debug.TotalDamage());
        CHECK(fast.GetState().qidian == debug.GetState().qidian);
        value_t reference = 0;
        for (const auto &hit : fast.Log().Intents()) {
            reference += ReferenceDamage(c, hit.effect, hit.snapshot, hit.outcome == RollResult::DOUBLE);
        }
        CHECK(fast.TotalDamage() == reference);
    }
    count_allocations = true;
    for (unsigned i = 0; i < 100; ++i) {
        fast.Start(4800, i);
        fast.Run();
        (void)fast.TotalDamage();
    }
    count_allocations = false;
    CHECK(allocations == 0);
    BatchOptions options;
    options.iterations = 1000;
    const auto A       = RunBatch(MACRO.program, options, tx::Rules(DATA));
    options.workers    = 4;
    const auto B       = RunBatch(MACRO.program, options, tx::Rules(DATA));
    CHECK(A.checksum == B.checksum && A.min_damage == B.min_damage && A.max_damage == B.max_damage);
}

void YouRenLifetime()
{
    tx::Config c;
    c.team_effects = tx::YOU_REN;
    auto r         = Run<AlwaysProc>("/scast wuwu", c, 321);
    CHECK(r.state.self_buff_stacks[tx::YOU_REN_BUFF] == 0);
    CHECK(r.state.self_buff_expires_at[tx::YOU_REN_BUFF] == 0);

    r                  = Run<FirstCritThenThreshold>("/scast wuwu\n/scast sanhuan\n/scast wuwu", c, 49);
    unsigned wuwu_hits = 0;
    for (const auto &hit : r.hits) {
        if (hit.skill == tx::WU_WO) {
            ++wuwu_hits;
            if (wuwu_hits == 1 || wuwu_hits == 2) {
                CHECK(hit.outcome == RollResult::DOUBLE);
            }
        }
    }
    CHECK(wuwu_hits == 2);
    CHECK(r.state.self_buff_stacks[tx::YOU_REN_BUFF] == 5);
    CHECK(r.state.self_buff_expires_at[tx::YOU_REN_BUFF] == 368);

    c.team_effects = 0;
    r              = Run<FirstCritThenThreshold>("/scast wuwu\n/scast sanhuan\n/scast wuwu", c, 49);
    wuwu_hits      = 0;
    for (const auto &hit : r.hits) {
        if (hit.skill == tx::WU_WO) {
            ++wuwu_hits;
            if (wuwu_hits == 2) {
                CHECK(hit.outcome == RollResult::HIT);
            }
        }
    }
    CHECK(wuwu_hits == 2);
}

void ActionDiagnostics()
{
    const auto OBSERVE = [](std::string_view text, tx::Config config, auto setup) {
        const auto                  MACRO = Compile(text);
        Simulation<tx::Rules, true> sim(MACRO.program, tx::Rules(tx::Prepare(config)));
        MacroObservation            observed;
        sim.SetDebugObserver(
            [](void *context, const MacroObservation &step) {
                if (step.phase == DebugPhase::ACTION) {
                    *static_cast<MacroObservation *>(context) = step;
                }
            },
            &observed);
        sim.Start(100, 1);
        setup(sim.MutableState());
        sim.StepOver();
        CHECK(observed.phase == DebugPhase::ACTION);
        return observed;
    };
    const auto UNCHANGED = [](State &) {
    };
    auto config           = tx::Config{};
    config.initial_qidian = 0;
    auto step             = OBSERVE("/cast wuwu", config, [](State &s) { s.qidian = 0; });
    CHECK(!step.result && step.action.reason == ActionBlockReason::QIDIAN && step.action.actual == 0 && step.action.required == 1);
    config                 = {};
    config.target.distance = 5;
    step                   = OBSERVE("/cast wuwu", config, UNCHANGED);
    CHECK(step.action.reason == ActionBlockReason::DISTANCE && step.action.actual == 5 && step.action.required == 4);
    step = OBSERVE("/cast wuwu", {}, [](State &s) { s.gcd_ready_at[0] = 16; });
    CHECK(step.action.reason == ActionBlockReason::GLOBAL_COOLDOWN && step.action.actual == 16);
    step = OBSERVE("/cast bahuang", {}, [](State &s) {
        s.skill_ready_at[tx::BA_HUANG] = 32;
        s.gcd_ready_at[0]              = 16;
    });
    CHECK(step.action.reason == ActionBlockReason::SKILL_COOLDOWN && step.action.actual == 32);
    step = OBSERVE("/cast bahuang", {}, [](State &s) {
        s.self_buff_stacks[tx::WEAPON_CW_BUFF] = 1;
        s.skill_ready_at[tx::BA_HUANG]         = 32;
        s.gcd_ready_at[0]                      = 16;
    });
    CHECK(step.action.reason == ActionBlockReason::GLOBAL_COOLDOWN && step.action.actual == 16);
    step = OBSERVE("/cast ziqi", {}, [](State &s) { s.skill_energy[tx::ZI_QI] = 0; });
    CHECK(step.action.reason == ActionBlockReason::CHARGES);
    step = OBSERVE("/fcast wuwu", {}, [](State &s) {
        s.preparing_skill                = tx::SUI_XING;
        s.skill_prepare_at[tx::SUI_XING] = 16;
    });
    CHECK(step.action.reason == ActionBlockReason::PREPARING && step.action.subject == tx::SUI_XING && step.action.actual == 16);
    step = OBSERVE("/cast wuwu", {}, [](State &s) {
        s.casting_skill                  = tx::WAN_JIAN;
        s.skill_casting_at[tx::WAN_JIAN] = 24;
    });
    CHECK(step.action.reason == ActionBlockReason::CHANNELING && step.action.subject == tx::WAN_JIAN);
    step = OBSERVE("/cast jinghua", {}, UNCHANGED);
    CHECK(step.action.reason == ActionBlockReason::MISSING_TALENT);
    config         = {};
    config.talents = tx::JING_HUA_TALENT;
    step           = OBSERVE("/cast jinghua", config, UNCHANGED);
    CHECK(step.action.reason == ActionBlockReason::MISSING_BUFF);

    // This migrated skill subset has no channel starter; seed an active owner
    // to exercise the real force-cast rejection path and its state mutations.
    const auto CHANNEL = [](State &s) {
        s.casting_skill                  = tx::WAN_JIAN;
        s.skill_casting_at[tx::WAN_JIAN] = 24;
        s.gcd_ready_at[0]                = 16;
    };
    const auto                    MACRO = Compile("/fcast wuwu\n/cast sanhuan");
    Simulation<tx::Rules, true>   debug(MACRO.program, tx::Rules{});
    std::vector<MacroObservation> observations;
    debug.SetDebugObserver(
        [](void *context, const MacroObservation &event) {
            static_cast<std::vector<MacroObservation> *>(context)->push_back(event);
        },
        &observations);
    debug.Start(100, 7);
    CHANNEL(debug.MutableState());
    debug.Continue();
    const auto INTERRUPTION = std::find_if(observations.begin(), observations.end(), [](const auto &event) {
        return event.action.interrupted == tx::WAN_JIAN;
    });
    CHECK(INTERRUPTION != observations.end() && !INTERRUPTION->result);
    CHECK(INTERRUPTION->action.reason == ActionBlockReason::GLOBAL_COOLDOWN);
    CHECK(std::count_if(observations.begin(), observations.end(), [](const auto &event) {
              return event.action.interrupted != INVALID_SLOT;
          }) == 1); // The interruption must not leak into later observations.
    Simulation<tx::Rules> fast(MACRO.program, tx::Rules{});
    fast.Start(100, 7);
    CHANNEL(fast.MutableState());
    fast.Run();
    CHECK(fast.Log().Mutations() == debug.Log().Mutations());
    CHECK(fast.Log().Intents() == debug.Log().Intents());
    CHECK(fast.TotalDamage() == debug.TotalDamage());
}

void Validation()
{
    auto c    = tx::Config{};
    c.talents = 1 << 19;
    Rejects([&] { tx::Prepare(c); });
    c                       = {};
    c.recipes[tx::BA_HUANG] = tx::CRIT2;
    Rejects([&] { tx::Prepare(c); });
    c                    = {};
    c.shengtaiji_recipes = tx::DAMAGE3;
    Rejects([&] { tx::Prepare(c); });
    c                 = {};
    c.renjian_recipes = tx::PREPARE1;
    Rejects([&] { tx::Prepare(c); });
    c                 = {};
    c.renjian_recipes = tx::RANGE1 | tx::RANGE2 | tx::RANGE3 | tx::DOT | tx::COOLDOWN;
    Rejects([&] { tx::Prepare(c); });
    c         = {};
    c.talents = tx::XUAN_MEN;
    Rejects([&] { tx::Prepare(c); });
    c              = {};
    c.team_effects = 2;
    Rejects([&] { tx::Prepare(c); });
    c                             = {};
    c.attributes.overcome_percent = 102;
    Rejects([&] { tx::Prepare(c); });
    c             = {};
    c.target.life = 0;
    Rejects([&] { tx::Prepare(c); });
    c                 = {};
    c.target.distance = std::numeric_limits<double>::quiet_NaN();
    Rejects([&] { tx::Prepare(c); });
    CHECK(!MacroCompiler::Compile("/cast unmigrated", tx::MacroOptions()).second.empty());
    CHECK(!MacroCompiler::Compile("/cast pozhao", tx::MacroOptions()).second.empty());
    auto chinese =
        MacroCompiler::Compile(std::string("/cast ") + std::string(tx::Skills.entries[tx::WU_WO].name), tx::MacroOptions());
    CHECK(chinese.second.empty() && chinese.first.program.lines[0].action_slot == tx::WU_WO);
    c                        = {};
    c.attributes.attack_base = c.attributes.attack_by_class = c.attributes.weapon = 1000000;
    c.attributes.crit_power = c.attributes.overcome = c.attributes.strain = c.attributes.surplus = 1000000;
    c.attributes.attack_percent = c.attributes.crit_power_percent = c.attributes.damage_percent = 10000;
    c.attributes.strain_percent = c.attributes.pve_percent = c.target.vulnerable = 10000;
    c.target.level                                                               = 110;
    c.target.shield                                                              = 0;
    c.talents                                                                    = tx::SupportedTalents;
    c.attributes.overcome_base = c.attributes.overcome_by_class = 1000000;
    c.attributes.overcome_percent                               = 10000;
    auto extremes                                               = tx::Prepare(c);
    for (unsigned effect = 0; effect < tx::EffectCount; ++effect) {
        CHECK(extremes->Damage(1, effect, 1) == ReferenceDamage(c, effect, true, true));
    }
    auto macro = Compile("/cast wuwu");
    Rejects([&] { const Simulation<tx::Rules> TOO_LARGE(macro.program, tx::Rules(extremes), 100000, 1); });
    State initial;
    initial.Resize(tx::SKILL_COUNT, tx::BUFF_COUNT);
    initial.skill_gcd_group[tx::WU_WO] = 0;
    auto            state              = initial;
    auto            scratch            = initial;
    DamageIntentLog log;
    SetGcd(state, log, 0, 24);
    SetSkillEnergy(state, log, tx::ZI_QI, 1);
    SetPrepare(state, log, tx::SUI_XING, 16, true);
    log.Record(0, tx::WU_WO, 0, 0, 0, 0, 0);
    (void)log.Reduce(initial, scratch, [](const auto &, const State &s) -> value_t {
        CHECK(s.SkillReadyAt(tx::WU_WO) == 24 && s.skill_energy[tx::ZI_QI] == 1);
        CHECK(s.preparing_skill == tx::SUI_XING && s.skill_prepare_at[tx::SUI_XING] == 16);
        return 0;
    });
}

void TeamSnapshots()
{
    tx::Config c;
    c.talents                  = tx::DIE_REN_TALENT;
    c.attributes.overcome_base = 25000;
    c.team_buffs = tx::team::Parse("ji_lei:0:10:1;chao_sheng:0:10:12;han_chang_lin_li:0:10:1;"
                                   "ji_lei:36:20:1;zhen_fen:24:48:10;nong_mei:24:48:1;po_feng:24:48:1;jie_huo:24:48:1");
    const auto MACRO = Compile("/scast wuwu");
    using Rules      = tx::BasicRules<ThresholdRolls, FixedQidianPhase>;
    Simulation<Rules> sim(MACRO.program, Rules(tx::Prepare(c)));
    sim.Start(145, 0);
    sim.Run();
    CHECK(sim.Log().Size() == 5); // WuWo, PoZhao, three DOT ticks.
    const auto &versions = sim.GetRules().Versions();
    const auto &first    = sim.Log().Intents().front();
    const auto &tick     = sim.Log().Intents()[2];
    CHECK(first.outcome == RollResult::DOUBLE && tick.outcome == RollResult::DOUBLE);
    CHECK(tick.frame == 48 && tick.snapshot_version == first.snapshot_version && tick.live_version != tick.snapshot_version);
    CHECK(versions[tick.snapshot_version].input.attack == 10000 * (1024 + 205) / 1024 + 2500);
    CHECK(versions[tick.snapshot_version].input.strain == 14800);
    CHECK(versions[tick.live_version].input.overcome == (25000 + 600 + 700) * (1024 + 205) / 1024);
    auto expected = c;
    expected.team_buffs.clear();
    expected.attributes.attack_percent    = 205;
    expected.attributes.strain            = 14800;
    expected.attributes.overcome_base    += 600 + 700;
    expected.attributes.overcome_percent  = 205;
    expected.attributes.ignore_shield     = 205;
    expected.target.shield               -= 1150;
    expected.target.vulnerable            = 20;
    CHECK(sim.GetRules().Reduce(tick) == ReferenceDamage(expected, tick.effect, 0, true));
    const auto &later = sim.Log().Intents()[3];
    CHECK(later.frame == 96 && later.outcome == RollResult::DOUBLE);
    expected.attributes.overcome_base    = 25000;
    expected.attributes.overcome_percent = expected.attributes.ignore_shield = expected.target.vulnerable = 0;
    expected.target.shield                                                                                = 30000;
    CHECK(sim.GetRules().Reduce(later) == ReferenceDamage(expected, later.effect, 0, true));
    CHECK(versions[first.snapshot_version].input.strain == 14800); // Historical version stays immutable.

    c.team_buffs                     = tx::team::Parse("zhen_fen:0:20:10;zhen_fen:5:30:20;zhen_fen:36:10:0;"
                                                       "chao_sheng:0:20:24;chao_sheng:7:30:1");
    auto                       buffs = Run("/cast sanchai", c, 38);
    const auto                 ZHEN  = static_cast<Slot>(tx::TEAM_BUFF_BEGIN + static_cast<Slot>(tx::team::ZHEN_FEN));
    std::vector<StateMutation> zhen_changes;
    for (const auto &m : buffs.mutations) {
        if (m.kind == MutationKind::BUFF && m.slot == ZHEN) {
            zhen_changes.push_back(m);
        }
    }
    CHECK(zhen_changes.size() == 4);
    CHECK(zhen_changes[0].c == 10 && zhen_changes[1].c == 20 && zhen_changes[1].a == 35);
    CHECK(zhen_changes[2].frame == 35 && zhen_changes[2].c == 0);
    CHECK(buffs.state.self_buff_stacks[ZHEN] == 0);

    // External applications and expiries precede DOT ticks at the same frame.
    c.team_buffs = tx::team::Parse("ji_lei:48:48:1");
    Simulation<Rules> boundary(MACRO.program, Rules(tx::Prepare(c)));
    boundary.Start(145, 0);
    boundary.Run();
    const auto &hits = boundary.Log().Intents();
    CHECK(boundary.GetRules().Versions()[hits[2].live_version].input.overcome == 25000 * (1024 + 205) / 1024);
    CHECK(boundary.GetRules().Versions()[hits[3].live_version].input.overcome == 25000);
    CHECK(boundary.GetRules().Versions()[hits[4].live_version].input.overcome == 25000);
    CHECK(boundary.GetRules().Versions()[hits[2].snapshot_version].input.attack == 12500);
    const auto PREPARING = Compile("/scast suixing");
    c.team_buffs         = tx::team::Parse("po_feng:1:10:1;han_ru_lei:16:30:1");
    Simulation<Rules> prepare_order(PREPARING.program, Rules(tx::Prepare(c)));
    prepare_order.Start(17, 0);
    prepare_order.Run();
    const auto &field_hit = prepare_order.Log().Intents().front();
    CHECK(field_hit.frame == 16);
    CHECK(prepare_order.GetRules().Versions()[field_hit.snapshot_version].input.attack == 10000 * (1024 + 51) / 1024 + 2500);

    c.team_buffs       = tx::team::Parse("ji_lei:0:10:1;ji_lei:10:20:1");
    auto       refresh = Run("/cast sanchai", c, 31);
    const auto JI      = static_cast<Slot>(tx::TEAM_BUFF_BEGIN + static_cast<Slot>(tx::team::JI_LEI));
    CHECK(std::none_of(refresh.mutations.begin(), refresh.mutations.end(), [JI](const auto &m) {
        return m.kind == MutationKind::BUFF && m.slot == JI && m.frame == 10 && m.c == 0;
    }));
    Rejects([] { tx::team::Parse("ji_lei:0:1:2"); });
    Rejects([] { tx::team::Parse("unknown:0:1:1"); });
    Rejects([] { tx::team::Parse("ji_lei:0:1"); });
    Rejects([] { tx::team::Parse("ji_lei:0:1:1:2"); });
    Rejects([] { tx::team::Parse("ji_lei:-1:1:1"); });
    c.attributes.overcome_base = -1;
    Rejects([&] { tx::Prepare(c); });
    Rejects([] { tx::team::Parse("hao_ling_san_jun:0:0:3"); });
    Rejects([] { tx::team::Parse("hao_ling_san_jun:0:480:48"); });
    c.attributes.overcome_base = 25000;
    c.team_buffs               = tx::team::Parse("hao_ling_san_jun:0:0:48;hao_ling_san_jun:57:0:24");
    const auto DRUMS           = Run("/cast sanchai", c, 961);
    const auto DRUM            = static_cast<Slot>(tx::TEAM_BUFF_BEGIN + static_cast<Slot>(tx::team::HAO_LING_SAN_JUN));
    std::vector<StateMutation> drum_changes;
    for (const auto &m : DRUMS.mutations) {
        if (m.kind == MutationKind::BUFF && m.slot == DRUM) {
            drum_changes.push_back(m);
        }
    }
    CHECK(drum_changes.size() == 3);
    CHECK(drum_changes[0].frame == 0 && drum_changes[0].c == 48);
    CHECK(drum_changes[1].frame == 480 && drum_changes[1].c == 24);
    CHECK(drum_changes[2].frame == 960 && drum_changes[2].c == 0);
}

void TeamVersionReuse()
{
    tx::Config c;
    c.attributes.overcome_base = 25000;
    c.talents                  = tx::DIE_REN_TALENT | tx::SHEN_MAI;
    for (Slot kind = 0; kind < tx::team::COUNT; ++kind) {
        const auto &d = tx::team::Definitions[kind];
        c.team_buffs.push_back({ static_cast<tx::team::Kind>(kind), 0, 0, d.max_stacks });
        c.team_buffs.push_back({ static_cast<tx::team::Kind>(kind),
                                 1601,
                                 kind == tx::team::HAO_LING_SAN_JUN ? 960 : 37,
                                 kind == tx::team::HAO_LING_SAN_JUN ? 2 : 1 });
    }
    const auto                  MACRO = Compile("/cast [qidian>=8] wuwu\n/cast bahuang\n/cast sanhuan\n/cast sanchai");
    const auto                  DATA  = tx::Prepare(c);
    Simulation<tx::Rules>       fast(MACRO.program, tx::Rules(DATA));
    Simulation<tx::Rules, true> debug(MACRO.program, tx::Rules(DATA));
    for (int seed = 0; seed < 20; ++seed) {
        fast.Start(4800, seed);
        fast.Run();
        debug.Start(4800, seed);
        while (debug.Continue()) {
        }
        CHECK(fast.Log().Intents() == debug.Log().Intents());
        CHECK(fast.Log().Mutations() == debug.Log().Mutations());
        CHECK(fast.TotalDamage() == debug.TotalDamage());
        CHECK(fast.GetRules().Versions().Size() < 100);
    }
    allocations       = 0;
    count_allocations = true;
    for (int seed = 0; seed < 20; ++seed) {
        fast.Start(4800, seed);
        fast.Run();
        (void)fast.TotalDamage();
    }
    count_allocations = false;
    CHECK(allocations == 0);
    BatchOptions options;
    options.iterations = 100;
    const auto A       = RunBatch(MACRO.program, options, tx::Rules(DATA));
    options.workers    = 4;
    const auto B       = RunBatch(MACRO.program, options, tx::Rules(DATA));
    CHECK(A.checksum == B.checksum);
    c.attribute_version_capacity = 1;
    Simulation<tx::Rules> small(MACRO.program, tx::Rules(tx::Prepare(c)));
    bool                  exhausted = false;
    try {
        small.Start(4800, 0);
        small.Run();
    } catch (const std::length_error &) {
        exhausted = true;
    }
    CHECK(exhausted);
    small.Start(1, 0);
    small.Run();
    CHECK(small.GetRules().Versions().Size() == 1 && small.TotalDamage() > 0);
}

int main()
{
    OffGcdDuringCooldown();
    TeamSnapshots();
    TeamVersionReuse();
    ArithmeticOracle();
    TimingsAndResources();
    NaturalQidianRegeneration();
    ChargesAndSnapshots();
    TalentOrder();
    ModeParity();
    Validation();
    ActionDiagnostics();
    PreparedFields();
    FieldOwnership();
    FieldDamageInputs();
    FieldModeParity();
    FieldExplosions();
    SplitSnapshots();
    LieYunLifetime();
    JianRuLifetime();
    YunZhongLifetime();
    JingHuaLifetime();
    WeaponCwLifetime();
    SetAttributeLifetime();
    YouRenLifetime();
    ExplosionModeParity();
    std::cout << "Tai Xu migration: " << (120 * tx::ProfileCount * tx::EffectCount * 2) + tx::EffectCount
              << " legacy damage comparisons, field/explosion/JianRu/YunZhong/JingHua/YouRen/live-snapshot checks, "
              << "300 mode comparisons, zero hot allocations, batch parity passed\n";
}
