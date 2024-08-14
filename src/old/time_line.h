#ifndef JX3DPS_TIME_LINE_H
#define JX3DPS_TIME_LINE_H

#include "type.h"

namespace JX3DPS {

class TimeLine
{
public:
    ~TimeLine() = default;

    static TimeLine &Instance();

    TimeLine(const TimeLine &)            = delete;
    TimeLine &operator=(const TimeLine &) = delete;

private:
    TimeLine() = default;

    frame_t _frame = 0;
};

} // namespace JX3DPS

#endif // JX3DPS_TIME_LINE_H