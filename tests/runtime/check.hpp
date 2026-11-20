#pragma once

#include <cstdlib>
#include <iostream>
#include <source_location>
#include <string_view>

namespace JX3DPS::Tests {
[[noreturn]] inline void FailCheck(std::string_view expression, std::source_location location = std::source_location::current())
{
    std::cerr << location.file_name() << ':' << location.line() << ": " << expression << '\n';
    std::exit(EXIT_FAILURE);
}
} // namespace JX3DPS::Tests

#define CHECK(expression) ((expression) ? static_cast<void>(0) : ::JX3DPS::Tests::FailCheck(#expression))
