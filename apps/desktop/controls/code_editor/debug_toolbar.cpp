#include "debug_toolbar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QToolButton>

class DebugToolbarPrivate
{
public:
    explicit DebugToolbarPrivate(DebugToolbar *q) :
        state(DebugToolbar::Stopped),
        continueButton(nullptr),
        pauseButton(nullptr),
        stepOverButton(nullptr),
        stepIntoButton(nullptr),
        stepOutButton(nullptr),
        restartButton(nullptr),
        stopButton(nullptr),
        q_ptr(q)
    {
    }

    DebugToolbar::DebugState state;

    QToolButton *continueButton;
    QToolButton *pauseButton;
    QToolButton *stepOverButton;
    QToolButton *stepIntoButton;
    QToolButton *stepOutButton;
    QToolButton *restartButton;
    QToolButton *stopButton;

private:
    DebugToolbar *q_ptr;
    Q_DECLARE_PUBLIC(DebugToolbar)
};

// ============================================================================
// DebugToolbar Implementation
// ============================================================================

DebugToolbar::DebugToolbar(QWidget *parent) :
    QWidget(parent),
    d_ptr(new DebugToolbarPrivate(this))
{
    SetupUI();
}

DebugToolbar::~DebugToolbar() = default;

void DebugToolbar::SetupUI()
{
    Q_D(DebugToolbar);

    // VSCode 深色主题样式
    setStyleSheet(R"(
        QWidget {
            background-color: #252526;
            border-bottom: 1px solid #3e3e42;
        }
        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 3px;
            padding: 4px;
            min-width: 28px;
            min-height: 28px;
            color: #cccccc;
        }
        QToolButton:hover {
            background-color: #2a2d2e;
        }
        QToolButton:pressed {
            background-color: #094771;
        }
        QToolButton:disabled {
            color: #656565;
        }
        QLabel {
            color: #cccccc;
            font-size: 11px;
            padding: 0 8px;
        }
    )");

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(4);

    // 调试标签
    auto *debugLabel = new QLabel("DEBUG", this);
    debugLabel->setStyleSheet("font-weight: bold; color: #f48771;");
    layout->addWidget(debugLabel);

    layout->addSpacing(8);

    // 继续/暂停按钮
    d->continueButton = new QToolButton(this);
    d->continueButton->setText("▶");  // 播放图标
    d->continueButton->setToolTip("继续 (F5)");
    connect(d->continueButton, &QToolButton::clicked, this, &DebugToolbar::ContinueClicked);
    layout->addWidget(d->continueButton);

    d->pauseButton = new QToolButton(this);
    d->pauseButton->setText("⏸");  // 暂停图标
    d->pauseButton->setToolTip("暂停 (F6)");
    d->pauseButton->setVisible(false);
    connect(d->pauseButton, &QToolButton::clicked, this, &DebugToolbar::PauseClicked);
    layout->addWidget(d->pauseButton);

    // 单步跳过
    d->stepOverButton = new QToolButton(this);
    d->stepOverButton->setText("⤵");  // 跳过图标
    d->stepOverButton->setToolTip("单步跳过 (F10)");
    connect(d->stepOverButton, &QToolButton::clicked, this, &DebugToolbar::StepOverClicked);
    layout->addWidget(d->stepOverButton);

    // 单步进入
    d->stepIntoButton = new QToolButton(this);
    d->stepIntoButton->setText("⤓");  // 进入图标
    d->stepIntoButton->setToolTip("单步进入 (F11)");
    connect(d->stepIntoButton, &QToolButton::clicked, this, &DebugToolbar::StepIntoClicked);
    layout->addWidget(d->stepIntoButton);

    // 单步跳出
    d->stepOutButton = new QToolButton(this);
    d->stepOutButton->setText("⤴");  // 跳出图标
    d->stepOutButton->setToolTip("单步跳出 (Shift+F11)");
    connect(d->stepOutButton, &QToolButton::clicked, this, &DebugToolbar::StepOutClicked);
    layout->addWidget(d->stepOutButton);

    // 重启
    d->restartButton = new QToolButton(this);
    d->restartButton->setText("⟳");  // 重启图标
    d->restartButton->setToolTip("重启 (Ctrl+Shift+F5)");
    connect(d->restartButton, &QToolButton::clicked, this, &DebugToolbar::RestartClicked);
    layout->addWidget(d->restartButton);

    // 停止
    d->stopButton = new QToolButton(this);
    d->stopButton->setText("⏹");  // 停止图标
    d->stopButton->setToolTip("停止 (Shift+F5)");
    connect(d->stopButton, &QToolButton::clicked, this, &DebugToolbar::StopClicked);
    layout->addWidget(d->stopButton);

    layout->addStretch();

    // 初始状态
    UpdateButtonStates();
}

void DebugToolbar::SetDebugState(DebugState state)
{
    Q_D(DebugToolbar);
    if (d->state != state) {
        d->state = state;
        UpdateButtonStates();
    }
}

DebugToolbar::DebugState DebugToolbar::GetDebugState() const
{
    Q_D(const DebugToolbar);
    return d->state;
}

void DebugToolbar::UpdateButtonStates()
{
    Q_D(DebugToolbar);

    switch (d->state) {
    case Stopped:
        // 停止状态：只有继续按钮可用
        d->continueButton->setVisible(true);
        d->continueButton->setEnabled(true);
        d->pauseButton->setVisible(false);
        d->stepOverButton->setEnabled(false);
        d->stepIntoButton->setEnabled(false);
        d->stepOutButton->setEnabled(false);
        d->restartButton->setEnabled(false);
        d->stopButton->setEnabled(false);
        break;

    case Running:
        // 运行状态：显示暂停按钮，禁用步进按钮
        d->continueButton->setVisible(false);
        d->pauseButton->setVisible(true);
        d->pauseButton->setEnabled(true);
        d->stepOverButton->setEnabled(false);
        d->stepIntoButton->setEnabled(false);
        d->stepOutButton->setEnabled(false);
        d->restartButton->setEnabled(true);
        d->stopButton->setEnabled(true);
        break;

    case Paused:
        // 暂停状态：所有按钮可用
        d->continueButton->setVisible(true);
        d->continueButton->setEnabled(true);
        d->pauseButton->setVisible(false);
        d->stepOverButton->setEnabled(true);
        d->stepIntoButton->setEnabled(true);
        d->stepOutButton->setEnabled(true);
        d->restartButton->setEnabled(true);
        d->stopButton->setEnabled(true);
        break;
    }
}
