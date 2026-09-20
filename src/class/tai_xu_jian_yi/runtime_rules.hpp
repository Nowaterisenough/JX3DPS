#ifndef JX3DPS_TAI_XU_RUNTIME_RULES_HPP
#define JX3DPS_TAI_XU_RUNTIME_RULES_HPP

#include "runtime_data.hpp"

namespace JX3DPS::runtime::tai_xu {

struct SplitMixRolls {
    std::uint64_t state = 0;
    void Reset(std::uint64_t seed) { state = seed; }
    std::uint64_t Next() {
        auto z = (state += 0x9e3779b97f4a7c15ULL);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }
};

struct FieldInstance {
    Slot skill = INVALID_SLOT;
    tick_t expires_at = 0, tick_at = 0;
    std::uint64_t order = 0;
    bool operator==(const FieldInstance &) const = default;
};

struct NaturalQidianPhase {
    tick_t operator()(std::uint64_t seed) const {
        // Legacy uses a separate integer RNG for the initial 0..15-frame phase.
        // Keep phase sampling independent of critical/proc rolls as well.
        SplitMixRolls phases{seed ^ 0x6a09e667f3bcc909ULL};
        return static_cast<tick_t>(phases.Next() & 15);
    }
};

// Rng is a compile-time policy so tests can inject old rolls without adding
// a replay/debug branch to the batch loop.
template <typename Rng = SplitMixRolls, typename QidianPhase = NaturalQidianPhase>
class BasicRules {
public:
    static constexpr bool owns_last_skill = true;
    explicit BasicRules(std::shared_ptr<const Prepared> prepared = Prepare(), Rng rng = {}, QidianPhase phase = {})
        : prepared_(std::move(prepared)), rng_(std::move(rng)), qidian_phase_(std::move(phase)) {
        if (!prepared_) throw std::invalid_argument("missing prepared Tai Xu configuration");
    }
    const Prepared &Data() const { return *prepared_; }
    std::uint16_t CurrentProfile() const { return profile_; }
    int CurrentCriticalBonus() const { return you_ren_crit_basis_points_; }
    const std::array<FieldInstance, 3> &Fields() const { return fields_; }

