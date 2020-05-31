#pragma once

#include <string_view>
#include <vector>

namespace commline {
    using argv = std::vector<std::string_view>;

    auto collect(int argc, const char** argv) -> commline::argv;
}
