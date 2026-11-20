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
        config.talents   = tx::DieRenTalent | tx::WuYi | tx::ShenMai | tx::XuanMen | tx::LieYun | tx::GuChang;
        config.equipment = tx::WeaponCW | tx::SetAttribute;
        config.attributes.overcome_base     = 20000;
        config.attributes.overcome_by_class = 5000;
        config.team_buffs                   = {
            { tx::team::JiLei,         0,  96,  1  },
            { tx::team::NongMei,       48, 96,  1  },
            { tx::team::HaoLingSanJun, 16, 0,   48 },
            { tx::team::PoFeng,        32, 160, 1  }
        };
        return config;
    }
    if (name == "mw_equipped") {
        mw::Config config;
        config.talents = mw::HaoZhong | mw::FeiFan | mw::ShiXiang | mw::ZhengLvHeMing | mw::LiuZhaoTalent | mw::KeMeng;
        config.equipment    = mw::EnchantShoes | mw::EnchantWrist | mw::EnchantBelt | mw::SetAttribute | mw::SetSkill |
                              mw::WeaponCW | mw::WeaponWater;
        config.team_effects = mw::YouRen | mw::JingMiao;
        return config;
    }
    throw std::invalid_argument("unknown scenario");
}

} // namespace JX3DPS::engine
