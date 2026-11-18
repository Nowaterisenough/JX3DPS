#pragma once

#include <QColor>
#include <QImage>
#include <QVariant>
#include <QWidget>
#include <algorithm>
#include <array>
#include <cmath>

namespace desktop::theme {
inline QColor DefaultAccent() { return QColor("#b6b6c0"); }
inline QColor AccentText() { return QColor("#171722"); }

inline double Luminance(const QColor &color) {
    auto linear = [](double channel) { return channel <= .04045 ? channel / 12.92 : std::pow((channel + .055) / 1.055, 2.4); };
    return .2126 * linear(color.redF()) + .7152 * linear(color.greenF()) + .0722 * linear(color.blueF());
}
inline double Contrast(const QColor &a, const QColor &b) {
    const auto x = Luminance(a), y = Luminance(b);
    return (std::max(x, y) + .05) / (std::min(x, y) + .05);
}
inline QColor ReadableAccent(QColor color, double minimum = 4.5) {
    if (!color.isValid()) color = DefaultAccent();
    auto lightness = color.lightnessF();
    while (Contrast(color, AccentText()) < minimum && lightness < .95) {
        lightness = std::min(.95, lightness + .01);
        color = QColor::fromHslF(color.hslHueF(), color.hslSaturationF(), lightness);
    }
    return color.toRgb();
}
inline QColor Blend(const QColor &a, const QColor &b, double amount) {
    return QColor::fromRgbF(a.redF() * (1 - amount) + b.redF() * amount,
        a.greenF() * (1 - amount) + b.greenF() * amount,
        a.blueF() * (1 - amount) + b.blueF() * amount);
}
struct AccentColors { QColor normal, hover, pressed, text; };
inline AccentColors MakeAccentColors(const QColor &accent) {
    const auto normal = ReadableAccent(accent);
    return {normal, Blend(normal, Qt::white, .12), ReadableAccent(Blend(normal, Qt::black, .08)), AccentText()};
}

inline QColor IconAccent(const QImage &source) {
    if (source.isNull()) return DefaultAccent();
    const auto image = source.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32);
    std::array<double, 36> hues{};
    // Match the displayed artwork crop, and ignore transparent frames, gray pixels and dark shadows.
    auto sample = [&](auto consume) {
        const int dx = image.width() / 16, dy = image.height() / 16;
        for (int y = dy; y < image.height() - dy; ++y) {
            for (int x = dx; x < image.width() - dx; ++x) {
                const auto color = image.pixelColor(x, y);
                if (color.alphaF() < .6 || color.hsvSaturationF() < .25 || color.valueF() < .16) continue;
                const double hue = color.hsvHueF() * 360;
                const double weight = color.alphaF() * color.hsvSaturationF() * (.25 + .75 * color.valueF());
                consume(color, hue, weight);
            }
        }
    };
    sample([&](const QColor &, double hue, double weight) { hues[std::min(35, int(hue / 10))] += weight; });
    int peak = 0;
    double best = 0;
    for (int i = 0; i < 36; ++i) {
        const auto score = hues[(i + 35) % 36] + hues[i] + hues[(i + 1) % 36];
        if (score > best) { peak = i; best = score; }
    }
    if (best == 0) return DefaultAccent();
    double red = 0, green = 0, blue = 0, total = 0;
    sample([&](const QColor &color, double hue, double weight) {
        const auto distance = std::abs(hue - (peak * 10 + 5));
        if (std::min(distance, 360 - distance) > 20) return;
        red += color.redF() * weight; green += color.greenF() * weight; blue += color.blueF() * weight; total += weight;
    });
    if (total == 0) return DefaultAccent();
    const auto dominant = QColor::fromRgbF(red / total, green / total, blue / total);
    const auto accent = QColor::fromHslF(dominant.hslHueF(), std::clamp(double(dominant.hslSaturationF()), .35, .8),
        std::clamp(double(dominant.lightnessF()), .60, .72));
    // Use the same 8-bit color for QSS, native palettes and custom painters.
    return QColor(ReadableAccent(accent, 5.5).name());
}

inline QColor AccentColor(const QWidget *widget) {
    // Parent lookup also covers native child dialogs/popups, without changing other workbench windows.
    for (auto *owner = widget; owner; owner = owner->parentWidget()) {
        const auto value = owner->property("workbenchAccent");
        if (value.canConvert<QColor>()) {
            const auto color = value.value<QColor>();
            if (color.isValid()) return color;
        }
    }
    return DefaultAccent();
}
} // namespace desktop::theme
