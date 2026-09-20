#ifndef JX3DPS_EQUIPMENT_EQUIPMENT_HPP
#define JX3DPS_EQUIPMENT_EQUIPMENT_HPP

#include "src/player/attribute.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cmath>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

namespace JX3DPS {

using json = nlohmann::json;

// Slots are deliberately numeric so the simulator can keep a fixed-size,
// cache-friendly loadout after the JSON boundary has been crossed.
enum class EquipmentSlot : std::uint8_t {
    HEAD,
    NECK,
    SHOULDER,
    BACK,
    CHEST,
    WAIST,
    WRIST,
    HANDS,
    LEGS,
    FEET,
    RING_1,
    RING_2,
    PENDANT,
    WEAPON,
    SPECIAL,
    COUNT
};

struct EquipmentStats {
    value_t agility = 0;
    value_t strength = 0;
    value_t spirit = 0;
    value_t spunk = 0;
    value_t weapon_damage = 0;
    value_t weapon_damage_rand = 0;
    value_t attack_power = 0;
    value_t physics_attack = 0;
    value_t magic_attack = 0;
    value_t critical = 0;
    value_t physics_critical = 0;
    value_t magic_critical = 0;
    value_t critical_power = 0;
    value_t physics_critical_power = 0;
    value_t magic_critical_power = 0;
    value_t overcome = 0;
    value_t physics_overcome = 0;
    value_t magic_overcome = 0;
    value_t strain = 0;
    value_t surplus = 0;
    value_t haste = 0;
    pctn_t damage_percent = 0;
    pctn_t pve_damage_percent = 0;

    void Add(const EquipmentStats &other)
    {
        agility += other.agility;
        strength += other.strength;
        spirit += other.spirit;
        spunk += other.spunk;
        weapon_damage += other.weapon_damage;
        weapon_damage_rand += other.weapon_damage_rand;
        attack_power += other.attack_power;
        physics_attack += other.physics_attack;
        magic_attack += other.magic_attack;
        critical += other.critical;
        physics_critical += other.physics_critical;
        magic_critical += other.magic_critical;
        critical_power += other.critical_power;
        physics_critical_power += other.physics_critical_power;
        magic_critical_power += other.magic_critical_power;
        overcome += other.overcome;
        physics_overcome += other.physics_overcome;
        magic_overcome += other.magic_overcome;
        strain += other.strain;
        surplus += other.surplus;
        haste += other.haste;
        damage_percent += other.damage_percent;
        pve_damage_percent += other.pve_damage_percent;
    }

