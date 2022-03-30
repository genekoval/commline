#pragma once

#include <commline/argv.h>
#include <commline/parser.h>
#include <commline/print.h>

#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace commline {
    struct describable {
        const std::string description;
    protected:
        describable(std::string_view description);
    };

    template <typename T>
    class option_base : public describable {
    protected:
        T value;

        option_base(
            std::initializer_list<std::string> aliases,
            std::string_view description
        ) :
            describable(description),
            aliases(aliases)
        {}
    public:
        using type = T;

        const std::vector<std::string> aliases;

        auto get() const -> T { return value; }

        auto print_help(
            std::ostream& out,
            std::optional<std::string_view>&& arg
        ) const -> void {
            constexpr auto spacing = 30;

            print::indent(out);

            auto it = aliases.begin();
            const auto end = aliases.end();
            auto space = spacing;

            while (it != end) {
                const auto& alias = *it;

                const auto is_short = alias.size() == 1;
                space -= (is_short ? 1 : 2) + alias.size();
                out << (is_short ? "-" : "--") << alias;

                if (++it != end) {
                    out << ", ";
                    space -= 2;
                }
            }

            if (arg) {
                const auto& value = arg.value();
                out << " " << value;
                space -= value.size() + 1;
            }

            if (space < 1) {
                // Not enough space.
                // Go to next line, and line up with the other descriptions.
                out << "\n";
                print::indent(out);
                space = spacing;
            }

            print::spaces(out, space);

            out << description << "\n";
        }
    };

    struct no_argument : option_base<bool> {
        no_argument(
            std::initializer_list<std::string> aliases,
            std::string_view description
        );

        auto print_help(std::ostream& out) const -> void {
            option_base<bool>::print_help(out, {});
        }

        auto set() -> void;
    };

    template <typename T>
    struct takes_argument : option_base<T> {
        const std::string argument_name;

        takes_argument(
            std::initializer_list<std::string> aliases,
            std::string_view description,
            std::string_view argument_name
        ) :
            option_base<T>(aliases, description),
            argument_name(argument_name)
        {}

        auto print_help(std::ostream& out) const -> void {
            option_base<T>::print_help(out, argument_name);
        }
    };

    struct single_argument : takes_argument<std::optional<std::string_view>> {
        single_argument(
            std::initializer_list<std::string> aliases,
            std::string_view description,
            std::string_view argument_name
        );

        auto set(std::string_view argument) -> void;
    };

    struct multiple_arguments : takes_argument<std::vector<std::string_view>> {
        multiple_arguments(
            std::initializer_list<std::string> aliases,
            std::string_view description,
            std::string_view argument_name
        );

        auto set(std::string_view argument) -> void;
    };

    struct flag {
        using type = no_argument::type;

        no_argument base;

        flag(
            std::initializer_list<std::string> aliases,
            std::string_view description
        );

        auto get() const -> type;
    };

    template <typename T>
    struct option {
        using type = T;

        single_argument base;
        const T default_value;

        option(
            std::initializer_list<std::string> aliases,
            std::string_view description,
            std::string_view argument_name
        ) :
            base(aliases, description, argument_name),
            default_value(T())
        {}

        option(
            std::initializer_list<std::string> aliases,
            std::string_view description,
            std::string_view argument_name,
            T&& default_value
        ) :
            base(aliases, description, argument_name),
            default_value(std::move(default_value))
        {}

        auto get() const -> T {
            auto value = base.get();
            if (value) return parser<T>::parse(*value);
            return default_value;
        }
    };

    template <typename T>
    struct list {
        using type = std::vector<T>;

        multiple_arguments base;

        list(
            std::initializer_list<std::string> aliases,
            std::string_view description,
            std::string_view argument_name
        ) :
            base(aliases, description, argument_name)
        {}

        auto get() const -> type {
            const auto value = base.get();
            auto result = type();

            for (const auto item : value) {
                result.push_back(parser<T>::parse(item));
            }

            return result;
        }
    };
}
