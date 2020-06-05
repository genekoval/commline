#pragma once

#include <string_view>

namespace commline {
    struct app {
        const std::string_view name;
        const std::string_view version;
        const std::string_view description;
        const std::string_view argv0;
    };
}