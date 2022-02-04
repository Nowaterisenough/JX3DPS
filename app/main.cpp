/**
 * @Description : JX3DPS APP 主函数
 * @Author      : NoWats
 * @Date        : 2022-01-26 22:28:25
 * @Update      : NoWats
 * @LastTime    : 2022-02-04 03:23:51
 * @FilePath    : \JX3DPS\app\main.cpp
 */

#pragma execution_character_set("utf-8")

#include <iostream>
#include <random>
#include <regex>

#include "Common/Version.h"
#include "Core/Macro.h"

int main(int argc, char *argv[])
{
    std::cout << JX3DPS::BRANCH << std::endl;
    std::cout << JX3DPS::VERSION << std::endl;

    const std::list<std::string> lines = {
        {"/cast [tnobuff:叠刃|qidian>7&bufftime:碎星辰>30] 无我无剑"},
        {"/cast [tbuff:叠刃>3|nobuff:碎星辰buff] 八荒归元"},
        {"/fcast [nobuff:碎星辰] 碎星辰"},
        {"/cast 三环套月"},
    };
    JX3DPS::Macro::Instance()->Parse(lines);

    // std::string str;
    // std::cin >> str;
    // std::regex reg("([a-z_]+):(.+)(>=|<=|~=|<|>|=)([0-9]+)");

    // std::smatch mat;
    // std::cout << std::boolalpha << str.find("|");
    // if (std::regex_search(str, mat, reg)) {
    //     std::cout << "true ";
    //     std::cout << mat[0];
    // } else {
    //     std::cout << "none " << str;
    // }
    return 0;
}
