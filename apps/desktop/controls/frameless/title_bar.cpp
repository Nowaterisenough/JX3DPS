#include "title_bar.h"

#include <vector>

#include <QEvent>
#include <QLabel>
#include <QMenu>
#include <QToolButton>

#include "title_bar_private.h"

TitleBar::TitleBar(QWidget *parent) : QFrame(parent), d_ptr(new TitleBarPrivate(this)) { }

TitleBar::~TitleBar() = default;

void TitleBar::AddWidget(int id, QWidget *widget, int stretch, Qt::Alignment alignment)
{
    Q_D(TitleBar);
    d->AddWidget(id, widget, stretch, alignment);
}

QWidget *TitleBar::TakeWidget(int id)
{
    Q_D(TitleBar);
    return d->TakeWidget(id);
}

QWidget *TitleBar::Widget(int id) const
{
    Q_D(const TitleBar);
    return d->Widget(id);
}

void TitleBar::SetMenu(int id, QMenu *menu)
{
    Q_D(TitleBar);
    d->SetMenu(id, menu);
}

void TitleBar::SetToolButton(int id, QToolButton *button)
{
    Q_D(TitleBar);
    d->SetToolButton(id, button);
}

void TitleBar::SetHostWidget(QWidget *w)
{
    Q_D(TitleBar);
    if (d->hostWidget != nullptr) {
        d->hostWidget->removeEventFilter(this);
    }
    d->hostWidget = w;
    if (w != nullptr) {
        w->installEventFilter(this);
    }
}

QWidget *TitleBar::HostWidget() const
{
    Q_D(const TitleBar);
    return d->hostWidget;
}

bool TitleBar::TitleFollowWindow() const
{
    Q_D(const TitleBar);
    return d->autoTitle;
}

void TitleBar::SetTitleFollowWindow(bool value)
{
    Q_D(TitleBar);
    d->autoTitle = value;
}

bool TitleBar::IconFollowWindow() const
{
    Q_D(const TitleBar);
    return d->autoIcon;
}

void TitleBar::SetIconFollowWindow(bool value)
{
    Q_D(TitleBar);
    d->autoIcon = value;
}

void TitleBar::UpdateLayout(const std::vector<int> &ids)
{
    Q_D(TitleBar);
    d->UpdateLayout(ids);
}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    Q_D(TitleBar);
    if (obj == d->hostWidget) {
        switch (event->type()) {
            case QEvent::WindowIconChange:
                if (d->autoIcon) {
                    auto *icon = qobject_cast<QLabel *>(d->widgets[ICON]);
                    if (icon != nullptr) {
                        icon->setPixmap(d->hostWidget->windowIcon().pixmap(16, 16));
                    }
                }
                break;
            case QEvent::WindowTitleChange:
                if (d->autoTitle) {
                    auto *title = qobject_cast<QLabel *>(d->widgets[TITLE]);
                    if (title != nullptr) {
                        title->setText(d->hostWidget->windowTitle());
                    }
                }
                break;
            default: break;
        }
    }
    return QFrame::eventFilter(obj, event);
}

TitleBarPrivate::TitleBarPrivate(TitleBar *q) : layout(new QBoxLayout(QBoxLayout::LeftToRight, q)), q_ptr(q)
{
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
}

void TitleBarPrivate::AddWidget(int id, QWidget *widget, int stretch, Qt::Alignment alignment)
{
    Q_Q(TitleBar);
    if (widgets.contains(id)) {
        auto *w = TakeWidget(id);
        w->deleteLater();
    }
    widgets[id] = widget;
    layout->addWidget(widget, stretch, alignment);
}

QWidget *TitleBarPrivate::TakeWidget(int id)
{
    if (!widgets.contains(id)) {
        return nullptr;
    }
    QWidget *widget = widgets[id];
    layout->removeWidget(widget);
    widgets.remove(id);
    return widget;
}

QWidget *TitleBarPrivate::Widget(int id) const
{
    return widgets.value(id);
}

void TitleBarPrivate::SetMenu(int id, QMenu *menu)
{
    auto *button = new QToolButton();
    button->setMenu(menu);
    button->setPopupMode(QToolButton::InstantPopup);
    AddWidget(id, button, 0, Qt::AlignLeft);
}

void TitleBarPrivate::SetToolButton(int id, QToolButton *button)
{
    AddWidget(id, button, 0, Qt::AlignLeft);
}

void TitleBarPrivate::UpdateLayout(const std::vector<int> &types)
{
    // 创建一个临时map来存储原有的stretch和alignment信息
    QMap<int, QPair<int, Qt::Alignment>> stretch_alignment_info;

    // 保存原有的stretch和alignment信息
    for (int i = 0; i < layout->count(); ++i) {
        const QLayoutItem *item = layout->itemAt(i);
        if (item->widget() != nullptr) {
            const int ID = widgets.key(item->widget(), -1);
            if (ID != -1) {
                stretch_alignment_info[ID] = qMakePair(layout->stretch(i), item->alignment());
            }
        }
    }

    // 清空现有布局
    const QLayoutItem *item = nullptr;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item;
    }

    // 添加控件，保留原有的stretch和alignment
    for (const int type : types) {
        if (type == static_cast<int>(TitleBar::SPACER_ITEM)) {
            layout->addItem(new QSpacerItem(5, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
        } else {
            AddWidgetIfExists(type, stretch_alignment_info);
        }
    }
}

void TitleBarPrivate::AddWidgetIfExists(int id, const QMap<int, QPair<int, Qt::Alignment>> &stretch_alignment_info)
{
    if (widgets.contains(id)) {
        QWidget *widget = widgets[id];
        if (stretch_alignment_info.contains(id)) {
            const auto &info = stretch_alignment_info[id];
            layout->addWidget(widget, info.first, info.second);
        } else {
            layout->addWidget(widget);
        }
    }
}
