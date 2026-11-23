#ifndef JX3DPS_MO_WEN_RUNTIME_RULES_HPP
#define JX3DPS_MO_WEN_RUNTIME_RULES_HPP

#include "src/core/runtime/combat_rolls.hpp"

#include "runtime_data.hpp"

namespace JX3DPS::runtime::mo_wen {

using runtime::SplitMixRolls;

struct AuraSnapshot
{
    std::uint16_t profile                                = 0;
    int           extra_power                            = 0;
    bool          operator==(const AuraSnapshot &) const = default;
};

struct Shadow
{
    tick_t        expires_at = 0, tick_at = 0;
    std::uint64_t order = 0;
    AuraSnapshot  snapshot{};
    bool          operator==(const Shadow &) const = default;
};

// Rng is a compile-time policy, also used for deterministic legacy draw replay.
template <typename Rng = SplitMixRolls>
class BasicRules
{
public:
    static constexpr bool owns_last_skill = true;

    explicit BasicRules(std::shared_ptr<const Prepared> prepared = Prepare(), Rng rng = {}) :
        prepared_(std::move(prepared)), rng_(std::move(rng))
    {
        if (!prepared_) {
            throw std::invalid_argument("missing prepared Mo Wen configuration");
        }
    }

    const Prepared &Data() const { return *prepared_; }

    AuraSnapshot CurrentAura() const { return Snapshot(); }

    double CurrentHaste() const { return TimingNow().haste; }

    Style CurrentStyle() const { return style_; }

    int ShadowCount() const { return shadow_count_; }

    const std::array<Shadow, 6> &Shadows() const { return shadows_; }

    template <typename Engine>
    void Setup(Engine &e)
    {
        target_timeline_.Setup(e, Data().config.target_events);
        auto &s = e.MutableState();
        if (s.skill_ready_at.size() != SKILL_COUNT || s.self_buff_stacks.size() != BUFF_COUNT) {
            throw std::invalid_argument("macro was not compiled for Mo Wen");
        }
        Data().ValidateCapacity(e.Log().DamageCapacity());
        for (Slot k = 0; k < SKILL_COUNT; ++k) {
            cooldown_[k] = e.Timers().Create({ TimerKind::SKILL_COOLDOWN, k }, 2);
            prepare_[k]  = e.Timers().Create({ TimerKind::SKILL_PREPARE, k }, 1);
        }
        channel_ = e.Timers().Create({ TimerKind::SKILL_CASTING }, 1);
        for (Slot b = 0; b < BUFF_COUNT; ++b) {
            tick_[b]   = e.Timers().Create({ TimerKind::BUFF_INTERVAL, b, IsDot(b) }, 10);
            expire_[b] = e.Timers().Create({ TimerKind::BUFF_DURATION, b, IsDot(b) }, 20);
        }
        for (Slot g = 0; g < gcd_.size(); ++g) {
            gcd_[g] = e.Timers().Create({ TimerKind::MACRO_WAKE, g }, 0);
        }
        for (Slot i = 0; i < shadows_.size(); ++i) {
            shadow_timer_[i] = e.Timers().Create({ TimerKind::FIELD, i }, 11);
        }
    }

    template <typename Engine>
    void Reset(Engine &e, std::uint64_t seed)
    {
        auto &s = e.MutableState();
        rng_.Reset(seed);
        style_        = Data().config.initial_style;
        profile_      = 0;
        timing_index_ = 0;
        miaoyi_power_ = jingmiao_power_ = 0;
        shadow_count_                   = 0;
        shadow_order_                   = 0;
        shadows_.fill({});
        dot_snapshot_.fill({});
        dot_interval_.fill(0);
        recharge_at_.fill(0);
        gu_snapshot_   = {};
        channel_index_ = channel_xianfeng_ = 0;
        zhiyin_active_                     = false;
        zhiyin_level_                      = 1;
        zhiyin_last_gong_                  = false;
        weapon_ready_at_                   = 480;
        shoes_ready_at_ = wrist_ready_at_ = belt_ready_at_ = 0;
        s.qidian                                           = 0;
        s.target_life                                      = Data().config.target.life;
        s.target_distance                                  = Data().config.target.distance;
        target_timeline_.Reset(e, Data().config.target_events);
        for (const Slot K : CastableSkills) {
            s.skill_gcd_group[K] = UsesGcd(K) || K == YI_XING ? 0 : INVALID_SLOT;
        }
        s.skill_gcd_group[SHU_YING] = 4;
        s.skill_gcd_group[GU_YING]  = 5;
        s.skill_gcd_group[WU_YIN]   = 6;
        SetStyleGroups(s);
        s.skill_energy[YU] = s.skill_energy[SHU_YING] = 3;
        if (Data().Has(LIU_ZHAO_TALENT)) {
            s.skill_energy[ZHI] = s.skill_energy[BIAN_ZHI] = 3;
        }
        if (Data().Has(CAN_LIAN_TALENT) && style_ == Style::YANG_CHUN_BAI_XUE) {
            Aura(e, CAN_LIAN, Forever, 1);
        }
        for (int i = 0; i < Data().config.initial_yingzi; ++i) {
            AddShadow(e);
        }
        if (Data().HasTeam(JING_MIAO)) {
            e.Timers().ArmAt(tick_[JING_MIAO_BUFF], Integer(0, 2));
        }
    }

    template <typename Engine>
    void BeforeMacroLine(Engine &e, const MacroLine &line)
    {
        // The legacy parser calls Player::StopReCastSkill before conditions,
        // including for YangChun (its skill-specific override is not consulted).
        if (e.GetState().casting_skill != INVALID_SLOT && IsForceCast(line.command)) {
            Interrupt(e);
        }
    }

    static bool MayActWhileBusy(const MacroLine &line)
    {
        return IsForceCast(line.command) || line.action_slot == YANG_CHUN || !IsCastable(line.action_slot);
    }