    template <typename Engine> void Setup(Engine &e) {
        auto &s = e.MutableState();
        if (s.skill_ready_at.size() != SkillCount || s.self_buff_stacks.size() != BuffCount)
            throw std::invalid_argument("macro was not compiled for the Tai Xu ruleset");
        if (Data().max_damage && e.Log().DamageCapacity() > static_cast<std::size_t>(std::numeric_limits<value_t>::max()/Data().max_damage))
            throw std::invalid_argument("damage capacity and attributes could overflow the fight total");
        for (Slot skill : CastableSkills) {
            if (UsesGcd(skill)) s.skill_gcd_group[skill] = 0;
            cooldown_[skill] = e.Timers().Create({TimerKind::SkillCooldown, skill}, 0);
            casting_[skill] = e.Timers().Create({TimerKind::SkillCasting, skill}, 0);
        }
        for (Slot skill = SuiXing; skill <= TunRi; ++skill)
            prepare_[skill-SuiXing] = e.Timers().Create({TimerKind::SkillPrepare, skill}, 0);
        jinghua_prepare_ = e.Timers().Create({TimerKind::SkillPrepare, JingHua}, 0);
        for (Slot slot = 0; slot < fields_.size(); ++slot)
            field_timers_[slot] = e.Timers().Create({TimerKind::Field, slot}, 5);
        for (Slot buff : {SuiXingBuff, TunRiBuff, QiShengBuff, ChiYingBuff, XuanMenBuff, LieYunBuff, JianRuBuff})
            buff_expire_[buff] = e.Timers().Create({TimerKind::BuffDuration, buff, buff == TunRiBuff}, 30);
        for (Slot buff : {YunZhongSuiXingBuff, YunZhongShengTaiBuff, YunZhongTunRiBuff, JingHuaBuff})
            buff_expire_[buff] = e.Timers().Create({TimerKind::BuffDuration, buff}, 30);
        weapon_cw_expire_ = e.Timers().Create({TimerKind::BuffDuration, WeaponCwBuff}, 30);
        buff_expire_[SetAttributeBuff] = e.Timers().Create({TimerKind::BuffDuration, SetAttributeBuff}, 30);
        buff_expire_[YouRenBuff] = e.Timers().Create({TimerKind::BuffDuration, YouRenBuff}, 30);
        wanxiang_tick_ = e.Timers().Create({TimerKind::BuffInterval, WanXiangBuff, true}, 9);
        wanxiang_expire_ = e.Timers().Create({TimerKind::BuffDuration, WanXiangBuff, true}, 30);
        hidden_tick_ = e.Timers().Create({TimerKind::BuffInterval, HiddenLieYun, true}, 5);
        hidden_expire_ = e.Timers().Create({TimerKind::BuffDuration, HiddenLieYun, true}, 6);
        renjian_tick_ = e.Timers().Create({TimerKind::BuffInterval, RenJianDot, true}, 12);
        renjian_expire_ = e.Timers().Create({TimerKind::BuffDuration, RenJianDot, true}, 13);
        jianru_tick_ = e.Timers().Create({TimerKind::BuffInterval, JianRuBuff}, 12);
        yunz_timers_[0] = e.Timers().Create({TimerKind::BuffInterval, YunZhongSuiXingBuff}, 12);
        yunz_timers_[1] = e.Timers().Create({TimerKind::BuffInterval, YunZhongShengTaiBuff}, 12);
        yunz_timers_[2] = e.Timers().Create({TimerKind::BuffInterval, YunZhongTunRiBuff}, 12);
        gcd_ = e.Timers().Create({TimerKind::MacroWake}, 0);
        natural_qidian_ = e.Timers().Create({TimerKind::ResourceRegen}, 19);
        // Each Buff executes its last tick before clearing. Independent buffs
        // have a stable order; cross-buff legacy container order is not assumed.
        dot_tick_ = e.Timers().Create({TimerKind::BuffInterval, DieRen, true}, 10);
        dot_expire_ = e.Timers().Create({TimerKind::BuffDuration, DieRen, true}, 11);
        purple_tick_ = e.Timers().Create({TimerKind::BuffInterval, Purple}, 20);
        purple_expire_ = e.Timers().Create({TimerKind::BuffDuration, Purple}, 21);
        feng_expire_ = e.Timers().Create({TimerKind::BuffDuration, FengShi}, 30);
    }
    template <typename Engine> void Reset(Engine &e, std::uint64_t seed) {
        auto &s = e.MutableState();
        rng_.Reset(seed);
        dot_snapshot_ = renjian_snapshot_ = 0;
        profile_ = 0;
        you_ren_crit_basis_points_ = 0;
        recharge_at_ = 0;
        weapon_cw_ready_at_ = 16 * 30;
        wanxiang_snapshot_ = 0;
        fields_.fill({});
        field_order_ = 0;
        jianru_tick_at_ = std::numeric_limits<tick_t>::max();
        // Cloning the legacy player activates ClassFeature and immediately adds
        // one point; phase zero adds another before the first macro evaluation.
        s.qidian = std::min(10, Data().config.initial_qidian + 1);
        const auto phase = qidian_phase_(seed);
        if (phase < 0 || phase >= 16) throw std::invalid_argument("invalid natural qidian phase");
        e.Timers().ArmAt(natural_qidian_, phase);
        s.target_life = Data().config.target.life;
        s.skill_energy[ZiQi] = Data().purple_charges;
    }
    template <typename Engine> bool Cast(Engine &e, const MacroLine &line) {
        auto &s = e.MutableState();
        const auto skill = line.action_slot;
        if (!IsCastable(skill)) throw std::invalid_argument("damage-only or unmigrated skill cannot be cast");
        if (s.preparing_skill != INVALID_SLOT) {
            if constexpr (Engine::debug_enabled)
                e.RejectAction(ActionBlockReason::Preparing, s.skill_prepare_at[s.preparing_skill] - s.now, 0, s.preparing_skill);
            return false; // /fcast cannot bypass IsCast in ba960f7.
        }
        if (s.casting_skill != INVALID_SLOT) {
            if (!IsForceCast(line.command)) {
                if constexpr (Engine::debug_enabled)
                    e.RejectAction(ActionBlockReason::Channeling, s.skill_casting_at[s.casting_skill] - s.now, 0, s.casting_skill);
                return false;
            }
            if constexpr (Engine::debug_enabled) e.NoteChannelInterrupt(s.casting_skill);
            InterruptCasting(e);
        }
        const bool free_bahuang = skill == BaHuang && s.self_buff_stacks[WeaponCwBuff];
        const auto ready_at = free_bahuang && s.skill_gcd_group[skill] != INVALID_SLOT
            ? s.gcd_ready_at[s.skill_gcd_group[skill]] : s.SkillReadyAt(skill);
        if (ready_at > s.now) {
            if constexpr (Engine::debug_enabled) {
                const auto reason = !free_bahuang && s.skill_ready_at[skill] == ready_at
                    ? ActionBlockReason::SkillCooldown : ActionBlockReason::GlobalCooldown;
                e.RejectAction(reason, ready_at - s.now, 0, skill);
            }
            return false;
        }
        if (skill == WuWo && s.qidian == 0) {
            if constexpr (Engine::debug_enabled) e.RejectAction(ActionBlockReason::Qidian, s.qidian, 1);
            return false;
        }
        if (skill == ZiQi && s.skill_energy[ZiQi] == 0) {
            if constexpr (Engine::debug_enabled) e.RejectAction(ActionBlockReason::Charges, s.skill_energy[ZiQi], 1, ZiQi);
            return false;
        }
        double range = skill == BaHuang ? 12 : 4;
        if (skill == WanJian || skill == JingHua) range = 8;
        else if (IsRenJian(skill)) range = Data().renjian_range;
        if ((skill == BaHuang || skill == SanHuan) && Data().Has(FengShiTalent)) range += 2;
        if (skill != ZiQi && !IsFieldSkill(skill) && Data().config.target.distance > range) {
            if constexpr (Engine::debug_enabled) e.RejectAction(ActionBlockReason::Distance, Data().config.target.distance, range);
            return false;
        }
        if (skill == JingHua && (!Data().Has(JingHuaTalent) || !s.self_buff_stacks[JingHuaBuff])) {
            if constexpr (Engine::debug_enabled) e.RejectAction(
                Data().Has(JingHuaTalent) ? ActionBlockReason::MissingBuff : ActionBlockReason::MissingTalent,
                s.self_buff_stacks[JingHuaBuff], 1, JingHuaBuff);
            return false;
        }
        if (skill != SanChai && !IsFieldSkill(skill)) s.last_skill = skill;
        if (UsesGcd(skill)) {
            SetGcd(s, e.Log(), 0, s.now + Data().gcd);
            e.Timers().ArmAt(gcd_, s.gcd_ready_at[0]);
        }
        if (IsFieldSkill(skill)) {
            const auto duration = skill == ShengTai ? Data().shengtaiji_prepare : Data().field_prepare;
            SetPrepare(s, e.Log(), skill, s.now + duration, true);
            SetSkillCooldown(s, e.Log(), skill, std::numeric_limits<tick_t>::max());
            e.Timers().ArmAt(prepare_[skill-SuiXing], s.now + duration);
            return true;
        }
        if (skill == JingHua) {
            SetSkillCooldown(s, e.Log(), JingHua, std::numeric_limits<tick_t>::max());
            SetPrepare(s, e.Log(), JingHua, s.now + Data().jinghua_prepare, true);
            e.Timers().ArmAt(jinghua_prepare_, s.now + Data().jinghua_prepare);
            Hit(e, JingHuaEffect, Snapshot(s));
            ChiYingProc(e);
            ShenMaiProc(e);
            TryWeaponCw(e);
            TrySetAttribute(e);
            return true;
        }
        switch (skill) {
            case WuWo: CastWuWo(e); break;
            case BaHuang: CastBaHuang(e); break;
            case SanHuan: CastSanHuan(e); break;
            case SanChai: {
                CooldownAt(e, SanChai, s.now + Data().sanchai_cd);
                const auto snapshot = Snapshot(s);
                const auto roll = Roll(SanChaiEffect, snapshot);
                ShenMaiProc(e);
                Record(e, SanChaiEffect, snapshot, roll);
                TryWeaponCw(e);
                TrySetAttribute(e);
                break;
            }
            case ZiQi: CastPurple(e); break;
            case WanJian: CastWanJian(e); break;
            case RenJian: case RenJianSuiXing: CastRenJian(e, skill); break;
        }
        return true;
    }
    template <typename Engine> void OnTimer(Engine &e, TimerHandle handle, DeadlineQueue::TimerId) {
        auto &s = e.MutableState();
        if (handle.kind == TimerKind::ResourceRegen) {
            if (s.qidian < 10) QidianAt(e, s.qidian + 1);
            e.Timers().ArmAt(natural_qidian_, s.now + 16);
        } else if (handle.kind == TimerKind::SkillPrepare && handle.slot == JingHua) {
            CompleteJingHua(e);
        } else if (handle.kind == TimerKind::SkillPrepare) {
            CompleteField(e, handle.slot);
        } else if (handle.kind == TimerKind::SkillCasting) {
            CompleteCasting(e, handle.slot);
        } else if (handle.kind == TimerKind::Field) {
            TickField(e, handle.slot);
        } else if (handle.kind == TimerKind::SkillCooldown && handle.slot == ZiQi) {
            SetSkillEnergy(s, e.Log(), ZiQi, s.skill_energy[ZiQi] + 1);
            if (s.skill_energy[ZiQi] < Data().purple_charges) {
                recharge_at_ = s.now + Data().purple_cd;
                e.Timers().ArmAt(cooldown_[ZiQi], recharge_at_);
            } else recharge_at_ = 0;
            SetSkillCooldown(s, e.Log(), ZiQi, 0);
        } else if (handle.kind == TimerKind::BuffInterval && handle.slot == HiddenLieYun) {
            PulseLieYun(e);
            const auto next = s.now + 8;
            SetBuff(s, e.Log(), true, HiddenLieYun, s.target_buff_expires_at[HiddenLieYun], next, 1);
            e.Timers().ArmAt(hidden_tick_, next);
        } else if (handle.kind == TimerKind::BuffInterval && handle.slot == RenJianDot) {
            DotHit(e, RenJianDotEffect, renjian_snapshot_);
            const auto next = s.now + Data().dot_interval;
            SetBuff(s, e.Log(), true, RenJianDot, s.target_buff_expires_at[RenJianDot], next, 1);
            e.Timers().ArmAt(renjian_tick_, next);
        } else if (handle.kind == TimerKind::BuffInterval && handle.slot == JianRuBuff) {
            Hit(e, JianRuEffect, Snapshot(s));
            const auto next = s.now + 16;
            jianru_tick_at_ = next;
            SetBuff(s, e.Log(), false, JianRuBuff, s.self_buff_expires_at[JianRuBuff], next, 1);
            e.Timers().ArmAt(jianru_tick_, next);
        } else if (handle.kind == TimerKind::BuffInterval && handle.slot >= YunZhongSuiXingBuff && handle.slot <= YunZhongTunRiBuff) {
            const auto effect = static_cast<std::uint16_t>(YunZhongSuiXingEffect + handle.slot - YunZhongSuiXingBuff);
            if (Data().config.target.distance <= 6) Hit(e, effect, Snapshot(s));
            ActivateJianRu(e);
            const auto next = s.now + 16;
            SetBuff(s, e.Log(), false, handle.slot, s.self_buff_expires_at[handle.slot], next, 1);
            e.Timers().ArmAt(yunz_timers_[handle.slot-YunZhongSuiXingBuff], next);
        } else if (handle.kind == TimerKind::BuffInterval && handle.target && handle.slot == WanXiangBuff) {
            const int stacks = s.target_buff_stacks[WanXiangBuff];
            if (!stacks) return;
            DotHit(e, static_cast<std::uint16_t>(WanXiangEffect + std::min(stacks, 3) - 1), wanxiang_snapshot_, stacks);
            const auto next = s.now + Data().dot_interval;
            SetBuff(s, e.Log(), true, WanXiangBuff, s.target_buff_expires_at[WanXiangBuff], next, stacks);
            e.Timers().ArmAt(wanxiang_tick_, next);
        } else if (handle.kind == TimerKind::BuffInterval && handle.target) {
            DotHit(e, DotEffect(s.target_buff_stacks[DieRen], 1), dot_snapshot_);
            const auto next = s.now + Data().dot_interval;
            SetBuff(s, e.Log(), true, DieRen, s.target_buff_expires_at[DieRen], next, s.target_buff_stacks[DieRen]);
            e.Timers().ArmAt(dot_tick_, next);
        } else if (handle.kind == TimerKind::BuffInterval) {
            QidianAt(e, s.qidian + 2);
            const auto next = s.now + 16;
            SetBuff(s, e.Log(), false, Purple, s.self_buff_expires_at[Purple], next, 1);
            e.Timers().ArmAt(purple_tick_, next);
        } else if (handle.kind == TimerKind::BuffDuration && handle.slot == WeaponCwBuff) {
            SetBuff(s, e.Log(), false, WeaponCwBuff, 0, 0, 0);
            SetSkillCooldown(s, e.Log(), BaHuang, s.now);
            e.Timers().Cancel(cooldown_[BaHuang]);
        } else if (handle.kind == TimerKind::BuffDuration && handle.target && handle.slot == WanXiangBuff) {
            SetBuff(s, e.Log(), true, WanXiangBuff, 0, 0, 0);
            e.Timers().Cancel(wanxiang_tick_);
        } else if (handle.kind == TimerKind::BuffDuration) {
            if (handle.target && handle.slot == DieRen) ClearDot(e);
            else {
                SetBuff(s, e.Log(), handle.target, handle.slot, 0, 0, 0);
                if (handle.slot == Purple) {
                    profile_ &= ~PurpleProfile;
                    e.Timers().Cancel(purple_tick_);
                } else if (handle.slot == SuiXingBuff) profile_ &= ~(SuiXingProfile|LiveGuChangProfile);
                else if (handle.slot == QiShengBuff) profile_ &= ~QiShengProfile;
                else if (handle.slot == XuanMenBuff) profile_ &= ~XuanMenProfileMask;
                else if (handle.slot == LieYunBuff) profile_ &= ~LieYunProfile;
                else if (handle.slot == HiddenLieYun) e.Timers().Cancel(hidden_tick_);
                else if (handle.slot == RenJianDot) e.Timers().Cancel(renjian_tick_);
                else if (handle.slot == JianRuBuff) {
                    e.Timers().Cancel(jianru_tick_);
                    jianru_tick_at_ = std::numeric_limits<tick_t>::max();
                }
                else if (handle.slot >= YunZhongSuiXingBuff && handle.slot <= YunZhongTunRiBuff)
                    e.Timers().Cancel(yunz_timers_[handle.slot-YunZhongSuiXingBuff]);
                else if (handle.slot == JingHuaBuff) {
                    SetSkillCooldown(s, e.Log(), JingHua, std::numeric_limits<tick_t>::max());
                } else if (handle.slot == SetAttributeBuff) profile_ &= ~SetAttributeProfile;
                else if (handle.slot == YouRenBuff) you_ren_crit_basis_points_ = 0;
            }
        }
    }
    value_t Reduce(const DamageIntent &hit) const {
        return Data().damage[hit.snapshot][hit.effect][static_cast<unsigned>(hit.outcome)];
    }
    value_t Reduce(const DamageIntent &hit, const State &) const { return Reduce(hit); }

private:
    struct Rolled { std::uint64_t key; RollResult outcome; };
    static double UnitSample(std::uint64_t key) {
        // Validation policies can preserve the exact legacy floating-point draw.
        if constexpr (requires { Rng::UnitSample(key); }) return Rng::UnitSample(key);
        else return static_cast<double>(key >> 11) * 0x1.0p-53;
    }
    Rolled Roll(std::uint16_t effect, std::uint16_t snapshot) {
        const auto key = rng_.Next();
        const auto sample = UnitSample(key);
        const auto chance = Data().critical_chance[ChanceProfile(snapshot)][effect]
            + you_ren_crit_basis_points_ * 1.0 / 10000;
        return {key, sample < chance ? RollResult::DOUBLE : RollResult::HIT};
    }
    // Attribute profile changes only on aura transitions, never per hit.
    std::uint16_t Snapshot(const State &) const { return profile_; }
    static std::uint16_t DotEffect(int stacks, int count) {
        return static_cast<std::uint16_t>(DieRenEffect + (stacks-1)*9 + count);
    }
    template <typename Engine> void Record(Engine &e, std::uint16_t effect, std::uint16_t snapshot,
                                            Rolled roll, int level = -1, int sub = -1) {
        const auto &f = Formulas[effect];
        e.Log().Record(e.GetState().now, f.skill, f.kind == FormulaKind::Zero ? INVALID_SLOT : 0,
            static_cast<std::uint16_t>(level < 0 ? f.level : level), roll.key, 0, 0, roll.outcome, effect, snapshot,
            static_cast<std::uint16_t>(sub < 0 ? f.sub : sub));
    }
    template <typename Engine> RollResult Hit(Engine &e, std::uint16_t effect, std::uint16_t snapshot, int level = -1, int sub = -1) {
        const auto roll = Roll(effect, snapshot);
        Record(e, effect, snapshot, roll, level, sub);
        return roll.outcome;
    }
    template <typename Engine> void DotHit(Engine &e, std::uint16_t effect, std::uint16_t snapshot, int sub = -1) {
        // Crit chance is snapshotted, while overcome and shield ignore are live.
        const auto roll = Roll(effect, snapshot);
        Record(e, effect, (snapshot & ~LiveProfileMask) | (profile_ & LiveProfileMask), roll, -1, sub);
    }
    template <typename Engine> void QidianAt(Engine &e, int qidian) {
        auto &s = e.MutableState();
        SetResources(s, e.Log(), std::clamp(qidian, 0, 10), s.energy, s.rage);
    }
    template <typename Engine> void CooldownAt(Engine &e, Slot slot, tick_t deadline) {
        SetSkillCooldown(e.MutableState(), e.Log(), slot, deadline);
        e.Timers().ArmAt(cooldown_[slot], deadline);
    }
    template <typename Engine> void StartCasting(Engine &e, Slot skill, tick_t duration) {
        if (!IsCastable(skill) || duration <= 0) throw std::invalid_argument("invalid channel duration");
        auto &s = e.MutableState();
        if (s.preparing_skill != INVALID_SLOT || s.casting_skill != INVALID_SLOT)
            throw std::logic_error("a cast is already in progress");
        const auto finish = s.now + duration;
        SetCasting(s, e.Log(), skill, finish, true);
        e.Timers().ArmAt(casting_[skill], finish);
    }
    template <typename Engine> void InterruptCasting(Engine &e) {
        auto &s = e.MutableState();
        if (s.casting_skill == INVALID_SLOT) return;
        const auto skill = s.casting_skill;
        SetCasting(s, e.Log(), skill, 0, false);
        e.Timers().Cancel(casting_[skill]);
    }
    template <typename Engine> void CompleteCasting(Engine &e, Slot skill) {
        auto &s = e.MutableState();
        if (s.casting_skill != skill || s.skill_casting_at[skill] != s.now) return;
        SetCasting(s, e.Log(), skill, 0, false);
    }
    bool Proc(int numerator, int denominator) {
        if constexpr (requires { rng_.Proc(numerator, denominator); }) return rng_.Proc(numerator, denominator);
        else {
            const auto key = rng_.Next();
            return UnitSample(key) < static_cast<double>(numerator) / denominator;
        }
    }
    template <typename Engine> void TriggerYouRen(Engine &e, RollResult outcome) {
        if (!Data().HasTeam(YouRen) || outcome != RollResult::DOUBLE) return;
        auto &s = e.MutableState();
        const int old_stacks = s.self_buff_stacks[YouRenBuff];
        const int stacks = std::min(5, old_stacks + 1);
        you_ren_crit_basis_points_ += (stacks - old_stacks) * 100;
        SetBuff(s, e.Log(), false, YouRenBuff, s.now + 320, 0, stacks);
        e.Timers().ArmAt(buff_expire_[YouRenBuff], s.now + 320);
    }
    template <typename Engine> void TryWeaponCw(Engine &e) {
        // Legacy rolls on every eligible hit, including during the internal CD.
        if (!Data().HasEquipment(WeaponCW) || !Proc(31, 1024) || e.GetState().now < weapon_cw_ready_at_) return;
        auto &s = e.MutableState();
        weapon_cw_ready_at_ = s.now + 16 * 30;
        SetBuff(s, e.Log(), false, WeaponCwBuff, s.now + 16 * 6, 0, 1);
        SetSkillCooldown(s, e.Log(), BaHuang, s.now);
        e.Timers().Cancel(cooldown_[BaHuang]);
        e.Timers().ArmAt(weapon_cw_expire_, s.now + 16 * 6);
    }
    template <typename Engine> void TrySetAttribute(Engine &e) {
        if (Data().HasEquipment(SetAttribute)) Aura(e, SetAttributeBuff, 96);
    }
    template <typename Engine> void AddWanXiang(Engine &e) {
        if (!e.GetState().self_buff_stacks[WeaponCwBuff]) return;
        auto &s = e.MutableState();
        const bool exists = s.target_buff_stacks[WanXiangBuff] != 0;
        const auto next = exists ? s.target_buff_tick_at[WanXiangBuff] : s.now + Data().dot_interval;
        const auto expiry = next + Data().dot_interval * 9;
        const auto stacks = std::min(3, s.target_buff_stacks[WanXiangBuff] + 1);
        SetBuff(s, e.Log(), true, WanXiangBuff, expiry, next, stacks);
        wanxiang_snapshot_ = Snapshot(s) & ~LiveGuChangProfile;
        e.Timers().ArmAt(wanxiang_tick_, next);
        e.Timers().ArmAt(wanxiang_expire_, expiry);
    }
    template <typename Engine> void ShenMaiProc(Engine &e) {
        // The pinned implementation gives two points even on a non-critical hit.
        if (Data().Has(ShenMai)) QidianAt(e, e.GetState().qidian + 2);
    }
    template <typename Engine> void AddFeng(Engine &e) {
        if (!Data().Has(FengShiTalent)) return;
        auto &s = e.MutableState();
        SetBuff(s, e.Log(), false, FengShi, s.now + 80, 0, 1);
        e.Timers().ArmAt(feng_expire_, s.now + 80);
    }
    template <typename Engine> void AddDot(Engine &e, int stacks) {
        auto &s = e.MutableState();
        auto next = s.target_buff_stacks[DieRen] ? s.target_buff_tick_at[DieRen] : s.now + Data().dot_interval;
        const auto expiry = next + static_cast<tick_t>(48*Data().haste*7);
        SetBuff(s, e.Log(), true, DieRen, expiry, next, std::min(Data().dot_max_stacks, s.target_buff_stacks[DieRen] + stacks));
        UpdateLieYun(e); // The immediate threshold proc precedes the new DOT snapshot.
        dot_snapshot_ = Snapshot(s) & ~LiveGuChangProfile;
        e.Timers().ArmAt(dot_tick_, next);
        e.Timers().ArmAt(dot_expire_, expiry);
    }
    template <typename Engine> void ClearDot(Engine &e, bool erase_first = false) {
        if (!erase_first) UpdateLieYun(e);
        SetBuff(e.MutableState(), e.Log(), true, DieRen, 0, 0, 0);
        if (erase_first) UpdateLieYun(e);
        e.Timers().Cancel(dot_tick_);
        e.Timers().Cancel(dot_expire_);
    }
    template <typename Engine> void CastWuWo(Engine &e) {
        auto &s = e.MutableState();
        const auto qidian = s.qidian;
        const auto effect = static_cast<std::uint16_t>((s.self_buff_stacks[FengShi] ? FengWuWoEffect : WuWoEffect) + qidian - 1);
        QidianAt(e, (Data().config.recipes[WuWo]&Qidian) ? 1 : 0);
        const auto roll = Hit(e, effect, Snapshot(s));
        TriggerYouRen(e, roll);
        if (qidian >= 6) Hit(e, PoZhaoEffect + (qidian-6)/2, Snapshot(s));
        if (Data().Has(WuYu) && s.target_buff_stacks[DieRen])
            CooldownAt(e, BaHuang, std::max(s.now, s.skill_ready_at[BaHuang] - (qidian+1)*8));
        ChiYingProc(e);
        if (Data().Has(DieRenTalent)) AddDot(e, 1 + static_cast<int>(roll));
        ShenMaiProc(e);
        if (Data().Has(FengShiTalent)) {
            SetBuff(s, e.Log(), false, FengShi, 0, 0, 0);
            e.Timers().Cancel(feng_expire_);
            AddDot(e, 1); // Legacy wrapper does this even when FengShi was absent.
        }
        TryWeaponCw(e);
        TrySetAttribute(e);
        TriggerYouRen(e, roll);
    }
    template <typename Engine> void CastBaHuang(Engine &e) {
        auto &s = e.MutableState();
        const bool weapon_window = s.self_buff_stacks[WeaponCwBuff] != 0;
        if (weapon_window) {
            SetSkillCooldown(s, e.Log(), BaHuang, s.now);
            e.Timers().Cancel(cooldown_[BaHuang]);
        } else CooldownAt(e, BaHuang, s.now + Data().bahuang_cd);
        QidianAt(e, s.qidian + 2 + ((Data().config.recipes[BaHuang]&Qidian) ? 1 : 0));
        const int level = std::clamp(static_cast<int>(10 - (s.target_life-.01)*10), 0, 9);
        const auto first_roll = Hit(e, BaHuangEffect, Snapshot(s), level);
        Hit(e, weapon_window ? BaHuangCwEffect : BaHuangExtraEffect + level, Snapshot(s));
        ShenMaiProc(e);
        AddFeng(e);
        AddOrTriggerJianRu(e);
        if (Data().Has(QieYu) && s.target_buff_stacks[DieRen] &&
            (s.target_life <= .4 || (s.self_buff_stacks[Purple] && s.target_life <= .6))) {
            const int count = static_cast<int>((s.target_buff_expires_at[DieRen]-s.now)/48 * Data().haste);
            DotHit(e, DotEffect(s.target_buff_stacks[DieRen], count), dot_snapshot_, 1);
            ClearDot(e);
        }
        ChiYingProc(e);
        TryWeaponCw(e);
        AddWanXiang(e);
        if (Data().HasEquipment(WeaponCW) && Proc(307, 1024)) Hit(e, WeaponCwExtraEffect, Snapshot(s));
        TrySetAttribute(e);
        TriggerYouRen(e, first_roll);
    }
    template <typename Engine> void CastSanHuan(Engine &e) {
        auto &s = e.MutableState();
        CooldownAt(e, SanHuan, s.now + Data().sanhuan_cd);
        QidianAt(e, s.qidian + 2);
        const auto first_roll = Hit(e, SanHuanEffect, Snapshot(s));
        for (int i = 1; i < 3; ++i) Hit(e, SanHuanEffect, Snapshot(s));
        if (Data().Has(HuanYue)) {
            QidianAt(e, s.qidian + 4);
            for (int i = 0; i < 3; ++i) Hit(e, PoZhaoEffect, Snapshot(s), -1, 1);
            for (int i = 0; i < 3 && s.target_buff_stacks[DieRen]; ++i) {
                const auto expiry = s.target_buff_expires_at[DieRen] - Data().dot_interval;
                if (expiry >= s.now) DotHit(e, DotEffect(s.target_buff_stacks[DieRen], 1), dot_snapshot_);
                if (expiry <= s.now) ClearDot(e, true);
                else {
                    SetBuff(s, e.Log(), true, DieRen, expiry, s.target_buff_tick_at[DieRen], s.target_buff_stacks[DieRen]);
                    e.Timers().ArmAt(dot_expire_, expiry);
                }
            }
        }
        ShenMaiProc(e);
        AddFeng(e);
        ChiYingProc(e);
        TryWeaponCw(e);
        TrySetAttribute(e);
        TriggerYouRen(e, first_roll);
    }
    template <typename Engine> void CastPurple(Engine &e) {
        auto &s = e.MutableState();
        if (s.skill_energy[ZiQi] == Data().purple_charges) {
            recharge_at_ = s.now + Data().purple_cd;
            e.Timers().ArmAt(cooldown_[ZiQi], recharge_at_);
        }
        SetSkillEnergy(s, e.Log(), ZiQi, s.skill_energy[ZiQi] - 1);
        SetSkillCooldown(s, e.Log(), ZiQi, s.skill_energy[ZiQi] ? 0 : recharge_at_);
        QidianAt(e, 10);
        const auto next = s.self_buff_stacks[Purple] ? s.self_buff_tick_at[Purple] : s.now + 16;
        const auto expiry = (s.self_buff_stacks[Purple] ? s.self_buff_expires_at[Purple] : s.now) + 160;
        SetBuff(s, e.Log(), false, Purple, expiry, next, 1);
        profile_ |= PurpleProfile;
        e.Timers().ArmAt(purple_tick_, next);
        e.Timers().ArmAt(purple_expire_, expiry);
        Record(e, ZeroEffect, Snapshot(s), {0, RollResult::HIT});
    }

