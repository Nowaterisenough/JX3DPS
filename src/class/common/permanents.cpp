#include "permanents.hpp"

#include <cmath>

#include <nlohmann/json.hpp>

#include "permanents_data.hpp"

namespace JX3DPS::runtime::season50 {
std::string_view PermanentFamily(const Book &b)
{
    return !b.raid.family.empty() ? b.raid.family : b.info.damage == "Physics" ? "外功" : "内功";
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
    return catalog.at(std::string(PermanentFamily(b)));
}

std::map<std::string, std::string> FullRaidPermanents(const Book &b)
{
    const auto info = b.info;
    const bool physical = info.damage == "Physics";
    const std::string formation(b.raid.formation.empty() ? (physical ? "凌雪阵（常驻）" : "天罗阵（常驻）") : b.raid.formation);
    const std::map<std::string_view, std::string> wines{{"力道", "汾酒"}, {"身法", "关外白酒"}, {"元气", "状元红"}, {"根骨", "高粱酒"}};
    return {{"阵法", formation}, {"食品增强", "云州刀削面·精（攻击)"}, {"食品辅助", "云芝竹沥羹·精（主属）"},
            {"药品增强", "中原破阵丹·精（攻击)"}, {"药品辅助", "五气归元丹·精（主属）"}, {"武器磨石", std::string(b.raid.whetstone)},
            {"团队宴席", physical ? "华岳开锋宴" : "未央宫庭宴"}, {"家园菜品", std::string(b.raid.dish)},
            {"家园酒品", wines.at(info.major) + "·旬又三（" + std::string(info.major) + "）"},
            {"家园香水", b.raid.penetration_perfume ? "煦风·宁为秋霜（破防）" : "煦风·燕燕于飞（会心）"}};
}

int ApplyPermanents(const Book &b, Attributes &a, int base_major, const std::map<std::string, std::string> &selected, bool raid_full)
{
    if (base_major < 0 || base_major > 1000000) {
        throw std::invalid_argument("invalid base major attribute");
    }
    if (selected.empty() && !raid_full) {
        return base_major;
    }
    const auto                    options = PermanentOptions(b);
    std::map<std::string, double> stats;
    auto choices = selected;
    if (raid_full) {
        choices.merge(FullRaidPermanents(b));
        // 袖气 adds all four stats; each class and common conversion is applied once below.
        for (auto stat : {"力道", "身法", "元气", "根骨"}) stats[stat] += 45;
        stats["strain"] += 70 + 21 + 30 + 100 * (7 + 7 + 7 + 5 + 5);
        if (b.info.damage == "Physics" && !b.raid.provides_attack_buff) stats["attackBonus"] += 51;
        const auto &formation = choices.at("阵法");
        if (formation == "莫问阵（常驻）" || formation == "天罗阵（常驻）") stats["criticalChance"] += .05;
        if (formation == "天罗阵（常驻）" && b.raid.own_formation == formation) stats["strainBonus"] += 20;
        if (formation == "刀宗阵" && b.raid.own_formation == formation) stats["criticalPower"] += 100;
    }
    for (const auto &[category, name] : choices) {
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
    const auto        info = b.info;
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
