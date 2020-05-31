#include <commline/commline>

namespace commline {
    describable::describable(std::string_view description) :
        description(description)
    {}

    flag::flag(
        std::initializer_list<std::string> aliases,
        std::string_view description
    ) :
        parameter(aliases, description)
    {
        val = false;
    }

    auto flag::set() -> void { val = true; }

    value::value(
        std::initializer_list<std::string> aliases,
        std::string_view description,
        std::string_view name
    ) :
        parameter(aliases, description),
        name(name)
    {}

    auto value::set(std::string_view value) -> void {
        val = value;
    }
}
