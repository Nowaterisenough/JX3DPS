/**
 * @Description : 宏解析
 * @Author      : NoWats
 * @Date        : 2022-02-02 16:30:03
 * @Update      : NoWats
 * @LastTime    : 2022-02-02 22:34:35
 * @FilePath    : \JX3DPS\modules\Core\Macro.h
 */

#ifndef MACRO_H
#define MACRO_H

#include "Global.h"

namespace JX3DPS {

using CastPtr = int;

class Macro
{
public:
    ~Macro();

    Macro(const Macro &macro) = delete;
    Macro &operator=(const Macro &macro) = delete;

    static Macro *Instance();

    int Parse(const std::list<std::string> &lines);

    int Parse(const std::string &line);

private:
    bool MacroStrToFuncPtr(const std::string &str);

    Macro();
};

} // namespace JX3DPS

#endif // MACRO_H
