#ifndef JX3DPS_TAI_XU_RUNTIME_RULES_HPP
#define JX3DPS_TAI_XU_RUNTIME_RULES_HPP

#include "runtime_data.hpp"

namespace JX3DPS::runtime::tai_xu {

struct SplitMixRolls
{
    std::uint64_t state = 0;

    void Reset(std::uint64_t seed) { state = seed; }

    std::uint64_t Next()
    {
        auto z = (state += 0x9e3779b97f4a7c15ULL);
        z      = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z      = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }
};

struct FieldInstance
{
    Slot          skill      = INVALID_SLOT;
    tick_t        expires_at = 0, tick_at = 0;
    std::uint64_t order                                   = 0;
    bool          operator==(const FieldInstance &) const = default;
};

struct NaturalQidianPhase
{
    tick_t operator()(std::uint64_t seed) const
    {
        // Legacy uses a separate integer RNG for the initial 0..15-frame phase.
        // Keep phase sampling independent of critical/proc rolls as well.
        SplitMixRolls phases{ seed ^ 0x6a09e667f3bcc909ULL };
        return static_cast<tick_t>(phases.Next() & 15);
    }
};

// Rng is a compile-time policy so tests can inject old rolls without adding
// a replay/debug branch to the batch loop.
template <typename Rng = SplitMixRolls, typename QidianPhase = NaturalQidianPhase>
class BasicRules
{
public:
    static constexpr bool owns_last_skill = true;

    explicit BasicRules(std::shared_ptr<const Prepared> prepared = Prepare(), Rng rng = {}, QidianPhase phase = {}) :
        prepared_(std::move(prepared)), rng_(std::move(rng)), qidian_phase_(std::move(phase))
    {
        if (!prepared_) {
            throw std::invalid_argument("missing prepared Tai Xu configuration");
        }
    }

    const Prepared &Data() const { return *prepared_; }

    std::uint16_t CurrentProfile() const { return profile_; }

    int CurrentCriticalBonus() const { return you_ren_crit_basis_points_; }

    const team::Modifiers &CurrentTeamModifiers() const { return team_modifiers_; }

    const AttributeVersions<AttributeVersion> &Versions() const { return versions_; }

    const std::array<FieldInstance, 3> &Fields() const { return fields_; }

    template <typename Engine>
    void Setup(Engine &e)
    {
        target_timeline_.Setup(e, Data().config.target_events);
        versions_.Reserve(Data().config.attribute_version_capacity);
        auto &s = e.MutableState();
        if (s.skill_ready_at.size() != SKILL_COUNT || s.self_buff_stacks.size() != BUFF_COUNT) {
            throw std::invalid_argument("macro was not compiled for the Tai Xu ruleset");
        }
        Data().ValidateCapacity(e.Log().DamageCapacity());
        for (const Slot SKILL : CastableSkills) {
            if (UsesGcd(SKILL)) {
                s.skill_gcd_group[SKILL] = 0;
            }
            cooldown_[SKILL] = e.Timers().Create({ TimerKind::SKILL_COOLDOWN, SKILL }, 2);
            casting_[SKILL]  = e.Timers().Create({ TimerKind::SKILL_CASTING, SKILL }, 2);
        }
        for (Slot skill = SUI_XING; skill <= TUN_RI; ++skill) {
            prepare_[skill - SUI_XING] = e.Timers().Create({ TimerKind::SKILL_PREPARE, skill }, 2);
        }
        jinghua_prepare_ = e.Timers().Create({ TimerKind::SKILL_PREPARE, JING_HUA }, 2);
        for (Slot slot = 0; slot < fields_.size(); ++slot) {
            field_timers_[slot] = e.Timers().Create({ TimerKind::FIELD, slot }, 5);
        }
        for (const Slot BUFF : { SUI_XING_BUFF, TUN_RI_BUFF, QI_SHENG_BUFF, CHI_YING_BUFF, XUAN_MEN_BUFF, LIE_YUN_BUFF, JIAN_RU_BUFF })
        {
            buff_expire_[BUFF] = e.Timers().Create({ TimerKind::BUFF_DURATION, BUFF, BUFF == TUN_RI_BUFF }, 30);
        }
        for (const Slot BUFF : { YUN_ZHONG_SUI_XING_BUFF, YUN_ZHONG_SHENG_TAI_BUFF, YUN_ZHONG_TUN_RI_BUFF, JING_HUA_BUFF })
        {
            buff_expire_[BUFF] = e.Timers().Create({ TimerKind::BUFF_DURATION, BUFF }, 30);
        }
        weapon_cw_expire_                = e.Timers().Create({ TimerKind::BUFF_DURATION, WEAPON_CW_BUFF }, 30);
        buff_expire_[SET_ATTRIBUTE_BUFF] = e.Timers().Create({ TimerKind::BUFF_DURATION, SET_ATTRIBUTE_BUFF }, 30);
        buff_expire_[YOU_REN_BUFF]       = e.Timers().Create({ TimerKind::BUFF_DURATION, YOU_REN_BUFF }, 30);
        wanxiang_tick_                   = e.Timers().Create({ TimerKind::BUFF_INTERVAL, WAN_XIANG_BUFF, true }, 9);
        wanxiang_expire_                 = e.Timers().Create({ TimerKind::BUFF_DURATION, WAN_XIANG_BUFF, true }, 30);
        hidden_tick_                     = e.Timers().Create({ TimerKind::BUFF_INTERVAL, HIDDEN_LIE_YUN, true }, 5);
        hidden_expire_                   = e.Timers().Create({ TimerKind::BUFF_DURATION, HIDDEN_LIE_YUN, true }, 6);
        renjian_tick_                    = e.Timers().Create({ TimerKind::BUFF_INTERVAL, REN_JIAN_DOT, true }, 12);
        renjian_expire_                  = e.Timers().Create({ TimerKind::BUFF_DURATION, REN_JIAN_DOT, true }, 13);
        jianru_tick_                     = e.Timers().Create({ TimerKind::BUFF_INTERVAL, JIAN_RU_BUFF }, 12);
        yunz_timers_[0]                  = e.Timers().Create({ TimerKind::BUFF_INTERVAL, YUN_ZHONG_SUI_XING_BUFF }, 12);
        yunz_timers_[1]   = e.Timers().Create({ TimerKind::BUFF_INTERVAL, YUN_ZHONG_SHENG_TAI_BUFF }, 12);
        yunz_timers_[2]   = e.Timers().Create({ TimerKind::BUFF_INTERVAL, YUN_ZHONG_TUN_RI_BUFF }, 12);
        gcd_              = e.Timers().Create({ TimerKind::MACRO_WAKE }, 2);
        natural_qidian_   = e.Timers().Create({ TimerKind::RESOURCE_REGEN }, 19);
        // Each Buff executes its last tick before clearing. Independent buffs
        // have a stable order; cross-buff legacy container order is not assumed.
        dot_tick_         = e.Timers().Create({ TimerKind::BUFF_INTERVAL, DIE_REN, true }, 10);
        dot_expire_       = e.Timers().Create({ TimerKind::BUFF_DURATION, DIE_REN, true }, 11);
        purple_tick_      = e.Timers().Create({ TimerKind::BUFF_INTERVAL, PURPLE }, 20);
        purple_expire_    = e.Timers().Create({ TimerKind::BUFF_DURATION, PURPLE }, 21);
        feng_expire_      = e.Timers().Create({ TimerKind::BUFF_DURATION, FENG_SHI }, 30);
        team_application_ = e.Timers().Create({ TimerKind::TEAM_APPLICATION }, 0);
        team_drum_ =
            e.Timers().Create({ TimerKind::BUFF_INTERVAL, static_cast<Slot>(TEAM_BUFF_BEGIN + static_cast<Slot>(team::HAO_LING_SAN_JUN)) }, 1);
        for (Slot i = 0; i < team::COUNT; ++i) {
            buff_expire_[TEAM_BUFF_BEGIN + i] = e.Timers().Create(
                { TimerKind::BUFF_DURATION, static_cast<Slot>(TEAM_BUFF_BEGIN + i), team::Definitions[i].target },
                1);
        }
    }

