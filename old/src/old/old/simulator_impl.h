#ifndef JX3DPS_SIMULATOR_IMPL_H
#define JX3DPS_SIMULATOR_IMPL_H

#include "error_code.h"
#include "json.h"

namespace JX3DPS {

class SimulatorImpl
{
public:
    virtual ~SimulatorImpl() = default;

    SimulatorImpl(const SimulatorImpl &)            = delete;
    SimulatorImpl &operator=(const SimulatorImpl &) = delete;

    static SimulatorImpl *GetInstance()
    {
        static SimulatorImpl instance;
        return &instance;
    }

    inline static double progress = 0.0;
    inline static json   result;

    err_t SimulateAsync(const char *const in);

    err_t SimulateSync(const char *const in, char *out);

    err_t SimulateSyncWithProgressInfo(const char *const in, char *out, void (*progress)(double));
                            
private:
    SimulatorImpl() = default;

    void UpdateProgress(double progress);

    void (*_progress)(double) = [](double) {
    };
};

} // namespace JX3DPS

#endif // JX3DPS_SIMULATOR_IMPL_H