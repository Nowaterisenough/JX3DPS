#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <random>
#include <stdexcept>

#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"

#include "check.hpp"

#define JX3DPS LegacyReference
#include "legacy_reference/Damage.hpp"
#undef JX3DPS

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace mw = JX3DPS::runtime::mo_wen;

static bool        counting    = false;
static std::size_t allocations = 0;

void *operator new(std::size_t n)
{
    if (counting) {
        ++allocations;
    }
    if (auto *p = std::malloc((n != 0u) ? n : 1)) {
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

struct HitsOnly
{
    void Reset(std::uint64_t) { }

    std::uint64_t Next() { return ~std::uint64_t{ 0 }; }

    int Integer(int, int high) { return high; }
};

struct AlwaysProc
{
    void Reset(std::uint64_t) { }

    std::uint64_t Next() { return 0; }

    int Integer(int low, int) { return low; }
};

CompiledMacro Compile(std::string_view text, bool debug = true)
{
    auto [macro, errors] = MacroCompiler::Compile(text, mw::MacroOptions(debug));
    for (const auto &e : errors) {
        std::cerr << e.message << '\n';
    }
    CHECK(errors.empty());
    return macro;
}

struct Result
{
    State                      state;
    std::vector<DamageIntent>  hits;
    std::vector<StateMutation> mutations;
    value_t                    damage = 0;
    mw::Style                  style{};
    std::array<mw::Shadow, 6>  shadows{};
};

template <typename Rng = HitsOnly>
Result Run(std::string_view text, mw::Config config = {}, tick_t duration = 640)
{
    const auto MACRO = Compile(text);
    using Rules      = mw::BasicRules<Rng>;
    Simulation<Rules> sim(MACRO.program, Rules(mw::Prepare(config)), 20000, 50000);
    sim.Start(duration, 7);
    sim.Run();
    return { sim.GetState(),    sim.Log().Intents(),           sim.Log().Mutations(),
             sim.TotalDamage(), sim.GetRules().CurrentStyle(), sim.GetRules().Shadows() };
}

std::vector<tick_t> Frames(const Result &r, mw::Effect effect)
{
    std::vector<tick_t> frames;
    for (const auto &hit : r.hits) {
        if (hit.effect == effect) {
            frames.push_back(hit.frame);
        }
    }
    return frames;
}

template <typename Rng = HitsOnly>
struct Harness
{
    static constexpr bool debug_enabled = false;
    State                 state;
    DeadlineQueue         queue;
    DamageIntentLog       log{ 20000, 50000 };
    mw::BasicRules<Rng>   rules;

    explicit Harness(mw::Config c = {}) : rules(mw::Prepare(c))
    {
        state.Resize(mw::SKILL_COUNT, mw::BUFF_COUNT);
        rules.Setup(*this);
        rules.Reset(*this, 7);
    }

    State &MutableState() { return state; }

    const State &GetState() const { return state; }

    DeadlineQueue &Timers() { return queue; }

    DamageIntentLog &Log() { return log; }

    void Advance(tick_t target)
    {
        CHECK(target >= state.now);
        while (auto d = queue.NextDelta()) {
            if (*d > target - state.now) {
                break;
            }
            queue.Advance(*d);
            state.now = queue.Now();
            queue.DispatchDue([this](auto h, auto id) { rules.OnTimer(*this, h, id); });
        }
        queue.Advance(target - state.now);
        state.now = target;
    }

    bool Cast(mw::SkillSlot skill, Command command = Command::CAST)
    {
        return rules.Cast(*this, { 0, 0, skill, command });
    }
};

void NamesAndValidation()
{
    const auto OPTIONS = mw::MacroOptions();
    for (Slot b = 0; b < mw::BUFF_COUNT; ++b) {
        CHECK(OPTIONS.resolve_buff(mw::Buffs.entries[b].name) == b);
        const auto NAME          = mw::Buffs.entries[b].name;
        const auto BARE          = Compile("/cast [buff:" + std::string(NAME) + "] yu");
        const auto EXPLICIT_ZERO = Compile("/cast [buff:" + std::string(NAME) + ">0] yu");
        CHECK(BARE.program.conditions.front().comparator == EXPLICIT_ZERO.program.conditions.front().comparator);
    }
    CHECK(OPTIONS.resolve_skill("pozhao") == INVALID_SLOT);
    CHECK(OPTIONS.resolve_buff(mw::Skills.entries[mw::SHANG].name) == mw::SHANG_DOT);
    for (const auto value : { -1LL, 1000001LL }) {
        mw::Config c;
        c.attributes.attack_base = value;
        Rejects([&] { mw::Prepare(c); });
    }
    mw::Config c;
    c.equipment = 1u << 7;
    Rejects([&] { mw::Prepare(c); });
    c                = {};
    c.initial_yingzi = 7;
    Rejects([&] { mw::Prepare(c); });
    c            = {};
    c.recipes[0] = mw::SHANG_CRIT2;
    Rejects([&] { mw::Prepare(c); });
    c                 = {};
    c.target.distance = std::numeric_limits<double>::quiet_NaN();
    Rejects([&] { mw::Prepare(c); });
    c                        = {};
    c.attributes.attack_base = c.attributes.overcome = c.attributes.crit_power = c.attributes.strain = 1000000;
    c.attributes.attack_percent = c.attributes.damage_percent = c.attributes.overcome_percent = 10000;
    c.attributes.strain_percent = c.attributes.pve_percent = c.target.vulnerable = 10000;
    c.target.level                                                               = 110;
    Rejects([&] { mw::Prepare(c); });
    CHECK(Run("/scast zhenglv1").hits.empty());
    c                 = {};
    c.target.distance = 21;
    CHECK(Run("/cast biangong", c).hits.empty());
    c.recipes[0] = mw::GONG_RANGE;
    CHECK(!Run("/cast biangong", c).hits.empty());
}

void DamageArithmetic()
{
    std::mt19937 random(42);
    for (unsigned i = 0; i < 4000; ++i) {
        const mw::DamageInput A{
            static_cast<value_t>(random() % 90000),
            static_cast<value_t>(random() % 40000),
            static_cast<value_t>(random() % 60000),
            static_cast<value_t>(random() % 30000),
            static_cast<value_t>(random() % 12000),
            int(random() % 2000),
            int(random() % 1000),
            int(random() % 1000),
            int(random() % 1000),
            int(random() % 700),
            int(random() % 1100)
        };
        mw::Target t;
        t.level       = 110 + (random() % 21);
        t.shield      = random() % 70000;
        t.vulnerable  = random() % 500;
        const auto &f = mw::Formulas[i % mw::E_COUNT];
        for (int critical = 0; critical < 2; ++critical) {
            using namespace LegacyReference;
            Value_t raw = 0;
            if (f.kind == mw::FormulaKind::SURPLUS) {
                raw = EffectDamage(SurplusDamage(A.surplus, f.channel, 120), A.damage_percent);
            } else if (f.kind != mw::FormulaKind::ZERO) {
                const auto coefficient =
                    f.kind == mw::FormulaKind::DOT ? MagicDotDamageCoefficient(f.channel, f.dot_count, f.dot_interval)
                                                   : MagicDamageCoefficient(f.channel, 0);
                raw = EffectDamageAll(A.attack, coefficient, 0, 0, f.fixed, A.damage_percent);
            }
            const auto EXPECTED = FinalMagicDamage(
                120,
                t.level,
                raw,
                t.shield,
                0,
                A.ignore_shield,
                A.effect_ignore_shield,
                A.overcome,
                critical,
                A.crit_power,
                A.crit_power_percent,
                A.strain,
                A.strain_percent,
                A.pve_percent,
                t.vulnerable);
            CHECK(mw::CalculateDamage(f, A, t, critical) != -1);
            CHECK(mw::CalculateDamage(f, A, t, critical) == EXPECTED);
        }
    }
    mw::Config c;
    c.talents    = mw::HAO_ZHONG | mw::FEI_FAN | mw::SHI_XIANG;
    c.equipment  = mw::WEAPON_CW | mw::SET_SKILL;
    c.recipes    = { mw::GONG_DAMAGE3 | mw::GONG_CRIT2,
                     mw::SHANG_DAMAGE3 | mw::SHANG_DAMAGE4 | mw::SHANG_DAMAGE5,
                     mw::ZHI_DAMAGE4,
                     mw::YU_CRIT3 | mw::YU_DAMAGE3 };
    const auto P = mw::Prepare(c);
    CHECK(P->formulas[mw::E_SHANG].channel == 120);
    CHECK(P->formulas[mw::E_SHANG_DOT].channel == 294);
    CHECK(P->Input(mw::E_GONG, 0).damage_percent == 31 + 51 + 102);
    CHECK(P->Input(mw::E_BIAN_ZHI, mw::MingJinProfile).damage_percent == 41 + 51 + 102 + 152 + 205);
    CHECK(P->Input(mw::E_YU, 0).effect_ignore_shield == 614);
    CHECK(std::abs(P->Chance(mw::E_GONG, 0) - (20000 / (9.530 * 8250) + .12)) < 1e-12);
    CHECK(P->Input(mw::E_GONG, mw::CanLianProfile).attack == 10000 * 1126 / 1024 + 2500);
}

void DotTiming()
{
    CHECK(Frames(Run("/scast shang"), mw::E_SHANG_DOT) == std::vector<tick_t>({ 48, 96, 144, 192, 240, 288 }));
    CHECK(Frames(Run("/scast jue"), mw::E_JUE_DOT) == std::vector<tick_t>({ 48, 96, 144, 192, 240, 288 }));
    CHECK(Frames(Run("/scast shang\n/scast shang"), mw::E_SHANG_DOT) == std::vector<tick_t>({ 48, 96, 144, 192, 240, 288 }));
    CHECK(Frames(Run("/scast gaoshan\n/scast shang"), mw::E_SHANG_DOT) == std::vector<tick_t>({ 39, 78, 117, 156, 195, 234 }));
    CHECK(Frames(Run("/scast shang\n/scast gaoshan"), mw::E_SHANG_DOT) == std::vector<tick_t>({ 48, 96, 144, 192, 240, 288 }));
    CHECK(Run("/scast shang").state.target_buff_stacks[mw::SHANG_DOT] == 0);
    // Refresh exactly at expiry after the final old tick creates a fresh six-tick DOT.
    Harness h;
    CHECK(h.Cast(mw::SHANG));
    h.Advance(288);
    CHECK(h.Cast(mw::SHANG));
    h.Advance(576);
    CHECK(std::count_if(h.log.Intents().begin(), h.log.Intents().end(), [](auto &x) {
              return x.effect == mw::E_SHANG_DOT;
          }) == 12);
}

void ChannelsAndStyles()
{
    CHECK(Run("/cast gong").hits.empty());
    CHECK(Frames(Run("/scast biangong"), mw::E_BIAN_GONG) == std::vector<tick_t>({ 28 }));
    CHECK(Frames(Run("/scast bianzhi"), mw::E_BIAN_ZHI) == std::vector<tick_t>({ 16, 32, 48 }));
    CHECK(Frames(Run("/scast bianzhi\n/sfcast yu"), mw::E_BIAN_ZHI).empty());
    CHECK(Run("/scast bianzhi\n/sfcast [buff:qufeng=1] yu").hits.empty());
    // A force-cast at frame 24 removes the remaining two segments.
    Harness h;
    CHECK(h.Cast(mw::BIAN_ZHI));
    h.Advance(24);
    CHECK(h.Cast(mw::YU, Command::FORCE_CAST));
    h.Advance(64);
    CHECK(std::count_if(h.log.Intents().begin(), h.log.Intents().end(), [](auto &x) {
              return x.effect == mw::E_BIAN_ZHI;
          }) == 1);
    CHECK(Frames(Run("/scast bianzhi\n/scast yu"), mw::E_YU) == std::vector<tick_t>({ 48 }));
    mw::Config c;
    c.talents = mw::HAO_QING;
    CHECK(Frames(Run("/scast bianzhi", c), mw::E_BIAN_ZHI) == std::vector<tick_t>({ 0, 16, 32, 48 }));
    const auto switch_result = Run("/scast biangong\n/scast yangchun\n/scast gong");
    CHECK(Frames(switch_result, mw::E_GONG) == std::vector<tick_t>({ 62 }));
    c                 = {};
    c.initial_style   = mw::Style::YANG_CHUN_BAI_XUE;
    const auto ACTIVE = Run("/scast gong\n/scast yangchun", c);
    CHECK(Frames(ACTIVE, mw::E_YANG_CHUN) == std::vector<tick_t>({ 0 }));
    CHECK(Frames(ACTIVE, mw::E_GONG) == std::vector<tick_t>({ 28 }));
    CHECK(Run("/scast yangchun", c).hits.empty());
    CHECK(Frames(Run("/scast zhi\n/scast yangchun", c), mw::E_ZHI) == std::vector<tick_t>({ 16, 32, 48 }));
    CHECK(Run("/scast zhi\n/sfcast yangchun", c).hits.empty());
    CHECK(Frames(Run("/scast biangong\n/sfcast yu"), mw::E_BIAN_GONG) == std::vector<tick_t>({ 28 }));
}

void ChargesAndGuYing()
{
    auto r = Run("/scast yu\n/scast yu\n/scast yu\n/scast yu", {}, 140);
    CHECK(Frames(r, mw::E_YU) == std::vector<tick_t>({ 0, 24, 48, 96 }));
    CHECK(r.state.skill_energy[mw::YU] == 0);
    mw::Config c;
    c.talents = mw::MING_JIN_TALENT;
    CHECK(Frames(Run("/scast yu\n/scast yu\n/scast yu\n/scast yu", c, 200), mw::E_YU) == std::vector<tick_t>({ 0, 24, 48, 176 }));
    c.talents = mw::LIU_ZHAO_TALENT | mw::SHI_XIANG;
    Harness h(c);
    CHECK(h.Cast(mw::BIAN_ZHI));
    h.Advance(48);
    CHECK(h.Cast(mw::YU));
    h.Advance(48);
    CHECK(h.state.skill_energy[mw::ZHI] == 3 && h.state.skill_energy[mw::BIAN_ZHI] == 3);
    Harness guying;
    CHECK(guying.Cast(mw::SHU_YING));
    CHECK(guying.Cast(mw::GU_YING));
    guying.Advance(24);
    CHECK(guying.Cast(mw::SHU_YING));
    guying.Advance(48);
    CHECK(guying.Cast(mw::GU_YING));
    CHECK(guying.state.skill_energy[mw::SHU_YING] == 2);
    guying.Advance(367);
    CHECK(guying.state.skill_energy[mw::SHU_YING] == 2);
    guying.Advance(368);
    CHECK(guying.state.skill_energy[mw::SHU_YING] == 3);
    CHECK(!guying.Cast(mw::GU_YING));
    Harness expiry;
    CHECK(expiry.Cast(mw::GU_YING));
    expiry.Advance(112);
    CHECK(expiry.state.self_buff_stacks[mw::GU_YING_BUFF] == 0);
    CHECK(expiry.state.SkillReadyAt(mw::GU_YING) == 2512);
}

void ShadowsAndQuFeng()
{
    mw::Config c;
    c.talents = mw::KE_MENG | mw::YUN_HAN_TALENT;
    Harness<AlwaysProc> h(c);
    CHECK(h.Cast(mw::SHU_YING));
    h.Advance(24);
    CHECK(h.Cast(mw::SHU_YING));
    h.Advance(48);
    CHECK(h.Cast(mw::SHU_YING));
    CHECK(h.rules.ShadowCount() == 6);
    h.Advance(400);
    CHECK(h.rules.ShadowCount() == 4);
    h.Advance(424);
    CHECK(h.rules.ShadowCount() == 2);
    h.Advance(448);
    CHECK(h.rules.ShadowCount() == 0);
    CHECK(h.state.self_buff_stacks[mw::YUN_HAN] == 0);
    CHECK(h.log.Size() == 3 + 72);
    Harness fifo;
    CHECK(fifo.Cast(mw::SHU_YING));
    fifo.Advance(24);
    CHECK(fifo.Cast(mw::SHU_YING));
    CHECK(fifo.Cast(mw::YI_XING));
    CHECK(fifo.rules.ShadowCount() == 1);
    CHECK(fifo.state.self_buff_expires_at[mw::YING_ZI] == 424);
    c                = {};
    c.initial_style  = mw::Style::YANG_CHUN_BAI_XUE;
    const auto exact = Run("/scast shang\n/scast gong\n/scast yu", c);
    CHECK(Frames(exact, mw::E_SURPLUS0) == std::vector<tick_t>({ 52 }));
    CHECK(Frames(exact, mw::E_SURPLUS1) == std::vector<tick_t>({ 52 }));
    CHECK(Frames(exact, mw::E_SURPLUS2) == std::vector<tick_t>({ 52 }));
    CHECK(Frames(Run("/scast biangong\n/scast bianzhi"), mw::E_SURPLUS0).empty());
}

void TalentAndEquipment()
{
    mw::Config c;
    c.talents = mw::XIAN_FENG_TALENT | mw::LIU_ZHAO_TALENT | mw::ZHENG_MING | mw::ZHI_ZHI;
    auto r    = Run("/scast shang\n/scast jue\n/scast yangchun\n/scast gaoshan\n/scast bianzhi", c, 500);
    CHECK(Frames(r, mw::E_LIU_ZHAO).size() == 6);
    CHECK(Frames(r, mw::E_XIAN_FENG).size() == 9);
    CHECK(r.state.self_buff_stacks[mw::XIAN_FENG] == 0);
    CHECK(Frames(r, mw::E_SURPLUS3).empty());
    c.initial_style      = mw::Style::YANG_CHUN_BAI_XUE;
    const auto ZHENGMING = Run("/scast shang\n/scast gong\n/scast yu", c);
    CHECK(Frames(ZHENGMING, mw::E_SURPLUS3) == std::vector<tick_t>({ 52 }));
    c             = {};
    c.equipment   = mw::WEAPON_CW | mw::SET_ATTRIBUTE;
    const auto CW = Run<AlwaysProc>("/cast biangong", c, 700);
    CHECK(!Frames(CW, mw::E_BIAN_GONG_CW).empty());
    CHECK(Frames(CW, mw::E_BIAN_GONG_CW).front() >= 480);
    CHECK(!Frames(CW, mw::E_SHEN_BING).empty() || !Frames(CW, mw::E_SHEN_BING3).empty());
    CHECK(Frames(CW, mw::E_SURPLUS0).size() == Frames(CW, mw::E_BIAN_GONG_CW).size());
    c         = {};
    c.talents = mw::ZHENG_LV_HE_MING;
    auto z    = Run("/scast zhenglv1", c, 600);
    CHECK(Frames(z, mw::E_ZHI_YIN3) == std::vector<tick_t>({ 320, 320, 320, 320, 320 }));
    CHECK(z.state.self_buff_stacks[mw::ZHI_YIN_MIAO_YI] == 0);
}

void EnchantsAndSnapshots()
{
    mw::Config c;
    c.equipment  = mw::ENCHANT_SHOES | mw::ENCHANT_WRIST | mw::ENCHANT_BELT | mw::WEAPON_WATER;
    const auto P = mw::Prepare(c);
    CHECK(P->damage.size() == mw::ProfileCount * 11 * 3);
    CHECK(mw::Prepare()->damage.size() == mw::ProfileCount);
    for (unsigned water = 0; water <= 10; ++water) {
        for (unsigned belt = 0; belt < 3; ++belt) {
            const unsigned SNAP = (mw::WaterUnit * water) + (mw::BeltUnit * belt) + mw::SetProfile;
            for (unsigned effect = 0; effect < mw::E_COUNT; ++effect) {
                for (const bool CRITICAL : { false, true }) {
                    DamageIntent hit;
                    hit.effect   = effect;
                    hit.snapshot = SNAP;
                    hit.outcome  = CRITICAL ? RollResult::DOUBLE : RollResult::HIT;
                    CHECK(P->Reduce(hit) == mw::CalculateDamage(P->formulas[effect], P->Input(effect, SNAP), c.target, CRITICAL));
                }
            }
        }
    }
    const auto R = Run<AlwaysProc>("/cast wuyin", c, 520);
    CHECK(Frames(R, mw::E_SHOES) == std::vector<tick_t>({ 0, 160, 320, 480 }));
    CHECK(Frames(R, mw::E_WRIST) == std::vector<tick_t>({ 0, 160, 320, 480 }));
    CHECK(R.hits[0].effect == mw::E_WRIST && R.hits[0].snapshot == 0);
    CHECK(R.hits[1].effect == mw::E_SHOES && R.hits[1].snapshot == mw::WaterUnit + 2 * mw::BeltUnit);
    CHECK(R.hits[2].effect == mw::E_WU_YIN && R.hits[2].snapshot == R.hits[1].snapshot);
    CHECK(R.state.self_buff_stacks[mw::WATER_BUFF] == 10);
    CHECK(R.state.self_buff_stacks[mw::BELT_BUFF] == 1);
    const auto EXPIRED = Run<AlwaysProc>("/scast shang", c, 300);
    CHECK(EXPIRED.state.self_buff_stacks[mw::WATER_BUFF] == 0 && EXPIRED.state.self_buff_stacks[mw::BELT_BUFF] == 0);
    for (const auto &hit : EXPIRED.hits) {
        if (hit.effect == mw::E_SHANG_DOT) {
            CHECK(hit.snapshot == mw::WaterUnit + 2 * mw::BeltUnit);
        }
    }
    const auto REFRESHED = Run<AlwaysProc>("/scast shang\n/scast jue\n/scast shang", c, 350);
    for (const auto &hit : REFRESHED.hits) {
        if (hit.effect == mw::E_SHANG_DOT) {
            CHECK((hit.snapshot & mw::WaterMask) == mw::WaterUnit * (hit.frame == 48 ? 1 : 3));
        }
    }
    CHECK(Frames(Run<HitsOnly>("/cast wuyin", c), mw::E_SHOES).empty());
    CHECK(Frames(Run<HitsOnly>("/cast wuyin", c), mw::E_WRIST).empty());
    Rejects([&] { P->ValidateCapacity(std::numeric_limits<std::size_t>::max()); });
}

constexpr std::string_view Rotation =
    "/cast [tnobuff:shang] shang\n/cast [tnobuff:jue] jue\n"
    "/cast shuying\n/cast gaoshan\n/cast [buff:qufeng=4] yu\n/cast bianzhi\n/cast biangong";

void ParityAndAllocation()
{
    mw::Config c;
    c.talents = mw::HAO_ZHONG | mw::FEI_FAN | mw::XIAN_FENG_TALENT | mw::LIU_ZHAO_TALENT | mw::HAO_QING |
                mw::SHI_XIANG | mw::ZHI_ZHI | mw::KE_MENG | mw::ZHENG_MING | mw::MING_JIN_TALENT | mw::YUN_HAN_TALENT |
                mw::CAN_LIAN_TALENT | mw::ZHENG_LV_HE_MING;
    c.equipment    = mw::WEAPON_CW | mw::SET_ATTRIBUTE | mw::SET_SKILL | mw::WEAPON_WATER | mw::ENCHANT_BELT |
                     mw::ENCHANT_SHOES | mw::ENCHANT_WRIST;
    c.team_effects = mw::YOU_REN | mw::JING_MIAO;
    const auto                  MACRO = Compile(std::string("/cast zhenglv1\n") + std::string(Rotation));
    const auto                  P     = mw::Prepare(c);
    Simulation<mw::Rules>       fast(MACRO.program, mw::Rules(P), 40000, 100000);
    Simulation<mw::Rules, true> debug(MACRO.program, mw::Rules(P), 40000, 100000);
    for (std::uint64_t seed = 0; seed < 16; ++seed) {
        counting = true;
        fast.Start(4800, seed);
        fast.Run();
        const auto TOTAL = fast.TotalDamage();
        counting         = false;
        CHECK(allocations == 0);
        debug.Start(4800, seed);
        while (!debug.Finished()) {
            debug.Continue();
        }
        CHECK(TOTAL == debug.TotalDamage());
        CHECK(fast.Log().Intents() == debug.Log().Intents());
        CHECK(fast.Log().Mutations() == debug.Log().Mutations());
        CHECK(fast.GetRules().Shadows() == debug.GetRules().Shadows());
    }
    debug.Start(160, 2);
    debug.SetBreakpoint(1);
    debug.Continue();
    CHECK(debug.DebugControl().paused);
    BatchOptions options;
    options.iterations        = 32;
    options.duration          = 4800;
    options.damage_capacity   = 40000;
    options.mutation_capacity = 100000;
    const auto SINGLE         = RunBatch(MACRO.program, options, mw::Rules(P));
    options.workers           = 4;
    const auto MULTI          = RunBatch(MACRO.program, options, mw::Rules(P));
    CHECK(SINGLE.checksum == MULTI.checksum);
    CHECK(SINGLE.min_damage == MULTI.min_damage && SINGLE.max_damage == MULTI.max_damage);
    const auto                  FORCED = Compile("/scast bianzhi\n/sfcast [buff:qufeng=1] yu");
    Simulation<mw::Rules>       force_fast(FORCED.program, mw::Rules{});
    Simulation<mw::Rules, true> force_debug(FORCED.program, mw::Rules{});
    force_fast.Start(100, 9);
    force_fast.Run();
    force_debug.Start(100, 9);
    force_debug.SetBreakpoint(1);
    force_debug.Continue();
    CHECK(force_debug.DebugControl().paused);
    CHECK(force_debug.GetState().casting_skill == mw::BIAN_ZHI);
    force_debug.StepInto();
    CHECK(force_debug.GetState().casting_skill == INVALID_SLOT);
    CHECK(force_debug.DebugControl().action.interrupted == mw::BIAN_ZHI);
    force_debug.SetBreakpoint(1, false);
    force_debug.Continue();
    CHECK(force_fast.Log().Intents() == force_debug.Log().Intents());
    CHECK(force_fast.Log().Mutations() == force_debug.Log().Mutations());
}

int main()
{
    NamesAndValidation();
    DamageArithmetic();
    DotTiming();
    ChannelsAndStyles();
    ChargesAndGuYing();
    ShadowsAndQuFeng();
    TalentAndEquipment();
    EnchantsAndSnapshots();
    ParityAndAllocation();
    std::cout << "Mo Wen: arithmetic, timers, mechanics, fast/debug parity and zero hot allocations passed.\n";
}
