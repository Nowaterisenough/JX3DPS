#pragma once
#include <map>

#include "book.hpp"

namespace JX3DPS::runtime::season50 {
struct SchoolInfo
{
    std::string_view code, faction, major, damage;
    int              attack_from_major, crit_from_major = 0, overcome_from_major = 0;
};

inline SchoolInfo Info(std::string_view code)
{
    static constexpr SchoolInfo info[] = {
        { "tx",   "纯阳", "身法", "Physics", 727, 41,  0   },
        { "mw",   "长歌", "根骨", "Lunar",   737, 31,  0   },
        { "axzy", "天策", "力道", "Physics", 563, 0,   205 },
        { "bxj",  "七秀", "根骨", "Lunar",   748, 20,  0   },
        { "lhj",  "蓬莱", "身法", "Physics", 707, 61,  0   },
        { "fsj",  "苍云", "身法", "Physics", 768, 0,   0   },
        { "baj",  "霸刀", "力道", "Physics", 553, 0,   215 },
        { "ztg",  "段氏", "元气", "Neutral", 625, 143, 0   },
        { "tlgd", "唐门", "元气", "Poison",  543, 225, 0   },
        { "txj",  "衍天", "元气", "Neutral", 645, 123, 0   },
        { "gfj",  "刀宗", "力道", "Physics", 604, 164, 0   },
        { "shxj", "万灵", "身法", "Physics", 717, 51,  0   },
        { "yly",  "无相", "根骨", "Lunar",   717, 51,  0   },
        { "wf",   "药宗", "根骨", "Poison",  707, 0,   61  },
        { "yjj",  "少林", "元气", "Solar",   666, 102, 0   },
        { "dj",   "五毒", "根骨", "Poison",  748, 0,   20  },
        { "fysj", "明教", "元气", "Solar",   635, 133, 0   },
        { "xcj",  "丐帮", "力道", "Physics", 573, 0,   195 },
        { "zxg",  "纯阳", "根骨", "Neutral", 727, 41,  0   },
        { "hjy",  "万花", "元气", "Neutral", 686, 0,   82  },
    };
    for (auto row : info) {
        if (row.code == code) {
            return row;
        }
    }
    throw std::invalid_argument("unknown specialization");
}

struct PermanentOption
{
    std::string                   category, name;
    int                           icon;
    std::map<std::string, double> stats;
};

std::span<const PermanentOption> PermanentOptions(const Book &book);
std::string_view                 PermanentFamily(const Book &book);
int ApplyPermanents(const Book &book, Attributes &attributes, int base_major, const std::map<std::string, std::string> &selected);
} // namespace JX3DPS::runtime::season50
