#include "frameless.h"

#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include <QWKWidgets/widgetwindowagent.h>

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

void Frameless::SetupTitleBar(TitleBarSetupFunction setupFunc)
{
    if (!setupFunc) {
        return;
    }

    // 如果已经有 titlebar，先移除
    if (_titleBar) {
        auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
        if (layout) {
            layout->removeWidget(_titleBar);
            _titleBar->deleteLater();
            _titleBar = nullptr;
        }
    }

    // 调用自定义的 setup 函数创建 titlebar
    _titleBar = setupFunc(this, _windowAgent);

    if (_titleBar) {
        // 设置到 windowAgent
        _windowAgent->setTitleBar(_titleBar);

        // 添加到布局
        auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
        if (layout) {
            layout->setMenuBar(_titleBar);
        }
    }
}

void Frameless::SetupDefaultTitleBar()
{
    SetupTitleBar([](Frameless *frameless, QWK::WidgetWindowAgent *agent) -> QWidget* {
        constexpr int BUTTON_WIDTH = 45;
        constexpr int HEIGHT       = 35;

        auto *windowBar = new TitleBar(frameless);
        windowBar->SetHostWidget(frameless);
        windowBar->setFixedHeight(HEIGHT);

        auto *icon = new QLabel(windowBar);
        icon->setFixedHeight(HEIGHT);
        icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

        auto *title = new QLabel(windowBar);
        title->setFixedHeight(HEIGHT);
        title->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

        auto *minimizeButton = new TitleBarButton(TitleBarButton::ButtonType::MINIMIZE, windowBar);
        minimizeButton->setFixedSize(BUTTON_WIDTH, HEIGHT);
        minimizeButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        QObject::connect(minimizeButton, &QPushButton::clicked, frameless, &Frameless::showMinimized);

        auto *maximizeButton = new TitleBarButton(TitleBarButton::ButtonType::MAXIMIZE, windowBar);
        maximizeButton->setFixedSize(BUTTON_WIDTH, HEIGHT);
        maximizeButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        QObject::connect(maximizeButton, &QPushButton::clicked, frameless, [frameless] {
            if (frameless->isMaximized()) {
                frameless->showNormal();
            } else {
                frameless->showMaximized();
            }
        });

        auto *closeButton = new TitleBarButton(TitleBarButton::ButtonType::CLOSE, windowBar);
        closeButton->setFixedSize(BUTTON_WIDTH, HEIGHT);
        closeButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        QObject::connect(closeButton, &QPushButton::clicked, frameless, &Frameless::close);

        auto *spacer = new QWidget(windowBar);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        windowBar->AddWidget(TitleBar::WidgetType::ICON, icon, 0, Qt::AlignCenter);
        windowBar->AddWidget(TitleBar::WidgetType::TITLE, title, 0, Qt::AlignCenter);
        windowBar->AddWidget(TitleBar::WidgetType::CUSTOM, spacer, 1, Qt::AlignCenter);
        windowBar->AddWidget(TitleBar::WidgetType::MIN_BUTTON, minimizeButton, 0, Qt::AlignCenter);
        windowBar->AddWidget(TitleBar::WidgetType::MAX_BUTTON, maximizeButton, 0, Qt::AlignCenter);
        windowBar->AddWidget(TitleBar::WidgetType::CLOSE_BUTTON, closeButton, 0, Qt::AlignCenter);

        windowBar->UpdateLayout({
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
        agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::WindowIcon, icon);
        agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Minimize, minimizeButton);
        agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Maximize, maximizeButton);
        agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Close, closeButton);

        return windowBar;
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