    template <typename Engine>
    void Reset(Engine &e, std::uint64_t seed)
    {
        auto &s = e.MutableState();
        rng_.Reset(seed);
        dot_snapshot_ = renjian_snapshot_ = 0;
        profile_                          = 0;
        versions_.Clear();
        if (++damage_generation_ == 0) {
            damage_cache_.fill({});
            damage_generation_ = 1;
        }
        team_modifiers_  = {};
        team_generation_ = captured_generation_ = 0;
        team_cursor_                            = 0;
        if (!Data().config.team_buffs.empty()) {
            e.Timers().ArmAt(team_application_, Data().config.team_buffs.front().frame);
        }
        you_ren_crit_basis_points_ = 0;
        recharge_at_               = 0;
        weapon_cw_ready_at_        = 16 * 30;
        wanxiang_snapshot_         = 0;
        fields_.fill({});
        field_order_     = 0;
        jianru_tick_at_  = std::numeric_limits<tick_t>::max();
        // Cloning the legacy player activates ClassFeature and immediately adds
        // one point; phase zero adds another before the first macro evaluation.
        s.qidian         = std::min(10, Data().config.initial_qidian + 1);
        const auto PHASE = qidian_phase_(seed);
        if (PHASE < 0 || PHASE >= 16) {
            throw std::invalid_argument("invalid natural qidian phase");
        }
        e.Timers().ArmAt(natural_qidian_, PHASE);
        s.target_life     = Data().config.target.life;
        s.target_distance = Data().config.target.distance;
        target_timeline_.Reset(e, Data().config.target_events);
        s.skill_energy[ZI_QI] = Data().purple_charges;
    }

    static bool MayActWhileBusy(const MacroLine &line)
    {
        return IsForceCast(line.command) || !IsCastable(line.action_slot);
    }

    static bool UsesPrimaryGcd(Slot skill) { return UsesGcd(skill); }

    static tick_t CastReadyAt(const State &s, Slot skill)
    {
        const auto GROUP = s.skill_gcd_group[skill];
        if (skill == BA_HUANG && (s.self_buff_stacks[WEAPON_CW_BUFF] != 0) && GROUP != INVALID_SLOT) {
            return s.gcd_ready_at[GROUP];
        }
        return s.SkillReadyAt(skill);
    }

    static bool CanAttempt(const State &s, const MacroLine &line)
    {
        if (!IsCastable(line.action_slot)) {
            return true; // Let Cast report the invalid skill if its condition passes.
        }
        if (s.preparing_skill != INVALID_SLOT) {
            return false;
        }
        // A force-cast may interrupt even when the new skill is on cooldown.
        if (s.casting_skill != INVALID_SLOT) {
            return IsForceCast(line.command);
        }
        return CastReadyAt(s, line.action_slot) <= s.now;
    }

    template <typename Engine>
    bool Cast(Engine &e, const MacroLine &line)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("tai_xu::Cast");
        auto                       &s           = e.MutableState();
        const auto                  SKILL       = line.action_slot;
        if (!IsCastable(SKILL)) {
            throw std::invalid_argument("damage-only or unmigrated skill cannot be cast");
        }
        if (s.preparing_skill != INVALID_SLOT) {
            if constexpr (Engine::debug_enabled) {
                e.RejectAction(ActionBlockReason::PREPARING, s.skill_prepare_at[s.preparing_skill] - s.now, 0, s.preparing_skill);
            }
            return false; // /fcast cannot bypass IsCast in ba960f7.
        }
        if (s.casting_skill != INVALID_SLOT) {
            if (!IsForceCast(line.command)) {
                if constexpr (Engine::debug_enabled) {
                    e.RejectAction(ActionBlockReason::CHANNELING, s.skill_casting_at[s.casting_skill] - s.now, 0, s.casting_skill);
                }
                return false;
            }
            if constexpr (Engine::debug_enabled) {
                e.NoteChannelInterrupt(s.casting_skill);
            }
            InterruptCasting(e);
        }
        const auto ready_at = CastReadyAt(s, SKILL);
        if (ready_at > s.now) {
            if constexpr (Engine::debug_enabled) {
                const bool FREE_BAHUANG = SKILL == BA_HUANG && s.self_buff_stacks[WEAPON_CW_BUFF];
                const auto REASON       = !FREE_BAHUANG && s.skill_ready_at[SKILL] == ready_at
                                              ? ActionBlockReason::SKILL_COOLDOWN
                                              : ActionBlockReason::GLOBAL_COOLDOWN;
                e.RejectAction(REASON, ready_at - s.now, 0, SKILL);
            }
            return false;
        }
        if (SKILL == WU_WO && s.qidian == 0) {
            if constexpr (Engine::debug_enabled) {
                e.RejectAction(ActionBlockReason::QIDIAN, s.qidian, 1);
            }
            return false;
        }
        if (SKILL == ZI_QI && s.skill_energy[ZI_QI] == 0) {
            if constexpr (Engine::debug_enabled) {
                e.RejectAction(ActionBlockReason::CHARGES, s.skill_energy[ZI_QI], 1, ZI_QI);
            }
            return false;
        }
        double range = SKILL == BA_HUANG ? 12 : 4;
        if (SKILL == WAN_JIAN || SKILL == JING_HUA) {
            range = 8;
        } else if (IsRenJian(SKILL)) {
            range = Data().renjian_range;
        }
        if ((SKILL == BA_HUANG || SKILL == SAN_HUAN) && Data().Has(FENG_SHI_TALENT)) {
            range += 2;
        }
        if (SKILL != ZI_QI && !IsFieldSkill(SKILL) && s.target_distance > range) {
            if constexpr (Engine::debug_enabled) {
                e.RejectAction(ActionBlockReason::DISTANCE, s.target_distance, range);
            }
            return false;
        }
        if (SKILL == JING_HUA && (!Data().Has(JING_HUA_TALENT) || !s.self_buff_stacks[JING_HUA_BUFF])) {
            if constexpr (Engine::debug_enabled) {
                e.RejectAction(Data().Has(JING_HUA_TALENT) ? ActionBlockReason::MISSING_BUFF : ActionBlockReason::MISSING_TALENT,
                               s.self_buff_stacks[JING_HUA_BUFF],
                               1,
                               JING_HUA_BUFF);
            }
            return false;
        }
        [[maybe_unused]] const auto EFFECT_TIMER = CastEffectTimer<Engine>();
        if (SKILL != SAN_CHAI && !IsFieldSkill(SKILL)) {
            s.last_skill = SKILL;
        }
        if (UsesGcd(SKILL)) {
            SetGcd(s, e.Log(), 0, s.now + Data().gcd);
            e.Timers().ArmAt(gcd_, s.gcd_ready_at[0]);
        }
        if (IsFieldSkill(SKILL)) {
            const auto DURATION = SKILL == SHENG_TAI ? Data().shengtaiji_prepare : Data().field_prepare;
            SetPrepare(s, e.Log(), SKILL, s.now + DURATION, true);
            SetSkillCooldown(s, e.Log(), SKILL, std::numeric_limits<tick_t>::max());
            e.Timers().ArmAt(prepare_[SKILL - SUI_XING], s.now + DURATION);
            return true;
        }
        if (SKILL == JING_HUA) {
            SetSkillCooldown(s, e.Log(), JING_HUA, std::numeric_limits<tick_t>::max());
            SetPrepare(s, e.Log(), JING_HUA, s.now + Data().jinghua_prepare, true);
            e.Timers().ArmAt(jinghua_prepare_, s.now + Data().jinghua_prepare);
            Hit(e, JingHuaEffect, Snapshot(s));
            ChiYingProc(e);
            ShenMaiProc(e);
            TryWeaponCw(e);
            TrySetAttribute(e);
            return true;
        }
        switch (SKILL) {
            case WU_WO: CastWuWo(e); break;
            case BA_HUANG: CastBaHuang(e); break;
            case SAN_HUAN: CastSanHuan(e); break;
            case SAN_CHAI:
            {
                CooldownAt(e, SAN_CHAI, s.now + Data().sanchai_cd);
                const auto snapshot = Snapshot(s);
                const auto ROLL     = Roll(SanChaiEffect, snapshot);
                ShenMaiProc(e);
                Record(e, SanChaiEffect, snapshot, ROLL);
                TryWeaponCw(e);
                TrySetAttribute(e);
                break;
            }
            case ZI_QI: CastPurple(e); break;
            case WAN_JIAN: CastWanJian(e); break;
            case REN_JIAN:
            case REN_JIAN_SUI_XING: CastRenJian(e, SKILL); break;
        }
        return true;
    }

