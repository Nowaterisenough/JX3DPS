#pragma once

#include "src/core/runtime.hpp"
#include <charconv>

namespace JX3DPS::runtime::tai_xu::team {
enum Kind : Slot { HanRuLei, PoFeng, JingFeng, JieHuo, ChaoSheng, ShengYuMingXin,
    ZhenFen, HanXiaoQianJun, ShuKuang, HanChangLinLi, JiLei, NongMei, SheShenHongFa, HaoLingSanJun, Count };

struct Modifiers {
    int attack_percent = 0, overcome_base = 0, overcome_percent = 0;
    int strain = 0, crit_basis_points = 0, ignore_shield = 0, shield = 0, vulnerable = 0;
    bool operator==(const Modifiers &) const = default;
    void Add(const Modifiers &m, int n) {
        attack_percent += m.attack_percent*n; overcome_base += m.overcome_base*n;
        overcome_percent += m.overcome_percent*n; strain += m.strain*n;
        crit_basis_points += m.crit_basis_points*n; ignore_shield += m.ignore_shield*n;
        shield += m.shield*n; vulnerable += m.vulnerable*n;
    }
};
struct Definition {
    std::string_view alias, name;
    tick_t duration;
    int max_stacks;
    bool target;
    Modifiers modifiers;
};
// Values and replacement/refresh semantics from ba960f7 src/Buff3rd.cpp.
inline constexpr std::array<Definition, Count> Definitions{{
    {"han_ru_lei", "撼如雷", 28800, 1, false, {.attack_percent=51}},
    {"po_feng", "破风", 224, 1, true, {.shield=-1150}},
    {"jing_feng", "劲风", 224, 1, true, {.shield=-1397}},
    {"jie_huo", "戒火", 240, 1, true, {.vulnerable=20}},
    {"chao_sheng", "朝圣", 128, 24, false, {.strain=400}},
    {"sheng_yu_ming_xin", "圣浴明心", 128, 24, false, {.strain=600}},
    {"zhen_fen", "振奋", 160, 125, false, {.overcome_base=60}},
    {"han_xiao_qian_jun", "寒啸千军", 240, 1, false, {.overcome_percent=256}},
    {"shu_kuang", "疏狂", 192, 1, false, {.attack_percent=307}},
    {"han_chang_lin_li", "酣畅淋漓", 80, 1, false, {.crit_basis_points=1000}},
    {"ji_lei", "激雷", 96, 1, false, {.attack_percent=205, .overcome_percent=205}},
    {"nong_mei", "弄梅", 96, 1, false, {.overcome_base=700, .ignore_shield=205}},
    {"she_shen_hong_fa", "舍身弘法", 320, 36, false, {.strain=400}},
    {"hao_ling_san_jun", "号令三军", 960, 48, false, {.strain=440}}
}};
struct Application {
    Kind kind = HanRuLei;
    tick_t frame = 0, duration = 0; // Zero duration selects the definition's duration.
    int stacks = 1; // Replace stacks and refresh; zero removes the entire aura.
    bool operator==(const Application &) const = default;
};
inline void Validate(const Application &a) {
    if (a.kind >= Count || a.frame < 0 || a.frame > 86400*16 || a.duration < 0 || a.duration > 86400*16 ||
        a.stacks < 0 || a.stacks > Definitions[a.kind].max_stacks)
        throw std::invalid_argument("invalid team buff kind, time or stack count");
    // The pinned engine subtracts inconsistent amounts for odd first-drum stacks.
    if (a.kind == HaoLingSanJun && ((a.stacks % 2) || (a.duration != 0 && a.duration != 960)))
        throw std::invalid_argument("HaoLingSanJun requires even initial stacks and its fixed 60-second duration");
}
// Shared CLI/desktop format, in exact simulation frames (16 frames/second).
inline std::vector<Application> Parse(std::string_view text) {
    std::vector<Application> result;
    while (!text.empty()) {
        const auto end = text.find_first_of(";\n");
        auto row = text.substr(0, end);
        if (!row.empty() && row.back() == '\r') row.remove_suffix(1);
        if (!row.empty()) {
            const auto colon = row.find(':');
            const auto name = row.substr(0, colon);
            Kind kind = Count;
            for (Slot i=0; i<Count; ++i)
                if (name == Definitions[i].alias || name == Definitions[i].name) kind = static_cast<Kind>(i);
            if (kind == Count || colon == row.npos) throw std::invalid_argument("team buff syntax: name:frame:duration:stacks");
            row.remove_prefix(colon+1);
            Application app{kind};
            for (int *field : {&app.frame, &app.duration, &app.stacks}) {
                const auto separator = row.find(':');
                const auto value = row.substr(0, separator);
                const auto [last, error] = std::from_chars(value.data(), value.data()+value.size(), *field);
                if (error != std::errc{} || last != value.data()+value.size() ||
                    (field != &app.stacks && separator == row.npos) || (field == &app.stacks && separator != row.npos))
                    throw std::invalid_argument("team buff syntax: name:frame:duration:stacks");
                if (separator != row.npos) row.remove_prefix(separator+1);
            }
            Validate(app);
            result.push_back(app);
        }
        if (end == text.npos) break;
        text.remove_prefix(end+1);
    }
    return result;
}
}
