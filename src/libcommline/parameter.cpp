#include <commline/commline>

#include <ext/string.h>

namespace commline {
    describable::describable(std::string_view description) :
        description(description)
    {}

    no_argument::no_argument(
        std::initializer_list<std::string> aliases,
        std::string_view description
    ) :
        option_base(aliases, description)
    {
        value = false;
    }

    auto no_argument::set() -> void { value = true; }

    single_argument::single_argument(
        std::initializer_list<std::string> aliases,
        std::string_view description,
        std::string_view argument_name
    ) :
        takes_argument(aliases, description, argument_name)
    {}

    auto single_argument::set(std::string_view argument) -> void {
        value = argument;
    }

    multiple_arguments::multiple_arguments(
        std::initializer_list<std::string> aliases,
        std::string_view description,
        std::string_view argument_name,
        std::string_view delimiter,
        bool discard_empty
    ) :
        takes_argument(aliases, description, argument_name),
        delimiter(delimiter),
        discard_empty(discard_empty)
    {}

    auto multiple_arguments::set(std::string_view argument) -> void {
        if (delimiter.empty()) {
            value.push_back(argument);
            return;
        }

        for (const auto& token : ext::string_range(argument, delimiter)) {
            if (discard_empty && token.empty()) continue;
            value.push_back(token);
        }
    }

    flag::flag(
        std::initializer_list<std::string> aliases,
        std::string_view description
    ) :
        base(aliases, description)
    {}

    auto flag::get() const -> type { return base.get(); }
}