    template <typename Engine> void CastWanJian(Engine &e) {
        auto &s = e.MutableState();
        CooldownAt(e, WanJian, s.now + Data().wanjian_cd);
        QidianAt(e, s.qidian + 1);
        const auto roll = Hit(e, WanJianEffect, Snapshot(s));
        TriggerYouRen(e, roll);
        ShenMaiProc(e);
        ActivateJianRu(e);
        if (Data().Has(XuJi)) AddDot(e, 1);
        TriggerJingHua(e);
        TryWeaponCw(e);
        ChiYingProc(e);
        TrySetAttribute(e);
    }

    template <typename Engine> void TriggerJingHua(Engine &e) {
        if (!Data().Has(JingHuaTalent)) return;
        auto &s = e.MutableState();
        if (!s.target_buff_stacks[TunRiBuff] || s.target_buff_expires_at[TunRiBuff] <= s.now) return;
        SetBuff(s, e.Log(), false, JingHuaBuff, s.now + 128, 0, 1);
        SetSkillCooldown(s, e.Log(), JingHua, 0);
        e.Timers().Cancel(cooldown_[JingHua]);
        e.Timers().ArmAt(buff_expire_[JingHuaBuff], s.now + 128);
    }

    template <typename Engine> void AddOrTriggerJianRu(Engine &e) {
        if (!Data().Has(JianRu)) return;
        auto &s = e.MutableState();
        if (!s.self_buff_stacks[JianRuBuff]) {
            if (!s.self_buff_stacks[Purple]) return;
            SetBuff(s, e.Log(), false, JianRuBuff, s.now + 98,
                std::numeric_limits<tick_t>::max(), 1);
            jianru_tick_at_ = std::numeric_limits<tick_t>::max();
            e.Timers().ArmAt(buff_expire_[JianRuBuff], s.now + 98);
            return;
        }
        if (jianru_tick_at_ != std::numeric_limits<tick_t>::max()) {
            // An active JianRu causes the stronger immediate hit and remains active.
            Hit(e, JianRuActiveEffect, Snapshot(s), -1, 1);
            return;
        }
        // Refresh an unactivated JianRu while Purple is still present.
        if (s.self_buff_stacks[Purple]) {
            SetBuff(s, e.Log(), false, JianRuBuff, s.now + 98,
                std::numeric_limits<tick_t>::max(), 1);
            e.Timers().ArmAt(buff_expire_[JianRuBuff], s.now + 98);
        }
    }

