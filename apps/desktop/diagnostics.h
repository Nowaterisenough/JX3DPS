#ifndef DESKTOP_DIAGNOSTICS_H
#define DESKTOP_DIAGNOSTICS_H
#include <QDebug>
#include <QString>

namespace desktop {
inline QString Diagnostic(const QString &message)
{
    static const std::pair<const char *, const char *> messages[] = {
        { "missing skill/buff name",                                                            "缺少技能或增益名称"                                       },
        { "unknown or out-of-range name: ",                                                     "当前心法不支持此名称："                                   },
        { "empty or unsupported condition",                                                     "条件为空或使用了不支持的语法"                             },
        { "invalid comparator/value",                                                           "比较符或比较值无效"                                       },
        { "invalid numeric value",                                                              "数值无效"                                                 },
        { "nobuff/tnobuff do not accept a comparator",                                          "nobuff / tnobuff 后不能使用比较符"                        },
        { "last_skill expects a skill name without a comparator",                               "last_skill 后应为技能名，不带比较符"                      },
        { "resource condition does not take a name",                                            "资源条件不能指定名称"                                     },
        { "explicit target ids are not supported by this macro compiler",                       "当前宏不支持指定目标编号"                                 },
        { "life/mana must be in [0,1]",                                                         "气血与内力比例须在 0 至 1 之间"                           },
        { "unknown condition: ",                                                                "未知条件："                                               },
        { "condition requires a comparator",                                                    "此条件需要比较符"                                         },
        { "stack/resource value must be an integer",                                            "层数或资源数量须为整数"                                   },
        { "time value overflows frame range",                                                   "时间超出可表示的帧范围"                                   },
        { "condition requires a named skill/buff",                                              "此条件需要指定技能或增益名称"                             },
        { "invalid frame rate or slot count",                                                   "帧率或技能槽数量无效"                                     },
        { "unknown command: ",                                                                  "未知指令："                                               },
        { "missing action",                                                                     "缺少施放技能"                                             },
        { "missing closing bracket",                                                            "缺少右方括号 ]"                                           },
        { "missing or invalid action",                                                          "缺少施放技能或技能语法无效"                               },
        { "haste produces a zero-length timer",                                                 "加速过高，导致技能时间不足一帧"                           },
        { "haste produces a zero-length Mo Wen timer",                                          "莫问加速过高，导致技能时间不足一帧"                       },
        { "unmigrated talent flags",                                                            "包含尚未迁移的奇穴"                                       },
        { "unmigrated equipment effects",                                                       "包含尚未迁移的装备特效"                                   },
        { "unmigrated team effects",                                                            "包含尚未迁移的团队增益"                                   },
        { "unsupported ShengTaiJi recipe",                                                      "生太极秘籍选择无效"                                       },
        { "unsupported RenJian recipe or more than four recipes",                               "人剑合一秘籍选择无效或超过四本"                           },
        { "unsupported recipes or more than four recipes on a skill",                           "秘籍选择无效；每个技能最多四本"                           },
        { "XuanMen/split overcome requires explicit overcome_base and valid overcome_by_class", "玄门需要有效的基础破防与心法破防加成值"                   },
        { "combat attribute outside supported range",                                           "战斗属性超出支持范围"                                     },
        { "attribute bonus outside supported range",                                            "属性加成超出支持范围"                                     },
        { "unsupported target, resource or delay configuration",                                "目标、资源或延迟配置无效"                                 },
        { "Mo Wen attributes and log capacity could overflow damage",                           "莫问属性与记录容量可能造成伤害溢出，请降低属性或战斗时长" },
        { "Mo Wen log capacity could overflow accumulated critical power",                      "莫问战斗记录容量可能造成累计会效溢出，请降低战斗时长"     },
        { "Mo Wen attribute outside 0..1000000",                                                "莫问属性须在 0 至 1000000 之间"                           },
        { "Mo Wen bonus outside 0..10000",                                                      "莫问属性加成超出支持范围"                                 },
        { "invalid shield ignore",                                                              "无视防御比例无效"                                         },
        { "invalid Mo Wen target",                                                              "莫问目标配置无效"                                         },
        { "unknown Mo Wen talent or team effect",                                               "莫问奇穴或团队增益无效"                                   },
        { "unknown Mo Wen equipment effect",                                                    "莫问装备特效无效"                                         },
        { "invalid initial Mo Wen state",                                                       "莫问初始曲风、影子或延迟无效"                             },
        { "invalid Mo Wen recipe selection",                                                    "莫问秘籍选择无效；每个技能最多四本"                       },
        { "batch iterations, workers and duration must be positive",                            "模拟次数、线程数量和时长须为正数"                         },
        { "same-frame action budget exceeded",                                                  "同一帧施放次数过多，请检查宏是否有无限循环"               },
        { "same-frame event budget exceeded",                                                   "同一帧事件次数过多，请检查配置"                           },
        { "event log capacity exhausted",                                                       "战斗记录容量不足，请缩短战斗时长或减少事件"               },
        { "macro was not compiled for Mo Wen",                                                  "此宏未按莫问心法编译"                                     },
        { "macro was not compiled for Tai Xu",                                                  "此宏未按太虚剑意心法编译"                                 }
    };
    for (const auto &[english, chinese] : messages) {
        const auto source = QString::fromUtf8(english);
        if (message == source) {
            return QString::fromUtf8(chinese);
        }
        if (source.endsWith(": ") && message.startsWith(source)) {
            return QString::fromUtf8(chinese) + message.mid(source.size());
        }
    }
    for (const auto ch : message) {
        if (ch.unicode() >= 0x4e00 && ch.unicode() <= 0x9fff) {
            return message;
        }
    }
    qWarning().noquote() << message;
    return QStringLiteral("模拟发生内部错误，详细原因已写入程序日志。");
}
} // namespace desktop
#endif
