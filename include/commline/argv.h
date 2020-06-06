#pragma once

#include <string_view>
#include <vector>

namespace commline {
    using argv = std::vector<std::string_view>;

    using iterator = argv::iterator;

    auto collect(int argc, const char** argv) -> commline::argv;
}