    template <typename Engine> void ActivateJianRu(Engine &e) {
        if (!Data().Has(JianRu)) return;
        auto &s = e.MutableState();
        if (!s.self_buff_stacks[JianRuBuff] || jianru_tick_at_ != std::numeric_limits<tick_t>::max()) return;
        jianru_tick_at_ = s.now + 16;
        SetBuff(s, e.Log(), false, JianRuBuff, s.self_buff_expires_at[JianRuBuff], jianru_tick_at_, 1);
        e.Timers().ArmAt(jianru_tick_, jianru_tick_at_);
    }

    template <typename Engine> void CompleteJingHua(Engine &e) {
        auto &s = e.MutableState();
        if (s.preparing_skill != JingHua) throw std::logic_error("stale JingHua prepare event");
        SetPrepare(s, e.Log(), JingHua, 0, false);
        s.last_skill = JingHua;
        Hit(e, JingHuaSuiEffect, Snapshot(s), -1, 1);
    }

    template <typename Engine> void CastRenJian(Engine &e, Slot skill) {
        auto &s = e.MutableState();
        CooldownAt(e, RenJian, s.now + Data().renjian_cd);
        CooldownAt(e, RenJianSuiXing, s.now + Data().renjian_cd);
        int count = 0;
        bool consumed_suixing = false;
        for (unsigned i = 0; i < fields_.size(); ++i) {
            const auto slot = OldestField();
            if (slot == INVALID_SLOT) break;
            const auto field_skill = fields_[slot].skill;
            // The old variant stops consuming at the second SuiXing at the front.
            if (skill == RenJianSuiXing && field_skill == SuiXing && consumed_suixing) break;
            consumed_suixing |= field_skill == SuiXing;
            fields_[slot] = {};
            e.Timers().Cancel(field_timers_[slot]);
            PublishField(e, field_skill);
            ++count;
            AddYunZhong(e, field_skill);
            const auto effect = skill == RenJian ? RenJianEffect : RenJianSuiXingEffect;
            const auto snapshot = Snapshot(s);
            const auto roll = Roll(effect, snapshot);
            ShenMaiProc(e);
            if (Data().config.renjian_recipes & Dot) AddRenJianDot(e);
            Record(e, effect, snapshot, roll);
            TryWeaponCw(e);
        }
        ChiYingProc(e);
        if (Data().Has(XuanMen)) Aura(e, XuanMenBuff, 640, std::min(3, s.self_buff_stacks[XuanMenBuff]+count));
        TrySetAttribute(e);
    }
    template <typename Engine> void AddYunZhong(Engine &e, Slot field_skill) {
        unsigned recipe = field_skill == SuiXing ? YunZhongSuiXing :
            field_skill == ShengTai ? YunZhongShengTai : YunZhongTunRi;
        if (!(Data().config.renjian_recipes & recipe)) return;
        auto &s = e.MutableState();
        const auto buff = static_cast<Slot>(YunZhongSuiXingBuff + field_skill - SuiXing);
        const auto next = s.self_buff_stacks[buff] ? s.self_buff_tick_at[buff] : s.now + 16;
        SetBuff(s, e.Log(), false, buff, s.now + 128, next, 1);
        e.Timers().ArmAt(yunz_timers_[buff-YunZhongSuiXingBuff], next);
        e.Timers().ArmAt(buff_expire_[buff], s.now + 128);
    }
    template <typename Engine> void AddRenJianDot(Engine &e) {
        auto &s = e.MutableState();
        const auto next = s.now + Data().dot_interval;
        const auto expiry = next + static_cast<tick_t>(48*Data().haste*3);
        SetBuff(s, e.Log(), true, RenJianDot, expiry, next, 1);
        renjian_snapshot_ = Snapshot(s) & ~LiveGuChangProfile;
        e.Timers().ArmAt(renjian_tick_, next);
        e.Timers().ArmAt(renjian_expire_, expiry);
    }
    template <typename Engine> void PulseLieYun(Engine &e) {
        if (Data().config.target.distance <= 15) Aura(e, LieYunBuff, 16);
    }
    template <typename Engine> void UpdateLieYun(Engine &e) {
        if (!Data().Has(LieYun)) return;
        auto &s = e.MutableState();
        if (s.target_buff_stacks[DieRen] < 4) {
            if (s.target_buff_stacks[HiddenLieYun]) {
                SetBuff(s, e.Log(), true, HiddenLieYun, 0, 0, 0);
                e.Timers().Cancel(hidden_tick_);
                e.Timers().Cancel(hidden_expire_);
            }
            return;
        }
        const bool exists = s.target_buff_stacks[HiddenLieYun] != 0;
        const auto next = exists ? s.target_buff_tick_at[HiddenLieYun] : s.now + 8;
        if (!exists) PulseLieYun(e);
        SetBuff(s, e.Log(), true, HiddenLieYun, s.now+384, next, 1);
        e.Timers().ArmAt(hidden_tick_, next);
        e.Timers().ArmAt(hidden_expire_, s.now+384);
    }

