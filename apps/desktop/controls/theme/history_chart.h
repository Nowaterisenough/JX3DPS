#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QToolTip>
#include <QWidget>

#include "workbench_theme.h"

namespace desktop {
class HistoryChart final : public QWidget
{
public:
    struct Point
    {
        quint64 id;
        double  dps;
    };

    std::function<void(quint64)> runSelected;

    explicit HistoryChart(QWidget *parent = nullptr) : QWidget(parent)
    {
        setObjectName(QStringLiteral("historyDpsChart"));
        setFixedHeight(310);
        setMouseTracking(true);
        setFocusPolicy(Qt::StrongFocus);
        setAccessibleName(QStringLiteral("模拟历史平均秒伤趋势"));
    }

    void SetPoints(QVector<Point> points)
    {
        points.erase(std::remove_if(points.begin(),
                                    points.end(),
                                    [](const Point &p) { return !std::isfinite(p.dps) || p.dps < 0; }),
                     points.end());
        points_  = std::move(points);
        hovered_ = -1;
        if (SelectedIndex() < 0) {
            selectedId_ = points_.isEmpty() ? 0 : points_.last().id;
        }
        setAccessibleDescription(
            points_.isEmpty()
                ? QStringLiteral("尚无已完成的模拟")
                : QStringLiteral("%1 次运行，最新平均秒伤 %2").arg(points_.size()).arg(points_.last().dps, 0, 'f', 1));
        update();
    }

    void SetSelectedRun(quint64 id)
    {
        selectedId_ = id;
        update();
    }

    void SetReference(std::optional<double> dps)
    {
        reference_ = dps && std::isfinite(*dps) && *dps >= 0 ? dps : std::nullopt;
        update();
    }

    QRectF PlotRect() const { return QRectF(24, 170, qMax(1, width() - 48), height() - 242); }

