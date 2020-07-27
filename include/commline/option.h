#pragma once

#include <commline/argv.h>
#include <commline/parse.h>

#include <optional>
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
    };

    struct no_argument : public option_base<bool> {
        no_argument(
            std::initializer_list<std::string> aliases,
            std::string_view description
        );

        auto set() -> void;
    };

    struct takes_argument {
        const std::string argument_name;

        takes_argument(std::string_view argument_name);
    };

    struct single_argument :
        option_base<std::optional<std::string_view>>,
        takes_argument
    {
        single_argument(
            std::initializer_list<std::string> aliases,
            std::string_view description,
            std::string_view argument_name
        );

        auto set(std::string_view argument) -> void;
    };

    struct multiple_arguments : option_base<argv>, takes_argument {
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
            if (value) return parse<T>(*value);
            return default_value;
        }
    };
}
