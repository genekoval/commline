#pragma once

#include <fmt/core.h>
#include <stdexcept>

namespace commline {
    struct cli_error : std::runtime_error {
        template <typename... Args>
        cli_error(std::string_view format_string, Args&&... args) :
            std::runtime_error(fmt::format(
                fmt::runtime(format_string),
                std::forward<Args>(args)...
            )) {}
    };
}
