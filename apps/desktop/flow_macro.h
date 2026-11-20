#pragma once

#include <array>
#include <cmath>
#include <utility>

#include <QJsonObject>
#include <QRegularExpression>
#include <QSet>
#include <QStringList>
#include <QVector>

#include "src/core/runtime/target_timeline.hpp"

namespace desktop {
struct FlowField
{
    const char *command{};
    const char *parameter{};
    const char *key{};
    double      initial{}, minimum{}, maximum{};
    int         decimals{};
    bool        percentage = false;
};

inline constexpr std::array<FlowField, 8> FlowFields{
    { { "/set_target", "level", "targetLevel", 124, 110, 130, 0 },
     { "/set_target", "shield", "shield", 30000, 0, 10000000, 0 },
     { "/set_target", "life", "targetLife", 100, 1, 100, 2, true },
     { "/set_target", "distance", "distance", 4, 0, 1000, 2 },
     { "/set_target", "vulnerable", "vulnerable", 0, 0, 976.5625, 4, true },
     { "/set_player", "qidian", "qidian", 10, 0, 10, 0 },
     { "/set_player", "shadows", "mwShadows", 0, 0, 6, 0 },
     { "/set_player", "style", "mwStyle", 1, 0, 1, 0 } }
};

inline QJsonObject FlowDefaults()
{
    QJsonObject values;
    for (const auto &field : FlowFields) {
        values.insert(QString::fromLatin1(field.key), field.initial);
    }
    return values;
}

inline QString FlowFieldName(int index)
{
    static const std::array<const char *, 8> names{ "目标等级", "目标防御", "目标气血", "目标距离",
                                                    "目标易伤", "初始气点", "初始影子", "初始曲目" };
    return QString::fromUtf8(names.at(index));
}

inline int FlowFieldIndex(const QString &key)
{
    for (int index = 0; std::cmp_less(index, FlowFields.size()); ++index) {
        if (key == QLatin1String(FlowFields[index].key)) {
            return index;
        }
    }
    return -1;
}

inline QString FlowValueText(const FlowField &field, double value)
{
    if (QByteArray(field.key) == "mwStyle" && (value == 0 || value == 1)) {
        return value == 0 ? QStringLiteral("阳春白雪") : QStringLiteral("高山流水");
    }
    return QString::number(value, 'g', 15) + (field.percentage ? QStringLiteral("%") : QString{});
}

inline QString FlowTimeText(int frame)
{
    auto fraction = QString::number((frame % 16) * 625).rightJustified(4, '0');
    while (fraction.endsWith('0')) {
        fraction.chop(1);
    }
    return QStringLiteral("%1:%2%3")
        .arg(frame / 960, 2, 10, QLatin1Char('0'))
        .arg((frame / 16) % 60, 2, 10, QLatin1Char('0'))
        .arg(fraction.isEmpty() ? QString{} : '.' + fraction);
}

inline bool ParseFlowTime(const QString &text, int &frame)
{
    static const QRegularExpression pattern(QStringLiteral("^([0-9]+):([0-9]{1,2})(?:\\.([0-9]{1,4}))?$"));
    const auto                      MATCH = pattern.match(text);
    if (!MATCH.hasMatch()) {
        return false;
    }
    bool       valid    = false;
    const auto MINUTES  = MATCH.captured(1).toLongLong(&valid);
    const auto SECONDS  = MATCH.captured(2).toInt();
    const auto FRACTION = MATCH.captured(3).leftJustified(4, '0').toInt();
    if (!valid || MINUTES > 1440 || SECONDS >= 60 || ((FRACTION % 625) != 0)) {
        return false;
    }
    const auto FRAMES = (((MINUTES * 60) + SECONDS) * 16) + (FRACTION / 625);
    if (FRAMES > JX3DPS::runtime::MaximumFlowFrame) {
        return false;
    }
    frame = static_cast<int>(FRAMES);
    return true;
}

struct FlowNode
{
    int         line = 0, frame = 0, timeColumn = -1, timeLength = 0, commandColumn = 0;
    QString     command, original, indent, tail;
    QJsonObject values;
    bool        explicitId = false;

    bool CanMove() const
    {
        if (command != "/set_target" || values.isEmpty()) {
            return false;
        }
        for (const auto &key : values.keys()) {
            if (key != "targetLife" && key != "distance") {
                return false;
            }
        }
        return true;
    }

    QString Summary() const
    {
        QStringList parts;
        for (int index = 0; std::cmp_less(index, FlowFields.size()); ++index) {
            const auto &field = FlowFields[index];
            if (values.contains(QString::fromLatin1(field.key))) {
                parts << FlowFieldName(index) + ' ' + FlowValueText(field, values.value(QString::fromLatin1(field.key)).toDouble());
            }
        }
        return parts.isEmpty() ? QStringLiteral("目标 1") : parts.join(QStringLiteral(" · "));
    }

    QString WithTime(int time) const
    {
        auto text = original;
        if (timeColumn >= 0) {
            text.replace(timeColumn, timeLength, FlowTimeText(time));
        } else if (time > 0) {
            text.insert(commandColumn, FlowTimeText(time) + ' ');
        }
        return text;
    }

