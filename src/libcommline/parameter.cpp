#include <commline/commline>

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

    takes_argument::takes_argument(std::string_view argument_name) :
        argument_name(argument_name)
    {}

    single_argument::single_argument(
        std::initializer_list<std::string> aliases,
        std::string_view description,
        std::string_view argument_name
    ) :
        option_base(aliases, description),
        takes_argument(argument_name)
    {}

    auto single_argument::set(std::string_view argument) -> void {
        value = argument;
    }

    multiple_arguments::multiple_arguments(
        std::initializer_list<std::string> aliases,
        std::string_view description,
        std::string_view argument_name
    ) :
        option_base(aliases, description),
        takes_argument(argument_name)
    {}

    auto multiple_arguments::set(std::string_view argument) -> void {
        value.push_back(argument);
    }

    flag::flag(
        std::initializer_list<std::string> aliases,
        std::string_view description
    ) :
        base(aliases, description)
    {}

    auto flag::get() const -> type { return base.get(); }
}
