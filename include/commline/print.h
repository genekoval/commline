#pragma once

#include <ostream>
#include <string_view>

namespace commline::print {
    auto header(std::ostream& out, std::string_view text) -> void;

    auto indent(std::ostream& out) -> void;

    auto spaces(std::ostream& out, int amount) -> void;
};
