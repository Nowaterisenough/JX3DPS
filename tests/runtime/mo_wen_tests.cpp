#include "src/class/mo_wen/runtime_rules.hpp"
#include "src/core/runtime/batch.hpp"
#include <cstdlib>
#include <iostream>
#include <new>
#include <random>

#define JX3DPS LegacyReference
#include "legacy_reference/Damage.hpp"
#undef JX3DPS

using namespace JX3DPS;
using namespace JX3DPS::runtime;
namespace mw=JX3DPS::runtime::mo_wen;
#define CHECK(x) do { if (!(x)) { std::cerr<<__LINE__<<": " #x "\n"; std::exit(1); } } while (false)

static bool counting=false;
static std::size_t allocations=0;
void *operator new(std::size_t n) {
    if (counting) ++allocations;
    if (auto p=std::malloc(n?n:1)) return p;
    throw std::bad_alloc();
}
void *operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void *p)noexcept { std::free(p); }
void operator delete[](void *p)noexcept { std::free(p); }
void operator delete(void *p,std::size_t)noexcept { std::free(p); }
void operator delete[](void *p,std::size_t)noexcept { std::free(p); }
template<typename F> void Rejects(F f) {
    try { f(); } catch (const std::invalid_argument &) { return; }
    CHECK(false);
}
struct HitsOnly {
    void Reset(std::uint64_t) {}
    std::uint64_t Next() { return ~std::uint64_t{0}; }
    int Integer(int,int high) { return high; }
};
struct AlwaysProc {
    void Reset(std::uint64_t) {}
    std::uint64_t Next() { return 0; }
    int Integer(int low,int) { return low; }
};
CompiledMacro Compile(std::string_view text,bool debug=true) {
    auto [macro,errors]=MacroCompiler::Compile(text,mw::MacroOptions(debug));
    for (const auto &e:errors) std::cerr<<e.message<<'\n';
    CHECK(errors.empty());
    return macro;
}
struct Result {
    State state;
    std::vector<DamageIntent> hits;
    std::vector<StateMutation> mutations;
    value_t damage=0;
    mw::Style style{};
    std::array<mw::Shadow,6> shadows{};
};
template<typename Rng=HitsOnly>
Result Run(std::string_view text,mw::Config config={},tick_t duration=640) {
    auto macro=Compile(text);
    using Rules=mw::BasicRules<Rng>;
    Simulation<Rules> sim(macro.program,Rules(mw::Prepare(config)),20000,50000);
    sim.Start(duration,7); sim.Run();
    return {sim.GetState(),sim.Log().Intents(),sim.Log().Mutations(),sim.TotalDamage(),
        sim.GetRules().CurrentStyle(),sim.GetRules().Shadows()};
}
std::vector<tick_t> Frames(const Result &r,mw::Effect effect) {
    std::vector<tick_t> frames;
    for (const auto &hit:r.hits) if (hit.effect==effect) frames.push_back(hit.frame);
    return frames;
}
template<typename Rng=HitsOnly> struct Harness {
    static constexpr bool debug_enabled=false;
    State state;
    DeadlineQueue queue;
    DamageIntentLog log{20000,50000};
    mw::BasicRules<Rng> rules;
    explicit Harness(mw::Config c={}):rules(mw::Prepare(c)) {
        state.Resize(mw::SkillCount,mw::BuffCount); rules.Setup(*this); rules.Reset(*this,7);
    }
    State &MutableState() { return state; }
    const State &GetState()const { return state; }
    DeadlineQueue &Timers() { return queue; }
    DamageIntentLog &Log() { return log; }
    void Advance(tick_t target) {
        CHECK(target>=state.now);
        while (auto d=queue.NextDelta()) {
            if (*d>target-state.now) break;
            queue.Advance(*d); state.now=queue.Now();
            queue.DispatchDue([this](auto h,auto id){rules.OnTimer(*this,h,id);});
        }
        queue.Advance(target-state.now); state.now=target;
    }
    bool Cast(mw::SkillSlot skill,Command command=Command::Cast) { return rules.Cast(*this,{0,0,skill,command}); }
};

