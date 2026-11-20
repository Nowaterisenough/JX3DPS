#include "simulation_toolbar.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

#include "../theme/accent_color.h"

class SimulationToolbarPrivate
{
public:
    explicit SimulationToolbarPrivate(SimulationToolbar *q) : q_ptr(q) { }

    bool         debugModeEnabled{ false };
    QToolButton *startSimulationButton{ nullptr };
    QToolButton *debugModeButton{ nullptr };

private:
    SimulationToolbar *q_ptr;
    Q_DECLARE_PUBLIC(SimulationToolbar)
};

SimulationToolbar::SimulationToolbar(QWidget *parent) : QWidget(parent), d_ptr(new SimulationToolbarPrivate(this))
{
    SetupUI();
}

SimulationToolbar::~SimulationToolbar() = default;

void SimulationToolbar::SetAccentColor(const QColor &accent)
{
    const auto COLORS = desktop::theme::MakeAccentColors(accent);
    setStyleSheet(QStringLiteral(R"(
        SimulationToolbar {
            background-color: transparent;
        }
        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 6px;
            padding: 7px 9px;
            color: #d4dceb;
            font-size: 13px;
        }
        QToolButton:hover {
            background-color: rgba(90, 93, 94, 0.31);
        }
        QToolButton:pressed {
            background-color: rgba(90, 93, 94, 0.5);
        }
        QToolButton:disabled {
            color: #5a5d5e;
        }
        QToolButton#startButton {
            border-radius: 8px;
            color: $onAccent;
            background-color: $accent;
            font-weight: 600;
        }
        QToolButton#startButton:hover { background-color: $accentHover; }
        QToolButton#startButton:pressed { background-color: $accentPressed; }
        QToolButton#startButton:disabled { background-color: #2d2d33; color: #72727d; }
        QToolButton#debugButton {
            color: #bbbbbb;
        }
        QToolButton#debugButton:checked {
            color: #ededf1;
            background-color: #2d2d33;
        }
        QLabel {
            color: #bbbbbb;
            font-size: 12px;
            padding: 0px 8px;
        }
    )")
                      .replace("$accentHover", COLORS.hover.name())
                      .replace("$accentPressed", COLORS.pressed.name())
                      .replace("$onAccent", COLORS.text.name())
                      .replace("$accent", COLORS.normal.name()));
}

void SimulationToolbar::SetupUI()
{
    Q_D(SimulationToolbar);
    SetAccentColor(desktop::theme::AccentColor(this));

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    // 开始模拟按钮
    d->startSimulationButton = new QToolButton(this);
    d->startSimulationButton->setText("开始模拟");
    d->startSimulationButton->setObjectName("startButton");
    d->startSimulationButton->setToolTip("开始模拟 (F5)");
    connect(d->startSimulationButton, &QToolButton::clicked, this, &SimulationToolbar::StartSimulationClicked);
    d->startSimulationButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(d->startSimulationButton, 1);

    // 调试模式切换按钮
    d->debugModeButton = new QToolButton(this);
    d->debugModeButton->setText("断点调试");
    d->debugModeButton->setObjectName("debugButton");
    d->debugModeButton->setToolTip("在循环宏中显示浮动调试工具条和状态；关闭时停止调试");
    d->debugModeButton->setCheckable(true);
    d->debugModeButton->setChecked(false);
    connect(d->debugModeButton, &QToolButton::toggled, this, [this, d](bool checked) {
        d->debugModeEnabled = checked;
        d->startSimulationButton->setText(checked ? QStringLiteral("开始调试") : QStringLiteral("开始模拟"));
        d->startSimulationButton->setToolTip(checked ? QStringLiteral("启动新的调试会话，在首行暂停") : QStringLiteral("开始批量模拟（F5）"));
        emit DebugModeToggled(checked);
        qDebug() << "[SimulationToolbar] 调试模式:" << (checked ? "开启" : "关闭");
    });
    layout->addWidget(d->debugModeButton);

    setFixedHeight(36);
}

bool SimulationToolbar::IsDebugModeEnabled() const
{
    Q_D(const SimulationToolbar);
    return d->debugModeEnabled;
}