    template <typename Engine>
    void OnTimer(Engine &e, TimerHandle handle, DeadlineQueue::TimerId)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("tai_xu::OnTimer");
        auto                       &s           = e.MutableState();
        if (handle.kind == TimerKind::TEAM_APPLICATION) {
            const auto &schedule = Data().config.team_buffs;
            while (team_cursor_ < schedule.size() && schedule[team_cursor_].frame == s.now) {
                ApplyTeam(e, schedule[team_cursor_++]);
            }
            if (team_cursor_ < schedule.size()) {
                e.Timers().ArmAt(team_application_, schedule[team_cursor_].frame);
            }
        } else if (handle.kind == TimerKind::BUFF_INTERVAL && handle.slot >= TEAM_BUFF_BEGIN) {
            const auto OLD       = s.self_buff_stacks[handle.slot];
            const auto REMAINING = OLD / 2;
            team_modifiers_.Add(team::Definitions[team::HAO_LING_SAN_JUN].modifiers, REMAINING - OLD);
            ++team_generation_;
            SetBuff(s, e.Log(), false, handle.slot, s.self_buff_expires_at[handle.slot], 0, REMAINING);
        } else if (handle.kind == TimerKind::BUFF_DURATION && handle.slot >= TEAM_BUFF_BEGIN) {
            ApplyTeam(e, { static_cast<team::Kind>(handle.slot - TEAM_BUFF_BEGIN), s.now, 0, 0 });
        } else if (handle.kind == TimerKind::RESOURCE_REGEN) {
            if (s.qidian < 10) {
                QidianAt(e, s.qidian + 1);
            }
            e.Timers().ArmAt(natural_qidian_, s.now + 16);
        } else if (handle.kind == TimerKind::SKILL_PREPARE && handle.slot == JING_HUA) {
            CompleteJingHua(e);
        } else if (handle.kind == TimerKind::SKILL_PREPARE) {
            CompleteField(e, handle.slot);
        } else if (handle.kind == TimerKind::SKILL_CASTING) {
            CompleteCasting(e, handle.slot);
        } else if (handle.kind == TimerKind::FIELD) {
            TickField(e, handle.slot);
        } else if (handle.kind == TimerKind::SKILL_COOLDOWN && handle.slot == ZI_QI) {
            SetSkillEnergy(s, e.Log(), ZI_QI, s.skill_energy[ZI_QI] + 1);
            if (s.skill_energy[ZI_QI] < Data().purple_charges) {
                recharge_at_ = s.now + Data().purple_cd;
                e.Timers().ArmAt(cooldown_[ZI_QI], recharge_at_);
            } else {
                recharge_at_ = 0;
            }
            SetSkillCooldown(s, e.Log(), ZI_QI, 0);
        } else if (handle.kind == TimerKind::BUFF_INTERVAL && handle.slot == HIDDEN_LIE_YUN) {
            PulseLieYun(e);
            const auto next = s.now + 8;
            SetBuff(s, e.Log(), true, HIDDEN_LIE_YUN, s.target_buff_expires_at[HIDDEN_LIE_YUN], next, 1);
            e.Timers().ArmAt(hidden_tick_, next);
        } else if (handle.kind == TimerKind::BUFF_INTERVAL && handle.slot == REN_JIAN_DOT) {
            DotHit(e, RenJianDotEffect, renjian_snapshot_);
            const auto next = s.now + Data().dot_interval;
            SetBuff(s, e.Log(), true, REN_JIAN_DOT, s.target_buff_expires_at[REN_JIAN_DOT], next, 1);
            e.Timers().ArmAt(renjian_tick_, next);
        } else if (handle.kind == TimerKind::BUFF_INTERVAL && handle.slot == JIAN_RU_BUFF) {
            Hit(e, JianRuEffect, Snapshot(s));
            const auto next = s.now + 16;
            jianru_tick_at_ = next;
            SetBuff(s, e.Log(), false, JIAN_RU_BUFF, s.self_buff_expires_at[JIAN_RU_BUFF], next, 1);
            e.Timers().ArmAt(jianru_tick_, next);
        } else if (handle.kind == TimerKind::BUFF_INTERVAL && handle.slot >= YUN_ZHONG_SUI_XING_BUFF && handle.slot <= YUN_ZHONG_TUN_RI_BUFF)
        {
            const auto effect = static_cast<std::uint16_t>(YunZhongSuiXingEffect + handle.slot - YUN_ZHONG_SUI_XING_BUFF);
            if (s.target_distance <= 6) {
                Hit(e, effect, Snapshot(s));
            }
            ActivateJianRu(e);
            const auto next = s.now + 16;
            SetBuff(s, e.Log(), false, handle.slot, s.self_buff_expires_at[handle.slot], next, 1);
            e.Timers().ArmAt(yunz_timers_[handle.slot - YUN_ZHONG_SUI_XING_BUFF], next);
        } else if (handle.kind == TimerKind::BUFF_INTERVAL && handle.target && handle.slot == WAN_XIANG_BUFF) {
            const int STACKS = s.target_buff_stacks[WAN_XIANG_BUFF];
            if (!STACKS) {
                return;
            }
            DotHit(e, static_cast<std::uint16_t>(WanXiangEffect + std::min(STACKS, 3) - 1), wanxiang_snapshot_, STACKS);
            const auto next = s.now + Data().dot_interval;
            SetBuff(s, e.Log(), true, WAN_XIANG_BUFF, s.target_buff_expires_at[WAN_XIANG_BUFF], next, STACKS);
            e.Timers().ArmAt(wanxiang_tick_, next);
        } else if (handle.kind == TimerKind::BUFF_INTERVAL && handle.target) {
            DotHit(e, DotEffect(s.target_buff_stacks[DIE_REN], 1), dot_snapshot_);
            const auto next = s.now + Data().dot_interval;
            SetBuff(s, e.Log(), true, DIE_REN, s.target_buff_expires_at[DIE_REN], next, s.target_buff_stacks[DIE_REN]);
            e.Timers().ArmAt(dot_tick_, next);
        } else if (handle.kind == TimerKind::BUFF_INTERVAL) {
            QidianAt(e, s.qidian + 2);
            const auto next = s.now + 16;
            SetBuff(s, e.Log(), false, PURPLE, s.self_buff_expires_at[PURPLE], next, 1);
            e.Timers().ArmAt(purple_tick_, next);
        } else if (handle.kind == TimerKind::BUFF_DURATION && handle.slot == WEAPON_CW_BUFF) {
            SetBuff(s, e.Log(), false, WEAPON_CW_BUFF, 0, 0, 0);
            SetSkillCooldown(s, e.Log(), BA_HUANG, s.now);
            e.Timers().Cancel(cooldown_[BA_HUANG]);
        } else if (handle.kind == TimerKind::BUFF_DURATION && handle.target && handle.slot == WAN_XIANG_BUFF) {
            SetBuff(s, e.Log(), true, WAN_XIANG_BUFF, 0, 0, 0);
            e.Timers().Cancel(wanxiang_tick_);
        } else if (handle.kind == TimerKind::BUFF_DURATION) {
            if (handle.target && handle.slot == DIE_REN) {
                ClearDot(e);
            } else {
                SetBuff(s, e.Log(), handle.target, handle.slot, 0, 0, 0);
                if (handle.slot == PURPLE) {
                    profile_ &= ~PURPLE_PROFILE;
                    e.Timers().Cancel(purple_tick_);
                } else if (handle.slot == SUI_XING_BUFF) {
                    profile_ &= ~(SUI_XING_PROFILE | LIVE_GU_CHANG_PROFILE);
                } else if (handle.slot == QI_SHENG_BUFF) {
                    profile_ &= ~QI_SHENG_PROFILE;
                } else if (handle.slot == XUAN_MEN_BUFF) {
                    profile_ &= ~XUAN_MEN_PROFILE_MASK;
                } else if (handle.slot == LIE_YUN_BUFF) {
                    profile_ &= ~LIE_YUN_PROFILE;
                } else if (handle.slot == HIDDEN_LIE_YUN) {
                    e.Timers().Cancel(hidden_tick_);
                } else if (handle.slot == REN_JIAN_DOT) {
                    e.Timers().Cancel(renjian_tick_);
                } else if (handle.slot == JIAN_RU_BUFF) {
                    e.Timers().Cancel(jianru_tick_);
                    jianru_tick_at_ = std::numeric_limits<tick_t>::max();
                } else if (handle.slot >= YUN_ZHONG_SUI_XING_BUFF && handle.slot <= YUN_ZHONG_TUN_RI_BUFF) {
                    e.Timers().Cancel(yunz_timers_[handle.slot - YUN_ZHONG_SUI_XING_BUFF]);
                } else if (handle.slot == JING_HUA_BUFF) {
                    SetSkillCooldown(s, e.Log(), JING_HUA, std::numeric_limits<tick_t>::max());
                } else if (handle.slot == SET_ATTRIBUTE_BUFF) {
                    profile_ &= ~SET_ATTRIBUTE_PROFILE;
                } else if (handle.slot == YOU_REN_BUFF) {
                    you_ren_crit_basis_points_ = 0;
                }
            }
        } else if (handle.kind == TimerKind::TARGET_CHANGE) {
            target_timeline_.Dispatch(e, Data().config.target_events);
        }
    }

    value_t Reduce(const DamageIntent &hit) const
    {
        const auto EFFECT_ROLL =
            (static_cast<std::uint32_t>(hit.effect) * 2) + static_cast<std::uint32_t>(hit.outcome == RollResult::DOUBLE);
        const auto INDEX = ((hit.snapshot_version * 0x9e3779b1u) ^ (hit.live_version * 0x85ebca6bu) ^ (EFFECT_ROLL * 17u)) &
                           (damage_cache_.size() - 1);
        auto &cached = damage_cache_[INDEX];
        if (cached.generation == damage_generation_ && cached.snapshot == hit.snapshot_version &&
            cached.live == hit.live_version && cached.effect_roll == EFFECT_ROLL)
        {
            return cached.damage;
        }
        const auto DAMAGE =
            Data().Damage(versions_[hit.snapshot_version], versions_[hit.live_version], hit.effect, hit.outcome == RollResult::DOUBLE);
        cached = { damage_generation_, hit.snapshot_version, hit.live_version, EFFECT_ROLL, DAMAGE };
        return DAMAGE;
    }

    value_t Reduce(const DamageIntent &hit, const State &) const { return Reduce(hit); }

