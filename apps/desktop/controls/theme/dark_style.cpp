#include "dark_style.h"

#include <QPalette>
#include <QWidget>
#include <QApplication>

DarkStyle::DarkStyle(QStyle *style) : QProxyStyle(style) { }

void DarkStyle::polish(QPalette &palette)
{
    // 深黑蓝色主题
    QColor darkBlue(10, 14, 20);        // #0a0e1a
    QColor lightText(224, 224, 224);    // #e0e0e0
    QColor brightText(255, 255, 255);   // #ffffff
    QColor darkBlueLight(20, 28, 52);   // 稍亮的深蓝色用于高亮
    QColor darkBlueDark(5, 7, 13);      // 更暗的深蓝色用于阴影

    palette.setColor(QPalette::Window, darkBlue);
    palette.setColor(QPalette::WindowText, lightText);
    palette.setColor(QPalette::Base, darkBlueDark);
    palette.setColor(QPalette::AlternateBase, darkBlue);
    palette.setColor(QPalette::ToolTipBase, darkBlueLight);
    palette.setColor(QPalette::ToolTipText, lightText);
    palette.setColor(QPalette::Text, lightText);
    palette.setColor(QPalette::Button, darkBlue);
    palette.setColor(QPalette::ButtonText, lightText);
    palette.setColor(QPalette::BrightText, brightText);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, darkBlueLight);
    palette.setColor(QPalette::HighlightedText, brightText);

    QProxyStyle::polish(palette);
}

void DarkStyle::polish(QWidget *widget)
{
    QProxyStyle::polish(widget);
}

void DarkStyle::polish(QApplication *app)
{
    QProxyStyle::polish(app);

    // 设置全局样式表，包括 ToolTip 样式
    app->setStyleSheet(app->styleSheet() + R"(
        QToolTip {
            border: 1px solid #454545;
            background-color: #2d2d2d;
            color: #cccccc;
            padding: 4px;
            font-family: "Microsoft YaHei";
            font-size: 10pt;
        }
    )");
}
