#pragma once

#include <functional> // std::function
#include <optional> // std::optional
#include <string> // std::string
#include <string_view> // std::string_view
#include <unordered_map> // std::unordered_map
#include <vector> // std::vector

namespace nova::cli {
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
        void value(std::string_view val);
    };

    class options {
        std::unordered_map<std::string,option*> opt_map;
        std::vector<option> opts;
    public:
        options& add(option&& opt);
        bool contains(const std::string& opt);
        option& get(const std::string& opt);
        std::optional<std::string> value(const std::string& opt);
    };

    class launcher {
        std::vector<std::string> args;
        std::string name;
        std::string version;
    public:
        launcher(std::string_view name, std::string_view version);

        int start(
            options& opts,
            unsigned int argc,
            const char** argv,
            const std::function<void(const std::vector<std::string>&)>& exec
        );
    };
}
