#ifndef TFA_UI_FRAMELESS_WIDGET_H
#define TFA_UI_FRAMELESS_WIDGET_H

#include <QPushButton>
#include <QWidget>
#include <functional>

#include "../export.h"

namespace QWK {
class WidgetWindowAgent;
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
