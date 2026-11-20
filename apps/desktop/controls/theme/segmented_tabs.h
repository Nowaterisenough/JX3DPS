#pragma once

#include <QFocusEvent>
#include <QHash>
#include <QHideEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTabBar>
#include <QTabWidget>

#include "magpie_motion.h"

namespace desktop {
class SegmentedTabBar final : public QTabBar
{
public:
    explicit SegmentedTabBar(QWidget *parent = nullptr) : QTabBar(parent), animation_(this)
    {
        setDrawBase(false);
        setMouseTracking(true);
        setExpanding(false);
        setElideMode(Qt::ElideRight);
        setProperty("segmentedControl", true);
        connect(&animation_, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
            indicator_ = value.toRectF();
            update();
        });
        connect(this, &QTabBar::currentChanged, this, [this] {
            const auto FROM = IndicatorRect();
            selected_       = currentIndex();
            if (!isVisible() || FROM.isEmpty() || target().isEmpty()) {
                snap();
            } else {
                motion::Animate(animation_, FROM, target(), motion::SlideMs);
            }
            UpdateColors();
        });
    }

    QRectF IndicatorRect() const
    {
        return animation_.state() == QAbstractAnimation::Running ? indicator_ : segmentRect(selected_);
    }

    bool SelectionAnimating() const { return animation_.state() == QAbstractAnimation::Running; }

    qreal PressScale(int index) const { return scales_.value(index, 1); }

    bool KeyboardFocusVisible() const { return keyboardFocus_ && hasFocus(); }

    void SetSegmentHeight(int height)
    {
        segmentHeight_ = height;
        setFixedHeight(height);
        updateGeometry();
    }

protected:
    QSize tabSizeHint(int index) const override
    {
        const int ICON = tabIcon(index).isNull() ? 0 : 22;
        auto      f    = font();
        f.setWeight(QFont::Normal);
        return { qMax(46, QFontMetrics(f).horizontalAdvance(tabText(index)) + 22 + ICON), segmentHeight_ };
    }

    QSize minimumTabSizeHint(int index) const override { return tabSizeHint(index); }

    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(theme::Raised));
        p.drawRoundedRect(QRectF(rect()).adjusted(.5, .5, -.5, -.5), 7, 7);
        if (!IndicatorRect().isEmpty()) {
            motion::Thumb(p, IndicatorRect());
        }
        for (int index = 0; index < count(); ++index) {
            if (!isTabVisible(index)) {
                continue;
            }
            const auto segment = segmentRect(index);
            if (!colors_.contains(index)) {
                colors_[index] = TargetColor(index);
            }
            p.save();
            p.translate(segment.center());
            p.scale(PressScale(index), PressScale(index));
            p.translate(-segment.center());
            auto f = font();
            f.setWeight(QFont::Normal);
            p.setFont(f);
            p.setPen(colors_[index]);
            auto label = segment.adjusted(7, 0, -7, 0);
            if (!tabIcon(index).isNull()) {
                const QRect ICON_RECT(qRound(label.left()), qRound(label.center().y() - 8), 16, 16);
                tabIcon(index).paint(&p, ICON_RECT, Qt::AlignCenter, isTabEnabled(index) ? QIcon::Normal : QIcon::Disabled);
                label.setLeft(label.left() + 22);
            }
            p.drawText(label, Qt::AlignCenter, p.fontMetrics().elidedText(tabText(index), Qt::ElideRight, qRound(label.width())));
            p.restore();
            if (index == currentIndex() && KeyboardFocusVisible()) {
                p.setPen(QPen(theme::AccentColor(this), 1));
                p.setBrush(Qt::NoBrush);
                p.drawRoundedRect(segment.adjusted(.5, .5, -.5, -.5), 5, 5);
            }
        }
    }

    void resizeEvent(QResizeEvent *event) override
    {
        QTabBar::resizeEvent(event);
        snap();
    }

    void showEvent(QShowEvent *event) override
    {
        QTabBar::showEvent(event);
        snap();
    }

    void hideEvent(QHideEvent *event) override
    {
        QTabBar::hideEvent(event);
        CancelPress();
        snap();
    }

    void changeEvent(QEvent *event) override
    {
        QTabBar::changeEvent(event);
        if (event->type() == QEvent::EnabledChange) {
            if (!isEnabled()) {
                CancelPress();
            }
            UpdateColors();
        }
    }

    void tabLayoutChange() override
    {
        if (currentIndex() == selected_) {
            snap();
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() != Qt::LeftButton) {
            QTabBar::mousePressEvent(event);
            return;
        }
        keyboardFocus_ = false;
        setFocus(Qt::MouseFocusReason);
        const int INDEX = tabAt(event->pos());
        if (INDEX >= 0 && isTabEnabled(INDEX)) {
            pressed_ = INDEX;
            SetPress(INDEX, true);
        }
        event->accept();
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        if (event->button() != Qt::LeftButton) {
            QTabBar::mouseReleaseEvent(event);
            return;
        }
        const int INDEX = pressed_;
        CancelPress();
        if (INDEX >= 0 && tabAt(event->pos()) == INDEX && isTabEnabled(INDEX)) {
            setCurrentIndex(INDEX);
            emit tabBarClicked(INDEX);
        }
        event->accept();
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        const int HOVERED = tabAt(event->pos());
        if (HOVERED != hovered_) {
            hovered_ = HOVERED;
            UpdateColors();
        }
        if (pressed_ >= 0) {
            SetPress(pressed_, HOVERED == pressed_);
        }
        QTabBar::mouseMoveEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        hovered_ = -1;
        UpdateColors();
        if (pressed_ >= 0) {
            SetPress(pressed_, false);
        }
        QTabBar::leaveEvent(event);
    }

    void focusInEvent(QFocusEvent *event) override
    {
        keyboardFocus_ = event->reason() == Qt::TabFocusReason || event->reason() == Qt::BacktabFocusReason ||
                         event->reason() == Qt::ShortcutFocusReason;
        QTabBar::focusInEvent(event);
        update();
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        keyboardFocus_ = true;
        if ((event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) && !event->isAutoRepeat()) {
            pressed_ = currentIndex();
            if (pressed_ >= 0) {
                SetPress(pressed_, true);
            }
            event->accept();
        } else {
            QTabBar::keyPressEvent(event);
        }
        update();
    }

    void keyReleaseEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
            CancelPress();
            event->accept();
        } else {
            QTabBar::keyReleaseEvent(event);
        }
    }

