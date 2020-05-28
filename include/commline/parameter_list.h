#pragma once

#include <commline/error.h>
#include <commline/parameter_types.h>

#include <tuple>
#include <unordered_map>
#include <variant>

namespace commline {
    template <typename... Parameters>
    class parameter_list {
        using list_type = std::tuple<Parameters...>;
        using parameter_type = std::variant<Parameters...>;

        template<std::size_t N>
        using value_type =
            typename std::tuple_element<N, list_type>::value_type;

        static auto missing_value(const std::string& alias) -> void {
            throw cli_error("missing value for: " + alias);
        }

        list_type parameters;
        std::unordered_map<std::string, parameter_type*> lookup_table;

        auto add(const parameter_type& p) -> void {
            std::visit([this, &p](auto&& param) {
                for (const auto& alias : param.aliases) {
                    lookup_table[alias] = &p;
                }
            }, p);
        }

        template <std::size_t... I>
        auto generate_lookup_table(std::index_sequence<I...>) -> void {
            ((
                add(std::get<I>(parameters))
            ), ...);
        }

        auto get(const std::string& alias) -> parameter_type& {
            try {
                return *(lookup_table.at(alias));
            }
            catch (const std::out_of_range& ex) {
                throw cli_error("unknown option: " + alias);
            }
        }

        auto set_flag(const parameter_type& p) -> bool {
            if (auto param = std::get_if<flag>(&p)) {
                (*param).enable();
                return true;
            }
            else return false;
        }

        auto set_value(
            const parameter_type& p,
            const std::string& value
        ) -> void {
            std::visit([&value](auto&& param) {
                param.set(value);
            }, p);
        }

        template <typename InputIt>
        auto handle_long_parameter(
            const std::string& alias,
            InputIt first,
            InputIt last
        ) -> void {
            auto& p = get(alias);

            if (set_flag(p)) return;
            if (first == last) missing_value(alias);

            set_value(p, std::string(*first));
        }

        template <typename InputIt>
        auto handle_short_parameter(
            const std::string& sequence,
            InputIt first,
            InputIt last
        ) -> void {
            auto it = sequence.begin();
            auto end = sequence.end();

            while (it != end) {
                const auto alias = std::string(1, *(it++));
                auto& p = get(alias);

                if (set_flag(p)) continue;

                // The parameter requires a value.
                // The option value is the next arg after the
                // sequence of short options. If there are more
                // options in the sequence or there are no more
                // args after the sequence, the nthe value is
                // missing.
                if (it != end || first == last) missing_value(alias);

                set_value(p, std::string(*first));
            }
        }
    public:
        parameter_list(Parameters&&... parameters) :
            parameters(std::make_tuple(parameters...))
        {
            generate_lookup_table(std::index_sequence_for<Parameters...>{});
        }

        template <typename InputIt, typename Callable>
        auto parse(InputIt first, InputIt last, Callable handle_arg) -> void {
            while (first != last) {
                const auto current = std::string(*first++);

                // A '--' by itself signifies the end of options.
                // Everything that follows is an argument.
                if (current == "--") while (first != last) handle_arg(*first++);
                else if (current.starts_with("--")) handle_long_parameter(
                    current.substr(2),
                    first,
                    last
                );
                else if (current.starts_with('-')) handle_short_parameter(
                    current.substr(1),
                    first,
                    last
                );
                else handle_arg(current);
            }
        }
    };
}