void NamesAndValidation() {
    const auto options=mw::MacroOptions();
    for (Slot b=0; b<mw::BuffCount; ++b) {
        CHECK(options.resolve_buff(mw::Buffs.entries[b].name)==b);
        const auto name=mw::Buffs.entries[b].name;
        const auto bare=Compile("/cast [buff:"+std::string(name)+"] yu");
        const auto explicit_zero=Compile("/cast [buff:"+std::string(name)+">0] yu");
        CHECK(bare.program.conditions.front().comparator==explicit_zero.program.conditions.front().comparator);
    }
    CHECK(options.resolve_skill("pozhao")==INVALID_SLOT);
    CHECK(options.resolve_buff(mw::Skills.entries[mw::Shang].name)==mw::ShangDot);
    for (const auto value:{-1LL,1000001LL}) {
        mw::Config c; c.attributes.attack_base=value; Rejects([&]{mw::Prepare(c);});
    }
    mw::Config c; c.equipment=1u<<7; Rejects([&]{mw::Prepare(c);});
    c={}; c.initial_yingzi=7; Rejects([&]{mw::Prepare(c);});
    c={}; c.recipes[0]=mw::ShangCrit2; Rejects([&]{mw::Prepare(c);});
    c={}; c.target.distance=std::numeric_limits<double>::quiet_NaN(); Rejects([&]{mw::Prepare(c);});
    c={}; c.attributes.attack_base=c.attributes.overcome=c.attributes.crit_power=c.attributes.strain=1000000;
    c.attributes.attack_percent=c.attributes.damage_percent=c.attributes.overcome_percent=10000;
    c.attributes.strain_percent=c.attributes.pve_percent=c.target.vulnerable=10000;
    c.target.level=110;
    Rejects([&]{mw::Prepare(c);});
    CHECK(Run("/scast zhenglv1").hits.empty());
    c={}; c.target.distance=21;
    CHECK(Run("/cast biangong",c).hits.empty());
    c.recipes[0]=mw::GongRange;
    CHECK(!Run("/cast biangong",c).hits.empty());
}
void DamageArithmetic() {
    std::mt19937 random(42);
    for (unsigned i=0; i<4000; ++i) {
        mw::DamageInput a{static_cast<value_t>(random()%90000),static_cast<value_t>(random()%40000),
            static_cast<value_t>(random()%60000),static_cast<value_t>(random()%30000),
            static_cast<value_t>(random()%12000),int(random()%2000),int(random()%1000),
            int(random()%1000),int(random()%1000),int(random()%700),int(random()%1100)};
        mw::Target t; t.level=110+random()%21; t.shield=random()%70000; t.vulnerable=random()%500;
        const auto &f=mw::Formulas[i%mw::ECount];
        for (int critical=0; critical<2; ++critical) {
            using namespace LegacyReference;
            Value_t raw=0;
            if (f.kind==mw::FormulaKind::Surplus) raw=EffectDamage(SurplusDamage(a.surplus,f.channel,120),a.damage_percent);
            else if (f.kind!=mw::FormulaKind::Zero) {
                const auto coefficient=f.kind==mw::FormulaKind::Dot
                    ? MagicDotDamageCoefficient(f.channel,f.dot_count,f.dot_interval)
                    : MagicDamageCoefficient(f.channel,0);
                raw=EffectDamageAll(a.attack,coefficient,0,0,f.fixed,a.damage_percent);
            }
            const auto expected=FinalMagicDamage(120,t.level,raw,t.shield,0,a.ignore_shield,
                a.effect_ignore_shield,a.overcome,critical,a.crit_power,a.crit_power_percent,
                a.strain,a.strain_percent,a.pve_percent,t.vulnerable);
            CHECK(mw::CalculateDamage(f,a,t,critical)!=-1);
            CHECK(mw::CalculateDamage(f,a,t,critical)==expected);
        }
    }
    mw::Config c; c.talents=mw::HaoZhong|mw::FeiFan|mw::ShiXiang;
    c.equipment=mw::WeaponCW|mw::SetSkill;
    c.recipes={mw::GongDamage3|mw::GongCrit2,mw::ShangDamage3|mw::ShangDamage4|mw::ShangDamage5,
        mw::ZhiDamage4,mw::YuCrit3|mw::YuDamage3};
    auto p=mw::Prepare(c);
    CHECK(p->formulas[mw::EShang].channel==120);
    CHECK(p->formulas[mw::EShangDot].channel==294);
    CHECK(p->Input(mw::EGong,0).damage_percent==31+51+102);
    CHECK(p->Input(mw::EBianZhi,mw::MingJinProfile).damage_percent==41+51+102+152+205);
    CHECK(p->Input(mw::EYu,0).effect_ignore_shield==614);
    CHECK(std::abs(p->Chance(mw::EGong,0)-(20000/(9.530*8250)+.12))<1e-12);
    CHECK(p->Input(mw::EGong,mw::CanLianProfile).attack==10000*1126/1024+2500);
}
void DotTiming() {
    CHECK(Frames(Run("/scast shang"),mw::EShangDot)==std::vector<tick_t>({48,96,144,192,240,288}));
    CHECK(Frames(Run("/scast jue"),mw::EJueDot)==std::vector<tick_t>({48,96,144,192,240,288}));
    CHECK(Frames(Run("/scast shang\n/scast shang"),mw::EShangDot)==std::vector<tick_t>({48,96,144,192,240,288}));
    CHECK(Frames(Run("/scast gaoshan\n/scast shang"),mw::EShangDot)==std::vector<tick_t>({39,78,117,156,195,234}));
    CHECK(Frames(Run("/scast shang\n/scast gaoshan"),mw::EShangDot)==std::vector<tick_t>({48,96,144,192,240,288}));
    CHECK(Run("/scast shang").state.target_buff_stacks[mw::ShangDot]==0);
    // Refresh exactly at expiry after the final old tick creates a fresh six-tick DOT.
    Harness h; CHECK(h.Cast(mw::Shang)); h.Advance(288); CHECK(h.Cast(mw::Shang));
    h.Advance(576);
    CHECK(std::count_if(h.log.Intents().begin(),h.log.Intents().end(),[](auto &x){return x.effect==mw::EShangDot;})==12);
}
void ChannelsAndStyles() {
    CHECK(Run("/cast gong").hits.empty());
    CHECK(Frames(Run("/scast biangong"),mw::EBianGong)==std::vector<tick_t>({28}));
    CHECK(Frames(Run("/scast bianzhi"),mw::EBianZhi)==std::vector<tick_t>({16,32,48}));
    CHECK(Frames(Run("/scast bianzhi\n/sfcast yu"),mw::EBianZhi)==std::vector<tick_t>{});
    CHECK(Run("/scast bianzhi\n/sfcast [buff:qufeng=1] yu").hits.empty());
    // A force-cast at frame 24 removes the remaining two segments.
    Harness h; CHECK(h.Cast(mw::BianZhi)); h.Advance(24); CHECK(h.Cast(mw::Yu,Command::ForceCast)); h.Advance(64);
    CHECK(std::count_if(h.log.Intents().begin(),h.log.Intents().end(),[](auto &x){return x.effect==mw::EBianZhi;})==1);
    CHECK(Frames(Run("/scast bianzhi\n/scast yu"),mw::EYu)==std::vector<tick_t>({48}));
    mw::Config c; c.talents=mw::HaoQing;
    CHECK(Frames(Run("/scast bianzhi",c),mw::EBianZhi)==std::vector<tick_t>({0,16,32,48}));
    const auto switch_result=Run("/scast biangong\n/scast yangchun\n/scast gong");
    CHECK(Frames(switch_result,mw::EGong)==std::vector<tick_t>({62}));
    c={}; c.initial_style=mw::Style::YangChunBaiXue;
    const auto active=Run("/scast gong\n/scast yangchun",c);
    CHECK(Frames(active,mw::EYangChun)==std::vector<tick_t>({0}));
    CHECK(Frames(active,mw::EGong)==std::vector<tick_t>({28}));
    CHECK(Run("/scast yangchun",c).hits.empty());
    CHECK(Frames(Run("/scast zhi\n/scast yangchun",c),mw::EZhi)==std::vector<tick_t>({16,32,48}));
    CHECK(Run("/scast zhi\n/sfcast yangchun",c).hits.empty());
    CHECK(Frames(Run("/scast biangong\n/sfcast yu"),mw::EBianGong)==std::vector<tick_t>({28}));
}
void ChargesAndGuYing() {
    auto r=Run("/scast yu\n/scast yu\n/scast yu\n/scast yu",{},140);
    CHECK(Frames(r,mw::EYu)==std::vector<tick_t>({0,24,48,96}));
    CHECK(r.state.skill_energy[mw::Yu]==0);
    mw::Config c; c.talents=mw::MingJinTalent;
    CHECK(Frames(Run("/scast yu\n/scast yu\n/scast yu\n/scast yu",c,200),mw::EYu)==std::vector<tick_t>({0,24,48,176}));
    c.talents=mw::LiuZhaoTalent|mw::ShiXiang;
    Harness h(c); CHECK(h.Cast(mw::BianZhi)); h.Advance(48); CHECK(h.Cast(mw::Yu));
    h.Advance(48);
    CHECK(h.state.skill_energy[mw::Zhi]==3 && h.state.skill_energy[mw::BianZhi]==3);
    Harness guying; CHECK(guying.Cast(mw::ShuYing)); CHECK(guying.Cast(mw::GuYing));
    guying.Advance(24); CHECK(guying.Cast(mw::ShuYing));
    guying.Advance(48); CHECK(guying.Cast(mw::GuYing));
    CHECK(guying.state.skill_energy[mw::ShuYing]==2);
    guying.Advance(367); CHECK(guying.state.skill_energy[mw::ShuYing]==2);
    guying.Advance(368); CHECK(guying.state.skill_energy[mw::ShuYing]==3);
    CHECK(!guying.Cast(mw::GuYing));
    Harness expiry; CHECK(expiry.Cast(mw::GuYing)); expiry.Advance(112);
    CHECK(expiry.state.self_buff_stacks[mw::GuYingBuff]==0);
    CHECK(expiry.state.SkillReadyAt(mw::GuYing)==2512);
}
void ShadowsAndQuFeng() {
    mw::Config c; c.talents=mw::KeMeng|mw::YunHanTalent;
    Harness<AlwaysProc> h(c);
    CHECK(h.Cast(mw::ShuYing)); h.Advance(24); CHECK(h.Cast(mw::ShuYing));
    h.Advance(48); CHECK(h.Cast(mw::ShuYing));
    CHECK(h.rules.ShadowCount()==6);
    h.Advance(400); CHECK(h.rules.ShadowCount()==4);
    h.Advance(424); CHECK(h.rules.ShadowCount()==2);
    h.Advance(448); CHECK(h.rules.ShadowCount()==0);
    CHECK(h.state.self_buff_stacks[mw::YunHan]==0);
    CHECK(h.log.Size()==3+72);
    Harness fifo; CHECK(fifo.Cast(mw::ShuYing)); fifo.Advance(24); CHECK(fifo.Cast(mw::ShuYing));
    CHECK(fifo.Cast(mw::YiXing)); CHECK(fifo.rules.ShadowCount()==1);
    CHECK(fifo.state.self_buff_expires_at[mw::YingZi]==424);
    c={}; c.initial_style=mw::Style::YangChunBaiXue;
    const auto exact=Run("/scast shang\n/scast gong\n/scast yu",c);
    CHECK(Frames(exact,mw::ESurplus0)==std::vector<tick_t>({52}));
    CHECK(Frames(exact,mw::ESurplus1)==std::vector<tick_t>({52}));
    CHECK(Frames(exact,mw::ESurplus2)==std::vector<tick_t>({52}));
    CHECK(Frames(Run("/scast biangong\n/scast bianzhi"),mw::ESurplus0).empty());
}
void TalentAndEquipment() {
    mw::Config c; c.talents=mw::XianFengTalent|mw::LiuZhaoTalent|mw::ZhengMing|mw::ZhiZhi;
    auto r=Run("/scast shang\n/scast jue\n/scast yangchun\n/scast gaoshan\n/scast bianzhi",c,500);
    CHECK(Frames(r,mw::ELiuZhao).size()==6);
    CHECK(Frames(r,mw::EXianFeng).size()==9);
    CHECK(r.state.self_buff_stacks[mw::XianFeng]==0);
    CHECK(Frames(r,mw::ESurplus3).empty());
    c.initial_style=mw::Style::YangChunBaiXue;
    const auto zhengming=Run("/scast shang\n/scast gong\n/scast yu",c);
    CHECK(Frames(zhengming,mw::ESurplus3)==std::vector<tick_t>({52}));
    c={}; c.equipment=mw::WeaponCW|mw::SetAttribute;
    auto cw=Run<AlwaysProc>("/cast biangong",c,700);
    CHECK(!Frames(cw,mw::EBianGongCw).empty());
    CHECK(Frames(cw,mw::EBianGongCw).front()>=480);
    CHECK(!Frames(cw,mw::EShenBing).empty() || !Frames(cw,mw::EShenBing3).empty());
    CHECK(Frames(cw,mw::ESurplus0).size()==Frames(cw,mw::EBianGongCw).size());
    c={}; c.talents=mw::ZhengLvHeMing;
    auto z=Run("/scast zhenglv1",c,600);
    CHECK(Frames(z,mw::EZhiYin3)==std::vector<tick_t>({320,320,320,320,320}));
    CHECK(z.state.self_buff_stacks[mw::ZhiYinMiaoYi]==0);
}
void EnchantsAndSnapshots() {
    mw::Config c; c.equipment=mw::EnchantShoes|mw::EnchantWrist|mw::EnchantBelt|mw::WeaponWater;
    const auto p=mw::Prepare(c);
    CHECK(p->damage.size()==mw::ProfileCount*11*3);
    CHECK(mw::Prepare()->damage.size()==mw::ProfileCount);
    for (unsigned water=0; water<=10; ++water) for (unsigned belt=0; belt<3; ++belt) {
        const unsigned snap=mw::WaterUnit*water+mw::BeltUnit*belt+mw::SetProfile;
        for (unsigned effect=0; effect<mw::ECount; ++effect) for (bool critical:{false,true}) {
            DamageIntent hit; hit.effect=effect; hit.snapshot=snap;
            hit.outcome=critical?RollResult::DOUBLE:RollResult::HIT;
            CHECK(p->Reduce(hit)==mw::CalculateDamage(p->formulas[effect],p->Input(effect,snap),c.target,critical));
        }
    }
    const auto r=Run<AlwaysProc>("/cast wuyin",c,520);
    CHECK(Frames(r,mw::EShoes)==std::vector<tick_t>({0,160,320,480}));
    CHECK(Frames(r,mw::EWrist)==std::vector<tick_t>({0,160,320,480}));
    CHECK(r.hits[0].effect==mw::EWrist && r.hits[0].snapshot==0);
    CHECK(r.hits[1].effect==mw::EShoes && r.hits[1].snapshot==mw::WaterUnit+2*mw::BeltUnit);
    CHECK(r.hits[2].effect==mw::EWuYin && r.hits[2].snapshot==r.hits[1].snapshot);
    CHECK(r.state.self_buff_stacks[mw::WaterBuff]==10);
    CHECK(r.state.self_buff_stacks[mw::BeltBuff]==1);
    const auto expired=Run<AlwaysProc>("/scast shang",c,300);
    CHECK(expired.state.self_buff_stacks[mw::WaterBuff]==0 && expired.state.self_buff_stacks[mw::BeltBuff]==0);
    for (const auto &hit:expired.hits) if (hit.effect==mw::EShangDot)
        CHECK(hit.snapshot==mw::WaterUnit+2*mw::BeltUnit);
    const auto refreshed=Run<AlwaysProc>("/scast shang\n/scast jue\n/scast shang",c,350);
    for (const auto &hit:refreshed.hits) if (hit.effect==mw::EShangDot)
        CHECK((hit.snapshot&mw::WaterMask)==mw::WaterUnit*(hit.frame==48?1:3));
    CHECK(Frames(Run<HitsOnly>("/cast wuyin",c),mw::EShoes).empty());
    CHECK(Frames(Run<HitsOnly>("/cast wuyin",c),mw::EWrist).empty());
    Rejects([&]{p->ValidateCapacity(std::numeric_limits<std::size_t>::max());});
}
constexpr std::string_view Rotation=
    "/cast [tnobuff:shang] shang\n/cast [tnobuff:jue] jue\n"
    "/cast shuying\n/cast gaoshan\n/cast [buff:qufeng=4] yu\n/cast bianzhi\n/cast biangong";
