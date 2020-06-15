#pragma once

#include <commline/error.h>
#include <commline/option.h>

#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <variant>

namespace commline {
    template <typename ...Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };

    template <typename ...Ts> overloaded(Ts...) -> overloaded<Ts...>;

    template <typename ...Options>
    class option_list {
        using tuple_type = std::tuple<Options...>;
        using variant_type = std::variant<
            no_argument*,
            single_argument*,
            multiple_arguments*
        >;

        template <std::size_t N>
        using type =
            std::tuple_element<N, tuple_type>::type::type;

        static auto missing_value(std::string_view alias) -> void {
            throw cli_error("missing value for: " + std::string(alias));
        }

        tuple_type opts;
        std::unordered_map<std::string_view, variant_type> option_map;

        auto add(variant_type&& var) -> void {
            std::visit([&](auto* opt) {
                for (const auto& alias : opt->aliases) {
                    option_map[alias] = var;
                }
            }, var);
        }

        template <std::size_t ...I>
        auto generate_map(std::index_sequence<I...>) -> void {
            ((
                add(variant_type(
                    &(std::get<I>(opts).base)
                ))
            ), ...);
        }

        template <std::size_t ...I>
        auto get_values(
            std::index_sequence<I...>
        ) const -> std::tuple<typename Options::type...> {
            return std::make_tuple(std::get<I>(opts).get()...);
        }

        auto find(std::string_view alias) -> variant_type& {
            try {
                return option_map.at(alias);
            }
            catch (const std::out_of_range& ex) {
                throw cli_error("unknown option: " + std::string(alias));
            }
        }

        template <typename InputIt>
        auto handle_long_parameter(
            std::string_view alias,
            InputIt first,
            InputIt last
        ) -> void {
            std::visit(overloaded {
                [&](no_argument* opt) { opt->set(); },
                [&](auto* opt) {
                    if (first == last) missing_value(alias);
                    opt->set(*first);
                }
            }, find(alias));
        }

        template <typename InputIt>
        auto handle_short_parameter(
            std::string_view sequence,
            InputIt first,
            InputIt last
        ) -> void {
            auto it = sequence.begin();
            const auto end = sequence.end();

            while (it != end) {
                const auto alias = std::string_view(it++, 1);

                std::visit(overloaded {
                    [&](no_argument* opt) { opt->set(); },
                    [&](auto* opt) {
                        // The parameter requires a value.
                        // The option value is the next arg after the
                        // sequence of short options. If there are more
                        // options in the sequence or there are no more
                        // args after the sequence, the value is missing.
                        if (it != end || first == last) missing_value(alias);
                        opt->set(*first);
                    }
                }, find(alias));
            }
        }
    public:
        option_list(tuple_type&& opts) : opts(std::move(opts)) {
            generate_map(std::index_sequence_for<Options...>());
        }

        option_list(Options&&... opts) :
            option_list(std::make_tuple(std::move(opts)...))
        {}

        template <std::size_t N>
        auto get() const -> type<N> {
            return std::get<N>(opts).get();
        }

        auto extract() const -> std::tuple<typename Options::type...> {
            return get_values(std::index_sequence_for<Options...>());
        }

        template <typename InputIt, typename Callable>
        auto parse(InputIt first, InputIt last, Callable handle_arg) -> void {
            while (first != last) {
                constexpr auto l = std::string_view("--");
                constexpr auto s = std::string_view("-");

                const auto current = std::string_view(*first++);

                // A '--' by itself signifies the end of options.
                // Everything that follows is an argument.
                if (current == l) while (first != last) handle_arg(*first++);
                // A '-' by itself is treated as an argument.
                else if (current == s) handle_arg(current);
                else if (current.starts_with(l)) handle_long_parameter(
                    current.substr(l.size()),
                    first,
                    last
                );
                else if (current.starts_with(s)) handle_short_parameter(
                    current.substr(s.size()),
                    first,
                    last
                );
                else handle_arg(current);
            }
        }
    };

    template <typename ...Options>
    auto options(Options&&... opts) -> std::tuple<Options...> {
        return std::make_tuple(std::move(opts)...);
    }
}
