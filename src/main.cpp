#include <iostream>
#include <random>

#include "Version.h"
#include "Core/Global.h"
#include "Core/Common.h"

const char *const APP_NAME    = "JX3DPS";
const char *const APP_VERSION = VERSION;

int main(int argc, char *argv[])
{
    std::cout << APP_NAME << std::endl;
    std::cout << APP_VERSION << std::endl;

    // srand(time(0));

    float sum  = 0.0;
    int   sumI = 0;
    for (int i = 0; i < 10000; ++i) {
        sum += Random(0.0, 100.0);
        sumI += Random(0, 1024);
    }
    std::cout << sum << std::endl << sumI << std::endl;
    return 0;
}
