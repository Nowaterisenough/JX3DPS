#include "debug_toolbar.h"

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPainter>
#include <QToolButton>
#include <QMouseEvent>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QSizePolicy>
#include <QStringList>
#include <QFontDatabase>
#include <QFont>
#include <QDebug>
#include <QStyle>
#include <QPointer>

// 自定义暂停按钮 - 绘制两个实心竖条
class PauseButton : public QToolButton
{
public:
    explicit PauseButton(QWidget *parent = nullptr) : QToolButton(parent) {}

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QToolButton::paintEvent(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 获取按钮颜色
        QColor color = property("toolbarColorRole").toString() == "pause"
                       ? QColor(116, 191, 241)
                       : QColor(187, 187, 187); // #bbbbbb

        if (!isEnabled()) {
            color = QColor(90, 93, 94); // #5a5d5e
        }

        // 在按钮中心绘制两个竖条
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);

        int centerX = width() / 2;
        int centerY = height() / 2;
        int barWidth = 1.5;   // 非常细的线条
        int barHeight = 12;
        int barSpacing = 5;   // 增加间距

        // 左竖条
        painter.drawRect(centerX - barWidth - barSpacing/2, centerY - barHeight/2,
                        barWidth, barHeight);
        // 右竖条
        painter.drawRect(centerX + barSpacing/2, centerY - barHeight/2,
                        barWidth, barHeight);
    }
};

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

    QStackedWidget *playPauseStack;
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
    QPoint savedRelativePos;  // 保存相对于父窗口的位置
    QPointer<QWidget> anchor;

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
    // 使用 Tool 而非 Popup，允许点击穿透到下方窗口
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    // 安装事件过滤器以监听父窗口移动
    if (parent) {
        parent->installEventFilter(this);
    }

    SetupUI();
}

DebugToolbar::~DebugToolbar() = default;

