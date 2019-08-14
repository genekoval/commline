#pragma once

#include <functional> // function
#include <optional> // optional
#include <stdexcept> // runtime_error
#include <string> // string
#include <string_view> // string_view
#include <unordered_map> // unordered_map
#include <vector> // vector

namespace nova::cli {
    struct cli_error : std::runtime_error {
        cli_error(const std::string& message);
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
        void value(std::string_view val);
    };

    class options {
        std::unordered_map<std::string,option*> opt_map;
        std::vector<option> opts;
    public:
        options& add(option&& opt);
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
