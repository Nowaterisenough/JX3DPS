#ifndef DESKTOP_RUNTIME_CATALOG_H
#define DESKTOP_RUNTIME_CATALOG_H

#include <span>
#include <variant>

#include <QString>

#include "src/class/mo_wen/runtime_data.hpp"
#include "src/class/tai_xu_jian_yi/runtime_data.hpp"

namespace desktop {
namespace rt = JX3DPS::runtime;
enum class Specialization
{
    TaiXu,
    MoWen
};
using Config = std::variant<rt::tai_xu::Config, rt::mo_wen::Config>;

inline Specialization ClassOf(const Config &config)
{
    return std::holds_alternative<rt::mo_wen::Config>(config) ? Specialization::MoWen : Specialization::TaiXu;
}

inline QString Text(std::string_view value)
{
    return QString::fromUtf8(value.data(), value.size());
}

inline QString ClassName(Specialization value)
{
    return value == Specialization::MoWen ? QStringLiteral("莫问") : QStringLiteral("太虚剑意");
}

inline rt::CompileOptions MacroOptions(Specialization value, bool debug = false)
{
    return value == Specialization::MoWen ? rt::mo_wen::MacroOptions(debug) : rt::tai_xu::MacroOptions(debug);
}

struct Catalog
{
    std::span<const rt::SkillDescriptor> skills;
    std::span<const rt::BuffDescriptor>  buffs;
    std::span<const rt::Slot>            castable;
    std::span<const std::string_view>    skillAliases, buffAliases;
};

inline Catalog Describe(Specialization value)
{
    if (value == Specialization::MoWen) {
        return { rt::mo_wen::Skills.entries, rt::mo_wen::Buffs.entries, rt::mo_wen::CastableSkills, rt::mo_wen::SkillAliases, rt::mo_wen::BuffAliases };
    }
    return { rt::tai_xu::Skills.entries, rt::tai_xu::Buffs.entries, rt::tai_xu::CastableSkills, rt::tai_xu::SkillAliases, rt::tai_xu::BuffAliases };
}

inline QString DisplayName(std::string_view name)
{
    return Text(name).replace(QStringLiteral("Dot"), QStringLiteral("持续伤害")).replace(QStringLiteral("DOT"), QStringLiteral("持续伤害"));
}

inline QString SkillName(Specialization value, rt::Slot skill)
{
    const auto skills = Describe(value).skills;
    return skill < skills.size() ? DisplayName(skills[skill].name) : QStringLiteral("未知技能");
}

inline QString DefaultMacro(Specialization value)
{
    if (value == Specialization::MoWen) {
        return QStringLiteral("# 莫问：持续伤害、影子与曲风循环\n/cast [tnobuff:商] 商\n/cast [tnobuff:角] 角\n/cast "
                              "疏影横斜\n/cast 高山流水\n/cast [buff:曲风=4] 羽\n/cast 变徵\n/cast 变宫\n");
    }
    return QStringLiteral(
        "# 太虚剑意：中文技能名与拼音别名均可使用\n/cast [nobuff:紫气东来&qidian<6] 紫气东来\n/cast [qidian>=8] "
        "无我无剑\n/cast 八荒归元\n/cast 三环套月\n/cast 三柴剑法\n/cast [qidian>=1] 无我无剑\n");
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
    for (const auto &[name, label] : fields) {
        if (key == QString::fromLatin1(name)) {
            return QString::fromUtf8(label);
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
