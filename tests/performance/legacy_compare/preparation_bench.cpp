#include <chrono>
#include <iostream>

#include "src/class/tai_xu_jian_yi/runtime_data.hpp"

int main()
{
    namespace tx = JX3DPS::runtime::tai_xu;
    tx::Config config;
    config.attributes.overcome_base = 25000;
    config.attributes.pve_percent   = 184;
    config.talents                  = tx::SupportedTalents;
    config.equipment                = tx::WEAPON_CW | tx::SET_ATTRIBUTE;
    constexpr int   REPETITIONS     = 1000;
    JX3DPS::value_t checksum        = 0;
    const auto      start           = std::chrono::steady_clock::now();
    for (int i = 0; i < REPETITIONS; ++i) {
        config.attributes.attack_base  = 10000 + i;
        const auto PREPARED            = tx::Prepare(config);
        checksum                      += PREPARED->max_damage;
    }
    const auto SECONDS = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    std::cout << "{\"preparations\":" << REPETITIONS << ",\"elapsed_s\":" << SECONDS
              << ",\"prepared_bytes\":" << sizeof(tx::Prepared) << ",\"checksum\":" << checksum << "}\n";
}
