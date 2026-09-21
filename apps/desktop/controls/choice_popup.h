#ifndef DESKTOP_CHOICE_POPUP_H
#define DESKTOP_CHOICE_POPUP_H

#include <QFrame>
#include <QKeyEvent>
#include <QLayout>
#include <QScreen>
#include <QWidget>

#include <algorithm>
#include <functional>

namespace desktop {

// Finalize geometry before showing; never resize a visible native combo popup.
class ChoicePopup final : public QFrame
{
public:
    enum Placement { Below, Beside };
    explicit ChoicePopup(QWidget *owner) : QFrame(owner, Qt::Popup | Qt::FramelessWindowHint)
    {
        setObjectName(QStringLiteral("choicePopup"));
        setAttribute(Qt::WA_NoMouseReplay);
        setFrameShape(QFrame::StyledPanel);
        setStyleSheet(QStringLiteral(
            "QFrame#choicePopup { background: #202631; border: 1px solid #8d7546; border-radius: 4px; }"));
    }

    void Open(QWidget *anchor, QSize preferred, QWidget *focus, Placement placement = Below)
    {
        const QRect available = anchor->screen()->availableGeometry();
        const QPoint top = anchor->mapToGlobal(QPoint());
        const QPoint bottom = anchor->mapToGlobal(QPoint(0, anchor->height()));
        const int below = std::max(0, available.bottom() - bottom.y() + 1);
        const int above = std::max(0, top.y() - available.top());
        const bool upwards = preferred.height() > below && above > below;
        preferred.setWidth(std::min(preferred.width(), available.width()));
        preferred.setHeight(std::min(preferred.height(), placement == Beside ? available.height() : std::max(1, upwards ? above : below)));
        int left = top.x();
        if (placement == Beside) {
            const int right = top.x() + anchor->width() + 8;
            left = right + preferred.width() <= available.right() + 1 ? right : top.x() - preferred.width() - 8;
        }
        const int x = std::clamp(left, available.left(), available.right() - preferred.width() + 1);
        const int y = std::clamp(placement == Beside ? top.y() : upwards ? top.y() - preferred.height() : bottom.y(),
                                 available.top(), available.bottom() - preferred.height() + 1);
        setGeometry(QRect(QPoint(x, y), preferred));
        if (layout()) layout()->activate();
        show();
        focus->setFocus(Qt::PopupFocusReason);
    }

    void Watch(QWidget *widget) { widget->installEventFilter(this); }
    std::function<void()> acceptChoice;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::KeyPress) {
            const auto key = static_cast<QKeyEvent *>(event)->key();
            if (key == Qt::Key_Escape || key == Qt::Key_Tab || key == Qt::Key_Backtab) {
                hide();
                return true;
            }
            if ((key == Qt::Key_Return || key == Qt::Key_Enter) && acceptChoice) {
                acceptChoice();
                return true;
            }
        }
        return QFrame::eventFilter(watched, event);
    }
};

} // namespace desktop
#endif
