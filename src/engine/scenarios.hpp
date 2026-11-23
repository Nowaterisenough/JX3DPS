#pragma once

#include "src/class/common/book.hpp"

namespace JX3DPS::engine {
using ScenarioConfig = runtime::season50::Config;

inline ScenarioConfig Scenario(std::string_view name)
{
    if (const auto *book = runtime::season50::Find(name)) {
        runtime::season50::Config cfg;
        cfg.book = book;
        cfg.precombat = book->precombat;
        cfg.attributes = book->default_attributes;
        cfg.recipes.assign(book->default_recipes.begin(), book->default_recipes.end());
        cfg.equipment.assign(book->default_equipment.begin(), book->default_equipment.end());
        cfg.talents.assign(book->default_talents.begin(), book->default_talents.end());
        return cfg;
    }
    throw std::invalid_argument("unknown scenario");
}

} // namespace JX3DPS::engine
