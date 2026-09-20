#ifndef JX3DPS_MO_WEN_RUNTIME_DATA_HPP
#define JX3DPS_MO_WEN_RUNTIME_DATA_HPP

#include "src/core/runtime/descriptors.hpp"
#include <bit>
#include <memory>

namespace JX3DPS::runtime::mo_wen {

// Pinned to ba960f7, level 120. Negative IDs are report labels, not game IDs.
enum SkillSlot : Slot {
    PoZhao, Gong, BianGong, Shang, Jue, Zhi, BianZhi, Yu, GaoShan, YangChun,
    ShuYing, GuYing, ZhengLv1, ZhengLv2, ZhengLv3, YiXing, WuYin,
    ShangDamage, JueDamage, XianFengDamage, LiuZhaoDamage, LiuZhaoSurplus,
    ZhengMingDamage, ShadowDamage, GongCwDamage, BianGongCwDamage, YuCwDamage,
    ShenBingDamage, ZhiYinDamage, ShoesDamage, WristDamage, SkillCount
};
enum BuffSlot : Slot {
    ShangDot, JueDot, XianFeng, XianFengMark, YunHan, CanLian, MingJin,
    LiuZhao, QuFeng, GaoShanBuff, YangChunBuff, ZhiYinMiaoYi, GuYingBuff,
    ZhengLvBuff, ZhiYinHeMing, YingZi, SetAttributeBuff, WeaponCwBuff,
    ShenBingGong, YouRenBuff, JingMiaoBuff, ShoesBuff, WristBuff, BeltBuff, WaterBuff, BuffCount
};
inline constexpr std::array<Slot, 16> CastableSkills{
    Gong, BianGong, Shang, Jue, Zhi, BianZhi, Yu, GaoShan, YangChun,
    ShuYing, GuYing, ZhengLv1, ZhengLv2, ZhengLv3, YiXing, WuYin
};
inline constexpr bool IsCastable(Slot s) { return s>=Gong && s<=WuYin; }
inline constexpr bool IsZhi(Slot s) { return s==Zhi || s==BianZhi; }
inline constexpr bool IsStyleSkill(Slot s) { return s==GaoShan || s==YangChun; }
inline constexpr bool IsZhengLv(Slot s) { return s>=ZhengLv1 && s<=ZhengLv3; }
inline constexpr bool UsesGcd(Slot s) { return (s>=Gong && s<=Yu) || IsZhengLv(s); }
inline constexpr bool IsDot(Slot b) { return b==ShangDot || b==JueDot || b==ShenBingGong; }
inline constexpr DescriptorTable<SkillDescriptor, SkillCount> Skills{{{
    {-1000, "\xe7\xa0\xb4\xe6\x8b\x9b", 0, 0, 0},
    {-1001, "\xe5\xae\xab", 0, 24, 28},
    {-1002, "\xe5\x8f\x98\xe5\xae\xab", 0, 24, 28},
    {-1003, "\xe5\x95\x86", 0, 24, 0},
    {-1004, "\xe8\xa7\x92", 0, 24, 0},
    {-1005, "\xe5\xbe\xb5", 0, 24, 16},
    {-1006, "\xe5\x8f\x98\xe5\xbe\xb5", 0, 24, 16},
    {-1007, "\xe7\xbe\xbd", 96, 24, 0},
    {-1008, "\xe9\xab\x98\xe5\xb1\xb1\xe6\xb5\x81\xe6\xb0\xb4", 0, 0, 6},
    {-1009, "\xe9\x98\xb3\xe6\x98\xa5\xe7\x99\xbd\xe9\x9b\xaa", 48, 0, 6},
    {-1010, "\xe7\x96\x8f\xe5\xbd\xb1\xe6\xa8\xaa\xe6\x96\x9c", 320, 0, 0},
    {-1011, "\xe5\xad\xa4\xe5\xbd\xb1\xe5\x8c\x96\xe5\x8f\x8c", 2400, 0, 0},
    {-1012, "\xe6\xad\xa3\xe5\xbe\x8b\xe5\x92\x8c\xe9\xb8\xa3", 960, 24, 0},
    {-1013, "\xe6\xad\xa3\xe5\xbe\x8b\xe5\x92\x8c\xe9\xb8\xa3\xc2\xb7\xe4\xba\x8c", 960, 24, 0},
    {-1014, "\xe6\xad\xa3\xe5\xbe\x8b\xe5\x92\x8c\xe9\xb8\xa3\xc2\xb7\xe4\xb8\x89", 960, 24, 0},
    {-1015, "\xe7\xa7\xbb\xe5\xbd\xa2\xe6\x8d\xa2\xe5\xbd\xb1", 0, 0, 0},
    {-1016, "\xe4\xba\x94\xe9\x9f\xb3\xe5\x85\xad\xe5\xbe\x8b", 16, 0, 0},
    {-1017, "\x44\x6f\x74\xc2\xb7\xe5\x95\x86", 0, 0, 0},
    {-1018, "\x44\x6f\x74\xc2\xb7\xe8\xa7\x92", 0, 0, 0},
    {-1019, "\xe5\xbc\xa6\xe9\xa3\x8e", 0, 0, 0},
    {-1020, "\xe6\xb5\x81\xe7\x85\xa7", 0, 0, 0},
    {-1021, "\xe7\xa0\xb4\xe6\x8b\x9b\xc2\xb7\xe6\xb5\x81\xe7\x85\xa7", 0, 0, 0},
    {-1022, "\xe4\xba\x89\xe9\xb8\xa3", 0, 0, 0},
    {-1023, "\xe5\xbd\xb1\xe5\xad\x90", 0, 0, 0},
    {-1024, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe5\xae\xab", 0, 0, 0},
    {-1025, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe5\x8f\x98\xe5\xae\xab", 0, 0, 0},
    {-1026, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe7\xbe\xbd", 0, 0, 0},
    {-1027, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe5\xae\xab\xc2\xb7\x44\x4f\x54", 0, 0, 0},
    {-1028, "\xe7\x9f\xa5\xe9\x9f\xb3\xe8\xa1\x8c\xe5\xb0\xbd", 0, 0, 0},
    {-1029, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe9\x9e\x8b", 0, 0, 0},
    {-1030, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe8\x85\x95", 0, 0, 0}
}}};
inline constexpr DescriptorTable<BuffDescriptor, BuffCount> Buffs{{{
    {-2000, "\x44\x6f\x74\xc2\xb7\xe5\x95\x86", 288, 48, 1},
    {-2001, "\x44\x6f\x74\xc2\xb7\xe8\xa7\x92", 288, 48, 1},
    {-2002, "\xe5\xbc\xa6\xe9\xa3\x8e", 160, 0, 5},
    {-2003, "\xe5\xbc\xa6\xe9\xa3\x8e\xe6\xa0\x87\xe8\xae\xb0", 53, 0, 1},
    {-2004, "\xe4\xba\x91\xe6\xb1\x89", 1600, 0, 9},
    {-2005, "\xe5\x8f\x82\xe8\xbf\x9e", 0, 0, 1},
    {-2006, "\xe4\xb9\xa6\xe7\xa6\xbb", 128, 0, 1},
    {-2007, "\xe6\xb5\x81\xe7\x85\xa7", 400, 0, 32},
    {-2008, "\xe6\x9b\xb2\xe9\xa3\x8e", 0, 0, 9},
    {-2009, "\xe9\xab\x98\xe5\xb1\xb1\xe6\xb5\x81\xe6\xb0\xb4", 192, 0, 1},
    {-2010, "\xe9\x98\xb3\xe6\x98\xa5\xe7\x99\xbd\xe9\x9b\xaa", 96, 0, 3},
    {-2011, "\xe7\x9f\xa5\xe9\x9f\xb3\xe5\xa6\x99\xe6\x84\x8f", 240, 0, 3},
    {-2012, "\xe5\xad\xa4\xe5\xbd\xb1\xe5\x8c\x96\xe5\x8f\x8c", 112, 0, 1},
    {-2013, "\xe6\xad\xa3\xe5\xbe\x8b\xe5\x92\x8c\xe9\xb8\xa3", 320, 0, 1},
    {-2014, "\xe7\x9f\xa5\xe9\x9f\xb3\xe5\x92\x8c\xe9\xb8\xa3", 320, 0, 5},
    {-2015, "\xe5\xbd\xb1\xe5\xad\x90", 400, 32, 6},
    {-2016, "\xe5\xa5\x97\xe8\xa3\x85\xc2\xb7\xe6\x8c\xa5\xe6\x95\xa3", 96, 0, 1},
    {-2017, "\xe6\xad\xa6\xe5\x99\xa8\xc2\xb7\xe6\xa9\x99\xe6\xad\xa6\xe7\x89\xb9\xe6\x95\x88", 96, 0, 1},
    {-2018, "\xe7\xa5\x9e\xe5\x85\xb5\xc2\xb7\xe5\xae\xab", 480, 48, 3},
    {-2019, "\xe6\xb8\xb8\xe5\x88\x83", 80, 0, 1},
    {-2020, "\xe7\xb2\xbe\xe5\xa6\x99", 80, 0, 1},
    {-2021, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe9\x9e\x8b", 0, 0, 1},
    {-2022, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe8\x85\x95", 0, 0, 1},
    {-2023, "\xe5\xa4\xa7\xe9\x99\x84\xe9\xad\x94\xc2\xb7\xe8\x85\xb0", 128, 0, 1},
    {-2024, "\xe6\xb0\xb4\xc2\xb7\xe6\x96\xa9\xe6\xb5\x81", 96, 0, 10}
}}};
inline constexpr std::array<std::string_view, SkillCount> SkillAliases{
    "pozhao", "gong", "biangong", "shang", "jue", "zhi", "bianzhi", "yu",
    "gaoshan", "yangchun", "shuying", "guying", "zhenglv1", "zhenglv2", "zhenglv3",
    "yixing", "wuyin", "shang_dot", "jue_dot", "xianfeng_damage", "liuzhao_damage",
    "liuzhao_surplus", "zhengming", "shadow_damage", "gong_cw", "biangong_cw",
    "yu_cw", "shenbing_dot", "zhiyin_damage", "shoes_damage", "wrist_damage"
};
inline constexpr std::array<std::string_view, BuffCount> BuffAliases{
    "shang", "jue", "xianfeng", "xianfeng_mark", "yunhan", "canlian", "mingjin",
    "liuzhao", "qufeng", "gaoshan", "yangchun", "zhiyinmiaoyi", "guying",
    "zhenglv", "zhiyinheming", "yingzi", "set_attribute", "weapon_cw",
    "shenbing_gong", "you_ren", "jingmiao", "shoes", "wrist", "belt", "weapon_water"
};
static_assert(Skills.Valid() && Buffs.Valid());
inline CompileOptions MacroOptions(bool debug=true) {
    CompileOptions o;
    o.skill_count=SkillCount; o.buff_count=BuffCount; o.keep_debug_info=debug;
    o.resolve_skill=[](std::string_view name) {
        for (Slot s : CastableSkills)
            if (name==Skills.entries[s].name || name==SkillAliases[s]) return s;
        return INVALID_SLOT;
    };
    o.resolve_buff=[](std::string_view name) {
        for (Slot b=0; b<BuffCount; ++b)
            if (name==Buffs.entries[b].name || name==BuffAliases[b]) return b;
        if (name==Skills.entries[Shang].name) return Slot{ShangDot};
        if (name==Skills.entries[Jue].name) return Slot{JueDot};
        return INVALID_SLOT;
    };
    return o;
}
enum Talent : unsigned {
    HaoZhong = 1u << 0, FeiFan = 1u << 1, ChangQing = 1u << 2, XianFengTalent = 1u << 3,
    LiuZhaoTalent = 1u << 4, HaoQing = 1u << 5, ShiXiang = 1u << 6, ZhiZhi = 1u << 7,
    KeMeng = 1u << 8, ZhengMing = 1u << 9, MingJinTalent = 1u << 10, XiuQi = 1u << 11,
    YunHanTalent = 1u << 12, CanLianTalent = 1u << 13, ZhengLvHeMing = 1u << 14
};
enum Recipe : unsigned { GongCrit2=1u<<0, GongCrit3=1u<<1, GongCrit4=1u<<2, GongDamage3=1u<<3, GongDamage4=1u<<4, GongPrepare1=1u<<5, GongPrepare2=1u<<6, GongRange=1u<<7,
    ShangCrit2=1u<<8, ShangCrit3=1u<<9, ShangCrit4=1u<<10, ShangDamage3=1u<<11, ShangDamage4=1u<<12, ShangDamage5=1u<<13,
    ZhiCrit2=1u<<14, ZhiCrit3=1u<<15, ZhiCrit4=1u<<16, ZhiDamage3=1u<<17, ZhiDamage4=1u<<18, ZhiRange=1u<<19,
    YuCrit3=1u<<20, YuCrit4=1u<<21, YuDamage3=1u<<22, YuDamage4=1u<<23 };
enum EquipmentEffect : unsigned { EnchantShoes=1u<<0, EnchantBelt=1u<<1, EnchantWrist=1u<<2, SetAttribute=1u<<3, WeaponCW=1u<<4, WeaponWater=1u<<5, SetSkill=1u<<6 };
enum TeamEffect : unsigned { YouRen=1u<<0, JingMiao=1u<<1 };
enum class Style : std::uint8_t { YangChunBaiXue, GaoShanLiuShui };

struct Attributes {
    value_t attack_base=10000, attack_by_class=2500, weapon=2000, crit=20000, crit_power=15000, overcome=25000, strain=10000, surplus=3000;
    value_t haste=0; int attack_percent=0, overcome_percent=0, haste_percent=0, crit_basis_points=0, crit_power_percent=0, damage_percent=0, strain_percent=0, pve_percent=0, ignore_shield=0;
};
struct Target { int level=124; value_t shield=30000; int vulnerable=0; double life=1.0, distance=4.0; };
struct Config {
    Attributes attributes; Target target; unsigned talents=0, equipment=0, team_effects=0; std::array<unsigned,4> recipes{};
    Style initial_style=Style::GaoShanLiuShui; int initial_yingzi=0; tick_t delay=0;
};


enum Effect : std::uint16_t {
    EGong, EBianGong, EShang, EJue, EZhi, EBianZhi, EYu, EYangChun, EYangChunExecute,
    EWuYin, ESurplus0, ESurplus1, ESurplus2, ESurplus3, EShangDot, EJueDot,
    EYingZi, EXianFeng, ELiuZhao, ELiuZhaoSurplus, EGongCw, EBianGongCw, EYuCw,
    EShenBing, EShenBing2, EShenBing3, EZhiYin, EZhiYin2, EZhiYin3,
    EZeroGaoShan, EZeroYangChun, EZeroShuYing, EZeroGuYing,
    EZeroZhengLv1, EZeroZhengLv2, EZeroZhengLv3, EShoes, EWrist, ECount
};
enum class FormulaKind : std::uint8_t { Direct, Dot, Surplus, Zero };
struct Formula {
    Slot skill=INVALID_SLOT;
    int fixed=0, channel=0;
    FormulaKind kind=FormulaKind::Zero;
    int dot_count=6, dot_interval=48;
    std::uint16_t sub=0, level=0;
};
inline constexpr auto Formulas=[] {
    std::array<Formula,ECount> f{};
    f[EGong]={Gong,180,277,FormulaKind::Direct};
    f[EBianGong]={BianGong,180,277,FormulaKind::Direct};
    f[EShang]={Shang,16,108,FormulaKind::Direct};
    f[EJue]={Jue,16,108,FormulaKind::Direct};
    f[EZhi]={Zhi,200,124,FormulaKind::Direct};
    f[EBianZhi]={BianZhi,200,124,FormulaKind::Direct};
    f[EYu]={Yu,149,208,FormulaKind::Direct};
    f[EYangChun]={YangChun,80,89,FormulaKind::Direct};
    f[EYangChunExecute]=f[EYangChun]; f[EYangChunExecute].level=1;
    f[EWuYin]={WuYin,0,4,FormulaKind::Direct};
    constexpr std::array ratios{0.25*0.5*1.3*1.2*0.5*1.11, 0.30*0.5*1.3*1.2*0.5*1.11,
        0.36*0.5*1.3*1.2*0.5*1.11, 0.4*0.9*0.7};
    for (unsigned i=0; i<4; ++i)
        f[ESurplus0+i]={i==3?ZhengMingDamage:PoZhao,0,static_cast<int>(1024*1024*(ratios[i]-1)),
            FormulaKind::Surplus,6,48,0,static_cast<std::uint16_t>(i)};
    f[EShangDot]={ShangDamage,19,263,FormulaKind::Dot};
    f[EJueDot]={JueDamage,19,257,FormulaKind::Dot};
    f[EYingZi]={ShadowDamage,180,61,FormulaKind::Direct,6,48,1};
    f[EXianFeng]={XianFengDamage,41,48,FormulaKind::Direct};
    f[ELiuZhao]={LiuZhaoDamage,120,static_cast<int>(61*1.25),FormulaKind::Direct};
    f[ELiuZhaoSurplus]={LiuZhaoSurplus,0,
        static_cast<int>(1024*1024*(0.125*0.5*1.3*1.2*0.5*1.11-1)),FormulaKind::Surplus};
    f[EGongCw]={GongCwDamage,0,277,FormulaKind::Direct,6,48,1};
    f[EBianGongCw]={BianGongCwDamage,0,277,FormulaKind::Direct,6,48,1};
    f[EYuCw]={YuCwDamage,21,50,FormulaKind::Direct,6,48,1};
    for (unsigned i=0; i<3; ++i) {
        f[EShenBing+i]={ShenBingDamage,58,618*static_cast<int>(i+1),FormulaKind::Dot,10,48,0,static_cast<std::uint16_t>(i)};
        f[EZhiYin+i]={ZhiYinDamage,43,std::array{1786,1453,775}[i],FormulaKind::Direct,6,48,0,static_cast<std::uint16_t>(i)};
    }
    for (unsigned i=0; i<7; ++i)
        f[EZeroGaoShan+i].skill=std::array<Slot,7>{GaoShan,YangChun,ShuYing,GuYing,ZhengLv1,ZhengLv2,ZhengLv3}[i];
    f[EShoes]={ShoesDamage,48,100,FormulaKind::Direct};
    f[EWrist]={WristDamage,48,90,FormulaKind::Direct};
    return f;
}();

// This bounded aura profile changes only on buff transitions.
inline constexpr unsigned YunHanMask=7, CanLianProfile=8, MingJinProfile=16;
inline constexpr unsigned YangChunUnit=32, YangChunMask=96, SetProfile=128, YouRenProfile=256;
inline constexpr unsigned ProfileCount=256;
inline constexpr unsigned WaterUnit=512, WaterMask=15*WaterUnit, BeltUnit=8192, BeltMask=3*BeltUnit;
inline constexpr unsigned ChanceProfile(unsigned p) {
    return ((p&SetProfile)?1:0) | ((p&YouRenProfile)?2:0);
}
struct DamageInput {
    value_t attack, crit_power, overcome, strain, surplus;
    int damage_percent, crit_power_percent, strain_percent, pve_percent;
    int ignore_shield, effect_ignore_shield;
};
inline value_t BeforeCritical(const Formula &f,const DamageInput &a,const Target &t) {
    if (f.kind==FormulaKind::Zero) return 0;
    value_t raw;
    if (f.kind==FormulaKind::Surplus)
        raw=static_cast<value_t>(a.surplus*((f.channel+(f.channel<0))/1024+1024)/1024*13.192);
    else {
        const double coefficient=f.kind==FormulaKind::Dot
            ? f.channel*std::max(16,f.dot_count*f.dot_interval/12)*1.0/f.dot_count/12/16/16
            : std::max(f.channel,16)*1.0/12/16;
        raw=f.fixed+static_cast<value_t>(a.attack*coefficient);
    }
    raw=raw*(1024+a.damage_percent)/1024;
    auto shield=t.shield-t.shield*a.ignore_shield/1024;
    shield=std::max<value_t>(0,shield-shield*a.effect_ignore_shield/1024);
    const int overcome=static_cast<int>(a.overcome*1024/(9.530*8250)+1024);
    const int reduction=static_cast<int>(shield*1024/(shield+5.091*(450*t.level-45750)));
    return raw*(overcome-overcome*reduction/1024)/1024;
}
inline value_t AfterCritical(value_t raw,int power,bool critical,int strain,int pve,
                             int vulnerable,double level) {
    if (critical) raw=static_cast<value_t>(raw+raw*.75+raw*power/1024);
    raw=static_cast<value_t>(raw*level);
    raw=raw*strain/1024;
    raw=raw*(1024+pve)/1024;
    return raw*(1024+vulnerable)/1024;
}
inline value_t CalculateDamage(const Formula &f,const DamageInput &a,const Target &t,bool critical) {
    const int power=static_cast<int>(a.crit_power*1024/(3.335*8250))+a.crit_power_percent;
    const int strain=static_cast<int>(a.strain*1024/(9.189*8250))+1024+a.strain_percent;
    const int delta=120-t.level;
    return AfterCritical(BeforeCritical(f,a,t),power,critical,strain,a.pve_percent,t.vulnerable,
        1.0+delta*(delta<0?.05:.15));
}
struct Timing {
    double haste=1;
    tick_t gcd=24, short_gcd=24, gong=28, zhi=16, dot=48, shadow=32, wuyin=16;
};
struct Prepared {
    Config config;
    std::array<Timing,2> timing{};
    tick_t yu_cooldown=96, yangchun_cooldown=48;
    std::array<Formula,ECount> formulas=Formulas;
    std::vector<std::array<std::array<value_t,2>,ECount>> damage;
    std::vector<std::array<value_t,ECount>> before_crit;
    std::array<std::array<int,ECount>,ProfileCount> power{};
    std::array<std::array<double,ECount>,4> critical_chance{};
    int strain=1024;
    double level=1;
    value_t max_damage=0;
    value_t max_before_crit=0;
    int max_power=0;
    unsigned water_profiles=1;

    explicit Prepared(Config input):config(std::move(input)) {
        Validate();
        const auto &a=config.attributes;
        for (unsigned i=0; i<timing.size(); ++i) {
            auto &t=timing[i];
            t.haste=1024.0/(static_cast<int>(a.haste/(11.695*8250)*1024)+a.haste_percent+1024+(i?205:0));
            t.gcd=static_cast<int>(24*t.haste+config.delay);
            t.short_gcd=static_cast<int>(24*t.haste);
            t.gong=static_cast<int>((28-2*std::popcount(config.recipes[0]&(GongPrepare1|GongPrepare2)))*t.haste);
            t.zhi=static_cast<int>(16*t.haste);
            t.dot=static_cast<int>(48*t.haste);
            t.shadow=static_cast<int>(32*t.haste);
            t.wuyin=static_cast<int>(16*t.haste);
            if (std::min({t.gcd,t.short_gcd,t.gong,t.zhi,t.dot,t.shadow,t.wuyin})<1)
                throw std::invalid_argument("haste produces a zero-length Mo Wen timer");
        }
        yu_cooldown=Has(MingJinTalent)?176:96;
        yangchun_cooldown=Has(CanLianTalent)?16:48;
        for (auto effect:{EShang,EShangDot}) {
            auto &channel=formulas[effect].channel;
            if (config.recipes[1]&ShangDamage3) channel=static_cast<int>(channel*1.03);
            if (config.recipes[1]&ShangDamage4) channel=static_cast<int>(channel*1.04);
            if (config.recipes[1]&ShangDamage5) channel=static_cast<int>(channel*1.05);
        }
        strain=static_cast<int>(a.strain*1024/(9.189*8250))+1024+a.strain_percent;
        const int delta=120-config.target.level;
        level=1.0+delta*(delta<0?.05:.15);
        water_profiles=HasEquipment(WeaponWater)?11:1;
        const unsigned profile_count=ProfileCount*water_profiles*(HasEquipment(EnchantBelt)?3:1);
        damage.resize(profile_count); before_crit.resize(profile_count);
        for (unsigned p=0; p<profile_count; ++p)
            for (unsigned e=0; e<ECount; ++e) {
                const unsigned encoded=(p%ProfileCount)+(p/ProfileCount%water_profiles)*WaterUnit+
                    (p/ProfileCount/water_profiles)*BeltUnit;
                const auto in=Input(e,encoded);
                before_crit[p][e]=BeforeCritical(formulas[e],in,config.target);
                max_before_crit=std::max(max_before_crit,before_crit[p][e]);
                if (p<ProfileCount) {
                    power[p][e]=static_cast<int>(in.crit_power*1024/(3.335*8250))+in.crit_power_percent;
                    max_power=std::max(max_power,power[p][e]);
                }
            }
        ValidateCapacity(1);
        for (unsigned p=0; p<profile_count; ++p)
            for (unsigned e=0; e<ECount; ++e)
                for (unsigned crit=0; crit<2; ++crit) {
                    damage[p][e][crit]=AfterCritical(before_crit[p][e],power[p%ProfileCount][e],crit!=0,
                        strain,config.attributes.pve_percent,config.target.vulnerable,level);
                    max_damage=std::max(max_damage,damage[p][e][crit]);
                }
        for (unsigned p=0; p<4; ++p)
            for (unsigned e=0; e<ECount; ++e) critical_chance[p][e]=Chance(e,p);
    }
    bool Has(Talent t)const { return (config.talents&t)!=0; }
    bool HasEquipment(EquipmentEffect e)const { return (config.equipment&e)!=0; }
    bool HasTeam(TeamEffect e)const { return (config.team_effects&e)!=0; }
    DamageInput Input(unsigned effect,unsigned profile)const {
        const auto &a=config.attributes;
        const int attack_bonus=(profile&CanLianProfile)?102:0;
        const int water=static_cast<int>((profile&WaterMask)/WaterUnit);
        const int belt=static_cast<int>((profile&BeltMask)/BeltUnit);
        DamageInput in{(a.attack_base+88*water)*(1024+a.attack_percent+attack_bonus)/1024+a.attack_by_class,
            a.crit_power,a.overcome*(1024+a.overcome_percent)/1024,a.strain,a.surplus,
            a.damage_percent+51*static_cast<int>(profile&YunHanMask)+(belt==2?51:belt==1?10:0),
            a.crit_power_percent+((profile&SetProfile)?41:0),a.strain_percent,a.pve_percent,a.ignore_shield,0};
        if (effect<=EYu || effect==EShangDot || effect==EJueDot)
            in.damage_percent+=(profile&MingJinProfile)?205:0;
        if (effect==EGong || effect==EBianGong) {
            in.damage_percent+=((config.recipes[0]&GongDamage3)?31:0)+((config.recipes[0]&GongDamage4)?41:0);
            if (Has(HaoZhong)) in.crit_power_percent+=102;
        }
        if (effect==EZhi || effect==EBianZhi) {
            in.damage_percent+=((config.recipes[2]&ZhiDamage3)?31:0)+((config.recipes[2]&ZhiDamage4)?41:0);
            if (Has(FeiFan)) in.damage_percent+=effect==EZhi?102:152;
        }
        if (effect==EGong || effect==EBianGong || effect==EZhi || effect==EBianZhi) {
            if (HasEquipment(SetSkill)) in.damage_percent+=102;
            if (HasEquipment(WeaponCW)) in.damage_percent+=51;
        }
        if (effect==EGong || effect==EZhi || effect==EGongCw)
            in.effect_ignore_shield+=307*static_cast<int>((profile&YangChunMask)/YangChunUnit);
        if (effect==EYu)
            in.damage_percent+=((config.recipes[3]&YuDamage3)?31:0)+((config.recipes[3]&YuDamage4)?41:0);
        if ((effect==EYu || effect==EYuCw) && Has(ShiXiang)) in.effect_ignore_shield+=614;
        if ((effect==EGongCw || effect==EBianGongCw) && Has(HaoZhong)) in.crit_power_percent+=102;
        if (effect==EYangChunExecute) in.damage_percent+=410;
        if (effect==EYingZi && Has(KeMeng)) in.crit_power_percent+=102;
        if (effect==ELiuZhao) in.effect_ignore_shield=922;
        return in;
    }
    double Chance(unsigned effect,unsigned profile)const {
        const auto &a=config.attributes;
        int bonus=a.crit_basis_points+((profile&1)?400:0)+((profile&2)?500:0);
        if (effect==EGong || effect==EBianGong) {
            bonus+=((config.recipes[0]&GongCrit2)?200:0)+((config.recipes[0]&GongCrit3)?300:0)+((config.recipes[0]&GongCrit4)?400:0);
            if (Has(HaoZhong)) bonus+=1000;
        }
        if (effect==EShang || effect==EShangDot)
            bonus+=((config.recipes[1]&ShangCrit2)?200:0)+((config.recipes[1]&ShangCrit3)?300:0)+((config.recipes[1]&ShangCrit4)?400:0);
        if (effect==EZhi || effect==EBianZhi)
            bonus+=((config.recipes[2]&ZhiCrit2)?200:0)+((config.recipes[2]&ZhiCrit3)?300:0)+((config.recipes[2]&ZhiCrit4)?400:0);
        if (effect==EYu) bonus+=((config.recipes[3]&YuCrit3)?300:0)+((config.recipes[3]&YuCrit4)?400:0);
        if (effect==EYingZi && Has(KeMeng)) bonus+=1000;
        return a.crit/(9.530*8250)+bonus/10000.0;
    }
    value_t Reduce(const DamageIntent &hit)const {
        const auto profile=CacheIndex(hit.snapshot);
        const bool critical=hit.outcome==RollResult::DOUBLE;
        // Legacy MiaoYi adds power even at the stack cap. Capture its unbounded
        // remainder in the payload instead of growing a cache dimension per fight.
        if (!critical || hit.base==0) return damage[profile][hit.effect][critical];
        return AfterCritical(before_crit[profile][hit.effect],
            power[hit.snapshot&(ProfileCount-1)][hit.effect]+static_cast<int>(hit.base),true,strain,
            config.attributes.pve_percent,config.target.vulnerable,level);
    }
    unsigned CacheIndex(unsigned profile)const {
        return (profile&(ProfileCount-1))+ProfileCount*((profile&WaterMask)/WaterUnit+
            water_profiles*((profile&BeltMask)/BeltUnit));
    }
    std::size_t CacheBytes()const {
        return damage.size()*sizeof(damage[0])+before_crit.size()*sizeof(before_crit[0])+sizeof(power)+sizeof(critical_chance);
    }
    void ValidateCapacity(std::size_t capacity)const {
        // Each MiaoYi grant emits five intents and adds at most 410 power.
        const long double extra=(Has(ZhengLvHeMing)?82.0L*capacity+410:0)+(HasTeam(JingMiao)?205:0);
        const long double limit=static_cast<long double>(std::numeric_limits<value_t>::max());
        auto checked=[&](long double v) {
            if (v>=limit) throw std::invalid_argument("Mo Wen attributes and log capacity could overflow damage");
            return v;
        };
        const auto crit_power=max_power+extra;
        if (crit_power>std::numeric_limits<int>::max())
            throw std::invalid_argument("Mo Wen log capacity could overflow accumulated critical power");
        auto upper=checked(max_before_crit*1.75L+checked(max_before_crit*crit_power)/1024+1);
        upper=checked(upper*level+1);
        upper=checked(upper*strain)/1024+1;
        upper=checked(upper*(1024+config.attributes.pve_percent))/1024+1;
        upper=checked(upper*(1024+config.target.vulnerable))/1024+1;
        checked(upper*capacity);
    }
    void Validate()const {
        const auto &a=config.attributes;
        for (value_t v:{a.attack_base,a.attack_by_class,a.weapon,a.crit,a.crit_power,a.overcome,a.strain,a.surplus,a.haste})
            if (v<0 || v>1000000) throw std::invalid_argument("Mo Wen attribute outside 0..1000000");
        for (int v:{a.attack_percent,a.overcome_percent,a.haste_percent,a.crit_basis_points,a.crit_power_percent,
                       a.damage_percent,a.strain_percent,a.pve_percent,config.target.vulnerable})
            if (v<0 || v>10000) throw std::invalid_argument("Mo Wen bonus outside 0..10000");
        if (a.ignore_shield<0 || a.ignore_shield>1024) throw std::invalid_argument("invalid shield ignore");
        if (config.target.level<110 || config.target.level>130 || config.target.shield<0 || config.target.shield>10000000 ||
            !std::isfinite(config.target.distance) || config.target.distance<0 || config.target.distance>1000 ||
            !std::isfinite(config.target.life) || config.target.life<.01 || config.target.life>1)
            throw std::invalid_argument("invalid Mo Wen target");
        if ((config.talents&~((1u<<15)-1)) || (config.team_effects&~(YouRen|JingMiao)))
            throw std::invalid_argument("unknown Mo Wen talent or team effect");
        if (config.equipment&~(SetAttribute|WeaponCW|SetSkill|EnchantShoes|EnchantWrist|EnchantBelt|WeaponWater))
            throw std::invalid_argument("unknown Mo Wen equipment effect");
        if (config.delay<0 || config.delay>160 || config.initial_yingzi<0 || config.initial_yingzi>6 ||
            (config.initial_style!=Style::YangChunBaiXue && config.initial_style!=Style::GaoShanLiuShui))
            throw std::invalid_argument("invalid initial Mo Wen state");
        constexpr std::array<unsigned,4> masks{255,(1u<<14)-256,((1u<<20)-1)^((1u<<14)-1),((1u<<24)-1)^((1u<<20)-1)};
        for (unsigned i=0; i<4; ++i)
            if ((config.recipes[i]&~masks[i]) || std::popcount(config.recipes[i])>4)
                throw std::invalid_argument("invalid Mo Wen recipe selection");
    }
};
inline std::shared_ptr<const Prepared> Prepare(Config config={}) {
    return std::make_shared<const Prepared>(std::move(config));
}
} // namespace JX3DPS::runtime::mo_wen
#endif
