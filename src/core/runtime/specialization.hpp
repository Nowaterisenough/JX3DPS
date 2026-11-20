#pragma once

#include <span>

#include "descriptors.hpp"

namespace JX3DPS::runtime {

struct NameBinding
{
    std::string_view name;
    Slot             slot;
};

struct SkillCatalog
{
    std::span<const SkillDescriptor>  skills;
    std::span<const BuffDescriptor>   buffs;
    std::span<const Slot>             castable;
    std::span<const std::string_view> skillAliases, buffAliases;
    std::span<const NameBinding>      extraBuffNames{};
};

inline CompileOptions CatalogMacroOptions(SkillCatalog catalog, bool debug = true)
{
    CompileOptions options;
    options.skill_count     = catalog.skills.size();
    options.buff_count      = catalog.buffs.size();
    options.keep_debug_info = debug;
    options.resolve_skill   = [catalog](std::string_view name) {
        for (const auto slot : catalog.castable) {
            if (name == catalog.skills[slot].name || name == catalog.skillAliases[slot]) {
                return slot;
            }
        }
        return INVALID_SLOT;
    };
    options.resolve_buff = [catalog](std::string_view name) {
        for (std::size_t slot = 0; slot < catalog.buffs.size() && slot < INVALID_SLOT; ++slot) {
            if (name == catalog.buffs[slot].name || name == catalog.buffAliases[slot]) {
                return static_cast<Slot>(slot);
            }
        }
        for (const auto &binding : catalog.extraBuffNames) {
            if (name == binding.name) {
                return binding.slot;
            }
        }
        return INVALID_SLOT;
    };
    return options;
}

// Each configuration names its module. Selection happens before entering the
// common simulator; the hot loop does not dispatch on a specialization ID.
template <class Config>
auto PrepareConfiguration(const Config &config)
{
    return Config::RuntimeModule::Prepare(config);
}

template <class Config>
auto LoadSpecialization(const Config &config)
{
    using Rules = typename Config::RuntimeModule::Rules;
    return Rules(PrepareConfiguration(config));
}

} // namespace JX3DPS::runtime
