#ifndef TFA_WINDOW_BAR_PRIVATE_H
#define TFA_WINDOW_BAR_PRIVATE_H

#include <QBoxLayout>
#include <QMap>

class QMenu;
class QToolButton;


class TitleBar;

class TitleBarPrivate
{
public:
    TitleBarPrivate(TitleBar *q);
    ~TitleBarPrivate() = default;

    void     AddWidget(int id, QWidget *widget, int stretch, Qt::Alignment alignment);
    QWidget *TakeWidget(int id);
    QWidget *Widget(int id) const;

    void SetMenu(int id, QMenu *menu);
    void SetToolButton(int id, QToolButton *button);

    void UpdateLayout(const std::vector<int> &ids);

    void AddWidgetIfExists(int id, const QMap<int, QPair<int, Qt::Alignment>> &stretchAlignmentInfo);

    QBoxLayout          *layout;
    QMap<int, QWidget *> widgets;
    QWidget             *hostWidget;
    bool                 autoTitle = true;
    bool                 autoIcon  = true;

private:
    TitleBar *q_ptr;
    Q_DECLARE_PUBLIC(TitleBar)
};


#endif // TFA_WINDOW_BAR_PRIVATE_H