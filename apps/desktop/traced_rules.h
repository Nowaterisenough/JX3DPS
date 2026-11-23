#ifndef DESKTOP_TRACED_RULES_H
#define DESKTOP_TRACED_RULES_H

#include "combat_history.h"
#include "runtime_backend.h"

namespace desktop {
inline ActorAttributes CurrentAttributes(const rt::season50::Rules &rules)
{
    const auto     &p = rules.Data();
    const auto     &a = p.config.attributes;
    ActorAttributes v;
    v.attack         = a.attack_base * (1024 + a.attack_percent) / 1024 + a.attack_by_class;
    v.weapon         = a.weapon;
    v.overcome       = a.overcome * (1024 + a.overcome_percent) / 1024;
    v.strain         = a.strain;
    v.surplus        = a.surplus;
    v.criticalChance = std::clamp(p.crit_basis_points, 0, 10000) / 10000.;
    v.haste          = p.haste;
    v.criticalMultiplier = 1.75 + std::min(1280, rt::season50::Rating(a.crit_power, 3.54) + a.crit_power_percent) / 1024.;
    v.damagePercent = a.damage_percent;
    v.strainPercent = a.strain_percent;
    v.pvePercent    = a.pve_percent;
    v.ignoreShield  = a.ignore_shield;
    return v;
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