    QPointF PointPosition(int index) const
    {
        if (index < 0 || index >= points_.size()) {
            return {};
        }
        const auto plot        = PlotRect();
        const auto [LOW, HIGH] = Range();
        return { points_.size() == 1 ? plot.center().x() : plot.left() + (plot.width() * index / (points_.size() - 1)),
                 plot.bottom() - (plot.height() * (points_[index].dps - LOW) / (HIGH - LOW)) };
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(hasFocus() ? theme::AccentColor(this) : QColor(theme::Border), 1));
        p.setBrush(QColor(theme::Surface));
        p.drawRoundedRect(QRectF(rect()).adjusted(.5, .5, -.5, -.5), 14, 14);
        const auto text = [&](QRectF         bounds,
                              const QString &value,
                              int            size,
                              const char    *color,
                              QFont::Weight  weight    = QFont::Normal,
                              int            alignment = Qt::AlignLeft | Qt::AlignVCenter) {
            auto font = this->font();
            font.setPixelSize(size);
            font.setWeight(weight);
            p.setFont(font);
            p.setPen(QColor(color));
            p.drawText(bounds, alignment, value);
        };
        p.setPen(QPen(QColor(theme::Muted), 1.5, Qt::SolidLine, Qt::RoundCap));
        for (int i = 0; i < 4; ++i) {
            p.drawLine(23 + (i * 4), 36, 23 + (i * 4), 28 - ((i % 3) * 3));
        }
        text({ 48, 16, 200, 28 }, QStringLiteral("战斗表现"), 15, theme::Text, QFont::DemiBold);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(theme::Raised));
        p.drawRoundedRect(QRectF(width() - 110, 20, 88, 24), 12, 12);
        text({ qreal(width() - 110), 20, 88, 24 }, QStringLiteral("%1 次记录").arg(points_.size()), 12, theme::Muted, QFont::Normal, Qt::AlignCenter);
        p.setPen(QColor(theme::Border));
        p.drawLine(20, 56, width() - 20, 56);
        text({ 20, 69, 220, 24 }, QStringLiteral("平均秒伤"), 14, theme::Text, QFont::DemiBold);
        if (points_.isEmpty()) {
            text({ 20, 95, qreal(width() - 40), 21 }, QStringLiteral("运行后显示真实战斗结果"), 12, theme::Muted);
            text(PlotRect().adjusted(0, -20, 0, 20), QStringLiteral("尚无模拟记录"), 15, theme::Muted, QFont::Normal, Qt::AlignCenter);
            text({ 20, qreal(height() - 40), qreal(width() - 40), 24 },
                 QStringLiteral("不显示演示数据；取消且未完成的运行不计入曲线。"),
                 12,
                 theme::Muted);
            return;
        }
        const bool LATEST_COMPLETE = hovered_ < 0 && SelectedIndex() < 0;
        const int  inspect         = hovered_ >= 0 ? hovered_ : SelectedIndex();
        const int  ACTIVE          = inspect >= 0 ? inspect : points_.size() - 1;
        const auto point           = points_[ACTIVE];
        text({ 20, 95, 240, 20 },
             (LATEST_COMPLETE ? QStringLiteral("最近完成 · 运行 #%1") : QStringLiteral("运行 #%1 · 已完成")).arg(point.id),
             12,
             theme::Muted);
        text({ 20, 115, qreal(width() / 2), 34 }, QString::number(point.dps, 'f', 1), 27, theme::Text, QFont::DemiBold);
        const double  BASELINE = Reference();
        const QString DELTA =
            BASELINE > 0
                ? QStringLiteral("%1%2%").arg(point.dps >= BASELINE ? "+" : "").arg(((point.dps / BASELINE) - 1) * 100, 0, 'f', 1)
                : QStringLiteral("--");
        text({ qreal(width() / 2), 122, qreal((width() / 2) - 24), 26 },
             QStringLiteral("较基准 %1").arg(DELTA),
             13,
             BASELINE <= 0           ? theme::Muted
             : point.dps >= BASELINE ? theme::Success
                                     : theme::Error,
             QFont::DemiBold,
             Qt::AlignRight | Qt::AlignVCenter);
        const auto   plot = PlotRect();
        QPainterPath line;
        for (int i = 0; i < points_.size(); ++i) {
            if (i == 0) {
                line.moveTo(PointPosition(i));
            } else {
                line.lineTo(PointPosition(i));
            }
        }
        if (points_.size() > 1) {
            auto area = line;
            area.lineTo(plot.bottomRight());
            area.lineTo(plot.bottomLeft());
            area.closeSubpath();
            QLinearGradient fill(plot.topLeft(), plot.bottomLeft());
            fill.setColorAt(0, QColor(240, 128, 120, 44));
            fill.setColorAt(1, QColor(240, 128, 120, 0));
            p.fillPath(area, fill);
            p.setPen(QPen(QColor(theme::Error), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            p.drawPath(line);
        }
        const auto [LOW, HIGH] = Range();
        const qreal BASELINE_Y = plot.bottom() - (plot.height() * (BASELINE - LOW) / (HIGH - LOW));
        QPen        baseline_pen(QColor(111, 217, 155, 160), 1);
        baseline_pen.setDashPattern({ 3, 5 });
        p.setPen(baseline_pen);
        p.drawLine(QPointF(plot.left(), BASELINE_Y), QPointF(plot.right(), BASELINE_Y));
        const auto MARKER = PointPosition(ACTIVE);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(111, 217, 155, 38));
        p.drawEllipse(MARKER, 11, 11);
        p.setPen(QPen(QColor(theme::Surface), 1.5));
        p.setBrush(QColor(theme::Success));
        p.drawEllipse(MARKER, 5, 5);
        const qreal LABEL_LEFT = std::clamp(MARKER.x() - 40, plot.left(), qMax(plot.left(), plot.right() - 80));
        text({ LABEL_LEFT, qMax(150.0, MARKER.y() - 28), 80, 20 },
             hovered_ >= 0     ? QStringLiteral("查看此轮")
             : LATEST_COMPLETE ? QStringLiteral("最近完成")
                               : QStringLiteral("当前运行"),
             11,
             theme::Success,
             QFont::DemiBold,
             Qt::AlignCenter);
        text({ 24, plot.bottom() + 12, 160, 20 }, QStringLiteral("运行 #%1").arg(points_.first().id), 11, theme::Muted);
        text({ qreal(width() - 184), plot.bottom() + 12, 160, 20 },
             QStringLiteral("运行 #%1").arg(points_.last().id),
             11,
             theme::Muted,
             QFont::Normal,
             Qt::AlignRight | Qt::AlignVCenter);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(theme::Raised));
        p.drawRoundedRect(QRectF(20, height() - 36, 154, 24), 12, 12);
        text({ 28, qreal(height() - 36), 138, 24 },
             reference_ ? QStringLiteral("● 使用对比基准") : QStringLiteral("● 首轮作为基准"),
             12,
             theme::Muted);
        text({ 180, qreal(height() - 36), qreal(width() - 204), 24 },
             QStringLiteral("点击曲线查看对应记录"),
             12,
             theme::Muted,
             QFont::Normal,
             Qt::AlignRight | Qt::AlignVCenter);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        hovered_ = IndexAt(event->position());
        if (hovered_ >= 0) {
            const auto &point = points_[hovered_];
            QToolTip::showText(event->globalPosition().toPoint(),
                               QStringLiteral("运行 #%1\n平均秒伤 %2").arg(point.id).arg(point.dps, 0, 'f', 1),
                               this);
        } else {
            QToolTip::hideText();
        }
        update();
    }

    void leaveEvent(QEvent *event) override
    {
        hovered_ = -1;
        QToolTip::hideText();
        update();
        QWidget::leaveEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        const int INDEX = IndexAt(event->position());
        if (event->button() == Qt::LeftButton && INDEX >= 0) {
            Select(INDEX);
        }
        QWidget::mousePressEvent(event);
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (!points_.isEmpty() && (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)) {
            Select(std::clamp(SelectedIndex() + (event->key() == Qt::Key_Left ? -1 : 1), 0, int(points_.size()) - 1));
            event->accept();
        } else {
            QWidget::keyPressEvent(event);
        }
    }

private:
    int SelectedIndex() const
    {
        for (int i = 0; i < points_.size(); ++i) {
            if (points_[i].id == selectedId_) {
                return i;
            }
        }
        return -1;
    }

    void Select(int index)
    {
        selectedId_ = points_[index].id;
        hovered_    = -1;
        if (runSelected) {
            runSelected(selectedId_);
        }
        update();
    }

    int IndexAt(QPointF pos) const
    {
        const auto plot = PlotRect();
        if (points_.isEmpty() || !plot.adjusted(-12, -24, 12, 24).contains(pos)) {
            return -1;
        }
        return std::clamp(qRound((pos.x() - plot.left()) / plot.width() * (points_.size() - 1)), 0, int(points_.size()) - 1);
    }

    double Reference() const { return reference_.value_or(points_.isEmpty() ? 0 : points_.first().dps); }

    std::pair<double, double> Range() const
    {
        double low  = Reference();
        double high = low;
        for (const auto &point : points_) {
            low  = qMin(low, point.dps);
            high = qMax(high, point.dps);
        }
        const double PAD = std::max({ 1.0, (high - low) * .15, high * .025 });
        return { qMax(0.0, low - PAD), high + PAD };
    }

    QVector<Point>        points_;
    std::optional<double> reference_;
    quint64               selectedId_ = 0;
    int                   hovered_    = -1;
};
} // namespace desktop
