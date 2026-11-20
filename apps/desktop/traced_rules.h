#ifndef DESKTOP_TRACED_RULES_H
#define DESKTOP_TRACED_RULES_H

#include "combat_history.h"
#include "runtime_backend.h"

namespace desktop {
template <class Input>
ActorAttributes AttributesFromInput(const Input &input, JX3DPS::value_t weapon, double critical_chance, double haste, int extra_power = 0)
{
    ActorAttributes a;
    a.attack         = input.attack;
    a.weapon         = weapon;
    a.overcome       = input.overcome;
    a.strain         = input.strain;
    a.surplus        = input.surplus;
    a.criticalChance = critical_chance;
    a.criticalMultiplier =
        1.75 + (static_cast<int>(input.crit_power * 1024 / (3.335 * 8250)) + input.crit_power_percent + extra_power) / 1024.0;
    a.haste         = haste;
    a.damagePercent = input.damage_percent;
    a.strainPercent = input.strain_percent;
    a.pvePercent    = input.pve_percent;
    a.ignoreShield  = input.ignore_shield;
    return a;
}

inline ActorAttributes CurrentAttributes(const rt::tai_xu::Rules &rules)
{
    const auto &data    = rules.Data();
    const auto  PROFILE = rules.CurrentProfile();
    return AttributesFromInput(data.Input(rt::tai_xu::ZeroEffect, PROFILE, rules.CurrentTeamModifiers()),
                               data.config.attributes.weapon,
                               data.Chance(rt::tai_xu::ZeroEffect, PROFILE, rules.CurrentTeamModifiers().crit_basis_points) +
                                   (rules.CurrentCriticalBonus() / 10000.0),
                               data.haste);
}

inline ActorAttributes CurrentAttributes(const rt::mo_wen::Rules &rules)
{
    const auto &data = rules.Data();
    const auto  AURA = rules.CurrentAura();
    auto result = AttributesFromInput(data.Input(rt::mo_wen::E_ZERO_SHU_YING, AURA.profile),
                                      data.config.attributes.weapon,
                                      data.Chance(rt::mo_wen::E_ZERO_SHU_YING, rt::mo_wen::ChanceProfile(AURA.profile)),
                                      rules.CurrentHaste(),
                                      AURA.extra_power);
    result.style   = static_cast<int>(rules.CurrentStyle());
    result.shadows = rules.ShadowCount();
    return result;
}

// Only single-fight trace/debug instantiate this type. Batch uses the base Rules
// directly, so it has no recording storage, callbacks, or runtime mode branch.
template <class Rules>
class TracedRules : public Rules
{
public:
    explicit TracedRules(Rules rules) : Rules(std::move(rules)) { }

    // Capture also observes rejected actions after a pre-condition channel interrupt.
    static bool CanAttempt(const rt::State &, const rt::MacroLine &) { return true; }

    static bool MayActWhileBusy(const rt::MacroLine &) { return true; }

    template <class Engine>
    void Reset(Engine &engine, std::uint64_t seed)
    {
        m_attributes.clear();
        Rules::Reset(engine, seed);
        Capture(engine.GetState());
    }

    template <class Engine>
    bool Cast(Engine &engine, const rt::MacroLine &line)
    {
        const bool result = Rules::Cast(engine, line);
        Capture(engine.GetState());
        return result;
    }

    template <class Engine>
    void OnTimer(Engine &engine, rt::TimerHandle handle, rt::DeadlineQueue::TimerId id)
    {
        Rules::OnTimer(engine, handle, id);
        Capture(engine.GetState());
    }

    const std::vector<AttributeSample> &AttributeHistory() const { return m_attributes; }

private:
    std::vector<AttributeSample> m_attributes;

    void Capture(const rt::State &state)
    {
        auto values      = CurrentAttributes(static_cast<const Rules &>(*this));
        values.lastSkill = state.last_skill;
        if (m_attributes.empty() || m_attributes.back().values != values) {
            m_attributes.push_back({ state.now, values });
        }
    }
};
} // namespace desktop
#endif
