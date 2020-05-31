#pragma once

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
    struct parameter : public describable {
        using value_type = T;

        const std::vector<std::string> aliases;

        T val;

        parameter(
            std::initializer_list<std::string> aliases,
            std::string_view description
        ) :
            describable(description),
            aliases(aliases)
        {}

        auto get() const -> const T& { return val; }
    };

    struct flag : public parameter<bool> {
        using parameter::parameter;

        auto set() -> void;
    };

    struct value : public parameter<std::string> {
        const std::string name;

        value(
            std::initializer_list<std::string> aliases,
            std::string_view description,
            std::string_view name
        );

        auto set(std::string_view val) -> void;
    };
}
