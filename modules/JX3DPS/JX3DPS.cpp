/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-06 20:22:30
 * @Update      : NoWats
 * @LastTime    : 2022-02-08 23:52:35
 * @FilePath    : \JX3DPS\modules\JX3DPS\JX3DPS.cpp
 */

#include "JX3DPS.h"

#include <iostream>

#include "Common/Version.h"

namespace JX3DPS {

class SimulatorImpl
{
public:
    static SimulatorImpl *Instance()
    {
        static SimulatorImpl instance;
        return &instance;
    }

    void Init()
    {
        // std::cout << "SimulatorImpl::Init() succeed." << std::endl;
    }

    const char *Name() const
    {
        return JX3DPS::NAME;
    }

    const char *Version() const
    {
        return JX3DPS::VERSION;
    }

    const char *Branch() const
    {
        return JX3DPS::BRANCH;
    }

private:
    SimulatorImpl()
    {
        std::cout << JX3DPS::NAME << std::endl;
        std::cout << "VERSION: " << JX3DPS::VERSION << std::endl;
        std::cout << "BRANCH: " << JX3DPS::BRANCH << std::endl;
    }
};

const char *Name()
{
    return SimulatorImpl::Instance()->Name();
}

const char *Version()
{
    return SimulatorImpl::Instance()->Version();
}

const char *Branch()
{
    return SimulatorImpl::Instance()->Branch();
}

namespace Simulator {

void Init()
{
    SimulatorImpl::Instance()->Init();
}

} // namespace Simulator

} // namespace JX3DPS