void DebugToolbar::SetupUI()
{
    Q_D(DebugToolbar);

    // 加载 Fluent System Icons 字体（需要 Regular 与 Filled 以便控制线条粗细）
    const QString fallbackFamily = QStringLiteral("Segoe Fluent Icons");
    QString regularFamily;
    QString filledFamily;

    struct FontCandidate {
        QString path;
        QString *outFamily;
    };

    const FontCandidate fontSources[] = {
        {QStringLiteral(":/resources/fonts/FluentSystemIcons-Regular.ttf"), &regularFamily},
        {QStringLiteral(":/resources/fonts/FluentSystemIcons-Filled.ttf"), &filledFamily},
    };

    for (const FontCandidate &source : fontSources) {
        const int fontId = QFontDatabase::addApplicationFont(source.path);
        if (fontId != -1) {
            const QStringList families = QFontDatabase::applicationFontFamilies(fontId);
            if (!families.isEmpty()) {
                *source.outFamily = families.front();
                qDebug() << "Loaded Fluent System Icons font:" << *source.outFamily << "from" << source.path;
            }
        } else {
            qDebug() << "Failed to load Fluent System Icons font from" << source.path;
        }
    }

    const QString primaryFamily = !regularFamily.isEmpty() ? regularFamily
                               : (!filledFamily.isEmpty() ? filledFamily : fallbackFamily);
    const QString actionFamily = !regularFamily.isEmpty() ? regularFamily : primaryFamily;
    const QString arrowFamily = !filledFamily.isEmpty() ? filledFamily : primaryFamily;
    const QString dragFamily = !filledFamily.isEmpty() ? filledFamily : primaryFamily;

    if (primaryFamily == fallbackFamily) {
        qDebug() << "Falling back to Segoe Fluent Icons";
    }
    // VSCode 调试工具栏尺寸：更紧凑，预留阴影空间
    const int shadowMargin = 16; // 阴影边距（上下左右各 8px）
    setFixedHeight(35 + shadowMargin);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // VSCode 精确样式
    setStyleSheet(QString(R"(
        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 3px;
            padding: 0px;
            min-width: 28px;
            min-height: 28px;
            max-width: 28px;
            max-height: 28px;
            color: #cbd2dc;
            font-family: "%1";
        }
        QToolButton:hover {
            background-color: rgba(38, 115, 235, 0.22);
        }
        QToolButton:pressed {
            background-color: rgba(90, 93, 94, 0.5);
        }
        QToolButton:disabled {
            color: #5a5d5e;
        }
        QToolButton[toolbarColorRole="pause"] {
            color: #74bff1;
        }
        QToolButton[toolbarColorRole="pause"]:disabled {
            color: #5a5d5e;
        }
        QToolButton[toolbarColorRole="primary"] {
            color: #76b5ff;
        }
        QToolButton[toolbarColorRole="primary"]:disabled {
            color: #5a5d5e;
        }
        QToolButton[toolbarColorRole="stop"] {
            color: #ee7873;
        }
        QToolButton[toolbarColorRole="warning"] { color: #ffb23e; }
        QToolButton[toolbarColorRole="warning"]:disabled { color: #5a5d5e; }
        QToolButton[toolbarColorRole="stop"]:disabled {
            color: #5a5d5e;
        }
        QLabel {
            color: #5a5d5e;
        }
        QLabel#dragHandle {
            color: #7f7f7f;
            font-size: 18px;
            padding: 0px 6px;
        }
    )").arg(primaryFamily));

    auto *layout = new QHBoxLayout(this);
    // 设置边距以留出阴影空间
    const int shadowRadius = 8;
    layout->setContentsMargins(6 + shadowRadius, 4 + shadowRadius, 6 + shadowRadius, 4 + shadowRadius);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignVCenter);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    // 拖拽手柄（三个竖点）
    auto *dragHandle = new QLabel(QStringLiteral(u"\uE57E"), this);  // Fluent: drag_20_filled
    dragHandle->setObjectName("dragHandle");
    dragHandle->setFixedWidth(26);
    dragHandle->setAlignment(Qt::AlignCenter);
    dragHandle->setCursor(Qt::SizeAllCursor);
    QFont dragFont(dragFamily);
    dragFont.setPointSizeF(18.0);
    dragHandle->setFont(dragFont);
    layout->addWidget(dragHandle);

    // 小间距
    layout->addSpacing(2);

    const auto applyColorRole = [](QToolButton *button, const char *role) {
        if (!button || !role) {
            return;
        }
        button->setProperty("toolbarColorRole", role);
        if (button->style()) {
            button->style()->unpolish(button);
            button->style()->polish(button);
        }
    };

    const auto setIconFont = [&](QToolButton *button, const QString &family, qreal pointSize) {
        if (!button) {
            return;
        }
        QFont font(family);
        font.setPointSizeF(pointSize);
        button->setFont(font);
    };

    // 继续/暂停按钮 - 使用 QStackedWidget 保证占据相同空间
    d->playPauseStack = new QStackedWidget(this);
    d->playPauseStack->setFixedSize(28, 28);

    d->continueButton = new QToolButton(d->playPauseStack);
    d->continueButton->setObjectName(QStringLiteral("debugContinueButton"));
    d->continueButton->setAccessibleName(QStringLiteral("开始或继续调试"));
    d->continueButton->setText(QStringLiteral(u"\uF605"));  // Fluent: play_20_regular
    d->continueButton->setToolTip(QStringLiteral("继续 (F5)"));
    connect(d->continueButton, &QToolButton::clicked, this, &DebugToolbar::ContinueClicked);
    setIconFont(d->continueButton, actionFamily, 13.4);
    applyColorRole(d->continueButton, "primary");

    d->pauseButton = new PauseButton(d->playPauseStack);
    d->pauseButton->setObjectName(QStringLiteral("debugPauseButton"));
    d->pauseButton->setAccessibleName(QStringLiteral("暂停调试"));
    d->pauseButton->setToolTip(QStringLiteral("暂停 (F6)"));
    connect(d->pauseButton, &QToolButton::clicked, this, &DebugToolbar::PauseClicked);
    applyColorRole(d->pauseButton, "pause");

    d->playPauseStack->addWidget(d->continueButton);
    d->playPauseStack->addWidget(d->pauseButton);
    d->playPauseStack->setCurrentWidget(d->continueButton);

    layout->addWidget(d->playPauseStack);

    layout->addSpacing(4);

    // 单步跳过
    d->stepOverButton = new QToolButton(this);
    d->stepOverButton->setObjectName(QStringLiteral("debugStepOverButton"));
    d->stepOverButton->setAccessibleName(QStringLiteral("执行一行"));
    d->stepOverButton->setText(QStringLiteral(u"\uE10F"));  // Fluent: arrow_step_over_20_filled
    d->stepOverButton->setToolTip(QStringLiteral("单步跳过 (F10)"));
    connect(d->stepOverButton, &QToolButton::clicked, this, &DebugToolbar::StepOverClicked);
    layout->addWidget(d->stepOverButton);
    setIconFont(d->stepOverButton, arrowFamily, 14.6);
    applyColorRole(d->stepOverButton, "pause");  // 使用蓝色

    // 单步进入
    d->stepIntoButton = new QToolButton(this);
    d->stepIntoButton->setObjectName(QStringLiteral("debugStepIntoButton"));
    d->stepIntoButton->setAccessibleName(QStringLiteral("执行一个条件或动作"));
    d->stepIntoButton->setText(QStringLiteral(u"\uE0FC"));  // Fluent: arrow_step_in_20_filled
    d->stepIntoButton->setToolTip(QStringLiteral("单步进入 (F11)"));
    connect(d->stepIntoButton, &QToolButton::clicked, this, &DebugToolbar::StepIntoClicked);
    layout->addWidget(d->stepIntoButton);
    setIconFont(d->stepIntoButton, arrowFamily, 14.6);
    applyColorRole(d->stepIntoButton, "pause");  // 使用蓝色

    // 单步跳出
    d->stepOutButton = new QToolButton(this);
    d->stepOutButton->setObjectName(QStringLiteral("debugStepOutButton"));
    d->stepOutButton->setAccessibleName(QStringLiteral("执行本轮宏"));
    d->stepOutButton->setText(QStringLiteral(u"\uE10B"));  // Fluent: arrow_step_out_20_filled
    d->stepOutButton->setToolTip(QStringLiteral("单步跳出 (Shift+F11)"));
    connect(d->stepOutButton, &QToolButton::clicked, this, &DebugToolbar::StepOutClicked);
    layout->addWidget(d->stepOutButton);
    setIconFont(d->stepOutButton, arrowFamily, 14.6);
    applyColorRole(d->stepOutButton, "pause");  // 使用蓝色

    layout->addSpacing(4);

    // 重启
    d->restartButton = new QToolButton(this);
    d->restartButton->setObjectName(QStringLiteral("debugRestartButton"));
    d->restartButton->setAccessibleName(QStringLiteral("重新调试"));
    d->restartButton->setText(QStringLiteral(u"\uF13D"));  // Fluent: arrow_clockwise_20_regular
    d->restartButton->setToolTip(QStringLiteral("重启 (Ctrl+Shift+F5)"));
    connect(d->restartButton, &QToolButton::clicked, this, &DebugToolbar::RestartClicked);
    layout->addWidget(d->restartButton);
    setIconFont(d->restartButton, actionFamily, 13.4);
    applyColorRole(d->restartButton, "warning");

    layout->addSpacing(4);

    // 停止
    d->stopButton = new QToolButton(this);
    d->stopButton->setObjectName(QStringLiteral("debugStopButton"));
    d->stopButton->setAccessibleName(QStringLiteral("停止调试"));
    d->stopButton->setText(QStringLiteral(u"\uF72A"));  // Fluent: stop_20_regular
    d->stopButton->setToolTip(QStringLiteral("停止 (Shift+F5)"));
    connect(d->stopButton, &QToolButton::clicked, this, &DebugToolbar::StopClicked);
    layout->addWidget(d->stopButton);
    setIconFont(d->stopButton, actionFamily, 13.4);
    applyColorRole(d->stopButton, "stop");

    // 初始状态
    UpdateButtonStates();

    // 根据内容自适应宽度
    QSize hint = layout->sizeHint();
    setFixedWidth(hint.width());
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
    case Finished:
        // 未启动与完成后禁用单步；完成后仍可重启或停止。
        d->playPauseStack->setCurrentWidget(d->continueButton);
        d->continueButton->setEnabled(true);
        d->stepOverButton->setEnabled(false);
        d->stepIntoButton->setEnabled(false);
        d->stepOutButton->setEnabled(false);
        d->restartButton->setEnabled(d->state == Finished);
        d->stopButton->setEnabled(d->state == Finished);
        d->continueButton->setToolTip(d->state == Finished ? QStringLiteral("重新调试（F5）")
                                                         : QStringLiteral("开始调试（F5）"));
        break;

    case Running:
        // 运行状态：显示暂停按钮，禁用步进按钮
        d->playPauseStack->setCurrentWidget(d->pauseButton);
        d->pauseButton->setEnabled(true);
        d->stepOverButton->setEnabled(false);
        d->stepIntoButton->setEnabled(false);
        d->stepOutButton->setEnabled(false);
        d->restartButton->setEnabled(true);
        d->stopButton->setEnabled(true);
        break;

    case Paused:
        // 暂停状态：所有按钮可用
        d->playPauseStack->setCurrentWidget(d->continueButton);
        d->continueButton->setEnabled(true);
        d->continueButton->setToolTip(QStringLiteral("继续（F5）"));
        d->stepOverButton->setEnabled(true);
        d->stepIntoButton->setEnabled(true);
        d->stepOutButton->setEnabled(true);
        d->restartButton->setEnabled(true);
        d->stopButton->setEnabled(true);
        break;
    }
}

