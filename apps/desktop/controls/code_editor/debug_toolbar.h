#ifndef DEBUG_TOOLBAR_H
#define DEBUG_TOOLBAR_H

#include <QWidget>
#include <QToolButton>

class DebugToolbarPrivate;

/**
 * @brief VSCode 风格的调试工具栏
 *
 * 提供调试控制功能：
 * - 继续/暂停
 * - 单步跳过
 * - 单步进入
 * - 单步跳出
 * - 重启
 * - 停止
 */
class DebugToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit DebugToolbar(QWidget *parent = nullptr);
    ~DebugToolbar() override;

    // 调试状态
    enum DebugState {
        Stopped,   // 停止状态
        Running,   // 运行中
        Paused     // 已暂停
    };

    void SetDebugState(DebugState state);
    DebugState GetDebugState() const;

signals:
    // 调试控制信号
    void ContinueClicked();    // 继续执行
    void PauseClicked();       // 暂停执行
    void StepOverClicked();    // 单步跳过
    void StepIntoClicked();    // 单步进入
    void StepOutClicked();     // 单步跳出
    void RestartClicked();     // 重启调试
    void StopClicked();        // 停止调试

private:
    void SetupUI();
    void UpdateButtonStates();

    QScopedPointer<DebugToolbarPrivate> d_ptr;
    Q_DECLARE_PRIVATE(DebugToolbar)
};

#endif // DEBUG_TOOLBAR_H
