#include "test_data_generator.h"
#include <QRandomGenerator>
#include <QPixmap>

QVector<Timeline::EventItem> TestDataGenerator::GenerateEvents()
{
    QVector<Timeline::EventItem> events;

    // 技能数据
    struct SkillData {
        QString name;
        QString iconPath;
        QColor color;
    };

    static const QList<SkillData> skills = {
        {"碎星辰", ":/resources/images/JX3/Icons/2942.png", QColor(255, 100, 100)},
        {"无我无剑", ":/resources/images/JX3/Icons/2941.png", QColor(100, 255, 100)},
        {"三环套月", ":/resources/images/JX3/Icons/2936.png", QColor(100, 100, 255)},
        {"八荒归元", ":/resources/images/JX3/Icons/2934.png", QColor(255, 255, 100)},
        {"吞日月", ":/resources/images/JX3/Icons/2933.png", QColor(255, 100, 255)},
        {"人剑合一", ":/resources/images/JX3/Icons/2931.png", QColor(100, 255, 255)},
        {"生太极", ":/resources/images/JX3/Icons/2927.png", QColor(200, 150, 100)},
        {"紫气东来", ":/resources/images/JX3/Icons/2926.png", QColor(150, 100, 200)}
    };

    // 宏信息
    struct MacroInfo {
        QString name;
        QColor color;
    };

    static const QList<MacroInfo> macros = {
        {"宏1-基础", QColor(100, 150, 255)},
        {"宏2-太极", QColor(255, 150, 100)},
        {"宏3-爆发", QColor(255, 100, 150)},
        {"循环", QColor(150, 255, 100)}
    };

    const int totalDuration = 600000; // 600秒
    int currentMacroIndex = 0;
    int macroSwitchCounter = 0;

    for (int ms = 0; ms < totalDuration;) {
        // 每隔15-25秒切换宏
        if (macroSwitchCounter >= 10 + QRandomGenerator::global()->bounded(7)) {
            currentMacroIndex = (currentMacroIndex + 1) % macros.size();
            macroSwitchCounter = 0;
        }

        const auto &currentMacro = macros[currentMacroIndex];
        int skillCount = 2 + QRandomGenerator::global()->bounded(2);

        for (int i = 0; i < skillCount && ms < totalDuration; ++i) {
            Timeline::EventItem event;
            int skillIndex = QRandomGenerator::global()->bounded(skills.size());
            const auto &skill = skills[skillIndex];

            event.timestamp = ms;
            event.name = skill.name;
            event.icon = QPixmap(skill.iconPath);
            event.color = skill.color;
            event.damage = 10000 + QRandomGenerator::global()->bounded(50000);
            event.rollResult = QRandomGenerator::global()->bounded(3) + 1;
            event.macroName = currentMacro.name;
            event.macroColor = currentMacro.color;
            events.append(event);

            ms += 50 + QRandomGenerator::global()->bounded(100);
        }

        ms += 1300 + QRandomGenerator::global()->bounded(400);
        macroSwitchCounter++;
    }

    return events;
}

QVector<Timeline::BuffItem> TestDataGenerator::GenerateBuffs(int totalDurationMs)
{
    QVector<Timeline::BuffItem> buffs;

    // Buff 1: 破防
    Timeline::BuffItem buff1;
    buff1.name = "破防";
    buff1.icon = QPixmap(":/resources/images/JX3/Icons/2942.png");
    buff1.color = QColor(255, 100, 100, 180);
    for (int t = 0; t < totalDurationMs; t += 35000) {
        buff1.segments.append({t, qMin(t + 15000, totalDurationMs)});
    }
    buffs.append(buff1);

    // Buff 2: 无双
    Timeline::BuffItem buff2;
    buff2.name = "无双";
    buff2.icon = QPixmap(":/resources/images/JX3/Icons/2941.png");
    buff2.color = QColor(100, 255, 100, 180);
    for (int t = 5000; t < totalDurationMs; t += 50000) {
        buff2.segments.append({t, qMin(t + 20000, totalDurationMs)});
    }
    buffs.append(buff2);

    // Buff 3: 会心
    Timeline::BuffItem buff3;
    buff3.name = "会心";
    buff3.icon = QPixmap(":/resources/images/JX3/Icons/2936.png");
    buff3.color = QColor(100, 100, 255, 180);
    for (int t = 10000; t < totalDurationMs; t += 37000) {
        buff3.segments.append({t, qMin(t + 12000, totalDurationMs)});
    }
    buffs.append(buff3);

    // Buff 4: 加速
    Timeline::BuffItem buff4;
    buff4.name = "加速";
    buff4.icon = QPixmap(":/resources/images/JX3/Icons/2934.png");
    buff4.color = QColor(255, 255, 100, 180);
    for (int t = 0; t < totalDurationMs; t += 90000) {
        buff4.segments.append({t, qMin(t + 30000, totalDurationMs)});
    }
    buffs.append(buff4);

    // Buff 5: 外功
    Timeline::BuffItem buff5;
    buff5.name = "外功";
    buff5.icon = QPixmap(":/resources/images/JX3/Icons/2933.png");
    buff5.color = QColor(255, 150, 50, 180);
    buff5.segments.append({0, totalDurationMs});
    buffs.append(buff5);

    // Buff 6: 橙武
    Timeline::BuffItem buff6;
    buff6.name = "橙武";
    buff6.icon = QPixmap(":/resources/images/JX3/Icons/2931.png");
    buff6.color = QColor(255, 165, 0, 180);
    for (int t = 20000; t < totalDurationMs; t += 63000) {
        buff6.segments.append({t, qMin(t + 18000, totalDurationMs)});
    }
    buffs.append(buff6);

    return buffs;
}
