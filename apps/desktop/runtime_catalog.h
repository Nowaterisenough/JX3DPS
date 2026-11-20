#ifndef DESKTOP_RUNTIME_CATALOG_H
#define DESKTOP_RUNTIME_CATALOG_H

#include <span>
#include <variant>

#include <QString>

#include "src/class/mo_wen/runtime_data.hpp"
#include "src/class/tai_xu_jian_yi/runtime_data.hpp"
#include "src/engine/default_macro.hpp"

namespace desktop {
namespace rt = JX3DPS::runtime;
enum class Specialization
{
    TAI_XU,
    MO_WEN
};
using Config = std::variant<rt::tai_xu::Config, rt::mo_wen::Config>;

inline Specialization ClassOf(const Config &config)
{
    return static_cast<Specialization>(config.index());
}

inline QString Text(std::string_view value)
{
    return QString::fromUtf8(value.data(), value.size());
}

inline QString ClassName(Specialization value)
{
    return value == Specialization::MO_WEN ? QStringLiteral("莫问") : QStringLiteral("太虚剑意");
}

inline rt::CompileOptions MacroOptions(Specialization value, bool debug = false)
{
    return rt::CatalogMacroOptions(value == Specialization::MO_WEN ? rt::mo_wen::Catalog : rt::tai_xu::Catalog, debug);
}

using Catalog = rt::SkillCatalog;

inline Catalog Describe(Specialization value)
{
    return value == Specialization::MO_WEN ? rt::mo_wen::Catalog : rt::tai_xu::Catalog;
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
    return Text(JX3DPS::engine::DefaultMacro(value == Specialization::MO_WEN));
}

inline QString FieldName(const QString &key)
{
    static const std::pair<const char *, const char *> fields[] = {
        { "iterations",       "模拟次数"       },
        { "workers",          "工作线程"       },
        { "seconds",          "战斗时长"       },
        { "seed",             "随机种子"       },
        { "qidian",           "初始气点"       },
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
