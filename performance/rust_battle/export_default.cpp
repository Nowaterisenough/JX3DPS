// Export immutable formula inputs and timer descriptors, never fight outcomes.
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <vector>

#include "src/class/tai_xu_jian_yi/runtime_rules.hpp"
using namespace JX3DPS;
using namespace JX3DPS::runtime;

struct Metadata
{
    State           state;
    DamageIntentLog log;

    struct Desc
    {
        TimerHandle handle;
        uint16_t    priority;
    };

    std::vector<Desc> timers;

    State &MutableState() { return state; }

    Metadata &Timers() { return *this; }

    DamageIntentLog &Log() { return log; }

    uint32_t Create(TimerHandle h, uint16_t p = 0)
    {
        timers.push_back({ h, p });
        return timers.size() - 1;
    }
};

int main(int argc, char *const *argv)
{
    if (argc != 3) {
        return 2;
    }
    std::ofstream out(argv[1]);
    out.exceptions(std::ios::failbit | std::ios::badbit);
    out << std::setprecision(17) << std::showpoint;
    std::ofstream cpp(argv[2]);
    cpp.exceptions(std::ios::failbit | std::ios::badbit);
    cpp << std::setprecision(17) << std::showpoint;
    const auto    DATA = tai_xu::Prepare();
    tai_xu::Rules rules(DATA);
    Metadata      meta;
    meta.state.Resize(tai_xu::SKILL_COUNT, tai_xu::BUFF_COUNT);
    rules.Setup(meta);
    out << "const TIMERS: &[(u8,u16,bool,u16)] = &[\n";
    for (const auto t : meta.timers) {
        out << '(' << int(t.handle.kind) << ',' << t.handle.slot << ',' << (t.handle.target ? "true" : "false") << ','
            << t.priority << "),\n";
    }
    out << "];\nconst FORMULAS: &[Formula] = &[\n";
    cpp << "constexpr FixedFormula formulas[] = {\n";
    for (size_t i = 0; i < DATA->formulas.size(); ++i) {
        const auto F = DATA->formulas[i];
        cpp << "{" << F.skill << ',' << F.sub << ',' << F.level << ',' << int(F.kind) << ',' << F.fixed << ','
            << F.weapon << ',' << F.count << ',' << DATA->coefficients[i] << ',' << DATA->surplus_damage[i] << ','
            << DATA->damage_bonus[i] << ',' << DATA->power_bonus[i] << "},\n";
        out << "Formula{skill:" << F.skill << ",sub:" << F.sub << ",level:" << F.level << ",kind:" << int(F.kind)
            << ",fixed:" << F.fixed << ",weapon:" << F.weapon << ",count:" << F.count
            << ",coefficient:" << DATA->coefficients[i] << ",surplus:" << DATA->surplus_damage[i]
            << ",bonus:" << DATA->damage_bonus[i] << ",power:" << DATA->power_bonus[i] << "},\n";
    }
    out << "];\nconst ATTRS: &[Attributes] = &[\n";
    cpp << "};\nconstexpr FixedAttributes attrs[] = {\n";
    for (const unsigned PROFILE : { 0u, 1u }) {
        const auto A = DATA->Resolve(PROFILE);
        cpp << "{" << A.input.attack << ',' << A.input.weapon << ',' << A.input.damage_percent << ',' << A.mitigation
            << ',' << A.critical_power + A.input.crit_power_percent << ',' << A.strain_factor << ','
            << A.input.pve_percent << ',' << A.vulnerable << ',' << A.critical_chance << "},\n";
        out << "Attributes{attack:" << A.input.attack << ",weapon:" << A.input.weapon << ",bonus:" << A.input.damage_percent
            << ",mitigation:" << A.mitigation << ",power:" << A.critical_power + A.input.crit_power_percent
            << ",strain:" << A.strain_factor << ",pve:" << A.input.pve_percent << ",vulnerable:" << A.vulnerable
            << ",chance:" << A.critical_chance << "},\n";
    }
    out << "];\nconst LEVEL: f64 = " << DATA->level_multiplier << ";\n";
    cpp << "};\nconstexpr double level_multiplier = " << DATA->level_multiplier << ";\n";
}
