/**
 * Project: JX3DPS
 * File: main.cpp
 * Description:
 * Created Date: 2023-07-19 03:39:15
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-09 03:25:04
 * Modified By: 难为水
 * -----
 * CHANGELOG:
 * Date      	By     	Comments
 * ----------	-------	----------------------------------------------------------
 */

#include <QApplication>
#include <QFontDatabase>

#include "TestUI.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QApplication::font().setStyleStrategy(QFont::PreferAntialias);
    QApplication::font().setHintingPreference(QFont::PreferFullHinting);

    TestUI widget;
    widget.show();

    return app.exec();
}