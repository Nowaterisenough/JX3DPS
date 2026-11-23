#ifndef DESKTOP_RUNTIME_CATALOG_H
#define DESKTOP_RUNTIME_CATALOG_H

#include <span>
#include <variant>

#include <QString>

#include "src/class/mo_wen/runtime_data.hpp"
#include "src/class/tai_xu_jian_yi/runtime_data.hpp"
#include "src/engine/default_macro.hpp"
#include "src/engine/scenarios.hpp"
#include "src/class/season50/permanents.hpp"

namespace desktop {
namespace rt = JX3DPS::runtime;
enum class Specialization
{
    LEGACY_TX = -2,
    LEGACY_MW = -1,
    TAI_XU    = 0,
    MO_WEN    = 1
};
using Config = JX3DPS::engine::ScenarioConfig;

inline const rt::season50::Book &School(Specialization s)
{
    return rt::season50::Books()[static_cast<std::size_t>(s)];
}

inline Specialization ClassOf(const Config &c)
{
    if (const auto *current = std::get_if<rt::season50::Config>(&c)) {
        const auto schools = rt::season50::Books();
        for (std::size_t i = 0; i < schools.size(); ++i) {
            if (current->book == &schools[i]) {
                return static_cast<Specialization>(i);
            }
        }
        throw std::invalid_argument("invalid school");
    }
    return c.index() == 0 ? Specialization::LEGACY_TX : Specialization::LEGACY_MW;
}

inline QString Text(std::string_view value)
{
    return QString::fromUtf8(value.data(), value.size());
}

inline Specialization SchoolFromId(const QString &code)
{
    const auto books = rt::season50::Books();
    for (std::size_t i = 0; i < books.size(); ++i) {
        if (Text(books[i].code) == code) {
            return static_cast<Specialization>(i);
        }
    }
    throw std::invalid_argument("unknown school code");
}

inline QString ClassName(Specialization s)
{
    if (s == Specialization::LEGACY_TX) {
        return QStringLiteral("太虚剑意");
    }
    if (s == Specialization::LEGACY_MW) {
        return QStringLiteral("莫问");
    }
    return Text(School(s).name);
}

using Catalog = rt::SkillCatalog;

inline Catalog Describe(Specialization s)
{
    if (s == Specialization::LEGACY_TX) {
        return rt::tai_xu::Catalog;
    }
    if (s == Specialization::LEGACY_MW) {
        return rt::mo_wen::Catalog;
    }
    return School(s).Catalog();
}

inline rt::CompileOptions MacroOptions(Specialization s, bool debug = false)
{
    return rt::CatalogMacroOptions(Describe(s), debug);
}

using SchoolInfo = rt::season50::SchoolInfo;

inline SchoolInfo Info(Specialization s)
{
    return rt::season50::Info(s == Specialization::LEGACY_TX ? "tx" : s == Specialization::LEGACY_MW ? "mw" : School(s).code);
}

inline QString DisplayName(std::string_view name)
{
    return Text(name).replace(QStringLiteral("Dot"), QStringLiteral("持续伤害")).replace(QStringLiteral("DOT"), QStringLiteral("持续伤害"));
}

inline QString SkillName(Specialization value, rt::Slot skill)
{
    const auto SKILLS = Describe(value).skills;
    return skill < SKILLS.size() ? DisplayName(SKILLS[skill].name) : QStringLiteral("未知技能");
}

inline QString DefaultMacro(Specialization value)
{
    if (static_cast<int>(value) < 0) {
        return Text(JX3DPS::engine::DefaultMacro(value == Specialization::LEGACY_MW));
    }
    return Text(School(value).default_macro);
}

inline QString FieldName(const QString &key)
{
    static const std::pair<const char *, const char *> fields[] = {
        { "iterations",       "模拟次数"       },
        { "workers",          "工作线程"       },
        { "seconds",          "战斗时长"       },
        { "seed",             "随机种子"       },
        { "qidian",           "初始气点"       },
        { "playerLife",       "自身气血"       },
        { "incomingDamage",   "单次受击"       },
        { "delay",            "输入延迟"       },
        { "targetLevel",      "目标等级"       },
        { "shield",           "目标防御"       },
        { "targetLife",       "目标气血"       },
        { "distance",         "目标距离"       },
        { "attackBase",       "基础攻击"       },
        { "attackClass",      "心法攻击"       },
        { "weapon",           "武器伤害"       },
        { "crit",             "会心等级"       },
        { "critPower",        "会效等级"       },
        { "overcomeBase",     "基础破防"       },
        { "overcomeClass",    "心法破防"       },
        { "strain",           "无双等级"       },
        { "surplus",          "破招等级"       },
        { "haste",            "加速等级"       },
        { "attackBonus",      "攻击加成"       },
        { "overcomeBonus",    "破防加成"       },
        { "critBonus",        "会心加成"       },
        { "critPowerBonus",   "会效加成"       },
        { "hasteBonus",       "加速加成"       },
        { "damageBonus",      "伤害加成"       },
        { "strainBonus",      "无双加成"       },
        { "pveBonus",         "非侠士伤害加成" },
        { "ignoreShield",     "无视防御"       },
        { "vulnerable",       "目标易伤"       },
        { "mwShadows",        "初始影子"       },
        { "mwStyle",          "初始曲目"       },
        { "importedMajor",    "已含主属性"     },
        { "importedStrength", "已含力道"       }
    };
    for (const auto &[NAME, LABEL] : fields) {
        if (key == QString::fromLatin1(NAME)) {
            return QString::fromUtf8(LABEL);
        }
    }
    if (key.startsWith("permanent")) {
        return QStringLiteral("常驻增益");
    }
    if (key.startsWith("talent") || key.startsWith("mwTalent")) {
        return QStringLiteral("奇穴选择");
    }
    if (key.startsWith("equipment") || key.startsWith("mwEquipment")) {
        return QStringLiteral("装备特效");
    }
    if (key.startsWith("team") || key.startsWith("mwTeam") || key.startsWith("txTeam")) {
        return QStringLiteral("团队增益");
    }
    return QStringLiteral("秘籍选择");
}
} // namespace desktop
#endif
