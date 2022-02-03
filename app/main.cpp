/**
 * @Description : JX3DPS APP 主函数
 * @Author      : NoWats
 * @Date        : 2022-01-26 22:28:25
 * @Update      : NoWats
 * @LastTime    : 2022-02-03 01:01:58
 * @FilePath    : \JX3DPS\app\main.cpp
 */

#pragma execution_character_set("utf-8")

#include <iostream>
#include <random>
#include <regex>

#include "Version.h"
#include "Global.h"
#include "Common.h"
#include "Attr.h"

int main(int argc, char *argv[])
{
    std::cout << JX3DPS::BRANCH << std::endl;
    std::cout << JX3DPS::VERSION << std::endl;

    std::string str;
    std::cin >> str;
    std::regex reg("([a-z_]+):(.+)(>=|<=|~=|<|>|=)([0-9]+)");

    std::smatch mat;
    std::cout << std::boolalpha << str.find("|");
    if (std::regex_search(str, mat, reg)) {
        std::cout << "true ";
        std::cout << mat[0];
    } else {
        std::cout << "none " << str;
    }
    return 0;
}
