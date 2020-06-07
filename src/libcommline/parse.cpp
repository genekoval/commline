#include <commline/option.h>

namespace commline {
    template <>
    auto parse(std::string_view argument) -> int {
        return std::stoi(std::string(argument));
    }
}
