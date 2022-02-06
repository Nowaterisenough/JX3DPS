/**
 * @Description :
 * @Author      : NoWats
 * @Date        : 2022-02-06 20:22:30
 * @Update      : NoWats
 * @LastTime    : 2022-02-07 00:06:39
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
        std::cout << "SimulatorImpl::Init() succeed." << std::endl;
    }

private:
    SimulatorImpl()
    {
        std::cout << "VERSION: " << JX3DPS::VERSION << std::endl;
        std::cout << "BRANCH: " << JX3DPS::BRANCH << std::endl;
    }
};

namespace Simulator {

void Init()
{
    SimulatorImpl::Instance()->Init();
}

} // namespace Simulator


} // namespace JX3DPS
