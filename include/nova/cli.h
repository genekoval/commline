#pragma once

#include <functional> // function
#include <optional> // optional
#include <stdexcept> // runtime_error
#include <string> // string
#include <string_view> // string_view
#include <unordered_map> // unordered_map
#include <vector> // vector

namespace nova::cli {
    class launcher;
    class options;

    using exec = std::function<void(const launcher&)>;

    struct cli_error : std::runtime_error {
        cli_error(const std::string& message);
    };

    class option {
    friend
        void parse_cli(
            unsigned int argc,
            const char** argv,
            options& opts,
            std::vector<std::string>& args
        );

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
        std::unordered_map<std::string,option*> opt_map;
        std::vector<option> opts;
    public:
        options& add(option&& opt);
        option& get(const std::string& opt);
        std::optional<std::string> value(const std::string& opt);
    };

    void parse_cli(
        unsigned int argc,
        const char** argv,
        options& opts,
        std::vector<std::string>& args
    );

    class launcher {
        std::vector<std::string> m_args;
        std::string name;
        options opts;
        std::string version;
    public:
        launcher(std::string_view name, std::string_view version, options& opts);

        const std::vector<std::string>& args() const;
        void print_error(std::string_view text);
        int start(unsigned int argc, const char** argv, const exec& program);
    };
}
