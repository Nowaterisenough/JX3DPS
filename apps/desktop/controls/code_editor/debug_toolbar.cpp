#include "debug_toolbar.h"

#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPointer>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QStringList>
#include <QStyle>
#include <QToolButton>

// 自定义暂停按钮 - 绘制两个实心竖条
class PauseButton : public QToolButton
{
public:
    explicit PauseButton(QWidget *parent = nullptr) : QToolButton(parent) { }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QToolButton::paintEvent(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 获取按钮颜色
        QColor color = property("toolbarColorRole").toString() == "pause" ? QColor(116, 191, 241) : QColor(187, 187, 187); // #bbbbbb

        if (!isEnabled()) {
            color = QColor(90, 93, 94); // #5a5d5e
        }

        // 在按钮中心绘制两个竖条
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);

        const int     CENTER_X    = width() / 2;
        const int     CENTER_Y    = height() / 2;
        constexpr int BAR_WIDTH   = 1; // 非常细的线条
        constexpr int BAR_HEIGHT  = 12;
        constexpr int BAR_SPACING = 5; // 增加间距

        // 左竖条
        painter.drawRect(CENTER_X - BAR_WIDTH - (BAR_SPACING / 2), CENTER_Y - (BAR_HEIGHT / 2), BAR_WIDTH, BAR_HEIGHT);
        // 右竖条
        painter.drawRect(CENTER_X + (BAR_SPACING / 2), CENTER_Y - (BAR_HEIGHT / 2), BAR_WIDTH, BAR_HEIGHT);
    }
};

class DebugToolbarPrivate
{
public:
    explicit DebugToolbarPrivate(DebugToolbar *q) : dragStartPos(), q_ptr(q) { }

    DebugToolbar::DebugState state{ DebugToolbar::STOPPED };

    QStackedWidget *playPauseStack{};
    QToolButton    *continueButton{ nullptr };
    QToolButton    *pauseButton{ nullptr };
    QToolButton    *stepOverButton{ nullptr };
    QToolButton    *stepIntoButton{ nullptr };
    QToolButton    *stepOutButton{ nullptr };
    QToolButton    *restartButton{ nullptr };
    QToolButton    *stopButton{ nullptr };

    // 拖动相关
    bool              dragging{ false };
    QPoint            dragStartPos;
    QPoint            savedRelativePos; // 保存相对于父窗口的位置
    QPointer<QWidget> anchor;

private:
    DebugToolbar *q_ptr;
    Q_DECLARE_PUBLIC(DebugToolbar)
};

// ============================================================================
// DebugToolbar Implementation
// ============================================================================

DebugToolbar::DebugToolbar(QWidget *parent) : QWidget(parent), d_ptr(new DebugToolbarPrivate(this))
{
    // 设置为浮窗
    // 使用 Tool 而非 Popup，允许点击穿透到下方窗口
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    // 安装事件过滤器以监听父窗口移动
    if (parent != nullptr) {
        parent->installEventFilter(this);
    }

    SetupUI();
}

DebugToolbar::~DebugToolbar() = default;

