#include "src/core/runtime.hpp"
#include "src/core/runtime/batch.hpp"
#include "fixture_rules.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>

using namespace JX3DPS;
using namespace JX3DPS::runtime;

struct Result { double milliseconds; std::uint64_t checksum; };

template <bool Debug>
Result Measure(const MacroProgram &program, int passes) {
    MacroExecutor<Debug> executor(program);
    State state;
    state.Resize(program.skill_count, program.buff_count);
    std::uint64_t checksum = 0;
    auto action = [](State &s, const MacroLine &line) {
        if (s.skill_ready_at[line.action_slot] > s.now) return false;
        s.skill_ready_at[line.action_slot] = s.now + 4 + line.action_slot;
        s.energy += line.action_slot + 1;
        return true;
    };
    const auto begin = std::chrono::steady_clock::now();
    for (int i = 0; i < passes; ++i) {
        state.now = i;
        state.qidian = i % 11;
        state.rage = i % 5;
        state.self_buff_stacks[0] = i % 3;
        state.target_buff_stacks[0] = i % 7;
        if constexpr (Debug) {
            executor.BeginPass();
            while (executor.Step(state, action)) {}
        } else {
            executor.EvaluateFast(state, action);
        }
        checksum += state.last_skill + state.energy;
    }
    const auto end = std::chrono::steady_clock::now();
    return {std::chrono::duration<double, std::milli>(end - begin).count(), checksum};
}

int main(int argc, char **argv) {
    const int passes = argc > 1 ? std::atoi(argv[1]) : 1000000;
    if (passes <= 0 || passes > 10000000) return 2;
    CompileOptions options;
    options.keep_debug_info = false;
    auto [macro, errors] = MacroCompiler::Compile(
        "/cast [qidian>7&buff:power] strike\n"
        "/cast [tbuff:power>=4|rage>3&qidian>5] burst\n"
        "/cast [skill_cd:strike<0.125&qidian>1] filler\n"
        "/cast fallback", options);
    if (!errors.empty()) return 3;
    Measure<false>(macro.program, 10000);
    Measure<true>(macro.program, 10000);
    std::cout << "macro passes per sample=" << passes << " (not full combat simulations)\n";
    std::cout << "instruction_bytes=" << sizeof(MacroLine) << " condition_bytes=" << sizeof(Condition)
              << " fast_executor_bytes=" << sizeof(FastMacroExecutor)
              << " debug_executor_bytes=" << sizeof(DebugMacroExecutor) << '\n';
    for (int sample = 0; sample < 5; ++sample) {
        Result fast{}, debug{};
        if (sample % 2 == 0) { fast = Measure<false>(macro.program, passes); debug = Measure<true>(macro.program, passes); }
        else { debug = Measure<true>(macro.program, passes); fast = Measure<false>(macro.program, passes); }
        if (fast.checksum != debug.checksum) return 4;
        std::cout << "sample=" << sample << " fast_ms=" << fast.milliseconds
                  << " debug_ms=" << debug.milliseconds << " checksum=" << fast.checksum << '\n';
    }
    if (argc > 2) {
        const int iterations = std::atoi(argv[2]);
        if (iterations <= 0 || iterations > 1000000) return 5;
        auto [fight, fight_errors] = MacroCompiler::Compile(
            "/cast [qidian>=6&buff:power] strike\n/cast [skill_cd:burst=0] burst\n/cast filler", options);
        if (!fight_errors.empty()) return 6;
        BatchOptions batch;
        batch.iterations = static_cast<std::uint64_t>(iterations);
        batch.seed = 42;
        std::uint64_t checksum = 0;
        for (auto workers : {1u, 4u}) {
            batch.workers = workers;
            const auto begin = std::chrono::steady_clock::now();
            const auto result = RunBatch<BenchmarkRules>(fight.program, batch);
            const auto end = std::chrono::steady_clock::now();
            if (workers == 1) checksum = result.checksum;
            else if (result.checksum != checksum) return 7;
            std::cout << "synthetic_fights=" << iterations << " duration_frames=" << batch.duration
                      << " workers=" << workers << " total_ms="
                      << std::chrono::duration<double, std::milli>(end - begin).count()
                      << " mean_damage=" << result.mean_damage << " checksum=" << result.checksum << '\n';
        }
    }
}
