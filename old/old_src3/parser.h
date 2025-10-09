#ifndef JX3DPS_PARSER_H
#define JX3DPS_PARSER_H

#include "error_code.h"
#include "type.h"

namespace JX3DPS {

template <typename T>
class Parser
{
public:
    Parser()          = default;
    virtual ~Parser() = default;

    virtual err_t Parse(const char *const in, T &out) = 0;
};

class Macro : public Parser<Macro>
{
public:
    Macro()          = default;
    virtual ~Macro() = default;

    err_t Parse(const char *const in, Macro &out) override final;
};

err_t Func(const char *const in, Macro &out)
{
    
    return ErrorCode::SUCCESS;
}

} // namespace JX3DPS

#endif // JX3DPS_PARSER_H