#pragma once

#include <commline/error.h>
#include <commline/parser.h>

#include <array>
#include <optional>
#include <ostream>
#include <span>
#include <tuple>
#include <variant>
#include <vector>

namespace commline {
    struct named_argument {
        std::string name;

        named_argument(std::string_view name);

        auto print_help(std::ostream& out) const -> void;
    };

    struct required_argument : named_argument {
        std::string_view value;

        required_argument(std::string_view name);
    };

    struct optional_argument: named_argument {
        std::optional<std::string_view> value;

        optional_argument(std::string_view name);

        auto print_help(std::ostream& out) const -> void;
    };

    struct argument_list : named_argument {
        std::span<const std::string_view> values;

        argument_list(std::string_view name);

        auto print_help(std::ostream& out) const -> void;
    };

    template <typename T>
    struct required {
        using type = T;

        required_argument base;

        required(std::string_view name) : base(name) {}

        auto value() const -> type {
            return parser<type>::parse(base.value);
        }
    };

    template <typename T>
    struct optional {
        using type = std::optional<T>;

        optional_argument base;

        optional(std::string_view name) : base(name) {}

        auto value() const -> type {
            if (base.value) return parser<T>::parse(base.value.value());
            return {};
        }
    };

    template <typename T>
    struct variadic {
        using type = std::vector<T>;

        argument_list base;

        variadic(std::string_view name) : base(name) {}

        auto value() const -> type {
            auto result = type();

            for (const auto val : base.values) {
                result.push_back(parser<T>::parse(val));
            }

            return result;
        }
    };

    template <typename ...Arguments>
    class positional_arguments {
        using tuple_t = std::tuple<Arguments...>;
        using var_t = std::variant<
            required_argument*,
            argument_list*,
            optional_argument*
        >;
        using result_t = std::tuple<typename Arguments::type...>;

        template <std::size_t N>
        using type = typename std::tuple_element_t<N, tuple_t>::type;

        static constexpr auto size_v = std::tuple_size_v<tuple_t>;

        [[noreturn]]
        static auto missing_value(const std::string& name) -> void {
            throw cli_error("not enough arguments: missing value for: " + name);
        }

        tuple_t arguments;
        std::array<var_t, size_v> bases;

        template <std::size_t ...I>
        auto generate_bases(std::index_sequence<I...>) -> void {
            ((bases[I] = &(std::get<I>(arguments).base)), ...);
        }

        template <std::size_t ...I>
        auto get_values(std::index_sequence<I...>) const -> result_t {
            return std::make_tuple(std::get<I>(arguments).value()...);
        }

        template <typename ArgsIt, typename BasesIt>
        auto read_reverse(
            const ArgsIt& args_begin,
            ArgsIt& args_end,
            const BasesIt& bases_begin,
            BasesIt& bases_end
        ) -> void {
            while (bases_begin != bases_end) {
                auto& current = *--bases_end;

                if (auto** arg = std::get_if<required_argument*>(&current)) {
                    auto& base = **arg;

                    if (args_begin == args_end) missing_value(base.name);

                    base.value = *--args_end;
                }
                else if (auto** arg = std::get_if<optional_argument*>(&current)) {
                    auto& base = **arg;
                    if (args_begin != args_end) base.value = *--args_end;
                }
                else return; // argument_list reached
            }
        }
    public:
        positional_arguments(tuple_t&& arguments) :
            arguments(std::move(arguments))
        {
            generate_bases(std::index_sequence_for<Arguments...>());
        }

        auto parse(std::span<const std::string_view> args) -> result_t {
            auto args_begin = args.begin();
            auto args_end = args.end();

            auto bases_begin = bases.begin();
            auto bases_end = bases.end();

            while (bases_begin != bases_end) {
                auto& current = *bases_begin++;

                if (auto** arg = std::get_if<required_argument*>(&current)) {
                    auto& base = **arg;

                    if (args_begin == args_end) missing_value(base.name);

                    base.value = *args_begin++;
                }
                else if (auto** arg = std::get_if<optional_argument*>(&current)) {
                    auto& base = **arg;

                    if (args_begin != args_end) base.value = *args_begin++;
                }
                else if (auto** arg = std::get_if<argument_list*>(&current)) {
                    read_reverse(args_begin, args_end, bases_begin, bases_end);

                    auto& base = **arg;
                    base.values = std::span(
                        args_begin,
                        args_end
                    );

                    std::advance(args_begin, base.values.size());
                }
            }

            if (args_begin != args_end) {
                throw cli_error("too many arguments");
            }

            return get_values(std::index_sequence_for<Arguments...>());
        }

        auto print_help(std::ostream& out) const -> void {
            if (bases.empty()) return;

            for (const auto& base : bases) {
                out << " ";

                std::visit([&out](auto* arg) {
                    arg->print_help(out);
                }, base);
            }
        }

        constexpr auto size() const -> std::size_t {
            return size_v;
        }
    };

    template <typename ...Arguments>
    auto arguments(Arguments&&... args) -> std::tuple<Arguments...> {
        return std::make_tuple(std::move(args)...);
    }
}
