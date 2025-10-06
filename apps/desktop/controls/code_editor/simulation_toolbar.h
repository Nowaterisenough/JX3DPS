#ifndef SIMULATION_TOOLBAR_H
#define SIMULATION_TOOLBAR_H

#include <QWidget>
#include <QToolButton>

class SimulationToolbarPrivate;

/**
 * @brief 模拟控制工具栏
 *
 * 提供：
 * - 开始模拟按钮
 * - 调试模式切换按钮
 */
class SimulationToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationToolbar(QWidget *parent = nullptr);
    ~SimulationToolbar() override;

    // 获取调试模式状态
    bool IsDebugModeEnabled() const;

signals:
    void StartSimulationClicked();         // 开始模拟
    void DebugModeToggled(bool enabled);  // 调试模式切换

private:
    void SetupUI();

    QScopedPointer<SimulationToolbarPrivate> d_ptr;
    Q_DECLARE_PRIVATE(SimulationToolbar)
};

#endif // SIMULATION_TOOLBAR_H
