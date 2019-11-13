#pragma once

#include <functional> // function
#include <initializer_list> // initializer_list
#include <optional> // optional
#include <stdexcept> // runtime_error
#include <string> // string
#include <string_view> // string_view
#include <unordered_map> // unordered_map
#include <vector> // vector

namespace nova::cli {
    class launcher;
    class option_table;

    struct cli_error : std::runtime_error {
        cli_error(const std::string& message);
    };

    class option {
    friend
        void parse_cli(
            unsigned int argc,
            const char** argv,
            option_table& opts,
            std::vector<std::string>& args
        );

        std::string m_description;
        bool m_has_arg;
        std::string m_long_opt;
        std::string m_opt;
        bool m_selected;
        std::string m_value;

        void value(std::string_view val);
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

        std::string description() const;
        bool has_arg() const;
        std::string long_opt() const;
        std::string opt() const;
        bool selected() const;
        std::string value() const;
    };

    class option_table {
    friend
        void parse_cli(
            unsigned int argc,
            const char** argv,
            option_table& opts,
            std::vector<std::string>& args
        );

        std::vector<option> opts;
        std::unordered_map<std::string,option*> opt_map;

        void add_to_map(option* opt);
        option& get(const std::string& key);
    public:
        option_table(std::initializer_list<option> option_list);

        void add(const option& opt);
        bool selected(const std::string& key) const;
        std::optional<std::string> value(const std::string& key) const;
    };

    void parse_cli(
        unsigned int argc,
        const char** argv,
        option_table& opts,
        std::vector<std::string>& args
    );

    class launcher {
        std::vector<std::string> m_args;
        std::string name;
        option_table opts;
        std::string version;

        void print_version() const;
    public:
        launcher(
            std::string_view name,
            std::string_view version,
            std::initializer_list<option> option_list
        );

        const std::vector<std::string>& args() const;
        const option_table& options() const;
        void print_error(std::string_view text) const;
        int start(unsigned int argc, const char** argv);
    };

    void start(const launcher&);
}
