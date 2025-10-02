#include "debug_toolbar.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QToolButton>
#include <QMouseEvent>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>

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
        dragging(false),
        dragStartPos(),
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

    // 拖动相关
    bool dragging;
    QPoint dragStartPos;

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
    // 设置为浮窗
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    SetupUI();
}

DebugToolbar::~DebugToolbar() = default;

void DebugToolbar::SetupUI()
{
    Q_D(DebugToolbar);

    // 设置固定大小和圆角
    setFixedHeight(40);
    setMinimumWidth(280);

    // VSCode 浮窗样式
    setStyleSheet(R"(
        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 4px;
            padding: 6px;
            min-width: 32px;
            min-height: 32px;
            color: #cccccc;
            font-size: 16px;
        }
        QToolButton:hover {
            background-color: rgba(255, 255, 255, 0.1);
        }
        QToolButton:pressed {
            background-color: rgba(255, 255, 255, 0.15);
        }
        QToolButton:disabled {
            color: #656565;
        }
    )");

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(2);

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

void DebugToolbar::ShowFloating()
{
    // 显示在父窗口的中上部
    if (parentWidget()) {
        QPoint center = parentWidget()->rect().center();
        move(center.x() - width() / 2, 60);
    }
    show();
}

void DebugToolbar::HideFloating()
{
    hide();
}

void DebugToolbar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制圆角半透明背景 - VSCode 深色主题
    QPainterPath path;
    path.addRoundedRect(rect(), 8, 8);

    // 背景色：深灰色，带透明度
    painter.fillPath(path, QColor(45, 45, 45, 240));

    // 绘制边框
    painter.setPen(QPen(QColor(60, 60, 60), 1));
    painter.drawPath(path);

    // 绘制顶部高光
    QPainterPath highlightPath;
    highlightPath.addRoundedRect(rect().adjusted(1, 1, -1, -rect().height() / 2), 7, 7);
    painter.fillPath(highlightPath, QColor(255, 255, 255, 10));
}

void DebugToolbar::mousePressEvent(QMouseEvent *event)
{
    Q_D(DebugToolbar);
    if (event->button() == Qt::LeftButton) {
        d->dragging = true;
        d->dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void DebugToolbar::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(DebugToolbar);
    if (d->dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - d->dragStartPos);
        event->accept();
    }
}
