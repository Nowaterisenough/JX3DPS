#pragma once
#include <string>

#include "src/core/runtime/batch_profile.hpp"

namespace JX3DPS::engine {
struct ProfileSummary
{
    struct Call
    {
        std::string function;
        uint32_t    parent;
        uint64_t    inclusive, self;
    };

    struct Worker
    {
        uint64_t fights, samplePoints;
    };

    std::vector<Call>                                                   calls;
    std::vector<Worker>                                                 workers;
    uint64_t                                                            fights = 0, samplePoints = 0;
    double                                                              workerMs = 0;
    std::array<double, static_cast<size_t>(runtime::FightPhase::COUNT)> phaseUs{};
};

inline ProfileSummary Summarize(const runtime::BatchProfile &profile)
{
    ProfileSummary                           report;
    runtime::CallTree                        merged;
    decltype(runtime::WorkerProfile::phases) phase_hits{};
    for (const auto &worker : profile.workers) {
        if (worker.error) {
            std::rethrow_exception(worker.error);
        }
        std::vector<std::uint32_t> ids(worker.calls.nodes.size());
        for (std::size_t i = 0; i < ids.size(); ++i) {
            const auto &node = worker.calls.nodes[i];
            if (i != 0u) {
                ids[i] = merged.Child(ids[node.parent], node.function);
            }
            merged.nodes[ids[i]].self += node.self;
        }
        uint64_t points = 0;
        for (std::size_t i = 0; i < phase_hits.size(); ++i) {
            phase_hits[i] += worker.phases[i];
            points        += worker.phases[i];
        }
        report.samplePoints += points;
        report.workers.push_back({ worker.fights, points });
        report.fights   += worker.fights;
        report.workerMs += std::chrono::duration<double, std::milli>(worker.elapsed).count();
    }
    for (const auto &node : merged.nodes) {
        report.calls.push_back({ node.function, node.parent, node.self, node.self });
    }
    for (size_t i = report.calls.size() - 1; i > 0; --i) {
        report.calls[report.calls[i].parent].inclusive += report.calls[i].inclusive;
    }
    if ((report.fights != 0u) && (report.samplePoints != 0u)) {
        const auto US_PER_POINT = report.workerMs * 1000.0 / report.fights / report.samplePoints;
        for (std::size_t i = 0; i < phase_hits.size(); ++i) {
            report.phaseUs[i] = US_PER_POINT * phase_hits[i];
        }
    }
    return report;
}

} // namespace JX3DPS::engine