void DebugToolbar::SetupUI()
{
    Q_D(DebugToolbar);

    // 加载 Fluent System Icons 字体（需要 Regular 与 Filled 以便控制线条粗细）
    const QString FALLBACK_FAMILY = QStringLiteral("Segoe Fluent Icons");
    QString       regular_family;
    QString       filled_family;

    struct FontCandidate
    {
        QString  path;
        QString *outFamily;
    };

    const FontCandidate FONT_SOURCES[] = {
        { QStringLiteral(":/resources/fonts/FluentSystemIcons-Regular.ttf"), &regular_family },
        { QStringLiteral(":/resources/fonts/FluentSystemIcons-Filled.ttf"),  &filled_family  },
    };

    for (const FontCandidate &source : FONT_SOURCES) {
        const int FONT_ID = QFontDatabase::addApplicationFont(source.path);
        if (FONT_ID != -1) {
            const QStringList FAMILIES = QFontDatabase::applicationFontFamilies(FONT_ID);
            if (!FAMILIES.isEmpty()) {
                *source.outFamily = FAMILIES.front();
                qDebug() << "Loaded Fluent System Icons font:" << *source.outFamily << "from" << source.path;
            }
        } else {
            qDebug() << "Failed to load Fluent System Icons font from" << source.path;
        }
    }

    const QString PRIMARY_FAMILY =
        !regular_family.isEmpty() ? regular_family : (!filled_family.isEmpty() ? filled_family : FALLBACK_FAMILY);
    const QString ACTION_FAMILY = !regular_family.isEmpty() ? regular_family : PRIMARY_FAMILY;
    const QString ARROW_FAMILY  = !filled_family.isEmpty() ? filled_family : PRIMARY_FAMILY;
    const QString DRAG_FAMILY   = !filled_family.isEmpty() ? filled_family : PRIMARY_FAMILY;

    if (PRIMARY_FAMILY == FALLBACK_FAMILY) {
        qDebug() << "Falling back to Segoe Fluent Icons";
    }
    // VSCode 调试工具栏尺寸：更紧凑，预留阴影空间
    const int SHADOW_MARGIN = 16; // 阴影边距（上下左右各 8px）
    setFixedHeight(35 + SHADOW_MARGIN);
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
    )")
                      .arg(PRIMARY_FAMILY));

    auto     *layout        = new QHBoxLayout(this);
    // 设置边距以留出阴影空间
    const int SHADOW_RADIUS = 8;
    layout->setContentsMargins(6 + SHADOW_RADIUS, 4 + SHADOW_RADIUS, 6 + SHADOW_RADIUS, 4 + SHADOW_RADIUS);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignVCenter);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    // 拖拽手柄（三个竖点）
    auto *drag_handle = new QLabel(QStringLiteral(u"\uE57E"), this); // Fluent: drag_20_filled
    drag_handle->setObjectName("dragHandle");
    drag_handle->setFixedWidth(26);
    drag_handle->setAlignment(Qt::AlignCenter);
    drag_handle->setCursor(Qt::SizeAllCursor);
    QFont drag_font(DRAG_FAMILY);
    drag_font.setPointSizeF(18.0);
    drag_handle->setFont(drag_font);
    layout->addWidget(drag_handle);

    // 小间距
    layout->addSpacing(2);

    const auto APPLY_COLOR_ROLE = [](QToolButton *button, const char *role) {
        if (!button || !role) {
            return;
        }
        button->setProperty("toolbarColorRole", role);
        if (button->style()) {
            button->style()->unpolish(button);
            button->style()->polish(button);
        }
    };

    const auto SET_ICON_FONT = [&](QToolButton *button, const QString &family, qreal point_size) {
        if (!button) {
            return;
        }
        QFont font(family);
        font.setPointSizeF(point_size);
        button->setFont(font);
    };

    // 继续/暂停按钮 - 使用 QStackedWidget 保证占据相同空间
    d->playPauseStack = new QStackedWidget(this);
    d->playPauseStack->setFixedSize(28, 28);

    d->continueButton = new QToolButton(d->playPauseStack);
    d->continueButton->setObjectName(QStringLiteral("debugContinueButton"));
    d->continueButton->setAccessibleName(QStringLiteral("开始或继续调试"));
    d->continueButton->setText(QStringLiteral(u"\uF605")); // Fluent: play_20_regular
    d->continueButton->setToolTip(QStringLiteral("继续 (F5)"));
    connect(d->continueButton, &QToolButton::clicked, this, &DebugToolbar::ContinueClicked);
    SET_ICON_FONT(d->continueButton, ACTION_FAMILY, 13.4);
    APPLY_COLOR_ROLE(d->continueButton, "primary");

    d->pauseButton = new PauseButton(d->playPauseStack);
    d->pauseButton->setObjectName(QStringLiteral("debugPauseButton"));
    d->pauseButton->setAccessibleName(QStringLiteral("暂停调试"));
    d->pauseButton->setToolTip(QStringLiteral("暂停 (F6)"));
    connect(d->pauseButton, &QToolButton::clicked, this, &DebugToolbar::PauseClicked);
    APPLY_COLOR_ROLE(d->pauseButton, "pause");

    d->playPauseStack->addWidget(d->continueButton);
    d->playPauseStack->addWidget(d->pauseButton);
    d->playPauseStack->setCurrentWidget(d->continueButton);

    layout->addWidget(d->playPauseStack);

    layout->addSpacing(4);

    // 单步跳过
    d->stepOverButton = new QToolButton(this);
    d->stepOverButton->setObjectName(QStringLiteral("debugStepOverButton"));
    d->stepOverButton->setAccessibleName(QStringLiteral("执行一行"));
    d->stepOverButton->setText(QStringLiteral(u"\uE10F")); // Fluent: arrow_step_over_20_filled
    d->stepOverButton->setToolTip(QStringLiteral("单步跳过 (F10)"));
    connect(d->stepOverButton, &QToolButton::clicked, this, &DebugToolbar::StepOverClicked);
    layout->addWidget(d->stepOverButton);
    SET_ICON_FONT(d->stepOverButton, ARROW_FAMILY, 14.6);
    APPLY_COLOR_ROLE(d->stepOverButton, "pause"); // 使用蓝色

    // 单步进入
    d->stepIntoButton = new QToolButton(this);
    d->stepIntoButton->setObjectName(QStringLiteral("debugStepIntoButton"));
    d->stepIntoButton->setAccessibleName(QStringLiteral("执行一个条件或动作"));
    d->stepIntoButton->setText(QStringLiteral(u"\uE0FC")); // Fluent: arrow_step_in_20_filled
    d->stepIntoButton->setToolTip(QStringLiteral("单步进入 (F11)"));
    connect(d->stepIntoButton, &QToolButton::clicked, this, &DebugToolbar::StepIntoClicked);
    layout->addWidget(d->stepIntoButton);
    SET_ICON_FONT(d->stepIntoButton, ARROW_FAMILY, 14.6);
    APPLY_COLOR_ROLE(d->stepIntoButton, "pause"); // 使用蓝色

    // 单步跳出
    d->stepOutButton = new QToolButton(this);
    d->stepOutButton->setObjectName(QStringLiteral("debugStepOutButton"));
    d->stepOutButton->setAccessibleName(QStringLiteral("执行本轮宏"));
    d->stepOutButton->setText(QStringLiteral(u"\uE10B")); // Fluent: arrow_step_out_20_filled
    d->stepOutButton->setToolTip(QStringLiteral("单步跳出 (Shift+F11)"));
    connect(d->stepOutButton, &QToolButton::clicked, this, &DebugToolbar::StepOutClicked);
    layout->addWidget(d->stepOutButton);
    SET_ICON_FONT(d->stepOutButton, ARROW_FAMILY, 14.6);
    APPLY_COLOR_ROLE(d->stepOutButton, "pause"); // 使用蓝色

    layout->addSpacing(4);

    // 重启
    d->restartButton = new QToolButton(this);
    d->restartButton->setObjectName(QStringLiteral("debugRestartButton"));
    d->restartButton->setAccessibleName(QStringLiteral("重新调试"));
    d->restartButton->setText(QStringLiteral(u"\uF13D")); // Fluent: arrow_clockwise_20_regular
    d->restartButton->setToolTip(QStringLiteral("重启 (Ctrl+Shift+F5)"));
    connect(d->restartButton, &QToolButton::clicked, this, &DebugToolbar::RestartClicked);
    layout->addWidget(d->restartButton);
    SET_ICON_FONT(d->restartButton, ACTION_FAMILY, 13.4);
    APPLY_COLOR_ROLE(d->restartButton, "warning");

    layout->addSpacing(4);

    // 停止
    d->stopButton = new QToolButton(this);
    d->stopButton->setObjectName(QStringLiteral("debugStopButton"));
    d->stopButton->setAccessibleName(QStringLiteral("停止调试"));
    d->stopButton->setText(QStringLiteral(u"\uF72A")); // Fluent: stop_20_regular
    d->stopButton->setToolTip(QStringLiteral("停止 (Shift+F5)"));
    connect(d->stopButton, &QToolButton::clicked, this, &DebugToolbar::StopClicked);
    layout->addWidget(d->stopButton);
    SET_ICON_FONT(d->stopButton, ACTION_FAMILY, 13.4);
    APPLY_COLOR_ROLE(d->stopButton, "stop");

    // 初始状态
    UpdateButtonStates();

    // 根据内容自适应宽度
    const QSize hint = layout->sizeHint();
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
        case STOPPED:
        case FINISHED:
            // 未启动与完成后禁用单步；完成后仍可重启或停止。
            d->playPauseStack->setCurrentWidget(d->continueButton);
            d->continueButton->setEnabled(true);
            d->stepOverButton->setEnabled(false);
            d->stepIntoButton->setEnabled(false);
            d->stepOutButton->setEnabled(false);
            d->restartButton->setEnabled(d->state == FINISHED);
            d->stopButton->setEnabled(d->state == FINISHED);
            d->continueButton->setToolTip(d->state == FINISHED ? QStringLiteral("重新调试（F5）") : QStringLiteral("开始调试（F5）"));
            break;

        case RUNNING:
            // 运行状态：显示暂停按钮，禁用步进按钮
            d->playPauseStack->setCurrentWidget(d->pauseButton);
            d->pauseButton->setEnabled(true);
            d->stepOverButton->setEnabled(false);
            d->stepIntoButton->setEnabled(false);
            d->stepOutButton->setEnabled(false);
            d->restartButton->setEnabled(true);
            d->stopButton->setEnabled(true);
            break;

        case PAUSED:
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
    if (d->anchor) {
        d->anchor->removeEventFilter(this);
    }
    d->anchor           = anchor;
    d->savedRelativePos = {};
    if (anchor != nullptr) {
        anchor->installEventFilter(this);
    }
    if (isVisible()) {
        UpdateFloatingPosition();
    }
}

