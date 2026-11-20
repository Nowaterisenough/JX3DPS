#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include <QColor>
#include <QImage>
#include <QVariant>
#include <QWidget>

namespace desktop::theme {
inline QColor DefaultAccent()
{
    return QColor("#b6b6c0");
}

inline QColor AccentText()
{
    return QColor("#171722");
}

inline double Luminance(const QColor &color)
{
    const auto LINEAR = [](double channel) {
        return channel <= .04045 ? channel / 12.92 : std::pow((channel + .055) / 1.055, 2.4);
    };
    return (.2126 * LINEAR(color.redF())) + (.7152 * LINEAR(color.greenF())) + (.0722 * LINEAR(color.blueF()));
}

inline double Contrast(const QColor &a, const QColor &b)
{
    const auto X = Luminance(a);
    const auto Y = Luminance(b);
    return (std::max(X, Y) + .05) / (std::min(X, Y) + .05);
}

inline QColor ReadableAccent(QColor color, double minimum = 4.5)
{
    if (!color.isValid()) {
        color = DefaultAccent();
    }
    auto lightness = color.lightnessF();
    while (Contrast(color, AccentText()) < minimum && lightness < .95) {
        lightness = std::min(.95, lightness + .01);
        color     = QColor::fromHslF(color.hslHueF(), color.hslSaturationF(), lightness);
    }
    return color.toRgb();
}

inline QColor Blend(const QColor &a, const QColor &b, double amount)
{
    return QColor::fromRgbF((a.redF() * (1 - amount)) + (b.redF() * amount),
                            (a.greenF() * (1 - amount)) + (b.greenF() * amount),
                            (a.blueF() * (1 - amount)) + (b.blueF() * amount));
}

struct AccentColors
{
    QColor normal, hover, pressed, text;
};

inline AccentColors MakeAccentColors(const QColor &accent)
{
    const auto NORMAL = ReadableAccent(accent);
    return { NORMAL, Blend(NORMAL, Qt::white, .12), ReadableAccent(Blend(NORMAL, Qt::black, .08)), AccentText() };
}

inline QColor IconAccent(const QImage &source)
{
    if (source.isNull()) {
        return DefaultAccent();
    }
    const auto IMAGE = source.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32);
    std::array<double, 36> hues{};
    // Match the displayed artwork crop, and ignore transparent frames, gray pixels and dark shadows.
    const auto             SAMPLE = [&](auto consume) {
        const int DX = IMAGE.width() / 16;
        const int DY = IMAGE.height() / 16;
        for (int y = DY; y < IMAGE.height() - DY; ++y) {
            for (int x = DX; x < IMAGE.width() - DX; ++x) {
                const auto COLOR = IMAGE.pixelColor(x, y);
                if (COLOR.alphaF() < .6 || COLOR.hsvSaturationF() < .25 || COLOR.valueF() < .16) {
                    continue;
                }
                const double HUE    = COLOR.hsvHueF() * 360;
                const double weight = COLOR.alphaF() * COLOR.hsvSaturationF() * (.25 + (.75 * COLOR.valueF()));
                consume(COLOR, HUE, weight);
            }
        }
    };
    SAMPLE([&](const QColor &, double hue, double weight) { hues[std::min(35, int(hue / 10))] += weight; });
    int    peak = 0;
    double best = 0;
    for (int i = 0; i < 36; ++i) {
        const auto SCORE = hues[(i + 35) % 36] + hues[i] + hues[(i + 1) % 36];
        if (SCORE > best) {
            peak = i;
            best = SCORE;
        }
    }
    if (best == 0) {
        return DefaultAccent();
    }
    double red   = 0;
    double green = 0;
    double blue  = 0;
    double total = 0;
    SAMPLE([&](const QColor &color, double hue, double weight) {
        const auto DISTANCE = std::abs(hue - ((peak * 10) + 5));
        if (std::min(DISTANCE, 360 - DISTANCE) > 20) {
            return;
        }
        red   += color.redF() * weight;
        green += color.greenF() * weight;
        blue  += color.blueF() * weight;
        total += weight;
    });
    if (total == 0) {
        return DefaultAccent();
    }
    const auto dominant = QColor::fromRgbF(red / total, green / total, blue / total);
    const auto accent   = QColor::fromHslF(dominant.hslHueF(),
                                           std::clamp(double(dominant.hslSaturationF()), .35, .8),
                                           std::clamp(double(dominant.lightnessF()), .60, .72));
    // Use the same 8-bit color for QSS, native palettes and custom painters.
    return QColor(ReadableAccent(accent, 5.5).name());
}

inline QColor AccentColor(const QWidget *widget)
{
    // Parent lookup also covers native child dialogs/popups, without changing other workbench windows.
    for (const auto *owner = widget; owner != nullptr; owner = owner->parentWidget()) {
        const auto value = owner->property("workbenchAccent");
        if (value.canConvert<QColor>()) {
            const auto COLOR = value.value<QColor>();
            if (COLOR.isValid()) {
                return COLOR;
            }
        }
    }
    return DefaultAccent();
}
} // namespace desktop::theme
