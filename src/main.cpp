#include <iostream>

#include "Version.h"

const char *const APP_NAME    = "JX3DPS";
const char *const APP_VERSION = VERSION;

int main(int argc, char *argv[])
{
    std::cout << APP_NAME << std::endl;
    std::cout << APP_VERSION << std::endl;
    return 0;
}
