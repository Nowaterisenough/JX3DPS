#pragma once
#include <memory>

#include "src/core/runtime/attribute_gains.hpp"

#include "jx3dps.h"
#include "profile.hpp"
#include "scenarios.hpp"

namespace JX3DPS::engine {
// Native client of the C ABI. No per-hit callbacks cross the ABI.
struct SkillTotal
{
    runtime::Slot skill;
    value_t       damage;
    uint64_t      count, hit, critical, insight, miss;
};

struct BatchResult : runtime::AttributeGainBatch
{
    std::vector<SkillTotal>       skills;
    std::optional<ProfileSummary> profile;
};

using Handle = std::unique_ptr<jx3dps_engine, decltype(&jx3dps_destroy)>;

inline Handle Create()
{
    Handle handle(jx3dps_create(), jx3dps_destroy);
    if (!handle) {
        throw std::bad_alloc();
    }
    return handle;
}

BatchResult Batch(jx3dps_engine                          *handle,
                  const ScenarioConfig                   &config,
                  std::string_view                        macro,
                  const runtime::BatchOptions            &options,
                  bool                                    collectSkills = false,
                  const runtime::AttributeGainIncrements *increments    = nullptr,
                  bool                                    hotspots      = false);
} // namespace JX3DPS::engine