void ParityAndAllocation() {
    mw::Config c;
    c.talents=mw::HaoZhong|mw::FeiFan|mw::XianFengTalent|mw::LiuZhaoTalent|mw::HaoQing|
        mw::ShiXiang|mw::ZhiZhi|mw::KeMeng|mw::ZhengMing|mw::MingJinTalent|mw::YunHanTalent|mw::CanLianTalent|mw::ZhengLvHeMing;
    c.equipment=mw::WeaponCW|mw::SetAttribute|mw::SetSkill|mw::WeaponWater|mw::EnchantBelt|mw::EnchantShoes|mw::EnchantWrist;
    c.team_effects=mw::YouRen|mw::JingMiao;
    auto macro=Compile(std::string("/cast zhenglv1\n")+std::string(Rotation));
    auto p=mw::Prepare(c);
    Simulation<mw::Rules> fast(macro.program,mw::Rules(p),40000,100000);
    Simulation<mw::Rules,true> debug(macro.program,mw::Rules(p),40000,100000);
    for (std::uint64_t seed=0; seed<16; ++seed) {
        counting=true;
        fast.Start(4800,seed); fast.Run(); const auto total=fast.TotalDamage();
        counting=false;
        CHECK(allocations==0);
        debug.Start(4800,seed);
        while (!debug.Finished()) debug.Continue();
        CHECK(total==debug.TotalDamage());
        CHECK(fast.Log().Intents()==debug.Log().Intents());
        CHECK(fast.Log().Mutations()==debug.Log().Mutations());
        CHECK(fast.GetRules().Shadows()==debug.GetRules().Shadows());
    }
    debug.Start(160,2); debug.SetBreakpoint(1);
    debug.Continue(); CHECK(debug.DebugControl().paused);
    BatchOptions options; options.iterations=32; options.duration=4800;
    options.damage_capacity=40000; options.mutation_capacity=100000;
    auto single=RunBatch(macro.program,options,mw::Rules(p));
    options.workers=4;
    auto multi=RunBatch(macro.program,options,mw::Rules(p));
    CHECK(single.checksum==multi.checksum);
    CHECK(single.min_damage==multi.min_damage && single.max_damage==multi.max_damage);
    auto forced=Compile("/scast bianzhi\n/sfcast [buff:qufeng=1] yu");
    Simulation<mw::Rules> force_fast(forced.program,mw::Rules{});
    Simulation<mw::Rules,true> force_debug(forced.program,mw::Rules{});
    force_fast.Start(100,9); force_fast.Run();
    force_debug.Start(100,9); force_debug.SetBreakpoint(1); force_debug.Continue();
    CHECK(force_debug.DebugControl().paused);
    CHECK(force_debug.GetState().casting_skill==mw::BianZhi);
    force_debug.StepInto();
    CHECK(force_debug.GetState().casting_skill==INVALID_SLOT);
    CHECK(force_debug.DebugControl().action.interrupted==mw::BianZhi);
    force_debug.SetBreakpoint(1,false); force_debug.Continue();
    CHECK(force_fast.Log().Intents()==force_debug.Log().Intents());
    CHECK(force_fast.Log().Mutations()==force_debug.Log().Mutations());
}
int main() {
    NamesAndValidation(); DamageArithmetic(); DotTiming(); ChannelsAndStyles();
    ChargesAndGuYing(); ShadowsAndQuFeng(); TalentAndEquipment(); EnchantsAndSnapshots(); ParityAndAllocation();
    std::cout<<"Mo Wen: arithmetic, timers, mechanics, fast/debug parity and zero hot allocations passed.\n";
}
