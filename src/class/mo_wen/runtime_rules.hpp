#ifndef JX3DPS_MO_WEN_RUNTIME_RULES_HPP
#define JX3DPS_MO_WEN_RUNTIME_RULES_HPP

#include "runtime_data.hpp"

namespace JX3DPS::runtime::mo_wen {

struct SplitMixRolls {
    std::uint64_t state=0;
    void Reset(std::uint64_t seed) { state=seed; }
    std::uint64_t Next() {
        auto z=(state+=0x9e3779b97f4a7c15ULL);
        z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;
        z=(z^(z>>27))*0x94d049bb133111ebULL;
        return z^(z>>31);
    }
};
struct AuraSnapshot {
    std::uint16_t profile=0;
    int extra_power=0;
    bool operator==(const AuraSnapshot &)const=default;
};
struct Shadow {
    tick_t expires_at=0, tick_at=0;
    std::uint64_t order=0;
    AuraSnapshot snapshot{};
    bool operator==(const Shadow &)const=default;
};

// Rng is a compile-time policy, also used for deterministic legacy draw replay.
template<typename Rng=SplitMixRolls>
class BasicRules {
public:
    static constexpr bool owns_last_skill=true;
    explicit BasicRules(std::shared_ptr<const Prepared> prepared=Prepare(),Rng rng={})
        :prepared_(std::move(prepared)),rng_(std::move(rng)) {
        if (!prepared_) throw std::invalid_argument("missing prepared Mo Wen configuration");
    }
    const Prepared &Data()const { return *prepared_; }
    AuraSnapshot CurrentAura()const { return Snapshot(); }
    double CurrentHaste()const { return TimingNow().haste; }
    Style CurrentStyle()const { return style_; }
    int ShadowCount()const { return shadow_count_; }
    const std::array<Shadow,6> &Shadows()const { return shadows_; }
    template<typename Engine> void Setup(Engine &e) {
        auto &s=e.MutableState();
        if (s.skill_ready_at.size()!=SkillCount || s.self_buff_stacks.size()!=BuffCount)
            throw std::invalid_argument("macro was not compiled for Mo Wen");
        Data().ValidateCapacity(e.Log().DamageCapacity());
        for (Slot k=0; k<SkillCount; ++k) {
            cooldown_[k]=e.Timers().Create({TimerKind::SkillCooldown,k},2);
            prepare_[k]=e.Timers().Create({TimerKind::SkillPrepare,k},1);
        }
        channel_=e.Timers().Create({TimerKind::SkillCasting},1);
        for (Slot b=0; b<BuffCount; ++b) {
            tick_[b]=e.Timers().Create({TimerKind::BuffInterval,b,IsDot(b)},10);
            expire_[b]=e.Timers().Create({TimerKind::BuffDuration,b,IsDot(b)},20);
        }
        for (Slot g=0; g<gcd_.size(); ++g) gcd_[g]=e.Timers().Create({TimerKind::MacroWake,g},0);
        for (Slot i=0; i<shadows_.size(); ++i) shadow_timer_[i]=e.Timers().Create({TimerKind::Field,i},11);
    }
    template<typename Engine> void Reset(Engine &e,std::uint64_t seed) {
        auto &s=e.MutableState();
        rng_.Reset(seed);
        style_=Data().config.initial_style;
        profile_=0; timing_index_=0; miaoyi_power_=jingmiao_power_=0;
        shadow_count_=0; shadow_order_=0; shadows_.fill({});
        dot_snapshot_.fill({}); dot_interval_.fill(0);
        recharge_at_.fill(0); gu_snapshot_={};
        channel_index_=channel_xianfeng_=0;
        zhiyin_active_=false; zhiyin_level_=1; zhiyin_last_gong_=false;
        weapon_ready_at_=480; shoes_ready_at_=wrist_ready_at_=belt_ready_at_=0;
        s.qidian=0; s.target_life=Data().config.target.life;
        for (Slot k:CastableSkills) s.skill_gcd_group[k]=UsesGcd(k)||k==YiXing?0:INVALID_SLOT;
        s.skill_gcd_group[ShuYing]=4; s.skill_gcd_group[GuYing]=5; s.skill_gcd_group[WuYin]=6;
        SetStyleGroups(s);
        s.skill_energy[Yu]=s.skill_energy[ShuYing]=3;
        if (Data().Has(LiuZhaoTalent)) s.skill_energy[Zhi]=s.skill_energy[BianZhi]=3;
        if (Data().Has(CanLianTalent) && style_==Style::YangChunBaiXue) Aura(e,CanLian,Forever,1);
        for (int i=0; i<Data().config.initial_yingzi; ++i) AddShadow(e);
        if (Data().HasTeam(JingMiao)) e.Timers().ArmAt(tick_[JingMiaoBuff],Integer(0,2));
    }
    template<typename Engine> void BeforeMacroLine(Engine &e,const MacroLine &line) {
        // The legacy parser calls Player::StopReCastSkill before conditions,
        // including for YangChun (its skill-specific override is not consulted).
        if (e.GetState().casting_skill!=INVALID_SLOT && IsForceCast(line.command)) Interrupt(e);
    }
    template<typename Engine> bool Cast(Engine &e,const MacroLine &line) {
        auto &s=e.MutableState();
        const auto k=line.action_slot;
        if (!IsCastable(k)) throw std::invalid_argument("damage-only Mo Wen skill cannot be cast");
        BeforeMacroLine(e,line);
        const bool active_yangchun=k==YangChun && style_==Style::YangChunBaiXue;
        if (s.preparing_skill!=INVALID_SLOT && !(active_yangchun && s.preparing_skill==Gong))
            return Reject(e,ActionBlockReason::Preparing,s.skill_prepare_at[s.preparing_skill]-s.now,0,s.preparing_skill);
        if (s.casting_skill!=INVALID_SLOT && !active_yangchun)
            return Reject(e,ActionBlockReason::Channeling,s.skill_casting_at[s.casting_skill]-s.now,0,s.casting_skill);
        const auto ready=s.SkillReadyAt(k);
        if (ready>s.now)
            return Reject(e,s.skill_ready_at[k]==ready?ActionBlockReason::SkillCooldown:ActionBlockReason::GlobalCooldown,ready-s.now,0,k);
        if ((k==Gong || k==Zhi) && style_!=Style::YangChunBaiXue)
            return Reject(e,ActionBlockReason::MissingBuff,1,0,YangChunBuff);
        if ((k==BianGong || k==BianZhi) && style_!=Style::GaoShanLiuShui)
            return Reject(e,ActionBlockReason::MissingBuff,0,1,GaoShanBuff);
        if (active_yangchun && s.preparing_skill==INVALID_SLOT && s.casting_skill==INVALID_SLOT)
            return Reject(e,ActionBlockReason::Preparing,0,1,Gong);
        if (IsZhengLv(k) && !Data().Has(ZhengLvHeMing))
            return Reject(e,ActionBlockReason::MissingTalent,0,1,ZhengLvBuff);
        if (Charged(k) && !s.skill_energy[k]) return Reject(e,ActionBlockReason::Charges,0,1,k);
        if (k==YiXing && !shadow_count_) return Reject(e,ActionBlockReason::MissingBuff,0,1,YingZi);
        if (k==ShuYing && shadow_count_==6) return Reject(e,ActionBlockReason::Charges,6,5,ShuYing);
        const auto range=Range(k);
        if (Data().config.target.distance>range)
            return Reject(e,ActionBlockReason::Distance,Data().config.target.distance,range,k);
        const auto previous=s.last_skill;
        s.last_skill=k;
        if (UsesGcd(k)) Gcd(e,0,TimingNow().gcd);
        switch (k) {
            case Gong: case BianGong:
                if (s.self_buff_stacks[WeaponCwBuff]) GongDamage(e,k,true);
                else {
                    SetPrepare(s,e.Log(),k,s.now+TimingNow().gong,true);
                    e.Timers().ArmAt(prepare_[k],s.now+TimingNow().gong);
                }
                break;
            case Shang: case Jue:
                AddXianFeng(e); Enchants(e); Equipment(e); Water(e);
                Hit(e,k==Shang?EShang:EJue,Snapshot(),true);
                AddQuFeng(e,(k==Shang?2:1)+static_cast<int>(style_));
                AddDot(e,k==Shang?ShangDot:JueDot);
                break;
            case Zhi: case BianZhi: StartZhi(e,k); break;
            case Yu:
                ConsumeCharge(e,Yu);
                AddQuFeng(e,4+static_cast<int>(style_));
                AddXianFeng(e); Enchants(e); Equipment(e); Water(e);
                if (Data().HasEquipment(WeaponCW) && Proc(307,1024)) Hit(e,EYuCw,Snapshot(),true);
                if (Data().Has(ShiXiang)) {
                    ShortenCharge(e,Yu,80);
                    if (Data().Has(LiuZhaoTalent)) ShortenCharge(e,Zhi,80);
                }
                if (Data().Has(MingJinTalent))
                    Aura(e,MingJin,(s.self_buff_stacks[MingJin]?s.self_buff_expires_at[MingJin]-s.now:0)+128,1);
                Hit(e,EYu,Snapshot());
                break;
            case GaoShan: case YangChun: CastStyle(e,k,previous); break;
            case ShuYing:
                Gcd(e,4,TimingNow().short_gcd); ConsumeCharge(e,ShuYing);
                Zero(e,EZeroShuYing);
                Wrist(e);
                AddShadow(e);
                if (Proc(Data().Has(KeMeng)?100:20,100)) AddShadow(e);
                break;
            case YiXing:
                MarkXianFeng(e); RemoveShadow(e,OldestShadow()); break;
            case GuYing: CastGuYing(e); break;
            case ZhengLv1: case ZhengLv2: case ZhengLv3:
                Cooldown(e,k,s.now+960);
                Zero(e,static_cast<Effect>(EZeroZhengLv1+k-ZhengLv1));
                Wrist(e);
                Aura(e,ZhengLvBuff,320,1);
                AddZhiYin(e);
                zhiyin_level_=k-ZhengLv1+1;
                break;
            case WuYin: Gcd(e,6,TimingNow().wuyin); Enchants(e); Equipment(e); Water(e); Hit(e,EWuYin,Snapshot()); break;
        }
        return true;
    }
    template<typename Engine> void OnTimer(Engine &e,TimerHandle h,DeadlineQueue::TimerId) {
        auto &s=e.MutableState();
        switch (h.kind) {
            case TimerKind::SkillPrepare:
                if (s.preparing_skill!=h.slot) return;
                SetPrepare(s,e.Log(),h.slot,0,false);
                if (h.slot==Gong || h.slot==BianGong) GongDamage(e,h.slot,false);
                else if (IsStyleSkill(h.slot)) CompleteStyle(e,h.slot);
                break;
            case TimerKind::SkillCasting: TickZhi(e); break;
            case TimerKind::SkillCooldown:
                if (Charged(h.slot)) RecoverCharge(e,h.slot);
                break;
            case TimerKind::BuffInterval:
                if (IsDot(h.slot)) TickDot(e,h.slot);
                else if (h.slot==JingMiaoBuff) {
                    Aura(e,JingMiaoBuff,80,1);
                    e.Timers().ArmAt(tick_[JingMiaoBuff],s.now+320+Integer(0,6));
                }
                break;
            case TimerKind::BuffDuration: Expire(e,h.slot); break;
            case TimerKind::Field: TickShadow(e,h.slot); break;
            default: break;
        }
    }
    value_t Reduce(const DamageIntent &hit)const { return Data().Reduce(hit); }

private:
    static constexpr tick_t Forever=std::numeric_limits<tick_t>::max();
    struct GuSnapshot {
        std::array<tick_t,SkillCount> cooldown{},recharge{};
        std::array<int,SkillCount> charges{};
        std::array<tick_t,4> groups{};
    };
    template<typename Engine> static bool Reject(Engine &e,ActionBlockReason reason,double actual,
                                                  double required,Slot subject) {
        if constexpr (Engine::debug_enabled) e.RejectAction(reason,actual,required,subject);
        return false;
    }
    const Timing &TimingNow()const { return Data().timing[timing_index_]; }
    bool Charged(Slot k)const { return k==Yu || k==ShuYing || (IsZhi(k) && Data().Has(LiuZhaoTalent)); }
    static Slot ChargeSlot(Slot k) { return k==BianZhi?Zhi:k; }
    tick_t Recharge(Slot k)const { return k==Yu?Data().yu_cooldown:k==ShuYing?320:128; }
    double Range(Slot k)const {
        if (IsStyleSkill(k) || k==YiXing || k==GuYing) return 1000;
        if ((k==Gong || k==BianGong) && (Data().config.recipes[0]&GongRange)) return 22;
        if (IsZhi(k) && (Data().config.recipes[2]&ZhiRange)) return 22;
        return 20;
    }
    static double Unit(std::uint64_t key) {
        if constexpr (requires { Rng::UnitSample(key); }) return Rng::UnitSample(key);
        else return static_cast<double>(key>>11)*0x1.0p-53;
    }
    int Integer(int low,int high) {
        if constexpr (requires { rng_.Integer(low,high); }) return rng_.Integer(low,high);
        else return low+static_cast<int>(rng_.Next()%static_cast<unsigned>(high-low+1));
    }
    bool Proc(int numerator,int denominator) { return Integer(1,denominator)<=numerator; }
    AuraSnapshot Snapshot()const { return {profile_,miaoyi_power_+jingmiao_power_}; }
    template<typename Engine> void Hit(Engine &e,Effect effect,AuraSnapshot snap,bool you_ren=false) {
        const auto key=rng_.Next();
        const bool critical=Unit(key)<Data().critical_chance[ChanceProfile(snap.profile)][effect];
        const bool shoes=effect<=EWuYin || effect==EGongCw || effect==EBianGongCw || effect==EYuCw;
        if (shoes && critical && Data().HasEquipment(EnchantShoes) && e.GetState().now>=shoes_ready_at_) {
            shoes_ready_at_=e.GetState().now+160;
            Hit(e,EShoes,Snapshot());
        }
        if (you_ren && critical && Data().HasTeam(YouRen)) Aura(e,YouRenBuff,80,1);
        const auto &f=Data().formulas[effect];
        e.Log().Record(e.GetState().now,f.skill,0,f.level,key,snap.extra_power,0,
            critical?RollResult::DOUBLE:RollResult::HIT,effect,snap.profile,f.sub);
    }
    template<typename Engine> void Zero(Engine &e,Effect effect) {
        e.Log().Record(e.GetState().now,Data().formulas[effect].skill,INVALID_SLOT,0,0,0,0,
            RollResult::HIT,effect,0);
    }
    template<typename Engine> void Gcd(Engine &e,Slot group,tick_t duration) {
        auto &s=e.MutableState();
        SetGcd(s,e.Log(),group,s.now+duration);
        e.Timers().ArmAt(gcd_[group],s.now+duration);
    }
    template<typename Engine> void Cooldown(Engine &e,Slot k,tick_t deadline) {
        SetSkillCooldown(e.MutableState(),e.Log(),k,deadline);
        if (deadline>e.GetState().now) e.Timers().ArmAt(cooldown_[k],deadline);
        else e.Timers().Cancel(cooldown_[k]);
    }
    template<typename Engine> void PublishCharge(Engine &e,Slot k,int count) {
        auto &s=e.MutableState();
        SetSkillEnergy(s,e.Log(),k,count);
        SetSkillCooldown(s,e.Log(),k,count?0:recharge_at_[k]);
        if (k==Zhi) {
            SetSkillEnergy(s,e.Log(),BianZhi,count);
            SetSkillCooldown(s,e.Log(),BianZhi,count?0:recharge_at_[k]);
        }
        if (count<3) e.Timers().ArmAt(cooldown_[k],recharge_at_[k]);
        else { recharge_at_[k]=0; e.Timers().Cancel(cooldown_[k]); }
    }
    template<typename Engine> void ConsumeCharge(Engine &e,Slot skill) {
        const Slot k=ChargeSlot(skill);
        const int count=e.GetState().skill_energy[k];
        if (count==3) recharge_at_[k]=e.GetState().now+Recharge(k);
        PublishCharge(e,k,count-1);
    }
    template<typename Engine> void RecoverCharge(Engine &e,Slot k) {
        const int count=std::min(3,e.GetState().skill_energy[k]+1);
        recharge_at_[k]=e.GetState().now+Recharge(k);
        PublishCharge(e,k,count);
    }
    template<typename Engine> void ShortenCharge(Engine &e,Slot k,tick_t delta) {
        const auto &s=e.GetState();
        if (s.skill_energy[k]==3) return;
        recharge_at_[k]-=delta;
        int count=s.skill_energy[k];
        if (recharge_at_[k]<=s.now) { ++count; recharge_at_[k]+=Recharge(k); }
        PublishCharge(e,k,count);
    }
    void SetStyleGroups(State &s) {
        s.skill_gcd_group[GaoShan]=style_==Style::GaoShanLiuShui?2:1;
        s.skill_gcd_group[YangChun]=style_==Style::YangChunBaiXue?3:1;
    }
    template<typename Engine> void Aura(Engine &e,Slot b,tick_t duration,int stacks) {
        auto &s=e.MutableState();
        const auto expiry=duration==Forever?Forever:s.now+duration;
        SetBuff(s,e.Log(),false,b,stacks||b==ZhiYinHeMing?expiry:0,0,stacks);
        UpdateProfile(s,b);
        if (duration!=Forever && (stacks || b==ZhiYinHeMing)) e.Timers().ArmAt(expire_[b],expiry);
        else e.Timers().Cancel(expire_[b]);
    }
    void UpdateProfile(const State &s,Slot b) {
        const int n=s.self_buff_stacks[b];
        switch (b) {
            case YunHan: profile_=static_cast<std::uint16_t>((profile_&~YunHanMask)|n); break;
            case CanLian: profile_=static_cast<std::uint16_t>((profile_&~CanLianProfile)|(n?CanLianProfile:0)); break;
            case MingJin: profile_=static_cast<std::uint16_t>((profile_&~MingJinProfile)|(n?MingJinProfile:0)); break;
            case YangChunBuff: profile_=static_cast<std::uint16_t>((profile_&~YangChunMask)|n*YangChunUnit); break;
            case SetAttributeBuff: profile_=static_cast<std::uint16_t>((profile_&~SetProfile)|(n?SetProfile:0)); break;
            case YouRenBuff: profile_=static_cast<std::uint16_t>((profile_&~YouRenProfile)|(n?YouRenProfile:0)); break;
            case GaoShanBuff: timing_index_=n!=0; break;
            case JingMiaoBuff: jingmiao_power_=n?205:0; break;
            case WaterBuff: profile_=static_cast<std::uint16_t>((profile_&~WaterMask)|n*WaterUnit); break;
            case BeltBuff: if (!n) profile_=static_cast<std::uint16_t>(profile_&~BeltMask); break;
        }
    }
    template<typename Engine> void ClearAura(Engine &e,Slot b) {
        SetBuff(e.MutableState(),e.Log(),false,b,0,0,0);
        UpdateProfile(e.GetState(),b);
        e.Timers().Cancel(expire_[b]);
    }
    template<typename Engine> void Expire(Engine &e,Slot b) {
        if (IsDot(b)) {
            SetBuff(e.MutableState(),e.Log(),true,b,0,0,0);
            e.Timers().Cancel(tick_[b]); return;
        }
        if (b==ZhiYinMiaoYi) miaoyi_power_-=static_cast<int>(102.5*(e.GetState().self_buff_stacks[b]+1));
        ClearAura(e,b);
        if (b==GuYingBuff) Gcd(e,5,2400);
        if (b==ZhengLvBuff) { zhiyin_level_=3; FinishZhiYin(e); }
        if (b==ZhiYinHeMing) { zhiyin_active_=false; zhiyin_level_=3; }
    }
    template<typename Engine> void AddQuFeng(Engine &e,int add) {
        const int n=e.GetState().self_buff_stacks[QuFeng]+add;
        Aura(e,QuFeng,Forever,n>=9?0:n);
        if (n==9) {
            PoZhaoHits(e);
            if (e.GetState().self_buff_stacks[ZhengLvBuff]) AddZhiYin(e);
        }
    }
    template<typename Engine> void PoZhaoHits(Engine &e) {
        if (Data().Has(LiuZhaoTalent)) {
            const int n=e.GetState().self_buff_stacks[LiuZhao];
            for (int i=0; i<n; ++i) Hit(e,ELiuZhaoSurplus,Snapshot());
            if (n) ClearAura(e,LiuZhao);
        }
        if (Data().Has(ZhengMing)) Hit(e,ESurplus3,Snapshot());
        for (auto effect:{ESurplus0,ESurplus1,ESurplus2}) Hit(e,effect,Snapshot());
    }
    template<typename Engine> void AddXianFeng(Engine &e) {
        if (Data().Has(XianFengTalent)) Aura(e,XianFeng,160,std::min(5,e.GetState().self_buff_stacks[XianFeng]+1));
    }
    template<typename Engine> void MarkXianFeng(Engine &e) {
        if (Data().Has(XianFengTalent) && e.GetState().self_buff_stacks[XianFeng]) Aura(e,XianFengMark,53,1);
    }
    template<typename Engine> void ClearXianFeng(Engine &e) {
        if (e.GetState().self_buff_stacks[XianFengMark] && e.GetState().self_buff_stacks[XianFeng]) ClearAura(e,XianFeng);
    }
    template<typename Engine> void Equipment(Engine &e) {
        if (Data().HasEquipment(SetAttribute) && Proc(10,100)) Aura(e,SetAttributeBuff,96,1);
        if (!Data().HasEquipment(WeaponCW)) return;
        const bool proc=Proc(25,1024);
        if (!proc || e.GetState().now<weapon_ready_at_) return;
        weapon_ready_at_=e.GetState().now+480;
        Aura(e,WeaponCwBuff,96,1);
        for (Slot k:{Slot{Yu},Slot{Zhi}}) if (Charged(k)) {
            recharge_at_[k]=e.GetState().now;
            // Legacy sets recharge to zero; the next skill trigger restores one charge.
            e.Timers().ArmAt(cooldown_[k],recharge_at_[k]);
            if (!e.GetState().skill_energy[k]) {
                SetSkillCooldown(e.MutableState(),e.Log(),k,recharge_at_[k]);
                if (k==Zhi) SetSkillCooldown(e.MutableState(),e.Log(),BianZhi,recharge_at_[k]);
            }
        }
    }
    template<typename Engine> void Wrist(Engine &e) {
        if (Data().HasEquipment(EnchantWrist) && e.GetState().now>=wrist_ready_at_ && Proc(10,100)) {
            wrist_ready_at_=e.GetState().now+160;
            Hit(e,EWrist,Snapshot());
        }
    }
    template<typename Engine> void Enchants(Engine &e) {
        Wrist(e);
        if (Data().HasEquipment(EnchantBelt) && e.GetState().now>=belt_ready_at_ && Proc(20,100)) {
            belt_ready_at_=e.GetState().now+480;
            const unsigned bonus=Proc(70,100)?2:1;
            profile_=static_cast<std::uint16_t>((profile_&~BeltMask)|bonus*BeltUnit);
            Aura(e,BeltBuff,128,1);
        }
    }
    template<typename Engine> void Water(Engine &e) {
        if (Data().HasEquipment(WeaponWater)) Aura(e,WaterBuff,96,std::min(10,e.GetState().self_buff_stacks[WaterBuff]+1));
    }
    template<typename Engine> void GongDamage(Engine &e,Slot k,bool cw) {
        if (!cw) AddQuFeng(e,k==Gong?3:4);
        AddXianFeng(e);
        if (!cw && k==BianGong && Data().Has(ZhiZhi)) {
            for (Slot b:{Slot{ShangDot},Slot{JueDot}})
                if (e.GetState().target_buff_stacks[b]) AddDot(e,b);
            Gcd(e,1,0);
        }
        Enchants(e); Equipment(e);
        if (cw) {
            if (e.GetState().self_buff_stacks[WeaponCwBuff]) AddDot(e,ShenBingGong);
            Water(e);
            PoZhaoHits(e);
            if (e.GetState().self_buff_stacks[ZhengLvBuff]) AddZhiYin(e);
            Aura(e,QuFeng,Forever,0);
        } else Water(e);
        Hit(e,cw?(k==Gong?EGongCw:EBianGongCw):(k==Gong?EGong:EBianGong),Snapshot(),!cw);
        ClearAura(e,YangChunBuff);
    }
    template<typename Engine> void StartZhi(Engine &e,Slot k) {
        auto &s=e.MutableState();
        if (Charged(k)) ConsumeCharge(e,k);
        channel_index_=0;
        SetCasting(s,e.Log(),k,s.now+3*TimingNow().zhi,true);
        e.Timers().ArmAt(channel_,s.now+TimingNow().zhi);
        AddQuFeng(e,k==Zhi?5:6);
        if (k==Zhi) ClearAura(e,YangChunBuff);
        AddXianFeng(e);
        channel_xianfeng_=s.self_buff_stacks[XianFeng];
        if (Data().Has(HaoQing)) ZhiDamage(e,k);
        ClearXianFeng(e);
    }
    template<typename Engine> void ZhiDamage(Engine &e,Slot k) {
        if (e.GetState().self_buff_stacks[XianFengMark])
            for (int i=0; i<channel_xianfeng_; ++i) Hit(e,EXianFeng,Snapshot());
        Equipment(e); Water(e);
        if (Data().Has(LiuZhaoTalent)) {
            const auto &s=e.GetState();
            const int n=(s.target_buff_stacks[ShangDot]>0)+(s.target_buff_stacks[JueDot]>0);
            for (int i=0; i<n; ++i) Hit(e,ELiuZhao,Snapshot());
            if (n) Aura(e,LiuZhao,400,std::min(32,s.self_buff_stacks[LiuZhao]+n));
        }
        Enchants(e);
        Hit(e,k==Zhi?EZhi:EBianZhi,Snapshot(),true);
    }
    template<typename Engine> void TickZhi(Engine &e) {
        auto &s=e.MutableState();
        const auto k=s.casting_skill;
        if (!IsZhi(k)) return;
        ZhiDamage(e,k);
        if (++channel_index_<3) {
            SetCasting(s,e.Log(),k,s.now+(3-channel_index_)*TimingNow().zhi,true);
            e.Timers().ArmAt(channel_,s.now+TimingNow().zhi);
        } else {
            SetCasting(s,e.Log(),k,0,false);
            if (k==Zhi) ClearAura(e,YangChunBuff);
            else ClearXianFeng(e);
        }
    }
    template<typename Engine> void Interrupt(Engine &e) {
        auto &s=e.MutableState();
        if constexpr (Engine::debug_enabled) e.NoteChannelInterrupt(s.casting_skill);
        SetCasting(s,e.Log(),s.casting_skill,0,false);
        e.Timers().Cancel(channel_);
        channel_index_=0;
        ClearAura(e,YangChunBuff);
    }
    template<typename Engine> void AddDot(Engine &e,Slot b) {
        auto &s=e.MutableState();
        const auto next=s.target_buff_stacks[b]?s.target_buff_tick_at[b]:s.now+TimingNow().dot;
        const int count=b==ShenBingGong?10:6;
        const auto expiry=next+static_cast<int>(48*TimingNow().haste*(count-1));
        const int stacks=b==ShenBingGong?std::min(3,s.target_buff_stacks[b]+1):1;
        dot_snapshot_[b]=Snapshot(); dot_interval_[b]=TimingNow().dot;
        SetBuff(s,e.Log(),true,b,expiry,next,stacks);
        e.Timers().ArmAt(tick_[b],next); e.Timers().ArmAt(expire_[b],expiry);
    }
    template<typename Engine> void TickDot(Engine &e,Slot b) {
        auto &s=e.MutableState();
        if (!s.target_buff_stacks[b]) return;
        const auto effect=b==ShangDot?EShangDot:b==JueDot?EJueDot:static_cast<Effect>(EShenBing+s.target_buff_stacks[b]-1);
        Hit(e,effect,dot_snapshot_[b]);
        const auto next=s.now+dot_interval_[b];
        SetBuff(s,e.Log(),true,b,s.target_buff_expires_at[b],next,s.target_buff_stacks[b]);
        if (next<=s.target_buff_expires_at[b]) e.Timers().ArmAt(tick_[b],next);
    }
    template<typename Engine> void CastStyle(Engine &e,Slot k,Slot previous) {
        auto &s=e.MutableState();
        const Style wanted=k==GaoShan?Style::GaoShanLiuShui:Style::YangChunBaiXue;
        if (style_==wanted) {
            if (k==GaoShan) { Zero(e,EZeroGaoShan); Gcd(e,2,800); Wrist(e); Aura(e,GaoShanBuff,192,1); }
            else {
                Gcd(e,3,Data().yangchun_cooldown); Equipment(e); Water(e); Enchants(e);
                Hit(e,s.target_life-.01<.5?EYangChunExecute:EYangChun,Snapshot());
                Aura(e,YangChunBuff,96,std::min(3,s.self_buff_stacks[YangChunBuff]+1));
            }
            return;
        }
        Zero(e,k==GaoShan?EZeroGaoShan:EZeroYangChun);
        Gcd(e,1,48);
        if (k==YangChun) ClearAura(e,GaoShanBuff);
        else if (!s.self_buff_stacks[ZhengLvBuff]) Aura(e,QuFeng,Forever,0);
        const bool prepare=k==GaoShan?(previous==Gong||previous==Zhi):(previous==BianGong||previous==BianZhi);
        if (prepare) {
            SetPrepare(s,e.Log(),k,s.now+6,true);
            e.Timers().ArmAt(prepare_[k],s.now+6);
        } else CompleteStyle(e,k);
    }
    template<typename Engine> void CompleteStyle(Engine &e,Slot k) {
        style_=k==GaoShan?Style::GaoShanLiuShui:Style::YangChunBaiXue;
        SetStyleGroups(e.MutableState());
        if (Data().Has(CanLianTalent)) {
            if (k==YangChun) Aura(e,CanLian,Forever,1);
            else ClearAura(e,CanLian);
        }
        MarkXianFeng(e);
    }
    Slot OldestShadow()const {
        Slot result=INVALID_SLOT;
        for (Slot i=0; i<shadows_.size(); ++i)
            if (shadows_[i].expires_at && (result==INVALID_SLOT || shadows_[i].order<shadows_[result].order)) result=i;
        return result;
    }
    template<typename Engine> void PublishShadows(Engine &e) {
        tick_t expiry=Forever,next=Forever;
        for (const auto &shadow:shadows_) if (shadow.expires_at) {
            expiry=std::min(expiry,shadow.expires_at); next=std::min(next,shadow.tick_at);
        }
        SetBuff(e.MutableState(),e.Log(),false,YingZi,shadow_count_?expiry:0,shadow_count_?next:0,shadow_count_);
    }
    template<typename Engine> void AddShadow(Engine &e) {
        // Legacy takes the seventh shadow's snapshot before removing the oldest.
        if (Data().Has(YunHanTalent)) Aura(e,YunHan,1600,shadow_count_+1);
        const auto snap=Snapshot();
        if (shadow_count_==6) RemoveShadow(e,OldestShadow());
        Slot slot=0;
        while (shadows_[slot].expires_at) ++slot;
        ++shadow_count_;
        if (Data().Has(YunHanTalent)) Aura(e,YunHan,1600,shadow_count_);
        const auto now=e.GetState().now;
        shadows_[slot]={now+400,now+TimingNow().shadow,shadow_order_++,snap};
        e.Timers().ArmAt(shadow_timer_[slot],shadows_[slot].tick_at);
        PublishShadows(e);
    }
    template<typename Engine> void RemoveShadow(Engine &e,Slot slot) {
        if (slot==INVALID_SLOT) return;
        shadows_[slot]={};
        --shadow_count_;
        e.Timers().Cancel(shadow_timer_[slot]);
        if (Data().Has(YunHanTalent)) Aura(e,YunHan,1600,shadow_count_);
        PublishShadows(e);
    }
    template<typename Engine> void TickShadow(Engine &e,Slot slot) {
        auto &shadow=shadows_[slot];
        const auto now=e.GetState().now;
        if (!shadow.expires_at) return;
        if (shadow.tick_at==now) {
            Hit(e,EYingZi,shadow.snapshot);
            shadow.tick_at=now+TimingNow().shadow; // Legacy shadows use live haste at each tick.
        }
        if (shadow.expires_at==now) RemoveShadow(e,slot);
        else {
            e.Timers().ArmAt(shadow_timer_[slot],std::min(shadow.expires_at,shadow.tick_at));
            PublishShadows(e);
        }
    }
    template<typename Engine> void CastGuYing(Engine &e) {
        auto &s=e.MutableState();
        Zero(e,EZeroGuYing);
        if (!s.self_buff_stacks[GuYingBuff]) {
            Wrist(e);
            for (Slot k=Gong; k<=ZhengLv3; ++k) {
                gu_snapshot_.cooldown[k]=std::max(0,s.skill_ready_at[k]-s.now);
                gu_snapshot_.recharge[k]=recharge_at_[k]?std::max(0,recharge_at_[k]-s.now):0;
                gu_snapshot_.charges[k]=s.skill_energy[k];
            }
            for (Slot g=1; g<=4; ++g) gu_snapshot_.groups[g-1]=std::max(0,s.gcd_ready_at[g]-s.now);
            Aura(e,GuYingBuff,112,1); Gcd(e,5,16);
        } else {
            ClearAura(e,GuYingBuff);
            for (Slot k=Gong; k<=ZhengLv3; ++k) {
                if (k==GuYing || k==BianZhi) continue;
                if (Charged(k)) {
                    recharge_at_[k]=s.now+gu_snapshot_.recharge[k];
                    PublishCharge(e,k,gu_snapshot_.charges[k]);
                } else Cooldown(e,k,s.now+gu_snapshot_.cooldown[k]);
            }
            for (Slot g=1; g<=4; ++g) Gcd(e,g,gu_snapshot_.groups[g-1]);
            Gcd(e,5,2400);
        }
    }
    template<typename Engine> void AddZhiYin(Engine &e) {
        auto &s=e.MutableState();
        const int n=zhiyin_active_?s.self_buff_stacks[ZhiYinHeMing]+1:0;
        zhiyin_active_=true;
        Aura(e,ZhiYinHeMing,320,n);
        if (n==5) { ClearAura(e,ZhengLvBuff); FinishZhiYin(e); return; }
        constexpr std::array sequence{2,3,5,6,4};
        const int qufeng=sequence[n];
        Aura(e,QuFeng,Forever,qufeng);
        if (qufeng==5) zhiyin_last_gong_=true;
        else if (zhiyin_last_gong_) {
            zhiyin_last_gong_=false;
            const auto end=s.now+Integer(8,16);
            // A synthetic prepare owner exposes this old short lock to the debugger.
            SetPrepare(s,e.Log(),ZhiYinDamage,end,true);
            e.Timers().ArmAt(prepare_[ZhiYinDamage],end);
        }
    }
    template<typename Engine> void FinishZhiYin(Engine &e) {
        if (!zhiyin_active_) return;
        zhiyin_active_=false;
        ClearAura(e,ZhiYinHeMing);
        auto &s=e.MutableState();
        const int old=s.self_buff_stacks[ZhiYinMiaoYi];
        const int stacks=std::min(3,old+4-zhiyin_level_);
        miaoyi_power_+=static_cast<int>(102.5*(stacks-old+1));
        Aura(e,ZhiYinMiaoYi,240,stacks);
        for (int i=0; i<5; ++i) Hit(e,static_cast<Effect>(EZhiYin+zhiyin_level_-1),Snapshot());
    }