    template <typename Engine> void Aura(Engine &e, Slot buff, tick_t duration, int stacks = 1) {
        auto &s = e.MutableState();
        SetBuff(s, e.Log(), buff == TunRiBuff, buff, s.now + duration, 0, stacks);
        if (buff == SuiXingBuff) profile_ |= SuiXingProfile | (Data().Has(GuChang) ? LiveGuChangProfile : 0);
        else if (buff == QiShengBuff) profile_ |= QiShengProfile;
        else if (buff == LieYunBuff) profile_ |= LieYunProfile;
        else if (buff == XuanMenBuff) profile_ = (profile_ & ~XuanMenProfileMask) | (stacks*XuanMenProfileUnit);
        else if (buff == SetAttributeBuff) profile_ |= SetAttributeProfile;
        e.Timers().ArmAt(buff_expire_[buff], s.now + duration);
    }
    template <typename Engine> void ChiYingProc(Engine &e) {
        auto &s = e.MutableState();
        if (!s.self_buff_stacks[ChiYingBuff]) return;
        Hit(e, ChiYingEffect, Snapshot(s));
        const auto stacks = s.self_buff_stacks[ChiYingBuff] - 1;
        SetBuff(s, e.Log(), false, ChiYingBuff, stacks ? s.self_buff_expires_at[ChiYingBuff] : 0, 0, stacks);
        if (!stacks) e.Timers().Cancel(buff_expire_[ChiYingBuff]);
    }
    template <typename Engine> void CompleteField(Engine &e, Slot skill) {
        auto &s = e.MutableState();
        if (s.preparing_skill != skill) throw std::logic_error("stale prepare event");
        SetPrepare(s, e.Log(), skill, 0, false);
        s.last_skill = skill;
        CooldownAt(e, skill, s.now + (skill == ShengTai ? Data().shengtaiji_cd : 160));
        QidianAt(e, s.qidian + 2);
        AddField(e, skill);
        if (Data().Has(ChangSheng)) Aura(e, ChiYingBuff, 240, std::min(3, s.self_buff_stacks[ChiYingBuff]+1));
        Record(e, SuiXingEffect + skill-SuiXing, Snapshot(s), {0, RollResult::HIT});
    }
    template <typename Engine> void PulseField(Engine &e, Slot skill) {
        if (skill == SuiXing) {
            if (Data().Has(QiSheng)) Aura(e, QiShengBuff, 48);
            Aura(e, SuiXingBuff, 64);
        } else if (skill == ShengTai) {
            if (Data().Has(QiSheng)) Aura(e, QiShengBuff, 48);
        } else Aura(e, TunRiBuff, 20);
    }
    template <typename Engine> void PublishField(Engine &e, Slot skill) {
        int count = 0;
        tick_t expiry = std::numeric_limits<tick_t>::max(), next = expiry;
        for (const auto &field : fields_) if (field.skill == skill) {
            ++count;
            expiry = std::min(expiry, field.expires_at);
            next = std::min(next, field.tick_at);
        }
        SetBuff(e.MutableState(), e.Log(), false, FieldSuiXing + skill-SuiXing,
            count ? expiry : 0, count ? next : 0, count);
    }
    template <typename Engine> void AddField(Engine &e, Slot skill) {
        Slot slot = INVALID_SLOT;
        for (Slot i = 0; i < fields_.size(); ++i) if (fields_[i].skill == INVALID_SLOT) { slot = i; break; }
        if (slot == INVALID_SLOT) {
            slot = OldestField();
            const auto old_skill = fields_[slot].skill;
            fields_[slot] = {};
            e.Timers().Cancel(field_timers_[slot]);
            PublishField(e, old_skill);
        }
        auto &field = fields_[slot];
        const auto now = e.GetState().now;
        field = {skill, now + (skill == SuiXing ? Data().suixing_duration : 384),
            now + (skill == TunRi ? 20 : 48), field_order_++};
        e.Timers().ArmAt(field_timers_[slot], field.tick_at);
        PublishField(e, skill);
        PulseField(e, skill);
    }
    Slot OldestField() const {
        Slot result = INVALID_SLOT;
        for (Slot i = 0; i < fields_.size(); ++i)
            if (fields_[i].skill != INVALID_SLOT && (result == INVALID_SLOT || fields_[i].order < fields_[result].order)) result = i;
        return result;
    }
    template <typename Engine> void TickField(Engine &e, Slot slot) {
        auto &field = fields_[slot];
        const auto skill = field.skill;
        const auto now = e.GetState().now;
        if (field.tick_at == now) {
            PulseField(e, skill);
            field.tick_at += skill == TunRi ? 20 : 48;
        }
        if (field.expires_at == now) field = {};
        else e.Timers().ArmAt(field_timers_[slot], std::min(field.tick_at, field.expires_at));
        PublishField(e, skill);
    }

