#ifndef DESKTOP_MACRO_STEP_H
#define DESKTOP_MACRO_STEP_H

#include <QMetaType>
#include <QString>
#include <QVector>

inline constexpr int MacroStepHistoryLimit = 2048;
enum class MacroStepKind { Condition, Action, Skipped, Interruption };
struct MacroStep {
    quint64 ordinal = 0;
    int frame = 0, line = 0;
    MacroStepKind kind = MacroStepKind::Condition;
    QString expression, outcome, details;
    bool passed = false;
};
using MacroSteps = QVector<MacroStep>;
Q_DECLARE_METATYPE(MacroSteps)

#endif
