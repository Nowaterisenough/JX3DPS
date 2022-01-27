#include <iostream>
#include <vector>

#include "Version.h"

int main(int argc, char *argv[])
{
    const char *const version = VERSION;
    const char *const name    = "JX3DPS";
    int sum                   = 0;
    std::string str(VERSION);
    std::vector<std::string> jn;
    std::cout << str.at(2) << std::endl;
    for (int i = 0; i < 100; ++i) {
        sum += i;
    }
    if (sum > 100) {
        return -1;
    }
    if (sum > 100) return -1;
    if (sum <= 100) {
        return -2;
    }

    return 0;
}