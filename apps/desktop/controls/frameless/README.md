# Frameless 组件使用说明

## 概述

Frameless 组件提供了一个灵活的无边框窗口解决方案，支持自定义 TitleBar。

## 设计理念

Frameless 组件采用**解耦设计**，不再与特定的 TitleBar 实现深度集成，而是通过 `SetupTitleBar` 接口允许用户安装不同的 titlebar 实现。

## 核心接口

### SetupTitleBar(TitleBarSetupFunction setupFunc)

通过回调函数的方式设置自定义的 TitleBar。

**函数签名：**
```cpp
using TitleBarSetupFunction = std::function<QWidget*(Frameless*, QWK::WidgetWindowAgent*)>;
void SetupTitleBar(TitleBarSetupFunction setupFunc);
```

**参数：**
- `frameless`: Frameless 窗口实例，可用于连接信号槽
- `agent`: QWK::WidgetWindowAgent 实例，用于设置系统按钮等

**返回值：**
- 创建的 titlebar widget

### SetupDefaultTitleBar()

使用内置的默认 TitleBar 实现（包含图标、标题、最小化/最大化/关闭按钮）。

## 使用示例

### 1. 使用默认 TitleBar

```cpp
#include "controls/frameless/frameless.h"

Frameless window;
window.SetupDefaultTitleBar();  // 使用默认 titlebar
window.setWindowTitle("My Application");
window.show();
```

### 2. 自定义简单 TitleBar

```cpp
#include "controls/frameless/frameless.h"
#include <QPushButton>
#include <QHBoxLayout>

Frameless window;

// 自定义 titlebar：只有一个关闭按钮
window.SetupTitleBar([](Frameless* frameless, QWK::WidgetWindowAgent* agent) -> QWidget* {
    auto* titleBar = new QWidget(frameless);
    titleBar->setFixedHeight(40);

    auto* layout = new QHBoxLayout(titleBar);
    layout->setContentsMargins(0, 0, 0, 0);

    // 添加标题
    auto* title = new QLabel("Simple Title", titleBar);
    layout->addWidget(title);

    layout->addStretch();

    // 只添加关闭按钮
    auto* closeBtn = new QPushButton("X", titleBar);
    closeBtn->setFixedSize(40, 40);
    QObject::connect(closeBtn, &QPushButton::clicked, frameless, &Frameless::close);
    layout->addWidget(closeBtn);

    // 设置到 agent
    agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Close, closeBtn);

    return titleBar;
});

window.show();
```

### 3. 使用现有的 TitleBar 组件

```cpp
#include "controls/frameless/frameless.h"
#include "controls/frameless/title_bar.h"
#include "controls/frameless/title_bar_button.h"

Frameless window;

window.SetupTitleBar([](Frameless* frameless, QWK::WidgetWindowAgent* agent) -> QWidget* {
    constexpr int BUTTON_WIDTH = 45;
    constexpr int HEIGHT = 35;

    // 创建自定义的 TitleBar
    auto* windowBar = new TitleBar(frameless);
    windowBar->SetHostWidget(frameless);
    windowBar->setFixedHeight(HEIGHT);

    // 添加自定义控件...
    auto* icon = new QLabel(windowBar);
    auto* title = new QLabel(windowBar);

    auto* closeButton = new TitleBarButton(TitleBarButton::ButtonType::CLOSE, windowBar);
    closeButton->setFixedSize(BUTTON_WIDTH, HEIGHT);
    QObject::connect(closeButton, &QPushButton::clicked, frameless, &Frameless::close);

    windowBar->AddWidget(TitleBar::WidgetType::ICON, icon, 0, Qt::AlignCenter);
    windowBar->AddWidget(TitleBar::WidgetType::TITLE, title, 0, Qt::AlignCenter);
    windowBar->AddWidget(TitleBar::WidgetType::CLOSE_BUTTON, closeButton, 0, Qt::AlignCenter);

    windowBar->UpdateLayout({
        TitleBar::WidgetType::ICON,
        TitleBar::WidgetType::TITLE,
        TitleBar::WidgetType::CLOSE_BUTTON,
    });

    // 设置系统按钮
    agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::WindowIcon, icon);
    agent->setSystemButton(QWK::WidgetWindowAgent::SystemButton::Close, closeButton);

    return windowBar;
});

window.show();
```

### 4. 不使用 TitleBar（完全自定义）

```cpp
Frameless window;
// 不调用 SetupTitleBar，窗口将没有 titlebar
window.show();
```

## 其他接口

### ContentWidget()

获取内容区域的 widget，用于添加应用的主要内容。

```cpp
Frameless window;
window.SetupDefaultTitleBar();

QWidget* content = window.ContentWidget();
auto* layout = new QVBoxLayout(content);
layout->addWidget(new QLabel("Hello World"));
```

### AddIgnoreWidget(QWidget* widget)

添加忽略的 widget，这些 widget 不会触发窗口拖动。

```cpp
window.AddIgnoreWidget(myButton);
```

### GetWindowAgent()

获取底层的 `QWK::WidgetWindowAgent` 实例，用于高级自定义。

```cpp
auto* agent = window.GetWindowAgent();
// 进行更多自定义设置...
```

## 优势

1. **解耦设计**：Frameless 不再与 TitleBar 强耦合
2. **灵活性**：可以轻松创建不同风格的 titlebar
3. **可扩展性**：支持完全自定义的 titlebar 实现
4. **向后兼容**：提供 `SetupDefaultTitleBar()` 保持原有功能

## 迁移指南

### 从旧版本迁移

**旧代码：**
```cpp
Frameless window;  // 自动创建 titlebar
window.show();
```

**新代码：**
```cpp
Frameless window;
window.SetupDefaultTitleBar();  // 显式调用设置 titlebar
window.show();
```

如果你想使用自定义 titlebar，只需将 `SetupDefaultTitleBar()` 替换为 `SetupTitleBar(customFunc)`。
