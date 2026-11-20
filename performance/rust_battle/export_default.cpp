// Export immutable formula inputs and timer descriptors, never fight outcomes.
#include <fstream>
#include <iomanip>

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

int main(int argc, char **argv)
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
    auto          data = tai_xu::Prepare();
    tai_xu::Rules rules(data);
    Metadata      meta;
    meta.state.Resize(tai_xu::SkillCount, tai_xu::BuffCount);
    rules.Setup(meta);
    out << "const TIMERS: &[(u8,u16,bool,u16)] = &[\n";
    for (auto t : meta.timers) {
        out << '(' << int(t.handle.kind) << ',' << t.handle.slot << ',' << (t.handle.target ? "true" : "false") << ','
            << t.priority << "),\n";
    }
    out << "];\nconst FORMULAS: &[Formula] = &[\n";
    cpp << "constexpr FixedFormula formulas[] = {\n";
    for (size_t i = 0; i < data->formulas.size(); ++i) {
        auto f = data->formulas[i];
        cpp << "{" << f.skill << ',' << f.sub << ',' << f.level << ',' << int(f.kind) << ',' << f.fixed << ','
            << f.weapon << ',' << f.count << ',' << data->coefficients[i] << ',' << data->surplus_damage[i] << ','
            << data->damage_bonus[i] << ',' << data->power_bonus[i] << "},\n";
        out << "Formula{skill:" << f.skill << ",sub:" << f.sub << ",level:" << f.level << ",kind:" << int(f.kind)
            << ",fixed:" << f.fixed << ",weapon:" << f.weapon << ",count:" << f.count
            << ",coefficient:" << data->coefficients[i] << ",surplus:" << data->surplus_damage[i]
            << ",bonus:" << data->damage_bonus[i] << ",power:" << data->power_bonus[i] << "},\n";
    }
    out << "];\nconst ATTRS: &[Attributes] = &[\n";
    cpp << "};\nconstexpr FixedAttributes attrs[] = {\n";
    for (unsigned profile : { 0u, 1u }) {
        auto a = data->Resolve(profile);
        cpp << "{" << a.input.attack << ',' << a.input.weapon << ',' << a.input.damage_percent << ',' << a.mitigation
            << ',' << a.critical_power + a.input.crit_power_percent << ',' << a.strain_factor << ','
            << a.input.pve_percent << ',' << a.vulnerable << ',' << a.critical_chance << "},\n";
        out << "Attributes{attack:" << a.input.attack << ",weapon:" << a.input.weapon << ",bonus:" << a.input.damage_percent
            << ",mitigation:" << a.mitigation << ",power:" << a.critical_power + a.input.crit_power_percent
            << ",strain:" << a.strain_factor << ",pve:" << a.input.pve_percent << ",vulnerable:" << a.vulnerable
            << ",chance:" << a.critical_chance << "},\n";
    }
    out << "];\nconst LEVEL: f64 = " << data->level_multiplier << ";\n";
    cpp << "};\nconstexpr double level_multiplier = " << data->level_multiplier << ";\n";
}
