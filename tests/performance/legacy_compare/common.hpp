#pragma once
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <nlohmann/json.hpp>

using Clock = std::chrono::steady_clock;
using Json = nlohmann::ordered_json;
struct Arguments {
    int iterations = 1000, seconds = 300, precision = 16, warmup = 5, crit = 20000;
    int phase = -1, initial_qidian = 10;
    std::string macro_path, trace_path, draws_path;
    std::vector<std::string> talents;
    bool dot = false, weapon_cw = false;
    Arguments(int argc, char **argv) {
        for (int i = 1; i < argc; ++i) {
            const std::string key = argv[i];
            if (i + 1 == argc) throw std::invalid_argument("missing argument");
            const std::string value = argv[++i];
            if (key == "--macro") macro_path = value;
            else if (key == "--trace") trace_path = value;
            else if (key == "--draws") draws_path = value;
            else if (key == "--iterations") iterations = std::stoi(value);
            else if (key == "--seconds") seconds = std::stoi(value);
            else if (key == "--precision") precision = std::stoi(value);
            else if (key == "--warmup") warmup = std::stoi(value);
            else if (key == "--crit") crit = std::stoi(value);
            else if (key == "--phase") phase = std::stoi(value);
            else if (key == "--initial-qidian") initial_qidian = std::stoi(value);
            else if (key == "--dot") dot = std::stoi(value) != 0;
            else if (key == "--weapon-cw") weapon_cw = std::stoi(value) != 0;
            else if (key == "--talents") {
                std::istringstream names(value);
                for (std::string name; std::getline(names, name, ','); ) talents.push_back(name);
            }
            else throw std::invalid_argument("unknown argument: " + key);
        }
        if (iterations <= 0 || seconds <= 0 || seconds > 86400 || precision <= 0 || warmup < 0 || macro_path.empty() ||
            phase < -1 || phase > 15 || initial_qidian < 0 || initial_qidian > 10)
            throw std::invalid_argument("invalid benchmark configuration");
        if (!trace_path.empty() && iterations != 1) throw std::invalid_argument("trace requires one fight");
#if defined(LEGACY_DRAW_RECORD) || defined(RUNTIME_DRAW_REPLAY)
        if (draws_path.empty() || trace_path.empty() || iterations != 1 || warmup != 0 || phase < 0 || precision != 1)
            throw std::invalid_argument("draw validation requires --draws, --trace, one fight, zero warmup, fixed phase and precision 1");
#else
        if (!draws_path.empty()) throw std::invalid_argument("use the separate draw validation executable");
#endif
    }
    std::string Macro() const {
        std::ifstream input(macro_path, std::ios::binary);
        if (!input) throw std::runtime_error("cannot read macro");
        std::string result(std::istreambuf_iterator<char>(input), {});
        if (result.starts_with("\xef\xbb\xbf")) result.erase(0, 3);
        return result;
    }
};
struct Totals {
    long long sum = 0, min = std::numeric_limits<long long>::max(), max = 0;
    std::uint64_t count = 0;
    void Add(long long damage) { sum += damage; min = std::min(min, damage); max = std::max(max, damage); ++count; }
    Json Result(const char *engine, double elapsed, const Arguments &args, const Json &sample) const {
        return Json{{"engine", engine}, {"iterations", count}, {"seconds", args.seconds},
            {"elapsed_s", elapsed}, {"fights_per_s", count / elapsed}, {"mean_damage", double(sum) / count},
            {"mean_dps", double(sum) / count / args.seconds}, {"min_damage", min}, {"max_damage", max},
            {"damage_sum", sum}, {"crit", args.crit}, {"dot", args.dot}, {"legacy_precision", args.precision},
            {"phase", args.phase}, {"initial_qidian", args.initial_qidian}, {"talents", args.talents},
            {"weapon_cw", args.weapon_cw}, {"sample", sample}};
    }
};
