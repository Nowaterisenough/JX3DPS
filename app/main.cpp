/**
 * Project: JX3DPS
 * File: main.cpp
 * Description:
 * Created Date: 2023-07-19 03:39:15
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-08 22:23:35
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include <QApplication>
#include <QFontDatabase>

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>

#include "MainWidget.h"
#include "Widget/Widget.h"

int main(int argc, char *argv[])
{
    // 简化日志设置，先尝试启动应用程序
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
    spdlog::info("Starting JX3DPS Simulator");

    QApplication app(argc, argv);
    spdlog::info("QApplication created successfully");

    int index = QFontDatabase::addApplicationFont(":/resources/NoWatsFont.ttf");
    if (index == -1) {
        spdlog::warn("Failed to load NoWatsFont.ttf, using system font");
    } else {
        QStringList families = QFontDatabase::applicationFontFamilies(index);
        if (!families.isEmpty()) {
            QString str = families.at(0);
            QFont   font(str, 10.5);
            font.setStyleStrategy(QFont::PreferAntialias);
            font.setHintingPreference(QFont::PreferFullHinting);
            qApp->setFont(font);
            spdlog::info("Loaded font: {}", str.toStdString());
        } else {
            spdlog::warn("Font families is empty, using system font");
        }
    }

    spdlog::info("Creating main widget...");
    try {
        // 先试试基础的Widget类
        spdlog::info("Testing base Widget class...");
        Widget baseWidget;
        baseWidget.SetTitle("Base Widget Test");
        spdlog::info("Base Widget created successfully");
        
        baseWidget.show();
        spdlog::info("Base Widget shown, testing JX3DPS Widget...");
        baseWidget.close();
        
        // 现在试试完整的JX3DPS Widget
        JX3DPS::Simulator::Widget widget;
        spdlog::info("JX3DPS Widget created successfully, showing...");
        widget.show();
        spdlog::info("Widget shown, starting event loop...");
    } catch (const std::exception& e) {
        spdlog::error("Exception caught: {}", e.what());
        return -1;
    } catch (...) {
        spdlog::error("Unknown exception caught");
        return -1;
    }

    return app.exec();
}