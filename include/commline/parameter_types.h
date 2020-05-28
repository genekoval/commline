#pragma once

#include <commline/parameter.h>

namespace commline {
    class flag : public parameter<bool> {
    public:
        flag(
            std::initializer_list<std::string> aliases,
            const std::string& description
        ) :
            parameter<bool>(aliases, description, false)
        {}

        auto enable() -> void;

        // Ensures compatability with options that require a value.
        // TODO Combine the flag type with the other option types, so that
        // this method makes sense and is not just a noop.
        auto set(const std::string& value) -> void;
    };

    class string : public needs_value<string, std::string> {
    public:
        static auto parse(input_type input) -> value_type;
        using needs_value::needs_value;
    };

    class integer : public needs_value<integer, int> {
    public:
        static auto parse(input_type input) -> value_type;
        using needs_value::needs_value;
    };

    class number : public needs_value<number, float> {
    public:
        static auto parse(input_type input) -> value_type;
        using needs_value::needs_value;
    };
}