QRect DebugToolbar::FloatingBounds() const
{
    Q_D(const DebugToolbar);
    const auto *anchor = d->anchor ? d->anchor.data() : parentWidget();
    return (anchor != nullptr) ? QRect(anchor->mapToGlobal(QPoint(0, 0)), anchor->size()) : geometry();
}

void DebugToolbar::UpdateFloatingPosition()
{
    Q_D(DebugToolbar);
    const auto BOUNDS   = FloatingBounds();
    auto       position = BOUNDS.topLeft() + QPoint(qMax(0, BOUNDS.width() - width() - 8), 8);
    if ((parentWidget() != nullptr) && !d->savedRelativePos.isNull()) {
        position = parentWidget()->mapToGlobal(d->savedRelativePos);
    }
    position.setX(qBound(BOUNDS.left(), position.x(), qMax(BOUNDS.left(), BOUNDS.right() - width() + 1)));
    position.setY(qBound(BOUNDS.top(), position.y(), qMax(BOUNDS.top(), BOUNDS.bottom() - height() + 1)));
    move(position);
    if (parentWidget() != nullptr) {
        d->savedRelativePos = parentWidget()->mapFromGlobal(position);
    }
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

    const int SHADOW_RADIUS = 8;
    const int BORDER_RADIUS = 8;

    // 计算内容区域（去除阴影边距）
    const QRect content_rect = rect().adjusted(SHADOW_RADIUS, SHADOW_RADIUS, -SHADOW_RADIUS, -SHADOW_RADIUS);

    // 绘制阴影 - 使用裁剪方式，只绘制外部阴影
    for (int i = 1; i <= SHADOW_RADIUS; ++i) {
        // 计算当前层的透明度（离内容越远越淡）
        // 使用指数衰减让内圈更深，外圈更淡
        const qreal PROGRESS = qreal(i) / SHADOW_RADIUS;
        const int   ALPHA    = int(42 * qPow(1.0 - PROGRESS, 2.5));

        // 外层矩形
        const QRectF outer_rect = content_rect.adjusted(-i, -i, i, i);
        // 内层矩形（用于裁剪）
        const QRectF inner_rect = content_rect.adjusted(-(i - 1), -(i - 1), (i - 1), (i - 1));

        // 创建外部路径和内部路径
        QPainterPath outer_path;
        outer_path.addRoundedRect(outer_rect, BORDER_RADIUS, BORDER_RADIUS);

        QPainterPath inner_path;
        inner_path.addRoundedRect(inner_rect, BORDER_RADIUS, BORDER_RADIUS);

        // 使用减法得到环形区域（只有边缘阴影）
        const QPainterPath SHADOW_PATH = outer_path.subtracted(inner_path);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, ALPHA));
        painter.drawPath(SHADOW_PATH);
    }

    // 绘制主体背景
    QPainterPath main_path;
    main_path.addRoundedRect(content_rect, BORDER_RADIUS, BORDER_RADIUS);

    painter.fillPath(main_path, QColor("#232327"));

    // 绘制边框
    painter.setPen(QPen(QColor("#333339"), 1));
    painter.drawPath(main_path);
}