    std::shared_ptr<const Prepared> prepared_;
    Rng rng_;
    [[no_unique_address]] QidianPhase qidian_phase_;
    DeadlineQueue::TimerId natural_qidian_{};
    std::array<DeadlineQueue::TimerId, SkillCount> cooldown_{};
    std::array<DeadlineQueue::TimerId, SkillCount> casting_{};
    std::array<DeadlineQueue::TimerId, 3> prepare_{}, field_timers_{};
    DeadlineQueue::TimerId jinghua_prepare_{};
    DeadlineQueue::TimerId weapon_cw_expire_{}, wanxiang_tick_{}, wanxiang_expire_{};
    std::array<DeadlineQueue::TimerId, BuffCount> buff_expire_{};
    std::array<FieldInstance, 3> fields_{};
    std::uint64_t field_order_ = 0;
    DeadlineQueue::TimerId gcd_{}, dot_tick_{}, dot_expire_{}, purple_tick_{}, purple_expire_{}, feng_expire_{};
    DeadlineQueue::TimerId hidden_tick_{}, hidden_expire_{}, renjian_tick_{}, renjian_expire_{}, jianru_tick_{};
    std::array<DeadlineQueue::TimerId, 3> yunz_timers_{};
    tick_t recharge_at_ = 0;
    tick_t weapon_cw_ready_at_ = 0;
    std::uint16_t dot_snapshot_ = 0, renjian_snapshot_ = 0, wanxiang_snapshot_ = 0;
    tick_t jianru_tick_at_ = std::numeric_limits<tick_t>::max();
    std::uint16_t profile_ = 0;
    int you_ren_crit_basis_points_ = 0;
};
using Rules = BasicRules<>;

} // namespace JX3DPS::runtime::tai_xu
#endif
