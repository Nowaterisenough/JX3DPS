#ifndef DESKTOP_CHOICE_POPUP_H
#define DESKTOP_CHOICE_POPUP_H

#include <QFrame>
#include <QHideEvent>
#include <QKeyEvent>
#include <QLayout>
#include <QRegion>
#include <QScreen>
#include <QVariantAnimation>
#include <QWidget>

#include <algorithm>
#include <functional>
#include "theme/accent_color.h"

namespace desktop {

// Finalize geometry before showing; never resize a visible native combo popup.
class ChoicePopup final : public QFrame
{
public:
    enum Placement { Below, Beside };
    enum Transition { Immediate, Expand };
    explicit ChoicePopup(QWidget *owner) : QFrame(owner, Qt::Popup | Qt::FramelessWindowHint), m_expansion(this)
    {
        setObjectName(QStringLiteral("choicePopup"));
        setAttribute(Qt::WA_NoMouseReplay);
        setAttribute(Qt::WA_WindowPropagation);
        setFrameShape(QFrame::StyledPanel);
        setStyleSheet(QStringLiteral(
            "QFrame#choicePopup { background: #232327; border: 1px solid #333339; border-radius: 8px; }"));
        m_expansion.setDuration(180);
        m_expansion.setEasingCurve(QEasingCurve::OutCubic);
        connect(&m_expansion, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
            Reveal(value.toReal());
        });
        connect(&m_expansion, &QVariantAnimation::finished, this, [this] { clearMask(); });
    }

    void Open(QWidget *anchor, QSize preferred, QWidget *focus, Placement placement = Below, Transition transition = Immediate)
    {
        auto colors = anchor->palette();
        colors.setColor(QPalette::Accent, theme::AccentColor(anchor));
        colors.setColor(QPalette::Link, theme::AccentColor(anchor));
        setPalette(colors);
        m_expansion.stop();
        clearMask();
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
        m_upwards = placement == Below && upwards;
        if (transition == Expand) Reveal(0);
        show();
        focus->setFocus(Qt::PopupFocusReason);
        if (transition == Expand) {
            m_expansion.setStartValue(0.0);
            m_expansion.setEndValue(1.0);
            m_expansion.start();
        }
    }

    bool ExpansionAnimating() const { return m_expansion.state() == QAbstractAnimation::Running; }
    void Watch(QWidget *widget) { widget->installEventFilter(this); }
    std::function<void()> acceptChoice;

protected:
    void hideEvent(QHideEvent *event) override
    {
        m_expansion.stop();
        clearMask();
        QFrame::hideEvent(event);
    }

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

private:
    void Reveal(qreal progress)
    {
        // Reveal the fixed layout without resizing the native popup or shifting its items.
        const int extent = std::clamp(qRound(height() * progress), 1, height());
        setMask(QRegion(0, m_upwards ? height() - extent : 0, width(), extent));
    }

    QVariantAnimation m_expansion;
    bool m_upwards = false;
};

} // namespace desktop
#endif
