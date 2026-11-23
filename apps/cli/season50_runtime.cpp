#include <charconv>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include <nlohmann/json.hpp>

#include "jx3dps.h"

using Json = nlohmann::json;

namespace {
std::string ReadFile(const char *path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::invalid_argument(std::string("cannot open ") + path);
    }
    std::ostringstream data;
    data << file.rdbuf();
    auto text = data.str();
    if (text.starts_with("\xEF\xBB\xBF")) {
        text.erase(0, 3);
    }
    return text;
}

int Positive(std::string_view s)
{
    int n          = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), n);
    if (ec != std::errc{} || ptr != s.data() + s.size() || n < 1) {
        throw std::invalid_argument("expected a positive integer");
    }
    return n;
}

Json Call(jx3dps_engine *h, const Json &request)
{
    auto reply = Json::parse(jx3dps_call(h, request.dump().c_str()));
    if (reply.is_object() && reply.contains("error")) {
        throw std::runtime_error(reply["error"].get<std::string>());
    }
    return reply;
}
} // namespace

int main(int argc, char **argv)
{
    try {
        std::unique_ptr<jx3dps_engine, decltype(&jx3dps_destroy)> handle(jx3dps_create(), jx3dps_destroy);
        if (!handle) {
            throw std::bad_alloc();
        }
        const auto schools = Call(handle.get(),
                                  {
                                      { "op", "schools" }
        });
        if (argc == 2 && std::string_view(argv[1]) == "--list") {
            std::cout << schools.dump(2) << '\n';
            return 0;
        }
#ifdef JX3DPS_CLI_SCHOOL
        const std::string school       = JX3DPS_CLI_SCHOOL;
        constexpr int     first_option = 1;
        const bool        help         = argc > 1 && std::string_view(argv[1]) == "--help";
#else
        const bool        help         = argc < 2 || std::string_view(argv[1]) == "--help";
        const std::string school       = argc >= 2 ? argv[1] : "";
        constexpr int     first_option = 2;
#endif
        if (help) {
            std::cout << argv[0] << " --list | "
#ifndef JX3DPS_CLI_SCHOOL
                      << "SCHOOL "
#endif
                      << "[--macro FILE] [--config FILE] [--seconds N] [--iterations N] [--seed N] [--trace]\n";
            return 0;
        }
        Json request = {
            { "op",         "start" },
            { "scenario",   school  },
            { "seconds",    60      },
            { "iterations", 1       },
            { "seed",       "42"    }
        };
        for (const auto &b : schools) {
            if (b["code"] == school) {
                request["macro"] = b["macro"];
            }
        }
        if (!request.contains("macro")) {
            throw std::invalid_argument("unknown specialization: " + school);
        }
        bool trace = false;
        for (int i = first_option; i < argc; ++i) {
            const std::string_view key = argv[i];
            if (key == "--trace") {
                trace = true;
                continue;
            }
            if (i + 1 == argc) {
                throw std::invalid_argument("missing option value");
            }
            const char *arg = argv[++i];
            if (key == "--macro") {
                request["macro"] = ReadFile(arg);
            } else if (key == "--config") {
                request["config"] = Json::parse(ReadFile(arg));
            } else if (key == "--seconds" || key == "--iterations") {
                request[std::string(key.substr(2))] = Positive(arg);
            } else if (key == "--seed") {
                request["seed"] = arg;
            } else {
                throw std::invalid_argument("unknown option: " + std::string(key));
            }
        }
        Json result;
        if (trace) {
            if (request["iterations"] != 1) {
                throw std::invalid_argument("--trace requires one fight");
            }
            request["debug"]       = true;
            request["breakpoints"] = Json::array();
            auto state             = Call(handle.get(), request);
            while (!state.value("finished", false)) {
                state = Call(handle.get(),
                             {
                                 { "op",     "debug"    },
                                 { "action", "continue" }
                });
            }
            result           = Call(handle.get(),
                                    {
                                        { "op",     "debug" },
                                        { "action", "trace" }
            });
            result["damage"] = state.at("damage");
        } else {
            Call(handle.get(), request);
            do {
                result = Call(handle.get(),
                              {
                                  { "op", "next" }
                });
            } while (!result.contains("skills"));
        }
        result["school"]  = school;
        result["ruleset"] = "cangsheng-50";
        std::cout << result.dump(2) << '\n';
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
