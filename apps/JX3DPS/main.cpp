/**
 * Project: JX3DPS
 * File: main.cpp
 * Description:
 * Created Date: 2023-05-29 17:22:39
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-26 22:17:59
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include <QApplication>
#include <QIcon>

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/spdlog.h>

#include "UI/Widget.h"

int main(int argc, char *argv[])
{
    auto fileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/log.log", 0, 0);
    auto logger = spdlog::default_logger();
    logger->sinks().push_back(fileSink);
    spdlog::flush_on(spdlog::level::info);
    logger->set_level(spdlog::level::info);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");

    QApplication a(argc, argv);

    QFont font("Microsoft YaHei UI", 10.5);
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setHintingPreference(QFont::PreferFullHinting);
    qApp->setFont(font);

    qApp->setWindowIcon(QIcon(":/resources/pics/app_icon.ico"));

    Widget w;
    w.show();
    return a.exec();
}
