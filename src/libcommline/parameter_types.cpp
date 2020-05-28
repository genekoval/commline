#include <commline/error.h>
#include <commline/parameter_types.h>

namespace commline {
    has_description::has_description(const std::string& description) :
        description(description)
    {}

    auto flag::enable() -> void { val = true; }

    auto flag::set(const std::string& value) -> void {}

    auto string::parse(input_type input) -> value_type {
        return input;
    }

    auto integer::parse(input_type input) -> value_type {
        try {
            return std::stoi(input);
        }
        catch (const std::invalid_argument& ex) {
            throw cli_error("invalid integer: " + input);
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
            throw cli_error("invalid number: " + input);
        }
        catch (const std::out_of_range& ex) {
            throw cli_error("number argument out of range: " + input);
        }
    }
}