void DebugToolbar::SetAnchorWidget(QWidget *anchor)
{
    Q_D(DebugToolbar);
    if (d->anchor) d->anchor->removeEventFilter(this);
    d->anchor = anchor;
    d->savedRelativePos = {};
    if (anchor) anchor->installEventFilter(this);
    if (isVisible()) UpdateFloatingPosition();
}

QRect DebugToolbar::FloatingBounds() const
{
    Q_D(const DebugToolbar);
    auto *anchor = d->anchor ? d->anchor.data() : parentWidget();
    return anchor ? QRect(anchor->mapToGlobal(QPoint(0, 0)), anchor->size()) : geometry();
}

void DebugToolbar::UpdateFloatingPosition()
{
    Q_D(DebugToolbar);
    const auto bounds = FloatingBounds();
    auto position = bounds.topLeft() + QPoint(qMax(0, bounds.width() - width() - 8), 8);
    if (parentWidget() && !d->savedRelativePos.isNull()) position = parentWidget()->mapToGlobal(d->savedRelativePos);
    position.setX(qBound(bounds.left(), position.x(), qMax(bounds.left(), bounds.right() - width() + 1)));
    position.setY(qBound(bounds.top(), position.y(), qMax(bounds.top(), bounds.bottom() - height() + 1)));
    move(position);
    if (parentWidget()) d->savedRelativePos = parentWidget()->mapFromGlobal(position);
}

