#include "SimulatorImpl.h"

#include "Log.h"

using namespace JX3DPS;

err_t SimulatorImpl::SimulateAsync(const char *const in)
{
    Log::Info("Simulate async. {}", in);
    UpdateProgress(1.0);
    return SUCCESS;
}

err_t SimulatorImpl::SimulateSync(const char *const in, char *out)
{
    Log::Info("Simulate sync. ");
    UpdateProgress(1.0);
    return SUCCESS;
}

err_t SimulatorImpl::SimulateSyncWithProgressInfo(const char *const in, char *out, void (*progress)(double))
{
    _progress = progress;
    Log::Info("Simulate sync with progress info. ");
    UpdateProgress(1.0);
    return SUCCESS;
}

void SimulatorImpl::UpdateProgress(double progress)
{
    this->progress = progress;
    _progress(progress);
}