    static bool CanAttempt(const State &s, const MacroLine &line)
    {
        const auto SKILL = line.action_slot;
        if (!IsCastable(SKILL)) {
            return true;
        }
        // BeforeMacroLine has already applied any force-cast interrupt. Cast handles the YangChun exception.
        if ((s.preparing_skill != INVALID_SLOT || s.casting_skill != INVALID_SLOT) && SKILL != YANG_CHUN) {
            return false;
        }
        return s.SkillReadyAt(SKILL) <= s.now;
    }

    template <typename Engine>
    bool Cast(Engine &e, const MacroLine &line)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::Cast");
        auto                       &s           = e.MutableState();
        const auto                  K           = line.action_slot;
        if (!IsCastable(K)) {
            throw std::invalid_argument("damage-only Mo Wen skill cannot be cast");
        }
        BeforeMacroLine(e, line);
        const bool ACTIVE_YANGCHUN = K == YANG_CHUN && style_ == Style::YANG_CHUN_BAI_XUE;
        if (s.preparing_skill != INVALID_SLOT && !(ACTIVE_YANGCHUN && s.preparing_skill == GONG)) {
            return Reject(e, ActionBlockReason::PREPARING, s.skill_prepare_at[s.preparing_skill] - s.now, 0, s.preparing_skill);
        }
        if (s.casting_skill != INVALID_SLOT && !ACTIVE_YANGCHUN) {
            return Reject(e, ActionBlockReason::CHANNELING, s.skill_casting_at[s.casting_skill] - s.now, 0, s.casting_skill);
        }
        const auto READY = s.SkillReadyAt(K);
        if (READY > s.now) {
            return Reject(e,
                          s.skill_ready_at[K] == READY ? ActionBlockReason::SKILL_COOLDOWN : ActionBlockReason::GLOBAL_COOLDOWN,
                          READY - s.now,
                          0,
                          K);
        }
        if ((K == GONG || K == ZHI) && style_ != Style::YANG_CHUN_BAI_XUE) {
            return Reject(e, ActionBlockReason::MISSING_BUFF, 1, 0, YANG_CHUN_BUFF);
        }
        if ((K == BIAN_GONG || K == BIAN_ZHI) && style_ != Style::GAO_SHAN_LIU_SHUI) {
            return Reject(e, ActionBlockReason::MISSING_BUFF, 0, 1, GAO_SHAN_BUFF);
        }
        if (ACTIVE_YANGCHUN && s.preparing_skill == INVALID_SLOT && s.casting_skill == INVALID_SLOT) {
            return Reject(e, ActionBlockReason::PREPARING, 0, 1, GONG);
        }
        if (IsZhengLv(K) && !Data().Has(ZHENG_LV_HE_MING)) {
            return Reject(e, ActionBlockReason::MISSING_TALENT, 0, 1, ZHENG_LV_BUFF);
        }
        if (Charged(K) && !s.skill_energy[K]) {
            return Reject(e, ActionBlockReason::CHARGES, 0, 1, K);
        }
        if (K == YI_XING && !shadow_count_) {
            return Reject(e, ActionBlockReason::MISSING_BUFF, 0, 1, YING_ZI);
        }
        if (K == SHU_YING && shadow_count_ == 6) {
            return Reject(e, ActionBlockReason::CHARGES, 6, 5, SHU_YING);
        }
        const auto RANGE = Range(K);
        if (s.target_distance > RANGE) {
            return Reject(e, ActionBlockReason::DISTANCE, s.target_distance, RANGE, K);
        }
        [[maybe_unused]] const auto EFFECT_TIMER = CastEffectTimer<Engine>();
        const auto                  PREVIOUS     = s.last_skill;
        s.last_skill                             = K;
        if (UsesGcd(K)) {
            Gcd(e, 0, TimingNow().gcd);
        }
        switch (K) {
            case GONG:
            case BIAN_GONG:
                if (s.self_buff_stacks[WEAPON_CW_BUFF]) {
                    GongDamage(e, K, true);
                } else {
                    SetPrepare(s, e.Log(), K, s.now + TimingNow().gong, true);
                    e.Timers().ArmAt(prepare_[K], s.now + TimingNow().gong);
                }
                break;
            case SHANG:
            case JUE:
                AddXianFeng(e);
                Enchants(e);
                Equipment(e);
                Water(e);
                Hit(e, K == SHANG ? E_SHANG : E_JUE, Snapshot(), true);
                AddQuFeng(e, (K == SHANG ? 2 : 1) + static_cast<int>(style_));
                AddDot(e, K == SHANG ? SHANG_DOT : JUE_DOT);
                break;
            case ZHI:
            case BIAN_ZHI: StartZhi(e, K); break;
            case YU:
                ConsumeCharge(e, YU);
                AddQuFeng(e, 4 + static_cast<int>(style_));
                AddXianFeng(e);
                Enchants(e);
                Equipment(e);
                Water(e);
                if (Data().HasEquipment(WEAPON_CW) && Proc(307, 1024)) {
                    Hit(e, E_YU_CW, Snapshot(), true);
                }
                if (Data().Has(SHI_XIANG)) {
                    ShortenCharge(e, YU, 80);
                    if (Data().Has(LIU_ZHAO_TALENT)) {
                        ShortenCharge(e, ZHI, 80);
                    }
                }
                if (Data().Has(MING_JIN_TALENT)) {
                    Aura(e, MING_JIN, (s.self_buff_stacks[MING_JIN] ? s.self_buff_expires_at[MING_JIN] - s.now : 0) + 128, 1);
                }
                Hit(e, E_YU, Snapshot());
                break;
            case GAO_SHAN:
            case YANG_CHUN: CastStyle(e, K, PREVIOUS); break;
            case SHU_YING:
                Gcd(e, 4, TimingNow().short_gcd);
                ConsumeCharge(e, SHU_YING);
                Zero(e, E_ZERO_SHU_YING);
                Wrist(e);
                AddShadow(e);
                if (Proc(Data().Has(KE_MENG) ? 100 : 20, 100)) {
                    AddShadow(e);
                }
                break;
            case YI_XING:
                MarkXianFeng(e);
                RemoveShadow(e, OldestShadow());
                break;
            case GU_YING: CastGuYing(e); break;
            case ZHENG_LV1:
            case ZHENG_LV2:
            case ZHENG_LV3:
                Cooldown(e, K, s.now + 960);
                Zero(e, static_cast<Effect>(E_ZERO_ZHENG_LV1 + K - ZHENG_LV1));
                Wrist(e);
                Aura(e, ZHENG_LV_BUFF, 320, 1);
                AddZhiYin(e);
                zhiyin_level_ = K - ZHENG_LV1 + 1;
                break;
            case WU_YIN:
                Gcd(e, 6, TimingNow().wuyin);
                Enchants(e);
                Equipment(e);
                Water(e);
                Hit(e, E_WU_YIN, Snapshot());
                break;
        }
        return true;
    }

