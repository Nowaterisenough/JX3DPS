#ifndef DESKTOP_CHOICE_POPUP_H
#define DESKTOP_CHOICE_POPUP_H

#include <algorithm>
#include <functional>

#include <QFrame>
#include <QHideEvent>
#include <QKeyEvent>
#include <QLayout>
#include <QRegion>
#include <QScreen>
#include <QVariantAnimation>
#include <QWidget>

#include "theme/accent_color.h"

namespace desktop {

// Finalize geometry before showing; never resize a visible native combo popup.
class ChoicePopup final : public QFrame
{
public:
    enum Placement
    {
        BELOW,
        BESIDE
    };

    enum Transition
    {
        IMMEDIATE,
        EXPAND
    };

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

    void Open(QWidget *anchor, QSize preferred, QWidget *focus, Placement placement = BELOW, Transition transition = IMMEDIATE)
    {
        auto colors = anchor->palette();
        colors.setColor(QPalette::Accent, theme::AccentColor(anchor));
        colors.setColor(QPalette::Link, theme::AccentColor(anchor));
        setPalette(colors);
        m_expansion.stop();
        clearMask();
        const QRect  AVAILABLE   = anchor->screen()->availableGeometry();
        const QPoint TOP         = anchor->mapToGlobal(QPoint());
        const QPoint BOTTOM      = anchor->mapToGlobal(QPoint(0, anchor->height()));
        const int    SPACE_BELOW = std::max(0, AVAILABLE.bottom() - BOTTOM.y() + 1);
        const int    SPACE_ABOVE = std::max(0, TOP.y() - AVAILABLE.top());
        const bool   UPWARDS     = preferred.height() > SPACE_BELOW && SPACE_ABOVE > SPACE_BELOW;
        preferred.setWidth(std::min(preferred.width(), AVAILABLE.width()));
        preferred.setHeight(std::min(preferred.height(),
                                     placement == BESIDE ? AVAILABLE.height() : std::max(1, UPWARDS ? SPACE_ABOVE : SPACE_BELOW)));
        int left = TOP.x();
        if (placement == BESIDE) {
            const int right = TOP.x() + anchor->width() + 8;
            left = right + preferred.width() <= AVAILABLE.right() + 1 ? right : TOP.x() - preferred.width() - 8;
        }
        const int X = std::clamp(left, AVAILABLE.left(), AVAILABLE.right() - preferred.width() + 1);
        const int Y = std::clamp(placement == BESIDE ? TOP.y()
                                 : UPWARDS           ? TOP.y() - preferred.height()
                                                     : BOTTOM.y(),
                                 AVAILABLE.top(),
                                 AVAILABLE.bottom() - preferred.height() + 1);
        setGeometry(QRect(QPoint(X, Y), preferred));
        if (layout() != nullptr) {
            layout()->activate();
        }
        m_upwards = placement == Placement::BELOW && UPWARDS;
        if (transition == EXPAND) {
            Reveal(0);
        }
        show();
        focus->setFocus(Qt::PopupFocusReason);
        if (transition == EXPAND) {
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
            const auto KEY = static_cast<QKeyEvent *>(event)->key();
            if (KEY == Qt::Key_Escape || KEY == Qt::Key_Tab || KEY == Qt::Key_Backtab) {
                hide();
                return true;
            }
            if ((KEY == Qt::Key_Return || KEY == Qt::Key_Enter) && acceptChoice) {
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
    bool              m_upwards = false;
};

} // namespace desktop
#endif