    QString WithValues(int time, const QString &kind, const QJsonObject &fields) const
    {
        QStringList parts;
        if (time > 0 || timeColumn >= 0) {
            parts << FlowTimeText(time);
        }
        parts << kind;
        if (explicitId && kind == "/set_target") {
            parts << "id=1";
        }
        for (const auto &field : FlowFields) {
            const auto KEY = QString::fromLatin1(field.key);
            if (fields.contains(KEY) && kind == QLatin1String(field.command)) {
                parts << QString::fromLatin1(field.parameter) + '=' + FlowValueText(field, fields.value(KEY).toDouble());
            }
        }
        return indent + parts.join(' ') + tail;
    }
};

inline bool ValidFlowValue(const FlowField &field, const QJsonValue &value)
{
    const auto NUMBER = value.toDouble();
    const auto SCALE  = std::pow(10., field.decimals);
    return value.isDouble() && std::isfinite(NUMBER) && NUMBER >= field.minimum && NUMBER <= field.maximum &&
           (field.decimals == 0 ? std::floor(NUMBER) == NUMBER : std::abs((NUMBER * SCALE) - std::round(NUMBER * SCALE)) < 0.0001);
}

inline QString FlowMacroFromValues(const QJsonObject &values)
{
    QStringList lines;
    for (const auto &field : FlowFields) {
        const auto value   = values.value(QString::fromLatin1(field.key)).toDouble(field.initial);
        const auto LITERAL = FlowValueText(field, value);
        lines << QStringLiteral("%1 %2=%3").arg(QString::fromLatin1(field.command), QString::fromLatin1(field.parameter), LITERAL);
    }
    return lines.join('\n');
}

struct FlowDiagnostic
{
    int     line, column, length;
    QString message;
};

struct CompiledFlow
{
    QJsonObject             values = FlowDefaults();
    QVector<FlowDiagnostic> errors;
    QVector<FlowNode>       nodes;

    std::vector<JX3DPS::runtime::TargetEvent> TargetEvents() const
    {
        std::vector<JX3DPS::runtime::TargetEvent> events;
        if (!errors.isEmpty()) {
            return events;
        }
        for (const auto &node : nodes) {
            if (node.frame == 0) {
                continue;
            }
            JX3DPS::runtime::TargetEvent event;
            event.frame = node.frame;
            if (node.values.contains("targetLife")) {
                event.life_basis_points = qRound(node.values.value("targetLife").toDouble() * 100);
            }
            if (node.values.contains("distance")) {
                event.distance_hundredths = qRound(node.values.value("distance").toDouble() * 100);
            }
            events.push_back(event);
        }
        std::stable_sort(events.begin(), events.end(), [](const auto &left, const auto &right) {
            return left.frame < right.frame;
        });
        return events;
    }

