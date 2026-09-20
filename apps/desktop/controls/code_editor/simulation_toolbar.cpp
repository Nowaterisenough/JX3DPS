#include "simulation_toolbar.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

class SimulationToolbarPrivate
{
public:
    explicit SimulationToolbarPrivate(SimulationToolbar *q) :
        debugModeEnabled(false), startSimulationButton(nullptr), debugModeButton(nullptr), q_ptr(q)
    {
    }

    bool         debugModeEnabled;
    QToolButton *startSimulationButton;
    QToolButton *debugModeButton;

private:
    SimulationToolbar *q_ptr;
    Q_DECLARE_PUBLIC(SimulationToolbar)
};

SimulationToolbar::SimulationToolbar(QWidget *parent) : QWidget(parent), d_ptr(new SimulationToolbarPrivate(this))
{
    SetupUI();
}

SimulationToolbar::~SimulationToolbar() = default;

void SimulationToolbar::SetupUI()
{
    Q_D(SimulationToolbar);

    // 样式
    setStyleSheet(R"(
        SimulationToolbar {
            background-color: transparent;
        }
        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 3px;
            padding: 7px 9px;
            color: #bbbbbb;
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
            color: #132820;
            background-color: #91c6af;
            font-weight: bold;
        }
        QToolButton#debugButton {
            color: #bbbbbb;
        }
        QToolButton#debugButton:checked {
            color: #75beff;
            background-color: rgba(117, 190, 255, 0.15);
        }
        QLabel {
            color: #bbbbbb;
            font-size: 12px;
            padding: 0px 8px;
        }
    )");

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
    d->debugModeButton->setToolTip("打开宏调试页面");
    d->debugModeButton->setCheckable(true);
    d->debugModeButton->setChecked(false);
    connect(d->debugModeButton, &QToolButton::toggled, [this, d](bool checked) {
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
