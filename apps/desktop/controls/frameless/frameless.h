#ifndef TFA_UI_FRAMELESS_WIDGET_H
#define TFA_UI_FRAMELESS_WIDGET_H

#include <QPushButton>
#include <QWidget>
#include <functional>

#include "../export.h"

namespace QWK {
#if __has_include(<QWKWidgets/widgetwindowagent.h>)
class WidgetWindowAgent;
#else
// QWindowKit is optional in source checkouts. Keep the desktop shell usable
// with the native QWidget frame when the third-party submodule is absent.
class WidgetWindowAgent {
public:
    enum class SystemButton { WindowIcon, Minimize, Maximize, Close };
    explicit WidgetWindowAgent(QWidget *) {}
    void setup(QWidget *) {}
    void setHitTestVisible(QWidget *) {}
    void setTitleBar(QWidget *) {}
    void setSystemButton(SystemButton, QWidget *) {}
};
#endif
} // namespace QWK


class CONTROLS_PRIVATE FramelessPrivate;

class CONTROLS_API Frameless : public QWidget
{
    Q_OBJECT

public:
    // TitleBar 设置函数类型：接收 Frameless* 和 QWK::WidgetWindowAgent*，返回创建的 titlebar widget
    using TitleBarSetupFunction = std::function<QWidget*(Frameless*, QWK::WidgetWindowAgent*)>;

    explicit Frameless(QWidget *parent = nullptr);
    virtual ~Frameless() = default;

    // 设置 titlebar（如果不调用，则不创建 titlebar）
    void SetupTitleBar(TitleBarSetupFunction setupFunc);

    // 便捷方法：使用默认的 TitleBar 实现
    void SetupDefaultTitleBar();

    QWidget *ContentWidget();
    void     AddIgnoreWidget(QWidget *widget);

    // 获取 WindowAgent，供外部自定义使用
    QWK::WidgetWindowAgent *GetWindowAgent() const { return _windowAgent; }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void InstallWindowAgent();
    void SetupUI();

    QWK::WidgetWindowAgent *_windowAgent   = nullptr;
    QWidget                *_centralWidget = nullptr;
    QWidget                *_titleBar      = nullptr;
};


#endif // TFA_UI_FRAMELESS_WIDGET_H