private:
    struct Rolled
    {
        std::uint64_t key;
        RollResult    outcome;
    };

    static double UnitSample(std::uint64_t key)
    {
        // Validation policies can preserve the exact legacy floating-point draw.
        if constexpr (requires { Rng::UnitSample(key); }) {
            return Rng::UnitSample(key);
        } else {
            return static_cast<double>(key >> 11) * 0x1.0p-53;
        }
    }

    Rolled Roll(std::uint16_t effect, AttributeVersionId snapshot)
    {
        const auto  KEY     = rng_.Next();
        const auto  SAMPLE  = UnitSample(KEY);
        const auto &version = versions_[snapshot];
        const auto CHANCE = version.critical_chance + Data().chance_bonus[effect] + (you_ren_crit_basis_points_ * 1.0 / 10000);
        return { KEY, SAMPLE < CHANCE ? RollResult::DOUBLE : RollResult::HIT };
    }

    AttributeVersionId Snapshot(const State &)
    {
        if ((versions_.Size() == 0u) || versions_[current_version_].profile != profile_ || captured_generation_ != team_generation_)
        {
            current_version_     = versions_.Append(Data().Resolve(profile_, team_modifiers_));
            captured_generation_ = team_generation_;
        }
        return current_version_;
    }

    static std::uint16_t DotEffect(int stacks, int count)
    {
        return static_cast<std::uint16_t>(DieRenEffect + ((stacks - 1) * 9) + count);
    }

    template <typename Engine>
    void Record(Engine &e, std::uint16_t effect, AttributeVersionId snapshot, Rolled roll, int level = -1, int sub = -1)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("tai_xu::Record");
        const auto                 &f           = Formulas[effect];
        const auto                  LIVE        = Snapshot(e.GetState());
        const auto                  PROFILE =
            static_cast<std::uint16_t>((versions_[snapshot].profile & ~LiveProfileMask) | (versions_[LIVE].profile & LiveProfileMask));
        e.Log().Record(e.GetState().now,
                       f.skill,
                       f.kind == FormulaKind::ZERO ? INVALID_SLOT : 0,
                       static_cast<std::uint16_t>(level < 0 ? f.level : level),
                       roll.key,
                       0,
                       0,
                       roll.outcome,
                       effect,
                       PROFILE,
                       static_cast<std::uint16_t>(sub < 0 ? f.sub : sub),
                       snapshot,
                       LIVE);
    }

    template <typename Engine>
    RollResult Hit(Engine &e, std::uint16_t effect, AttributeVersionId snapshot, int level = -1, int sub = -1)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("tai_xu::Hit");
        const auto                  ROLL        = Roll(effect, snapshot);
        Record(e, effect, snapshot, ROLL, level, sub);
        return ROLL.outcome;
    }

    template <typename Engine>
    void DotHit(Engine &e, std::uint16_t effect, AttributeVersionId snapshot, int sub = -1)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("tai_xu::DotHit");
        // Crit chance is snapshotted, while overcome and shield ignore are live.
        const auto                  ROLL        = Roll(effect, snapshot);
        Record(e, effect, snapshot, ROLL, -1, sub);
    }

    template <typename Engine>
    void ApplyTeam(Engine &e, const team::Application &app)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("tai_xu::ApplyTeam");
        auto                       &s           = e.MutableState();
        const auto                  slot        = static_cast<Slot>(TEAM_BUFF_BEGIN + static_cast<Slot>(app.kind));
        const auto                 &d           = team::Definitions[app.kind];
        const auto                  OLD         = d.target ? s.target_buff_stacks[slot] : s.self_buff_stacks[slot];
        if (app.kind == team::HAO_LING_SAN_JUN && OLD && app.stacks) {
            return;
        }
        team_modifiers_.Add(d.modifiers, app.stacks - OLD);
        if (app.stacks != OLD) {
            ++team_generation_;
        }
        const auto EXPIRY = app.stacks ? s.now + (app.duration ? app.duration : d.duration) : 0;
        const auto next   = app.kind == team::HAO_LING_SAN_JUN && app.stacks ? s.now + 480 : 0;
        SetBuff(s, e.Log(), d.target, slot, EXPIRY, next, app.stacks);
        if (app.kind == team::HAO_LING_SAN_JUN) {
            if (next) {
                e.Timers().ArmAt(team_drum_, next);
            } else {
                e.Timers().Cancel(team_drum_);
            }
        }
        if (app.stacks) {
            e.Timers().ArmAt(buff_expire_[slot], EXPIRY);
        } else {
            e.Timers().Cancel(buff_expire_[slot]);
        }
    }

    template <typename Engine>
    void QidianAt(Engine &e, int qidian)
    {
        auto &s = e.MutableState();
        SetResources(s, e.Log(), std::clamp(qidian, 0, 10), s.energy, s.rage);
    }

    template <typename Engine>
    void CooldownAt(Engine &e, Slot slot, tick_t deadline)
    {
        SetSkillCooldown(e.MutableState(), e.Log(), slot, deadline);
        e.Timers().ArmAt(cooldown_[slot], deadline);
    }

    template <typename Engine>
    void StartCasting(Engine &e, Slot skill, tick_t duration)
    {
        if (!IsCastable(skill) || duration <= 0) {
            throw std::invalid_argument("invalid channel duration");
        }
        auto &s = e.MutableState();
        if (s.preparing_skill != INVALID_SLOT || s.casting_skill != INVALID_SLOT) {
            throw std::logic_error("a cast is already in progress");
        }
        const auto FINISH = s.now + duration;
        SetCasting(s, e.Log(), skill, FINISH, true);
        e.Timers().ArmAt(casting_[skill], FINISH);
    }

    template <typename Engine>
    void InterruptCasting(Engine &e)
    {
        auto &s = e.MutableState();
        if (s.casting_skill == INVALID_SLOT) {
            return;
        }
        const auto SKILL = s.casting_skill;
        SetCasting(s, e.Log(), SKILL, 0, false);
        e.Timers().Cancel(casting_[SKILL]);
    }

    template <typename Engine>
    void CompleteCasting(Engine &e, Slot skill)
    {
        auto &s = e.MutableState();
        if (s.casting_skill != skill || s.skill_casting_at[skill] != s.now) {
            return;
        }
        SetCasting(s, e.Log(), skill, 0, false);
    }

    bool Proc(int numerator, int denominator)
    {
        if constexpr (requires { rng_.Proc(numerator, denominator); }) {
            return rng_.Proc(numerator, denominator);
        } else {
            const auto KEY = rng_.Next();
            return UnitSample(KEY) < static_cast<double>(numerator) / denominator;
        }
    }

    template <typename Engine>
    void TriggerYouRen(Engine &e, RollResult outcome)
    {
        if (!Data().HasTeam(YOU_REN) || outcome != RollResult::DOUBLE) {
            return;
        }
        auto     &s                 = e.MutableState();
        const int OLD_STACKS        = s.self_buff_stacks[YOU_REN_BUFF];
        const int STACKS            = std::min(5, OLD_STACKS + 1);
        you_ren_crit_basis_points_ += (STACKS - OLD_STACKS) * 100;
        SetBuff(s, e.Log(), false, YOU_REN_BUFF, s.now + 320, 0, STACKS);
        e.Timers().ArmAt(buff_expire_[YOU_REN_BUFF], s.now + 320);
    }

    template <typename Engine>
    void TryWeaponCw(Engine &e)
    {
        // Legacy rolls on every eligible hit, including during the internal CD.
        if (!Data().HasEquipment(WEAPON_CW) || !Proc(31, 1024) || e.GetState().now < weapon_cw_ready_at_) {
            return;
        }
        auto &s             = e.MutableState();
        weapon_cw_ready_at_ = s.now + (16 * 30);
        SetBuff(s, e.Log(), false, WEAPON_CW_BUFF, s.now + (16 * 6), 0, 1);
        SetSkillCooldown(s, e.Log(), BA_HUANG, s.now);
        e.Timers().Cancel(cooldown_[BA_HUANG]);
        e.Timers().ArmAt(weapon_cw_expire_, s.now + (16 * 6));
    }

    template <typename Engine>
    void TrySetAttribute(Engine &e)
    {
        if (Data().HasEquipment(SET_ATTRIBUTE)) {
            Aura(e, SET_ATTRIBUTE_BUFF, 96);
        }
    }

    template <typename Engine>
    void AddWanXiang(Engine &e)
    {
        if (!e.GetState().self_buff_stacks[WEAPON_CW_BUFF]) {
            return;
        }
        auto      &s      = e.MutableState();
        const bool EXISTS = s.target_buff_stacks[WAN_XIANG_BUFF] != 0;
        const auto next   = EXISTS ? s.target_buff_tick_at[WAN_XIANG_BUFF] : s.now + Data().dot_interval;
        const auto EXPIRY = next + (Data().dot_interval * 9);
        const auto STACKS = std::min(3, s.target_buff_stacks[WAN_XIANG_BUFF] + 1);
        SetBuff(s, e.Log(), true, WAN_XIANG_BUFF, EXPIRY, next, STACKS);
        wanxiang_snapshot_ = Snapshot(s);
        e.Timers().ArmAt(wanxiang_tick_, next);
        e.Timers().ArmAt(wanxiang_expire_, EXPIRY);
    }

    template <typename Engine>
    void ShenMaiProc(Engine &e)
    {
        // The pinned implementation gives two points even on a non-critical hit.
        if (Data().Has(SHEN_MAI)) {
            QidianAt(e, e.GetState().qidian + 2);
        }
    }

    template <typename Engine>
    void AddFeng(Engine &e)
    {
        if (!Data().Has(FENG_SHI_TALENT)) {
            return;
        }
        auto &s = e.MutableState();
        SetBuff(s, e.Log(), false, FENG_SHI, s.now + 80, 0, 1);
        e.Timers().ArmAt(feng_expire_, s.now + 80);
    }

    template <typename Engine>
    void AddDot(Engine &e, int stacks)
    {
        auto      &s    = e.MutableState();
        const auto next = s.target_buff_stacks[DIE_REN] ? s.target_buff_tick_at[DIE_REN] : s.now + Data().dot_interval;
        const auto EXPIRY = next + static_cast<tick_t>(48 * Data().haste * 7);
        SetBuff(s, e.Log(), true, DIE_REN, EXPIRY, next, std::min(Data().dot_max_stacks, s.target_buff_stacks[DIE_REN] + stacks));
        UpdateLieYun(e); // The immediate threshold proc precedes the new DOT snapshot.
        dot_snapshot_ = Snapshot(s);
        e.Timers().ArmAt(dot_tick_, next);
        e.Timers().ArmAt(dot_expire_, EXPIRY);
    }

    template <typename Engine>
    void ClearDot(Engine &e, bool erase_first = false)
    {
        if (!erase_first) {
            UpdateLieYun(e);
        }
        SetBuff(e.MutableState(), e.Log(), true, DIE_REN, 0, 0, 0);
        if (erase_first) {
            UpdateLieYun(e);
        }
        e.Timers().Cancel(dot_tick_);
        e.Timers().Cancel(dot_expire_);
    }

    template <typename Engine>
    void CastWuWo(Engine &e)
    {
        [[maybe_unused]] const auto FRAME_SCOPE  = RuleFunctionScope<Engine>("tai_xu::CastWuWo");
        auto                       &s            = e.MutableState();
        const auto                  SPENT_QIDIAN = s.qidian;
        const auto                  effect =
            static_cast<std::uint16_t>((s.self_buff_stacks[FENG_SHI] ? FengWuWoEffect : WuWoEffect) + SPENT_QIDIAN - 1);
        QidianAt(e, (Data().config.recipes[WU_WO] & Recipe::QIDIAN) ? 1 : 0);
        const auto ROLL = Hit(e, effect, Snapshot(s));
        TriggerYouRen(e, ROLL);
        if (SPENT_QIDIAN >= 6) {
            Hit(e, PoZhaoEffect + ((SPENT_QIDIAN - 6) / 2), Snapshot(s));
        }
        if (Data().Has(WU_YU) && s.target_buff_stacks[DIE_REN]) {
            CooldownAt(e, BA_HUANG, std::max(s.now, s.skill_ready_at[BA_HUANG] - ((SPENT_QIDIAN + 1) * 8)));
        }
        ChiYingProc(e);
        if (Data().Has(DIE_REN_TALENT)) {
            AddDot(e, 1 + static_cast<int>(ROLL));
        }
        ShenMaiProc(e);
        if (Data().Has(FENG_SHI_TALENT)) {
            SetBuff(s, e.Log(), false, FENG_SHI, 0, 0, 0);
            e.Timers().Cancel(feng_expire_);
            AddDot(e, 1); // Legacy wrapper does this even when FengShi was absent.
        }
        TryWeaponCw(e);
        TrySetAttribute(e);
        TriggerYouRen(e, ROLL);
    }

    template <typename Engine>
    void CastBaHuang(Engine &e)
    {
        [[maybe_unused]] const auto FRAME_SCOPE   = RuleFunctionScope<Engine>("tai_xu::CastBaHuang");
        auto                       &s             = e.MutableState();
        const bool                  WEAPON_WINDOW = s.self_buff_stacks[WEAPON_CW_BUFF] != 0;
        if (WEAPON_WINDOW) {
            SetSkillCooldown(s, e.Log(), BA_HUANG, s.now);
            e.Timers().Cancel(cooldown_[BA_HUANG]);
        } else {
            CooldownAt(e, BA_HUANG, s.now + Data().bahuang_cd);
        }
        QidianAt(e, s.qidian + 2 + ((Data().config.recipes[BA_HUANG] & QIDIAN) ? 1 : 0));
        const int  LEVEL      = std::clamp(static_cast<int>(10 - ((s.target_life - .01) * 10)), 0, 9);
        const auto FIRST_ROLL = Hit(e, BaHuangEffect, Snapshot(s), LEVEL);
        Hit(e, WEAPON_WINDOW ? BaHuangCwEffect : BaHuangExtraEffect + LEVEL, Snapshot(s));
        ShenMaiProc(e);
        AddFeng(e);
        AddOrTriggerJianRu(e);
        if (Data().Has(QIE_YU) && s.target_buff_stacks[DIE_REN] &&
            (s.target_life <= .4 || (s.self_buff_stacks[PURPLE] && s.target_life <= .6)))
        {
            const int count = static_cast<int>((s.target_buff_expires_at[DIE_REN] - s.now) / 48 * Data().haste);
            DotHit(e, DotEffect(s.target_buff_stacks[DIE_REN], count), dot_snapshot_, 1);
            ClearDot(e);
        }
        ChiYingProc(e);
        TryWeaponCw(e);
        AddWanXiang(e);
        if (Data().HasEquipment(WEAPON_CW) && Proc(307, 1024)) {
            Hit(e, WeaponCwExtraEffect, Snapshot(s));
        }
        TrySetAttribute(e);
        TriggerYouRen(e, FIRST_ROLL);
    }

    template <typename Engine>
    void CastSanHuan(Engine &e)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("tai_xu::CastSanHuan");
        auto                       &s           = e.MutableState();
        CooldownAt(e, SAN_HUAN, s.now + Data().sanhuan_cd);
        QidianAt(e, s.qidian + 2);
        const auto FIRST_ROLL = Hit(e, SanHuanEffect, Snapshot(s));
        for (int i = 1; i < 3; ++i) {
            Hit(e, SanHuanEffect, Snapshot(s));
        }
        if (Data().Has(HUAN_YUE)) {
            QidianAt(e, s.qidian + 4);
            for (int i = 0; i < 3; ++i) {
                Hit(e, PoZhaoEffect, Snapshot(s), -1, 1);
            }
            for (int i = 0; i < 3 && s.target_buff_stacks[DIE_REN]; ++i) {
                const auto EXPIRY = s.target_buff_expires_at[DIE_REN] - Data().dot_interval;
                if (EXPIRY >= s.now) {
                    DotHit(e, DotEffect(s.target_buff_stacks[DIE_REN], 1), dot_snapshot_);
                }
                if (EXPIRY <= s.now) {
                    ClearDot(e, true);
                } else {
                    SetBuff(s, e.Log(), true, DIE_REN, EXPIRY, s.target_buff_tick_at[DIE_REN], s.target_buff_stacks[DIE_REN]);
                    e.Timers().ArmAt(dot_expire_, EXPIRY);
                }
            }
        }
        ShenMaiProc(e);
        AddFeng(e);
        ChiYingProc(e);
        TryWeaponCw(e);
        TrySetAttribute(e);
        TriggerYouRen(e, FIRST_ROLL);
    }

    template <typename Engine>
    void CastPurple(Engine &e)
    {
        auto &s = e.MutableState();
        if (s.skill_energy[ZI_QI] == Data().purple_charges) {
            recharge_at_ = s.now + Data().purple_cd;
            e.Timers().ArmAt(cooldown_[ZI_QI], recharge_at_);
        }
        SetSkillEnergy(s, e.Log(), ZI_QI, s.skill_energy[ZI_QI] - 1);
        SetSkillCooldown(s, e.Log(), ZI_QI, s.skill_energy[ZI_QI] ? 0 : recharge_at_);
        QidianAt(e, 10);
        const auto next   = s.self_buff_stacks[PURPLE] ? s.self_buff_tick_at[PURPLE] : s.now + 16;
        const auto EXPIRY = (s.self_buff_stacks[PURPLE] ? s.self_buff_expires_at[PURPLE] : s.now) + 160;
        SetBuff(s, e.Log(), false, PURPLE, EXPIRY, next, 1);
        profile_ |= PURPLE_PROFILE;
        e.Timers().ArmAt(purple_tick_, next);
        e.Timers().ArmAt(purple_expire_, EXPIRY);
        Record(e, ZeroEffect, Snapshot(s), { 0, RollResult::HIT });
    }

    template <typename Engine>
    void CastWanJian(Engine &e)
    {
        auto &s = e.MutableState();
        CooldownAt(e, WAN_JIAN, s.now + Data().wanjian_cd);
        QidianAt(e, s.qidian + 1);
        const auto ROLL = Hit(e, WanJianEffect, Snapshot(s));
        TriggerYouRen(e, ROLL);
        ShenMaiProc(e);
        ActivateJianRu(e);
        if (Data().Has(XU_JI)) {
            AddDot(e, 1);
        }
        TriggerJingHua(e);
        TryWeaponCw(e);
        ChiYingProc(e);
        TrySetAttribute(e);
    }

    template <typename Engine>
    void TriggerJingHua(Engine &e)
    {
        if (!Data().Has(JING_HUA_TALENT)) {
            return;
        }
        auto &s = e.MutableState();
        if (!s.target_buff_stacks[TUN_RI_BUFF] || s.target_buff_expires_at[TUN_RI_BUFF] <= s.now) {
            return;
        }
        SetBuff(s, e.Log(), false, JING_HUA_BUFF, s.now + 128, 0, 1);
        SetSkillCooldown(s, e.Log(), JING_HUA, 0);
        e.Timers().Cancel(cooldown_[JING_HUA]);
        e.Timers().ArmAt(buff_expire_[JING_HUA_BUFF], s.now + 128);
    }

    template <typename Engine>
    void AddOrTriggerJianRu(Engine &e)
    {
        if (!Data().Has(JIAN_RU)) {
            return;
        }
        auto &s = e.MutableState();
        if (!s.self_buff_stacks[JIAN_RU_BUFF]) {
            if (!s.self_buff_stacks[PURPLE]) {
                return;
            }
            SetBuff(s, e.Log(), false, JIAN_RU_BUFF, s.now + 98, std::numeric_limits<tick_t>::max(), 1);
            jianru_tick_at_ = std::numeric_limits<tick_t>::max();
            e.Timers().ArmAt(buff_expire_[JIAN_RU_BUFF], s.now + 98);
            return;
        }
        if (jianru_tick_at_ != std::numeric_limits<tick_t>::max()) {
            // An active JianRu causes the stronger immediate hit and remains active.
            Hit(e, JianRuActiveEffect, Snapshot(s), -1, 1);
            return;
        }
        // Refresh an unactivated JianRu while Purple is still present.
        if (s.self_buff_stacks[PURPLE]) {
            SetBuff(s, e.Log(), false, JIAN_RU_BUFF, s.now + 98, std::numeric_limits<tick_t>::max(), 1);
            e.Timers().ArmAt(buff_expire_[JIAN_RU_BUFF], s.now + 98);
        }
    }

    template <typename Engine>
    void ActivateJianRu(Engine &e)
    {
        if (!Data().Has(JIAN_RU)) {
            return;
        }
        auto &s = e.MutableState();
        if (!s.self_buff_stacks[JIAN_RU_BUFF] || jianru_tick_at_ != std::numeric_limits<tick_t>::max()) {
            return;
        }
        jianru_tick_at_ = s.now + 16;
        SetBuff(s, e.Log(), false, JIAN_RU_BUFF, s.self_buff_expires_at[JIAN_RU_BUFF], jianru_tick_at_, 1);
        e.Timers().ArmAt(jianru_tick_, jianru_tick_at_);
    }

    template <typename Engine>
    void CompleteJingHua(Engine &e)
    {
        auto &s = e.MutableState();
        if (s.preparing_skill != JING_HUA) {
            throw std::logic_error("stale JingHua prepare event");
        }
        SetPrepare(s, e.Log(), JING_HUA, 0, false);
        s.last_skill = JING_HUA;
        Hit(e, JingHuaSuiEffect, Snapshot(s), -1, 1);
    }

    template <typename Engine>
    void CastRenJian(Engine &e, Slot skill)
    {
        auto &s = e.MutableState();
        CooldownAt(e, REN_JIAN, s.now + Data().renjian_cd);
        CooldownAt(e, REN_JIAN_SUI_XING, s.now + Data().renjian_cd);
        int  count            = 0;
        bool consumed_suixing = false;
        for (unsigned i = 0; i < fields_.size(); ++i) {
            const auto slot = OldestField();
            if (slot == INVALID_SLOT) {
                break;
            }
            const auto FIELD_SKILL = fields_[slot].skill;
            // The old variant stops consuming at the second SuiXing at the front.
            if (skill == REN_JIAN_SUI_XING && FIELD_SKILL == SUI_XING && consumed_suixing) {
                break;
            }
            consumed_suixing |= FIELD_SKILL == SUI_XING;
            fields_[slot]     = {};
            e.Timers().Cancel(field_timers_[slot]);
            PublishField(e, FIELD_SKILL);
            ++count;
            AddYunZhong(e, FIELD_SKILL);
            const auto effect   = skill == REN_JIAN ? RenJianEffect : RenJianSuiXingEffect;
            const auto snapshot = Snapshot(s);
            const auto ROLL     = Roll(effect, snapshot);
            ShenMaiProc(e);
            if (Data().config.renjian_recipes & DOT) {
                AddRenJianDot(e);
            }
            Record(e, effect, snapshot, ROLL);
            TryWeaponCw(e);
        }
        ChiYingProc(e);
        if (Data().Has(XUAN_MEN)) {
            Aura(e, XUAN_MEN_BUFF, 640, std::min(3, s.self_buff_stacks[XUAN_MEN_BUFF] + count));
        }
        TrySetAttribute(e);
    }

    template <typename Engine>
    void AddYunZhong(Engine &e, Slot field_skill)
    {
        const unsigned RECIPE = field_skill == SUI_XING    ? YUN_ZHONG_SUI_XING
                                : field_skill == SHENG_TAI ? YUN_ZHONG_SHENG_TAI
                                                           : YUN_ZHONG_TUN_RI;
        if (!(Data().config.renjian_recipes & RECIPE)) {
            return;
        }
        auto      &s    = e.MutableState();
        const auto BUFF = static_cast<Slot>(YUN_ZHONG_SUI_XING_BUFF + field_skill - SUI_XING);
        const auto next = s.self_buff_stacks[BUFF] ? s.self_buff_tick_at[BUFF] : s.now + 16;
        SetBuff(s, e.Log(), false, BUFF, s.now + 128, next, 1);
        e.Timers().ArmAt(yunz_timers_[BUFF - YUN_ZHONG_SUI_XING_BUFF], next);
        e.Timers().ArmAt(buff_expire_[BUFF], s.now + 128);
    }

    template <typename Engine>
    void AddRenJianDot(Engine &e)
    {
        auto      &s      = e.MutableState();
        const auto next   = s.now + Data().dot_interval;
        const auto EXPIRY = next + static_cast<tick_t>(48 * Data().haste * 3);
        SetBuff(s, e.Log(), true, REN_JIAN_DOT, EXPIRY, next, 1);
        renjian_snapshot_ = Snapshot(s);
        e.Timers().ArmAt(renjian_tick_, next);
        e.Timers().ArmAt(renjian_expire_, EXPIRY);
    }

    template <typename Engine>
    void PulseLieYun(Engine &e)
    {
        if (e.GetState().target_distance <= 15) {
            Aura(e, LIE_YUN_BUFF, 16);
        }
    }

    template <typename Engine>
    void UpdateLieYun(Engine &e)
    {
        if (!Data().Has(LIE_YUN)) {
            return;
        }
        auto &s = e.MutableState();
        if (s.target_buff_stacks[DIE_REN] < 4) {
            if (s.target_buff_stacks[HIDDEN_LIE_YUN]) {
                SetBuff(s, e.Log(), true, HIDDEN_LIE_YUN, 0, 0, 0);
                e.Timers().Cancel(hidden_tick_);
                e.Timers().Cancel(hidden_expire_);
            }
            return;
        }
        const bool EXISTS = s.target_buff_stacks[HIDDEN_LIE_YUN] != 0;
        const auto next   = EXISTS ? s.target_buff_tick_at[HIDDEN_LIE_YUN] : s.now + 8;
        if (!EXISTS) {
            PulseLieYun(e);
        }
        SetBuff(s, e.Log(), true, HIDDEN_LIE_YUN, s.now + 384, next, 1);
        e.Timers().ArmAt(hidden_tick_, next);
        e.Timers().ArmAt(hidden_expire_, s.now + 384);
    }

    template <typename Engine>
    void Aura(Engine &e, Slot buff, tick_t duration, int stacks = 1)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("tai_xu::Aura");
        auto                       &s           = e.MutableState();
        SetBuff(s, e.Log(), buff == TUN_RI_BUFF, buff, s.now + duration, 0, stacks);
        if (buff == SUI_XING_BUFF) {
            profile_ |= SUI_XING_PROFILE | (Data().Has(GU_CHANG) ? LIVE_GU_CHANG_PROFILE : 0);
        } else if (buff == QI_SHENG_BUFF) {
            profile_ |= QI_SHENG_PROFILE;
        } else if (buff == LIE_YUN_BUFF) {
            profile_ |= LIE_YUN_PROFILE;
        } else if (buff == XUAN_MEN_BUFF) {
            profile_ = (profile_ & ~XUAN_MEN_PROFILE_MASK) | (stacks * XUAN_MEN_PROFILE_UNIT);
        } else if (buff == SET_ATTRIBUTE_BUFF) {
            profile_ |= SET_ATTRIBUTE_PROFILE;
        }
        e.Timers().ArmAt(buff_expire_[buff], s.now + duration);
    }

    template <typename Engine>
    void ChiYingProc(Engine &e)
    {
        auto &s = e.MutableState();
        if (!s.self_buff_stacks[CHI_YING_BUFF]) {
            return;
        }
        Hit(e, ChiYingEffect, Snapshot(s));
        const auto STACKS = s.self_buff_stacks[CHI_YING_BUFF] - 1;
        SetBuff(s, e.Log(), false, CHI_YING_BUFF, STACKS ? s.self_buff_expires_at[CHI_YING_BUFF] : 0, 0, STACKS);
        if (!STACKS) {
            e.Timers().Cancel(buff_expire_[CHI_YING_BUFF]);
        }
    }

    template <typename Engine>
    void CompleteField(Engine &e, Slot skill)
    {
        auto &s = e.MutableState();
        if (s.preparing_skill != skill) {
            throw std::logic_error("stale prepare event");
        }
        SetPrepare(s, e.Log(), skill, 0, false);
        s.last_skill = skill;
        CooldownAt(e, skill, s.now + (skill == SHENG_TAI ? Data().shengtaiji_cd : 160));
        QidianAt(e, s.qidian + 2);
        AddField(e, skill);
        if (Data().Has(CHANG_SHENG)) {
            Aura(e, CHI_YING_BUFF, 240, std::min(3, s.self_buff_stacks[CHI_YING_BUFF] + 1));
        }
        Record(e, SuiXingEffect + skill - SUI_XING, Snapshot(s), { 0, RollResult::HIT });
    }

    template <typename Engine>
    void PulseField(Engine &e, Slot skill)
    {
        if (skill == SUI_XING) {
            if (Data().Has(QI_SHENG)) {
                Aura(e, QI_SHENG_BUFF, 48);
            }
            Aura(e, SUI_XING_BUFF, 64);
        } else if (skill == SHENG_TAI) {
            if (Data().Has(QI_SHENG)) {
                Aura(e, QI_SHENG_BUFF, 48);
            }
        } else {
            Aura(e, TUN_RI_BUFF, 20);
        }
    }

    template <typename Engine>
    void PublishField(Engine &e, Slot skill)
    {
        int    count  = 0;
        tick_t expiry = std::numeric_limits<tick_t>::max();
        tick_t next   = expiry;
        for (const auto &field : fields_) {
            if (field.skill == skill) {
                ++count;
                expiry = std::min(expiry, field.expires_at);
                next   = std::min(next, field.tick_at);
            }
        }
        SetBuff(e.MutableState(), e.Log(), false, FIELD_SUI_XING + skill - SUI_XING, count ? expiry : 0, count ? next : 0, count);
    }

    template <typename Engine>
    void AddField(Engine &e, Slot skill)
    {
        Slot slot = INVALID_SLOT;
        for (Slot i = 0; i < fields_.size(); ++i) {
            if (fields_[i].skill == INVALID_SLOT) {
                slot = i;
                break;
            }
        }
        if (slot == INVALID_SLOT) {
            slot                 = OldestField();
            const auto OLD_SKILL = fields_[slot].skill;
            fields_[slot]        = {};
            e.Timers().Cancel(field_timers_[slot]);
            PublishField(e, OLD_SKILL);
        }
        auto      &field = fields_[slot];
        const auto NOW   = e.GetState().now;
        field = { skill, NOW + (skill == SUI_XING ? Data().suixing_duration : 384), NOW + (skill == TUN_RI ? 20 : 48), field_order_++ };
        e.Timers().ArmAt(field_timers_[slot], field.tick_at);
        PublishField(e, skill);
        PulseField(e, skill);
    }

    Slot OldestField() const
    {
        Slot result = INVALID_SLOT;
        for (Slot i = 0; i < fields_.size(); ++i) {
            if (fields_[i].skill != INVALID_SLOT && (result == INVALID_SLOT || fields_[i].order < fields_[result].order))
            {
                result = i;
            }
        }
        return result;
    }

    template <typename Engine>
    void TickField(Engine &e, Slot slot)
    {
        [[maybe_unused]] const auto FRAME_SCOPE = RuleFunctionScope<Engine>("tai_xu::TickField");
        auto                       &field       = fields_[slot];
        const auto                  SKILL       = field.skill;
        const auto                  NOW         = e.GetState().now;
        if (field.tick_at == NOW) {
            PulseField(e, SKILL);
            field.tick_at += SKILL == TUN_RI ? 20 : 48;
        }
        if (field.expires_at == NOW) {
            field = {};
        } else {
            e.Timers().ArmAt(field_timers_[slot], std::min(field.tick_at, field.expires_at));
        }
        PublishField(e, SKILL);
    }

    std::shared_ptr<const Prepared>                 prepared_;
    Rng                                             rng_;
    [[no_unique_address]] QidianPhase               qidian_phase_;
    TargetTimeline                                  target_timeline_;
    DeadlineQueue::TimerId                          natural_qidian_{};
    std::array<DeadlineQueue::TimerId, SKILL_COUNT> cooldown_{};
    std::array<DeadlineQueue::TimerId, SKILL_COUNT> casting_{};
    std::array<DeadlineQueue::TimerId, 3>           prepare_{}, field_timers_{};
    DeadlineQueue::TimerId                          jinghua_prepare_{};
    DeadlineQueue::TimerId                          weapon_cw_expire_{}, wanxiang_tick_{}, wanxiang_expire_{};
    std::array<DeadlineQueue::TimerId, BUFF_COUNT>  buff_expire_{};
    std::array<FieldInstance, 3>                    fields_{};
    std::uint64_t                                   field_order_ = 0;
    DeadlineQueue::TimerId gcd_{}, dot_tick_{}, dot_expire_{}, purple_tick_{}, purple_expire_{}, feng_expire_{};
    DeadlineQueue::TimerId hidden_tick_{}, hidden_expire_{}, renjian_tick_{}, renjian_expire_{}, jianru_tick_{};
    std::array<DeadlineQueue::TimerId, 3> yunz_timers_{};
    tick_t                                recharge_at_        = 0;
    tick_t                                weapon_cw_ready_at_ = 0;
    AttributeVersionId dot_snapshot_ = 0, renjian_snapshot_ = 0, wanxiang_snapshot_ = 0, current_version_ = 0;
    AttributeVersions<AttributeVersion> versions_;

    struct DamageCacheEntry
    {
        std::uint32_t generation = 0, snapshot = 0, live = 0, effect_roll = 0;
        value_t       damage = 0;
    };

    // Direct-mapped, bounded and worker-local. Full keys make collisions harmless.
    mutable std::array<DamageCacheEntry, 1024> damage_cache_{};
    std::uint32_t                              damage_generation_ = 0;
    team::Modifiers                            team_modifiers_{};
    std::uint32_t                              team_generation_ = 0, captured_generation_ = 0;
    DeadlineQueue::TimerId                     team_application_{}, team_drum_{};
    std::size_t                                team_cursor_               = 0;
    tick_t                                     jianru_tick_at_            = std::numeric_limits<tick_t>::max();
    std::uint16_t                              profile_                   = 0;
    int                                        you_ren_crit_basis_points_ = 0;
};

