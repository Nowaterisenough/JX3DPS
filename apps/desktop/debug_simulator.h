#ifndef DEBUG_SIMULATOR_H
#define DEBUG_SIMULATOR_H

#include <QString>
#include <QObject>
#include <memory>

/**
 * @brief 调试模拟器 - 连接DebugSession和JX3DPS引擎
 *
 * 负责：
 * 1. 解析宏文本为ExprSkillsHash
 * 2. 初始化Player和Targets
 * 3. 生成KeyFrameSequence
 * 4. 单步执行KeyFrame
 * 5. 提供玩家状态查询
 */
class DebugSimulator : public QObject
{
    Q_OBJECT

public:
    explicit DebugSimulator(QObject *parent = nullptr);
    ~DebugSimulator();

    // 初始化模拟器（解析宏，创建Player）
    bool Initialize(const QString &macroText, QString &errorMessage);

    // 单步进入（进入条件判定，逐个评估条件）
    bool StepInto();

    // 单步跳过（执行完整的一行指令）
    bool StepOver();

    // 单步跳出（跳出当前作用域）
    bool StepOut();

    // 单步执行（执行一个KeyFrame）- 保留兼容性
    bool StepOne();

    // 获取当前状态
    struct PlayerState {
        double lifePercent;
        double manaPercent;
        int qidian;
        int rage;
        int energy;
        int targetId;
        double targetLifePercent;
        int currentFrame;
        double currentSeconds;
        QString currentMacro;
        QString lastSkill;
        int currentMacroLine;  // 当前执行的宏行号（1-based）
    };
    PlayerState GetPlayerState() const;

    // 检查是否执行完成
    bool IsFinished() const;

    // 重置模拟器
    void Reset();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // DEBUG_SIMULATOR_H