void DebugToolbar::mousePressEvent(QMouseEvent *event)
{
    Q_D(DebugToolbar);
    if (event->button() == Qt::LeftButton) {
        // 检查点击位置是否在内容区域内（排除阴影区域）
        const int   SHADOW_RADIUS = 8;
        const QRect content_rect  = rect().adjusted(SHADOW_RADIUS, SHADOW_RADIUS, -SHADOW_RADIUS, -SHADOW_RADIUS);

        if (content_rect.contains(event->pos())) {
            d->dragging     = true;
            d->dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        } else {
            event->ignore(); // 点击阴影区域，忽略事件
        }
    }
}

void DebugToolbar::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(DebugToolbar);
    if (d->dragging && ((event->buttons() & Qt::LeftButton) != 0u)) {
        QPoint new_pos = event->globalPosition().toPoint() - d->dragStartPos;

        if (parentWidget() != nullptr) {
            const auto BOUNDS = FloatingBounds();
            new_pos.setX(qBound(BOUNDS.left(), new_pos.x(), qMax(BOUNDS.left(), BOUNDS.right() - width() + 1)));
            new_pos.setY(qBound(BOUNDS.top(), new_pos.y(), qMax(BOUNDS.top(), BOUNDS.bottom() - height() + 1)));
            d->savedRelativePos = parentWidget()->mapFromGlobal(new_pos);
        }

        move(new_pos);
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
        (event->type() == QEvent::Move || event->type() == QEvent::Resize))
    {
        UpdateFloatingPosition();
    }

    return QWidget::eventFilter(watched, event);
}
