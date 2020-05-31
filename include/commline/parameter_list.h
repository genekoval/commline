#pragma once

#include <commline/parameter.h>

#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <variant>

#include <iostream>

namespace commline {
    template <typename ...Parameters>
    class parameter_list {
        using list_type = std::tuple<Parameters...>;
        using parameter_type = std::variant<flag*, value*>;

        template <std::size_t N>
        using value_type =
            std::tuple_element<N, list_type>::type::value_type;

        static auto missing_value(std::string_view alias) -> void {
            throw cli_error("missing value for: " + std::string(alias));
        }

        list_type parameters;
        std::unordered_map<std::string_view, parameter_type> lookup_table;

        auto add(parameter_type&& param) -> void {
            std::visit([&](auto&& p) {
                for (const auto& alias : p->aliases) {
                    lookup_table[alias] = param;
                }
            }, param);
        }

        template <std::size_t... I>
        auto generate_lookup_table(std::index_sequence<I...>) -> void {
            ((
                add(parameter_type(&std::get<I>(parameters)))
            ), ...);
        }

        auto find(std::string_view alias) -> parameter_type& {
            try {
                return lookup_table.at(alias);
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
            std::visit([&, first, last](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, flag*>) arg->set();
                else if (first == last) missing_value(alias);
                else arg->set(*first);

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
                auto& param = find(alias);

                if (auto p = std::get_if<flag*>(&param)) {
                    (*p)->set();
                    continue;
                }

                // The parameter requires a value.
                // The option value is the next arg after the
                // sequence of short options. If there are more
                // options in the sequence or there are no more
                // args after the sequence, the value is
                // missing.
                if (it != end || first == last) missing_value(alias);

                std::get<value*>(param)->set(*first);
            }
        }
    public:
        template <typename ...P>
        parameter_list(P&&... params) : parameters(std::make_tuple(params...)) {
            generate_lookup_table(std::index_sequence_for<Parameters...>());
        }

        template <std::size_t N>
        auto get() const -> const value_type<N>& {
            return std::get<N>(parameters).get();
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
}