private:
    QColor TargetColor(int index) const
    {
        return QColor(!isEnabled() || !isTabEnabled(index) ? "#56565f"
                      : index == currentIndex()            ? theme::Text
                      : index == hovered_                  ? "#b9b9c1"
                                                           : "#8b8b94");
    }

    void UpdateColors()
    {
        for (int index = 0; index < count(); ++index) {
            if (!colorAnimations_.contains(index)) {
                auto *a                 = new QVariantAnimation(this);
                colorAnimations_[index] = a;
                connect(a, &QVariantAnimation::valueChanged, this, [this, index](const QVariant &value) {
                    colors_[index] = value.value<QColor>();
                    update();
                });
            }
            motion::Animate(*colorAnimations_[index], colors_.value(index, TargetColor(index)), TargetColor(index), motion::TextMs);
        }
    }

    void SetPress(int index, bool down)
    {
        if (!pressAnimations_.contains(index)) {
            auto *a                 = new QVariantAnimation(this);
            pressAnimations_[index] = a;
            connect(a, &QVariantAnimation::valueChanged, this, [this, index](const QVariant &value) {
                scales_[index] = value.toReal();
                update();
            });
        }
        auto       *a  = pressAnimations_[index];
        const qreal TO = down ? motion::PressScale : 1;
        if (a->endValue().isValid() && a->endValue().toReal() == TO) {
            return;
        }
        motion::Animate(*a, PressScale(index), TO, motion::PressMs);
    }

    void CancelPress()
    {
        if (pressed_ >= 0) {
            SetPress(pressed_, false);
        }
        pressed_ = -1;
    }

    QRectF segmentRect(int index) const
    {
        return index >= 0 && index < count() && isTabVisible(index) ? QRectF(tabRect(index)).adjusted(2, 2, -2, -2) : QRectF{};
    }

    QRectF target() const { return segmentRect(currentIndex()); }

    void snap()
    {
        animation_.stop();
        selected_  = currentIndex();
        indicator_ = target();
        update();
    }

    QVariantAnimation               animation_;
    QRectF                          indicator_;
    QHash<int, QColor>              colors_;
    QHash<int, qreal>               scales_;
    QHash<int, QVariantAnimation *> colorAnimations_, pressAnimations_;
    int                             hovered_ = -1, selected_ = -1, pressed_ = -1;
    bool                            keyboardFocus_ = false;
    int                             segmentHeight_ = 30;
};

class SegmentedTabs final : public QTabWidget
{
public:
    explicit SegmentedTabs(QWidget *parent = nullptr) : QTabWidget(parent) { setTabBar(new SegmentedTabBar(this)); }
};
} // namespace desktop