    template <typename Engine>
    void OnTimer(Engine &e, TimerHandle h, DeadlineQueue::TimerId)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::OnTimer");
        auto                       &s           = e.MutableState();
        switch (h.kind) {
            case TimerKind::SKILL_PREPARE:
                if (s.preparing_skill != h.slot) {
                    return;
                }
                SetPrepare(s, e.Log(), h.slot, 0, false);
                if (h.slot == GONG || h.slot == BIAN_GONG) {
                    GongDamage(e, h.slot, false);
                } else if (IsStyleSkill(h.slot)) {
                    CompleteStyle(e, h.slot);
                }
                break;
            case TimerKind::SKILL_CASTING: TickZhi(e); break;
            case TimerKind::SKILL_COOLDOWN:
                if (Charged(h.slot)) {
                    RecoverCharge(e, h.slot);
                }
                break;
            case TimerKind::BUFF_INTERVAL:
                if (IsDot(h.slot)) {
                    TickDot(e, h.slot);
                } else if (h.slot == JING_MIAO_BUFF) {
                    Aura(e, JING_MIAO_BUFF, 80, 1);
                    e.Timers().ArmAt(tick_[JING_MIAO_BUFF], s.now + 320 + Integer(0, 6));
                }
                break;
            case TimerKind::BUFF_DURATION: Expire(e, h.slot); break;
            case TimerKind::TARGET_CHANGE: target_timeline_.Dispatch(e, Data().config.target_events); break;
            case TimerKind::FIELD: TickShadow(e, h.slot); break;
            default: break;
        }
    }

    value_t Reduce(const DamageIntent &hit) const { return Data().Reduce(hit); }

