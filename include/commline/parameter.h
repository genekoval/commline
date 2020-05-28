#pragma once

#include <string>
#include <vector>

namespace commline {
    class has_description {
    protected:
        has_description(const std::string& description);
    public:
        const std::string description;
    };

    template <typename Value>
    class parameter : public has_description {
    public:
        using value_type = Value;
        const std::vector<std::string> aliases;
    protected:
        Value val;

        template <typename Container>
        parameter(
            Container aliases,
            const std::string& description,
            Value default_value
        ) :
            has_description(description),
            aliases(aliases),
            val(default_value)
        {}
    public:
        auto value() const -> const Value& {
            return val;
        }
    };

    template <typename Derived, typename Value>
    class needs_value : public parameter<Value> {
    protected:
        using input_type = const std::string&;
    public:
        const std::string value_name;

        template <typename Container>
        needs_value(
            Container aliases,
            const std::string& description,
            const std::string& value_name,
            Value default_value
        ) :
            parameter<Value>(aliases, description, default_value),
            value_name(value_name)
        {}

        auto set(input_type input) -> void {
            this->val = Derived::parse(input);
        }
    };
}
