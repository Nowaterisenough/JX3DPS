#ifndef TEST_DATA_GENERATOR_H
#define TEST_DATA_GENERATOR_H

#include "controls/timeline/timeline.h"
#include <QVector>

/**
 * @brief 测试数据生成器
 *
 * 职责: 为Timeline生成模拟的战斗数据
 *
 * Linus原则:
 * - 测试代码不应该混在main里
 * - 数据生成是独立职责,应该独立出来
 */
class TestDataGenerator
{
public:
    // 生成技能释放事件 (600秒战斗数据)
    static QVector<Timeline::EventItem> GenerateEvents();

    // 生成Buff覆盖数据
    static QVector<Timeline::BuffItem> GenerateBuffs(int totalDurationMs);
};

#endif // TEST_DATA_GENERATOR_H
