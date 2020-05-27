#pragma once

#include <commline/parameter.h>

namespace commline {
    class flag : public parameter<bool> {
    public:
        flag(
            const std::string& description,
            std::initializer_list<std::string> aliases
        );

        auto enable() -> void;
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
