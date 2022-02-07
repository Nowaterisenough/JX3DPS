/**
 * @Description : JX3DPS APP 主函数
 * @Author      : NoWats
 * @Date        : 2022-01-26 22:28:25
 * @Update      : NoWats
 * @LastTime    : 2022-02-07 00:18:30
 * @FilePath    : \JX3DPS\app\JX3DPS Application\main.cpp
 */

#include <QApplication>

#include "Widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