    void ApplyTo(Attribute &attribute) const
    {
        using T = Attribute::Type;
        attribute.Add(T::AGILITY_BASE, agility);
        attribute.Add(T::STRENGTH_BASE, strength);
        attribute.Add(T::SPIRIT_BASE, spirit);
        attribute.Add(T::SPUNK_BASE, spunk);
        attribute.Add(T::WEAPON_DAMAGE_BASE, weapon_damage);
        attribute.Add(T::WEAPON_DAMAGE_RAND, weapon_damage_rand);
        attribute.Add(T::ATTACK_POWER_BASE, attack_power);
        attribute.Add(T::PHYSICS_ATTACK_POWER_BASE, physics_attack);
        attribute.Add(T::MAGIC_ATTACK_POWER_BASE, magic_attack);
        attribute.Add(T::CRITICAL_STRIKE, critical);
        attribute.Add(T::PHYSICS_CRITICAL_STRIKE, physics_critical);
        attribute.Add(T::MAGIC_CRITICAL_STRIKE, magic_critical);
        attribute.Add(T::CRITICAL_STRIKE_POWER, critical_power);
        attribute.Add(T::PHYSICS_CRITICAL_STRIKE_POWER, physics_critical_power);
        attribute.Add(T::MAGIC_CRITICAL_STRIKE_POWER, magic_critical_power);
        attribute.Add(T::OVERCOME_BASE, overcome);
        attribute.Add(T::PHYSICS_OVERCOME_BASE, physics_overcome);
        attribute.Add(T::MAGIC_OVERCOME_BASE, magic_overcome);
        attribute.Add(T::STRAIN_BASE, strain);
        attribute.Add(T::SURPLUS_VALUE_BASE, surplus);
        attribute.Add(T::HASTE_BASE, haste);
        attribute.Add(T::DAMAGE_ADDITIONAL_PERCENT_INT, damage_percent);
        attribute.Add(T::PVE_DAMAGE_ADDITIONAL_PERCENT_INT, pve_damage_percent);
    }
};

struct EquipmentEffect {
    jx3id_t id = 0;
    int icon = 0;
    std::string name;
    std::string description;
    EquipmentStats stats;
    bool triggered = false;
};

struct EquipmentItem {
    jx3id_t id = 0;
    EquipmentSlot slot = EquipmentSlot::HEAD;
    std::string name;
    int icon = 0;
    int level = 0;
    int quality = 0;
    int refine_level = 0;
    EquipmentStats stats;
    std::vector<EquipmentEffect> effects;
    std::vector<EquipmentEffect> gems;
    std::vector<EquipmentEffect> enchantments;
    std::string set_name;
};

struct EquipmentSetBonus {
    std::string name;
    int pieces = 0;
    EquipmentStats stats;
    std::vector<EquipmentEffect> effects;
};

struct EquipmentRuntimeCache {
    EquipmentStats stats;
    std::uint64_t triggered_effect_mask = 0;
    std::array<int, static_cast<size_t>(EquipmentSlot::COUNT)> icons{};
    std::array<jx3id_t, static_cast<size_t>(EquipmentSlot::COUNT)> ids{};
};

namespace equipment_detail {

inline std::string CompactName(std::string value)
{
    value.erase(std::remove_if(value.begin(), value.end(), [](unsigned char c) {
        return std::isspace(c) || c == '_' || c == '-';
    }), value.end());
    return value;
}

inline value_t NumberFromText(const std::string &text)
{
    static const std::regex number(R"([-+]?\d+(?:\.\d+)?)");
    std::smatch match;
    if (!std::regex_search(text, match, number)) return 0;
    try {
        return static_cast<value_t>(std::llround(std::stod(match.str())));
    } catch (...) {
        return 0;
    }
}

inline bool AddNamedStat(EquipmentStats &stats, std::string name, value_t value)
{
    name = CompactName(std::move(name));
    if (name == "身法" || name == "atAgilityBase") stats.agility += value;
    else if (name == "力道" || name == "atStrengthBase") stats.strength += value;
    else if (name == "根骨" || name == "atSpiritBase") stats.spirit += value;
    else if (name == "元气" || name == "atSpunkBase") stats.spunk += value;
    else if (name == "基础武器伤害" || name == "atWeaponDamageBase") stats.weapon_damage += value;
    else if (name == "浮动武器伤害" || name == "atWeaponDamageRand") stats.weapon_damage_rand += value;
    else if (name == "攻击力" || name == "基础攻击" || name == "攻击力加成" || name == "atAttackPowerBase") stats.attack_power += value;
    else if (name == "外功基础攻击" || name == "外功攻击" || name == "atPhysicsAttackPowerBase") stats.physics_attack += value;
    else if (name == "内功基础攻击" || name == "内功攻击" || name == "atMagicAttackPowerBase") stats.magic_attack += value;
    else if (name == "会心等级" || name == "atCriticalStrike") stats.critical += value;
    else if (name == "外功会心等级" || name == "atPhysicsCriticalStrike") stats.physics_critical += value;
    else if (name == "内功会心等级" || name == "atMagicCriticalStrike") stats.magic_critical += value;
    else if (name == "会效等级" || name == "会心效果等级" || name == "atCriticalStrikePower") stats.critical_power += value;
    else if (name == "外功会效等级" || name == "外功会心效果等级" || name == "atPhysicsCriticalStrikePower") stats.physics_critical_power += value;
    else if (name == "内功会效等级" || name == "内功会心效果等级" || name == "atMagicCriticalStrikePower") stats.magic_critical_power += value;
    else if (name == "破防等级" || name == "基础破防等级" || name == "atOvercomeBase") stats.overcome += value;
    else if (name == "外功破防等级" || name == "atPhysicsOvercomeBase") stats.physics_overcome += value;
    else if (name == "内功破防等级" || name == "atMagicOvercomeBase") stats.magic_overcome += value;
    else if (name == "无双" || name == "无双等级" || name == "atStrainBase") stats.strain += value;
    else if (name == "破招值" || name == "破招" || name == "atSurplusValueBase") stats.surplus += value;
    else if (name == "加速等级" || name == "atHasteBase") stats.haste += value;
    else if (name == "会心加成" || name == "外功会心加成" || name == "内功会心加成") stats.critical += value;
    else if (name == "会效加成" || name == "外功会效加成" || name == "内功会效加成") stats.critical_power += value;
    else if (name == "伤害加成") stats.damage_percent += static_cast<pctn_t>(value);
    else if (name == "PVE伤害加成" || name == "PVE伤害") stats.pve_damage_percent += static_cast<pctn_t>(value);
    else {
        // 魔盒的套装/附魔文本有时只提供中文描述，例如“身法提高607”。
        // 在初始化阶段识别这些文本，运行期仍只使用已经合并的数值。
        if (name.find("身法") != std::string::npos) stats.agility += value;
        else if (name.find("外功攻击") != std::string::npos) stats.physics_attack += value;
        else if (name.find("内功攻击") != std::string::npos) stats.magic_attack += value;
        else if (name.find("外功破防") != std::string::npos) stats.physics_overcome += value;
        else if (name.find("破防") != std::string::npos) stats.overcome += value;
        else if (name.find("外功会心") != std::string::npos) stats.physics_critical += value;
        else if (name.find("会心") != std::string::npos) stats.critical += value;
        else if (name.find("会效") != std::string::npos) stats.critical_power += value;
        else if (name.find("无双") != std::string::npos) stats.strain += value;
        else if (name.find("破招") != std::string::npos) stats.surplus += value;
        else if (name.find("加速") != std::string::npos) stats.haste += value;
        else return false;
    }
    return true;
}

inline EquipmentStats ParseStats(const json &value)
{
    EquipmentStats result;
    if (value.is_string()) {
        const auto text = value.get<std::string>();
        AddNamedStat(result, text, NumberFromText(text));
    } else if (value.is_object()) {
        for (auto it = value.begin(); it != value.end(); ++it) {
            if (it.value().is_number()) {
                AddNamedStat(result, it.key(), it.value().get<value_t>());
            } else if (it.value().is_string()) {
                const auto text = it.value().get<std::string>();
                AddNamedStat(result, text, NumberFromText(text));
            }
        }
    } else if (value.is_array()) {
        for (const auto &entry : value) {
            if (!entry.is_object()) continue;
            std::string name = entry.value("Type", entry.value("type", ""));
            value_t number = entry.value("Value", entry.value("value", 0));
            if (number == 0 && entry.contains("label")) number = NumberFromText(entry["label"].get<std::string>());
            if (!AddNamedStat(result, name, number) && entry.contains("label")) {
                AddNamedStat(result, entry["label"].get<std::string>(), number);
            }
        }
    }
    return result;
}

inline EquipmentSlot ParseSlot(std::string value)
{
    value = CompactName(std::move(value));
    if (value == "head" || value == "帽子" || value == "头部") return EquipmentSlot::HEAD;
    if (value == "neck" || value == "necklace" || value == "项链") return EquipmentSlot::NECK;
    if (value == "shoulder" || value == "肩膀" || value == "护肩") return EquipmentSlot::SHOULDER;
    if (value == "back" || value == "披风") return EquipmentSlot::BACK;
    if (value == "chest" || value == "jacket" || value == "上衣") return EquipmentSlot::CHEST;
    if (value == "waist" || value == "belt" || value == "腰带") return EquipmentSlot::WAIST;
    if (value == "wrist" || value == "护腕" || value == "护手") return EquipmentSlot::WRIST;
    if (value == "hands" || value == "手套") return EquipmentSlot::HANDS;
    if (value == "legs" || value == "bottoms" || value == "裤子" || value == "下装") return EquipmentSlot::LEGS;
    if (value == "feet" || value == "shoes" || value == "鞋子") return EquipmentSlot::FEET;
    if (value == "ring" || value == "戒指" || value == "ring1" || value == "戒指1") return EquipmentSlot::RING_1;
    if (value == "ring2" || value == "戒指2") return EquipmentSlot::RING_2;
    if (value == "pendant" || value == "腰坠") return EquipmentSlot::PENDANT;
    if (value == "weapon" || value == "武器" || value == "短兵" || value == "短兵类" || value == "投掷") return EquipmentSlot::WEAPON;
    if (value == "special" || value == "饰品" || value == "特殊" || value == "trinket") return EquipmentSlot::SPECIAL;
    return EquipmentSlot::HEAD;
}

inline EquipmentEffect ParseEffect(const json &value)
{
    EquipmentEffect result;
    if (value.is_string()) {
        result.name = value.get<std::string>();
        return result;
    }
    if (!value.is_object()) return result;
    result.id = value.value("id", value.value("ID", 0));
    result.icon = value.value("icon", value.value("Icon", value.value("IconID", 0)));
    result.name = value.value("name", value.value("Name", ""));
    result.description = value.value("description", value.value("Description", value.value("Desc", "")));
    result.triggered = value.value("triggered", value.value("Triggered", false));
    if (value.contains("stats")) result.stats.Add(ParseStats(value["stats"]));
    if (value.contains("attributes")) result.stats.Add(ParseStats(value["attributes"]));
    if (value.contains("AttributeAdditional")) result.stats.Add(ParseStats(value["AttributeAdditional"]));
    return result;
}

inline EquipmentItem ParseItem(const json &value, EquipmentSlot fallback)
{
    EquipmentItem result;
    result.slot = fallback;
    if (!value.is_object()) return result;
    // JX3BOX uses both numeric source_id and composite strings such as
    // "8_42406". Prefer the numeric source ID so the JSON boundary never
    // throws while the simulator is being initialized.
    if (value.contains("source_id") && value["source_id"].is_number())
        result.id = value["source_id"].get<jx3id_t>();
    else if (value.contains("SourceID") && value["SourceID"].is_number())
        result.id = value["SourceID"].get<jx3id_t>();
    else if (value.contains("id") && value["id"].is_number())
        result.id = value["id"].get<jx3id_t>();
    else if (value.contains("ID") && value["ID"].is_number())
        result.id = value["ID"].get<jx3id_t>();
    else if (value.contains("id") && value["id"].is_string()) {
        const auto idText = value["id"].get<std::string>();
        const auto separator = idText.find_last_of('_');
        try { result.id = static_cast<jx3id_t>(std::stoul(idText.substr(separator == std::string::npos ? 0 : separator + 1))); }
        catch (...) { result.id = 0; }
    }
    result.name = value.value("name", value.value("Name", ""));
    result.icon = value.value("icon", value.value("Icon", value.value("IconID",
                            value.value("icon_id", 0))));
    result.level = value.value("level", value.value("Level", 0));
    result.quality = value.value("quality", value.value("Quality", 0));
    result.refine_level = value.value("refine_level", value.value("refineLevel", value.value("MaxStrengthLevel", 0)));
    if (value.contains("slot")) result.slot = ParseSlot(value["slot"].get<std::string>());
    if (value.contains("type")) result.slot = ParseSlot(value["type"].get<std::string>());
    result.set_name = value.value("set_name", value.value("setName", ""));
    if (value.contains("Set")) {
        if (value["Set"].is_object()) result.set_name = value["Set"].value("name", result.set_name);
        else if (value["Set"].is_string()) result.set_name = value["Set"].get<std::string>();
    }
    if (value.contains("set")) {
        if (value["set"].is_object()) result.set_name = value["set"].value("name", result.set_name);
        else if (value["set"].is_string()) result.set_name = value["set"].get<std::string>();
    }
    if (value.contains("stats")) result.stats.Add(ParseStats(value["stats"]));
    if (value.contains("attributes")) result.stats.Add(ParseStats(value["attributes"]));
    if (value.contains("AttributeAdditional")) result.stats.Add(ParseStats(value["AttributeAdditional"]));
    if (value.contains("effects") && value["effects"].is_array()) {
        for (const auto &effect : value["effects"]) result.effects.push_back(ParseEffect(effect));
    }
    const auto parseItemEffects = [&result, &value](const char *key, std::vector<EquipmentEffect> &target) {
        if (!value.contains(key) || !value[key].is_array()) return;
        for (const auto &entry : value[key]) target.push_back(ParseEffect(entry));
    };
    parseItemEffects("diamonds", result.gems);
    parseItemEffects("gems", result.gems);
    parseItemEffects("enchant", result.enchantments);
    parseItemEffects("enchantments", result.enchantments);
    return result;
}

} // namespace equipment_detail

struct EquipmentLoadout {
    std::array<EquipmentItem, static_cast<size_t>(EquipmentSlot::COUNT)> items{};
    std::array<bool, static_cast<size_t>(EquipmentSlot::COUNT)> equipped{};
    std::vector<EquipmentEffect> effects;
    std::vector<EquipmentSetBonus> set_bonuses;
    EquipmentRuntimeCache runtime;

