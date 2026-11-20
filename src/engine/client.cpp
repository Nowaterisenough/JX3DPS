#include "client.hpp"

#include <string>

#include "config_json.hpp"

namespace JX3DPS::engine {
using Json = nlohmann::json;

static runtime::BatchStats ReadStats(const Json &j)
{
    runtime::BatchStats s;
    j.at("count").get_to(s.count);
    j.at("mean_damage").get_to(s.mean_damage);
    j.at("m2_damage").get_to(s.m2_damage);
    j.at("min_damage").get_to(s.min_damage);
    j.at("max_damage").get_to(s.max_damage);
    s.checksum = std::stoull(j.at("checksum").get<std::string>());
    return s;
}

BatchResult Batch(jx3dps_engine                          *handle,
                  const ScenarioConfig                   &config,
                  std::string_view                        macro,
                  const runtime::BatchOptions            &o,
                  bool                                    collect,
                  const runtime::AttributeGainIncrements *increments,
                  bool                                    hotspots)
{
    Json request = {
        { "op",               "batch"                           },
        { "scenario",         config.index() == 0 ? "tx" : "mw" },
        { "macro",            macro                             },
        { "iterations",       o.iterations                      },
        { "frames",           o.duration                        },
        { "workers",          o.workers                         },
        { "seed",             std::to_string(o.seed)            },
        { "damageCapacity",   o.damage_capacity                 },
        { "mutationCapacity", o.mutation_capacity               },
        { "collectSkills",    collect                           },
        { "hotspots",         hotspots                          }
    };
    std::visit([&](const auto &cfg) { request["config"] = cfg; }, config);
    if (increments != nullptr) {
        request["increments"] = *increments;
    }
    const auto RESPONSE = Json::parse(jx3dps_call(handle, request.dump().c_str()));
    if (RESPONSE.contains("error")) {
        throw std::runtime_error(RESPONSE.at("error").get<std::string>());
    }
    BatchResult result;
    result.primary = ReadStats(RESPONSE.at("stats"));
    if (increments != nullptr) {
        for (size_t i = 0; i < result.gains.size(); ++i) {
            const auto &gain = RESPONSE.at("gains").at(i);
            if (!gain.is_null()) {
                result.gains[i] = ReadStats(gain);
            }
        }
    }
    for (const auto &s : RESPONSE.at("skills")) {
        result.skills.push_back(
            { s.at("skill"), std::stoll(s.at("damage").get<std::string>()), s.at("count"), s.at("hit"), s.at("critical"), s.at("insight"), s.at("miss") });
    }
    if (RESPONSE.contains("profile")) {
        const auto &p       = RESPONSE.at("profile");
        auto       &summary = result.profile.emplace();
        p.at("fights").get_to(summary.fights);
        p.at("samples").get_to(summary.samplePoints);
        p.at("workerMs").get_to(summary.workerMs);
        p.at("phaseUs").get_to(summary.phaseUs);
        for (const auto &w : p.at("workers")) {
            summary.workers.push_back({ w.at(0), w.at(1) });
        }
        for (const auto &c : p.at("calls")) {
            summary.calls.push_back({ c.at(0), c.at(1), c.at(2), c.at(3) });
        }
    }
    return result;
}
} // namespace JX3DPS::engine
