/**
 * Project: JX3DPS
 * File: main.cpp
 * Description:
 * Created Date: 2023-07-19 03:39:15
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-13 07:57:26
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

#include "JX3DPSWidget.h"

int main(int argc, char *argv[])
{
    auto fileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/log.log", 0, 0);
    auto logger = spdlog::default_logger();
    logger->sinks().push_back(fileSink);
    spdlog::flush_on(spdlog::level::debug);
    logger->set_level(spdlog::level::info);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");

    QApplication app(argc, argv);

    int     index = QFontDatabase::addApplicationFont(":/resources/NoWatsFont.ttf");
    QString str   = QFontDatabase::applicationFontFamilies(index).at(0);
    QFont   font(str, 10.5);

    font.setStyleStrategy(QFont::PreferAntialias);
    font.setHintingPreference(QFont::PreferFullHinting);
    qApp->setFont(font);

    JX3DPS::Simulator::Widget widget;
    widget.show();

    return app.exec();
}