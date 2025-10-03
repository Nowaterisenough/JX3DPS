#ifndef DEBUG_TOOLBAR_H
#define DEBUG_TOOLBAR_H

#include <QWidget>
#include <QToolButton>

class DebugToolbarPrivate;

/**
 * @brief VSCode 风格的浮动调试工具栏
 *
 * 特性：
 * - 浮窗设计，可拖动
 * - 圆角半透明背景
 * - 紧凑的按钮布局
 * - VSCode 深色主题配色
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

    // 显示/隐藏浮窗
    void ShowFloating();
    void HideFloating();

signals:
    // 调试控制信号
    void ContinueClicked();    // 继续执行
    void PauseClicked();       // 暂停执行
    void StepOverClicked();    // 单步跳过
    void StepIntoClicked();    // 单步进入
    void StepOutClicked();     // 单步跳出
    void RestartClicked();     // 重启调试
    void StopClicked();        // 停止调试

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void SetupUI();
    void UpdateButtonStates();

    QScopedPointer<DebugToolbarPrivate> d_ptr;
    Q_DECLARE_PRIVATE(DebugToolbar)
};

#endif // DEBUG_TOOLBAR_H
