#include "frameless.h"

#include <QLabel>
#include <QLayout>
#include <QPushButton>

#if __has_include(<QWKWidgets/widgetwindowagent.h>)
#    include <QWKWidgets/widgetwindowagent.h>
#endif

#include "title_bar.h"
#include "title_bar_button.h"

Frameless::Frameless(QWidget *parent) : QWidget(parent), _centralWidget(new QWidget(this))
{
    InstallWindowAgent();
    SetupUI();
}

QWidget *Frameless::ContentWidget()
{
    return _centralWidget;
}

void Frameless::AddIgnoreWidget(QWidget *widget)
{
    _windowAgent->setHitTestVisible(widget);
}

void Frameless::SetupTitleBar(TitleBarSetupFunction setup_func)
{
    if (!setup_func) {
        return;
    }

    // 如果已经有 titlebar，先移除
    if (_titleBar != nullptr) {
        auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
        if (layout != nullptr) {
            layout->removeWidget(_titleBar);
            _titleBar->deleteLater();
            _titleBar = nullptr;
        }
    }

    // 调用自定义的 setup 函数创建 titlebar
    _titleBar = setup_func(this, _windowAgent);

    if (_titleBar != nullptr) {
        // 设置到 windowAgent
        _windowAgent->setTitleBar(_titleBar);

        // 添加到布局
        auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
        if (layout != nullptr) {
            layout->setMenuBar(_titleBar);
        }
    }
}

void Frameless::SetupDefaultTitleBar()
{
    SetupTitleBar([](Frameless *frameless, QWK::WidgetWindowAgent *agent) -> QWidget * {
        constexpr int BUTTON_WIDTH = 46;
        constexpr int HEIGHT       = 30;

        auto *window_bar = new TitleBar(frameless);
        window_bar->SetHostWidget(frameless);
        window_bar->setFixedHeight(HEIGHT);
        window_bar->setObjectName(QStringLiteral("windowTitleBar"));
        window_bar->setStyleSheet(QStringLiteral(
            "QFrame#windowTitleBar { background: #1a1a1e; border: 0; } QLabel { color: #ededf1; font-size: 12px; }"));

        auto *icon = new QLabel(window_bar);
        icon->setFixedHeight(HEIGHT);
        icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

        auto *title = new QLabel(window_bar);
        title->setFixedHeight(HEIGHT);
        title->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

        auto *minimize_button = new TitleBarButton(TitleBarButton::ButtonType::MINIMIZE, window_bar);
        minimize_button->setFixedSize(BUTTON_WIDTH, HEIGHT);
        minimize_button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        QObject::connect(minimize_button, &QPushButton::clicked, frameless, &Frameless::showMinimized);

        auto *maximize_button = new TitleBarButton(TitleBarButton::ButtonType::MAXIMIZE, window_bar);
        maximize_button->setFixedSize(BUTTON_WIDTH, HEIGHT);
        maximize_button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        QObject::connect(maximize_button, &QPushButton::clicked, frameless, [frameless] {
            if (frameless->isMaximized()) {
                frameless->showNormal();
            } else {
                frameless->showMaximized();
            }
        });

        auto *close_button = new TitleBarButton(TitleBarButton::ButtonType::CLOSE, window_bar);
        close_button->setFixedSize(BUTTON_WIDTH, HEIGHT);
        close_button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        QObject::connect(close_button, &QPushButton::clicked, frameless, &Frameless::close);

        auto *spacer = new QWidget(window_bar);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        window_bar->AddWidget(TitleBar::WidgetType::ICON, icon, 0, Qt::AlignCenter);
        window_bar->AddWidget(TitleBar::WidgetType::TITLE, title, 0, Qt::AlignCenter);
        window_bar->AddWidget(TitleBar::WidgetType::CUSTOM, spacer, 1, Qt::AlignCenter);
        window_bar->AddWidget(TitleBar::WidgetType::MIN_BUTTON, minimize_button, 0, Qt::AlignCenter);
        window_bar->AddWidget(TitleBar::WidgetType::MAX_BUTTON, maximize_button, 0, Qt::AlignCenter);
        window_bar->AddWidget(TitleBar::WidgetType::CLOSE_BUTTON, close_button, 0, Qt::AlignCenter);

        window_bar->UpdateLayout({
            TitleBar::WidgetType::SPACER_ITEM,
            TitleBar::WidgetType::ICON,
            TitleBar::WidgetType::SPACER_ITEM,
            TitleBar::WidgetType::SPACER_ITEM,
            TitleBar::WidgetType::TITLE,
            TitleBar::WidgetType::CUSTOM,
            TitleBar::WidgetType::SPACER_ITEM,
            TitleBar::WidgetType::MIN_BUTTON,
            TitleBar::WidgetType::MAX_BUTTON,
            TitleBar::WidgetType::CLOSE_BUTTON,
        });

        // 设置系统按钮
#if __has_include(<QWKWidgets/widgetwindowagent.h>)
        agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::WindowIcon, icon);
        agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Minimize, minimize_button);
        agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Maximize, maximize_button);
        agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Close, close_button);
#endif

        return window_bar;
    });
}

void Frameless::InstallWindowAgent()
{
    _windowAgent = new QWK::WidgetWindowAgent(this);
    _windowAgent->setup(this);
}

void Frameless::SetupUI()
{
    // 创建主布局
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(_centralWidget);
}

bool Frameless::eventFilter(QObject *watched, QEvent *event)
{
    return QWidget::eventFilter(watched, event);
}