void DebugToolbar::ShowFloating()
{
    UpdateFloatingPosition();
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

    // 先清空整个区域为透明
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect(), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    const int shadowRadius = 8;
    const int borderRadius = 8;

    // 计算内容区域（去除阴影边距）
    QRect contentRect = rect().adjusted(shadowRadius, shadowRadius, -shadowRadius, -shadowRadius);

    // 绘制阴影 - 使用裁剪方式，只绘制外部阴影
    for (int i = 1; i <= shadowRadius; ++i) {
        // 计算当前层的透明度（离内容越远越淡）
        // 使用指数衰减让内圈更深，外圈更淡
        qreal progress = qreal(i) / shadowRadius;
        int alpha = int(42 * qPow(1.0 - progress, 2.5));

        // 外层矩形
        QRectF outerRect = contentRect.adjusted(-i, -i, i, i);
        // 内层矩形（用于裁剪）
        QRectF innerRect = contentRect.adjusted(-(i-1), -(i-1), (i-1), (i-1));

        // 创建外部路径和内部路径
        QPainterPath outerPath;
        outerPath.addRoundedRect(outerRect, borderRadius, borderRadius);

        QPainterPath innerPath;
        innerPath.addRoundedRect(innerRect, borderRadius, borderRadius);

        // 使用减法得到环形区域（只有边缘阴影）
        QPainterPath shadowPath = outerPath.subtracted(innerPath);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, alpha));
        painter.drawPath(shadowPath);
    }

    // 绘制主体背景
    QPainterPath mainPath;
    mainPath.addRoundedRect(contentRect, borderRadius, borderRadius);

    painter.fillPath(mainPath, QColor("#232327"));

    // 绘制边框
    painter.setPen(QPen(QColor("#333339"), 1));
    painter.drawPath(mainPath);
}

void DebugToolbar::mousePressEvent(QMouseEvent *event)
{
    Q_D(DebugToolbar);
    if (event->button() == Qt::LeftButton) {
        // 检查点击位置是否在内容区域内（排除阴影区域）
        const int shadowRadius = 8;
        QRect contentRect = rect().adjusted(shadowRadius, shadowRadius, -shadowRadius, -shadowRadius);

        if (contentRect.contains(event->pos())) {
            d->dragging = true;
            d->dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        } else {
            event->ignore();  // 点击阴影区域，忽略事件
        }
    }
}

void DebugToolbar::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(DebugToolbar);
    if (d->dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPosition().toPoint() - d->dragStartPos;

        if (parentWidget()) {
            const auto bounds = FloatingBounds();
            newPos.setX(qBound(bounds.left(), newPos.x(), qMax(bounds.left(), bounds.right() - width() + 1)));
            newPos.setY(qBound(bounds.top(), newPos.y(), qMax(bounds.top(), bounds.bottom() - height() + 1)));
            d->savedRelativePos = parentWidget()->mapFromGlobal(newPos);
        }

        move(newPos);
        event->accept();
    }
}

void DebugToolbar::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(DebugToolbar);
    if (event->button() == Qt::LeftButton) {
        d->dragging = false;
        event->accept();
    }
}

bool DebugToolbar::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(DebugToolbar);

    if ((watched == parentWidget() || watched == d->anchor) && isVisible() &&
        (event->type() == QEvent::Move || event->type() == QEvent::Resize)) UpdateFloatingPosition();

    return QWidget::eventFilter(watched, event);
}
