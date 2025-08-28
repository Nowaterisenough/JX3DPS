#include <random>
#include <vector>

#include <benchmark/benchmark.h>

using Value_t  = long long;
using PctInt_t = int;

const PctInt_t JX3_PERCENT_INT_BASE                          = 1024;
const double   JX3_PERCENT_FLOAT_BASE                        = 1.0;
const double   JX3_CRITICAL_STRIKE_POWER_PARAM               = 3.335;
const PctInt_t JX3_LEVEL_PARAM                               = 450;
const PctInt_t JX3_LEVEL_CONST                               = 45750;
const double   JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE = 1.75;

enum Roll
{
    HIT    = 0,   // 命中
    DOUBLE = 1,   // 会心
    SIGHT  = 4,   // 识破
    MISS   = 126, // 偏离
};

// roll: 0 为命中, 1 为会心, 4为识破, 126为偏离
inline Value_t CalculateDamageBitwise(Value_t overcomeDamage, int roll, Value_t criticalStrikePower, PctInt_t effectCriticalStrikePowerPercentInt)
{
    PctInt_t criticalStrikePowerPercentInt =
        criticalStrikePower * JX3_PERCENT_INT_BASE /
        (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * 120 - JX3_LEVEL_CONST));

    Value_t temp = overcomeDamage >> (roll / 2);

    return static_cast<Value_t>(
        temp + temp * (roll & 1) * (JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE - JX3_PERCENT_FLOAT_BASE) +
        temp * (roll & 1) * (criticalStrikePowerPercentInt + effectCriticalStrikePowerPercentInt) / JX3_PERCENT_INT_BASE);
}

// roll: 0 为命中, 1 为会心, 4为识破, 126为偏离
inline Value_t CalculateDamageSwitch(Value_t overcomeDamage, int roll, Value_t criticalStrikePower, PctInt_t effectCriticalStrikePowerPercentInt)
{
    PctInt_t criticalStrikePowerPercentInt =
        criticalStrikePower * JX3_PERCENT_INT_BASE /
        (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * 120 - JX3_LEVEL_CONST));

    switch (roll) {
        case HIT: return overcomeDamage;
        case DOUBLE:
            return overcomeDamage +
                   overcomeDamage * (JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE - JX3_PERCENT_FLOAT_BASE) +
                   overcomeDamage * (criticalStrikePowerPercentInt + effectCriticalStrikePowerPercentInt) / JX3_PERCENT_INT_BASE;
        case SIGHT: return overcomeDamage >> 2;
        default: return 0;
    }
}

static void BM_CalculateDamageBitwise(benchmark::State &state)
{
    std::mt19937                             gen(42); // 固定种子以确保可重现性
    std::uniform_int_distribution<>          roll_dis(0, 3);
    std::uniform_int_distribution<long long> damage_dis(1, 154873);
    std::uniform_int_distribution<int>       effect_dis(1, 512);
    std::uniform_int_distribution<int>       power_dis(1, 3000);

    std::vector<int> rolls = { 0, 1, 4, 126 };

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            CalculateDamageBitwise(damage_dis(gen), rolls[roll_dis(gen)], power_dis(gen), effect_dis(gen)));
    }
}

static void BM_CalculateDamageSwitch(benchmark::State &state)
{
    std::mt19937                             gen(42); // 固定种子以确保可重现性
    std::uniform_int_distribution<>          roll_dis(0, 3);
    std::uniform_int_distribution<long long> damage_dis(1, 154873);
    std::uniform_int_distribution<int>       effect_dis(1, 512);
    std::uniform_int_distribution<int>       power_dis(1, 3000);

    std::vector<int> rolls = { 0, 1, 4, 126 };

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            CalculateDamageSwitch(damage_dis(gen), rolls[roll_dis(gen)], power_dis(gen), effect_dis(gen)));
    }
}

BENCHMARK(BM_CalculateDamageBitwise);
BENCHMARK(BM_CalculateDamageSwitch);

BENCHMARK_MAIN();