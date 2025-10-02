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

void Frameless::AddTabBar(QWidget *widget, int stretch)
{
    auto *windowBar = qobject_cast<TitleBar *>(_windowAgent->titleBar());
    if (windowBar) {
        windowBar->AddWidget(TitleBar::WidgetType::CUSTOM, widget, stretch, Qt::AlignCenter);
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
    }
}

void Frameless::InstallWindowAgent()
{
    _windowAgent = new QWK::WidgetWindowAgent(this);
    _windowAgent->setup(this);
}

void Frameless::SetupUI()
{
    InitTitleBar();
}

void Frameless::InitTitleBar()
{
    constexpr int BUTTON_WIDTH = 45;
    constexpr int HEIGHT       = 35;

    auto *windowBar = new TitleBar(this);
    windowBar->SetHostWidget(this);
    windowBar->setFixedHeight(HEIGHT);

    auto *icon = new QLabel(windowBar);
    icon->setFixedHeight(HEIGHT);
    icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    auto *title = new QLabel(windowBar);
    title->setFixedHeight(HEIGHT);
    title->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    _minimizeButton = new TitleBarButton(TitleBarButton::ButtonType::MINIMIZE, windowBar);
    _minimizeButton->setFixedSize(BUTTON_WIDTH, HEIGHT);
    _minimizeButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    connect(_minimizeButton, &QPushButton::clicked, this, &Frameless::showMinimized);

    _maximizeButton = new TitleBarButton(TitleBarButton::ButtonType::MAXIMIZE, windowBar);
    _maximizeButton->setFixedSize(BUTTON_WIDTH, HEIGHT);
    _maximizeButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    connect(_maximizeButton, &QPushButton::clicked, this, [=, this] {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });

    auto *closeButton = new TitleBarButton(TitleBarButton::ButtonType::CLOSE, windowBar);
    closeButton->setFixedSize(BUTTON_WIDTH, HEIGHT);
    closeButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    connect(closeButton, &QPushButton::clicked, this, &Frameless::close);

    auto *spacer = new QWidget(windowBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    windowBar->AddWidget(TitleBar::WidgetType::ICON, icon, 0, Qt::AlignCenter);
    windowBar->AddWidget(TitleBar::WidgetType::TITLE, title, 0, Qt::AlignCenter);
    windowBar->AddWidget(TitleBar::WidgetType::CUSTOM, spacer, 1, Qt::AlignCenter);
    windowBar->AddWidget(TitleBar::WidgetType::MIN_BUTTON, _minimizeButton, 0, Qt::AlignCenter);
    windowBar->AddWidget(TitleBar::WidgetType::MAX_BUTTON, _maximizeButton, 0, Qt::AlignCenter);
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

    _windowAgent->setup(this);
    _windowAgent->setTitleBar(windowBar);
    _windowAgent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::WindowIcon, icon);
    _windowAgent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Minimize, _minimizeButton);
    _windowAgent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Maximize, _maximizeButton);
    _windowAgent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Close, closeButton);

    auto *layout = new QVBoxLayout(this);
    layout->setMenuBar(windowBar);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(_centralWidget);
}

void Frameless::HideButton()
{
    _maximizeButton->hide();
    _minimizeButton->hide();
}

bool Frameless::eventFilter(QObject *watched, QEvent *event)
{
    // 监听窗口标志更改事件
    if (watched == this && event->type() == QEvent::Show) {
        CheckWindowFlags();
    }

    return QWidget::eventFilter(watched, event);
}

void Frameless::CheckWindowFlags()
{
    bool isDialog = this->windowFlags() & Qt::Dialog;

    if (isDialog) {
        // 如果是Dialog类型，只保留关闭按钮
        HideButton();

        // 更新布局
        auto *windowBar = qobject_cast<TitleBar *>(_windowAgent->titleBar());
        if (windowBar) {
            windowBar->UpdateLayout({
                TitleBar::WidgetType::SPACER_ITEM,
                TitleBar::WidgetType::ICON,
                TitleBar::WidgetType::SPACER_ITEM,
                TitleBar::WidgetType::SPACER_ITEM,
                TitleBar::WidgetType::TITLE,
                TitleBar::WidgetType::CUSTOM,
                TitleBar::WidgetType::SPACER_ITEM,
                TitleBar::WidgetType::CLOSE_BUTTON,
            });
        }
    } else {
        // 如果不是Dialog类型，显示所有按钮
        if (_minimizeButton && _maximizeButton) {
            _minimizeButton->show();
            _maximizeButton->show();
        }

        // 更新布局为完整版
        auto *windowBar = qobject_cast<TitleBar *>(_windowAgent->titleBar());
        if (windowBar) {
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
        }
    }
}