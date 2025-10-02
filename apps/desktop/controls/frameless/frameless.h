#ifndef TFA_UI_FRAMELESS_WIDGET_H
#define TFA_UI_FRAMELESS_WIDGET_H

#include <QPushButton>
#include <QWidget>

#include "../export.h"

namespace QWK {
class WidgetWindowAgent;
} // namespace QWK


class CONTROLS_PRIVATE FramelessPrivate;

class CONTROLS_API Frameless : public QWidget
{
    Q_OBJECT

public:
    explicit Frameless(QWidget *parent = nullptr);
    virtual ~Frameless() = default;

    void     HideButton();
    QWidget *ContentWidget();
    void     AddIgnoreWidget(QWidget *widget);
    void     AddTabBar(QWidget *widget, int stretch);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void InstallWindowAgent();
    void SetupUI();
    void InitTitleBar();
    void CheckWindowFlags();

    QWK::WidgetWindowAgent *_windowAgent    = nullptr;
    QWidget                *_centralWidget  = nullptr;
    QPushButton            *_minimizeButton = nullptr;
    QPushButton            *_maximizeButton = nullptr;
};


#endif // TFA_UI_FRAMELESS_WIDGET_H
