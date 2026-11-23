#pragma once
#include <map>

#include "book.hpp"

namespace JX3DPS::runtime::season50 {
struct PermanentOption
{
    std::string                   category, name;
    int                           icon;
    std::map<std::string, double> stats;
};

std::span<const PermanentOption> PermanentOptions(const Book &book);
std::string_view                 PermanentFamily(const Book &book);
std::map<std::string, std::string> FullRaidPermanents(const Book &book);
int ApplyPermanents(const Book &book, Attributes &attributes, int base_major, const std::map<std::string, std::string> &selected, bool raid_full = false);
} // namespace JX3DPS::runtime::season50
