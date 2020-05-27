#include <commline/error.h>
#include <commline/parameter_types.h>

namespace commline {
    has_description::has_description(const std::string& description) :
        description(description)
    {}

    flag::flag(
        const std::string& description,
        std::initializer_list<std::string> aliases
    ) :
        parameter<bool>(aliases, description, false)
    {}

    auto flag::enable() -> void { val = true; }

    auto string::parse(input_type input) -> value_type {
        return input;
    }

    auto integer::parse(input_type input) -> value_type {
        try {
            return std::stoi(input);
        }
        catch (const std::invalid_argument& ex) {
            throw cli_error("failed to parse integer: " + input);
        }
        catch (const std::out_of_range& ex) {
            throw cli_error("integer argument out of range: " + input);
        }
    }

    auto number::parse(input_type input) -> value_type {
        try {
            return std::stof(input);
        }
        catch (const std::invalid_argument& ex) {
            throw cli_error("failed to parse number: " + input);
        }
        catch (const std::out_of_range& ex) {
            throw cli_error("number argument out of range: " + input);
        }
    }
}
