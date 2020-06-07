#pragma once

#include <string_view>

namespace commline {
    template <typename T>
    auto parse(std::string_view argument) -> T { return argument; }

    template <>
    auto parse(std::string_view argument) -> int;
}
