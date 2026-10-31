#pragma once

#include <QEasingCurve>
#include <QPainter>
#include <QPainterPath>
#include <QVariantAnimation>
#include "workbench_theme.h"

namespace desktop::motion {
// Magpie 0.1.1559: app.js THUMB_MS / THUMB_EASE and app.css --ease / :active.
inline constexpr int SlideMs = 280;
inline constexpr int TextMs = 180;
inline constexpr int PressMs = 140;
inline constexpr qreal PressScale = .96;
inline QEasingCurve Ease() {
    QEasingCurve curve(QEasingCurve::BezierSpline);
    curve.addCubicBezierSegment({.2, .8}, {.2, 1}, {1, 1});
    return curve;
}
inline void Animate(QVariantAnimation &animation, const QVariant &from, const QVariant &to, int milliseconds) {
    animation.stop();
    animation.setDuration(milliseconds);
    animation.setEasingCurve(Ease());
    animation.setStartValue(from);
    animation.setEndValue(to);
    animation.start();
}
inline void Thumb(QPainter &p, QRectF rect, bool header = false) {
    const qreal radius = header ? 6 : 5;
    QPainterPath shadow, inside;
    shadow.addRoundedRect(rect.translated(0, 1), radius, radius);
    inside.addRoundedRect(rect, radius, radius);
    p.fillPath(shadow.subtracted(inside), QColor(0, 0, 0, header ? 76 : 31));
    p.setBrush(header ? QColor(255, 255, 255, 38) : QColor(theme::Surface));
    p.setPen(QPen(header ? QColor(255, 255, 255, 20) : QColor(0, 0, 0, 15), .5));
    p.drawRoundedRect(rect, radius, radius);
}
} // namespace desktop::motion
