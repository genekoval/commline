#include <commline/arguments.h>

#include <algorithm>
#include <ctype.h>

namespace {
    auto uppercase(std::string_view text) -> std::string {
        auto copy = std::string(text);
        std::transform(copy.begin(), copy.end(), copy.begin(), toupper);
        return copy;
    }
}

namespace commline {
    named_argument::named_argument(std::string_view name) : name(name) {}

    auto named_argument::print_help(std::ostream& out) const -> void {
        out << uppercase(name);
    }

    optional_argument::optional_argument(std::string_view name) :
        named_argument(name)
    {}

    auto optional_argument::print_help(std::ostream& out) const -> void {
        out << "[";
        named_argument::print_help(out);
        out << "]";
    }

    required_argument::required_argument(std::string_view name) :
        named_argument(name)
    {}

    argument_list::argument_list(std::string_view name) :
        named_argument(name)
    {}

    auto argument_list::print_help(std::ostream& out) const -> void {
        named_argument::print_help(out);
        out << "...";
    }
}
