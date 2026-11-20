#pragma once

#include <QFocusEvent>
#include <QPainter>
#include <QToolButton>

#include "magpie_motion.h"

namespace desktop {
// Magpie's .text.ci-auto: neutral control text, persistent 16% accent fill when checked, no checkbox glyph.
class MagpieCheckButton final : public QToolButton
{
public:
    explicit MagpieCheckButton(QWidget *parent = nullptr) :
        QToolButton(parent), colorAnimation_(this), pressAnimation_(this)
    {
        setCheckable(true);
        setFocusPolicy(Qt::StrongFocus);
        setCursor(Qt::PointingHandCursor);
        setFixedHeight(26);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setProperty("magpieCheckable", true);
        connect(this, &QAbstractButton::toggled, this, [this] { Transition(); });
        connect(this, &QAbstractButton::pressed, this, [this] {
            motion::Animate(pressAnimation_, scale_, motion::PressScale, motion::PressMs);
        });
        connect(this, &QAbstractButton::released, this, [this] {
            motion::Animate(pressAnimation_, scale_, 1.0, motion::PressMs);
        });
        connect(&colorAnimation_, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
            progress_ = value.toReal();
            update();
        });
        connect(&pressAnimation_, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
            scale_ = value.toReal();
            update();
        });
    }

    QSize sizeHint() const override { return { fontMetrics().horizontalAdvance(text()) + 14, 26 }; }

    QSize minimumSizeHint() const override { return sizeHint(); }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        if (!isEnabled()) {
            painter.setOpacity(.45);
        }
        painter.translate(QRectF(rect()).center());
        painter.scale(scale_, scale_);
        painter.translate(-QRectF(rect()).center());
        const auto accent     = theme::AccentColor(this);
        const auto BASE       = QColor(underMouse() && !isChecked() ? theme::Raised : theme::Surface);
        const auto BACKGROUND = theme::Blend(BASE, accent, .16 * progress_);
        const auto INK = theme::Blend(QColor(underMouse() && !isChecked() ? theme::Text : "#a1a1aa"), accent, progress_);
        painter.setPen(Qt::NoPen);
        painter.setBrush(BACKGROUND);
        painter.drawRoundedRect(QRectF(rect()), 6, 6);
        painter.setPen(INK);
        painter.drawText(rect(), Qt::AlignCenter, text());
        if (keyboardFocus_ && hasFocus()) {
            painter.setPen(QPen(accent, 1));
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(QRectF(rect()).adjusted(.5, .5, -.5, -.5), 6, 6);
        }
    }

    void enterEvent(QEnterEvent *event) override
    {
        QToolButton::enterEvent(event);
        update();
    }

    void leaveEvent(QEvent *event) override
    {
        QToolButton::leaveEvent(event);
        update();
    }

    void focusInEvent(QFocusEvent *event) override
    {
        keyboardFocus_ = event->reason() == Qt::TabFocusReason || event->reason() == Qt::BacktabFocusReason ||
                         event->reason() == Qt::ShortcutFocusReason;
        QToolButton::focusInEvent(event);
        update();
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        keyboardFocus_ = false;
        QToolButton::mousePressEvent(event);
    }

    void hideEvent(QHideEvent *event) override
    {
        colorAnimation_.stop();
        pressAnimation_.stop();
        progress_ = isChecked() ? 1 : 0;
        scale_    = 1;
        QToolButton::hideEvent(event);
    }

private:
    void Transition()
    {
        if (isVisible()) {
            motion::Animate(colorAnimation_, progress_, isChecked() ? 1.0 : 0.0, 160);
        } else {
            colorAnimation_.stop();
            progress_ = isChecked() ? 1 : 0;
            update();
        }
    }

    QVariantAnimation colorAnimation_, pressAnimation_;
    qreal             progress_ = 0, scale_ = 1;
    bool              keyboardFocus_ = false;
};
} // namespace desktop
