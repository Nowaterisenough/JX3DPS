/**
 * Project: JX3DPS
 * File: TestRoll
 * Description: 
 * Created Date: 2023-11-11 19:58:12
 * Author: 难为水
 * -----
 * Last Modified: 2023-11-11 19:58:15
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

using Value_t = long long;

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
inline Value_t RollDamage1(Value_t overcomeDamage, int roll, Value_t criticalStrikePower, PctInt_t effectCriticalStrikePowerPercentInt)
{
    PctInt_t criticalStrikePowerPercentInt =
        criticalStrikePower * JX3_PERCENT_INT_BASE /
        (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * 120 - JX3_LEVEL_CONST));

    Value_t temp = overcomeDamage >> (roll / 2);

    return static_cast<Value_t>(
        temp + temp * (roll & 1) * (JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE - JX3_PERCENT_FLOAT_BASE) +
        temp * (roll & 1) * (criticalStrikePowerPercentInt + effectCriticalStrikePowerPercentInt) /
            JX3_PERCENT_INT_BASE);
}

// roll: 0 为命中, 1 为会心, 4为识破, 126为偏离
inline Value_t RollDamage2(Value_t overcomeDamage, int roll, Value_t criticalStrikePower, PctInt_t effectCriticalStrikePowerPercentInt)
{
    PctInt_t criticalStrikePowerPercentInt =
        criticalStrikePower * JX3_PERCENT_INT_BASE /
        (JX3_CRITICAL_STRIKE_POWER_PARAM * (JX3_LEVEL_PARAM * 120 - JX3_LEVEL_CONST));

    switch (roll) {
        case HIT: return overcomeDamage;
        case DOUBLE:
            return overcomeDamage +
                   overcomeDamage * (JX3_PLAYER_CRITICAL_STRIKE_POWER_PERCENT_BASE - JX3_PERCENT_FLOAT_BASE) +
                   overcomeDamage * (criticalStrikePowerPercentInt + effectCriticalStrikePowerPercentInt) /
                       JX3_PERCENT_INT_BASE;
        case SIGHT: return overcomeDamage >> 2;
        default: return 0;
    }
}

int main()
{
    const int NUM_TESTS = 100000000;

    std::random_device                       rd;
    std::mt19937                             gen(rd());
    std::uniform_int_distribution<>          roll_dis(0, 3);
    std::uniform_int_distribution<long long> damage_dis(1, 154873);
    std::uniform_int_distribution<int>       effect_dis(1, 512);
    std::uniform_int_distribution<int>       power_dis(1, 3000);

    std::vector<int> rolls = { 0, 1, 4, 126 };

    auto      start1 = std::chrono::high_resolution_clock::now();
    long long sum1   = 0;
    for (int i = 0; i < NUM_TESTS; ++i) {
        sum1 += RollDamage1(damage_dis(gen), rolls[roll_dis(gen)], power_dis(gen), effect_dis(gen));
    }
    auto                          end1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds1 = end1 - start1;
    std::cout << "Sum of RollDamage1 results: " << sum1 << std::endl;
    std::cout << "RollDamage1 elapsed time: " << elapsed_seconds1.count() << "s\n";

    auto      start2 = std::chrono::high_resolution_clock::now();
    long long sum2   = 0;
    for (int i = 0; i < NUM_TESTS; ++i) {
        sum2 += RollDamage2(damage_dis(gen), rolls[roll_dis(gen)], power_dis(gen), effect_dis(gen));
    }
    auto                          end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds2 = end2 - start2;
    std::cout << "Sum of RollDamage2 results: " << sum2 << std::endl;
    std::cout << "RollDamage2 elapsed time: " << elapsed_seconds2.count() << "s\n";

    std::cout << RollDamage1(154873, 0, 3000, 512) << std::endl;
    std::cout << RollDamage2(154873, 0, 3000, 512) << std::endl;

    std::cout << RollDamage1(154873, 1, 3000, 512) << std::endl;
    std::cout << RollDamage2(154873, 1, 3000, 512) << std::endl;

    std::cout << RollDamage1(154873, 4, 3000, 512) << std::endl;
    std::cout << RollDamage2(154873, 4, 3000, 512) << std::endl;

    std::cout << RollDamage1(154873, 126, 3000, 512) << std::endl;
    std::cout << RollDamage2(154873, 126, 3000, 512) << std::endl;

    return 0;
}