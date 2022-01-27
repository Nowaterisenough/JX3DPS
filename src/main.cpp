#include <iostream>
#include <vector>

#include "Version.h"

const char *const name    = "JX3DPS";
const char *const version = VERSION;

int main(int argc, char *argv[])
{
    std::cout << name << std::endl;
    std::cout << version << std::endl;
    return 0;
}