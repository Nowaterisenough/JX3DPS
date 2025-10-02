#ifndef TFA_UI_WINDOW_BAR_H
#define TFA_UI_WINDOW_BAR_H

#include <QFrame>

#include "../export.h"

class QMenu;
class QToolButton;


class CONTROLS_PRIVATE TitleBarPrivate;

class CONTROLS_API TitleBar : public QFrame
{
    Q_OBJECT

public:
    enum WidgetType
    {
        TITLE,
        ICON,
        MIN_BUTTON,
        MAX_BUTTON,
        CLOSE_BUTTON,
        MENU_BAR,
        TOOL_BAR,
        SPACER_ITEM,
        CUSTOM,
    };

    explicit TitleBar(QWidget *parent = nullptr);
    ~TitleBar();

    void     AddWidget(int id, QWidget *widget, int stretch = 0, Qt::Alignment alignment = Qt::AlignLeft);
    QWidget *TakeWidget(int id);
    QWidget *Widget(int id) const;

    void SetMenu(int id, QMenu *menu);
    void SetToolButton(int id, QToolButton *button);

    void     SetHostWidget(QWidget *w);
    QWidget *HostWidget() const;

    bool TitleFollowWindow() const;
    void SetTitleFollowWindow(bool value);

    bool IconFollowWindow() const;
    void SetIconFollowWindow(bool value);

    void UpdateLayout(const std::vector<int> &ids);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Q_DECLARE_PRIVATE(TitleBar)
    TitleBarPrivate *d_ptr;
};


#endif // TFA_UI_WINDOW_BAR_H