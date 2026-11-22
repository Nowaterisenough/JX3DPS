#include "permanents.hpp"

#include <cmath>

#include <nlohmann/json.hpp>

#include "permanents_data.hpp"

namespace JX3DPS::runtime::season50 {
std::string_view PermanentFamily(const Book &b)
{
    return b.code == "tlgd" ? "天罗诡道" : Info(b.code).damage == "Physics" ? "外功" : "内功";
}

std::span<const PermanentOption> PermanentOptions(const Book &b)
{
    static const auto catalog = [] {
        std::map<std::string, std::vector<PermanentOption>, std::less<>> families;
        const auto data = nlohmann::ordered_json::parse(PermanentData);
        for (const auto &[family, categories] : data.items()) {
            for (const auto &[category, items] : categories.items()) {
                for (const auto &item : items) {
                    families[family].push_back(
                        { category, item.at("name"), item.at("icon"), item.at("stats").get<std::map<std::string, double>>() });
                }
            }
        }
        return families;
    }();
    return catalog.find(PermanentFamily(b))->second;
}

int ApplyPermanents(const Book &b, Attributes &a, int base_major, const std::map<std::string, std::string> &selected)
{
    if (base_major < 0 || base_major > 1000000) {
        throw std::invalid_argument("invalid base major attribute");
    }
    if (selected.empty()) {
        return base_major;
    }
    const auto                    options = PermanentOptions(b);
    std::map<std::string, double> stats;
    for (const auto &[category, name] : selected) {
        const auto it = std::ranges::find_if(options, [&](const auto &item) {
            return item.category == category && item.name == name;
        });
        if (it == options.end()) {
            throw std::invalid_argument("此心法不支持该常驻增益：" + category + "/" + name);
        }
        for (const auto &[key, value] : it->stats) {
            stats[key] += value;
        }
    }
    const auto        info = Info(b.code);
    const std::string major(info.major);
    const auto        flat = [&](const std::string &key) {
        return static_cast<int>(std::lround(stats[key]));
    };
    const int  added = flat("major") + flat(major);
    const int  total = base_major + added;
    const auto delta = [&](int ratio) {
        return total * ratio / 1024 - base_major * ratio / 1024;
    };
    const bool        physical = info.damage == "Physics";
    const std::string power = physical ? "力道" : "元气", critical = physical ? "身法" : "根骨";
    // Selected gear already contains the conversion from its primary attribute.
    const auto        converted = [&](const std::string &stat, int rate) {
        return stat == major ? total * rate / 100 - base_major * rate / 100 : flat(stat) * rate / 100;
    };
    a.attack_base        += flat("attackBase") + converted(power, 19);
    a.attack_by_class    += delta(info.attack_from_major);
    a.weapon             += flat("weapon");
    a.overcome           += flat("overcomeBase") + delta(info.overcome_from_major) + converted(power, 6);
    a.crit               += flat("crit") + delta(info.crit_from_major) + converted(critical, 25);
    a.strain             += flat("strain");
    a.haste              += flat("haste");
    a.strain_percent     += flat("strainBonus");
    a.attack_percent     += flat("attackBonus");
    a.overcome_percent   += flat("overcomeBonus");
    a.crit_power_percent += flat("criticalPower");
    a.crit_basis_points  += std::lround(stats["criticalChance"] * 10000.);
    a.damage_percent     += std::lround(stats["damageBonus"] * 1024.);
    a.ignore_shield       = std::min(1024, a.ignore_shield + flat("ignoreShield"));
    return total;
}
} // namespace JX3DPS::runtime::season50
