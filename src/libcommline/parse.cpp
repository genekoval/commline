#include <commline/option.h>

namespace commline {
    auto parser<std::string>::parse(std::string_view argument) -> std::string {
        return std::string(argument);
    }

    auto parser<int>::parse(std::string_view argument) -> int {
        const auto string = parser<std::string>::parse(argument);
        return std::stoi(string);
    }
}
