#include "dark_style.h"

#include <QPalette>
#include <QWidget>
#include <QApplication>
#include <QFont>
#include <QPainter>
#include <QStyleOption>
#include "workbench_theme.h"

DarkStyle::DarkStyle(QStyle *style) : QProxyStyle(style) { }

void DarkStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                              const QWidget *widget) const
{
    const bool up = element == PE_IndicatorArrowUp || element == PE_IndicatorSpinUp;
    const bool down = element == PE_IndicatorArrowDown || element == PE_IndicatorSpinDown;
    if (!up && !down) {
        QProxyStyle::drawPrimitive(element, option, painter, widget);
        return;
    }
    const auto center = QRectF(option->rect).center();
    const auto color = option->state & State_Enabled ? QColor(desktop::theme::Muted) : QColor("#777e8d");
    const qreal direction = up ? -1.0 : 1.0;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(color, 1.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setBrush(Qt::NoBrush);
    painter->drawPolyline(QPolygonF{QPointF(center.x() - 2.5, center.y() - direction),
                                    QPointF(center.x(), center.y() + direction * 1.5),
                                    QPointF(center.x() + 2.5, center.y() - direction)});
    painter->restore();
}

void DarkStyle::polish(QPalette &palette)
{
    namespace theme = desktop::theme;
    palette.setColor(QPalette::Window, QColor(theme::Canvas));
    palette.setColor(QPalette::WindowText, QColor(theme::Text));
    palette.setColor(QPalette::Base, QColor(theme::Field));
    palette.setColor(QPalette::AlternateBase, QColor(theme::Surface));
    palette.setColor(QPalette::ToolTipBase, QColor(theme::Raised));
    palette.setColor(QPalette::ToolTipText, QColor(theme::Text));
    palette.setColor(QPalette::Text, QColor(theme::Text));
    palette.setColor(QPalette::PlaceholderText, QColor(theme::Muted));
    palette.setColor(QPalette::Button, QColor(theme::Raised));
    palette.setColor(QPalette::ButtonText, QColor(theme::Text));
    palette.setColor(QPalette::BrightText, QColor("#ffffff"));
    palette.setColor(QPalette::Link, theme::DefaultAccent());
    palette.setColor(QPalette::Accent, theme::DefaultAccent());
    palette.setColor(QPalette::Highlight, QColor(theme::Selection));
    palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    for (const auto role : {QPalette::WindowText, QPalette::Text, QPalette::ButtonText})
        palette.setColor(QPalette::Disabled, role, QColor("#8a909e"));

    QProxyStyle::polish(palette);
}

void DarkStyle::polish(QWidget *widget)
{
    QProxyStyle::polish(widget);
}

void DarkStyle::polish(QApplication *app)
{
    QProxyStyle::polish(app);

    auto font = app->font();
    font.setFamily(QStringLiteral("Microsoft YaHei UI"));
    font.setWeight(QFont::Normal);
    font.setPixelSize(14);
    app->setFont(font);

    // 设置全局样式表，包括 ToolTip 样式
    app->setStyleSheet(app->styleSheet() + R"(
        QToolTip {
            border: 1px solid #333339;
            border-radius: 6px;
            background-color: #232327;
            color: #ededf1;
            padding: 7px 9px;
            font-size: 12px;
        }
    )");
}