    static EquipmentLoadout FromJson(const json &config)
    {
        EquipmentLoadout result;
        const json *root = &config;
        if (config.contains("equipment")) root = &config["equipment"];
        else if (config.contains("Equipment")) root = &config["Equipment"];

        // 魔盒物品把套装效果内嵌在每一件装备的 set.attributes 中。
        // 在配置边界去重并转成固定的件数奖励，热循环不再解析这些文本。
        const auto collectEmbeddedSet = [&result](const json &item) {
            const json *set = item.contains("set") ? &item["set"] :
                              (item.contains("Set") ? &item["Set"] : nullptr);
            if (!set || !set->is_object() || !set->contains("attributes")) return;
            const std::string setName = set->value("name", "");
            const auto &attributes = (*set)["attributes"];
            if (setName.empty() || !attributes.is_object()) return;
            for (auto it = attributes.begin(); it != attributes.end(); ++it) {
                int pieces = 0;
                try { pieces = std::stoi(it.key()); } catch (...) { continue; }
                if (pieces <= 0) continue;
                auto found = std::find_if(result.set_bonuses.begin(), result.set_bonuses.end(),
                                          [&](const EquipmentSetBonus &bonus) {
                                              return bonus.name == setName && bonus.pieces == pieces;
                                          });
                if (found == result.set_bonuses.end()) {
                    EquipmentSetBonus bonus;
                    bonus.name = setName;
                    bonus.pieces = pieces;
                    bonus.stats.Add(equipment_detail::ParseStats(it.value()));
                    result.set_bonuses.push_back(std::move(bonus));
                }
            }
        };
        const auto storeItem = [&result](EquipmentItem parsed) {
            auto index = static_cast<size_t>(parsed.slot);
            // A live item search labels both rings as "戒指". Assign the
            // second occurrence to the second fixed slot at initialization.
            if (parsed.slot == EquipmentSlot::RING_1 && result.equipped[index])
                index = static_cast<size_t>(EquipmentSlot::RING_2);
            if (index < result.items.size()) {
                result.items[index] = std::move(parsed);
                result.equipped[index] = true;
            }
        };

        if (root->is_object()) {
            if (root->contains("items")) {
                const auto &items = (*root)["items"];
                if (items.is_array()) {
                    for (const auto &item : items) {
                        auto parsed = equipment_detail::ParseItem(item, EquipmentSlot::HEAD);
                        collectEmbeddedSet(item);
                        storeItem(std::move(parsed));
                    }
                } else if (items.is_object()) {
                    for (auto it = items.begin(); it != items.end(); ++it) {
                        auto parsed = equipment_detail::ParseItem(it.value(), equipment_detail::ParseSlot(it.key()));
                        collectEmbeddedSet(it.value());
                        storeItem(std::move(parsed));
                    }
                }
            }
            // Also accept the compact {"head": {...}, "weapon": {...}} form.
            for (auto it = root->begin(); it != root->end(); ++it) {
                const auto slot = equipment_detail::ParseSlot(it.key());
                const auto index = static_cast<size_t>(slot);
                if (index < result.items.size() && it.value().is_object() &&
                    (it.key() != "items" && it.key() != "sets" && it.key() != "set_bonuses")) {
                    auto parsed = equipment_detail::ParseItem(it.value(), slot);
                    collectEmbeddedSet(it.value());
                    storeItem(std::move(parsed));
                }
            }
            const auto parseEffects = [&result](const json &value) {
                if (value.is_array()) for (const auto &entry : value) result.effects.push_back(equipment_detail::ParseEffect(entry));
            };
            if (root->contains("effects")) parseEffects((*root)["effects"]);
            if (root->contains("equip_effects")) parseEffects((*root)["equip_effects"]);
            if (root->contains("EquipEffects")) parseEffects((*root)["EquipEffects"]);
            if (root->contains("consumables")) parseEffects((*root)["consumables"]);
            if (root->contains("Consumables")) parseEffects((*root)["Consumables"]);
            if (root->contains("permanents")) parseEffects((*root)["permanents"]);
            if (root->contains("Permanents")) {
                const auto &permanents = (*root)["Permanents"];
                if (permanents.is_array()) parseEffects(permanents);
                else if (permanents.is_object()) for (auto it = permanents.begin(); it != permanents.end(); ++it) parseEffects(it.value());
            }
            const json *sets = root->contains("set_bonuses") ? &(*root)["set_bonuses"] :
                               (root->contains("equipment_sets") ? &(*root)["equipment_sets"] : nullptr);
            if (sets && sets->is_array()) {
                for (const auto &entry : *sets) {
                    EquipmentSetBonus bonus;
                    bonus.name = entry.value("name", entry.value("Name", ""));
                    bonus.pieces = entry.value("pieces", entry.value("Pieces", 0));
                    if (entry.contains("stats")) bonus.stats.Add(equipment_detail::ParseStats(entry["stats"]));
                    if (entry.contains("attributes")) bonus.stats.Add(equipment_detail::ParseStats(entry["attributes"]));
                    if (entry.contains("effects") && entry["effects"].is_array())
                        for (const auto &effect : entry["effects"]) bonus.effects.push_back(equipment_detail::ParseEffect(effect));
                    result.set_bonuses.push_back(std::move(bonus));
                }
            }
        }

        result.BuildRuntime();
        return result;
    }

