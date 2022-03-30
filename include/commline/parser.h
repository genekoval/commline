#pragma once

#include <optional>
#include <string_view>

namespace commline {
    template <typename T>
    struct parser {
        static auto parse(std::string_view argument) -> T {
            return argument;
        }
    };

    template <>
    struct parser<std::string> {
        static auto parse(std::string_view argument) -> std::string;
    };

    template <>
    struct parser<int> {
        static auto parse(std::string_view argument) -> int;
    };

    template <typename T>
    struct parser<std::optional<T>> {
        static auto parse(std::string_view argument) -> std::optional<T> {
            return parser<T>::parse(argument);
        }
    };
}