    QString ErrorText() const
    {
        return errors.isEmpty() ? QString{}
                                : QStringLiteral("流程宏第 %1 行：%2").arg(errors.front().line).arg(errors.front().message);
    }
};

// Frame-zero assignments initialize configuration. Later health/distance events
// enter the shared scheduler; text is never parsed during a fight.
inline CompiledFlow CompileFlowMacro(const QString &source)
{
    CompiledFlow result;
    if (source.size() > 65536) {
        result.errors.append({ 1, 0, 1, QStringLiteral("流程宏不能超过 65536 个字符。") });
        return result;
    }
    static const QRegularExpression tokenPattern(QStringLiteral("\\S+"));
    static const QRegularExpression namePattern(QStringLiteral("^[a-z_]+$"));
    static const QRegularExpression numberPattern(
        QStringLiteral("^[+-]?(?:[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+)(?:[eE][+-]?[0-9]+)?$"));
    static const QRegularExpression timePattern(QStringLiteral("^[0-9]+:[0-9]+(?:\\.[0-9]+)?$"));
    const auto                      LINES = source.split('\n');
    for (int row = 0; row < LINES.size(); ++row) {
        auto text = LINES[row];
        for (int column = 0; column < text.size(); ++column) {
            if ((column == 0 || text[column - 1].isSpace()) && (text[column] == '#' || text.mid(column, 2) == "//")) {
                text.truncate(column);
                break;
            }
        }
        QVector<QRegularExpressionMatch> tokens;
        auto                             matches = tokenPattern.globalMatch(text);
        while (matches.hasNext()) {
            tokens.append(matches.next());
        }
        if (tokens.isEmpty()) {
            continue;
        }
        FlowNode node;
        node.line                  = row + 1;
        node.original              = LINES[row];
        node.indent                = text.left(tokens.front().capturedStart());
        const auto LAST_END        = tokens.back().capturedEnd();
        node.tail                  = LINES[row].mid(LAST_END);
        const int  PREVIOUS_ERRORS = result.errors.size();
        const auto FAIL            = [&](int index, const QString &message) {
            const auto &token = tokens[index];
            result.errors.append({ row + 1, int(token.capturedStart()), int(token.capturedLength()), message });
        };
        int command_index = 0;
        if (timePattern.match(tokens.front().captured()).hasMatch()) {
            if (!ParseFlowTime(tokens.front().captured(), node.frame)) {
                FAIL(0, QStringLiteral("时间格式为 分:秒，最多四位小数，且必须是 1/16 秒的整数倍（如 00:00.0625），不超过 24 小时。"));
                continue;
            }
            node.timeColumn = tokens.front().capturedStart();
            node.timeLength = tokens.front().capturedLength();
            if (++command_index == tokens.size()) {
                FAIL(0, QStringLiteral("时间后缺少流程指令。"));
                continue;
            }
        }
        const auto COMMAND = tokens[command_index].captured();
        node.command       = COMMAND;
        node.commandColumn = tokens[command_index].capturedStart();
        if (COMMAND != "/set_target" && COMMAND != "/set_player") {
            FAIL(command_index, QStringLiteral("仅支持 /set_target 和 /set_player；循环技能请写入循环宏，多目标等指令尚未支持。"));
            continue;
        }
        if (command_index + 1 == tokens.size()) {
            FAIL(command_index, QStringLiteral("缺少参数，请使用 参数=值。"));
            continue;
        }
        QSet<QString> assigned;
        for (int index = command_index + 1; index < tokens.size(); ++index) {
            const auto TOKEN = tokens[index].captured();
            const int  EQUAL = TOKEN.indexOf('=');
            if (EQUAL <= 0 || EQUAL == TOKEN.size() - 1) {
                FAIL(index, QStringLiteral("参数应写成 参数=值，等号两侧不留空格。"));
                continue;
            }
            const auto PARAMETER = TOKEN.left(EQUAL);
            auto       literal   = TOKEN.mid(EQUAL + 1);
            if (!namePattern.match(PARAMETER).hasMatch()) {
                FAIL(index, QStringLiteral("参数名只能使用小写字母和下划线。"));
                continue;
            }
            if (assigned.contains(PARAMETER)) {
                FAIL(index, QStringLiteral("同一行重复设置了参数 %1。分行可按顺序覆盖。").arg(PARAMETER));
                continue;
            }
            assigned.insert(PARAMETER);
            if (COMMAND == "/set_target" && PARAMETER == "id") {
                if (literal != "1") {
                    FAIL(index, QStringLiteral("当前引擎仅支持单目标 id=1。"));
                }
                node.explicitId = true;
                continue;
            }
            const FlowField *field = nullptr;
            for (const auto &candidate : FlowFields) {
                if (COMMAND == QLatin1String(candidate.command) && PARAMETER == QLatin1String(candidate.parameter)) {
                    field = &candidate;
                }
            }
            if (field == nullptr) {
                FAIL(index, QStringLiteral("未知参数：%1。").arg(PARAMETER));
                continue;
            }
            if (node.frame > 0 && (COMMAND != "/set_target" || (PARAMETER != "life" && PARAMETER != "distance"))) {
                FAIL(index, QStringLiteral("战中仅支持目标气血 life 和距离 distance；其他参数须放在 0 秒。"));
                continue;
            }
            double number = 0;
            bool   valid  = false;
            if (QByteArray(field->key) == "mwStyle" && (literal == QStringLiteral("阳春白雪") || literal == QStringLiteral("高山流水")))
            {
                number = literal == QStringLiteral("阳春白雪") ? 0 : 1;
                valid  = true;
            } else {
                const bool percent = literal.endsWith('%');
                if (percent) {
                    literal.chop(1);
                }
                if (percent && !field->percentage) {
                    FAIL(index, QStringLiteral("此参数不接受百分号。"));
                    continue;
                }
                if (numberPattern.match(literal).hasMatch()) {
                    number = literal.toDouble(&valid);
                }
                if (field->percentage && !percent) {
                    number *= 100;
                }
            }
            if (!valid || !ValidFlowValue(*field, number)) {
                if (QByteArray(field->key) == "mwStyle") {
                    FAIL(index, QStringLiteral("style 只能为阳春白雪/高山流水，或对应的 0/1。"));
                    continue;
                }
                FAIL(index,
                     QStringLiteral("%1 必须在 %2–%3%4 之间，最多 %5 位小数。")
                         .arg(PARAMETER)
                         .arg(field->minimum)
                         .arg(field->maximum)
                         .arg(field->percentage ? "%" : "")
                         .arg(field->decimals));
                continue;
            }
            node.values.insert(QString::fromLatin1(field->key), number);
        }
        if (node.frame > 0 && node.values.isEmpty() && result.errors.size() == PREVIOUS_ERRORS) {
            FAIL(command_index, QStringLiteral("战中节点必须设置目标气血或距离。"));
        }
        if (result.errors.size() != PREVIOUS_ERRORS) {
            continue;
        }
        if (node.frame == 0) {
            for (auto field = node.values.begin(); field != node.values.end(); ++field) {
                result.values.insert(field.key(), field.value());
            }
        }
        result.nodes.append(node);
        if (result.nodes.size() > int(JX3DPS::runtime::MaximumTargetEvents)) {
            FAIL(command_index, QStringLiteral("流程宏最多支持 256 个节点。"));
            break;
        }
    }
    return result;
}
} // namespace desktop
