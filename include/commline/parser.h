#pragma once

#include <commline/error.h>

#include <concepts>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

namespace commline {
    template <typename T>
    struct parser {
        static auto parse(std::string_view argument) -> T { return argument; }
    };

    template <>
    struct parser<std::string> {
        static auto parse(std::string_view argument) -> std::string;
    };

    template <std::integral T>
    class parser<T> {
        static constexpr auto min = std::numeric_limits<T>::min();
        static constexpr auto max = std::numeric_limits<T>::max();
        static constexpr auto base = 0;

        [[noreturn]]
        static auto out_of_range(std::string_view argument) {
            throw cli_error(
                "argument '{}' is outside the range of {} and {}",
                argument,
                min,
                max
            );
        }
    public:
        static auto parse(std::string_view argument) -> T {
            const auto string = parser<std::string>::parse(argument);

            try {
                if constexpr (std::is_same_v<long long, T>) {
                    return std::stoll(string);
                }
                else if constexpr (std::is_same_v<unsigned long long, T>) {
                    return std::stoull(string);
                }
                else if constexpr (std::is_same_v<long, T>) {
                    return std::stol(string);
                }
                else if constexpr (std::is_same_v<unsigned long, T>) {
                    return std::stoul(string);
                }
                else if constexpr (std::is_signed_v<T>) {
                    const auto value = std::stoi(string);
                    if (value > max || value < min) out_of_range(argument);
                    return static_cast<T>(value);
                }
                else {
                    const auto value = std::stoul(string);
                    if (value > max || value < min) out_of_range(argument);
                    return static_cast<T>(value);
                }
            }
            catch (const std::invalid_argument& ex) {
                throw cli_error(
                    "could not convert argument '{}' to integer",
                    argument
                );
            }
            catch (const std::out_of_range& ex) {
                out_of_range(argument);
            }

            __builtin_unreachable();
        }
    };

    template <typename T>
    struct parser<std::optional<T>> {
        static auto parse(std::string_view argument) -> std::optional<T> {
            return parser<T>::parse(argument);
        }
    };
}
