#include "time_line.h"

using namespace JX3DPS;

TimeLine &TimeLine::Instance()
{
    static TimeLine instance;
    return instance;
}