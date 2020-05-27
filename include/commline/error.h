#pragma once

#include <stdexcept> // runtime_error

namespace commline {
    struct cli_error : std::runtime_error {
        using runtime_error::runtime_error;
    };
}
