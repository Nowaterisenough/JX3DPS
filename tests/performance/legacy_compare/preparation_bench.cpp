#include "src/class/tai_xu_jian_yi/runtime_data.hpp"
#include <chrono>
#include <iostream>

int main() {
    namespace tx = JX3DPS::runtime::tai_xu;
    tx::Config config;
    config.attributes.overcome_base = 25000;
    config.attributes.pve_percent = 184;
    config.talents = tx::SupportedTalents;
    config.equipment = tx::WeaponCW | tx::SetAttribute;
    constexpr int repetitions = 1000;
    JX3DPS::value_t checksum = 0;
    const auto start = std::chrono::steady_clock::now();
    for (int i=0; i<repetitions; ++i) {
        config.attributes.attack_base = 10000+i;
        const auto prepared = tx::Prepare(config);
        checksum += prepared->max_damage;
    }
    const auto seconds = std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
    std::cout << "{\"preparations\":" << repetitions << ",\"elapsed_s\":" << seconds
        << ",\"prepared_bytes\":" << sizeof(tx::Prepared) << ",\"checksum\":" << checksum << "}\n";
}