using Rules = BasicRules<>;

class DamageReducer
{
public:
    explicit DamageReducer(std::shared_ptr<const Prepared> prepared) : prepared_(std::move(prepared)) { }

    void Setup(std::size_t damage_capacity)
    {
        prepared_->ValidateCapacity(damage_capacity);
        versions_.reserve(prepared_->config.attribute_version_capacity);
    }

    template <class SourceRules>
    void Bind(const SourceRules &source)
    {
        if (source.Versions().Size() > versions_.capacity()) {
            throw std::length_error("attribute version capacity exhausted");
        }
        versions_.clear();
        for (const auto &version : source.Versions()) {
            versions_.push_back(prepared_->Resolve(version.profile, version.team));
        }
    }

    value_t Reduce(const DamageIntent &hit) const
    {
        return prepared_->Damage(versions_[hit.snapshot_version], versions_[hit.live_version], hit.effect, hit.outcome == RollResult::DOUBLE);
    }

private:
    std::shared_ptr<const Prepared> prepared_;
    std::vector<AttributeVersion>   versions_;
};

struct Module
{
    static constexpr bool replay_strain = true;
    using Rules                         = tai_xu::Rules;
    using DamageReducer                 = tai_xu::DamageReducer;

    static auto Prepare(const Config &config) { return tai_xu::Prepare(config); }
};

} // namespace JX3DPS::runtime::tai_xu
#endif
