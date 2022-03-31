#include <commline/parser.h>

namespace commline {
    auto parser<std::string>::parse(std::string_view argument) -> std::string {
        return std::string(argument);
    }
}
