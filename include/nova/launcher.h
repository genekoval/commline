#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace nova {
    class launcher {
        std::string name;
        std::string version;
    public:
        launcher(std::string_view name, std::string_view version);

        int start(const std::function<void()>& exec);
    };

    class option {
        std::string m_description;
        bool m_has_arg;
        std::string m_long_opt;
        std::string m_opt;
        std::string m_value;
    public:
        option(std::string_view opt, std::string_view description);
        option(
            std::string_view opt,
            bool has_arg,
            std::string_view description
        );
        option(
            std::string_view opt,
            std::string_view long_opt,
            bool has_arg,
            std::string_view description
        );

        std::string description();
        bool has_arg();
        std::string long_opt();
        std::string opt();
        std::string value();
    };

    class options {
        std::unordered_map<std::string,option> opt_map;
    public:
        options add(option opt);
        std::optional<std::string> value(std::string_view opt);
    };
}