    void BuildRuntime()
    {
        runtime = {};
        std::vector<int> set_counts(set_bonuses.size(), 0);
        for (size_t i = 0; i < items.size(); ++i) {
            if (!equipped[i]) continue;
            runtime.stats.Add(items[i].stats);
            runtime.icons[i] = items[i].icon;
            runtime.ids[i] = items[i].id;
            for (const auto &effect : items[i].effects) runtime.stats.Add(effect.stats);
            for (const auto &effect : items[i].gems) runtime.stats.Add(effect.stats);
            for (const auto &effect : items[i].enchantments) runtime.stats.Add(effect.stats);
            if (!items[i].set_name.empty()) {
                for (size_t set_index = 0; set_index < set_bonuses.size(); ++set_index) {
                    if (set_bonuses[set_index].name == items[i].set_name) set_counts[set_index]++;
                }
            }
        }
        for (size_t i = 0; i < set_bonuses.size(); ++i) {
            if (set_bonuses[i].pieces <= 0 || set_counts[i] < set_bonuses[i].pieces) continue;
            runtime.stats.Add(set_bonuses[i].stats);
            for (const auto &effect : set_bonuses[i].effects) runtime.stats.Add(effect.stats);
            if (i < 64) runtime.triggered_effect_mask |= (std::uint64_t{1} << i);
        }
        for (const auto &effect : effects) {
            runtime.stats.Add(effect.stats);
            if (effect.id > 0 && effect.id < 64) runtime.triggered_effect_mask |= (std::uint64_t{1} << effect.id);
        }
    }

    void ApplyTo(Attribute &attribute) const { runtime.stats.ApplyTo(attribute); }
};

inline std::string EquipmentIconUrl(int icon)
{
    return std::string("https://icon.jx3box.com/icon/") + std::to_string(icon) + ".png";
}

} // namespace JX3DPS

#endif // JX3DPS_EQUIPMENT_EQUIPMENT_HPP
