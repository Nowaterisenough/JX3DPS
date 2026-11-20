#pragma once

#include <variant>

#include "src/class/mo_wen/runtime_data.hpp"
#include "src/class/tai_xu_jian_yi/runtime_data.hpp"

namespace JX3DPS::engine {
using ScenarioConfig = std::variant<runtime::tai_xu::Config, runtime::mo_wen::Config>;

inline ScenarioConfig Scenario(std::string_view name)
{
    namespace tx = JX3DPS::runtime::tai_xu;
    namespace mw = JX3DPS::runtime::mo_wen;
    if (name == "tx") {
        return tx::Config{};
    }
    if (name == "mw") {
        return mw::Config{};
    }
    if (name == "tx_buffs") {
        tx::Config config;
        config.talents   = tx::DIE_REN_TALENT | tx::WU_YI | tx::SHEN_MAI | tx::XUAN_MEN | tx::LIE_YUN | tx::GU_CHANG;
        config.equipment = tx::WEAPON_CW | tx::SET_ATTRIBUTE;
        config.attributes.overcome_base     = 20000;
        config.attributes.overcome_by_class = 5000;
        config.team_buffs                   = {
            { tx::team::JI_LEI,           0,  96,  1  },
            { tx::team::NONG_MEI,         48, 96,  1  },
            { tx::team::HAO_LING_SAN_JUN, 16, 0,   48 },
            { tx::team::PO_FENG,          32, 160, 1  }
        };
        return config;
    }
    if (name == "mw_equipped") {
        mw::Config config;
        config.talents = mw::HAO_ZHONG | mw::FEI_FAN | mw::SHI_XIANG | mw::ZHENG_LV_HE_MING | mw::LIU_ZHAO_TALENT | mw::KE_MENG;
        config.equipment    = mw::ENCHANT_SHOES | mw::ENCHANT_WRIST | mw::ENCHANT_BELT | mw::SET_ATTRIBUTE |
                              mw::SET_SKILL | mw::WEAPON_CW | mw::WEAPON_WATER;
        config.team_effects = mw::YOU_REN | mw::JING_MIAO;
        return config;
    }
    throw std::invalid_argument("unknown scenario");
}

} // namespace JX3DPS::engine
