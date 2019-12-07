#pragma once

#include <functional> // function
#include <initializer_list> // initializer_list
#include <optional> // optional
#include <stdexcept> // runtime_error
#include <string> // string
#include <string_view> // string_view
#include <unordered_map> // unordered_map
#include <vector> // vector

namespace commline {
    struct cli_error : std::runtime_error {
        cli_error(const std::string& message);
    };

    class program;
    program& current_program();

    class program {
    friend program& current_program();
    private:
        std::string m_name;
        std::string m_version;

        program(
            std::string_view name,
            std::string_view version
        );
    public:
        std::string_view name();
        std::string_view version();
    };

    class action;

    class option {
    friend class cli;
    private:
        std::string m_description;
        bool m_has_arg;
        std::string m_long_opt;
        std::string m_opt;
        bool m_selected;
        std::string m_value;

        void value(std::string_view val);
    public:
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
    friend class cli;
    private:
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

    class cli {
    friend class action;
    private:
        std::vector<std::string> m_args;
        std::string m_exec_path;
        option_table* option_ptr;

        cli(std::string_view exec_path, option_table* option_ptr);

        void parse(unsigned int argc, char** argv);
    public:
        const std::vector<std::string>& args() const;
        std::string_view exec_path() const;
        const option_table& options() const;
    };

    void main(const cli&);

    using exec = std::function<void(const cli&)>;

    class action {
        exec function;
        option_table opts;
    public:
        action(
            std::initializer_list<option> option_list,
            const exec function
        );

        void operator()(char* exec_path, unsigned int argc, char** argv);
    };

    class command {
        action m_action;
        std::string m_description;
    public:
        command(
            std::string_view description,
            std::initializer_list<option> option_list,
            const exec function
        );

        const action& get_action();
    };

    class origin {
        action m_action;
        std::unordered_map<std::string, command> commands;
    public:
        origin(
            std::initializer_list<option> options,
            std::initializer_list<std::pair<
                const std::string,
                command
            >> commands
        );

        int run(unsigned int argc, char** argv);
    };

    namespace util {
        void print_error(const std::exception& ex);
    }
}
