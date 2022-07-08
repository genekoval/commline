#pragma once

#include <span>

namespace commline {
    using argv = std::span<const char* const>;
    using iterator = argv::iterator;
}
