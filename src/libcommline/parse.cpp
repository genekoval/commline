#include <commline/option.h>

namespace commline {
    template <>
    auto parse(std::string_view argument) -> std::string {
        return std::string(argument);
    }

    template <>
    auto parse<int>(std::string_view argument) -> int {
        return std::stoi(std::string(argument));
    }
}