private:
    static constexpr tick_t Forever = std::numeric_limits<tick_t>::max();

    struct GuSnapshot
    {
        std::array<tick_t, SKILL_COUNT> cooldown{}, recharge{};
        std::array<int, SKILL_COUNT>    charges{};
        std::array<tick_t, 4>           groups{};
    };

    template <typename Engine>
    static bool Reject(Engine &e, ActionBlockReason reason, double actual, double required, Slot subject)
    {
        if constexpr (Engine::debug_enabled) {
            e.RejectAction(reason, actual, required, subject);
        }
        return false;
    }

    const Timing &TimingNow() const { return Data().timing[timing_index_]; }

    bool Charged(Slot k) const { return k == YU || k == SHU_YING || (IsZhi(k) && Data().Has(LIU_ZHAO_TALENT)); }

    static Slot ChargeSlot(Slot k) { return k == BIAN_ZHI ? static_cast<Slot>(ZHI) : k; }

    tick_t Recharge(Slot k) const { return k == YU ? Data().yu_cooldown : k == SHU_YING ? 320 : 128; }

    double Range(Slot k) const
    {
        if (IsStyleSkill(k) || k == YI_XING || k == GU_YING) {
            return 1000;
        }
        if ((k == GONG || k == BIAN_GONG) && (Data().config.recipes[0] & GONG_RANGE)) {
            return 22;
        }
        if (IsZhi(k) && (Data().config.recipes[2] & ZHI_RANGE)) {
            return 22;
        }
        return 20;
    }

    static double Unit(std::uint64_t key)
    {
        if constexpr (requires { Rng::UnitSample(key); }) {
            return Rng::UnitSample(key);
        } else {
            return static_cast<double>(key >> 11) * 0x1.0p-53;
        }
    }

    int Integer(int low, int high)
    {
        if constexpr (requires { rng_.Integer(low, high); }) {
            return rng_.Integer(low, high);
        } else {
            return low + static_cast<int>(rng_.Next() % static_cast<unsigned>(high - low + 1));
        }
    }

    bool Proc(int numerator, int denominator) { return Integer(1, denominator) <= numerator; }

    AuraSnapshot Snapshot() const { return { profile_, miaoyi_power_ + jingmiao_power_ }; }

    template <typename Engine>
    void Hit(Engine &e, Effect effect, AuraSnapshot snap, bool you_ren = false)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::Hit");
        const auto                  KEY         = rng_.Next();
        const bool                  CRITICAL = Unit(KEY) < Data().critical_chance[ChanceProfile(snap.profile)][effect];
        const bool SHOES = effect <= E_WU_YIN || effect == E_GONG_CW || effect == E_BIAN_GONG_CW || effect == E_YU_CW;
        if (SHOES && CRITICAL && Data().HasEquipment(ENCHANT_SHOES) && e.GetState().now >= shoes_ready_at_) {
            shoes_ready_at_ = e.GetState().now + 160;
            Hit(e, E_SHOES, Snapshot());
        }
        if (you_ren && CRITICAL && Data().HasTeam(YOU_REN)) {
            Aura(e, YOU_REN_BUFF, 80, 1);
        }
        const auto &f = Data().formulas[effect];
        e.Log().Record(e.GetState().now, f.skill, 0, f.level, KEY, snap.extra_power, 0, CRITICAL ? RollResult::DOUBLE : RollResult::HIT, effect, snap.profile, f.sub);
    }

    template <typename Engine>
    void Zero(Engine &e, Effect effect)
    {
        e.Log().Record(e.GetState().now, Data().formulas[effect].skill, INVALID_SLOT, 0, 0, 0, 0, RollResult::HIT, effect, 0);
    }

    template <typename Engine>
    void Gcd(Engine &e, Slot group, tick_t duration)
    {
        auto &s = e.MutableState();
        SetGcd(s, e.Log(), group, s.now + duration);
        e.Timers().ArmAt(gcd_[group], s.now + duration);
    }

    template <typename Engine>
    void Cooldown(Engine &e, Slot k, tick_t deadline)
    {
        SetSkillCooldown(e.MutableState(), e.Log(), k, deadline);
        if (deadline > e.GetState().now) {
            e.Timers().ArmAt(cooldown_[k], deadline);
        } else {
            e.Timers().Cancel(cooldown_[k]);
        }
    }

    template <typename Engine>
    void PublishCharge(Engine &e, Slot k, int count)
    {
        auto &s = e.MutableState();
        SetSkillEnergy(s, e.Log(), k, count);
        SetSkillCooldown(s, e.Log(), k, count ? 0 : recharge_at_[k]);
        if (k == ZHI) {
            SetSkillEnergy(s, e.Log(), BIAN_ZHI, count);
            SetSkillCooldown(s, e.Log(), BIAN_ZHI, count ? 0 : recharge_at_[k]);
        }
        if (count < 3) {
            e.Timers().ArmAt(cooldown_[k], recharge_at_[k]);
        } else {
            recharge_at_[k] = 0;
            e.Timers().Cancel(cooldown_[k]);
        }
    }

    template <typename Engine>
    void ConsumeCharge(Engine &e, Slot skill)
    {
        const Slot K     = ChargeSlot(skill);
        const int  count = e.GetState().skill_energy[K];
        if (count == 3) {
            recharge_at_[K] = e.GetState().now + Recharge(K);
        }
        PublishCharge(e, K, count - 1);
    }

    template <typename Engine>
    void RecoverCharge(Engine &e, Slot k)
    {
        const int count = std::min(3, e.GetState().skill_energy[k] + 1);
        recharge_at_[k] = e.GetState().now + Recharge(k);
        PublishCharge(e, k, count);
    }

    template <typename Engine>
    void ShortenCharge(Engine &e, Slot k, tick_t delta)
    {
        const auto &s = e.GetState();
        if (s.skill_energy[k] == 3) {
            return;
        }
        recharge_at_[k] -= delta;
        int count        = s.skill_energy[k];
        if (recharge_at_[k] <= s.now) {
            ++count;
            recharge_at_[k] += Recharge(k);
        }
        PublishCharge(e, k, count);
    }

    void SetStyleGroups(State &s)
    {
        s.skill_gcd_group[GAO_SHAN]  = style_ == Style::GAO_SHAN_LIU_SHUI ? 2 : 1;
        s.skill_gcd_group[YANG_CHUN] = style_ == Style::YANG_CHUN_BAI_XUE ? 3 : 1;
    }

    template <typename Engine>
    void Aura(Engine &e, Slot b, tick_t duration, int stacks)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::Aura");
        auto                       &s           = e.MutableState();
        const auto                  EXPIRY      = duration == Forever ? Forever : s.now + duration;
        SetBuff(s, e.Log(), false, b, stacks || b == ZHI_YIN_HE_MING ? EXPIRY : 0, 0, stacks);
        UpdateProfile(s, b);
        if (duration != Forever && (stacks || b == ZHI_YIN_HE_MING)) {
            e.Timers().ArmAt(expire_[b], EXPIRY);
        } else {
            e.Timers().Cancel(expire_[b]);
        }
    }

    void UpdateProfile(const State &s, Slot b)
    {
        const int N = s.self_buff_stacks[b];
        switch (b) {
            case YUN_HAN: profile_ = static_cast<std::uint16_t>((profile_ & ~YunHanMask) | N); break;
            case CAN_LIAN:
                profile_ = static_cast<std::uint16_t>((profile_ & ~CanLianProfile) | ((N != 0) ? CanLianProfile : 0));
                break;
            case MING_JIN:
                profile_ = static_cast<std::uint16_t>((profile_ & ~MingJinProfile) | ((N != 0) ? MingJinProfile : 0));
                break;
            case YANG_CHUN_BUFF:
                profile_ = static_cast<std::uint16_t>((profile_ & ~YangChunMask) | (N * YangChunUnit));
                break;
            case SET_ATTRIBUTE_BUFF:
                profile_ = static_cast<std::uint16_t>((profile_ & ~SetProfile) | ((N != 0) ? SetProfile : 0));
                break;
            case YOU_REN_BUFF:
                profile_ = static_cast<std::uint16_t>((profile_ & ~YouRenProfile) | ((N != 0) ? YouRenProfile : 0));
                break;
            case GAO_SHAN_BUFF: timing_index_ = static_cast<unsigned int>(N != 0); break;
            case JING_MIAO_BUFF: jingmiao_power_ = (N != 0) ? 205 : 0; break;
            case WATER_BUFF: profile_ = static_cast<std::uint16_t>((profile_ & ~WaterMask) | (N * WaterUnit)); break;
            case BELT_BUFF:
                if (N == 0) {
                    profile_ = static_cast<std::uint16_t>(profile_ & ~BeltMask);
                }
                break;
        }
    }

    template <typename Engine>
    void ClearAura(Engine &e, Slot b)
    {
        SetBuff(e.MutableState(), e.Log(), false, b, 0, 0, 0);
        UpdateProfile(e.GetState(), b);
        e.Timers().Cancel(expire_[b]);
    }

    template <typename Engine>
    void Expire(Engine &e, Slot b)
    {
        if (IsDot(b)) {
            SetBuff(e.MutableState(), e.Log(), true, b, 0, 0, 0);
            e.Timers().Cancel(tick_[b]);
            return;
        }
        if (b == ZHI_YIN_MIAO_YI) {
            miaoyi_power_ -= static_cast<int>(102.5 * (e.GetState().self_buff_stacks[b] + 1));
        }
        ClearAura(e, b);
        if (b == GU_YING_BUFF) {
            Gcd(e, 5, 2400);
        }
        if (b == ZHENG_LV_BUFF) {
            zhiyin_level_ = 3;
            FinishZhiYin(e);
        }
        if (b == ZHI_YIN_HE_MING) {
            zhiyin_active_ = false;
            zhiyin_level_  = 3;
        }
    }

    template <typename Engine>
    void AddQuFeng(Engine &e, int add)
    {
        const int N = e.GetState().self_buff_stacks[QU_FENG] + add;
        Aura(e, QU_FENG, Forever, N >= 9 ? 0 : N);
        if (N == 9) {
            PoZhaoHits(e);
            if (e.GetState().self_buff_stacks[ZHENG_LV_BUFF]) {
                AddZhiYin(e);
            }
        }
    }

    template <typename Engine>
    void PoZhaoHits(Engine &e)
    {
        if (Data().Has(LIU_ZHAO_TALENT)) {
            const int N = e.GetState().self_buff_stacks[LIU_ZHAO];
            for (int i = 0; i < N; ++i) {
                Hit(e, E_LIU_ZHAO_SURPLUS, Snapshot());
            }
            if (N) {
                ClearAura(e, LIU_ZHAO);
            }
        }
        if (Data().Has(ZHENG_MING)) {
            Hit(e, E_SURPLUS3, Snapshot());
        }
        for (const auto effect : { E_SURPLUS0, E_SURPLUS1, E_SURPLUS2 }) {
            Hit(e, effect, Snapshot());
        }
    }

    template <typename Engine>
    void AddXianFeng(Engine &e)
    {
        if (Data().Has(XIAN_FENG_TALENT)) {
            Aura(e, XIAN_FENG, 160, std::min(5, e.GetState().self_buff_stacks[XIAN_FENG] + 1));
        }
    }

    template <typename Engine>
    void MarkXianFeng(Engine &e)
    {
        if (Data().Has(XIAN_FENG_TALENT) && e.GetState().self_buff_stacks[XIAN_FENG]) {
            Aura(e, XIAN_FENG_MARK, 53, 1);
        }
    }

    template <typename Engine>
    void ClearXianFeng(Engine &e)
    {
        if (e.GetState().self_buff_stacks[XIAN_FENG_MARK] && e.GetState().self_buff_stacks[XIAN_FENG]) {
            ClearAura(e, XIAN_FENG);
        }
    }

    template <typename Engine>
    void Equipment(Engine &e)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::Equipment");
        if (Data().HasEquipment(SET_ATTRIBUTE) && Proc(10, 100)) {
            Aura(e, SET_ATTRIBUTE_BUFF, 96, 1);
        }
        if (!Data().HasEquipment(WEAPON_CW)) {
            return;
        }
        const bool PROC = Proc(25, 1024);
        if (!PROC || e.GetState().now < weapon_ready_at_) {
            return;
        }
        weapon_ready_at_ = e.GetState().now + 480;
        Aura(e, WEAPON_CW_BUFF, 96, 1);
        for (const Slot K : { Slot{ YU }, Slot{ ZHI } }) {
            if (Charged(K)) {
                recharge_at_[K] = e.GetState().now;
                // Legacy sets recharge to zero; the next skill trigger restores one charge.
                e.Timers().ArmAt(cooldown_[K], recharge_at_[K]);
                if (!e.GetState().skill_energy[K]) {
                    SetSkillCooldown(e.MutableState(), e.Log(), K, recharge_at_[K]);
                    if (K == ZHI) {
                        SetSkillCooldown(e.MutableState(), e.Log(), BIAN_ZHI, recharge_at_[K]);
                    }
                }
            }
        }
    }

    template <typename Engine>
    void Wrist(Engine &e)
    {
        if (Data().HasEquipment(ENCHANT_WRIST) && e.GetState().now >= wrist_ready_at_ && Proc(10, 100)) {
            wrist_ready_at_ = e.GetState().now + 160;
            Hit(e, E_WRIST, Snapshot());
        }
    }

    template <typename Engine>
    void Enchants(Engine &e)
    {
        Wrist(e);
        if (Data().HasEquipment(ENCHANT_BELT) && e.GetState().now >= belt_ready_at_ && Proc(20, 100)) {
            belt_ready_at_       = e.GetState().now + 480;
            const unsigned BONUS = Proc(70, 100) ? 2 : 1;
            profile_             = static_cast<std::uint16_t>((profile_ & ~BeltMask) | (BONUS * BeltUnit));
            Aura(e, BELT_BUFF, 128, 1);
        }
    }

    template <typename Engine>
    void Water(Engine &e)
    {
        if (Data().HasEquipment(WEAPON_WATER)) {
            Aura(e, WATER_BUFF, 96, std::min(10, e.GetState().self_buff_stacks[WATER_BUFF] + 1));
        }
    }

    template <typename Engine>
    void GongDamage(Engine &e, Slot k, bool cw)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::GongDamage");
        if (!cw) {
            AddQuFeng(e, k == GONG ? 3 : 4);
        }
        AddXianFeng(e);
        if (!cw && k == BIAN_GONG && Data().Has(ZHI_ZHI)) {
            for (const Slot B : { Slot{ SHANG_DOT }, Slot{ JUE_DOT } }) {
                if (e.GetState().target_buff_stacks[B]) {
                    AddDot(e, B);
                }
            }
            Gcd(e, 1, 0);
        }
        Enchants(e);
        Equipment(e);
        if (cw) {
            if (e.GetState().self_buff_stacks[WEAPON_CW_BUFF]) {
                AddDot(e, SHEN_BING_GONG);
            }
            Water(e);
            PoZhaoHits(e);
            if (e.GetState().self_buff_stacks[ZHENG_LV_BUFF]) {
                AddZhiYin(e);
            }
            Aura(e, QU_FENG, Forever, 0);
        } else {
            Water(e);
        }
        Hit(e, cw ? (k == GONG ? E_GONG_CW : E_BIAN_GONG_CW) : (k == GONG ? E_GONG : E_BIAN_GONG), Snapshot(), !cw);
        ClearAura(e, YANG_CHUN_BUFF);
    }

    template <typename Engine>
    void StartZhi(Engine &e, Slot k)
    {
        auto &s = e.MutableState();
        if (Charged(k)) {
            ConsumeCharge(e, k);
        }
        channel_index_ = 0;
        SetCasting(s, e.Log(), k, s.now + (3 * TimingNow().zhi), true);
        e.Timers().ArmAt(channel_, s.now + TimingNow().zhi);
        AddQuFeng(e, k == ZHI ? 5 : 6);
        if (k == ZHI) {
            ClearAura(e, YANG_CHUN_BUFF);
        }
        AddXianFeng(e);
        channel_xianfeng_ = s.self_buff_stacks[XIAN_FENG];
        if (Data().Has(HAO_QING)) {
            ZhiDamage(e, k);
        }
        ClearXianFeng(e);
    }

    template <typename Engine>
    void ZhiDamage(Engine &e, Slot k)
    {
        if (e.GetState().self_buff_stacks[XIAN_FENG_MARK]) {
            for (int i = 0; i < channel_xianfeng_; ++i) {
                Hit(e, E_XIAN_FENG, Snapshot());
            }
        }
        Equipment(e);
        Water(e);
        if (Data().Has(LIU_ZHAO_TALENT)) {
            const auto &s = e.GetState();
            const int   N = (s.target_buff_stacks[SHANG_DOT] > 0) + (s.target_buff_stacks[JUE_DOT] > 0);
            for (int i = 0; i < N; ++i) {
                Hit(e, E_LIU_ZHAO, Snapshot());
            }
            if (N) {
                Aura(e, LIU_ZHAO, 400, std::min(32, s.self_buff_stacks[LIU_ZHAO] + N));
            }
        }
        Enchants(e);
        Hit(e, k == ZHI ? E_ZHI : E_BIAN_ZHI, Snapshot(), true);
    }

    template <typename Engine>
    void TickZhi(Engine &e)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::TickZhi");
        auto                       &s           = e.MutableState();
        const auto                  K           = s.casting_skill;
        if (!IsZhi(K)) {
            return;
        }
        ZhiDamage(e, K);
        if (++channel_index_ < 3) {
            SetCasting(s, e.Log(), K, s.now + ((3 - channel_index_) * TimingNow().zhi), true);
            e.Timers().ArmAt(channel_, s.now + TimingNow().zhi);
        } else {
            SetCasting(s, e.Log(), K, 0, false);
            if (K == ZHI) {
                ClearAura(e, YANG_CHUN_BUFF);
            } else {
                ClearXianFeng(e);
            }
        }
    }

    template <typename Engine>
    void Interrupt(Engine &e)
    {
        auto &s = e.MutableState();
        if constexpr (Engine::debug_enabled) {
            e.NoteChannelInterrupt(s.casting_skill);
        }
        SetCasting(s, e.Log(), s.casting_skill, 0, false);
        e.Timers().Cancel(channel_);
        channel_index_ = 0;
        ClearAura(e, YANG_CHUN_BUFF);
    }

    template <typename Engine>
    void AddDot(Engine &e, Slot b)
    {
        auto      &s      = e.MutableState();
        const auto next   = s.target_buff_stacks[b] ? s.target_buff_tick_at[b] : s.now + TimingNow().dot;
        const int  count  = b == SHEN_BING_GONG ? 10 : 6;
        const auto EXPIRY = next + static_cast<int>(48 * TimingNow().haste * (count - 1));
        const int  STACKS = b == SHEN_BING_GONG ? std::min(3, s.target_buff_stacks[b] + 1) : 1;
        dot_snapshot_[b]  = Snapshot();
        dot_interval_[b]  = TimingNow().dot;
        SetBuff(s, e.Log(), true, b, EXPIRY, next, STACKS);
        e.Timers().ArmAt(tick_[b], next);
        e.Timers().ArmAt(expire_[b], EXPIRY);
    }

    template <typename Engine>
    void TickDot(Engine &e, Slot b)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::TickDot");
        auto                       &s           = e.MutableState();
        if (!s.target_buff_stacks[b]) {
            return;
        }
        const auto effect = b == SHANG_DOT ? E_SHANG_DOT
                            : b == JUE_DOT ? E_JUE_DOT
                                           : static_cast<Effect>(E_SHEN_BING + s.target_buff_stacks[b] - 1);
        Hit(e, effect, dot_snapshot_[b]);
        const auto next = s.now + dot_interval_[b];
        SetBuff(s, e.Log(), true, b, s.target_buff_expires_at[b], next, s.target_buff_stacks[b]);
        if (next <= s.target_buff_expires_at[b]) {
            e.Timers().ArmAt(tick_[b], next);
        }
    }

    template <typename Engine>
    void CastStyle(Engine &e, Slot k, Slot previous)
    {
        auto       &s      = e.MutableState();
        const Style WANTED = k == GAO_SHAN ? Style::GAO_SHAN_LIU_SHUI : Style::YANG_CHUN_BAI_XUE;
        if (style_ == WANTED) {
            if (k == GAO_SHAN) {
                Zero(e, E_ZERO_GAO_SHAN);
                Gcd(e, 2, 800);
                Wrist(e);
                Aura(e, GAO_SHAN_BUFF, 192, 1);
            } else {
                Gcd(e, 3, Data().yangchun_cooldown);
                Equipment(e);
                Water(e);
                Enchants(e);
                Hit(e, s.target_life - .01 < .5 ? E_YANG_CHUN_EXECUTE : E_YANG_CHUN, Snapshot());
                Aura(e, YANG_CHUN_BUFF, 96, std::min(3, s.self_buff_stacks[YANG_CHUN_BUFF] + 1));
            }
            return;
        }
        Zero(e, k == GAO_SHAN ? E_ZERO_GAO_SHAN : E_ZERO_YANG_CHUN);
        Gcd(e, 1, 48);
        if (k == YANG_CHUN) {
            ClearAura(e, GAO_SHAN_BUFF);
        } else if (!s.self_buff_stacks[ZHENG_LV_BUFF]) {
            Aura(e, QU_FENG, Forever, 0);
        }
        const bool PREPARE = k == GAO_SHAN ? (previous == GONG || previous == ZHI) : (previous == BIAN_GONG || previous == BIAN_ZHI);
        if (PREPARE) {
            SetPrepare(s, e.Log(), k, s.now + 6, true);
            e.Timers().ArmAt(prepare_[k], s.now + 6);
        } else {
            CompleteStyle(e, k);
        }
    }

    template <typename Engine>
    void CompleteStyle(Engine &e, Slot k)
    {
        style_ = k == GAO_SHAN ? Style::GAO_SHAN_LIU_SHUI : Style::YANG_CHUN_BAI_XUE;
        SetStyleGroups(e.MutableState());
        if (Data().Has(CAN_LIAN_TALENT)) {
            if (k == YANG_CHUN) {
                Aura(e, CAN_LIAN, Forever, 1);
            } else {
                ClearAura(e, CAN_LIAN);
            }
        }
        MarkXianFeng(e);
    }

    Slot OldestShadow() const
    {
        Slot result = INVALID_SLOT;
        for (Slot i = 0; i < shadows_.size(); ++i) {
            if ((shadows_[i].expires_at != 0) && (result == INVALID_SLOT || shadows_[i].order < shadows_[result].order))
            {
                result = i;
            }
        }
        return result;
    }

    template <typename Engine>
    void PublishShadows(Engine &e)
    {
        tick_t expiry = Forever;
        tick_t next   = Forever;
        for (const auto &shadow : shadows_) {
            if (shadow.expires_at) {
                expiry = std::min(expiry, shadow.expires_at);
                next   = std::min(next, shadow.tick_at);
            }
        }
        SetBuff(e.MutableState(), e.Log(), false, YING_ZI, shadow_count_ ? expiry : 0, shadow_count_ ? next : 0, shadow_count_);
    }

    template <typename Engine>
    void AddShadow(Engine &e)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::AddShadow");
        // Legacy takes the seventh shadow's snapshot before removing the oldest.
        if (Data().Has(YUN_HAN_TALENT)) {
            Aura(e, YUN_HAN, 1600, shadow_count_ + 1);
        }
        const auto SNAP = Snapshot();
        if (shadow_count_ == 6) {
            RemoveShadow(e, OldestShadow());
        }
        Slot slot = 0;
        while (shadows_[slot].expires_at) {
            ++slot;
        }
        ++shadow_count_;
        if (Data().Has(YUN_HAN_TALENT)) {
            Aura(e, YUN_HAN, 1600, shadow_count_);
        }
        const auto NOW = e.GetState().now;
        shadows_[slot] = { NOW + 400, NOW + TimingNow().shadow, shadow_order_++, SNAP };
        e.Timers().ArmAt(shadow_timer_[slot], shadows_[slot].tick_at);
        PublishShadows(e);
    }

    template <typename Engine>
    void RemoveShadow(Engine &e, Slot slot)
    {
        if (slot == INVALID_SLOT) {
            return;
        }
        shadows_[slot] = {};
        --shadow_count_;
        e.Timers().Cancel(shadow_timer_[slot]);
        if (Data().Has(YUN_HAN_TALENT)) {
            Aura(e, YUN_HAN, 1600, shadow_count_);
        }
        PublishShadows(e);
    }

    template <typename Engine>
    void TickShadow(Engine &e, Slot slot)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("mo_wen::TickShadow");
        auto                       &shadow      = shadows_[slot];
        const auto                  NOW         = e.GetState().now;
        if (!shadow.expires_at) {
            return;
        }
        if (shadow.tick_at == NOW) {
            Hit(e, E_YING_ZI, shadow.snapshot);
            shadow.tick_at = NOW + TimingNow().shadow; // Legacy shadows use live haste at each tick.
        }
        if (shadow.expires_at == NOW) {
            RemoveShadow(e, slot);
        } else {
            e.Timers().ArmAt(shadow_timer_[slot], std::min(shadow.expires_at, shadow.tick_at));
            PublishShadows(e);
        }
    }

    template <typename Engine>
    void CastGuYing(Engine &e)
    {
        auto &s = e.MutableState();
        Zero(e, E_ZERO_GU_YING);
        if (!s.self_buff_stacks[GU_YING_BUFF]) {
            Wrist(e);
            for (Slot k = GONG; k <= ZHENG_LV3; ++k) {
                gu_snapshot_.cooldown[k] = std::max(0, s.skill_ready_at[k] - s.now);
                gu_snapshot_.recharge[k] = recharge_at_[k] ? std::max(0, recharge_at_[k] - s.now) : 0;
                gu_snapshot_.charges[k]  = s.skill_energy[k];
            }
            for (Slot g = 1; g <= 4; ++g) {
                gu_snapshot_.groups[g - 1] = std::max(0, s.gcd_ready_at[g] - s.now);
            }
            Aura(e, GU_YING_BUFF, 112, 1);
            Gcd(e, 5, 16);
        } else {
            ClearAura(e, GU_YING_BUFF);
            for (Slot k = GONG; k <= ZHENG_LV3; ++k) {
                if (k == GU_YING || k == BIAN_ZHI) {
                    continue;
                }
                if (Charged(k)) {
                    recharge_at_[k] = s.now + gu_snapshot_.recharge[k];
                    PublishCharge(e, k, gu_snapshot_.charges[k]);
                } else {
                    Cooldown(e, k, s.now + gu_snapshot_.cooldown[k]);
                }
            }
            for (Slot g = 1; g <= 4; ++g) {
                Gcd(e, g, gu_snapshot_.groups[g - 1]);
            }
            Gcd(e, 5, 2400);
        }
    }

    template <typename Engine>
    void AddZhiYin(Engine &e)
    {
        auto     &s    = e.MutableState();
        const int N    = zhiyin_active_ ? s.self_buff_stacks[ZHI_YIN_HE_MING] + 1 : 0;
        zhiyin_active_ = true;
        Aura(e, ZHI_YIN_HE_MING, 320, N);
        if (N == 5) {
            ClearAura(e, ZHENG_LV_BUFF);
            FinishZhiYin(e);
            return;
        }
        constexpr std::array SEQUENCE{ 2, 3, 5, 6, 4 };
        const int            QUFENG = SEQUENCE[N];
        Aura(e, QU_FENG, Forever, QUFENG);
        if (QUFENG == 5) {
            zhiyin_last_gong_ = true;
        } else if (zhiyin_last_gong_) {
            zhiyin_last_gong_ = false;
            const auto END    = s.now + Integer(8, 16);
            // A synthetic prepare owner exposes this old short lock to the debugger.
            SetPrepare(s, e.Log(), ZHI_YIN_DAMAGE, END, true);
            e.Timers().ArmAt(prepare_[ZHI_YIN_DAMAGE], END);
        }
    }

    template <typename Engine>
    void FinishZhiYin(Engine &e)
    {
        if (!zhiyin_active_) {
            return;
        }
        zhiyin_active_ = false;
        ClearAura(e, ZHI_YIN_HE_MING);
        auto     &s       = e.MutableState();
        const int OLD     = s.self_buff_stacks[ZHI_YIN_MIAO_YI];
        const int STACKS  = std::min(3, OLD + 4 - zhiyin_level_);
        miaoyi_power_    += static_cast<int>(102.5 * (STACKS - OLD + 1));
        Aura(e, ZHI_YIN_MIAO_YI, 240, STACKS);
        for (int i = 0; i < 5; ++i) {
            Hit(e, static_cast<Effect>(E_ZHI_YIN + zhiyin_level_ - 1), Snapshot());
        }
    }

    std::shared_ptr<const Prepared>                 prepared_;
    Rng                                             rng_;
    std::array<DeadlineQueue::TimerId, SKILL_COUNT> cooldown_{}, prepare_{};
    std::array<DeadlineQueue::TimerId, BUFF_COUNT>  tick_{}, expire_{};
    std::array<DeadlineQueue::TimerId, 7>           gcd_{};
    std::array<DeadlineQueue::TimerId, 6>           shadow_timer_{};
    DeadlineQueue::TimerId                          channel_{};
    std::array<AuraSnapshot, BUFF_COUNT>            dot_snapshot_{};
    std::array<tick_t, BUFF_COUNT>                  dot_interval_{};
    std::array<tick_t, SKILL_COUNT>                 recharge_at_{};
    std::array<Shadow, 6>                           shadows_{};
    GuSnapshot                                      gu_snapshot_{};
    Style                                           style_        = Style::GAO_SHAN_LIU_SHUI;
    std::uint16_t                                   profile_      = 0;
    unsigned                                        timing_index_ = 0;
    int                                             miaoyi_power_ = 0, jingmiao_power_ = 0;
    int            channel_index_ = 0, channel_xianfeng_ = 0, shadow_count_ = 0, zhiyin_level_ = 1;
    std::uint64_t  shadow_order_    = 0;
    tick_t         weapon_ready_at_ = 480, shoes_ready_at_ = 0, wrist_ready_at_ = 0, belt_ready_at_ = 0;
    bool           zhiyin_active_ = false, zhiyin_last_gong_ = false;
    TargetTimeline target_timeline_;
};

using Rules = BasicRules<>;

class DamageReducer
{
public:
    explicit DamageReducer(std::shared_ptr<const Prepared> prepared) : prepared_(std::move(prepared)) { }

    void Setup(std::size_t damage_capacity) { prepared_->ValidateCapacity(damage_capacity); }

    template <class SourceRules>
    void Bind(const SourceRules &)
    {
    }

    value_t Reduce(const DamageIntent &hit) const { return prepared_->Reduce(hit); }

    value_t Reduce(DamageGrouping::Key key) const
    {
        return prepared_->Reduce(key.profile, key.effect_roll / 2, (key.effect_roll & 1) != 0, key.extra_power);
    }

private:
    std::shared_ptr<const Prepared> prepared_;
};

struct Module
{
    static constexpr bool replay_strain = true;
    using DamageGrouping                = mo_wen::DamageGrouping;
    using Rules                         = mo_wen::Rules;
    using DamageReducer                 = mo_wen::DamageReducer;

    static auto Prepare(const Config &config) { return mo_wen::Prepare(config); }
};

} // namespace JX3DPS::runtime::mo_wen
#endif