    std::shared_ptr<const Prepared> prepared_;
    Rng rng_;
    std::array<DeadlineQueue::TimerId,SkillCount> cooldown_{},prepare_{};
    std::array<DeadlineQueue::TimerId,BuffCount> tick_{},expire_{};
    std::array<DeadlineQueue::TimerId,7> gcd_{};
    std::array<DeadlineQueue::TimerId,6> shadow_timer_{};
    DeadlineQueue::TimerId channel_{};
    std::array<AuraSnapshot,BuffCount> dot_snapshot_{};
    std::array<tick_t,BuffCount> dot_interval_{};
    std::array<tick_t,SkillCount> recharge_at_{};
    std::array<Shadow,6> shadows_{};
    GuSnapshot gu_snapshot_{};
    Style style_=Style::GaoShanLiuShui;
    std::uint16_t profile_=0;
    unsigned timing_index_=0;
    int miaoyi_power_=0,jingmiao_power_=0;
    int channel_index_=0,channel_xianfeng_=0,shadow_count_=0,zhiyin_level_=1;
    std::uint64_t shadow_order_=0;
    tick_t weapon_ready_at_=480,shoes_ready_at_=0,wrist_ready_at_=0,belt_ready_at_=0;
    bool zhiyin_active_=false,zhiyin_last_gong_=false;
};
using Rules=BasicRules<>;

} // namespace JX3DPS::runtime::mo_wen
#endif
